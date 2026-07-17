/**
 * @file thinfnan.h
 * Therion number constants.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#pragma once

#include <cmath>
#include <cfloat>


// nan handling
#ifdef NAN

#define thnan NAN
#define thisnan std::isnan

#else

#define thnan -9e99
#define thisnan(number) (number == thnan)

#endif


// infinity handling
#ifdef INFINITY

#ifdef THLINUX
#define thinf INFINITY
// Linux C isinf() returns -1/0/1, but C++11 isinf() returns bool.
#define thisinf(number) (std::isinf(number) ? (number < 0 ? -1 : 1) : 0)
#else
#define thinf 1e100
#define thisinf(number) (number >= thinf ? 1 : (number <= -thinf ? -1 : 0))
#endif

#else

#define thinf 1e100
#define thisinf(number) (number >= thinf ? 1 : (number <= -thinf ? -1 : 0))

#endif

/**
 * Update double variable if nan.
 *
 * @param oval Original value
 * @param uval Update value
 */
 
void thnan_update(double & oval, double uval);


/**
 * A inf nan printing macro.
 *
 * -Inf -> -999.999 
 *  Inf ->  999.999
 *  NaN -> 1000.0001
 */
 
#define thinn(cislo) (thisnan(cislo) ? 1000.0001 : \
    (thisinf(cislo) == 1 ? 999.999 : \
    (thisinf(cislo) == -1 ? -999.999 : cislo)))


bool thapproximately_equal(double a, double b, double epsilon);
bool thessentially_equal(double a, double b, double epsilon);
bool thdefinitely_greater_than(double a, double b, double epsilon);
bool thdefinitely_less_than(double a, double b, double epsilon);

inline constexpr double THPI = 3.1415926535898;

double thnanpow2(double number);
double thdxyz2length(double dx, double dy, double dz);
double thdxyz2b(double dx, double dy);
double thdxyz2bearing(double dx, double dy);
double thdxyz2clino(double dx, double dy, double dz);
