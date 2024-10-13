#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif
#include <cmath>

#include "thproj.h"
#include "thcsdata.h"
#include "thcs.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

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

double undefined = NAN;

TEST_CASE( "projections: init", "[proj]" ) {
    CHECK(thcs_check(thcs_get_params(TTCS_JTSK03)));
}

//TEST_CASE( "projections: init garbage", "[proj]" ) {
//    CHECK_FALSE(thcs_check("+proj=garbage +ellps=wgs84"));
//}

TEST_CASE( "projections: is lat/long", "[proj]" ) {
    CHECK(thcs_islatlong(thcs_get_params(TTCS_JTSK03))==false);
    CHECK(thcs_islatlong(thcs_get_params(TTCS_EPSG + 32634))==false);
    CHECK(thcs_islatlong(thcs_get_params(TTCS_LAT_LONG)));
    CHECK(thcs_islatlong(thcs_get_params(TTCS_EPSG + 4326)));
}

TEST_CASE( "projections: meridian convergence", "[proj]" ) {
    // therion uses a convention that the convergence is positive west to central meridian
    CHECK(coord_equal(thcsconverg(TTCS_EPSG + 32634, p1_utm_e, p1_utm_n), -atan(tan(p1_ll_lambda-deg2rad(21.0))*sin(p1_ll_phi))*180/M_PI, 0.1/3600));   // 0.1 deg second
    // https://geodesyapps.ga.gov.au/geographic-to-grid
    CHECK(coord_equal(thcsconverg(TTCS_EPSG + 32634, 350812.125, 5318235.614),  1.486562, 0.01/3600));  // 48 N, 19 E (central meridian 21 E)
    CHECK(coord_equal(thcsconverg(TTCS_EPSG + 32634, 649187.875, 5318235.614), -1.486562, 0.01/3600));  // 48 N, 23 E
    CHECK(coord_equal(thcsconverg(TTCS_EPSG + 32734, 350812.125, 4681764.386), -1.486562, 0.01/3600));  // 48 S, 19 E
    CHECK(coord_equal(thcsconverg(TTCS_EPSG + 32734, 649187.875, 4681764.386),  1.486562, 0.01/3600));  // 48 S, 23 E
}

TEST_CASE( "projections: UTM zones", "[proj]" ) {
    CHECK(thcs2zone(TTCS_JTSK03, 509063.948, 1303089.825,0)==34);
    CHECK(thcs2zone(TTCS_JTSK03, 509063.963, 1303089.823,0)==33);
}

TEST_CASE( "projections: EPSG label", "[proj]" ) {
#if PROJ_VER < 6
    CHECK((epsg_labels.count(32634) > 0 && strcmp(epsg_labels[32634],"WGS 84 / UTM zone 34N") == 0));
#else
    CHECK(thcs_get_label("+init=epsg:32634") == "WGS 84 / UTM zone 34N");
#endif
}


TEST_CASE( "projections: JTSK03 -- utm, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_JTSK03, TTCS_UTM34N,
            p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_jtsk_h, 0.001));
}

TEST_CASE( "projections: JTSK03 -- utm, auto=false", "[proj]" ) {
    thcs2cs(TTCS_JTSK03, TTCS_ETRS34,
            p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_jtsk_h, 0.001));
}

TEST_CASE( "projections: JTSK03 -- utm, auto=false, NaN z coordinate", "[proj]" ) {
    thcs2cs(TTCS_JTSK03, TTCS_ETRS34,
            p1_jtsk_y, p1_jtsk_x, undefined, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(std::isnan(z));
}

/*
TEST_CASE( "projections: +krovak +czech -- utm, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs("+proj=krovak +czech +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0", thcs_get_params(TTCS_UTM34N),
            p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_jtsk_h, 0.001));
} */

TEST_CASE( "projections: iJTSK03 -- utm, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_IJTSK03, TTCS_UTM34N,
            -p1_jtsk_y, -p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_jtsk_h, 0.001));
}

TEST_CASE( "projections: iJTSK03 -- utm, auto=false", "[proj]" ) {
    thcs2cs(TTCS_IJTSK03, TTCS_UTM34N,
            -p1_jtsk_y, -p1_jtsk_x, p1_jtsk_h, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, p1_jtsk_h, 0.001));
}

TEST_CASE( "projections: latlong -- JTSK03", "[proj]" ) {
    thcs2cs(TTCS_LAT_LONG, TTCS_JTSK03,
        p1_ll_lambda, p1_ll_phi, p1_ll_h, x, y, z);
    CHECK(coord_equal(x, p1_jtsk_y, 0.4));
    CHECK(coord_equal(y, p1_jtsk_x, 0.05));
}

TEST_CASE( "projections: JTSK03 -- EPSG_4417, auto=false", "[proj]" ) {
    thcs2cs(TTCS_JTSK03, TTCS_EPSG + 4417,
        p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
#if PROJ_VER < 6
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
#else
    // epsg code missing towgs84 parameters; but see "EPSG_32634 -- EPSG_4417 auto=true" which works
    // however, adding auto to this test doesn't help PROJ to find a suitable transformation from JTSK03 (but works for conversion from therion's built-in UTM34N)
    CHECK(coord_equal(x, p1_s42_y, 130));
    CHECK(coord_equal(y, p1_s42_x, 40));
#endif
}

/*
#include <iostream>
TEST_CASE( "projections: JTSK03 -- EPSG_4417, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_JTSK03, TTCS_EPSG + 4417,
        p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
#if PROJ_VER < 6
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
#else
    // epsg code missing towgs84 parameters; but see "EPSG_32634 -- EPSG_4417 auto=true" which works
    // however, adding auto to this test doesn't help PROJ to find a suitable transformation from JTSK03 (but works for conversion from therion's built-in UTM34N)
    CHECK(coord_equal(x, p1_s42_y, 130));
    CHECK(coord_equal(y, p1_s42_x, 40));
cout << x << " " << y << " " << p1_s42_y << " " << p1_s42_x << endl;
#endif
}
*/

TEST_CASE( "projections: iJTSK03 -- EPSG_4417, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_IJTSK03, TTCS_EPSG + 4417,
        -p1_jtsk_y, -p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
#if PROJ_VER < 6
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
#else
    // epsg code missing towgs84 parameters; but see "EPSG_32634 -- EPSG_4417 auto=true" which works
    // however, adding auto to this test doesn't help PROJ to find a suitable transformation from JTSK03 (but works for conversion from therion's built-in UTM34N)
    CHECK(coord_equal(x, p1_s42_y, 130));
    CHECK(coord_equal(y, p1_s42_x, 40));
#endif
}

TEST_CASE( "UTM34N -- EPSG_4417 auto=true", "[proj]" ) {    // UTM34N -> S42
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_UTM34N, TTCS_EPSG + 4417,
        p1_utm_e, p1_utm_n, p1_utm_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
}

TEST_CASE( "EPSG_32634 -- EPSG_4417 auto=true", "[proj]" ) {    // UTM34N -> S42
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_EPSG + 32634, TTCS_EPSG + 4417,
        p1_utm_e, p1_utm_n, p1_utm_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_s42_y, 1.3));
    CHECK(coord_equal(y, p1_s42_x, 0.05));
}

TEST_CASE( "EPSG_32634 -- EPSG_4417 auto=false", "[proj]" ) {   // UTM34N -> S42
    thcs2cs(TTCS_EPSG + 32634, TTCS_EPSG + 4417,
        p1_utm_e, p1_utm_n, p1_utm_h, x, y, z);
    CHECK(coord_equal(x, p1_s42_y, 130));
    CHECK(coord_equal(y, p1_s42_x, 40));
}


TEST_CASE( "EPSG_4326 -- EPSG_32634 auto=true", "[proj]" ) {  // LATLON -> UTM34N
    thcs_cfg.proj_auto = true;
    thcs2cs(TTCS_EPSG + 4326, TTCS_EPSG + 32634,
        p1_ll_lambda, p1_ll_phi, p1_ll_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 0.4));
    CHECK(coord_equal(y, p1_utm_n, 0.02));
}

TEST_CASE( "EPSG_4326 -- EPSG_32634 auto=false", "[proj]" ) {  // LATLON -> UTM34N
    thcs2cs(TTCS_EPSG + 4326, TTCS_EPSG + 32634,
        p1_ll_lambda, p1_ll_phi, p1_ll_h, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.4));
    CHECK(coord_equal(y, p1_utm_n, 0.02));
}

// null grid
TEST_CASE( "s-merc -- EPSG_32634", "[proj]" ) {  // Pseudo Mercator -> UTM34N
    thcs2cs(TTCS_S_MERC, TTCS_EPSG + 32634,
        2187796.40, 6264051.68, 2025.44, x, y, z);
    CHECK(coord_equal(x, p1_utm_e, 0.01));
    CHECK(coord_equal(y, p1_utm_n, 0.01));
    CHECK(coord_equal(z, 2025.44, 0.001));
}

/*
// GRID
TEST_CASE( "projections: JTSK grid -- utm, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs("+proj=krovak +ellps=bessel +czech +nadgrids=slovak", thcs_get_params(TTCS_UTM34N),
            p1_jtsk_y, p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 1));
    CHECK(coord_equal(y, p1_utm_n, 1));
    CHECK(coord_equal(z, p1_utm_h, 1));
}

// GRID
TEST_CASE( "projections: iJTSK grid -- utm, auto=true", "[proj]" ) {
    thcs_cfg.proj_auto = true;
    thcs2cs("+proj=krovak +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +nadgrids=slovak", thcs_get_params(TTCS_UTM34N),
            -p1_jtsk_y, -p1_jtsk_x, p1_jtsk_h, x, y, z);
    thcs_cfg.proj_auto = false;
    CHECK(coord_equal(x, p1_utm_e, 1));
    CHECK(coord_equal(y, p1_utm_n, 1));
    CHECK(coord_equal(z, p1_utm_h, 1));
}
*/

// tbc
