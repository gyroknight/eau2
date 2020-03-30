#pragma once

#include <cassert>
#include <cstddef>

#include <sstream>

namespace {}  // namespace

template <class T>
Column<T>::Column() : __size(0) {}

template <typename T>
Column<T>::Column(Column<T>&& other)
    : __data(std::move(other.__data)), __size() {
    other.__data.clear();
    other.__data.shrink_to_fit();
}

template <typename T>
Column<T>::Column(const Column<T>& other) : __size(other.__size) {
    __data.reserve(other.__data.size());

    // fill new vector with new ptrs to objects
    for (const auto& e : other._data) {
        __data.push_back(e);
    }
}

template <typename T>
Column<T>::Column(std::initializer_list<T> ll) : __size() {
    __data.reserve(ll.size());

    for (const auto& e : ll) {
        push_back(e);
    }
}

template <typename T>
T Column<T>::get(size_t idx) const {
    size_t chunkIdx = idx / Chunk<T>::size();
    size_t itemIdx = idx % Chunk<T>::size();

    return (*__data[chunkIdx])[itemIdx];
}

template <typename T>
void Column<T>::set(size_t idx, T val) {
    // same logic as get for index logic
    size_t chunkIdx = idx / Chunk<T>::size();
    size_t itemIdx = idx % Chunk<T>::size();

    (*__data[chunkIdx])[itemIdx] = val;
}

template <typename T>
void Column<T>::push_back(T val) {
    size_t chunkIdx;
    size_t itemIdx = __size++ % Chunk<T>::size();
    if (itemIdx == 0) {
        __data.push_back(std::make_shared<Chunk<T>>());
    }

    chunkIdx = __data.size() - 1;
    (*__data[chunkIdx])[itemIdx] = val;
}

template <typename T>
size_t Column<T>::size() const {
    return __size;
}

template <typename T>
std::string Column<T>::str() const {
    std::stringstream ss;

    for (size_t i = 0; i < size(); i++) {
        ss << get(i);
        if (i != size() - 1) {
            ss << ", ";
        }
    }

    return ss.str();
}