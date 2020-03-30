#pragma once

// specializations
template <>
inline char Schema::colToType(const Column<int>& col) {
    return 'I';
}

template <>
inline char Schema::colToType(const Column<double>& col) {
    return 'D';
}

template <>
inline char Schema::colToType(const Column<bool>& col) {
    return 'B';
}

template <>
inline char Schema::colToType(const Column<std::string>& col) {
    return 'S';
}

template <typename T>
inline char Schema::colToType(const Column<T>& col) {
    return 'U';
}