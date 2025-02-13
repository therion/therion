/**
 * @file thtf.cxx
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
 
#include "thtf.h"
#include "thexception.h"
#include "thparse.h"


void thtf::parse_scale(char * sstr)
{
  int sv;
  thparse_double(sv,this->sfactor,sstr);
  if (sv != TT_SV_NUMBER)
    throw thexception(fmt::format("invalid scale factor -- {}", sstr));
  if (this->sfactor == 0.0)
    throw thexception("invalid scale factor -- 0.0");
}
  
double thtf::transform(double value)
{
  return value * this->sfactor * this->ufactor;
}


int thtf::get_units() {
  return this->units;
}




