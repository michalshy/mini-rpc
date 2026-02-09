#include "coder.h"

#include <catch2/catch_test_macros.hpp>
#include <tuple>

TEST_CASE("encode/decode u16 round-trip") {
    using namespace mini_rpc;

    buffer buf;
    encode_u16(buf, 1234);
    REQUIRE(decode_u16(buf.data()) == 1234);
}

TEST_CASE("encode/decode args round-trip") {
    using namespace mini_rpc;

    buffer buf;
    encode_args(buf, 1, 2, 3.0);
    int offset = 0;
    auto [a, b, c] = decode_args<std::tuple<int, int, double>>(buf.data(), offset);
    REQUIRE(a == 1);
    REQUIRE(b == 2);
    REQUIRE(c == 3.0);
}
