#include "win.h"

#ifdef MINI_RPC_WIN

#include "network_utils.h"
#include <WinSock2.h>
#include <stdexcept>

namespace mini_rpc {

WindowsSocket::WindowsSocket(std::string _endpoint) : endpoint(_endpoint) {}
WindowsSocket::WindowsSocket(SOCKET _sock) : sock(_sock) {}
WindowsSocket::~WindowsSocket() {
    close();
}

void WindowsSocket::connect() {
    if (sock != INVALID_SOCKET) {
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("socket() failed");
    }

    sockaddr_in addr = create_sockaddr_in(endpoint);

    if (::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        ::closesocket(sock);
        throw std::runtime_error("connect() failed");
    }
}

size_t WindowsSocket::send(const std::byte* data, size_t size) {
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("send on closed socket");
    }

    int n = ::send(sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0);
    if (n < 0) {
        throw std::runtime_error("write() failed");
    }

    return static_cast<size_t>(n);
}

size_t WindowsSocket::recv(std::byte* data, size_t size) {
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("recv on closed socket");
    }

    int n = ::recv(sock, reinterpret_cast<char*>(data), static_cast<int>(size), 0);
    if (n < 0) {
        throw std::runtime_error("read() failed");
    }

    return static_cast<std::size_t>(n);
}
void WindowsSocket::close() {
    if (sock != INVALID_SOCKET) {
        ::closesocket(sock);
        sock = INVALID_SOCKET;
    }
}
} // namespace mini_rpc

#endif