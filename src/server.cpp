#include "server.h"

#include "coder.h"
#include "frame.h"
#include "result.h"
#include "server/os/unix_server.h"
#include "server/rpc_server.h"

#include <cstdint>
#include <memory>

#ifdef MINI_RPC_UNIX
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#elif defined(MINI_RPC_WIN)

#endif

namespace mini_rpc {

namespace session {
constexpr uint8_t MAX_CONNECTIONS = 5;
}

Server::Server(std::string _endpoint) : rpc(std::make_unique<RpcServer>()) {
#ifdef MINI_RPC_UNIX
    server_transport = std::make_unique<UnixServerSocket>(_endpoint);
#elif defined(MINI_RPC_WIN)
    // Win impl coming
#endif
}

void Server::run() {
    server_transport->bind();
    server_transport->listen(session::MAX_CONNECTIONS);

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