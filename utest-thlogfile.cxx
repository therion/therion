#include "thlogfile.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <fstream>
#include <filesystem>

TEST_CASE("thlog()")
{
    // tests (with the help of code coverage) that the thlogfile instance
    // is created and correctly destroyed at the end of the program
    [[maybe_unused]] auto& log = thlog();
}

TEST_CASE("thlogfile")
{
    std::filesystem::remove("test.log");

    thlogfile log;
    log.set_file_name("test.log");
    REQUIRE(strcmp(log.get_file_name(), "test.log") == 0);
    log.set_logging(true);
    REQUIRE(log.get_logging());
    log.print("test\n");
    log.printf("hello, %s\n", "world");
    log.printf("test2\n");

    std::vector<std::string> lines;
    std::ifstream test_file("test.log");
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
