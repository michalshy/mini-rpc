#include "rpc_client.h"

#include "os/unix.h"

namespace mini_rpc {
RpcClient::RpcClient(std::string _endpoint) {
#ifdef MINI_RPC_UNIX
    auto transport = std::make_unique<UnixSocket>(_endpoint);
    transport->connect();
    framer = std::make_unique<Framer>(std::move(transport));
#elif defined(MINI_RPC_WIN)
    // win impl coming
#endif
}
} // namespace mini_rpc