#pragma once

#include "common.h"
#include <cstddef>

namespace mini_rpc {
class ITransport {
public:
    virtual ~ITransport() = default;

    virtual ConnectionResult connect() = 0;
    virtual TransferResult send(const std::byte* data, size_t size) = 0;
    virtual TransferResult recv(std::byte* data, size_t size) = 0;
    virtual ConnectionResult close() = 0;
};
} // namespace mini_rpc
