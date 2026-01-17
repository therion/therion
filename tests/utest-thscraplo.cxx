#include "thscraplo.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thscraplo")
{
    thscraplo obj;
    REQUIRE(obj.lfreefirst);
    REQUIRE(obj.lfreelast);
    REQUIRE(obj.next_line == nullptr);
    REQUIRE(obj.next_outline == nullptr);
    REQUIRE(obj.next_scrap_line == nullptr);
    REQUIRE(obj.line == nullptr);
    REQUIRE(obj.mode == TT_OUTLINE_NO);
}
