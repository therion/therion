#include "thobjectid.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <cstring>

TEST_CASE("thobjectid")
{
    SECTION("default constructor")
    {
        thobjectid obj;
        REQUIRE(strcmp(obj.name, "") == 0);
        REQUIRE(obj.sid == 0);
    }
    SECTION("parameterized constructor")
    {
        thobjectid obj("name", 1);
        REQUIRE(strcmp(obj.name, "name") == 0);
        REQUIRE(obj.sid == 1);
    }
    SECTION("operator<()")
    {
        thobjectid obj1("name", 1);
        thobjectid obj2("name", 1);
        REQUIRE_FALSE(obj1 < obj2);
        REQUIRE_FALSE(obj2 < obj1);

        thobjectid obj3("name", 2);
        REQUIRE(obj1 < obj3);
        REQUIRE_FALSE(obj3 < obj1);

        thobjectid obj4("a_name", 1);
        REQUIRE(obj4 < obj1);
        REQUIRE_FALSE(obj1 < obj4);
    }
}
