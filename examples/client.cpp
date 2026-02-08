#include "client.h"

#include <print>

int main() {
    mini_rpc::Client c("/tmp/rpc.sock");

    mini_rpc::Result res = c.call("add", 10, 20);
    std::println("{}", res.as<int>());
}