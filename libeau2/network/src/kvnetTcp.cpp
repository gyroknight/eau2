/**
 * @file kvnetTcp.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "kvnetTcp.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

#include "directory.hpp"
#include "message.hpp"
#include "register.hpp"
#include "serial.hpp"
#include "tcpUtils.hpp"

namespace {
constexpr const char* REGISTRAR = "127.0.0.1";
constexpr const char* PORT = "4500";
constexpr size_t MAX_EVENTS = 10;
constexpr int POLL_TIMEOUT_MS = 500;
}  // namespace

KVNetTCP::KVNetTCP() {}
KVNetTCP::~KVNetTCP() {
    shutdown();
    std::cout << "Shutting down network...";
    // Wait for sender and receiver to stop
    _senderThread.join();
    _receiverThread.join();
    std::cout << " done." << std::endl;
}

// Registers node with the registrar and awaits directory before starting sender
// and receiver
size_t KVNetTCP::registerNode(const char* address, const char* port) {
    // Can't register more than once
    if (_idx) throw std::runtime_error("Node is already registered");

    ssize_t ret;
    struct addrinfo* registrarInfo = TCP::generateAddrinfo(PORT, REGISTRAR);

    // registrar is always node 0
    _sendSocks[0] = TCP::createSocket(registrarInfo);

    if (connect(_sendSocks[0], registrarInfo->ai_addr,
                registrarInfo->ai_addrlen) == -1) {
        close(_sendSocks[0]);
        freeaddrinfo(registrarInfo);
        throw std::runtime_error("Unable to connect to registrar");
    }

    freeaddrinfo(registrarInfo);

    // send registration
    struct addrinfo* connInfo = TCP::generateAddrinfo(port, address);
    struct sockaddr_in* connAddr =
        reinterpret_cast<struct sockaddr_in*>(connInfo->ai_addr);
    Register regMsg(connAddr->sin_addr.s_addr, connAddr->sin_port);
    freeaddrinfo(connInfo);

    ret = TCP::sendPacket(_sendSocks[0], regMsg.serialize());

    // wait for directory
    std::unique_ptr<Message> msg = _readMsg(_sendSocks[0]);
    std::unique_ptr<Directory> dirMsg(dynamic_cast<Directory*>(msg.release()));
    if (!dirMsg) {
        throw std::runtime_error("Invalid Directory from registrar");
    }

    // Set node index and populate directory
    _idx = dirMsg->idx();
    std::cout << "Node is index " << _idx << std::endl;
    _dir.insert(std::end(_dir), std::begin(dirMsg->dir()),
                std::end(dirMsg->dir()));

    // start sender and receiver
    _senderThread = std::thread(&KVNetTCP::_sender, this);
    _receiverThread = std::thread(&KVNetTCP::_receiver, this, port);

    _netUp = true;

    return _idx;
}

// Might want to play around with serialization here instead of in event handler
// Adds a Message to be sent
void KVNetTCP::send(std::shared_ptr<Message> msg) {
    // Loopback interface
    if (msg->target() == _idx) {
        std::unique_ptr<Message> msgCopy =
            Message::deserialize(msg->serialize());
        const std::lock_guard<std::mutex> lock(_receiveLock);
        _receiving.push(std::move(msgCopy));
    } else {
        const std::lock_guard<std::mutex> lock(_sendLock);
        _sending.push(msg);
        _senderCv.notify_all();
    }
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

bool KVNetTCP::ready() {
    static std::atomic_bool wasReady = false;

    if (wasReady) return _netUp;
    // if (!_netUp) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(POLL_TIMEOUT_MS));
    // }

    // For debug, we want to block indefinitely
    while (!_netUp) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    wasReady = wasReady || _netUp;

    return _netUp;
}

void KVNetTCP::shutdown() { _netUp = false; }

// Reads in Message bytestream from node and deserializes
std::unique_ptr<Message> KVNetTCP::_readMsg(int sock) {
    auto msg = std::make_unique<std::vector<uint8_t>>();

    // Read Command Header
    if (TCP::recvData(sock, *msg, Serial::CMD_HDR_SIZE)) {
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
            if (_readPut(sock, *msg)) return nullptr;
            break;
        case MsgKind::Reply:
            if (_readReply(sock, *msg)) return nullptr;
            break;
        case MsgKind::Get:
            if (_readGet(sock, *msg)) return nullptr;
            break;
        case MsgKind::WaitAndGet:
            if (_readWaitAndGet(sock, *msg)) return nullptr;
            break;
        case MsgKind::Directory:
            if (_readDirectory(sock, *msg)) return nullptr;
            break;
        default:
            return nullptr;
    }

    return Message::deserialize(std::move(msg));
}

// Reads bytestream for a Payload
ssize_t KVNetTCP::_readPayload(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;
    std::stack<uint64_t> pending;  // Keeps track of pending Payloads for
                                   // multi-Payload transmissions

    // Get first Payload
    if ((ret = TCP::recvData(sock, msg, Serial::PAYLOAD_HDR_SIZE))) {
        std::cerr << "Failed to get Payload header\n";
        return ret;
    }

    uint64_t payloadsLeft = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - 2 * sizeof(uint64_t)));
    uint64_t size = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - sizeof(uint64_t)));

    if ((ret = TCP::recvData(sock, msg, size))) {
        std::cerr << "Failed to get Payload data\n";
        return ret;
    }

    // Get next Payloads if applicable
    while (payloadsLeft) {
        if ((ret = TCP::recvData(sock, msg, Serial::PAYLOAD_HDR_SIZE))) {
            std::cerr << "Failed to get Payload header\n";
            return ret;
        }

        uint64_t newPayloadsLeft = *reinterpret_cast<uint64_t*>(
            msg.data() + (msg.size() - 2 * sizeof(uint64_t)));
        size = *reinterpret_cast<uint64_t*>(msg.data() +
                                            (msg.size() - sizeof(uint64_t)));

        if ((ret = TCP::recvData(sock, msg, size))) {
            std::cerr << "Failed to get Payload data\n";
            return ret;
        }

        payloadsLeft--;

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
ssize_t KVNetTCP::_readPut(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;

    if ((ret = TCP::recvData(sock, msg, 2 * sizeof(uint64_t)))) {
        std::cerr << "Failed to get Put data\n";
        return ret;
    }

    if ((ret = _readPayload(sock, msg))) {
        std::cerr << "Failed to get Put key\n";
        return ret;
    }

    if ((ret = _readPayload(sock, msg))) {
        std::cerr << "Failed to get Put payload\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Reply
ssize_t KVNetTCP::_readReply(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;

    if ((ret = _readPayload(sock, msg))) {
        std::cerr << "Failed to get Reply payload\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Get
ssize_t KVNetTCP::_readGet(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get WaitAndGet additional info plus Key idx
    if ((ret = TCP::recvData(sock, msg, 16))) {
        std::cerr << "Failed to get Get data\n";
        return ret;
    }

    if ((ret = _readPayload(sock, msg))) {
        std::cerr << "Failed to get Get key\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for WaitAndGet
ssize_t KVNetTCP::_readWaitAndGet(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get WaitAndGet additional info plus Key idx
    if ((ret = TCP::recvData(sock, msg, 20))) {
        std::cerr << "Failed to get WaitAndGet data\n";
        return ret;
    }

    if ((ret = _readPayload(sock, msg))) {
        std::cerr << "Failed to get WaitAndGet key\n";
        return ret;
    }

    return 0;
}

// Reads bytestream for Directory
ssize_t KVNetTCP::_readDirectory(int sock, std::vector<uint8_t>& msg) {
    ssize_t ret;

    // Get Directory additional info
    if ((ret = TCP::recvData(sock, msg, 16))) {
        std::cerr << "Failed to get Directory data\n";
        return ret;
    }

    uint64_t nodes = *reinterpret_cast<uint64_t*>(
        msg.data() + (msg.size() - sizeof(uint64_t)));

    if ((ret = TCP::recvData(sock, msg,
                             nodes * (sizeof(uint32_t) + sizeof(uint16_t))))) {
        std::cerr << "Failed to get Directory data\n";
        return ret;
    }

    return 0;
}

void KVNetTCP::_sender() {
    // Delay if network status isn't set as up, shouldn't take more than 500 ms
    // to change
    ready();

    while (_netUp) {
        std::unique_lock<std::mutex> lock(_sendLock);
        if (_senderCv.wait_for(lock, std::chrono::milliseconds(POLL_TIMEOUT_MS),
                               [this] { return !_sending.empty(); })) {
            while (!_sending.empty()) {
                // Send all pending Messages
                std::shared_ptr<Message> msg = _sending.front();
                _sending.pop();

                uint64_t target = msg->target();

                if (!_sendSocks.count(target)) {
                    // No existing connection, create socket
                    char connIP[INET_ADDRSTRLEN];
                    auto connPort =
                        std::to_string(htons(_dir[target].sin_port));

                    struct addrinfo* connAddrinfo = TCP::generateAddrinfo(
                        connPort.c_str(),
                        inet_ntop(AF_INET, &(_dir[target].sin_addr), connIP,
                                  sizeof(connIP)));

                    _sendSocks[target] = TCP::createSocket(connAddrinfo);

                    if (connect(_sendSocks[target], connAddrinfo->ai_addr,
                                connAddrinfo->ai_addrlen) == -1) {
                        close(_sendSocks[target]);
                        freeaddrinfo(connAddrinfo);
                        std::cerr << "Failed to connect to node " << target
                                  << '\n';
                        throw std::runtime_error("Unable to connect to node");
                    }

                    freeaddrinfo(connAddrinfo);
                }

                if (TCP::sendPacket(_sendSocks[target], msg->serialize())) {
                    throw std::runtime_error("Failed to send Message");
                }

                std::cout << "Network message sent: " << msg->sender() << " -> "
                          << msg->target() << std::endl;
            }
        }
    }
}

void KVNetTCP::_receiver(const char* port) {
    std::vector<int> sockfds;

    int listenSock = TCP::createSocket(port);
    TCP::bindToPort(listenSock, port);
    if (listen(listenSock, _dir.size())) {
        throw std::runtime_error("Unable to listen to port");
    }
    sockfds.push_back(listenSock);
    std::cout << "Listen socket is " << listenSock << std::endl;

    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }

    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = listenSock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenSock, &ev) == -1) {
        throw std::runtime_error("Failed to add listen socket to epoll");
    }
    sockfds.push_back(epollfd);
    std::cout << "epoll socket is " << epollfd << std::endl;

    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = _sendSocks[0];
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, _sendSocks[0], &ev) == -1) {
        throw std::runtime_error("Failed to add registrar socket to epoll");
    }
    sockfds.push_back(_sendSocks[0]);

    int readyfds;
    struct sockaddr_storage connAddr;
    socklen_t addrLen;

    // Delay if network status isn't set as up, shouldn't take more than 500 ms
    // to change
    ready();

    while (_netUp) {
        readyfds = epoll_wait(epollfd, events, MAX_EVENTS, POLL_TIMEOUT_MS);

        for (int ii = 0; ii < readyfds; ii++) {
            std::cout << "Event: " << std::hex << events[ii].events
                      << std::endl;
            if (events[ii].events & EPOLLRDHUP) {
                std::cerr << "Socket disconnected\n";
                shutdown();
            } else if (events[ii].data.fd == listenSock) {
                int connSock = accept(
                    listenSock, reinterpret_cast<struct sockaddr*>(&connAddr),
                    &addrLen);

                if (connSock == -1)
                    throw std::runtime_error(
                        "Failed to accept incoming connection");

                if (!_inDirectory(
                        reinterpret_cast<struct sockaddr_in*>(&connAddr)
                            ->sin_addr.s_addr)) {
                    std::cerr << "Unknown connection, dropping\n";
                    close(connSock);
                    continue;
                }

                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = connSock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connSock, &ev) == -1)
                    throw std::runtime_error(
                        "Unable to add connection socket to epoll");
                sockfds.push_back(connSock);
                std::cout << "Added new connection " << connSock << std::endl;
            } else if (events[ii].events & EPOLLIN) {
                std::unique_ptr<Message> msg = _readMsg(events[ii].data.fd);

                if (!msg) {
                    std::cerr << "Invalid Message received\n";
                    continue;
                }

                if (msg->target() != _idx) {
                    std::cerr << "Message not for this node, dropping\n";
                    continue;
                }

                const std::lock_guard<std::mutex> lock(_receiveLock);
                std::cout << "Network message received: " << msg->sender()
                          << " -> " << msg->target() << std::endl;
                _receiving.push(std::move(msg));
            }
        }
    }

    // Shut down
    for (int& sockfd : sockfds) {
        close(sockfd);
    }
}

bool KVNetTCP::_inDirectory(in_addr_t address) {
    for (struct sockaddr_in& context : _dir) {
        if (context.sin_addr.s_addr == address) {
            return true;
        }
    }

    return false;
}