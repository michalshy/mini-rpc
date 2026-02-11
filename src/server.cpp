#include "server.h"

#include "coder.h"
#include "frame.h"
#include "result.h"
#include "server/os/unix_server.h"
#include "server/os/win_server.h"
#include "server/rpc_server.h"

#include <cstdint>
#include <memory>

namespace mini_rpc {

Server::Server(std::string _endpoint) : rpc(std::make_unique<RpcServer>()) {
#ifdef MINI_RPC_UNIX
    server_transport = std::make_unique<UnixServerSocket>(_endpoint);
#elif defined(MINI_RPC_WIN)
    server_transport = std::make_unique<WindowsServerSocket>(_endpoint);
#endif
}

void Server::run() {
    server_transport->bind();
    server_transport->listen();

    while (!stopped) {
        auto transport = server_transport->accept();
        Framer framer(std::move(transport));

        while (!stopped) {
            auto msg = framer.recv_message();
            if (!msg) {
                break; // client disconnected cleanly
            }

            buffer response = rpc->handle_message(*msg);
            framer.send_message(response);
        }
    }
}

void Server::stop() {
    stopped = true;
}

Server::~Server() {}
} // namespace mini_rpc