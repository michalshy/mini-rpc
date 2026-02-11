#pragma once

#ifdef MINI_RPC_WIN

#include "transport.h"

#include <string>

namespace mini_rpc {

class WindowsSocket : public ITransport {
public:
    explicit WindowsSocket(std::string _endpoint);
    ~WindowsSocket() override;

    void connect() override;
    size_t send(const std::byte* data, size_t size) override;
    size_t recv(std::byte* data, size_t size) override;
    void close() override;
};
} // namespace mini_rpc

#endif