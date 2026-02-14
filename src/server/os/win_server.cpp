#include "win_server.h"
#include "client/os/win.h"
#include <print>
#include <stdexcept>
#include <winsock2.h>

#ifdef MINI_RPC_WIN
namespace mini_rpc {

WindowsServerSocket::WindowsServerSocket(std::string _endpoint) : endpoint(_endpoint) {}
WindowsServerSocket::~WindowsServerSocket() {
    close();
}

void WindowsServerSocket::bind() {
    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        throw std::runtime_error("bind() failed");

    auto colon = endpoint.rfind(':');
    std::string host = endpoint.substr(0, colon);
    std::string port = endpoint.substr(colon + 1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(std::stoi(port)));
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
        throw std::runtime_error("bind() failed");
}

void WindowsServerSocket::listen() {
    if (::listen(sock, session::MAX_CONNECTIONS) == SOCKET_ERROR)
        throw std::runtime_error("listen() failed");
}
std::unique_ptr<ITransport> WindowsServerSocket::accept() {
    SOCKET _sock = ::accept(sock, nullptr, nullptr);
    if (_sock == INVALID_SOCKET)
        return nullptr;

    return std::make_unique<WindowsSocket>(_sock);
}
void WindowsServerSocket::close() {
    if (sock != INVALID_SOCKET) {
        ::shutdown(sock, SD_BOTH);
        ::closesocket(sock);
        sock = INVALID_SOCKET;
    }
}
} // namespace mini_rpc

#endif
