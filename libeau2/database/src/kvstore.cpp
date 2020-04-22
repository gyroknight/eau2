/**
 * @file kvstore.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "kvstore.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "get.hpp"
#include "key.hpp"
#include "kill.hpp"
#include "kvnet.hpp"
#include "message.hpp"
#include "payload.hpp"
#include "put.hpp"
#include "reply.hpp"
#include "serial.hpp"
#include "waitandget.hpp"

namespace {
constexpr size_t WAIT_GET_TIMEOUT_S =
    60;  // How long to wait for network responses before failing in seconds
}  // namespace

// Constructs a KVStore using the communication layer provided
KVStore::KVStore(KVNet& kvNet, const char* address, const char* port)
    : _kvNet(kvNet) {
    _listener = std::thread(&KVStore::_listen, this, address, port);
}

// Shuts down listener and destroys the KVStore
KVStore::~KVStore() {
    auto msg = std::make_shared<Kill>(_idx, _idx);
    _kvNet.send(msg);
    _listener.join();
}

// Adds a DataFrame to the store at the key provided, assuming it does not
// already exist
void KVStore::insert(const Key& key, DFPtr value) {
    std::shared_lock<std::shared_mutex> readLock(_storeMutex);
    // Currently ignores
    if (_store.count(key)) {
        std::cerr << "Key " << key.name() << " already exists on node " << _idx
                  << ".\n";
        return;
    }
    readLock.unlock();

    const std::lock_guard<std::shared_mutex> writeLock(_storeMutex);
    _store[key] = value;
    _cv.notify_all();
}

// Waits for data of a given key to become available, if it doesn't within the
// timeout period, the data cannot be found
DFPtr KVStore::waitAndGet(const Key& key) {
    std::shared_lock<std::shared_mutex> lock(_storeMutex);

    if (_store.count(key)) {
        return _store[key];
    }

    fetch(key, true);

    if (_cv.wait_for(lock, std::chrono::seconds(WAIT_GET_TIMEOUT_S),
                     [this, &key] { return _store.count(key) == 1; })) {
        return _store[key];
    } else {
        std::cerr << "Request timed out, unable to find dataframe.\n";
        return nullptr;
    }
}

// Attempts to fetch a remote DataFrame, can wait for data to become available
// if not present
void KVStore::fetch(const Key& key, bool wait) {
    _readyGuard();
    if (key.home() != _idx) {
        const std::lock_guard<std::mutex> lock(_pendingMutex);
        if (wait) {
            auto msg = std::make_shared<WaitAndGet>(_idx, key.home(), key);
            _pending.insert({msg->id(), msg});
            _kvNet.send(msg);
            std::cout << "Requested " << key.name() << " from node "
                      << key.home() << " with ID " << msg->id() << std::endl;
        } else {
            auto msg = std::make_shared<Get>(_idx, key.home(), key);
            _pending.insert({msg->id(), msg});
            _kvNet.send(msg);
        }
    }
}

// Places a DataFrame at the given Key, either locally if Key matches current
// index, or sends it remotely to its home
void KVStore::push(const Key& key, DFPtr value) {
    _readyGuard();
    if (key.home() == _idx) {
        insert(key, value);
    } else {
        _kvNet.send(std::make_shared<Put>(_idx, key, value));
    }
}

// Polling logic that processes received messages from the network
void KVStore::_listen(const char* address, const char* port) {
    _idx = _kvNet.registerNode(address, port);
    bool listening = true;
    while (listening) {
        std::shared_ptr<Message> msg = _kvNet.receive();

        // Temp
        std::shared_ptr<Put> putMsg;

        if (msg) {
            std::cout << "Node " << _idx << " received message ID " << msg->id()
                      << " with type "
                      << std::to_string(Message::msgKindToValue(msg->kind()))
                      << std::endl;

            if (msg->sender() == 0 && msg->kind() != MsgKind::Kill) {
                std::cerr << "Warning: Non-Kill Message received from index 0, "
                             "reserved for registrar\n";
            }

            switch (msg->kind()) {
                case MsgKind::Put:
                    // Temp code, for now we accept values without
                    // acknowledgement, should eventually change in the future
                    putMsg = std::dynamic_pointer_cast<Put>(msg);
                    insert(putMsg->key(), putMsg->value());
                    break;
                case MsgKind::Reply:
                    _postReply(std::dynamic_pointer_cast<Reply>(msg));
                    break;
                case MsgKind::Get:
                    _sendGetReply(std::dynamic_pointer_cast<Get>(msg));
                    break;
                case MsgKind::WaitAndGet:
                    _startWaitAndGetReply(
                        std::dynamic_pointer_cast<WaitAndGet>(msg));
                    break;
                case MsgKind::Kill:
                    listening = false;
                    _kvNet.shutdown();
                    break;
                default:
                    std::cerr
                        << "Unsupported message. "
                        << std::to_string(Message::msgKindToValue(msg->kind()))
                        << '\n';
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void KVStore::_postReply(std::shared_ptr<Reply> reply) {
    if (!reply) return;
    std::lock_guard<std::mutex> lock(_pendingMutex);
    try {
        std::shared_ptr<Message> msg = _pending.at(reply->id());

        std::shared_ptr<Get> getMsg;

        switch (msg->kind()) {
            case MsgKind::Get:
            case MsgKind::WaitAndGet:
                getMsg = std::dynamic_pointer_cast<Get>(msg);
                if (reply->payload()->type() == Serial::Type::DataFrame) {
                    insert(getMsg->key(), reply->payload()->asDataFrame());
                } else {
                    // Not sure what we'll use Get for that aren't dataframes
                }
                break;
            default:
                std::cerr << "Unsolicited reply, discarding\n";
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "No pending message with ID " << reply->id() << '\n';
    }
}

void KVStore::_sendGetReply(std::shared_ptr<Get> msg) {
    const std::shared_lock<std::shared_mutex> lock(_storeMutex);
    if (_store.count(msg->key())) {
        DFPtr df = _store[msg->key()];
        if (msg->colIdx() == UINT64_MAX && msg->rowIdx() == UINT64_MAX) {
            auto reply =
                std::make_shared<Reply>(_idx, msg->sender(), msg->id());
            reply->setPayload(df);
            _kvNet.send(reply);
            std::cout << "Node " << _idx << " sent reply with ID " << msg->id()
                      << std::endl;
        }
    }
}

void KVStore::_startWaitAndGetReply(std::shared_ptr<WaitAndGet> msg) {
    auto asyncResponse = [this, msg]() {
        DFPtr df = waitAndGet(msg->key());
        _sendGetReply(msg);
    };

    std::thread thread(asyncResponse);
    thread.detach();
}

// We wait for the network to be online and check that the index was properly
// set. 0 is a reserved value for the registrar, so the new index must not be 0
void KVStore::_readyGuard() {
    if (!_kvNet.ready()) throw std::runtime_error("Network is not ready");
    while (_idx == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}