#include "win_server.h"

#ifdef MINI_RPC_WIN

namespace mini_rpc {

WindowsServerSocket::WindowsServerSocket(std::string _endpoint) {}
WindowsServerSocket::~WindowsServerSocket() {}

void WindowsServerSocket::bind() {}
void WindowsServerSocket::listen(int backlog) {}
std::unique_ptr<ITransport> WindowsServerSocket::accept() {
    return nullptr;
}
void WindowsServerSocket::close() {}
} // namespace mini_rpc

#endif