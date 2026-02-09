#include "rpc_client.h"

#include "os/unix.h"

namespace mini_rpc {
RpcClient::RpcClient(std::string _endpoint) {
#ifdef __linux__
    auto transport = std::make_unique<UnixSocket>(_endpoint);
    transport->connect();
    framer = std::make_unique<Framer>(std::move(transport));
#else
    // win impl coming
#endif
}
} // namespace mini_rpc