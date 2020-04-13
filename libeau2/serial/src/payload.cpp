/**
 * @file reply.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "payload.hpp"

#include <cstring>

#include "dataframe.hpp"

Payload::Payload(std::vector<uint8_t>::iterator start,
                 std::vector<uint8_t>::iterator end) {
    if (std::distance(start, end) < Serial::PAYLOAD_HDR_SIZE)
        throw std::invalid_argument("Payload is too small");
    _type = Serial::valueToType(*start++);
    uint64_t dataSize = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    if (std::distance(start, end) != dataSize)
        throw std::invalid_argument("Invalid Payload header");
    _data = std::vector<uint8_t>(start, end);
}

Payload::Payload(double val) : _type(Serial::Type::Double) {
    _data.resize(sizeof(double));
    memcpy(_data.data(), &val, _data.size());
}

Payload::Payload(std::string& str) : _type(Serial::Type::String) {
    _data.resize(str.length() + 1);
    memcpy(_data.data(), str.c_str(), _data.size());
}

Payload::Payload(std::shared_ptr<DataFrame> df)
    : _type(Serial::Type::DataFrame),
      _payloadsLeft(df->_data.size()),
      _ref(df) {}

size_t Payload::size() const { return _data.size() + Serial::PAYLOAD_HDR_SIZE; }

Serial::Type Payload::type() { return _type; }

uint64_t Payload::asU64() {
    if (_type != Serial::Type::U64)
        throw std::runtime_error("Payload is not uint64");
    return *reinterpret_cast<uint64_t*>(_data.data());
}

double Payload::asDouble() {
    if (_type != Serial::Type::Double)
        throw std::runtime_error("Payload is not double");

    return *reinterpret_cast<double*>(_data.data());
}

ExtString Payload::asString() {
    if (_type != Serial::Type::String)
        throw std::runtime_error("Payload is not string");
    return std::make_shared<std::string>(
        reinterpret_cast<const char*>(_data.data()));
}

std::shared_ptr<DataFrame> Payload::asDataFrame() {
    if (_type != Serial::Type::DataFrame)
        throw std::runtime_error("Payload is not a dataframe");
    return std::static_pointer_cast<DataFrame>(_ref);
}

// TODO: Lots to put here, need to show how Keys, Columns, and Dataframes are
// encoded
void Payload::serialize(Serializer& ss) {}