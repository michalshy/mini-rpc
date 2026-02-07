#include "unix.h"
#include <cstddef>

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace mini_rpc {

    UnixSocket::UnixSocket(std::string _endpoint)
        : endpoint(std::move(_endpoint))
    {
        
    }

    UnixSocket::~UnixSocket()
    {
        close();
    }


    void UnixSocket::connect()
    {
        if (fd != -1) return;

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd == -1) {
            throw std::runtime_error("socket() failed");
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, endpoint.c_str(),
            sizeof(addr.sun_path) - 1);

        if (::connect(fd, 
            reinterpret_cast<sockaddr*>(&addr),
            sizeof(sockaddr_un)) == -1) {
                ::close(fd);
                fd = -1;
                throw std::runtime_error("connect() failed");
            }
    }

    size_t UnixSocket::send(const std::byte* data, size_t size)
    {
        if (fd == -1) {
            throw std::runtime_error("send on closed socket");
        }

        ssize_t n = ::write(fd, data, size);
        if (n < 0) {
            throw std::runtime_error("write() failed");
        }

        return static_cast<size_t>(n);
    }

    size_t UnixSocket::recv(std::byte* data, size_t size) 
    {
        if (fd == -1) {
            throw std::runtime_error("recv on closed socket");
        }
        
        ssize_t n = ::read(fd, data, size);
        if (n < 0) { 
            throw std::runtime_error("read() failed");
        }

        return static_cast<std::size_t>(n);
    }

    void UnixSocket::close()
    {
        if (fd != -1) {
            ::close(fd);
            fd = -1;
        }
    }

    
}