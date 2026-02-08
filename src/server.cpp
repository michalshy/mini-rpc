#include "server.h"

#include "coder.h"
#include "frame.h"
#include "result.h"
#include "server/os/unix_server.h"
#include "server/rpc_server.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

namespace mini_rpc {

namespace session {
constexpr uint MAX_CONNECTIONS = 5;
}

Server::Server(std::string _endpoint) : rpc(std::make_unique<RpcServer>()) {
#ifdef __linux__
    server_transport = std::make_unique<UnixServerSocket>(_endpoint);
#else
    server_transport = std::make_unique<UnixServerSocket>(_endpoint);
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

constexpr void Server::stop() {
    stopped = true;
}

Server::~Server() {}
} // namespace mini_rpc