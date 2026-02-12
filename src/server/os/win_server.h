#pragma once

#ifdef MINI_RPC_WIN

#include "transport_server.h"
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace mini_rpc {

class WindowsServerSocket : public IServerTransport {
public:
    explicit WindowsServerSocket(std::string _endpoint);
    ~WindowsServerSocket() override;

    void bind() override;
    void listen() override;
    std::unique_ptr<ITransport> accept() override;
    void close() override;

protected:
    SOCKET sock;
};
} // namespace mini_rpc

#endif
