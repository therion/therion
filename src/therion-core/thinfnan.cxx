/**
 * @file thinfnan.cxx
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
 
#include "thinfnan.h"

void thnan_update(double & oval, double uval)
{
  if (thisnan(oval))
    oval = uval;
}

bool thapproximately_equal(double a, double b, double epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool thessentially_equal(double a, double b, double epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool thdefinitely_greater_than(double a, double b, double epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool thdefinitely_less_than(double a, double b, double epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

double thnanpow2(double number)
{
    return (thisnan(number) ? 0.0 : number) * (thisnan(number) ? 0.0 : number);
}

double thdxyz2length(double dx, double dy, double dz)
{
    return sqrt(thnanpow2(dx) + thnanpow2(dy) + thnanpow2(dz));
}

double thdxyz2b(double dx, double dy)
{
    return 270 - (atan2(dy,dx) / THPI * 180.0 + 180);
}

double thdxyz2bearing(double dx, double dy)
{
    return thdxyz2b(dx,dy) < 0.0 ? thdxyz2b(dx,dy) + 360.0 : thdxyz2b(dx,dy);
}

double thdxyz2clino(double dx, double dy, double dz)
{
    return atan2(dz,sqrt(thnanpow2(dx) + thnanpow2(dy))) / THPI * 180.0;
}
