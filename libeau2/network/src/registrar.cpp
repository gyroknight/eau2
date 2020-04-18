/**
 * @file registrar.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "registrar.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>

#include "tcpUtils.hpp"

namespace {
constexpr const char* PORT = "4500";
constexpr size_t NUM_NODES = 5;
}  // namespace

Registrar::Registrar(const char* port, size_t maxConn) {
    _sockfd = TCP::createSocket(port);
    TCP::bindToPort(_sockfd, port);
    _directory.reserve(maxConn);
}

Registrar::~Registrar() {
    for (struct sockaddr_in& node : _directory) {
        // kill nodes
    }

    close(_sockfd);
}

void Registrar::start() {
    if (listen(_sockfd, NUM_NODES)) {
        throw std::runtime_error("Unable to listen to port");
    }

    while (true) {
        // listening logic
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

    return 0;
}