// test_handler.cpp
#include <catch2/catch_test_macros.hpp>
#include "server.h"


TEST_CASE("wrap_function does not execute function on creation") {
    bool called = false;

    auto handler = mini_rpc::wrap_function([&](int, int) {
        called = true;
        return 0;
    });

    REQUIRE(called == false);
}
