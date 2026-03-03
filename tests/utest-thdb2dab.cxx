#include "thdb2dab.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thdb2dab")
{
    thdb2dab obj;
    REQUIRE(obj.line == nullptr);
    REQUIRE(obj.next_line == nullptr);
    REQUIRE(obj.prev_line == nullptr);
}
