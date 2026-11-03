#include "thdata.h"
#include "thdatabase.h"
#include "thdataleg.h"
#include "thexception.h"
#include "thparse.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <cstring>
#include <string>
#include <tuple>
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

/// Readings stored in the data order, without the terminating unknowns.
std::vector<int> order(const thdata & data)
{
    return std::vector<int>(data.d_order, data.d_order + data.d_nitems);
}

/// What the reading of a data line leaves behind in a leg: station, length, bearing, gradient.
/// Rendered as text so that unset (NaN) readings compare equal.
using leg_summary = std::tuple<std::string, std::string, std::string, std::string>;

/// Applies the data command and feeds the given lines to thdata::insert_data_leg().
std::vector<leg_summary> read_legs(const std::vector<std::string> & command,
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
    std::vector<leg_summary> legs;
    for (const auto & leg : data.leg_list)
        legs.emplace_back(leg.station.name ? leg.station.name : "", std::to_string(leg.length),
                          std::to_string(leg.bearing), std::to_string(leg.gradient));
    return legs;
}

} // namespace

TEST_CASE("thdataleg note keywords")
{
    SECTION("note and noteall are recognised")
    {
        REQUIRE(thmatch_token("note", thtt_dataleg_comp) == TT_DATALEG_NOTE);
        REQUIRE(thmatch_token("noteall", thtt_dataleg_comp) == TT_DATALEG_NOTEALL);
    }
    SECTION("the similar notes and notebook keep their meaning")
    {
        REQUIRE(thmatch_token("notes", thtt_dataleg_comp) == TT_DATALEG_NOTES);
        REQUIRE(thmatch_token("notebook", thtt_dataleg_comp) == TT_DATALEG_NOTES);
    }
    SECTION("the table stays sorted, as required by the binary search")
    {
        const thstok * tok = thtt_dataleg_comp;
        REQUIRE(tok->s != nullptr);
        for (; tok[1].s != nullptr; ++tok)
            REQUIRE(std::strcmp(tok[0].s, tok[1].s) < 0);
    }
}

TEST_CASE("thdataleg_consumes_rest_of_line")
{
    REQUIRE(thdataleg_consumes_rest_of_line(TT_DATALEG_IGNOREALL));
    REQUIRE(thdataleg_consumes_rest_of_line(TT_DATALEG_NOTEALL));
    REQUIRE_FALSE(thdataleg_consumes_rest_of_line(TT_DATALEG_IGNORE));
    REQUIRE_FALSE(thdataleg_consumes_rest_of_line(TT_DATALEG_NOTE));
    REQUIRE_FALSE(thdataleg_consumes_rest_of_line(TT_DATALEG_NEWLINE));
    REQUIRE_FALSE(thdataleg_consumes_rest_of_line(TT_DATALEG_NOTES));
}

TEST_CASE("thdata data note readings")
{
    thdata data;

    SECTION("note is accepted with an interleaved station")
    {
        set_data(data, {"normal", "station", "note", "newline", "tape", "compass", "clino"});
        REQUIRE(data.d_type == TT_DATATYPE_NORMAL);
        REQUIRE(data.di_note);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_NOTE, TT_DATALEG_NEWLINE,
                                                TT_DATALEG_LENGTH, TT_DATALEG_BEARING, TT_DATALEG_GRADIENT});
    }
    SECTION("note may precede station")
    {
        set_data(data, {"normal", "note", "station", "newline", "tape", "compass", "clino"});
        REQUIRE(data.di_note);
        REQUIRE(data.di_station);
    }
    SECTION("noteall is accepted in nosurvey style")
    {
        set_data(data, {"nosurvey", "station", "noteall"});
        REQUIRE(data.d_type == TT_DATATYPE_NOSURVEY);
        REQUIRE(data.di_note);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_NOTEALL});
    }
    SECTION("note is accepted in dimensions style")
    {
        set_data(data, {"dimensions", "station", "left", "right", "note"});
        REQUIRE(data.d_type == TT_DATATYPE_DIMS);
        REQUIRE(data.di_note);
    }
    SECTION("noteall ends the reading list like ignoreall")
    {
        set_data(data, {"nosurvey", "station", "noteall", "ignore"});
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_NOTEALL});
    }
    SECTION("ignore and ignoreall still work and do not set the note flag")
    {
        set_data(data, {"nosurvey", "station", "ignoreall"});
        REQUIRE_FALSE(data.di_note);
        REQUIRE(order(data) == std::vector<int>{TT_DATALEG_STATION, TT_DATALEG_IGNOREALL});
    }
    SECTION("the note flag is cleared by the next data command")
    {
        set_data(data, {"nosurvey", "station", "note"});
        REQUIRE(data.di_note);
        set_data(data, {"normal", "from", "to", "tape", "compass", "clino"});
        REQUIRE_FALSE(data.di_note);
    }
}

TEST_CASE("thdata data note readings are rejected")
{
    thdata data;

    SECTION("with from and to")
    {
        REQUIRE_THROWS_AS(set_data(data, {"normal", "from", "to", "note", "tape", "compass", "clino"}),
                          thexception);
        REQUIRE_THROWS_AS(set_data(data, {"nosurvey", "from", "to", "noteall"}), thexception);
    }
    SECTION("without station")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"normal", "note", "tape", "compass", "clino"}),
                            "note reading requires station reading");
    }
    SECTION("when given twice")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"nosurvey", "station", "note", "note"}),
                            "duplicate identifier -- note");
        REQUIRE_THROWS_WITH(set_data(data, {"nosurvey", "station", "note", "noteall"}),
                            "duplicate identifier -- noteall");
    }
    SECTION("after newline")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"normal", "station", "newline", "note", "tape", "compass", "clino"}),
                            "interleaved reading after newline -- note");
    }
    SECTION("in place of the readings required by the style")
    {
        REQUIRE_THROWS_WITH(set_data(data, {"normal", "station", "note", "newline", "tape", "compass"}),
                            "not all data for given style");
        REQUIRE_THROWS_WITH(set_data(data, {"cartesian", "station", "note"}),
                            "not all data for given style");
    }
}

TEST_CASE("thdata note readings are read like ignore readings")
{
    SECTION("note reads a single argument")
    {
        const std::vector<std::vector<std::string>> lines{{"a", "first note"}, {"b", "second note"}};
        const auto with_note = read_legs({"nosurvey", "station", "note"}, lines);
        const auto with_ignore = read_legs({"nosurvey", "station", "ignore"}, lines);
        REQUIRE(with_note.size() == 2);
        REQUIRE(std::get<0>(with_note[0]) == "a");
        REQUIRE(std::get<0>(with_note[1]) == "b");
        REQUIRE(with_note == with_ignore);
    }
    SECTION("note before station")
    {
        const std::vector<std::vector<std::string>> lines{{"first note", "a"}, {"second note", "b"}};
        const auto with_note = read_legs({"nosurvey", "note", "station"}, lines);
        const auto with_ignore = read_legs({"nosurvey", "ignore", "station"}, lines);
        REQUIRE(with_note.size() == 2);
        REQUIRE(std::get<0>(with_note[1]) == "b");
        REQUIRE(with_note == with_ignore);
    }
    SECTION("noteall skips the rest of the line")
    {
        const std::vector<std::vector<std::string>> lines{{"a", "many", "words", "here"}, {"b"}};
        const auto with_noteall = read_legs({"nosurvey", "station", "noteall"}, lines);
        const auto with_ignoreall = read_legs({"nosurvey", "station", "ignoreall"}, lines);
        REQUIRE(with_noteall.size() == 2);
        REQUIRE(std::get<0>(with_noteall[0]) == "a");
        REQUIRE(std::get<0>(with_noteall[1]) == "b");
        REQUIRE(with_noteall == with_ignoreall);
    }
    SECTION("note on an interleaved station line")
    {
        const std::vector<std::vector<std::string>> lines{
            {"a", "note a"}, {"1.5", "90", "10"}, {"b", "note b"}, {"2.0", "180", "-5"}};
        const std::vector<std::string> command_note{"normal", "station", "note", "newline",
                                                    "tape",   "compass", "clino"};
        const std::vector<std::string> command_ignore{"normal", "station", "ignore", "newline",
                                                      "tape",   "compass", "clino"};
        const auto with_note = read_legs(command_note, lines);
        const auto with_ignore = read_legs(command_ignore, lines);
        REQUIRE_FALSE(with_note.empty());
        REQUIRE(with_note == with_ignore);
    }
}
