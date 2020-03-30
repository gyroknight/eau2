#pragma once

#include <array>

namespace {
constexpr size_t CHUNK_SIZE = 10;
}  // namespace

template <typename T>
class Chunk {
   private:
    std::array<T, CHUNK_SIZE> __data;

   public:
    Chunk();

    // delete copy constructor
    Chunk(const Chunk&) = delete;

    // move constructor
    Chunk(Chunk<T>&& other);

    // access element
    T& operator[](size_t idx);

    T& at(size_t idx);

    // get size
    static size_t size() { return CHUNK_SIZE; };
};

#include "chunk.tpp"