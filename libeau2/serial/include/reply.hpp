/**
 * @file reply.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "message.hpp"

class Payload;

// Data requested by another node
class Reply : public Message {
   private:
    std::shared_ptr<Payload> _payload = nullptr;

    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   public:
    Reply(size_t sender, size_t target, size_t id);

    template <typename T>
    void setPayload(T payload);

    std::shared_ptr<Payload> payload();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};

#include "reply.tpp"