#include "kvstore.hpp"

#include <iostream>

#include "get.hpp"
#include "key.hpp"
#include "kill.hpp"
#include "put.hpp"
#include "reply.hpp"
#include "waitandget.hpp"

namespace {
constexpr size_t WAIT_GET_TIMEOUT_S = 10;
}  // namespace

KVStore::KVStore(KVNet& kvNet) : _kvNet(kvNet) {
    _listener = std::thread(&KVStore::_listen, this);
}

KVStore::~KVStore() {
    auto msg = std::make_shared<Kill>(_idx, _idx);
    _kvNet.send(msg);
    _listener.join();
}

void KVStore::insert(const Key& key, std::shared_ptr<DataFrame> value) {
    if (_validKeys.count(key)) {
        std::cerr << "Key " << key.name() << " already exists on node " << _idx
                  << ".\n";
        return;
    }

    _store[key] = value;

    const std::lock_guard<std::mutex> lockKeys(_keyMutex);
    _validKeys.insert(key);
    _cv.notify_all();
}

std::shared_ptr<DataFrame> KVStore::waitAndGet(const Key& key) {
    std::unique_lock<std::mutex> lockKeys(_keyMutex);

    if (_validKeys.count(key)) {
        return _store[key];
    }

    fetch(key, true);

    if (_cv.wait_for(lockKeys, std::chrono::seconds(WAIT_GET_TIMEOUT_S),
                     [this, &key] { return _validKeys.count(key) == 1; })) {
        std::shared_ptr<DataFrame> ret = _store[key];
        lockKeys.release();
        return ret;
    } else {
        std::cout << "Request timed out, unable to find dataframe."
                  << std::endl;
        return nullptr;
    }
}
void KVStore::fetch(const Key& key, bool wait) {
    if (key.home() != _idx) {
        if (wait)
            _kvNet.send(std::make_shared<WaitAndGet>(_idx, key.home(), key));
        else
            _kvNet.send(std::make_shared<Get>(_idx, key.home(), key));
    }
}

void KVStore::push(const Key& key, std::shared_ptr<DataFrame> value) {
    _kvNet.send(std::make_shared<Put>(_idx, key, value));
}

void KVStore::_listen() {
    _idx = _kvNet.registerNode();
    bool listening = true;
    while (listening) {
        std::shared_ptr<Message> msg = _kvNet.receive(_idx);

        if (msg) {
            switch (msg->kind()) {
                case MsgKind::Reply:
                    _postReply(std::dynamic_pointer_cast<Reply>(msg));
                    break;
                case MsgKind::Get:
                    _sendGetReply(std::dynamic_pointer_cast<Get>(msg));
                    break;
                case MsgKind::WaitAndGet:
                    break;
                case MsgKind::Kill:
                    listening = false;
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
    _pendingMutex.lock();
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

    _pendingMutex.unlock();
}

void KVStore::_sendGetReply(std::shared_ptr<Get> msg) {
    const std::lock_guard<std::mutex> lock(_keyMutex);
    if (_validKeys.count(msg->key())) {
        std::shared_ptr<DataFrame> df = _store[msg->key()];
        if (msg->colIdx() == UINT64_MAX && msg->rowIdx() == UINT64_MAX) {
            auto reply =
                std::make_shared<Reply>(_idx, msg->sender(), msg->id());
            reply->setPayload(df);
            _kvNet.send(reply);
        }
    }
}