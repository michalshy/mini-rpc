#include "frame.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __linux__
    #include "os/unix.h"
#else
    #include "os/unix.h"
#endif

namespace mini_rpc {

    Framer::Framer(std::string endpoint)
    {
        // Come up with more sufisticated method to establish OS specific transport
        #ifdef __linux__
            transport = std::make_unique<UnixSocket>(std::move(endpoint));
        #else
            transport = std::make_unique<UnixSocket>(std::move(endpoint));
        #endif

        transport->connect();
    }

    void Framer::send_message(const std::vector<std::byte>& message)
    {
        uint size = message.size();

        write_all(message.data(), message.size());
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

    }
}