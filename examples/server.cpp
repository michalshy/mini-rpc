#include "server.h"

int main() {
    mini_rpc::Server s("/tmp/rpc.sock");
    
    s.register_handler("add", [](int a, int b){
        return a + b;
    });

    s.run();
}