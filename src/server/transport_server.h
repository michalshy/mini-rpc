#pragma once

#include "client/transport.h"

#include <memory>

namespace mini_rpc {

namespace session {
constexpr uint8_t MAX_CONNECTIONS = 5;
constexpr uint16_t BUFFER_SIZE = 1024;
} // namespace session

class IServerTransport {
public:
    virtual ~IServerTransport() = default;

    virtual void bind() = 0;
    virtual void listen() = 0;
    virtual std::unique_ptr<ITransport> accept() = 0;
    virtual void close() = 0;
};
} // namespace mini_rpc