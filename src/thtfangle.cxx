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


enum {TT_TFU_DEG, TT_TFU_GRAD, TT_TFU_MIN, TT_TFU_UNKNOWN};


static const thstok thtt_tfunits_angle[] = {
  {"deg", TT_TFU_DEG},
  {"degree", TT_TFU_DEG},
  {"degrees", TT_TFU_DEG},
  {"grad", TT_TFU_GRAD},
  {"grads", TT_TFU_GRAD},
  {"mil",TT_TFU_GRAD},
  {"mils", TT_TFU_GRAD},
  {"min", TT_TFU_MIN},
  {"minute", TT_TFU_MIN},
  {"minutes", TT_TFU_MIN},
  {NULL, TT_TFU_UNKNOWN},
};


void thtfangle::parse_units(char * ustr)
{
  switch (thmatch_token(ustr, thtt_tfunits_angle)) {
    case TT_TFU_DEG:
      this->ufactor = 1.0;
      break;
    case TT_TFU_MIN:
      this->ufactor = 0.0166666666666666666666666666666666666666666;
      break;
    case TT_TFU_GRAD:
      this->ufactor = 0.9;
      break;
    case TT_TFU_UNKNOWN:
      ththrow(("unknown angle unit -- %s", ustr))
      break;
  }
  
}



