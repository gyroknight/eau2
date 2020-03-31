// lang::CwC
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "serial.hpp"

using ExtString = std::shared_ptr<std::string>;

// Serialized data to include with messages
class Payload {
   private:
    Serial::Type __type = Serial::Type::Unknown;
    std::vector<uint8_t> __data;

    friend class Serializer;

   public:
    Payload(std::vector<uint8_t>::iterator start,
            std::vector<uint8_t>::iterator end);

    Payload(double val);

    Payload(std::string& str);

    size_t size() const;

    Serial::Type type();

    double asDouble();

    ExtString asString();

    uint64_t asU64();
};