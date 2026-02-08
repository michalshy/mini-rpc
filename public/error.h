#pragma once

#include <cstdint>
namespace mini_rpc {
enum class Error : uint8_t { MethodNotFound, InvalidArguments, HandlerError, Internal };
}