/**
 * @file registrar.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */
#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared_mutex>
#include <unordered_map>
#include <vector>

class Registrar {
   private:
    int _sockfd;
    std::vector<struct sockaddr_in> _directory;
    std::shared_mutex _dirMutex;
    std::unordered_map<uint64_t, uint16_t>
        _sockfds;  // map of connections from node index to socket

   public:
    Registrar(const char* port, size_t maxConn);

    void start();
    void stop();
    size_t addNode(in_addr_t address, in_port_t port);
};