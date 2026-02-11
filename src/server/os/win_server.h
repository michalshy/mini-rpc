#pragma once

#ifdef MINI_RPC_WIN

#include "transport_server.h"

#include <string>

namespace mini_rpc {

class WindowsServerSocket : IServerTransport {
public:
    explicit WindowsServerSocket(std::string _endpoint);
    ~WindowsServerSocket() override;

    void bind() override;
    void listen(int backlog) override;
    std::unique_ptr<ITransport> accept() override;
    void close() override;
};
} // namespace mini_rpc

#endif
