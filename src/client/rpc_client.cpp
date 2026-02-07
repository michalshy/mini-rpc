#include "rpc_client.h"

namespace mini_rpc {
    RpcClient::RpcClient(std::string _endpoint)
    {
        #ifdef __linux__
            auto transport = std::make_unique<UnixSocket>(_endpoint);
        #else
            auto transport = std::make_unique<UnixSocket>(_endpoint);
        #endif

        transport->connect();
        framer = std::make_unique<Framer>(std::move(transport));
    }
}