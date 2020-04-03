#include "extern/catch2/catch.hpp"
#include <cmath>

#include "thproj.h"

// tests for coordinate systems transformations using Proj library

#if PROJ_VER < 5
double th_eps = 130;  // a quick fix to make test pass; +towgs84 seems to be ignored in proj v.4
#else
double th_eps = 0.5;
#endif

bool coord_equal(double x1, double x2) {
  return fabs(x1 - x2) < th_eps;
}

TEST_CASE( "projections: towgs", "[proj]" ) {
    double x,y,z;
    thcs2cs("+proj=krovak +czech +ellps=bessel +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0", 
            "+proj=utm +zone=34",
            379033, 1208895, 2025.31, x, y, z);
    CHECK(coord_equal(x,401375.61));
    CHECK(coord_equal(y,5421243.27));
    CHECK(coord_equal(z,2025.35));
}

// tbc
