#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>

namespace TCP {

inline struct addrinfo* generateAddrinfo(const char* port,
                                         const char* address = nullptr) {
    int rv;
    struct addrinfo hints;
    struct addrinfo* info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // No address provided, use own address
    if (!address) hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(address, port, &hints, &info))) {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
        throw std::runtime_error("Failed to get network info");
    }

    return info;
}

inline int createSocket(struct addrinfo* info, const char* port,
                        const char* address) {
    int sockfd;
    struct addrinfo* connInfo = info ? info : generateAddrinfo(port, address);
    if ((sockfd = socket(connInfo->ai_family, connInfo->ai_socktype,
                         connInfo->ai_protocol)) == -1) {
        if (!info) freeaddrinfo(connInfo);
        throw std::runtime_error("Failed to create socket");
    }
    if (!info) freeaddrinfo(connInfo);

    return sockfd;
}

inline int createSocket(struct addrinfo* info) {
    return createSocket(info, nullptr, nullptr);
}

inline int createSocket(const char* port) {
    return createSocket(nullptr, port, nullptr);
}

inline int createSocket(const char* port, const char* address) {
    return createSocket(nullptr, port, address);
}

inline void bindToPort(int sockfd, const char* port) {
    struct addrinfo* connInfo = generateAddrinfo(port);
    int enableReuseAddr = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr,
                   sizeof(int)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }

    if (bind(sockfd, connInfo->ai_addr, connInfo->ai_addrlen)) {
        close(sockfd);
        freeaddrinfo(connInfo);
        throw std::runtime_error("Unable to bind to port");
    }

    freeaddrinfo(connInfo);
}

inline ssize_t sendPacket(int socket,
                          std::unique_ptr<std::vector<uint8_t>> packet,
                          int flags = 0) {
    ssize_t totalSent = 0;
    ssize_t bytesLeft = packet->size();
    ssize_t sent;

    // Try once in case we don't want to block
    sent = send(socket, packet->data() + totalSent, packet->size() - totalSent,
                flags);
    if (sent == -1) return -errno;
    totalSent += sent;
    bytesLeft -= sent;

    // Send rest of data
    while (totalSent < packet->size()) {
        sent = send(socket, packet->data() + totalSent,
                    packet->size() - totalSent, 0);
        if (sent == -1) break;
        totalSent += sent;
        bytesLeft -= sent;
    }

    return sent == -1 ? -errno : 0;
}

inline ssize_t recvData(int socket, void* buf, size_t bytes) {
    ssize_t totalRecvd = 0;
    ssize_t bytesLeft = bytes;
    ssize_t recvd;

    uint8_t* dataStart = static_cast<uint8_t*>(buf);

    while (totalRecvd < bytes) {
        recvd = recv(socket, dataStart + totalRecvd, bytesLeft, 0);
        if (recvd == -1) break;
        totalRecvd += recvd;
        bytesLeft += recvd;
    }

    return recvd == -1 ? -errno : 0;
}

// Receives bytes amount of data from socket and appends it to current
inline ssize_t recvData(int socket, std::vector<uint8_t>& current,
                        size_t bytes) {
    current.resize(current.size() + bytes);
    uint8_t* dataStart = current.data() - bytes;

    return recvData(socket, dataStart, bytes);
}
}  // namespace TCP