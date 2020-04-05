#include "extern/catch2/catch.hpp"
#include <cmath>

#include "thproj.h"
#include "thcsdata.h"
#include "thcs.h"

// tests for coordinate systems transformations using Proj library

double th_eps = 0.05;

bool coord_equal(double x1, double x2, double precision = 0) {
  return fabs(x1 - x2) < (precision == 0 ? th_eps : precision);
}

double dms2dec(int d, int m, double s) {
  return (double) d + m/60.0 + s/3600.0;
}

double deg2rad(double d) {
  return d / 180.0 * M_PI;
}

double x,y,z;

// p1 data from skgeodesy.sk
double p1_ll_phi = deg2rad(dms2dec(48, 56, 10.50838));
double p1_ll_lambda = deg2rad(dms2dec(19, 39, 11.93192));
double p1_ll_h = 2068.93;

double p1_jtsk_y = 379033.66;
double p1_jtsk_x = 1208895.36;
double p1_jtsk_h = 2025.31;

double p1_utm_e = 401375.238;
double p1_utm_n = 5421243.277;
double p1_utm_h = 2025.437;

double p1_s42_y = 7401458.68;
double p1_s42_x = 5423542.23;

TEST_CASE( "projections: JTSK03 -- utm", "[proj]" ) {
    thcs2cs(thcs_get_params(TTCS_JTSK03), thcs_get_params(TTCS_UTM34N),
            p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_utm_h, 0.1));
}

TEST_CASE( "projections: iJTSK03 -- utm", "[proj]" ) {
    thcs2cs(thcs_get_params(TTCS_IJTSK03), thcs_get_params(TTCS_UTM34N),
            -p1_jtsk_y, -p1_jtsk_x, p1_jtsk_h, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_utm_h, 0.1));
}

TEST_CASE( "projections: latlong -- JTSK03", "[proj]" ) {
    thcs2cs(thcs_get_params(TTCS_LAT_LONG), thcs_get_params(TTCS_JTSK03),
        p1_ll_lambda, p1_ll_phi, p1_ll_h, x, y, z);
    CHECK(coord_equal(x, p1_jtsk_y, 0.4));
    CHECK(coord_equal(y, p1_jtsk_x, 0.05));
}

TEST_CASE( "projections: JTSK03 -- EPSG_4417", "[proj]" ) {
    thcs2cs(thcs_get_params(TTCS_JTSK03), thcs_get_params(TTCS_EPSG + 4417),
        p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
#if PROJ_VER < 6
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
#else
    // epsg code missing towgs84 parameters
    CHECK(coord_equal(x, p1_s42_y, 130));
    CHECK(coord_equal(y, p1_s42_x, 40));
#endif
}

// tbc
