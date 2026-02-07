#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "transport.h"


namespace mini_rpc {

    class Framer
    {
    public:
        explicit Framer(std::string endpoint);

        void send_message(const std::vector<std::byte>& message);

    protected:
        std::unique_ptr<ITransport> transport;

        void write_all(const std::byte* data, size_t size);
        void read_all(std::byte* data, size_t size);
    };
}