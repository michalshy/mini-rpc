#include "server.h"

#include <string>

int main() {
    mini_rpc::Server s("/tmp/rpc.sock");

    s.register_handler("add", [](int a, int b) {
        int result = a + b;
        return result;
    });

    s.run();
}