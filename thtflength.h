/**
 * @file thtflength.h
 * Units transformation class.
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
 
#ifndef thtflength_h
#define thtflength_h

#include "thtf.h"
#include "thparse.h"


enum {TT_TFU_YD, TT_TFU_FT, TT_TFU_IN, TT_TFU_M, TT_TFU_CM, TT_TFU_MM, TT_TFU_UNKNOWN_LENGTH};

static const thstok thtt_tfunits_length[] = {
  {"centimeter", TT_TFU_CM},
  {"centimeters", TT_TFU_CM},
  {"centimetre", TT_TFU_CM},
  {"centimetres", TT_TFU_CM},
  {"cm", TT_TFU_CM},
  {"feet", TT_TFU_FT},
  {"feets", TT_TFU_FT},
  {"ft", TT_TFU_FT},
  {"in", TT_TFU_IN},
  {"inch", TT_TFU_IN},
  {"inches", TT_TFU_IN},
  {"m", TT_TFU_M},
  {"meter", TT_TFU_M},
  {"meters", TT_TFU_M},
  {"metre", TT_TFU_M},
  {"metres", TT_TFU_M},
  {"metric", TT_TFU_M},
  {"millimeter", TT_TFU_MM},
  {"millimeters", TT_TFU_MM},
  {"millimetre", TT_TFU_MM},
  {"millimetres", TT_TFU_MM},
  {"mm", TT_TFU_MM},
  {"yard", TT_TFU_YD},
  {"yards", TT_TFU_YD},
  {"yd", TT_TFU_YD},
	{NULL, TT_TFU_UNKNOWN_LENGTH},
};


/**
 * Length transformation class.
 */
 
class thtflength : public thtf {

  public:
  
  thtflength();
  
  /**
   * Parse units factor.
   */
   
  virtual void parse_units(char * ustr);
  
};


#endif


