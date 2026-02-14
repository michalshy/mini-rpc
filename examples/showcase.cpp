#include "client.h"
#include "server.h"

#include <print>
#include <thread>

#ifdef MINI_RPC_WIN
constexpr auto endpoint = "127.0.0.1:5432";
#else
constexpr auto endpoint = "/tmp/rpc.sock";
#endif

int main() {
    mini_rpc::Server server(endpoint);

    // Arithmetic handlers
    server.register_handler("add", [](int a, int b) -> int { return a + b; });

    server.register_handler("multiply", [](double a, double b) -> double { return a * b; });

    // Handler that can fail
    server.register_handler("divide", [](int a, int b) -> int {
        if (b == 0)
            throw std::runtime_error("division by zero");
        return a / b;
    });

    server.register_handler("ping", []() { std::println("[server] got pinged"); });

    std::thread server_thread([&] { server.run(); });

    // Give server time to bind
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    mini_rpc::Client client(endpoint);

    auto sum = client.call("add", 10, 32);
    std::println("add(10, 32) = {}", sum.as<int>());

    auto product = client.call("multiply", 3.14, 2.0);
    std::println("multiply(3.14, 2.0) = {}", product.as<double>());

    auto bad_div = client.call("divide", 1, 0);
    if (!bad_div.ok()) {
        std::println("divide(1, 0) failed: error code {}", static_cast<int>(bad_div.error()));
    }

    auto missing = client.call("nonexistent", 42);
    if (!missing.ok()) {
        std::println("nonexistent method: error code {}", static_cast<int>(missing.error()));
    }

    auto result = client.call("add", 5, 5);
    if (auto val = result.try_as<int>()) {
        std::println("add(5, 5) = {}", *val);
    }

    client.call("ping");

    server.stop();
    server_thread.join();
}
