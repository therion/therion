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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thparse.h"
#include "thtfangle.h"
#include "thexception.h"
#include "thinfnan.h"
#include <math.h>

thtfangle::thtfangle() {

  this->units = TT_TFU_DEG;
  this->allow_percentage = false;

}


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
    case TT_TFU_GRAD:
      this->ufactor = 0.9;
      break;
	  case TT_TFU_PERC:
			if (this->allow_percentage) {
	      this->ufactor = 1.0;
				break;
			}
    case TT_TFU_UNKNOWN_ANGLE:
      ththrow(("unknown angle unit -- %s", ustr))
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
