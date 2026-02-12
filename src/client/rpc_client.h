#pragma once

#include "coder.h"
#include "error.h"
#include "frame/frame.h"
#include "result.h"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

// PLATFORM INCLUDES
#ifdef MINI_RPC_UNIX
#include <sys/types.h>
#elif defined(MINI_RPC_WIN)
#endif

namespace mini_rpc {

class RpcClient {
public:
    explicit RpcClient(std::string _endpoint);

    template<typename... Args>
    Result send_raw(std::string method, Args&&... args) {
        buffer message;

        encode_u16(message, method.size());
        encode_bytes(message, method.data(), method.size());
        encode_args(message, std::forward<Args>(args)...);

        framer->send_message(message);

        auto response = framer->recv_message();
        if (!response) {
            return Result{Error::ConnectionLost};
        }

        auto& bytes = *response;
        auto status = Error(static_cast<uint8_t>(bytes[0]));

        if (status != Error::None) {
            return Result{status};
        }

        buffer payload(bytes.begin() + 1, bytes.end());
        return Result{std::move(payload)};
    }

protected:
    std::unique_ptr<Framer> framer;
};
} // namespace mini_rpc