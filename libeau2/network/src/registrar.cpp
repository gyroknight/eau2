/**
 * @file registrar.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "registrar.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "directory.hpp"
#include "kill.hpp"
#include "register.hpp"
#include "serial.hpp"
#include "tcpUtils.hpp"

namespace {
constexpr const char* PORT = "4500";
constexpr size_t NUM_NODES = 1;
constexpr size_t MAX_EVENTS = 10;
}  // namespace

Registrar::Registrar(const char* port, size_t maxConn) {
    _sockfd = TCP::createSocket(port);
    TCP::bindToPort(_sockfd, port);
    _directory.reserve(maxConn + 1);
    // Reserve idx 0 so that node idx matches dir idx (idx is registrar)
    _directory.resize(1);
}

void Registrar::start() {
    if (listen(_sockfd, NUM_NODES)) {
        throw std::runtime_error("Unable to listen to port");
    }

    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }

    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = _sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, _sockfd, &ev) == -1) {
        throw std::runtime_error("Failed to add listen socket to epoll");
    }

    int readyfds;
    struct sockaddr_storage connAddr;
    socklen_t addrLen;

    while (_directory.size() < NUM_NODES + 1) {
        // listening logic
        readyfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        for (int ii = 0; ii < readyfds; ii++) {
            if (events[ii].events & EPOLLRDHUP) {
                std::cerr << "Socket disconnected\n";
                stop();
                return;
            } else if (events[ii].data.fd == _sockfd) {
                int connSock = accept(
                    _sockfd, reinterpret_cast<struct sockaddr*>(&connAddr),
                    &addrLen);

                if (connSock == -1)
                    throw std::runtime_error(
                        "Failed to accept incoming connection");

                // int flags = fcntl(connSock, F_GETFL);
                // if (flags == -1)
                //     throw std::runtime_error("Failed to open connection
                //     flags");
                // if (fcntl(connSock, F_SETFL, flags | O_NONBLOCK) == -1)
                //     throw std::runtime_error(
                //         "Failed to set connection as nonblocking");

                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = connSock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connSock, &ev) == -1)
                    throw std::runtime_error(
                        "Unable to add connection socket to epoll");
            } else {
                auto msgBuf = std::make_unique<std::vector<uint8_t>>();

                // Read Command Header
                if (TCP::recvData(events[ii].data.fd, *msgBuf,
                                  Serial::CMD_HDR_SIZE))
                    throw std::runtime_error("Failed to get command header");

                // Read Register Message
                if (Message::valueToMsgKind((*msgBuf)[0]) != MsgKind::Register)
                    throw std::runtime_error(
                        "Received non-Register message from connection");
                if (TCP::recvData(events[ii].data.fd, *msgBuf,
                                  sizeof(uint32_t) + sizeof(uint16_t)))
                    throw std::runtime_error("Failed to get node address info");

                auto msg = Message::deserialize(std::move(msgBuf));
                std::unique_ptr<Register> registerMsg(
                    dynamic_cast<Register*>(msg.release()));

                if (!registerMsg)
                    throw std::runtime_error(
                        "Corrupted Register message received");

                _directory.push_back(registerMsg->context());

                _sockfds[_directory.size() - 1] = events[ii].data.fd;

                std::cout << "Node " << _directory.size() - 1 << " registered"
                          << std::endl;

                if (_directory.size() >= NUM_NODES + 1) break;
            }
        }
    }

    // All nodes registered, do not accept any more connections
    close(_sockfd);

    for (size_t ii = 1; ii < _directory.size(); ii++) {
        Directory dir(0, ii, ii);
        for (struct sockaddr_in& context : _directory) {
            dir.addNode(context.sin_addr.s_addr, context.sin_port);
        }

        TCP::sendPacket(_sockfds[ii], dir.serialize());
    }
}

void Registrar::stop() {
    for (size_t ii = 1; ii < _directory.size(); ii++) {
        Kill killMsg(0, ii);
        TCP::sendPacket(_sockfds[ii], killMsg.serialize());
        close(_sockfds[ii]);
    }
}

// Adds a node to the directory
size_t Registrar::addNode(in_addr_t address, in_port_t port) {
    const std::lock_guard<std::shared_mutex> lock(_dirMutex);
    struct sockaddr_in node;
    node.sin_family = AF_INET;
    node.sin_addr.s_addr = address;
    node.sin_port = port;
    _directory.push_back(node);
    return _directory.size() - 1;
}

int main(int argc, char* argv[]) {
    Registrar registrar{PORT, NUM_NODES};

    registrar.start();

    // while (true) {
    //     usleep(1);
    // }
    sleep(5);

    registrar.stop();

    return 0;
}