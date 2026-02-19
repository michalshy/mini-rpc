#include "server.h"

#include "client.h"
#include "server/os/unix_server.h"
#include "server/os/win_server.h"
#include "server/rpc_server.h"

#include "platform/platform.h"
#include "worker_pool.h"

#include <memory>
#include <print>
#include <utility>

namespace mini_rpc {

Server::Server(std::string _endpoint) : rpc(std::make_unique<RpcServer>()), endpoint(_endpoint) {
    platform_init();
#ifdef MINI_RPC_UNIX
    server_transport = std::make_unique<UnixServerSocket>(_endpoint);
#elif defined(MINI_RPC_WIN)
    server_transport = std::make_unique<WindowsServerSocket>(_endpoint);
#endif
    pool = std::make_unique<WorkerPool>(*rpc);
}

Server::~Server() {}

void Server::run() {
    server_transport->bind();
    server_transport->listen();

    while (true) {
        auto transport = server_transport->accept();

        if (!transport)
            break;

        if (stopped) {
            break;
        }

        pool->push_connection(std::move(transport));
    }
}

void Server::stop() {
    pool->close();
    stopped = true;
    Client dummy(endpoint);
    server_transport->close();
}
} // namespace mini_rpc