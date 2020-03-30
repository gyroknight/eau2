#pragma once

template <typename T>
inline void Reply::setPayload(const Column<T>& col) {
    if (!__payloads.empty()) throw std::runtime_error("Payload already set");
}