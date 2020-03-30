// lang::Cpp
#include "payload.hpp"

#include <cstring>

Payload::Payload(std::vector<uint8_t>::iterator start,
                 std::vector<uint8_t>::iterator end) {
    if (std::distance(start, end) < Serial::PAYLOAD_HDR_SIZE)
        throw std::invalid_argument("Payload is too small");
    __type = Serial::valueToType(*start++);
    uint64_t dataSize = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    if (std::distance(start, end) != dataSize)
        throw std::invalid_argument("Invalid Payload header");
    __data = std::vector<uint8_t>(start, end);
}

Payload::Payload(double val) : __type(Serial::Type::Double) {
    __data.resize(sizeof(double));
    memcpy(__data.data(), &val, __data.size());
}

Payload::Payload(std::string& str) : __type(Serial::Type::String) {
    __data.resize(str.length() + 1);
    memcpy(__data.data(), str.c_str(), __data.size());
}

size_t Payload::size() const {
    return __data.size() + Serial::PAYLOAD_HDR_SIZE;
}

Serial::Type Payload::type() { return __type; }

double Payload::asDouble() {
    if (__type != Serial::Type::Double)
        throw std::runtime_error("Payload is not double");

    return *reinterpret_cast<double*>(__data.data());
}

ExtString Payload::asString() {
    if (__type != Serial::Type::String)
        throw std::runtime_error("Payload is not string");
    return std::make_shared<std::string>(
        reinterpret_cast<const char*>(__data.data()));
}

uint64_t Payload::asU64() {
    if (__type != Serial::Type::U64)
        throw std::runtime_error("Payload is not uint64");
    return *reinterpret_cast<uint64_t*>(__data.data());
}