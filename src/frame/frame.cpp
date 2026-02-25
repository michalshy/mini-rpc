#include "frame.h"

#include "common.h"
#include "transport.h"

#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <print>
#include <stdexcept>

namespace mini_rpc {

Framer::Framer(std::unique_ptr<ITransport> _transport) : transport(std::move(_transport)) {}

void Framer::send_message(const buffer& message) {
    size_t size = message.size();
    [[maybe_unused]] TransferResult write_size_result =
        write_all(reinterpret_cast<const std::byte*>(&size), sizeof(size));
    [[maybe_unused]] TransferResult write_message_result =
        write_all(message.data(), message.size());
}

std::optional<buffer> Framer::recv_message() {
    size_t size = 0;

    TransferResult res_size = read_all(reinterpret_cast<std::byte*>(&size), sizeof(size));

    if (!res_size.has_value())
        return std::nullopt;

    buffer msg(size);
    TransferResult res_msg = read_all(msg.data(), size);
    if (!res_msg.has_value())
        return std::nullopt;

    return msg;
}

void Framer::close() {
    [[maybe_unused]] ConnectionResult res = transport->close();
    // handle later
}

TransferResult Framer::write_all(const std::byte* data, size_t size) {
    size_t send_bytes = 0;
    while (send_bytes < size) {
        TransferResult res = transport->send(data + send_bytes, size - send_bytes);
        if (!res.has_value())
            return std::unexpected(res.error());
        send_bytes += res.value();
    }
    return {send_bytes};
}

TransferResult Framer::read_all(std::byte* data, size_t size) {
    size_t read_bytes = 0;
    while (read_bytes < size) {
        TransferResult res = transport->recv(data + read_bytes, size - read_bytes);
        if (!res.has_value())
            return std::unexpected(res.error());

        if (res.value() == 0)
            return std::unexpected(E::READ_ERROR);

        read_bytes += res.value();
    }
    return {read_bytes};
}
} // namespace mini_rpc