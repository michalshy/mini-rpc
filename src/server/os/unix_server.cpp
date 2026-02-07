#include "unix_server.h"
#include "os/unix.h"
#include "transport.h"
#include <cstring>
#include <memory>
#include <print>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace mini_rpc {
    UnixServerSocket::UnixServerSocket(std::string _endpoint)
        : endpoint(_endpoint)
    {
        
    }

    UnixServerSocket::~UnixServerSocket()
    {
        ::close(listen_fd);
    }

    void UnixServerSocket::bind()
    {
        listen_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (listen_fd == -1)
            throw std::runtime_error("socket() failed");

        ::unlink(endpoint.c_str());

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, endpoint.c_str(),
            sizeof(addr.sun_path) - 1);

        if (::bind(listen_fd,
                reinterpret_cast<sockaddr*>(&addr), 
                sizeof(addr)) == -1)
            throw std::runtime_error("bind() failed");
    }

    void UnixServerSocket::listen(int clients)
    {
        if (::listen(listen_fd, clients) == -1)
            throw std::runtime_error("listen() failed");
    }

    std::unique_ptr<ITransport> UnixServerSocket::accept()
    {
        int fd = ::accept(listen_fd, nullptr, nullptr);
        if (fd == -1)
            throw std::runtime_error("accept() failed");

        return std::make_unique<UnixSocket>(fd);
    }

    void UnixServerSocket::close()
    {
        if (listen_fd != -1)
        {
            ::close(listen_fd);
            listen_fd = -1;
        }
    }
}