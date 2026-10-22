#include "thdatabase.h"
#include "thdata.h"
#include "thchenc.h"
#include "thexception.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <cstring>

namespace {

// Sets a copyright option on obj from a list of arguments.
void set_copyright(thdata & obj, std::initializer_list<const char *> args)
{
    std::vector<char *> argv;
    for (auto a : args)
        argv.push_back(const_cast<char *>(a));
    obj.set(thcmd_option_desc(TT_DATAOBJECT_COPYRIGHT, int(argv.size())), argv.data(), TT_UTF_8, 0);
}

}

TEST_CASE("copyright licence")
{
    auto obj = thdb.create<thdata>(thobjectsrc("test.th", 1));

    SECTION("without licence")
    {
        set_copyright(*obj, {"2024", "CUCC Expo"});
        REQUIRE(obj->copyright_map.size() == 1);
        REQUIRE(strcmp(obj->copyright_map.begin()->first.name, "CUCC Expo") == 0);
        REQUIRE(strcmp(obj->copyright_map.begin()->first.licence, "") == 0);
    }
    SECTION("with licence")
    {
        set_copyright(*obj, {"2024", "CUCC Expo", "CC BY-SA"});
        REQUIRE(obj->copyright_map.size() == 1);
        REQUIRE(strcmp(obj->copyright_map.begin()->first.name, "CUCC Expo") == 0);
        REQUIRE(strcmp(obj->copyright_map.begin()->first.licence, "CC BY-SA") == 0);
    }
    SECTION("same holder with different licences kept apart")
    {
        set_copyright(*obj, {"2024", "CUCC Expo", "GPLv2+"});
        set_copyright(*obj, {"2025", "CUCC Expo", "CC BY-SA"});
        set_copyright(*obj, {"2026", "CUCC Expo"});
        REQUIRE(obj->copyright_map.size() == 3);
    }
    SECTION("same holder and licence joined")
    {
        set_copyright(*obj, {"2024", "CUCC Expo", "CC BY-SA"});
        set_copyright(*obj, {"2025", "CUCC Expo", "CC BY-SA"});
        REQUIRE(obj->copyright_map.size() == 1);
    }
    SECTION("too many arguments")
    {
        REQUIRE_THROWS_AS(set_copyright(*obj, {"2024", "CUCC Expo", "CC BY-SA", "extra"}), thexception);
    }
}
