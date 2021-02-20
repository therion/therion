#include <catch2/catch.hpp>
#include "loch/icase.h"
#include "thparse.h"
#include "thcsdata.h"

using namespace std::string_literals;

TEST_CASE("icase_equals")
{

    REQUIRE(icase_equals("", ""));
    REQUIRE(icase_equals("hello", "hello"));
    REQUIRE(icase_equals("hello", "HELLO"s));
    REQUIRE(icase_equals("hElLo"s, "HELLO"));
    REQUIRE(icase_equals("HeLlo"s, "hello"s));

    REQUIRE_FALSE(icase_equals("hello", "world"));
    REQUIRE_FALSE(icase_equals("hello", "world!"));
    REQUIRE_FALSE(icase_equals("hello"s, "WORLD"s));
}

TEST_CASE("icase_less_than")
{
    REQUIRE(icase_less_than("hello", "world"));
    REQUIRE(icase_less_than("hello", "World"));
    REQUIRE(icase_less_than("Hello", "world"));
    REQUIRE(icase_less_than("Hello", "World"));
    REQUIRE_FALSE(icase_less_than("hello", "aworld"s));
    REQUIRE_FALSE(icase_less_than("hello"s, "HELLO"));
    REQUIRE_FALSE(icase_less_than("HELLO"s, "hello"s));
}

TEST_CASE("thstok")
{
    SECTION("case sensitive")
    {
        static const thstok test_data[] = {
            {"IJTSK03", TTCS_IJTSK03},
            {"JTSK", TTCS_JTSK},
            {"JTSK03", TTCS_JTSK03},
            {"LAT-LONG", TTCS_LAT_LONG},
            {"LONG-LAT", TTCS_LONG_LAT},
            {"OSGB:HA", TTCS_OSGB_HA},
            {"OSGB:HB", TTCS_OSGB_HB},
            {"OSGB:HC", TTCS_OSGB_HC},
            {"OSGB:HD", TTCS_OSGB_HD},
            {nullptr, TTCS_UNKNOWN}};

        SECTION("missing key") 
        {
            REQUIRE(thcasematch_token("not there", test_data) == TTCS_UNKNOWN);
        }

        auto values = GENERATE(from_range(std::begin(test_data), std::end(test_data)-1));
        REQUIRE(thcasematch_token(values.s, test_data) == values.tok);
    }

    SECTION("case insensitive")
    {
        static const thstok test_data[] = {
            {"IJTSK03", TTCS_IJTSK03},
            {"JTSK", TTCS_JTSK},
            {"JTSK03", TTCS_JTSK03},
            {"LAT-LONG", TTCS_LAT_LONG},
            {"local", TTCS_LOCAL},
            {"LONG-LAT", TTCS_LONG_LAT},
            {"OSGB:HA", TTCS_OSGB_HA},
            {"OSGB:HB", TTCS_OSGB_HB},
            {"OSGB:HC", TTCS_OSGB_HC},
            {"OSGB:HD", TTCS_OSGB_HD},
            {nullptr, TTCS_UNKNOWN}};

        SECTION("missing key")
        {
            REQUIRE(thcasematch_token("not there", test_data) == TTCS_UNKNOWN);
        }
        
        auto values = GENERATE(from_range(std::begin(test_data), std::end(test_data)-1));
        REQUIRE(thcasematch_token(values.s, test_data) == values.tok);
    }
}
