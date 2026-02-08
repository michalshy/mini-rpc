#pragma once

#include "frame.h"
#include "transport.h"
#include <memory>
#include <print>
#include <sys/types.h>
#include <utility>
#include <string>

#include "coder.h"

namespace mini_rpc {

    class RpcClient
    {
    public:
        explicit RpcClient(std::string _endpoint);

        template<typename... Args>
        void send_raw(std::string method, Args&&... args) {
            buffer message;

            encode_u16(message, method.size());
            encode_bytes(message, method.data(), method.size());
            encode_args(message, std::forward<Args>(args)...);

            framer->send_message(message);

            auto result = framer->recv_message();

            if(result) {
                std::println("received result!");
            } else {
                std::println("did not received :(!");
            }
        }
    protected:
        std::unique_ptr<Framer> framer;
    };
}