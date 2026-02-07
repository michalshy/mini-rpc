#pragma once

#include <memory>
#include <string>
#include <utility>
#include "rpc_client.h"

namespace mini_rpc {
    class Client {
    public:
        explicit Client(std::string _endpoint);

        template<typename... Args>
        void call(std::string method, Args&&... args) {
            rpc->send_raw(std::move(method), std::forward<Args>(args)...);
        }
    protected:

        std::unique_ptr<RpcClient> rpc;
    };
}