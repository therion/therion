#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include "thexception.h"
#include "thparse.h"

TEST_CASE("thparse_image")
{
    double width{};
    double height{};
    double dpi{};
    int type{};

    SECTION("invalid image")
    {
        CHECK_THROWS_AS(thparse_image("", width, height, dpi, type), thexception);
    }

    SECTION("JPEG")
    {
        thparse_image("samples/morphing/sample1/cave1.jpg", width, height, dpi, type);

        CHECK(width == 608);
        CHECK(height == 732);
        CHECK(dpi == 72);
        CHECK(type == TT_IMG_TYPE_JPEG);
    }
    
    SECTION("PNG")
    {
        thparse_image("samples/morphing/sample2/cave.png", width, height, dpi, type);

        CHECK(width == 772);
        CHECK(height == 337);
        CHECK(dpi == 225);
        CHECK(type == TT_IMG_TYPE_PNG);
    }
}
