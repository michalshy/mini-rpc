#include "win_server.h"
#include <winsock2.h>

#ifdef MINI_RPC_WIN

namespace mini_rpc {

WindowsServerSocket::WindowsServerSocket(std::string _endpoint) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = ::socket(AF_INET, SOCK_STREAM, 0);
}
WindowsServerSocket::~WindowsServerSocket() {
    //
}

void WindowsServerSocket::bind() {}
void WindowsServerSocket::listen() {}
std::unique_ptr<ITransport> WindowsServerSocket::accept() {
    return nullptr;
}
void WindowsServerSocket::close() {}
} // namespace mini_rpc

#endif
