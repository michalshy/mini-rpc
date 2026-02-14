#include "client.h"
#include "server.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>

#ifdef MINI_RPC_WIN
constexpr auto endpoint = "127.0.0.1:5432";
#else
constexpr auto endpoint = "/tmp/rpc.sock";
#endif

TEST_CASE("client-server round-trip") {
    using namespace mini_rpc;

    RpcServer rpc;
    rpc.register_handler("multiply", [](int a, int b) -> int { return a * b; });

    buffer request;
    std::string method = "multiply";
    encode_u16(request, method.size());
    encode_bytes(request, method.data(), method.size());
    encode_args(request, 6, 7);

    buffer response = rpc.handle_message(request);

    auto status = Error(static_cast<uint8_t>(response[0]));
    REQUIRE(status == Error::None);

    buffer payload(response.begin() + 1, response.end());
    Result result{std::move(payload)};
    REQUIRE(result.as<int>() == 42);
}