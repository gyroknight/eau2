// lang::Cpp
#pragma once

#include "column.hpp"
#include "dataframe.hpp"
#include "key.hpp"
#include "message.hpp"
#include "payload.hpp"

class Reply : public Message {
   public:
    Serial::Type __type;
    std::vector<std::shared_ptr<Payload>> __payloads;

    Reply(size_t sender, size_t target, size_t id);

    template <typename T>
    void setPayload(const Column<T>& col);

    void setPayload(double val);

    void setPayload(std::shared_ptr<DataFrame> val);

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};

#include "reply.tpp"