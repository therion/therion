#include "thobjectsrc.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thobjectsrc")
{
    SECTION("default constructor")
    {
        thobjectsrc thobjectsrc;
        REQUIRE(!thobjectsrc.is_valid());
        thobjectsrc.name = "name";
        REQUIRE(thobjectsrc.is_valid());
    }
    SECTION("parameterized constructor")
    {
        thobjectsrc thobjectsrc("name2", 13);
        REQUIRE(thobjectsrc.is_valid());
        REQUIRE(strcmp(thobjectsrc.name, "name2") == 0);
        REQUIRE(thobjectsrc.line == 13);
    }
}
