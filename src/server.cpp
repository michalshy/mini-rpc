#include "server.h"
#include "frame.h"
#include "server/os/unix_server.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace mini_rpc {

    namespace session
    {
        constexpr uint MAX_CONNECTIONS = 5;
    }

    Server::Server(std::string _endpoint)
    {
        #ifdef __linux__
            server_transport = std::make_unique<UnixServerSocket>(_endpoint);
        #else
            server_transport = std::make_unique<UnixServerSocket>(_endpoint);
        #endif
    }

    void Server::run() {
        server_transport->bind();
        server_transport->listen(session::MAX_CONNECTIONS);

        while(!stopped) {
            auto transport = server_transport->accept();

            Framer framer(std::move(transport));
        }
    }

    constexpr void Server::stop() {
        stopped = true;
    }

    Server::~Server()
    {

    }
}