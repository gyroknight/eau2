#include "registrar.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>

namespace {
constexpr const char* PORT = "4500";
constexpr size_t NUM_NODES = 5;
}  // namespace

Registrar::Registrar(const char* port, size_t maxConn) {
    int rv;
    struct addrinfo hints;
    struct addrinfo* servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(nullptr, PORT, &hints, &servinfo))) {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
        throw std::runtime_error("Failed to get network info");
    }

    int enableReuseAddr = 1;

    if ((_sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                          servinfo->ai_protocol)) == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr,
                   sizeof(int)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }

    if (bind(_sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) {
        close(_sockfd);
        throw std::runtime_error("Unable to bind to port");
    }

    freeaddrinfo(servinfo);

    _directory.reserve(maxConn);
}

Registrar::~Registrar() {
    for (struct in_addr& node : _directory) {
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

size_t Registrar::addNode(in_addr_t address) {
    const std::lock_guard<std::shared_mutex> lock(_dirMutex);
    struct in_addr node;
    node.s_addr = address;
    _directory.push_back(node);
    return _directory.size() - 1;
}

int main(int argc, char* argv[]) {
    Registrar registrar{PORT, NUM_NODES};

    registrar.start();

    return 0;
}