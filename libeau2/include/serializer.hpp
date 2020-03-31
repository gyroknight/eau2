// lang::CwC
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "payload.hpp"
#include "serial.hpp"

// Builds a serial bytestream of items
class Serializer {
   private:
    std::unique_ptr<std::vector<uint8_t>> __bytestream =
        std::make_unique<std::vector<uint8_t>>();

   public:
    Serializer();

    template <typename T>
    Serializer& add(T value);
    template <typename T>
    Serializer& add(std::shared_ptr<T> ptr);
    template <typename T>
    Serializer& add(std::vector<T> vector);
    Serializer& addBytes(void* bytes, size_t size);
    std::unique_ptr<std::vector<uint8_t>> generate();
};

#include "serializer.tpp"