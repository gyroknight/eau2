/**
 * @file directory.hpp
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

#include "commondefs.hpp"
#include "message.hpp"

class Directory : public Message {
   private:
    uint64_t _idx;
    std::vector<struct sockaddr_in> _dir;

    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   public:
    Directory(uint64_t sender, uint64_t target, uint64_t idx);

    void addNode(in_addr_t address, in_port_t port);
    uint64_t idx();
    std::vector<struct sockaddr_in>& dir();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};