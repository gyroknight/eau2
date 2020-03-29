#pragma once

#include <cassert>
#include <cstddef>

const size_t CHUNK_SIZE = 8196;

template <typename T>
Column<T>::Column() : _data(), _size() {}

template <typename T>
Column<T>::Column(Column<T>&& other) : _data(std::move(other._data)), _size() {
    other._data.clear();
    other._data.shrink_to_fit();
}

template <typename T>
Column<T>::Column(const Column<T>& other) : _data(), _size() {
    _data.reserve(other._data.size());

    // fill new vector with new unique ptrs to objects
    for (const auto& e : other._data) {
        _data.push_back(e);
    }
}

template <typename T>
Column<T>::Column(std::initializer_list<T> ll) : _data(), _size() {
    _data.emplace_back(ll);
}

template <typename T>
T Column<T>::get(size_t idx) {
    assert(idx < _size);

    auto chunk = _data.at(idx / CHUNK_SIZE);

    return chunk->at(idx % CHUNK_SIZE);
}

template <typename T>
void Column<T>::set(size_t idx, T val) {
    assert(idx < _size);

    auto chunk = _data.at(idx / CHUNK_SIZE);

    chunk->at(idx % CHUNK_SIZE) = val;
}

template <typename T>
void Column<T>::push_back(T val) {

    // create new chunk if this is the first element of a new chunk
    if (_size % CHUNK_SIZE == 0) {
        _data.push_back(std::make_shared<Chunk<T>>(CHUNK_SIZE));
    }

    // put it in
    auto chunk = _data.at(_size / CHUNK_SIZE);
    chunk->at(_size % CHUNK_SIZE) = val;

    _size++;
}

template <typename T>
size_t Column<T>::size() const { return _size; }

// specializations
template<>
char Column<int>::type() const { return 'I'; }
template<>
char Column<double>::type() const { return 'D'; }
template<>
char Column<bool>::type() const { return 'B'; }
template<>
char Column<std::string>::type() const { return 'S'; }