#pragma once

#include <vector>

template <typename T>
class Chunk {
   private:
    std::vector<T> __data;

   public:
    Chunk(size_t size);
};

template <typename T>
Chunk<T>::Chunk(size_t size) : __data(size) {}