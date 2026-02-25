#pragma once

#include <expected>
#include <vector>
namespace mini_rpc {

using buffer = std::vector<std::byte>;

enum class E {
    SOCKET_ERROR,
    CONNECT_ERROR,
    SEND_ERROR,
    WRITE_ERROR,
    RECV_ERROR,
    READ_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
};

using ConnectionResult = std::expected<void, E>;
using TransferResult = std::expected<size_t, E>;
} // namespace mini_rpc
