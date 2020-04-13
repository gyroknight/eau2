/**
 * @file register.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#pragma once

#include <netdb.h>

#include "message.hpp"

class Register : public Message {
   private:
    struct sockaddr_in _context;

   public:
    Register(in_addr_t address, in_port_t port);

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};