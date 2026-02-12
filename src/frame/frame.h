#pragma once

#include "common.h"
#include "transport.h"
#include <cstddef>
#include <memory>
#include <optional>


namespace mini_rpc {

class Framer {
public:
    explicit Framer(std::unique_ptr<ITransport> _transport);

    void send_message(const buffer& message);
    std::optional<buffer> recv_message();

protected:
    std::unique_ptr<ITransport> transport;

    void write_all(const std::byte* data, size_t size);
    void read_all(std::byte* data, size_t size);
};
} // namespace mini_rpc
