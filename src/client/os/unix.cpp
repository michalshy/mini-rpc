#include "unix.h"
#include <expected>

#ifdef MINI_RPC_UNIX

#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

namespace mini_rpc {

UnixSocket::UnixSocket(std::string _endpoint) : endpoint(std::move(_endpoint)) {}

UnixSocket::UnixSocket(int _fd) : fd(_fd) {}

UnixSocket::~UnixSocket() {
    [[maybe_unused]] ConnectionResult res = close();
    // later handle
}

ConnectionResult UnixSocket::connect() {
    if (fd != -1) {
        return {};
    }

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        return std::unexpected(E::SOCKET_ERROR);
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, endpoint.c_str(), sizeof(addr.sun_path) - 1);

    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_un)) == -1) {
        ::close(fd);
        fd = -1;
        return std::unexpected(E::CONNECT_ERROR);
    }

    return {};
}

TransferResult UnixSocket::send(const std::byte* data, size_t size) {
    if (fd == -1) {
        return std::unexpected(E::SEND_ERROR);
    }

    ssize_t n = ::write(fd, data, size);
    if (n < 0) {
        return std::unexpected(E::WRITE_ERROR);
    }

    return static_cast<size_t>(n);
}

TransferResult UnixSocket::recv(std::byte* data, size_t size) {
    if (fd == -1) {
        return std::unexpected(E::RECV_ERROR);
    }

    ssize_t n = ::read(fd, data, size);
    if (n < 0) {
        return std::unexpected(E::READ_ERROR);
    }

    return static_cast<std::size_t>(n);
}

ConnectionResult UnixSocket::close() {
    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
    return {};
}

} // namespace mini_rpc

#endif