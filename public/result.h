#pragma once

#include "error.h"

#include <expected>
#include <vector>

namespace mini_rpc {
using buffer = std::vector<std::byte>;

class Result {
    std::expected<buffer, Error> data;

public:
    bool ok() const;

    template<typename T>
    T as() const {}

    Error error() const;
};
} // namespace mini_rpc