// lang::CwC
#include "serializer.hpp"

Serializer::Serializer() {}

std::unique_ptr<std::vector<uint8_t>> Serializer::generate() {
    _bytestream->shrink_to_fit();
    std::unique_ptr<std::vector<uint8_t>> ret = std::move(_bytestream);
    _bytestream = std::make_unique<std::vector<uint8_t>>();
    return ret;
}

Serializer& Serializer::addBytes(void* bytes, size_t size) {
    uint8_t* data = static_cast<uint8_t*>(bytes);
    for (size_t ii = 0; ii < size; ii++) _bytestream->push_back(data[ii]);
    return *this;
}