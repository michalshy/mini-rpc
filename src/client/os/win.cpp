#include "win.h"

#ifdef MINI_RPC_WIN

namespace mini_rpc {

WindowsSocket::WindowsSocket(std::string _endpoint) {}
WindowsSocket::~WindowsSocket() {}

void WindowsSocket::connect() {}
size_t WindowsSocket::send(const std::byte* data, size_t size) {
    return 0;
}
size_t WindowsSocket::recv(std::byte* data, size_t size) {
    return 0;
}
void WindowsSocket::close() {}
} // namespace mini_rpc

#endif