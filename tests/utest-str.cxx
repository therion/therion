#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#else
#include <catch2/catch.hpp>
#endif
#include "icase.h"
#include "thparse.h"
#include "thcsdata.h"
#include "thsvg.h"

using namespace std::string_literals;

TEST_CASE("icase_equal")
{
    const icase_equal comparator;
    REQUIRE(comparator("", ""));
    REQUIRE(comparator("hello", "hello"));
    REQUIRE(comparator("hello", "HELLO"s));
    REQUIRE(comparator("hElLo"s, "HELLO"));
    REQUIRE(comparator("HeLlo"s, "hello"s));

    REQUIRE_FALSE(comparator("hello", "world"));
    REQUIRE_FALSE(comparator("hello", "world!"));
    REQUIRE_FALSE(comparator("hello"s, "WORLD"s));
}

TEST_CASE("icase_less")
{
    const icase_less comparator;
    REQUIRE(comparator("hello", "world"));
    REQUIRE(comparator("hello", "World"));
    REQUIRE(comparator("Hello", "world"));
    REQUIRE(comparator("Hello", "World"));
    REQUIRE_FALSE(comparator("hello", "aworld"s));
    REQUIRE_FALSE(comparator("hello"s, "HELLO"));
    REQUIRE_FALSE(comparator("HELLO"s, "hello"s));
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

TEST_CASE("sanitize_xml_id", "[string]")
{
    CHECK(sanitize_xml_id("") == "_");
    CHECK(sanitize_xml_id("1a") == "_1a");
    CHECK(sanitize_xml_id("a1") == "a1");
    CHECK(sanitize_xml_id(":a1_:") == ":a1_:");
    CHECK(sanitize_xml_id("@a/b") == "_a_b");
    CHECK(sanitize_xml_id("!@#$%^&*()[X]") == "___________X_");
}
