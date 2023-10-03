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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#ifndef thinfnan_h
#define thinfnan_h

#include <cmath>


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


// infnan.h
#endif

/**
 * Print number in nan format.
 */

#define thprintinfnan(cislo) {\
  if (thisnan(cislo)) \
    thprintf("thnan"); \
  else if (thisinf(cislo) == 1) \
    thprintf("thinf"); \
  else if (thisinf(cislo) == -1) \
    thprintf("-thinf"); \
  else \
    thprintf("%lg",cislo);}


#define THPI 3.1415926535898
#define thnanpow2(cislo) ((thisnan(cislo) ? 0.0 : cislo) * (thisnan(cislo) ? 0.0 : cislo))
#define thdxyz2length(dx,dy,dz) (sqrt(thnanpow2(dx) + thnanpow2(dy) + thnanpow2(dz)))
#define thdxyz2b(dx,dy,dz) (270 - (atan2(dy,dx) / THPI * 180.0 + 180))
#define thdxyz2bearing(dx,dy,dz) (thdxyz2b(dx,dy,dz) < 0.0 ? thdxyz2b(dx,dy,dz) + 360.0 : thdxyz2b(dx,dy,dz))
#define thdxyz2clino(dx,dy,dz) (atan2(dz,sqrt(thnanpow2(dx) + thnanpow2(dy))) / THPI * 180.0)
