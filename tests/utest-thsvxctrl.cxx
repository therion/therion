#include "thsvxctrl.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thsvx_flags_changed")
{
    SECTION("flags Survex writes break the block")
    {
        REQUIRE(thsvx_flags_changed(TT_LEGFLAG_NONE, TT_LEGFLAG_SURFACE));
        REQUIRE(thsvx_flags_changed(TT_LEGFLAG_NONE, TT_LEGFLAG_DUPLICATE));
        REQUIRE(thsvx_flags_changed(TT_LEGFLAG_NONE, TT_LEGFLAG_SPLAY));
    }
    SECTION("flags Survex knows nothing about do not")
    {
        REQUIRE_FALSE(thsvx_flags_changed(TT_LEGFLAG_NONE, TT_LEGFLAG_APPROXIMATE));
        REQUIRE_FALSE(thsvx_flags_changed(TT_LEGFLAG_NONE, TT_LEGFLAG_ARTIFICIAL));
        REQUIRE_FALSE(thsvx_flags_changed(TT_LEGFLAG_APPROXIMATE, TT_LEGFLAG_ARTIFICIAL));
    }
    SECTION("a known flag still counts when an unknown one changes with it")
    {
        REQUIRE(thsvx_flags_changed(TT_LEGFLAG_APPROXIMATE,
                                    TT_LEGFLAG_APPROXIMATE | TT_LEGFLAG_SURFACE));
        REQUIRE_FALSE(thsvx_flags_changed(TT_LEGFLAG_SURFACE,
                                          TT_LEGFLAG_SURFACE | TT_LEGFLAG_APPROXIMATE));
        REQUIRE_FALSE(thsvx_flags_changed(TT_LEGFLAG_SURFACE,
                                          TT_LEGFLAG_SURFACE | TT_LEGFLAG_ARTIFICIAL));
    }
}
