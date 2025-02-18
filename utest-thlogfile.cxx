#include "thlogfile.h"

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

TEST_CASE("thlog()")
{
    // tests (with the help of code coverage) that the thlogfile instance
    // is created and correctly destroyed at the end of the program
    [[maybe_unused]] auto& log = thlog();
}
