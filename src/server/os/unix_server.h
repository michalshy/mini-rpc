#pragma once

#ifdef MINI_RPC_UNIX

#include "server/transport_server.h"

namespace mini_rpc {

class UnixServerSocket : public IServerTransport {
public:
    explicit UnixServerSocket(std::string _endpoint);
    ~UnixServerSocket() override;

    void bind() override;
    void listen(int backlog) override;
    std::unique_ptr<ITransport> accept() override;
    void close() override;

protected:
    int listen_fd{-1};
    std::string endpoint;
};

} // namespace mini_rpc
#endif