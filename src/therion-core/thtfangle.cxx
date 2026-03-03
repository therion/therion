/**
 * @file thtfangle.cxx
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
 
#include "therion.h"
#include "thparse.h"
#include "thtfangle.h"
#include "thexception.h"
#include "thinfnan.h"
#include <math.h>

thtfangle::thtfangle() : thtf(TT_TFU_DEG) {}

bool mils_warning(true);

void thtfangle::parse_units(char * ustr)
{

  this->units = thmatch_token(ustr, thtt_tfunits_angle);

  switch (this->units) {
    case TT_TFU_DEG:
    case TT_TFU_DMS:
      this->ufactor = 1.0;
      break;
    case TT_TFU_MIN:
      this->ufactor = 0.0166666666666666666666666666666666666666666;
      break;
    case TT_TFU_MILS:
      if (mils_warning) {
        thwarning("using mil/mils as angle unit is deprecated");
        mils_warning = false;
      }
      [[fallthrough]];
    case TT_TFU_GRAD:
      this->ufactor = 0.9;
      break;
	  case TT_TFU_PERC:
			if (this->allow_percentage) {
	      this->ufactor = 1.0;
				break;
			}
      [[fallthrough]];
    case TT_TFU_UNKNOWN_ANGLE:
      throw thexception(fmt::format("unknown angle unit -- {}", ustr));
      break;
  }
  
}

double thtfangle::transform(double value) {
  switch (this->units) {
	 case TT_TFU_PERC:
	 	return (atan(thtf::transform(value) / 100.0) / THPI * 180.0); 
	  break;
	default:
	  return thtf::transform(value);
  }
}
