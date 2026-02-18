#include "rpc_server.h"

#include "coder.h"
#include "error.h"

#include <cstdint>
#include <string>

namespace mini_rpc {

buffer RpcServer::handle_message(const buffer& message) {
    int offset = 0;

    uint16_t method_size = decode_u16(message.data());
    offset += sizeof(method_size);

    std::string_view method_name(reinterpret_cast<const char*>(message.data() + offset), method_size);
    offset += method_size;

    buffer args_buf(message.begin() + offset, message.end());
    buffer response;

    auto iter = handles.find(method_name);
    if (iter == handles.end()) {
        encode_u8(response, static_cast<uint8_t>(Error::MethodNotFound));
        return response;
    }

    try {
        encode_u8(response, static_cast<uint8_t>(Error::None));
        buffer payload = iter->second.invoke(args_buf);
        response.insert(response.end(), payload.begin(), payload.end());
    } catch (...) {
        response.clear();
        encode_u8(response, static_cast<uint8_t>(Error::HandlerError));
    }

    return response;
}
} // namespace mini_rpc