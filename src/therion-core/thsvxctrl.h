/**
 * @file thsvxctrl.h
 * Survex controller.
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
 
#ifndef thsvxctrl_h
#define thsvxctrl_h

#include <stdio.h>
#include "thdataleg.h"
#include "thobjectsrc.h"
#include <map>


typedef std::map < unsigned long, thobjectsrc * > thsvxctrl_src_maptype;  ///< Source map type.


/**
 * Does a change of leg flags require a new *flags line in the .svx file?
 *
 * Only the flags Survex understands are written, so a flag it knows nothing
 * about -- approximate, artificial -- must not break the block.
 */

constexpr bool thsvx_flags_changed(int lhs, int rhs) noexcept
{
  constexpr int known =
    TT_LEGFLAG_SURFACE | TT_LEGFLAG_DUPLICATE | TT_LEGFLAG_SPLAY;
  return (lhs & known) != (rhs & known);
}


/**
 * Survex controller.
 */
 
class thsvxctrl {

  thdataleg pdl;

  double meridian_convergence = 0.0, lastleggridmc = 0.0;
  int lastleggridmccs = 0;
  
  unsigned long svxf_ln;
  thsvxctrl_src_maptype src_map;
  
  FILE * svxf = nullptr;
  
  void write_survey_leg(thdataleg * legp);
  
  void write_survey_fix(thdatafix * fixp);

  void write_double(double dd);
  
  void transcript_log_file(class thdatabase * dbp, const char * lfnm);

  void load_err_file(class thdatabase * dbp, const char * lfnm);

  public:  
  
  /**
   * Standard constructor.
   */
  
  thsvxctrl();
  
  
  /**
   * Destructor.
   */
  
  ~thsvxctrl();
  
  
  /**
   * Process survey data in given database.
   */
   
  void process_survey_data(class thdatabase * dbp);
  
};


#endif


