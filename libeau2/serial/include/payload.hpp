// lang::CwC
#pragma once

#include <vector>

#include "column.hpp"
#include "commondefs.hpp"
#include "serial.hpp"

class DataFrame;
class Key;
class Serializer;

// Serialized data to include with messages
class Payload {
   private:
    Serial::Type _type = Serial::Type::Unknown;
    uint64_t _payloadsLeft = 0;
    std::vector<uint8_t> _data;
    std::shared_ptr<void> _ref = nullptr;

    friend class Serializer;

    template <typename T>
    void _addColType(const Column<T>& col);

   public:
    Payload(std::vector<uint8_t>::iterator start,
            std::vector<uint8_t>::iterator end);

    Payload(double val);

    Payload(std::string& str);

    template <typename T>
    Payload(std::shared_ptr<Column<T>> col);

    Payload(const Key& key);

    Payload(std::shared_ptr<DataFrame> df);

    size_t size() const;

    Serial::Type type();

    uint64_t asU64();

    double asDouble();

    ExtString asString();

    std::shared_ptr<DataFrame> asDataFrame();

    void serialize(Serializer& ss);
};

#include "payload.tpp"