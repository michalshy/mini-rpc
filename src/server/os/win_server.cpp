#include "win_server.h"

#ifdef MINI_RPC_WIN

#include <windows.h>

#include <WinBase.h>

namespace mini_rpc {

WindowsServerSocket::WindowsServerSocket(std::string _endpoint) {
    HANDLE a = CreateNamedPipeA(_endpoint.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE,
                                session::MAX_CONNECTIONS, 1024, 1024, 0, NULL);
}
WindowsServerSocket::~WindowsServerSocket() {}

void WindowsServerSocket::bind() {}
void WindowsServerSocket::listen() {}
std::unique_ptr<ITransport> WindowsServerSocket::accept() {
    return nullptr;
}
void WindowsServerSocket::close() {}
} // namespace mini_rpc

#endif