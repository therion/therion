/**
 * @file thbezier.cxx
 */

/*
 * Bezier interpolation for inkscape drawing code.
 * Original code published in:
 *   An Algorithm for Automatically Fitting Digitized Curves
 *   by Philip J. Schneider
 *  "Graphics Gems", Academic Press, 1990
 *
 * Authors:
 *   Philip J. Schneider
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   Peter Moulder <pmoulder@mail.csse.monash.edu.au>
 *
 * Copyright (C) 1990 Philip J. Schneider
 * Copyright (C) 2001 Lauris Kaplinski
 * Copyright (C) 2001 Ximian, Inc.
 * Copyright (C) 2003,2004 Monash University
 *
 */

  
/* Copyright (C) 2007 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thbezier.h"
#include <cassert>
#include <cstring>
#include <cstdio>

#define __SP_BEZIER_UTILS_C__

#define SP_HUGE 1e5
#define noBEZIER_DEBUG

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

//#include <glib/gmessages.h>
//#include <glib/gmem.h>
//#include <glib/gtypes.h>

//#include <libnr/nr-forward.h>
//#include <libnr/nr-point-fns.h>

#include <math.h>
#ifdef THMSVC
#include <float.h>
#define hypot _hypot
#endif

#define g_return_val_if_fail(C,V) if (!(C)) return (V);
#define g_return_if_fail(C) if (!(C)) return;
#define g_new(T,L) new T [L];
#define g_free(V) if (V != NULL) delete [] V
#define g_assert(X) assert(X)

/* You might try changing the above to <cmath> if you have problems.
 * Whether you use math.h or cmath, you may need to edit the .cpp file
 * and/or other .h files to use the same header file.
 */

#if defined(__isnan)
# define isNaN(_a) (__isnan(_a))	/* MacOSX/Darwin definition < 10.4 */
#elif defined(WIN32) || defined(_isnan)
# define isNaN(_a) (_isnan(_a)) 	/* Win32 definition */
#elif defined(isnan) || defined(__FreeBSD__) || defined(__osf__)
# define isNaN(_a) (isnan(_a))		/* GNU definition */
#else
# define isNaN(_a) (std::isnan(_a))
#endif

/* If the above doesn't work, then try (a != a).
 * Also, please report a bug as per http://www.inkscape.org/report_bugs.php,
 * giving information about what platform and compiler version you're using.
 */


#if defined(__isfinite)
# define isFinite(_a) (__isfinite(_a))	/* MacOSX/Darwin definition < 10.4 */
#elif defined(__sgi)
# define isFinite(_a) (_isfinite(_a))
#elif defined(isfinite)
# define isFinite(_a) (isfinite(_a))
#elif defined(__osf__)
# define isFinite(_a) (finite(_a) && !isNaN(_a))
#elif defined(THMSVC)
# define isFinite(_a) (_finite(_a) && !isNaN(_a))
#else
# define isFinite(_a) (std::isfinite(_a))
#endif
/* If the above doesn't work, then try (finite(_a) && !isNaN(_a)) or (!isNaN((_a) - (_a))).
 * Also, please report a bug as per http://www.inkscape.org/report_bugs.php,
 * giving information about what platform and compiler version you're using.
 */





class NR_Point {

private:
    double _pt[2];

public:

    inline NR_Point()
    { _pt[0] = _pt[1] = 0; }

    inline NR_Point(double x, double y) {
        _pt[0] = x;
        _pt[1] = y;
    }

    inline NR_Point(NR_Point const &p) {
        for (unsigned i = 0; i < 2; ++i) {
            _pt[i] = p._pt[i];
        }
    }

    inline NR_Point &operator=(NR_Point const &p) {
        for (unsigned i = 0; i < 2; ++i) {
            _pt[i] = p._pt[i];
        }
        return *this;
    }

    inline double operator[](unsigned i) const {
        return _pt[i];
    }

    inline double &operator[](unsigned i) {
        return _pt[i];
    }

    //double operator[](size_t d) const throw() { return _pt[d]; }
    //double &operator[](size_t d) throw() { return _pt[d]; }

    /** Return a point like this point but rotated -90 degrees.
        (If the y axis grows downwards and the x axis grows to the
        right, then this is 90 degrees counter-clockwise.)
    **/
    NR_Point ccw() const {
        return NR_Point(_pt[1], -_pt[0]);
    }

    /** Return a point like this point but rotated +90 degrees.
        (If the y axis grows downwards and the x axis grows to the
        right, then this is 90 degrees clockwise.)
    **/
    NR_Point cw() const {
        return NR_Point(-_pt[1], _pt[0]);
    }

    void normalize();

    inline NR_Point &operator+=(NR_Point const &o) {
        for ( unsigned i = 0 ; i < 2 ; ++i ) {
            _pt[i] += o._pt[i];
        }
        return *this;
    }
  
    inline NR_Point &operator-=(NR_Point const &o) {
        for ( unsigned i = 0 ; i < 2 ; ++i ) {
            _pt[i] -= o._pt[i];
        }
        return *this;
    }
  
    inline NR_Point &operator/=(double const s) {
        for ( unsigned i = 0 ; i < 2 ; ++i ) {
            _pt[i] /= s;
        }
        return *this;
    }

    inline NR_Point &operator*=(double const s) {
        for ( unsigned i = 0 ; i < 2 ; ++i ) {
            _pt[i] *= s;
        }
        return *this;
    }

    //inline bool operator == (const NR_Point &in_pnt) const {
    //    return ((_pt[0] == in_pnt[0]) && (_pt[1] == in_pnt[1]));
    //}

};

inline NR_Point rot90(NR_Point const &p)
{
    return NR_Point(-p[1], p[0]);
}


inline NR_Point operator+(NR_Point const &a, NR_Point const &b)
{
    NR_Point ret;
    for (int i = 0; i < 2; i++) {
        ret[i] = a[i] + b[i];
    }
    return ret;
}

inline double dot(NR_Point const &a, NR_Point const &b)
{
    double ret = 0;
    for ( int i = 0 ; i < 2 ; i++ ) {
        ret += a[i] * b[i];
    }
    return ret;
}


void NR_Point::normalize() {
	double len = hypot(_pt[0], _pt[1]);
	g_return_if_fail(len != 0);
	g_return_if_fail(!isNaN(len));
	static double const inf = 1e300;
	if(len != inf) {
		*this /= len;
	} else {
		unsigned n_inf_coords = 0;
		/* Delay updating pt in case neither coord is infinite. */
		NR_Point tmp;
		for ( unsigned i = 0 ; i < 2 ; ++i ) {
			if ( _pt[i] == inf ) {
				++n_inf_coords;
				tmp[i] = 1.0;
			} else if ( _pt[i] == -inf ) {
				++n_inf_coords;
				tmp[i] = -1.0;
			} else {
				tmp[i] = 0.0;
			}
		}
		switch (n_inf_coords) {
		case 0:
			/* Can happen if both coords are near +/-DBL_MAX. */
			*this /= 4.0;
			len = hypot(_pt[0], _pt[1]);
			g_assert(len != inf);
			*this /= len;
			break;

		case 1:
			*this = tmp;
			break;

		case 2:
			*this = NR_Point(sqrt(0.5) * tmp[0], sqrt(0.5) * tmp[1]);
			break;
		}
	}
}

inline NR_Point operator-(NR_Point const &a, NR_Point const &b)
{
    NR_Point ret;
    for (int i = 0; i < 2; i++) {
        ret[i] = a[i] - b[i];
    }
    return ret;
}

/** This is a rotation (sort of). */
inline NR_Point operator^(NR_Point const &a, NR_Point const &b)
{
    NR_Point const ret(a[0] * b[0] - a[1] * b[1],
                    a[1] * b[0] + a[0] * b[1]);
    return ret;
}

inline NR_Point operator-(NR_Point const &a)
{
    NR_Point ret;
    for(unsigned i = 0; i < 2; i++) {
        ret[i] = -a[i];
    }
    return ret;
}

inline NR_Point operator*(double const s, NR_Point const &b)
{
    NR_Point ret;
    for(int i = 0; i < 2; i++) {
        ret[i] = s * b[i];
    }
    return ret;
}

inline NR_Point operator/(NR_Point const &b, double const d)
{
    NR_Point ret;
    for(int i = 0; i < 2; i++) {
        ret[i] = b[i] / d;
    }
    return ret;
}


inline bool operator==(NR_Point const &a, NR_Point const &b)
{
    return ( ( a[0] == b[0] ) && ( a[1] == b[1] ) );
}

inline bool operator!=(NR_Point const &a, NR_Point const &b)
{
    return ( ( a[0] != b[0] ) || ( a[1] != b[1] ) );
}

inline double L2(NR_Point const &p) {
	return hypot(p[0], p[1]);
}

bool is_zero(NR_Point const &p)
{
    return ( p[0] == 0 &&
             p[1] == 0   );
}

NR_Point unit_vector(NR_Point const &a)
{
    NR_Point ret(a);
    ret.normalize();
    return ret;
}



NR_Point bezier_pt(unsigned degree, NR_Point const V[], double t);

long sp_bezier_fit_cubic(NR_Point bezier[], NR_Point const data[], long len, double error);

long sp_bezier_fit_cubic_r(NR_Point bezier[], NR_Point const data[], long len, double error,
                           unsigned max_beziers);

long sp_bezier_fit_cubic_full(NR_Point bezier[], int split_points[], NR_Point const data[], long len,
                              NR_Point const &tHat1, NR_Point const &tHat2,
                              double error, unsigned max_beziers);

NR_Point sp_darray_left_tangent(NR_Point const d[], unsigned const len);
NR_Point sp_darray_left_tangent(NR_Point const d[], unsigned const len, double const tolerance_sq);
NR_Point sp_darray_right_tangent(NR_Point const d[], unsigned const length, double const tolerance_sq);

/*
 * Temporary fix for various misdefinitions of isnan().
 * isnan() is becoming undef'd in some .h files. 
 * #include this last in your .cpp file to get it right.
 *
 * The problem is that isnan and isfinite are part of C99 but aren't part of
 * the C++ standard (which predates C99).
 *
 * Authors:
 *   Inkscape groupies and obsessive-compulsives
 *
 * Copyright (C) 2004 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 *
 * 2005 modification hereby placed in public domain.  Probably supercedes the 2004 copyright
 * for the code itself.
 */


typedef NR_Point BezierCurve[];

/* Forward declarations */
static void generate_bezier(NR_Point b[], NR_Point const d[], double const u[], unsigned len,
                            NR_Point const &tHat1, NR_Point const &tHat2, double tolerance_sq);
static void estimate_lengths(NR_Point bezier[],
                             NR_Point const data[], double const u[], unsigned len,
                             NR_Point const &tHat1, NR_Point const &tHat2);
static void estimate_bi(NR_Point b[4], unsigned ei,
                        NR_Point const data[], double const u[], unsigned len);
static void reparameterize(NR_Point const d[], unsigned len, double u[], BezierCurve const bezCurve);
static double NewtonRaphsonRootFind(BezierCurve const Q, NR_Point const &P, double u);
static NR_Point sp_darray_center_tangent(NR_Point const d[], unsigned center, unsigned length);
static NR_Point sp_darray_right_tangent(NR_Point const d[], unsigned const len);
static unsigned copy_without_nans_or_adjacent_duplicates(NR_Point const src[], unsigned src_len, NR_Point dest[]);
static void chord_length_parameterize(NR_Point const d[], double u[], unsigned len);
static double compute_max_error_ratio(NR_Point const d[], double const u[], unsigned len,
                                      BezierCurve const bezCurve, double tolerance,
                                      unsigned *splitPoint);
static double compute_hook(NR_Point const &a, NR_Point const &b, double const u, BezierCurve const bezCurve,
                           double const tolerance);


static NR_Point const unconstrained_tangent(0, 0);


/*
 *  B0, B1, B2, B3 : Bezier multipliers
 */

#define B0(u) ( ( 1.0 - u )  *  ( 1.0 - u )  *  ( 1.0 - u ) )
#define B1(u) ( 3 * u  *  ( 1.0 - u )  *  ( 1.0 - u ) )
#define B2(u) ( 3 * u * u  *  ( 1.0 - u ) )
#define B3(u) ( u * u * u )

#ifdef BEZIER_DEBUG
# define DOUBLE_ASSERT(x) g_assert( ( (x) > -SP_HUGE ) && ( (x) < SP_HUGE ) )
# define BEZIER_ASSERT(b) do { \
           DOUBLE_ASSERT((b)[0][0]); DOUBLE_ASSERT((b)[0][1]);  \
           DOUBLE_ASSERT((b)[1][0]); DOUBLE_ASSERT((b)[1][1]);  \
           DOUBLE_ASSERT((b)[2][0]); DOUBLE_ASSERT((b)[2][1]);  \
           DOUBLE_ASSERT((b)[3][0]); DOUBLE_ASSERT((b)[3][1]);  \
         } while(0)
#else
# define DOUBLE_ASSERT(x) do { } while(0)
# define BEZIER_ASSERT(b) do { } while(0)
#endif


/**
 * Fit a single-segment Bezier curve to a set of digitized points.
 *
 * \return Number of segments generated, or -1 on error.
 */
long
sp_bezier_fit_cubic(NR_Point *bezier, NR_Point const *data, long len, double error)
{
    return sp_bezier_fit_cubic_r(bezier, data, len, error, 1);
}

/**
 * Fit a multi-segment Bezier curve to a set of digitized points, with
 * possible weedout of identical points and NaNs.
 *
 * \param max_beziers Maximum number of generated segments
 * \param Result array, must be large enough for n. segments * 4 elements.
 *
 * \return Number of segments generated, or -1 on error.
 */
long
sp_bezier_fit_cubic_r(NR_Point bezier[], NR_Point const data[], long const len, double const error, unsigned const max_beziers)
{
    g_return_val_if_fail(bezier != NULL, -1);
    g_return_val_if_fail(data != NULL, -1);
    g_return_val_if_fail(len > 0, -1);
    g_return_val_if_fail(max_beziers < (1ul << (31 - 2 - 1 - 3)), -1);

    NR_Point *uniqued_data = g_new(NR_Point, len);
    unsigned uniqued_len = copy_without_nans_or_adjacent_duplicates(data, len, uniqued_data);

    if ( uniqued_len < 2 ) {
        g_free(uniqued_data);
        return 0;
    }

    /* Call fit-cubic function with recursion. */
    long const ret = sp_bezier_fit_cubic_full(bezier, NULL, uniqued_data, uniqued_len,
                                              unconstrained_tangent, unconstrained_tangent,
                                              error, max_beziers);
    g_free(uniqued_data);
    return ret;
}

/** 
 * Copy points from src to dest, filter out points containing NaN and
 * adjacent points with equal x and y.
 * \return length of dest
 */
static unsigned
copy_without_nans_or_adjacent_duplicates(NR_Point const src[], unsigned src_len, NR_Point dest[])
{
    unsigned si = 0;
    for (;;) {
        if ( si == src_len ) {
            return 0;
        }
        if (!isNaN(src[si][0]) &&
            !isNaN(src[si][1])) {
            dest[0] = NR_Point(src[si]);
            ++si;
            break;
        }
    }
    unsigned di = 0;
    for (; si < src_len; ++si) {
        NR_Point const src_pt = NR_Point(src[si]);
        if ( src_pt != dest[di]
             && !isNaN(src_pt[0])
             && !isNaN(src_pt[1])) {
            dest[++di] = src_pt;
        }
    }
    unsigned dest_len = di + 1;
    g_assert( dest_len <= src_len );
    return dest_len;
}

/**
 * Fit a multi-segment Bezier curve to a set of digitized points, without
 * possible weedout of identical points and NaNs.
 * 
 * \pre data is uniqued, i.e. not exist i: data[i] == data[i + 1].
 * \param max_beziers Maximum number of generated segments
 * \param Result array, must be large enough for n. segments * 4 elements.
 */
long
sp_bezier_fit_cubic_full(NR_Point bezier[], int split_points[],
                         NR_Point const data[], long const len,
                         NR_Point const &tHat1, NR_Point const &tHat2,
                         double const error, unsigned const max_beziers)
{
    int const maxIterations = 4;   /* Max times to try iterating */

    g_return_val_if_fail(bezier != NULL, -1);
    g_return_val_if_fail(data != NULL, -1);
    g_return_val_if_fail(len > 0, -1);
    g_return_val_if_fail(max_beziers >= 1, -1);
    g_return_val_if_fail(error >= 0.0, -1);

    if ( len < 2 ) return 0;

    if ( len == 2 ) {
        /* We have 2 points, which can be fitted trivially. */
        bezier[0] = data[0];
        bezier[3] = data[len - 1];
        double const dist = ( L2( data[len - 1]
                                  - data[0] )
                              / 3.0 );
        if (isNaN(dist)) {
            /* Numerical problem, fall back to straight line segment. */
            bezier[1] = bezier[0];
            bezier[2] = bezier[3];
        } else {
            bezier[1] = ( is_zero(tHat1)
                          ? ( 2 * bezier[0] + bezier[3] ) / 3.
                          : bezier[0] + dist * tHat1 );
            bezier[2] = ( is_zero(tHat2)
                          ? ( bezier[0] + 2 * bezier[3] ) / 3.
                          : bezier[3] + dist * tHat2 );
        }
        BEZIER_ASSERT(bezier);
        return 1;
    }

    /*  Parameterize points, and attempt to fit curve */
    unsigned splitPoint;   /* Point to split point set at. */
    bool is_corner;
    {
        double *u = g_new(double, len);
        chord_length_parameterize(data, u, len);
        if ( u[len - 1] == 0.0 ) {
            /* Zero-length path: every point in data[] is the same.
             *
             * (Clients aren't allowed to pass such data; handling the case is defensive
             * programming.)
             */
            g_free(u);
            return 0;
        }

        generate_bezier(bezier, data, u, len, tHat1, tHat2, error);
        reparameterize(data, len, u, bezier);

        /* Find max deviation of points to fitted curve. */
        double const tolerance = sqrt(error + 1e-9);
        double maxErrorRatio = compute_max_error_ratio(data, u, len, bezier, tolerance, &splitPoint);

        if ( fabs(maxErrorRatio) <= 1.0 ) {
            BEZIER_ASSERT(bezier);
            g_free(u);
            return 1;
        }

        /* If error not too large, then try some reparameterization and iteration. */
        if ( 0.0 <= maxErrorRatio && maxErrorRatio <= 3.0 ) {
            for (int i = 0; i < maxIterations; i++) {
                generate_bezier(bezier, data, u, len, tHat1, tHat2, error);
                reparameterize(data, len, u, bezier);
                maxErrorRatio = compute_max_error_ratio(data, u, len, bezier, tolerance, &splitPoint);
                if ( fabs(maxErrorRatio) <= 1.0 ) {
                    BEZIER_ASSERT(bezier);
                    g_free(u);
                    return 1;
                }
            }
        }
        g_free(u);
        is_corner = (maxErrorRatio < 0);
    }

    if (is_corner) {
        g_assert(splitPoint < unsigned(len));
        if (splitPoint == 0) {
            if (is_zero(tHat1)) {
                /* Got spike even with unconstrained initial tangent. */
                ++splitPoint;
            } else {
                return sp_bezier_fit_cubic_full(bezier, split_points, data, len, unconstrained_tangent, tHat2,
                                                error, max_beziers);
            }
        } else if (splitPoint == unsigned(len - 1)) {
            if (is_zero(tHat2)) {
                /* Got spike even with unconstrained final tangent. */
                --splitPoint;
            } else {
                return sp_bezier_fit_cubic_full(bezier, split_points, data, len, tHat1, unconstrained_tangent,
                                                error, max_beziers);
            }
        }
    }

    if ( 1 < max_beziers ) {
        /*
         *  Fitting failed -- split at max error point and fit recursively
         */
        unsigned const rec_max_beziers1 = max_beziers - 1;

        NR_Point recTHat2, recTHat1;
        if (is_corner) {
            g_return_val_if_fail(0 < splitPoint && splitPoint < unsigned(len - 1), -1);
            recTHat1 = recTHat2 = unconstrained_tangent;
        } else {
            /* Unit tangent vector at splitPoint. */
            recTHat2 = sp_darray_center_tangent(data, splitPoint, len);
            recTHat1 = -recTHat2;
        }
        long const nsegs1 = sp_bezier_fit_cubic_full(bezier, split_points, data, splitPoint + 1,
                                                     tHat1, recTHat2, error, rec_max_beziers1);
        if ( nsegs1 < 0 ) {
#ifdef BEZIER_DEBUG
            g_print("fit_cubic[1]: recursive call failed\n");
#endif
            return -1;
        }
        g_assert( nsegs1 != 0 );
        if (split_points != NULL) {
            split_points[nsegs1 - 1] = splitPoint;
        }
        unsigned const rec_max_beziers2 = max_beziers - nsegs1;
        long const nsegs2 = sp_bezier_fit_cubic_full(bezier + nsegs1*4,
                                                     ( split_points == NULL
                                                       ? NULL
                                                       : split_points + nsegs1 ),
                                                     data + splitPoint, len - splitPoint,
                                                     recTHat1, tHat2, error, rec_max_beziers2);
        if ( nsegs2 < 0 ) {
#ifdef BEZIER_DEBUG
            g_print("fit_cubic[2]: recursive call failed\n");
#endif
            return -1;
        }

#ifdef BEZIER_DEBUG
        g_print("fit_cubic: success[nsegs: %d+%d=%d] on max_beziers:%u\n",
                nsegs1, nsegs2, nsegs1 + nsegs2, max_beziers);
#endif
        return nsegs1 + nsegs2;
    } else {
        return -1;
    }
}


/**
 * Fill in \a bezier[] based on the given data and tangent requirements, using
 * a least-squares fit.
 *
 * Each of tHat1 and tHat2 should be either a zero vector or a unit vector.
 * If it is zero, then bezier[1 or 2] is estimated without constraint; otherwise,
 * it bezier[1 or 2] is placed in the specified direction from bezier[0 or 3].
 *
 * \param tolerance_sq Used only for an initial guess as to tangent directions
 *   when \a tHat1 or \a tHat2 is zero.
 */
static void
generate_bezier(NR_Point bezier[],
                NR_Point const data[], double const u[], unsigned const len,
                NR_Point const &tHat1, NR_Point const &tHat2,
                double const tolerance_sq)
{
    bool const est1 = is_zero(tHat1);
    bool const est2 = is_zero(tHat2);
    NR_Point est_tHat1( est1
                         ? sp_darray_left_tangent(data, len, tolerance_sq)
                         : tHat1 );
    NR_Point est_tHat2( est2
                         ? sp_darray_right_tangent(data, len, tolerance_sq)
                         : tHat2 );
    estimate_lengths(bezier, data, u, len, est_tHat1, est_tHat2);
    /* We find that sp_darray_right_tangent tends to produce better results
       for our current freehand tool than full estimation. */
    if (est1) {
        estimate_bi(bezier, 1, data, u, len);
        if (bezier[1] != bezier[0]) {
            est_tHat1 = unit_vector(bezier[1] - bezier[0]);
        }
        estimate_lengths(bezier, data, u, len, est_tHat1, est_tHat2);
    }
}


static void
estimate_lengths(NR_Point bezier[],
                 NR_Point const data[], double const uPrime[], unsigned const len,
                 NR_Point const &tHat1, NR_Point const &tHat2)
{
    double C[2][2];   /* Matrix C. */
    double X[2];      /* Matrix X. */

    /* Create the C and X matrices. */
    C[0][0] = 0.0;
    C[0][1] = 0.0;
    C[1][0] = 0.0;
    C[1][1] = 0.0;
    X[0]    = 0.0;
    X[1]    = 0.0;

    /* First and last control points of the Bezier curve are positioned exactly at the first and
       last data points. */
    bezier[0] = data[0];
    bezier[3] = data[len - 1];

    for (unsigned i = 0; i < len; i++) {
        /* Bezier control point coefficients. */
        double const b0 = B0(uPrime[i]);
        double const b1 = B1(uPrime[i]);
        double const b2 = B2(uPrime[i]);
        double const b3 = B3(uPrime[i]);

        /* rhs for eqn */
        NR_Point const a1 = b1 * tHat1;
        NR_Point const a2 = b2 * tHat2;

        C[0][0] += dot(a1, a1);
        C[0][1] += dot(a1, a2);
        C[1][0] = C[0][1];
        C[1][1] += dot(a2, a2);

        /* Additional offset to the data point from the predicted point if we were to set bezier[1]
           to bezier[0] and bezier[2] to bezier[3]. */
        NR_Point const shortfall
            = ( data[i]
                - ( ( b0 + b1 ) * bezier[0] )
                - ( ( b2 + b3 ) * bezier[3] ) );
        X[0] += dot(a1, shortfall);
        X[1] += dot(a2, shortfall);
    }

    /* We've constructed a pair of equations in the form of a matrix product C * alpha = X.
       Now solve for alpha. */
    double alpha_l, alpha_r;

    /* Compute the determinants of C and X. */
    double const det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    if ( det_C0_C1 != 0 ) {
        /* Apparently Kramer's rule. */
        double const det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
        double const det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];
        alpha_l = det_X_C1 / det_C0_C1;
        alpha_r = det_C0_X / det_C0_C1;
    } else {
        /* The matrix is under-determined.  Try requiring alpha_l == alpha_r.
         *
         * One way of implementing the constraint alpha_l == alpha_r is to treat them as the same
         * variable in the equations.  We can do this by adding the columns of C to form a single
         * column, to be multiplied by alpha to give the column vector X.
         *
         * We try each row in turn.
         */
        double const c0 = C[0][0] + C[0][1];
        if (c0 != 0) {
            alpha_l = alpha_r = X[0] / c0;
        } else {
            double const c1 = C[1][0] + C[1][1];
            if (c1 != 0) {
                alpha_l = alpha_r = X[1] / c1;
            } else {
                /* Let the below code handle this. */
                alpha_l = alpha_r = 0.;
            }
        }
    }

    /* If alpha negative, use the Wu/Barsky heuristic (see text).  (If alpha is 0, you get
       coincident control points that lead to divide by zero in any subsequent
       NewtonRaphsonRootFind() call.) */
    /// \todo Check whether this special-casing is necessary now that 
    /// NewtonRaphsonRootFind handles non-positive denominator.
    if ( alpha_l < 1.0e-6 ||
         alpha_r < 1.0e-6   )
    {
        alpha_l = alpha_r = ( L2( data[len - 1]
                                  - data[0] )
                              / 3.0 );
    }

    /* Control points 1 and 2 are positioned an alpha distance out on the tangent vectors, left and
       right, respectively. */
    bezier[1] = alpha_l * tHat1 + bezier[0];
    bezier[2] = alpha_r * tHat2 + bezier[3];

    return;
}

static double lensq(NR_Point const p) {
    return dot(p, p);
}

static void
estimate_bi(NR_Point bezier[4], unsigned const ei,
            NR_Point const data[], double const u[], unsigned const len)
{
    g_return_if_fail(1 <= ei && ei <= 2);
    unsigned const oi = 3 - ei;
    double num[2] = {0., 0.};
    double den = 0.;
    for (unsigned i = 0; i < len; ++i) {
        double const ui = u[i];
        double const b[4] = {
            B0(ui),
            B1(ui),
            B2(ui),
            B3(ui)
        };

        for (unsigned d = 0; d < 2; ++d) {
            num[d] += b[ei] * (b[0]  * bezier[0][d] +
                               b[oi] * bezier[oi][d] +
                               b[3]  * bezier[3][d] +
                               - data[i][d]);
        }
        den -= b[ei] * b[ei];
    }

    if (den != 0.) {
        for (unsigned d = 0; d < 2; ++d) {
            bezier[ei][d] = num[d] / den;
        }
    } else {
        bezier[ei] = ( oi * bezier[0] + ei * bezier[3] ) / 3.;
    }
}

/**
 * Given set of points and their parameterization, try to find a better assignment of parameter
 * values for the points.
 *
 *  \param d  Array of digitized points.
 *  \param u  Current parameter values.
 *  \param bezCurve  Current fitted curve.
 *  \param len  Number of values in both d and u arrays.
 *              Also the size of the array that is allocated for return.
 */
static void
reparameterize(NR_Point const d[],
               unsigned const len,
               double u[],
               BezierCurve const bezCurve)
{
    g_assert( 2 <= len );

    unsigned const last = len - 1;
    g_assert( bezCurve[0] == d[0] );
    g_assert( bezCurve[3] == d[last] );
    g_assert( u[0] == 0.0 );
    g_assert( u[last] == 1.0 );
    /* Otherwise, consider including 0 and last in the below loop. */

    for (unsigned i = 1; i < last; i++) {
        u[i] = NewtonRaphsonRootFind(bezCurve, d[i], u[i]);
    }
}

/**
 *  Use Newton-Raphson iteration to find better root.
 *  
 *  \param Q  Current fitted curve
 *  \param P  Digitized point
 *  \param u  Parameter value for "P"
 *  
 *  \return Improved u
 */
static double
NewtonRaphsonRootFind(BezierCurve const Q, NR_Point const &P, double const u)
{
    g_assert( 0.0 <= u );
    g_assert( u <= 1.0 );

    /* Generate control vertices for Q'. */
    NR_Point Q1[3];
    for (unsigned i = 0; i < 3; i++) {
        Q1[i] = 3.0 * ( Q[i+1] - Q[i] );
    }

    /* Generate control vertices for Q''. */
    NR_Point Q2[2];
    for (unsigned i = 0; i < 2; i++) {
        Q2[i] = 2.0 * ( Q1[i+1] - Q1[i] );
    }

    /* Compute Q(u), Q'(u) and Q''(u). */
    NR_Point const Q_u  = bezier_pt(3, Q, u);
    NR_Point const Q1_u = bezier_pt(2, Q1, u);
    NR_Point const Q2_u = bezier_pt(1, Q2, u);

    /* Compute f(u)/f'(u), where f is the derivative wrt u of distsq(u) = 0.5 * the square of the
       distance from P to Q(u).  Here we're using Newton-Raphson to find a stationary point in the
       distsq(u), hopefully corresponding to a local minimum in distsq (and hence a local minimum
       distance from P to Q(u)). */
    NR_Point const diff = Q_u - P;
    double numerator = dot(diff, Q1_u);
    double denominator = dot(Q1_u, Q1_u) + dot(diff, Q2_u);

    double improved_u;
    if ( denominator > 0. ) {
        /* One iteration of Newton-Raphson:
           improved_u = u - f(u)/f'(u) */
        improved_u = u - ( numerator / denominator );
    } else {
        /* Using Newton-Raphson would move in the wrong direction (towards a local maximum rather
           than local minimum), so we move an arbitrary amount in the right direction. */
        if ( numerator > 0. ) {
            improved_u = u * .98 - .01;
        } else if ( numerator < 0. ) {
            /* Deliberately asymmetrical, to reduce the chance of cycling. */
            improved_u = .031 + u * .98;
        } else {
            improved_u = u;
        }
    }

    if (!isFinite(improved_u)) {
        improved_u = u;
    } else if ( improved_u < 0.0 ) {
        improved_u = 0.0;
    } else if ( improved_u > 1.0 ) {
        improved_u = 1.0;
    }

    /* Ensure that improved_u isn't actually worse. */
    {
        double const diff_lensq = lensq(diff);
        for (double proportion = .125; ; proportion += .125) {
            if ( lensq( bezier_pt(3, Q, improved_u) - P ) > diff_lensq ) {
                if ( proportion > 1.0 ) {
                    //g_warning("found proportion %g", proportion);
                    improved_u = u;
                    break;
                }
                improved_u = ( ( 1 - proportion ) * improved_u  +
                               proportion         * u            );
            } else {
                break;
            }
        }
    }

    DOUBLE_ASSERT(improved_u);
    return improved_u;
}

/** 
 * Evaluate a Bezier curve at parameter value \a t.
 * 
 * \param degree The degree of the Bezier curve: 3 for cubic, 2 for quadratic etc.
 * \param V The control points for the Bezier curve.  Must have (\a degree+1)
 *    elements.
 * \param t The "parameter" value, specifying whereabouts along the curve to
 *    evaluate.  Typically in the range [0.0, 1.0].
 *
 * Let s = 1 - t.
 * BezierII(1, V) gives (s, t) * V, i.e. t of the way
 * from V[0] to V[1].
 * BezierII(2, V) gives (s**2, 2*s*t, t**2) * V.
 * BezierII(3, V) gives (s**3, 3 s**2 t, 3s t**2, t**3) * V.
 *
 * The derivative of BezierII(i, V) with respect to t
 * is i * BezierII(i-1, V'), where for all j, V'[j] =
 * V[j + 1] - V[j].
 */
NR_Point
bezier_pt(unsigned const degree, NR_Point const V[], double const t)
{
    /** Pascal's triangle. */
    static int const pascal[4][4] = {{1},
                                     {1, 1},
                                     {1, 2, 1},
                                     {1, 3, 3, 1}};
    //g_assert( degree < G_N_ELEMENTS(pascal) );
    g_assert( degree < 5 );
    double const s = 1.0 - t;

    /* Calculate powers of t and s. */
    double spow[4];
    double tpow[4];
    spow[0] = 1.0; spow[1] = s;
    tpow[0] = 1.0; tpow[1] = t;
    for (unsigned i = 1; i < degree; ++i) {
        spow[i + 1] = spow[i] * s;
        tpow[i + 1] = tpow[i] * t;
    }

    NR_Point ret = spow[degree] * V[0];
    for (unsigned i = 1; i <= degree; ++i) {
        ret += pascal[degree][i] * spow[degree - i] * tpow[i] * V[i];
    }
    return ret;
}

/*
 * ComputeLeftTangent, ComputeRightTangent, ComputeCenterTangent :
 * Approximate unit tangents at endpoints and "center" of digitized curve
 */

/** 
 * Estimate the (forward) tangent at point d[first + 0.5].
 *
 * Unlike the center and right versions, this calculates the tangent in 
 * the way one might expect, i.e., wrt increasing index into d.
 * \pre (2 \<= len) and (d[0] != d[1]).
 **/
NR_Point
sp_darray_left_tangent(NR_Point const d[], unsigned const len)
{
    g_assert( len >= 2 );
    g_assert( d[0] != d[1] );
    return unit_vector( d[1] - d[0] );
}

/** 
 * Estimates the (backward) tangent at d[last - 0.5].
 *
 * \note The tangent is "backwards", i.e. it is with respect to 
 * decreasing index rather than increasing index.
 *
 * \pre 2 \<= len.
 * \pre d[len - 1] != d[len - 2].
 * \pre all[p in d] in_svg_plane(p).
 */
static NR_Point
sp_darray_right_tangent(NR_Point const d[], unsigned const len)
{
    g_assert( 2 <= len );
    unsigned const last = len - 1;
    unsigned const prev = last - 1;
    g_assert( d[last] != d[prev] );
    return unit_vector( d[prev] - d[last] );
}

/** 
 * Estimate the (forward) tangent at point d[0].
 *
 * Unlike the center and right versions, this calculates the tangent in 
 * the way one might expect, i.e., wrt increasing index into d.
 *
 * \pre 2 \<= len.
 * \pre d[0] != d[1].
 * \pre all[p in d] in_svg_plane(p).
 * \post is_unit_vector(ret).
 **/
NR_Point
sp_darray_left_tangent(NR_Point const d[], unsigned const len, double const tolerance_sq)
{
    g_assert( 2 <= len );
    g_assert( 0 <= tolerance_sq );
    for (unsigned i = 1;;) {
        NR_Point const pi(d[i]);
        NR_Point const t(pi - d[0]);
        double const distsq = dot(t, t);
        if ( tolerance_sq < distsq ) {
            return unit_vector(t);
        }
        ++i;
        if (i == len) {
            return ( distsq == 0
                     ? sp_darray_left_tangent(d, len)
                     : unit_vector(t) );
        }
    }
}

/** 
 * Estimates the (backward) tangent at d[last].
 *
 * \note The tangent is "backwards", i.e. it is with respect to 
 * decreasing index rather than increasing index.
 *
 * \pre 2 \<= len.
 * \pre d[len - 1] != d[len - 2].
 * \pre all[p in d] in_svg_plane(p).
 */
NR_Point
sp_darray_right_tangent(NR_Point const d[], unsigned const len, double const tolerance_sq)
{
    g_assert( 2 <= len );
    g_assert( 0 <= tolerance_sq );
    unsigned const last = len - 1;
    for (unsigned i = last - 1;; i--) {
        NR_Point const pi(d[i]);
        NR_Point const t(pi - d[last]);
        double const distsq = dot(t, t);
        if ( tolerance_sq < distsq ) {
            return unit_vector(t);
        }
        if (i == 0) {
            return ( distsq == 0
                     ? sp_darray_right_tangent(d, len)
                     : unit_vector(t) );
        }
    }
}

/** 
 * Estimates the (backward) tangent at d[center], by averaging the two 
 * segments connected to d[center] (and then normalizing the result).
 *
 * \note The tangent is "backwards", i.e. it is with respect to 
 * decreasing index rather than increasing index.
 *
 * \pre (0 \< center \< len - 1) and d is uniqued (at least in 
 * the immediate vicinity of \a center).
 */
static NR_Point
sp_darray_center_tangent(NR_Point const d[],
                         unsigned const center,
                         unsigned const len)
{
    g_assert( center != 0 );
    g_assert( center < len - 1 );

    NR_Point ret;
    if ( d[center + 1] == d[center - 1] ) {
        /* Rotate 90 degrees in an arbitrary direction. */
        NR_Point const diff = d[center] - d[center - 1];
        ret = rot90(diff);
    } else {
        ret = d[center - 1] - d[center + 1];
    }
    ret.normalize();
    return ret;
}


/**
 *  Assign parameter values to digitized points using relative distances between points.
 *
 *  \pre Parameter array u must have space for \a len items.
 */
static void
chord_length_parameterize(NR_Point const d[], double u[], unsigned const len)
{
    g_return_if_fail( 2 <= len );

    /* First let u[i] equal the distance travelled along the path from d[0] to d[i]. */
    u[0] = 0.0;
    for (unsigned i = 1; i < len; i++) {
        double const dist = L2( d[i] - d[i-1] );
        u[i] = u[i-1] + dist;
    }

    /* Then scale to [0.0 .. 1.0]. */
    double tot_len = u[len - 1];
    g_return_if_fail( tot_len != 0 );
    if (isFinite(tot_len)) {
        for (unsigned i = 1; i < len; ++i) {
            u[i] /= tot_len;
        }
    } else {
        /* We could do better, but this probably never happens anyway. */
        for (unsigned i = 1; i < len; ++i) {
            u[i] = i / (double) ( len - 1 );
        }
    }

    /** \todo
     * It's been reported that u[len - 1] can differ from 1.0 on some 
     * systems (amd64), despite it having been calculated as x / x where x 
     * is isFinite and non-zero.
     */
    if (u[len - 1] != 1) {
        double const diff = u[len - 1] - 1;
        if (fabs(diff) > 1e-13) {
            fprintf(stderr,"u[len - 1] = %19g (= 1 + %19g), expecting exactly 1",
                      u[len - 1], diff);
        }
        u[len - 1] = 1;
    }

#ifdef BEZIER_DEBUG
    g_assert( u[0] == 0.0 && u[len - 1] == 1.0 );
    for (unsigned i = 1; i < len; i++) {
        g_assert( u[i] >= u[i-1] );
    }
#endif
}




/**
 * Find the maximum squared distance of digitized points to fitted curve, and (if this maximum
 * error is non-zero) set \a *splitPoint to the corresponding index.
 *
 * \pre 2 \<= len.
 * \pre u[0] == 0.
 * \pre u[len - 1] == 1.0.
 * \post ((ret == 0.0)
 *        || ((*splitPoint \< len - 1)
 *            \&\& (*splitPoint != 0 || ret \< 0.0))).
 */
static double
compute_max_error_ratio(NR_Point const d[], double const u[], unsigned const len,
                        BezierCurve const bezCurve, double const tolerance,
                        unsigned *const splitPoint)
{
    g_assert( 2 <= len );
    unsigned const last = len - 1;
    g_assert( bezCurve[0] == d[0] );
    g_assert( bezCurve[3] == d[last] );
    g_assert( u[0] == 0.0 );
    g_assert( u[last] == 1.0 );
    /* I.e. assert that the error for the first & last points is zero.
     * Otherwise we should include those points in the below loop.
     * The assertion is also necessary to ensure 0 < splitPoint < last.
     */

    double maxDistsq = 0.0; /* Maximum error */
    double max_hook_ratio = 0.0;
    unsigned snap_end = 0;
    NR_Point prev = bezCurve[0];
    for (unsigned i = 1; i <= last; i++) {
        NR_Point const curr = bezier_pt(3, bezCurve, u[i]);
        double const distsq = lensq( curr - d[i] );
        if ( distsq > maxDistsq ) {
            maxDistsq = distsq;
            *splitPoint = i;
        }
        double const hook_ratio = compute_hook(prev, curr, .5 * (u[i - 1] + u[i]), bezCurve, tolerance);
        if (max_hook_ratio < hook_ratio) {
            max_hook_ratio = hook_ratio;
            snap_end = i;
        }
        prev = curr;
    }

    double const dist_ratio = sqrt(maxDistsq) / tolerance;
    double ret;
    if (max_hook_ratio <= dist_ratio) {
        ret = dist_ratio;
    } else {
        g_assert(0 < snap_end);
        ret = -max_hook_ratio;
        *splitPoint = snap_end - 1;
    }
    g_assert( ret == 0.0
              || ( ( *splitPoint < last )
                   && ( *splitPoint != 0 || ret < 0. ) ) );
    return ret;
}

/** 
 * Whereas compute_max_error_ratio() checks for itself that each data point 
 * is near some point on the curve, this function checks that each point on 
 * the curve is near some data point (or near some point on the polyline 
 * defined by the data points, or something like that: we allow for a
 * "reasonable curviness" from such a polyline).  "Reasonable curviness" 
 * means we draw a circle centred at the midpoint of a..b, of radius 
 * proportional to the length |a - b|, and require that each point on the 
 * segment of bezCurve between the parameters of a and b be within that circle.
 * If any point P on the bezCurve segment is outside of that allowable 
 * region (circle), then we return some metric that increases with the 
 * distance from P to the circle.
 *
 *  Given that this is a fairly arbitrary criterion for finding appropriate 
 *  places for sharp corners, we test only one point on bezCurve, namely 
 *  the point on bezCurve with parameter halfway between our estimated 
 *  parameters for a and b.  (Alternatives are taking the farthest of a
 *  few parameters between those of a and b, or even using a variant of 
 *  NewtonRaphsonFindRoot() for finding the maximum rather than minimum 
 *  distance.)
 */
static double
compute_hook(NR_Point const &a, NR_Point const &b, double const u, BezierCurve const bezCurve,
             double const tolerance)
{
    NR_Point const P = bezier_pt(3, bezCurve, u);
    NR_Point const diff = .5 * (a + b) - P;
    double const dist = L2(diff);
    if (dist < tolerance) {
        return 0;
    }
    double const allowed = L2(b - a) + tolerance;
    return dist / allowed;
    /** \todo 
     * effic: Hooks are very rare.  We could start by comparing 
     * distsq, only resorting to the more expensive L2 in cases of 
     * uncertainty.
     */
}









void thbezier_curve::clear()
{
  this->m_segments.clear();
  this->m_current_segment = this->m_segments.end();
}


thbezier_segment * thbezier_curve::insert_segment()
{
  return &(*this->m_segments.insert(this->m_segments.end(), thbezier_segment()));
}


thbezier_segment * thbezier_curve::get_first_segment()
{
  this->m_current_segment = this->m_segments.begin();
  return this->get_current_segment();
}
  
thbezier_segment * thbezier_curve::get_current_segment()
{
  if (this->m_current_segment != this->m_segments.end())
    return &(*this->m_current_segment);
  else
    return NULL;
}
  
thbezier_segment * thbezier_curve::get_next_segment()
{
  this->m_current_segment++;
  return this->get_current_segment();
}


void thbezier_curve::copy_polyline(struct thbezier_polyline * line, double err)
{
  this->clear();
  size_t i, max = line->get_length();
  long nbp;
  if (max < 3)
    return;

  NR_Point * poly, * bezier;
  poly = new NR_Point[max];
  bezier = new NR_Point[4 * max];
  thbezier_point * p;
  for(i = 0, p = line->get_first_point(); p != NULL; p = line->get_next_point(), i++) {
    poly[i][0] = p->m_x;
    poly[i][1] = p->m_y;
  }
  NR_Point stan(0, 0), etan(0,0);
  if ((poly[0][0] == poly[max-1][0]) && (poly[0][1] == poly[max-1][1])) {
    stan = NR_Point(poly[1][0] - poly[0][0], poly[1][1] - poly[0][1]);
    stan.normalize();
    etan = NR_Point(poly[max-1][0] - poly[max-2][0], poly[max-1][1] - poly[max-2][1]);
    etan.normalize();
    stan += etan;
    if (is_zero(stan)) stan = etan;
    else stan.normalize();
  }
  //nbp = (long) sp_bezier_fit_cubic_r(bezier, poly, (long) max, err, (unsigned int)max);
  nbp = (long) sp_bezier_fit_cubic_full(bezier, NULL, poly, (long) max, stan, -stan, err, (unsigned int)max);
  thbezier_segment * s;
  s = this->insert_segment();
  s->m_p = thbezier_point(bezier[0][0], bezier[0][1]);
  for(i = 0; i < (size_t) nbp; i++) {
    s = this->insert_segment();
    s->m_cp1 = thbezier_point(bezier[4*i+1][0],bezier[4*i+1][1]);
    s->m_cp2 = thbezier_point(bezier[4*i+2][0],bezier[4*i+2][1]);
    s->m_p = thbezier_point(bezier[4*i+3][0],bezier[4*i+3][1]);
  }

}


void thbezier_polyline::clear()
{
  this->m_points.clear();
  this->m_current_point = this->m_points.end();
}


thbezier_point * thbezier_polyline::insert_point()
{
  return &(*this->m_points.insert(this->m_points.end(), thbezier_point()));
}


thbezier_point * thbezier_polyline::get_first_point()
{
  this->m_current_point = this->m_points.begin();
  return this->get_current_point();
}


thbezier_point * thbezier_polyline::get_current_point()
{
  if (this->m_current_point != this->m_points.end())
    return &(*this->m_current_point);
  else
    return NULL;
}


thbezier_point * thbezier_polyline::get_next_point()
{
  this->m_current_point++;
  return this->get_current_point();
}


void thbezier_polyline::copy_curve(struct thbezier_curve * curve, double err)
{
  this->clear();

  // TODO: bezier 2 polyline conversion
}


void thbezier_main() {

  FILE * fin, * fout;

  fin = fopen("therion.bci","r");
  double x, y, px(0.0), py(0.0);
  bool xfirst(true);
  thbezier_polyline line;
  thbezier_curve curve;
  thbezier_point * pt;
  while (fscanf(fin,"%lf%lf",&x,&y) == 2) {
    if ((xfirst) || (x != px) || (y != py)) {
      pt = line.insert_point();
      pt->m_x = px = x;
      pt->m_y = py = y;
      pt->m_valid = true;
    }
    xfirst = false;
  }
  fclose(fin);
  if (line.m_points.size() < 3) return;
  
  curve.copy_polyline(&line, 2.0);
  thbezier_segment *s;
  fout = fopen("therion.bco","w");
  s = curve.get_first_segment();
  fprintf(fout, "%.2lf %.2lf\n", s->m_p.m_x, s->m_p.m_y);
  for(s = curve.get_next_segment(); s != NULL; s = curve.get_next_segment()) {
    fprintf(fout, "%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n",s->m_cp1.m_x, s->m_cp1.m_y, s->m_cp2.m_x, s->m_cp2.m_y, s->m_p.m_x, s->m_p.m_y);
  }
  fclose(fout);

}
