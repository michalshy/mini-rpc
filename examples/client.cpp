#include "client.h"

int main() {
    mini_rpc::Client c("/tmp/rpc.sock");

    c.call("add", 10, 20);
    //c.call("echo", "hej");
}