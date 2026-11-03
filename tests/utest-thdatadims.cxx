#include "thdata.h"
#include "thdatabase.h"
#include "thdataleg.h"
#include "thexception.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <cmath>
#include <string>
#include <vector>

namespace {

/// Calls thdata::set_data_data() like the `data` command does.
void set_data(thdata & data, std::vector<std::string> words)
{
    std::vector<char *> args;
    for (auto & word : words)
        args.push_back(word.data());
    data.set_data_data(static_cast<int>(args.size()), args.data());
}

/// Readings stored in the data order.
std::vector<int> order(const thdata & data)
{
    return std::vector<int>(data.d_order, data.d_order + data.d_nitems);
}

/// Station and dimensions as read from one data line.
struct dims_summary {
    std::string station;
    double up, down, left, right;
};

/// Applies the data command and feeds the given lines to thdata::insert_data_leg().
std::vector<dims_summary> read_dims(const std::vector<std::string> & command,
                                    const std::vector<std::vector<std::string>> & lines)
{
    thdatabase db;
    thdata data;
    data.db = &db;
    set_data(data, command);
    for (auto line : lines) {
        std::vector<char *> args;
        for (auto & word : line)
            args.push_back(word.data());
        data.insert_data_leg(static_cast<int>(args.size()), args.data());
    }
    std::vector<dims_summary> dims;
    for (const auto & dim : data.dims_list)
        dims.push_back({dim.station.name ? dim.station.name : "", dim.up, dim.down, dim.left, dim.right});
    return dims;
}

} // namespace

TEST_CASE("thdata dimensions style needs no dimension when ignore or note is given")
{
    thdata data;

    SECTION("ignore in place of the dimensions")
    {
        set_data(data, {"dimensions", "station", "ignore"});
        REQUIRE(data.d_type == TT_DATATYPE_DIMS);
        REQUIRE(data.di_station);
        REQUIRE(data.di_ignore);
        REQUIRE_FALSE(data.di_up);
        REQUIRE_FALSE(data.di_down);
        REQUIRE_FALSE(data.di_left);
        REQUIRE_FALSE(data.di_right);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_IGNORE});
    }
    SECTION("ignoreall in place of the dimensions")
    {
        set_data(data, {"dimensions", "station", "ignoreall"});
        REQUIRE(data.di_ignore);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_IGNOREALL});
    }
    SECTION("note in place of the dimensions")
    {
        set_data(data, {"dimensions", "station", "note"});
        REQUIRE(data.d_type == TT_DATATYPE_DIMS);
        REQUIRE(data.di_note);
        REQUIRE_FALSE(data.di_ignore);
        REQUIRE_FALSE(data.di_up);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_NOTE});
    }
    SECTION("noteall in place of the dimensions")
    {
        set_data(data, {"dimensions", "station", "noteall"});
        REQUIRE(data.di_note);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_NOTEALL});
    }
    SECTION("note together with some dimensions")
    {
        set_data(data, {"dimensions", "station", "left", "right", "note"});
        REQUIRE(data.di_left);
        REQUIRE(data.di_right);
        REQUIRE(data.di_note);
    }
    SECTION("ignore before station")
    {
        set_data(data, {"dimensions", "ignore", "station"});
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_IGNORE, TT_DATALEG_STATION});
    }
    SECTION("ignore together with some dimensions")
    {
        set_data(data, {"dimensions", "station", "up", "ignore"});
        REQUIRE(data.di_up);
        REQUIRE(data.di_ignore);
    }
    SECTION("some of the dimensions only")
    {
        set_data(data, {"dimensions", "station", "up", "down"});
        REQUIRE(data.di_up);
        REQUIRE(data.di_down);
        REQUIRE_FALSE(data.di_left);
        REQUIRE_FALSE(data.di_right);
    }
    SECTION("all the dimensions")
    {
        set_data(data, {"dimensions", "station", "left", "right", "up", "down"});
        REQUIRE(data.di_left);
        REQUIRE(data.di_right);
        REQUIRE(data.di_up);
        REQUIRE(data.di_down);
        REQUIRE_FALSE(data.di_ignore);
    }
    SECTION("the ignore flag is cleared by the next data command")
    {
        set_data(data, {"dimensions", "station", "ignore"});
        REQUIRE(data.di_ignore);
        set_data(data, {"dimensions", "station", "up"});
        REQUIRE_FALSE(data.di_ignore);
    }
}

TEST_CASE("thdata dimensions style is still rejected")
{
    thdata data;

    SECTION("with the station only")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "station"}), "not all data for given style");
    }
    SECTION("without the station")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "ignore"}), "not all data for given style");
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "up", "down"}), "not all data for given style");
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "note"}), "note reading requires station reading");
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "noteall"}), "note reading requires station reading");
    }
    SECTION("with from and to")
    {
        REQUIRE_THROWS_AS(set_data(data, {"dimensions", "from", "to", "ignore"}), thexception);
        REQUIRE_THROWS_AS(set_data(data, {"dimensions", "from", "to", "up"}), thexception);
        REQUIRE_THROWS_AS(set_data(data, {"dimensions", "from", "to", "note"}), thexception);
    }
    SECTION("with a second note")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"dimensions", "station", "note", "noteall"}),
                            "duplicate identifier -- noteall");
    }
}

TEST_CASE("thdata ignore does not complete other data styles")
{
    thdata data;

    REQUIRE_THROWS_WITH(set_data(data, {"normal", "station", "ignore"}), "not all data for given style");
    REQUIRE_THROWS_WITH(set_data(data, {"cartesian", "station", "ignoreall"}), "not all data for given style");
    REQUIRE_THROWS_WITH(set_data(data, {"normal", "station", "note"}), "not all data for given style");
    REQUIRE_THROWS_WITH(set_data(data, {"cartesian", "station", "noteall"}), "not all data for given style");
    REQUIRE_NOTHROW(set_data(data, {"nosurvey", "station"}));
}

TEST_CASE("thdata dimensions are read without any dimension")
{
    SECTION("ignore")
    {
        const auto dims = read_dims({"dimensions", "station", "ignore"}, {{"a", "x"}, {"b", "y"}});
        REQUIRE(dims.size() == 2);
        REQUIRE(dims[0].station == "a");
        REQUIRE(dims[1].station == "b");
        for (const auto & dim : dims) {
            REQUIRE(std::isnan(dim.up));
            REQUIRE(std::isnan(dim.down));
            REQUIRE(std::isnan(dim.left));
            REQUIRE(std::isnan(dim.right));
        }
    }
    SECTION("ignoreall, also on a line holding the station only")
    {
        const auto dims = read_dims({"dimensions", "station", "ignoreall"}, {{"a", "many", "words"}, {"b"}});
        REQUIRE(dims.size() == 2);
        REQUIRE(dims[0].station == "a");
        REQUIRE(dims[1].station == "b");
        REQUIRE(std::isnan(dims[0].up));
        REQUIRE(std::isnan(dims[1].left));
    }
    SECTION("note")
    {
        const auto dims = read_dims({"dimensions", "station", "note"}, {{"a", "first note"}, {"b", "second note"}});
        REQUIRE(dims.size() == 2);
        REQUIRE(dims[0].station == "a");
        REQUIRE(dims[1].station == "b");
        for (const auto & dim : dims) {
            REQUIRE(std::isnan(dim.up));
            REQUIRE(std::isnan(dim.down));
            REQUIRE(std::isnan(dim.left));
            REQUIRE(std::isnan(dim.right));
        }
    }
    SECTION("noteall, also on a line holding the station only")
    {
        const auto dims = read_dims({"dimensions", "station", "noteall"}, {{"a", "many", "words"}, {"b"}});
        REQUIRE(dims.size() == 2);
        REQUIRE(dims[0].station == "a");
        REQUIRE(dims[1].station == "b");
        REQUIRE(std::isnan(dims[0].up));
        REQUIRE(std::isnan(dims[1].right));
    }
    SECTION("a note does not disturb stated dimensions")
    {
        const auto dims = read_dims({"dimensions", "station", "left", "note"}, {{"a", "0.5", "some note"}});
        REQUIRE(dims.size() == 1);
        REQUIRE(dims[0].station == "a");
        REQUIRE(std::fabs(dims[0].left - 0.5) < 1e-9);
        REQUIRE(std::isnan(dims[0].right));
    }
    SECTION("stated dimensions keep their values and the others stay unset")
    {
        const auto dims = read_dims({"dimensions", "station", "up", "down"}, {{"a", "1.5", "2.5"}});
        REQUIRE(dims.size() == 1);
        REQUIRE(dims[0].station == "a");
        REQUIRE(std::fabs(dims[0].up - 1.5) < 1e-9);
        REQUIRE(std::fabs(dims[0].down - 2.5) < 1e-9);
        REQUIRE(std::isnan(dims[0].left));
        REQUIRE(std::isnan(dims[0].right));
    }
}
