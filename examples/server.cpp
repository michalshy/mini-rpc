#include "server.h"

#include <string>

#ifdef MINI_RPC_WIN
constexpr auto endpoint = "localhost:5432";
#else
constexpr auto endpoint = "/tmp/rpc.sock";
#endif

int main() {
    mini_rpc::Server s(endpoint);

    s.register_handler("add", [](int a, int b) {
        int result = a + b;
        return result;
    });

    s.run();
}