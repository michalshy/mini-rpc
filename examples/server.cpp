#include "server.h"
#include <print>
#include <string>

int main() {
    mini_rpc::Server s("/tmp/rpc.sock");
    
    s.register_handler("add", [](int a, int b){
        int result = a + b;
        std::println("{}", result);
        return result;
    });

    s.run();
}