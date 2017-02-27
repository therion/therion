/**
 * @file thsvxctrl.h
 * Survex controler.
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
 
#ifndef thsvxctrl_h
#define thsvxctrl_h

#include <stdio.h>
#include "thdataleg.h"
#include "thobjectsrc.h"
#include <map>


typedef std::map < unsigned long, thobjectsrc * > thsvxctrl_src_maptype;  ///< Source map type.


/**
 * Survex controler.
 */
 
class thsvxctrl {

  thdataleg pdl;

  double meridian_convergence, lastleggridmc;
  int lastleggridmccs;
  
  unsigned long svxf_ln;
  thsvxctrl_src_maptype src_map;
  
  FILE * svxf;
  
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


