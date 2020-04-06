#pragma once

#include <exception>

template <typename T>
inline Chunk<T>::Chunk() {}

// move constructor
template <typename T>
inline Chunk<T>::Chunk(Chunk<T>&& other) : _data(std::move(other._data)) {}

// access element
template <typename T>
inline T& Chunk<T>::operator[](size_t idx) {
    return _data[idx];
}

// access element with bounds checking
template <typename T>
inline T& Chunk<T>::at(size_t idx) {
    return _data.at(idx);
}