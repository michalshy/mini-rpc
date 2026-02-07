#include "client.h"

int main() {
    mini_rpc::client c("/tmp/rpc.sock");

    c.call("add", 10, 20);
}