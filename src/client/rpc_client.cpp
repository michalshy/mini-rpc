#include "rpc_client.h"

#include "os/unix.h"
#include "os/win.h"

namespace mini_rpc {
RpcClient::RpcClient(std::string _endpoint) {
#ifdef MINI_RPC_UNIX
    auto transport = std::make_unique<UnixSocket>(_endpoint);
    transport->connect();
#elif defined(MINI_RPC_WIN)
    auto transport = std::make_unique<WindowsSocket>(_endpoint);
#endif
    framer = std::make_unique<Framer>(std::move(transport));
}
} // namespace mini_rpc