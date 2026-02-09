# mini-rpc

A lightweight RPC framework written in modern C++23. Register a function on the server, call it from the client — that's it.

## Quick Example

```cpp
#include "server.h"
#include "client.h"
#include <thread>
#include <print>

int main() {
    constexpr auto endpoint = "/tmp/mini_rpc_showcase.sock";

    // --- Server setup ---
    mini_rpc::Server server(endpoint);

    server.register_handler("add", [](int a, int b) -> int {
        return a + b;
    });

    server.register_handler("multiply", [](double a, double b) -> double {
        return a * b;
    });

    // Handlers can throw — the client receives Error::HandlerError
    server.register_handler("divide", [](int a, int b) -> int {
        if (b == 0) throw std::runtime_error("division by zero");
        return a / b;
    });

    server.register_handler("ping", []() {
        std::println("[server] got pinged");
    });

    std::thread server_thread([&] { server.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // --- Client usage ---
    mini_rpc::Client client(endpoint);

    // Basic call with return value
    auto sum = client.call("add", 10, 32);
    std::println("add(10, 32) = {}", sum.as<int>());

    // Different types
    auto product = client.call("multiply", 3.14, 2.0);
    std::println("multiply(3.14, 2.0) = {}", product.as<double>());

    // Error handling — handler throws
    auto bad_div = client.call("divide", 1, 0);
    if (!bad_div.ok()) {
        std::println("divide(1, 0) failed: HandlerError ({})",
                     static_cast<int>(bad_div.error()));
    }

    // Error handling — method doesn't exist
    auto missing = client.call("nonexistent", 42);
    if (!missing.ok()) {
        std::println("nonexistent method: MethodNotFound ({})",
                     static_cast<int>(missing.error()));
    }

    // Safe extraction with try_as (never throws)
    auto result = client.call("add", 5, 5);
    if (auto val = result.try_as<int>()) {
        std::println("add(5, 5) = {}", *val);
    }

    // Void call
    client.call("ping");

    server.stop();
    server_thread.join();
}
```

**Output:**
```
add(10, 32) = 42
multiply(3.14, 2.0) = 6.28
divide(1, 0) failed: HandlerError (3)
nonexistent method: MethodNotFound (1)
add(5, 5) = 10
[server] got pinged
```

## Features

- **Type-safe RPC** — register any callable, arguments are serialized automatically
- **Result type** — `as<T>()` throws on error, `try_as<T>()` returns `std::optional` (never throws)
- **Error propagation** — handler exceptions become `Error::HandlerError` on the client
- **Zero dependencies** — only the C++23 standard library (Catch2 for tests only)
- **Unix socket transport** — local IPC via `/tmp/*.sock`

## Error Codes

| Error | Value | Meaning |
|-------|-------|---------|
| `None` | 0 | Success |
| `MethodNotFound` | 1 | Server has no handler for this method |
| `InvalidArguments` | 2 | Argument decoding failed |
| `HandlerError` | 3 | Handler threw an exception |
| `Internal` | 4 | Internal library error |
| `ConnectionLost` | 5 | Socket disconnected |
| `InvalidPayload` | 6 | `as<T>()` size mismatch |

## Result API

```cpp
auto result = client.call("method", args...);

result.ok()          // true if call succeeded
result.error()       // the Error enum value (only valid if !ok())
result.as<int>()     // extract value — throws on error or type mismatch
result.try_as<int>() // extract value — returns std::nullopt on failure
```

`as<T>()` and `try_as<T>()` mirror `std::get` and `std::get_if` from `std::variant`.

## Building

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

## Running Tests

```bash
cd build
ctest
```

## Requirements

- C++23 compiler (GCC 13+ or Clang 18+)
- CMake 3.14+

## Wire Protocol

```
Request:  [method_name_size:u16][method_name:bytes][args:bytes]
Response: [error:u8][payload:bytes]

Framed as: [message_size:u32][message:bytes]
```
