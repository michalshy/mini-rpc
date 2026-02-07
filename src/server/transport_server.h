#pragma once

#include "client/transport.h"
#include <memory>

namespace mini_rpc {

    class IServerTransport
    {
    public:
        virtual ~IServerTransport() = default;

        virtual void bind() = 0;
        virtual void listen(int clients) = 0;
        virtual std::unique_ptr<ITransport> accept() = 0;
        virtual void close() = 0;
    };
}