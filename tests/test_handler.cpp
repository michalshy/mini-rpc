// test_handler.cpp
#include "coder.h"
#include "error.h"
#include "rpc_server.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>

TEST_CASE("handler invocation round-trip") {
    using namespace mini_rpc;

    RpcServer rpc;
    rpc.register_handler("add", [](int a, int b) -> int { return a + b; });

    buffer msg;
    std::string name = "add";
    encode_u16(msg, name.size());
    encode_bytes(msg, name.data(), name.size());
    encode_args(msg, 10, 20);

    buffer resp = rpc.handle_message(msg);
    REQUIRE(static_cast<uint8_t>(resp[0]) == 0); // Error::None
    int result;
    std::memcpy(&result, resp.data() + 1, sizeof(int));
    REQUIRE(result == 30);
}

TEST_CASE("unknown method error") {
    using namespace mini_rpc;

    RpcServer rpc;
    buffer msg;
    std::string name = "null";
    encode_u16(msg, name.size());
    encode_bytes(msg, name.data(), name.size());

    buffer resp = rpc.handle_message(msg);
    REQUIRE(resp[0] == static_cast<std::byte>(Error::MethodNotFound));
}

TEST_CASE("wrap_function does not execute function on creation") {
    bool called = false;

    auto handler = mini_rpc::wrap_function([&](int, int) {
        called = true;
        return 0;
    });

    REQUIRE(called == false);
}
