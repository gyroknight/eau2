/**
 * @file register.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <netinet/in.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "message.hpp"

class Register : public Message {
   private:
    struct sockaddr_in _context;

    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   public:
    Register(in_addr_t address, in_port_t port);

    struct sockaddr_in context();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};