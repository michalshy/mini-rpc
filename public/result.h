#pragma once

#include "error.h"

#include <cstring>
#include <expected>
#include <optional>
#include <type_traits>
#include <vector>

namespace mini_rpc {
using buffer = std::vector<std::byte>;

class Result {

    std::expected<buffer, Error> data;

public:
    Result(Error error) : data(std::unexpected(error)) {}
    Result(buffer buffer) : data(std::move(buffer)) {}

    /// Returns true if the RPC call succeeded.
    [[nodiscard]] bool ok() const;

    /// Extracts the result value. T must match the server handler's return type.
    /// @throws Error is the RPC call failed (!ok())
    /// @throws Error if T doesn't match the encoded payload size
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    T as() const {
        if (!ok()) {
            throw data.error();
        }
        if (data->size() != sizeof(T)) {
            throw Error::InvalidPayload;
        }
        T value;
        std::memcpy(&value, data->data(), sizeof(T));
        return value;
    }

    /// Extracts the result value without throwing.
    /// @returns std::nullopt if the call failed or T doesn't match
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    std::optional<T> try_as() const {
        if (!ok() || data->size() != sizeof(T)) {
            return std::nullopt;
        }
        T value;
        std::memcpy(&value, data->data(), sizeof(T));
        return value;
    }

    /// Returns error, only valid for !ok()
    [[nodiscard]] Error error() const;
};
} // namespace mini_rpc