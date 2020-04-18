/**
 * @file serializer.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "commondefs.hpp"

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
    Serializer& add(ColPtr<T> col);

    Serializer& add(DFPtr df);

    template <typename T>
    Serializer& add(std::vector<T> vector);

    Serializer& addBytes(void* bytes, size_t size);

    std::unique_ptr<std::vector<uint8_t>> generate();
};

#include "serializer.tpp"