#include "thdatastation.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <cstring>

TEST_CASE("thdatastation")
{
    thdatastation obj;
    REQUIRE(obj.get_class_id() == TT_DATASTATION_CMD);
    REQUIRE(strcmp(obj.get_class_name(), "thdatastation") == 0);
    REQUIRE(obj.is(TT_DATASTATION_CMD));
    REQUIRE(obj.is(TT_DATAOBJECT_CMD));
    REQUIRE_FALSE(obj.is(TT_UNKNOWN_CMD));
}
