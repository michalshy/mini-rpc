#pragma once

#include "server/rpc_server.h"
#include "server/transport_server.h"

#include <atomic>
#include <memory>
#include <string>
#include <utility>

namespace mini_rpc {
class Server {
public:
    explicit Server(std::string _endpoint);
    ~Server();

    /// Register a handler for the given method name.
    /// @param func ANy callable with trivally copyable args and return type (or void)
    /// If func throws, the client receives Error::HandlerError
    template<typename Func>
    void register_handler(std::string method, Func&& func) {
        rpc->register_handler(std::move(method), std::forward<Func>(func));
    }

    /// Start accepting connection. Blocks thread
    void run();
    /// Signal server to stop
    void stop();

protected:
    std::unique_ptr<IServerTransport> server_transport;
    std::unique_ptr<RpcServer> rpc;

    std::atomic_bool stopped;
};
} // namespace mini_rpc