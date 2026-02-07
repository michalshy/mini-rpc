#pragma once

#include "frame.h"
#include "transport.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <string>

namespace mini_rpc {

    class RpcClient
    {
    public:
        explicit RpcClient(std::string _endpoint)
            : framer(std::make_unique<Framer>(_endpoint))
        {}

        template<typename... Args>
        void send_raw(std::string method, Args&&... args) {
            // message has to be prepared and passed to framer
            
            buffer message;

            encode_bytes(message, method.data(), method.size());
            encode_args(message, std::forward<Args>(args)...);

            framer->send_message(message);
        }
    protected:
        std::unique_ptr<Framer> framer;

        inline void encode_u16(buffer& buf, uint16_t v) {
            auto p = reinterpret_cast<const std::byte*>(&v);
            buf.insert(buf.end(), p, p + sizeof(v));
        }

        inline void encode_u32(buffer& buf, uint32_t v) {
            auto p = reinterpret_cast<const std::byte*>(&v);
            buf.insert(buf.end(), p, p + sizeof(v));
        }

        inline void encode_bytes(buffer& buf, const void* data, size_t size) {
            auto p = reinterpret_cast<const std::byte*>(data);
            buf.insert(buf.end(), p, p + size);
        }

        template<typename... Args>
        void encode_args(buffer& buf, Args&&... args) {
            (encode_arg(buf, std::forward<Args>(args)), ...);
        }
        
        template<typename T>
        requires std::is_trivially_copyable_v<T>
        void encode_arg(buffer& buf, const T& value) {
            auto p = reinterpret_cast<const std::byte*>(&value);
            buf.insert(buf.end(), p, p + sizeof(T));
        }
    };
}