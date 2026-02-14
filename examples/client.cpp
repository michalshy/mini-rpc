#include "client.h"

#include <print>
#include <string>

#ifdef MINI_RPC_WIN
std::string endpoint = "127.0.0.1:5432";
#else
std::string endpoint = "/tmp/rpc.sock";
#endif

int main() {
    mini_rpc::Client c(endpoint);

    mini_rpc::Result res = c.call("add", 10, 20);
    std::println("{}", res.as<int>());
}