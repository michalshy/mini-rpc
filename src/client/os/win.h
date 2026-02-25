#pragma once

#ifdef MINI_RPC_WIN

#include "common.h"
#include "transport.h"

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace mini_rpc {

class WindowsSocket : public ITransport {
public:
    explicit WindowsSocket(std::string _endpoint);
    explicit WindowsSocket(SOCKET _sock);
    ~WindowsSocket() override;

    ConnectionResult connect() override;
    TransferResult send(const std::byte* data, size_t size) override;
    TransferResult recv(std::byte* data, size_t size) override;
    ConnectionResult close() override;

protected:
    SOCKET sock{INVALID_SOCKET};
    std::string endpoint;
};
} // namespace mini_rpc

#endif