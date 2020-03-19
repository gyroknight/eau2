#pragma once

#include <cassert>
#include <cstddef>

namespace {
// floor(log2(x)) for help mapping indices -> arrays
// from https://stackoverflow.com/a/994623
inline size_t log2_(size_t val) {
    assert(val != 0);

    size_t lg = 0;
    while (val >>= 1) ++lg;

    return lg;
}
}  // namespace

template <class T>
inline Column<T>::Column() : __data() {}

template <typename T>
inline Column<T>::Column(std::initializer_list<T> ll) : __data(ll) {}

template <typename T>
inline Column<T>::Column(Column<T>& other) : __data(other.__data) {}

template <typename T>
inline T Column<T>::get(size_t idx) {
    assert(idx < __size);
    Array<T>* arr = __data.get(0);

    // easy case - within the first array
    if (idx < arr->size()) return arr[idx];

    // get the index as if the first array is 1 element long
    idx = idx - arr->size() + 1;

    // which array to look in
    size_t arridx = log2_(idx + 1);
    arr = __data.get(arridx);

    // get the offset in that tree (subtract math.pow(2, arridx))
    size_t offidx = idx + 1 - arr->size();

    return arr[offidx];
}

template <typename T>
inline void Column<T>::set(size_t idx, T val) {
    assert(idx < size_);

    // same logic as get for index logic

    Array<T>* arr = __data->get(0);

    if (idx < arr->size()) {
        arr[idx] = val;
        return;
    }

    idx = idx - arr->size() + 1;

    size_t arridx = log2_(idx + 1);
    arr = __data->get(arridx);

    size_t offidx = idx + 1 - arr->size();
    arr[offidx] = val;
}

template <typename T>
inline void Column<T>::push_back(T val) {
    Array<T>* arr = __data->get(0);

    size_t idx = __size;

    if (idx < arr->size()) {
        // val fits in the first array
        arr[idx] = val;
        __size++;
        return;
    }

    idx = idx - arr->size() + 1;
    size_t arridx = log2_(idx + 1);  // should always be > 1 here

    // last array
    arr = __data->get(__data->size() - 1);

    if (idx + 1 < ((size_t)1 << __data->size())) {
        // val fits in the final array (could also check idx + 1 < 1 <<
        // arrays_->size())

        assert(arridx == __data->size() - 1);  // invariant check

        size_t offidx = idx + 1 - arr->size();

        arr[offidx] = val;
        __size++;
    } else {
        // val needs to go in a new array - double the size of the previous
        // one

        assert(arridx == __data->size());  // invariant check

        // TODO maybe cap this size - would require redoing a lot of these
        // methods because the math doesn't work as nicely
        FloatArray* newarr = new FloatArray(1 << __data->size());
        __data->push_back(newarr);
        newarr[0] = val;
        __size++;
    }
}