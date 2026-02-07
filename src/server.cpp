#include "server.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <errno.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <print>

namespace mini_rpc {

    namespace session
    {
        constexpr uint MAX_CONNECTIONS = 5;
        constexpr uint BUFF_SIZE = 1024; 
    }

    Server::Server(std::string _endpoint)
        : endpoint(_endpoint)
    {

    }

    void Server::run() {
        struct sockaddr_un addr;
        int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
        std::println("Server socket fd = {}\n", sfd);

        if (sfd == -1) {
            throw std::runtime_error("Socket error");
        }

        if (endpoint.length() > sizeof(addr.sun_path) - 1) {
            throw std::runtime_error(
                std::format("Server socket path is too long: {}", endpoint)
            );
        }

        if (remove(endpoint.c_str()) == -1 && errno != ENOENT) {
            throw std::runtime_error(
                std::format("Remove: {}", endpoint)
            );
        }

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);

        if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
            throw std::runtime_error("Binding error");
        }

        if (listen(sfd, session::MAX_CONNECTIONS) == -1) {
            throw std::runtime_error("Listening error");
        }

        ssize_t num_read;
        char buf[session::BUFF_SIZE];

        for(;;) {
            std::println("Waiting to accept a connection...");
            int cfd = accept(sfd, nullptr, nullptr);
            std::println("Accepted socket fd = {}", cfd);

            while((num_read = read(cfd, buf, session::BUFF_SIZE)) > 0) {
                if (write(STDOUT_FILENO, buf, num_read) != num_read) {
                    throw std::runtime_error("Failed write");
                }
            }

            if (num_read == -1) {
                throw std::runtime_error("Read");
            }

            if (close(cfd) == -1) {
                throw std::runtime_error("Close");
            }
        }
    }

    constexpr void Server::stop() {

    }

    Server::~Server()
    {

    }
}