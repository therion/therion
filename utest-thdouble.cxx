#include "thdouble.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thdouble")
{
    REQUIRE(fmt::format("{}", thdouble()) == "0");
    REQUIRE(fmt::format("{}", thdouble(10000000.0, 5)) == "10000000");
    REQUIRE(fmt::format("{}", thdouble(0.001, 5)) == "0.001");
    REQUIRE(fmt::format("{}", thdouble(0.001, 4)) == "0.001");
    REQUIRE(fmt::format("{}", thdouble(0.001, 3)) == "0.001");
    REQUIRE(fmt::format("{}", thdouble(0.001, 2)) == "0");
    REQUIRE(fmt::format("{}", thdouble(0.001, 1)) == "0");
    REQUIRE(fmt::format("{}", thdouble(0.0000000000000000001, 5)) == "0");
    REQUIRE(fmt::format("{}", thdouble(0.5555555, 3)) == "0.556");
    REQUIRE(fmt::format("{}", thdouble(0.10203, 10)) == "0.10203");
    REQUIRE(fmt::format("{}", thdouble(0.102030, 10)) == "0.10203");
    REQUIRE(fmt::format("{}", 0.55555_thd) == "0.56");
}
