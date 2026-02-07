#pragma once

#include "client/transport.h"

#include <cstddef>
#include <string>

namespace mini_rpc {
    class UnixSocket : public ITransport {
    public:
        explicit UnixSocket(std::string _endpoint);
        ~UnixSocket() override;
        
        void connect() override;
        size_t send(const std::byte* data, size_t size) override;
        size_t recv(std::byte* data, size_t size) override;
        void close() override;
    protected:
        int fd{-1};
        std::string endpoint;
    };
}