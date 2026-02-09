#pragma once

#include <cstddef>

#ifdef __linux__
#include <sys/types.h>
#endif

#include <vector>

using buffer = std::vector<std::byte>;

namespace mini_rpc {
class ITransport {
public:
    virtual ~ITransport() = default;

    virtual void connect() = 0;
    virtual size_t send(const std::byte* data, size_t size) = 0;
    virtual size_t recv(std::byte* data, size_t size) = 0;
    virtual void close() = 0;
};
} // namespace mini_rpc