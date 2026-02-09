#include "client.h"
#include "server.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("client-server round-trip") {
    using namespace mini_rpc;

    Server server("/tmp/test_rpc.sock");
    server.register_handler("multiply", [](int a, int b) -> int { return a * b; });

    std::thread t([&] { server.run(); });

    Client client("/tmp/test_rpc.sock");
    auto result = client.call("multiply", 6, 7);
    REQUIRE(result.ok());
    REQUIRE(result.as<int>() == 42);

    server.stop();
    t.join();
}