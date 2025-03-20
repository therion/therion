#include "thlogfile.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <fstream>
#include <filesystem>

using namespace std::string_literals;
using namespace std::string_view_literals;

TEST_CASE("get_thlogfile()")
{
    // tests (with the help of code coverage) that the thlogfile instance
    // is created and correctly destroyed at the end of the program
    [[maybe_unused]] auto& log = get_thlogfile();
}

TEST_CASE("thlogfile")
{
    static const std::string file_name = "test.log";

    std::filesystem::remove(file_name);
    REQUIRE_FALSE(std::filesystem::exists(file_name));

    {
        thlogfile log;
        log.set_file_name(file_name.c_str());
        REQUIRE(log.get_file_name() == file_name);
        log.set_logging(true);
        REQUIRE(log.get_logging());
        log.print("test\n");
        log.print("hello, world\n"s);
        log.print("test2\n"sv);
    }

    std::vector<std::string> lines;
    std::ifstream test_file(file_name);
    std::string line;
    while (std::getline(test_file, line))
    {
        lines.push_back(line);
    }
    REQUIRE(lines.size() == 3);
    REQUIRE(lines[0] == "test");
    REQUIRE(lines[1] == "hello, world");
    REQUIRE(lines[2] == "test2");
}
