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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
    ththrow("invalid scale factor -- {}", sstr);
  if (this->sfactor == 0.0)
    ththrow("invalid scale factor -- 0.0");
}
  
double thtf::transform(double value)
{
  return value * this->sfactor * this->ufactor;
}


int thtf::get_units() {
  return this->units;
}




