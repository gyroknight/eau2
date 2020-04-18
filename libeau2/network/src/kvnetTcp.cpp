/**
 * @file kvnetTcp.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "kvnetTcp.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <stack>

#include "directory.hpp"
#include "register.hpp"
#include "tcpUtils.hpp"

namespace {
constexpr const char* REGISTRAR = "127.0.0.1";
constexpr const char* PORT = "4500";
}  // namespace

KVNetTCP::KVNetTCP() {}
KVNetTCP::~KVNetTCP() {
    // Net is down
    _netUp = false;

    // Wait for sender and receiver to stop
    if (_sender.joinable()) _sender.join();
    if (_receiver.joinable()) _receiver.join();
}

// Registers node with the registrar and awaits directory before starting sender
// and receiver
size_t KVNetTCP::registerNode(const char* port) {
    // Can't register more than once
    if (_idx) throw std::runtime_error("Node is already registered");

    ssize_t ret;
    struct addrinfo* registrarInfo = TCP::generateAddrinfo(PORT, REGISTRAR);

    // registrar is always node 0
    _sockfdsLock.lock();
    _sockfds[0] = TCP::createSocket(registrarInfo);
    _sockfdsLock.unlock();

    // we want a predetermined connection port
    TCP::bindToPort(_sockfds[0], port);

    if (connect(_sockfds[0], registrarInfo->ai_addr,
                registrarInfo->ai_addrlen) == -1) {
        close(_sockfds[0]);
        freeaddrinfo(registrarInfo);
        throw std::runtime_error("Unable to connect to registrar");
    }

    freeaddrinfo(registrarInfo);

    // send registration
    struct addrinfo* connInfo = TCP::generateAddrinfo(port);
    struct sockaddr_in* address =
        reinterpret_cast<struct sockaddr_in*>(connInfo->ai_addr);
    Register regMsg(address->sin_addr.s_addr, address->sin_port);
    freeaddrinfo(connInfo);

    ret = TCP::sendPacket(_sockfds[0], regMsg.serialize());

    // wait for directory
    std::unique_ptr<Message> msg = _readMsg(0);
    std::unique_ptr<Directory> dirMsg(dynamic_cast<Directory*>(msg.release()));
    if (!dirMsg) {
        throw std::runtime_error("Invalid Directory from registrar");
    }

    // Set node index and populate directory
    _idx = dirMsg->idx();
    _dir.insert(std::end(_dir), std::begin(dirMsg->dir()),
                std::end(dirMsg->dir()));

    // start sender and receiver
}

// Might want to play around with serialization here instead of in event handler
// Adds a Message to be sent
void KVNetTCP::send(std::shared_ptr<Message> msg) {
    const std::lock_guard<std::mutex> lock(_sendLock);
    _sending.push(msg);
}

// Removes a Message that has been received for processing
std::unique_ptr<Message> KVNetTCP::receive() {
    const std::lock_guard<std::mutex> lock(_receiveLock);
    if (!_receiving.empty()) {
        std::unique_ptr<Message> msg = std::move(_receiving.front());
        _receiving.pop();
        return msg;
    }

    return nullptr;
}

// Reads in Message bytestream from node and deserializes
std::unique_ptr<Message> KVNetTCP::_readMsg(uint64_t idx) {
    std::shared_lock<std::shared_mutex> lock(_sockfdsLock);
    if (!_sockfds.count(idx)) return nullptr;
    auto msg = std::make_unique<std::vector<uint8_t>>();

    // Read Command Header
    if (TCP::recvData(_sockfds[idx], *msg, Serial::CMD_HDR_SIZE)) {
        std::cerr << "Failed to get command header\n";
        return nullptr;
    }

    // Read Message-specific data
    switch (Message::valueToMsgKind((*msg)[0])) {
        case MsgKind::Ack:
        case MsgKind::Nack:
        case MsgKind::Kill:
            break;
        case MsgKind::Put:
            if (_readPut(idx, *msg)) return nullptr;
            break;
        case MsgKind::Reply:
            if (_readReply(idx, *msg)) return nullptr;
            break;
        case MsgKind::Get:
            if (_readGet(idx, *msg)) return nullptr;
            break;
        case MsgKind::WaitAndGet:
            if (_readWaitAndGet(idx, *msg)) return nullptr;
            break;
        case MsgKind::Directory:
            if (_readDirectory(idx, *msg)) return nullptr;
            break;
        default:
            return nullptr;
    }

    return Message::deserialize(std::move(msg));
}

// Reads bytestream for a Payload
ssize_t KVNetTCP::_readPayload(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;
    std::stack<uint64_t> pending;  // Keeps track of pending Payloads for
                                   // multi-Payload transmissions

    // Get first Payload
    if ((ret = TCP::recvData(_sockfds[idx], msg, 17))) {
        std::cerr << "Failed to get Payload header\n";
        return ret;
    }

    uint64_t payloadsLeft = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - 2 * sizeof(uint64_t)));
    uint64_t size = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - sizeof(uint64_t)));

    if ((ret = TCP::recvData(_sockfds[idx], msg, size))) {
        std::cerr << "Failed to get Payload data\n";
        return ret;
    }

    // Get next Payloads if applicable
    while (payloadsLeft) {
        if ((ret = TCP::recvData(_sockfds[idx], msg, 17))) {
            std::cerr << "Failed to get Payload header\n";
            return ret;
        }

        uint64_t newPayloadsLeft = *reinterpret_cast<uint64_t*>(
            msg.data() + (msg.size() - 2 * sizeof(uint64_t)));
        size = *reinterpret_cast<uint64_t*>(msg.data() +
                                            (msg.size() - sizeof(uint64_t)));

        if ((ret = TCP::recvData(_sockfds[idx], msg, size))) {
            std::cerr << "Failed to get Payload data\n";
            return ret;
        }

        if (newPayloadsLeft) {
            pending.push(payloadsLeft);
            payloadsLeft = newPayloadsLeft;
        }

        if (!payloadsLeft && !pending.empty()) {
            payloadsLeft = pending.top();
            pending.pop();
        }
    }

    return 0;
}

// Reads bytestream for Put
ssize_t KVNetTCP::_readPut(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get WaitAndGet additional info plus Key idx
    if ((ret = TCP::recvData(_sockfds[idx], msg, 16))) {
        std::cerr << "Failed to get Put data\n";
        return ret;
    }

    if ((ret = _readPayload(_sockfds[idx], msg))) {
        std::cerr << "Failed to get Put key\n";
        return ret;
    }

    if ((ret = _readPayload(_sockfds[idx], msg))) {
        std::cerr << "Failed to get Put payload\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Reply
ssize_t KVNetTCP::_readReply(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;

    if ((ret = _readPayload(_sockfds[idx], msg))) {
        std::cerr << "Failed to get Reply payload\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Get
ssize_t KVNetTCP::_readGet(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get WaitAndGet additional info plus Key idx
    if ((ret = TCP::recvData(_sockfds[idx], msg, 16))) {
        std::cerr << "Failed to get Get data\n";
        return ret;
    }

    if ((ret = _readPayload(_sockfds[idx], msg))) {
        std::cerr << "Failed to get Get key\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for WaitAndGet
ssize_t KVNetTCP::_readWaitAndGet(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get WaitAndGet additional info plus Key idx
    if ((ret = TCP::recvData(_sockfds[idx], msg, 20))) {
        std::cerr << "Failed to get WaitAndGet data\n";
        return ret;
    }

    if ((ret = _readPayload(_sockfds[idx], msg))) {
        std::cerr << "Failed to get WaitAndGet key\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Directory
ssize_t KVNetTCP::_readDirectory(uint64_t idx, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get Directory additional info
    if ((ret = TCP::recvData(_sockfds[idx], msg, 16))) {
        std::cerr << "Failed to get Directory data\n";
        return ret;
    }

    uint64_t nodes = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - sizeof(uint64_t)));

    if ((ret = TCP::recvData(_sockfds[idx], msg,
                             nodes * (sizeof(uint32_t) + sizeof(uint16_t))))) {
        std::cerr << "Failed to get Directory data\n";
        return ret;
    }

    return 0;
}