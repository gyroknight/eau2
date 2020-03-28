#pragma once

#include <vector>

template <typename T>
class Chunk {
   private:
    std::vector<T> _data;

   public:
    Chunk(size_t size);

    // delete copy constructor
    Chunk(const Chunk&) = delete;

    // move constructor
    Chunk(Chunk<T>&& other);

    // access element
    T& at(size_t index);

    // get size
    size_t size() { return _data.size(); }
};

template <typename T>
Chunk<T>::Chunk(size_t size) : _data(size) {}

template <typename T>
Chunk<T>::Chunk(Chunk<T>&& other) : _data(std::move(other._data)) {}

template <typename T>
T& Chunk<T>::at(size_t index) {
    T& datum = _data.at(index);
    return datum;
}