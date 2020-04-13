/**
 * @file serializer.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "serializer.hpp"

Serializer::Serializer() {}

Serializer& Serializer::add(std::shared_ptr<DataFrame> df) {
    Payload payload(df);
    payload.serialize(*this);
    return *this;
}

/**
 * @brief Adds size number of bytes from address to bytestream buffer.
 *
 * @param bytes
 * @param size
 * @return Serializer&
 */
Serializer& Serializer::addBytes(void* bytes, size_t size) {
    uint8_t* data = static_cast<uint8_t*>(bytes);
    for (size_t ii = 0; ii < size; ii++) _bytestream->push_back(data[ii]);
    return *this;
}

/**
 * @brief Finalizes the current buffer as a bytestream and clears buffer before
 * returning final bytestream
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Serializer::generate() {
    _bytestream->shrink_to_fit();
    std::unique_ptr<std::vector<uint8_t>> ret = std::move(_bytestream);
    _bytestream = std::make_unique<std::vector<uint8_t>>();
    return ret;
}