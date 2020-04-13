/**
 * @file serializer.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

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
    std::unique_ptr<std::vector<uint8_t>> _bytestream =
        std::make_unique<std::vector<uint8_t>>();

   public:
    Serializer();

    template <typename T>
    Serializer& add(T value);

    template <typename T>
    Serializer& add(std::shared_ptr<Column<T>> col);

    Serializer& add(std::shared_ptr<DataFrame> df);

    template <typename T>
    Serializer& add(std::vector<T> vector);

    Serializer& addBytes(void* bytes, size_t size);

    std::unique_ptr<std::vector<uint8_t>> generate();
};

#include "serializer.tpp"