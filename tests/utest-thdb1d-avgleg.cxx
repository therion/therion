/**
 * @file utest-thdb1d-avgleg.cxx
 * Unit tests for consecutive-leg averaging in thdb1d, including legs whose
 * stations are written in reversed order (e.g. 5->4 following 4->5).
 *
 * The averaging logic is tested by constructing thdataleg objects with
 * total_dx/dy/dz pre-set (as thdb1d does after polar->Cartesian conversion)
 * and then running the same loop that lives in thdb1d::process_data().
 *
 * All legs use data_type = TT_DATATYPE_NORMAL and flags = TT_LEGFLAG_NONE
 * unless a test is specifically exercising those guards.
 */

#ifdef CATCH2_V3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include "thdataleg.h"
#include "thinfnan.h"

#include <cmath>
#include <list>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static constexpr double TOLERANCE = 1e-9;

/**
 * Build a minimal thdataleg for averaging tests.
 *
 * @param from_id   from-station unsigned id
 * @param to_id     to-station unsigned id
 * @param length    tape length (metres)
 * @param bearing   bearing (degrees, 0-360)
 * @param gradient  gradient/clino (degrees, -90 to +90)
 * @param data_type TT_DATATYPE_* constant (default NORMAL)
 * @param flags     TT_LEGFLAG_* bitmask (default NONE)
 */
static thdataleg make_leg(unsigned long from_id, unsigned long to_id,
                          double length, double bearing, double gradient,
                          int data_type = TT_DATATYPE_NORMAL,
                          int flags = TT_LEGFLAG_NONE)
{
    thdataleg leg;
    leg.is_valid    = true;
    leg.direction   = true;
    leg.data_type   = data_type;
    leg.flags       = flags;
    leg.from.id     = from_id;
    leg.to.id       = to_id;
    leg.length      = length;
    leg.bearing     = bearing;
    leg.gradient    = gradient;

    // Reproduce the polar->Cartesian conversion from thdb1d.cxx so that
    // total_dx/dy/dz are consistent with the raw readings.
    double total_bearing  = bearing;
    double total_gradient = gradient;
    double horiz          = length * std::cos(total_gradient / 180.0 * THPI);
    leg.total_dx          = horiz * std::sin(total_bearing / 180.0 * THPI);
    leg.total_dy          = horiz * std::cos(total_bearing / 180.0 * THPI);
    leg.total_dz          = length * std::sin(total_gradient / 180.0 * THPI);
    leg.total_length      = length;
    leg.total_bearing     = total_bearing;
    leg.total_gradient    = total_gradient;
    leg.adj_dx            = leg.total_dx;
    leg.adj_dy            = leg.total_dy;
    leg.adj_dz            = leg.total_dz;
    return leg;
}

/**
 * Run the averaging loop from thdb1d::process_data() over a leg list.
 * This is a verbatim copy of the production loop so that the tests exercise
 * exactly the same code path.
 */
static void run_averaging(thdataleg_list & leg_list)
{
    thdataleg_list::iterator prev_lei = leg_list.end();
    int avg_count = 0;

    for (auto lei = leg_list.begin(); lei != leg_list.end(); lei++) {
        if (!lei->is_valid)
            continue;

        bool same_order = (avg_count > 0
            && prev_lei != leg_list.end()
            && lei->from.id == prev_lei->from.id
            && lei->to.id   == prev_lei->to.id
            && lei->data_type == prev_lei->data_type
            && lei->flags     == prev_lei->flags);

        bool is_reversed_leg = (!same_order
            && avg_count > 0
            && prev_lei != leg_list.end()
            && lei->from.id == prev_lei->to.id
            && lei->to.id   == prev_lei->from.id
            && lei->data_type == prev_lei->data_type
            && lei->flags     == prev_lei->flags);

        if (same_order || is_reversed_leg) {
            double merge_dx = is_reversed_leg ? -lei->total_dx : lei->total_dx;
            double merge_dy = is_reversed_leg ? -lei->total_dy : lei->total_dy;
            double merge_dz = is_reversed_leg ? -lei->total_dz : lei->total_dz;

            prev_lei->total_dx = (prev_lei->total_dx * avg_count + merge_dx) / (avg_count + 1);
            prev_lei->total_dy = (prev_lei->total_dy * avg_count + merge_dy) / (avg_count + 1);
            prev_lei->total_dz = (prev_lei->total_dz * avg_count + merge_dz) / (avg_count + 1);

            prev_lei->adj_dx = prev_lei->total_dx;
            prev_lei->adj_dy = prev_lei->total_dy;
            prev_lei->adj_dz = prev_lei->total_dz;

            prev_lei->total_length   = thdxyz2length(prev_lei->total_dx, prev_lei->total_dy, prev_lei->total_dz);
            prev_lei->total_bearing  = thdxyz2bearing(prev_lei->total_dx, prev_lei->total_dy, prev_lei->total_dz);
            prev_lei->total_gradient = thdxyz2clino(prev_lei->total_dx, prev_lei->total_dy, prev_lei->total_dz);

            prev_lei->length = (prev_lei->length * avg_count + lei->length) / (avg_count + 1);
            if (prev_lei->direction) {
                prev_lei->bearing  = prev_lei->total_bearing;
                prev_lei->gradient = prev_lei->total_gradient;
            } else {
                prev_lei->bearing = prev_lei->total_bearing - 180.0;
                if (prev_lei->bearing < 0.0)
                    prev_lei->bearing += 360.0;
                prev_lei->gradient = -prev_lei->total_gradient;
            }

            lei->is_valid = false;
            avg_count++;
        } else {
            prev_lei = lei;
            avg_count = 1;
        }
    }
}

/** Count legs that remain valid after averaging. */
static int count_valid(const thdataleg_list & legs)
{
    int n = 0;
    for (const auto & leg : legs)
        if (leg.is_valid)
            n++;
    return n;
}

/** Return the first valid leg in the list (or a default-constructed one). */
static thdataleg first_valid(const thdataleg_list & legs)
{
    for (const auto & leg : legs)
        if (leg.is_valid)
            return leg;
    return thdataleg{};
}

// ---------------------------------------------------------------------------
// Tests: same-order legs (existing behaviour, must still pass)
// ---------------------------------------------------------------------------

TEST_CASE("avg_two_forward_legs")
{
    // 4->5 twice: two identical readings.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.1, 23.1));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    thdataleg result = first_valid(legs);
    REQUIRE(result.from.id == 4UL);
    REQUIRE(result.to.id   == 5UL);
    // length should be arithmetic mean
    REQUIRE(std::abs(result.length - (2.41 + 2.43) / 2.0) < TOLERANCE);
}

TEST_CASE("avg_three_forward_legs")
{
    // 4->5 three times.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.1, 23.1));
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.0, 23.2));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    double expected_length = (2.41 + 2.43 + 2.41) / 3.0;
    REQUIRE(std::abs(first_valid(legs).length - expected_length) < TOLERANCE);
}

// ---------------------------------------------------------------------------
// Tests: reversed-order legs (new behaviour)
// ---------------------------------------------------------------------------

TEST_CASE("avg_same_pair_reversed_order_second")
{
    // Forward 4->5 then reversed 5->4.
    // Both connect stations 4 and 5; the reversed leg's Cartesian vector
    // is negated before averaging.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    thdataleg result = first_valid(legs);
    // The surviving leg is the first one (4->5).
    REQUIRE(result.from.id == 4UL);
    REQUIRE(result.to.id   == 5UL);
    // Averaged length is the mean of the two tape readings.
    double expected_length = (2.41 + 2.39) / 2.0;
    REQUIRE(std::abs(result.length - expected_length) < TOLERANCE);
    // total_bearing must point in the 4->5 direction (approximately 25 degrees).
    REQUIRE(result.total_bearing > 20.0);
    REQUIRE(result.total_bearing < 30.0);
}

TEST_CASE("avg_same_pair_reversed_order_first")
{
    // Reversed leg first: 7->6, then forward 6->7.
    // The first leg becomes prev_lei; the second is negated and merged in.
    thdataleg_list legs;
    legs.push_back(make_leg(7UL, 6UL, 2.39, 205.2, -23.4));
    legs.push_back(make_leg(6UL, 7UL, 2.41, 25.1,   23.2));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    thdataleg result = first_valid(legs);
    // The surviving leg is the first one (7->6).
    REQUIRE(result.from.id == 7UL);
    REQUIRE(result.to.id   == 6UL);
    double expected_length = (2.39 + 2.41) / 2.0;
    REQUIRE(std::abs(result.length - expected_length) < TOLERANCE);
    // total_bearing must point in the 7->6 direction (approximately 205 degrees).
    REQUIRE(result.total_bearing > 200.0);
    REQUIRE(result.total_bearing < 210.0);
}

TEST_CASE("avg_same_pair_multiple_reversed")
{
    // Forward then two reversed: 4->5, 5->4, 5->4.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4));
    legs.push_back(make_leg(5UL, 4UL, 2.40, 205.0, -23.3));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    REQUIRE(first_valid(legs).from.id == 4UL);
    REQUIRE(first_valid(legs).to.id   == 5UL);
    double expected_length = (2.41 + 2.39 + 2.40) / 3.0;
    REQUIRE(std::abs(first_valid(legs).length - expected_length) < TOLERANCE);
}

TEST_CASE("avg_same_pair_mixed_order")
{
    // Forward, reversed, forward: 4->5, 5->4, 4->5.
    // All three should merge into the original 4->5 leg.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.0,  23.1));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    REQUIRE(first_valid(legs).from.id == 4UL);
    REQUIRE(first_valid(legs).to.id   == 5UL);
    double expected_length = (2.41 + 2.39 + 2.43) / 3.0;
    REQUIRE(std::abs(first_valid(legs).length - expected_length) < TOLERANCE);
}

TEST_CASE("avg_cartesian_direction_after_merge")
{
    // After merging a reversed leg, total_bearing must be derived from the
    // averaged Cartesian vector, not from the raw bearings.
    // Use exact opposite bearings so the cancellation is numerically clean.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.40, 90.0,  0.0));   // due East
    legs.push_back(make_leg(5UL, 4UL, 2.40, 270.0, 0.0));   // due West (reversed)

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    thdataleg result = first_valid(legs);
    // Averaged Cartesian vector still points East (90 degrees).
    REQUIRE(std::abs(result.total_bearing - 90.0) < 1e-6);
    REQUIRE(std::abs(result.total_gradient - 0.0) < 1e-6);
}

// ---------------------------------------------------------------------------
// Tests: non-merging cases
// ---------------------------------------------------------------------------

TEST_CASE("no_avg_different_pair")
{
    // 4->5 then 5->6: different pairs, no merge.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2));
    legs.push_back(make_leg(5UL, 6UL, 2.43, 30.0, 10.0));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 2);
}

TEST_CASE("no_avg_non_adjacent_same_order")
{
    // 4->5, unrelated 6->7, 4->5: second 4->5 must NOT merge with the first.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2));
    legs.push_back(make_leg(6UL, 7UL, 1.00, 90.0,  0.0));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.0, 23.1));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 3);
}

TEST_CASE("no_avg_non_adjacent_reversed")
{
    // 4->5, unrelated 6->7, then reversed 5->4: must NOT merge
    // because the consecutive run was broken by 6->7.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2));
    legs.push_back(make_leg(6UL, 7UL, 1.00, 90.0,   0.0));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 3);
}

TEST_CASE("no_avg_different_flags")
{
    // Two 4->5 legs with different flags: no merge.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2, TT_DATATYPE_NORMAL, TT_LEGFLAG_NONE));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.0, 23.1, TT_DATATYPE_NORMAL, TT_LEGFLAG_SURFACE));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 2);
}

TEST_CASE("no_avg_different_data_type")
{
    // Two 4->5 legs with different data_type: no merge.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1, 23.2, TT_DATATYPE_NORMAL,    TT_LEGFLAG_NONE));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.0, 23.1, TT_DATATYPE_CARTESIAN, TT_LEGFLAG_NONE));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 2);
}

TEST_CASE("no_avg_reversed_different_flags")
{
    // Reversed pair 4->5 / 5->4 but different flags: no merge.
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2, TT_DATATYPE_NORMAL, TT_LEGFLAG_NONE));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4, TT_DATATYPE_NORMAL, TT_LEGFLAG_DUPLICATE));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 2);
}

// ---------------------------------------------------------------------------
// Tests: length averaging correctness
// ---------------------------------------------------------------------------

TEST_CASE("avg_length_correctness_four_legs")
{
    // Reproduce the user's example: three same-order then one reversed.
    // 4 5 2.41  25.1  23.2
    // 4 5 2.43  25.1  23.1
    // 4 5 2.41  25.0  23.2
    // 5 4 2.39 205.2 -23.4
    thdataleg_list legs;
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.1,  23.2));
    legs.push_back(make_leg(4UL, 5UL, 2.43, 25.1,  23.1));
    legs.push_back(make_leg(4UL, 5UL, 2.41, 25.0,  23.2));
    legs.push_back(make_leg(5UL, 4UL, 2.39, 205.2, -23.4));

    run_averaging(legs);

    REQUIRE(count_valid(legs) == 1);
    double expected_length = (2.41 + 2.43 + 2.41 + 2.39) / 4.0;
    REQUIRE(std::abs(first_valid(legs).length - expected_length) < TOLERANCE);
    // Direction of surviving leg unchanged (4->5).
    REQUIRE(first_valid(legs).from.id == 4UL);
    REQUIRE(first_valid(legs).to.id   == 5UL);
}
