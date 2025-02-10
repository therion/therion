#include "thscrapen.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thscrapen")
{
    thscrapen obj;
    REQUIRE(obj.next_end == nullptr);
    REQUIRE(obj.l1 == nullptr);
    REQUIRE(obj.lp1 == nullptr);
    REQUIRE(obj.l2 == nullptr);
    REQUIRE(obj.lp2 == nullptr);
    REQUIRE(obj.cxt == 0.0);
    REQUIRE(obj.cyt == 0.0);
    REQUIRE_FALSE(obj.active);
}
