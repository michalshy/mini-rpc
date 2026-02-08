#pragma once

#include <cstdint>
namespace mini_rpc {
enum class Error : uint8_t {
    None = 0,
    MethodNotFound,
    InvalidArguments,
    HandlerError,
    Internal,
    ConnectionLost,
    InvalidPayload,
};
}