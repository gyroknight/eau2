#pragma once

#include <exception>

template <typename T>
inline Chunk<T>::Chunk() {}

template <typename T>
inline Chunk<T>::Chunk(Chunk<T>&& other) : _data(std::move(other._data)) {}

template <typename T>
inline T& Chunk<T>::operator[](size_t idx) {
    return _data[idx];
}

template <typename T>
inline T& Chunk<T>::at(size_t idx) {
    return _data.at(idx);
}