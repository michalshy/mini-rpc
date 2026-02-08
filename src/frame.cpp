#include "frame.h"
#include "transport.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <print>
#include <stdexcept>

namespace mini_rpc {

    Framer::Framer(std::unique_ptr<ITransport> _transport)
    : transport(std::move(_transport)) {}


    void Framer::send_message(const buffer& message)
    {
        uint size = message.size();
        write_all(reinterpret_cast<const std::byte*>(&size), sizeof(size));
        write_all(message.data(), message.size());
    }

    std::optional<buffer> Framer::recv_message()
    {
        uint32_t size = 0;

        try {
            read_all(reinterpret_cast<std::byte*>(&size), sizeof(size));
        } catch (...) {
            return std::nullopt; // clean EOF before header
        }

        buffer msg(size);
        read_all(msg.data(), size);

        return msg;
    }

    void Framer::write_all(const std::byte* data, size_t size) {
        uint send_bytes = 0;
        while(send_bytes < size) {
            size_t n = transport->send(data + send_bytes, size - send_bytes);
            if (n == 0) {
                throw std::runtime_error("connection closed during writing");
            }
            send_bytes += n;
        }
    }

    void Framer::read_all(std::byte* data, size_t size) {
        size_t read_bytes = 0;
        while (read_bytes < size)
        {
            size_t n = transport->recv(data + read_bytes, size - read_bytes);
            if (n == 0)
                throw std::runtime_error("connection closed during read_all");

            read_bytes += n;
        }
    }
}