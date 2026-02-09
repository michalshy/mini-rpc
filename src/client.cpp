#include "client.h"

#include "rpc_client.h"

#include <cstdint>
#include <memory>

namespace mini_rpc {

namespace session {
constexpr uint16_t BUFF_SIZE = 1024;
}

Client::Client(std::string _endpoint) : rpc(std::make_unique<RpcClient>(_endpoint)) {}
} // namespace mini_rpc