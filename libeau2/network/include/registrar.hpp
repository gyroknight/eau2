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
#include <vector>

class Registrar {
   private:
    int _sockfd;
    std::vector<struct sockaddr_in> _directory;
    std::shared_mutex _dirMutex;

   public:
    Registrar(const char* port, size_t maxConn);
    ~Registrar();

    void start();
    size_t addNode(in_addr_t address, in_port_t port);
};