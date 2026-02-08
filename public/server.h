#pragma once

#include "server/rpc_server.h"
#include "server/transport_server.h"
#include <atomic>
#include <memory>
#include <string>
#include <map>
#include <utility>
#include <functional>
#include <vector>

namespace mini_rpc {

    using buffer = std::vector<std::byte>;

    class Server {
    public:
        explicit Server(std::string _endpoint);
        ~Server();

        template<typename Func>
        void register_handler(std::string method, Func&& func) {
            rpc->register_handler(std::move(method), std::forward<Func>(func));
        }
    
        void run(); // blocking
        constexpr void stop(); // async safe  
    protected:
        std::unique_ptr<IServerTransport> server_transport;
        std::unique_ptr<RpcServer> rpc;
        
        std::atomic_bool stopped;
    };
}