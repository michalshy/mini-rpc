#include "coder.h"
#include "error.h"
#include "result.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <optional>

TEST_CASE("Result buffer") {
    using namespace mini_rpc;

    buffer buf;
    uint16_t val = 42;
    encode_u16(buf, val);
    Result res(std::move(buf));
    REQUIRE(res.ok());
    REQUIRE(res.as<uint16_t>() == 42);
}

TEST_CASE("Result error") {
    using namespace mini_rpc;

    Result r(Error::InvalidPayload);
    REQUIRE(!r.ok());
    REQUIRE(r.error() == Error::InvalidPayload);
    REQUIRE(r.try_as<int>() == std::nullopt);
}