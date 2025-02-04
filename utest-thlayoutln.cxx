#include "thlayoutln.h"
#include "thlayout.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thlayoutln")
{
    thlayoutln obj;
    REQUIRE(obj.line == nullptr);
    REQUIRE(obj.smid == 0);
    REQUIRE(strcmp(obj.path, "") == 0);
    REQUIRE(obj.code == TT_LAYOUT_CODE_TEX_ATLAS);
}
