#pragma once

#include "result.h"
#include "rpc_client.h"

#include <memory>
#include <string>
#include <utility>

namespace mini_rpc {
class Client {
public:
    explicit Client(std::string _endpoint);
    ~Client() = default;

    template<typename... Args>
    Result call(std::string method, Args&&... args) {
        return rpc->send_raw(std::move(method), std::forward<Args>(args)...);
    }

protected:
    std::unique_ptr<RpcClient> rpc;
};
} // namespace mini_rpc