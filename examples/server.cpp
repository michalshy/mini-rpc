#include "server.h"

int main() {
    mini_rpc::server s("/tmp/rpc.sock");
    s.run();
}