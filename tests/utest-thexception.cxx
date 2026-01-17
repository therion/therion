#include "thexception.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thexception")
{
    SECTION("message")
    {
        const std::string expected_msg = "test";

        thexception e(expected_msg);
        REQUIRE(e.what() == expected_msg);
    }
    
    SECTION("message with exception")
    {
        const std::string expected_msg = "test -- runtime error";

        thexception obj("test", std::runtime_error("runtime error"));
        REQUIRE(obj.what() == expected_msg);
    }
}
