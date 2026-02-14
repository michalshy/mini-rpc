#include "rpc_client.h"

#include "os/unix.h"
#include "os/win.h"

#include "platform/platform.h"

namespace mini_rpc {
RpcClient::RpcClient(std::string _endpoint) {

    platform_init();

#ifdef MINI_RPC_UNIX
    auto transport = std::make_unique<UnixSocket>(_endpoint);
#elif defined(MINI_RPC_WIN)
    auto transport = std::make_unique<WindowsSocket>(_endpoint);
#endif
    transport->connect();
    framer = std::make_unique<Framer>(std::move(transport));
}
} // namespace mini_rpc