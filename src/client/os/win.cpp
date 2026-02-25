#include "win.h"

#ifdef MINI_RPC_WIN

#include <WinSock2.h>
#include <stdexcept>

namespace mini_rpc {

WindowsSocket::WindowsSocket(std::string _endpoint) : endpoint(_endpoint) {}
WindowsSocket::WindowsSocket(SOCKET _sock) : sock(_sock) {}
WindowsSocket::~WindowsSocket() {
    close();
}

ConnectionResult WindowsSocket::connect() {
    if (sock != INVALID_SOCKET) {
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("socket() failed");
    }

    auto colon = endpoint.rfind(':');
    std::string host = endpoint.substr(0, colon);
    std::string port = endpoint.substr(colon + 1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(std::stoi(port)));
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        ::closesocket(sock);
        throw std::runtime_error("connect() failed");
    }
}

TransferResult WindowsSocket::send(const std::byte* data, size_t size) {
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("send on closed socket");
    }

    int n = ::send(sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0);
    if (n < 0) {
        throw std::runtime_error("write() failed");
    }

    return static_cast<size_t>(n);
}

TransferResult WindowsSocket::recv(std::byte* data, size_t size) {
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("recv on closed socket");
    }

    int n = ::recv(sock, reinterpret_cast<char*>(data), static_cast<int>(size), 0);
    if (n < 0) {
        throw std::runtime_error("read() failed");
    }

    return static_cast<std::size_t>(n);
}
ConnectionResult WindowsSocket::close() {
    if (sock != INVALID_SOCKET) {
        ::closesocket(sock);
    }
}
} // namespace mini_rpc

#endif