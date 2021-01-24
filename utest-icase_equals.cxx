#include <catch2/catch.hpp>
#include "loch/icase_equals.h"

TEST_CASE("icase_equals")
{
    using namespace std::string_literals;

    REQUIRE(icase_equals("", ""));
    REQUIRE(icase_equals("hello", "hello"));
    REQUIRE(icase_equals("hello", "HELLO"s));
    REQUIRE(icase_equals("hElLo"s, "HELLO"));
    REQUIRE(icase_equals("HeLlo"s, "hello"s));

    REQUIRE_FALSE(icase_equals("hello", "world"));
    REQUIRE_FALSE(icase_equals("hello", "world!"));
    REQUIRE_FALSE(icase_equals("hello"s, "WORLD"s));
}
