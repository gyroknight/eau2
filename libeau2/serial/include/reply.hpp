/**
 * @file reply.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */
#pragma once

#include "column.hpp"
#include "dataframe.hpp"
#include "key.hpp"
#include "message.hpp"
#include "payload.hpp"

// Data requested by another node
class Reply : public Message {
   private:
    std::shared_ptr<Payload> _payload = nullptr;

   public:
    Reply(size_t sender, size_t target, size_t id);

    template <typename T>
    void setPayload(T payload);

    std::shared_ptr<Payload> payload();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};

#include "reply.tpp"