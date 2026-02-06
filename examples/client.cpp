#include "client.h"

int main() {
    mini_rpc::client c("/tmp/rpc.sock");
    c.debug_connect();
}