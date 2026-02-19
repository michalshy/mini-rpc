#include "client.h"
#include "server.h"
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

#ifdef MINI_RPC_WIN
constexpr auto endpoint = "127.0.0.1:5432";
#else
constexpr auto endpoint = "/tmp/rpc.sock";
#endif

TEST_CASE("two clients can connect and call handlers concurrently") {
    mini_rpc::Server server(endpoint);
    server.register_handler("add", [](int a, int b) -> int { return a + b; });

    std::thread server_thread([&] { server.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int result1 = -1;
    int result2 = -1;

    std::thread client1_thread([&] {
        mini_rpc::Client client(endpoint);
        result1 = client.call("add", 1, 2).as<int>();
    });

    std::thread client2_thread([&] {
        mini_rpc::Client client(endpoint);
        result2 = client.call("add", 3, 4).as<int>();
    });

    client1_thread.join();
    client2_thread.join();

    REQUIRE(result1 == 3);
    REQUIRE(result2 == 7);

    server.stop();
    server_thread.join();
}