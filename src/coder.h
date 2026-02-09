#pragma once

#include "wrapper.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <utility>
#include <vector>

using buffer = std::vector<std::byte>;

namespace mini_rpc {

inline void encode_u8(buffer& buf, uint8_t v) {
    auto p = reinterpret_cast<const std::byte*>(&v);
    buf.insert(buf.end(), p, p + sizeof(v));
}

inline void encode_u16(buffer& buf, uint16_t v) {
    auto p = reinterpret_cast<const std::byte*>(&v);
    buf.insert(buf.end(), p, p + sizeof(v));
}

inline uint16_t decode_u16(const std::byte* data) {
    uint16_t value;
    std::memcpy(&value, data, sizeof(uint16_t));
    return value;
}

inline void encode_u32(buffer& buf, uint32_t v) {
    auto p = reinterpret_cast<const std::byte*>(&v);
    buf.insert(buf.end(), p, p + sizeof(v));
}

inline void encode_bytes(buffer& buf, const void* data, size_t size) {
    auto p = reinterpret_cast<const std::byte*>(data);
    buf.insert(buf.end(), p, p + size);
}

inline buffer decode_bytes(const std::byte* data, size_t size) {
    return buffer(data, data + size);
}

template<typename T>
    requires std::is_trivially_copyable_v<T>
void encode_arg(buffer& buf, const T& value) {
    auto p = reinterpret_cast<const std::byte*>(&value);
    buf.insert(buf.end(), p, p + sizeof(T));
}

template<typename T>
    requires std::is_trivially_copyable_v<T>
T decode_arg(const std::byte* data, int& offset) {
    T value;
    std::memcpy(&value, data + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

template<typename... Args>
void encode_args(buffer& buf, Args&&... args) {
    (encode_arg(buf, std::forward<Args>(args)), ...);
}

template<typename Tuple, std::size_t... I>
Tuple decode_args_impl(const std::byte* data, int& offset, std::index_sequence<I...>) {
    return Tuple{decode_arg<std::tuple_element_t<I, Tuple>>(data, offset)...};
}

template<typename Tuple>
Tuple decode_args(const std::byte* data, int& offset) {
    return decode_args_impl<Tuple>(data, offset,
                                   std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}
} // namespace mini_rpc