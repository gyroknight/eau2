#pragma once

#include <cassert>
#include <cstddef>
#include <sstream>

namespace {}  // namespace

template <class T>
Column<T>::Column() : _size(0) {}

template <typename T>
Column<T>::Column(std::initializer_list<T> ll) : _size(ll.size()) {
    size_t chunkedSize = ll.size() / Chunk<T>::size();
    if (ll.size() % Chunk<T>::size()) chunkedSize++;
    _data.reserve(chunkedSize);

    for (const T& e : ll) {
        push_back(e);
    }
}

template <typename T>
T Column<T>::get(size_t idx) const {
    size_t chunkIdx = idx / Chunk<T>::size();
    size_t itemIdx = idx % Chunk<T>::size();

    return (*_data[chunkIdx])[itemIdx];
}

template <typename T>
void Column<T>::set(size_t idx, T val) {
    // same logic as get for index logic
    size_t chunkIdx = idx / Chunk<T>::size();
    size_t itemIdx = idx % Chunk<T>::size();

    (*_data[chunkIdx])[itemIdx] = val;
}

template <typename T>
void Column<T>::push_back(T val) {
    size_t chunkIdx;
    size_t itemIdx = _size++ % Chunk<T>::size();
    if (itemIdx == 0) {
        _data.push_back(std::make_shared<Chunk<T>>());
    }

    chunkIdx = _data.size() - 1;
    (*_data[chunkIdx])[itemIdx] = val;
}

template <typename T>
size_t Column<T>::size() const {
    return _size;
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