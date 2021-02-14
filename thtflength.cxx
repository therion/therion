/**
 * @file thtflength.cxx
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
 

#include "thtflength.h"
#include "thexception.h"

thtflength::thtflength() {
  this->units = TT_TFU_M;
}


void thtflength::parse_units(char * ustr) {

  this->units = thmatch_token(ustr, thtt_tfunits_length);

  switch (this->units) {
    case TT_TFU_CM:
      this->ufactor = 0.01;
      break;
    case TT_TFU_MM:
      this->ufactor = 0.001;
      break;
    case TT_TFU_FT:
      this->ufactor = 0.3048;
      break;
    case TT_TFU_IN:
      this->ufactor = 0.0254;
      break;
    case TT_TFU_M:
      this->ufactor = 1.0;
      break;
    case TT_TFU_YD:
      this->ufactor = 0.9144;
      break;
    case TT_TFU_UNKNOWN_LENGTH:
      ththrow("unknown length unit -- {}", ustr);
      break;
  }
  
}







