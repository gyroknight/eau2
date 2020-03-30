#pragma once

#include <exception>

template <typename T>
inline Chunk<T>::Chunk() {}

template <typename T>
inline Chunk<T>::Chunk(Chunk<T>&& other) : __data(std::move(other.__data)) {}

template <typename T>
inline T& Chunk<T>::operator[](size_t idx) {
    return __data[idx];
}

template <typename T>
inline T& Chunk<T>::at(size_t idx) {
    return __data.at(idx);
}