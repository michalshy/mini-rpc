#include "client.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
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
        constexpr uint BUFF_SIZE = 1024; 
    }

    client::client(std::string _endpoint)
        : endpoint(_endpoint)
    {

    }

    void client::debug_connect()
    {
        struct sockaddr_un addr;
        ssize_t num_read;
        char buf[session::BUFF_SIZE];
        
        int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
        std::println("Client socket fd = {}", sfd);    

        if (sfd == -1) {
            throw std::runtime_error("Socket error");
        }

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);

        if (connect(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1) {
            throw std::runtime_error("Connect");
        }

        while ((num_read = read(STDIN_FILENO, buf, session::BUFF_SIZE)) > 0) {
            if (write(sfd, buf, num_read) != num_read) {
                throw std::runtime_error("Write error");
            }
        }

        if (num_read == -1) {
            throw std::runtime_error("Read");
        }

        exit(EXIT_SUCCESS);
    }
}