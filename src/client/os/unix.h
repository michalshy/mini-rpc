#pragma once

#include "common.h"
#ifdef MINI_RPC_UNIX

#include "client/transport.h"

#include <cstddef>
#include <string>

namespace mini_rpc {
class UnixSocket : public ITransport {
public:
    explicit UnixSocket(std::string _endpoint);
    explicit UnixSocket(int fd);
    ~UnixSocket() override;

    ConnectionResult connect() override;
    TransferResult send(const std::byte* data, size_t size) override;
    TransferResult recv(std::byte* data, size_t size) override;
    ConnectionResult close() override;

protected:
    int fd{-1};
    std::string endpoint;
};
} // namespace mini_rpc

#endif