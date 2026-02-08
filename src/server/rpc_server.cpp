#include "rpc_server.h"
#include <string>
#include "coder.h"

namespace mini_rpc {

    Result RpcServer::handle_message(const buffer& message) 
    {
        int offset = 0;

        uint16_t method_size = decode_u16(message.data());
        offset += sizeof(method_size);
        
        buffer method_buffer = decode_bytes(message.data() + offset, method_size);
        std::string method_name = reinterpret_cast<const char*>(method_buffer.data());
        offset += method_size;

        buffer args_buf(message.begin() + offset, message.end());
        buffer response;

        handles[method_name].invoke(args_buf, response);
        return 0;
    }
}