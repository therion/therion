/**
 * @file thexpmodel.h
 * Export class.
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
 
#ifndef thexpmodel_h
#define thexpmodel_h


#include "thexport.h"


/**
 * Model export options.
 */

enum {
  TT_EXPMODEL_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMODEL_OPT_FORMAT,  ///< Output option.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_expmodel_opt[] = {
  {"-fmt", TT_EXPMODEL_OPT_FORMAT},
  {"-format", TT_EXPMODEL_OPT_FORMAT},
  {NULL, TT_EXPMODEL_OPT_UNKNOWN}
};



/**
 * Model export formats.
 */

enum {
  TT_EXPMODEL_FMT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMODEL_FMT_SURVEX,  ///< survex
  TT_EXPMODEL_FMT_COMPASS,  ///< compass
  TT_EXPMODEL_FMT_THERION,  ///< therion
};


/**
 * Format parsing table.
 */
 
static const thstok thtt_expmodel_fmt[] = {
  {"compass", TT_EXPMODEL_FMT_COMPASS},
  {"survex", TT_EXPMODEL_FMT_SURVEX},
  {"therion", TT_EXPMODEL_FMT_THERION},
  {NULL, TT_EXPMODEL_FMT_UNKNOWN}
};


/**
 * Main export class.
 */
 
class thexpmodel : public thexport {

  public:

  int format;  ///< Output format.
  
  void export_3d_file(class thdatabase * dbp);  ///< Export survex 3d file.
  
  void export_plt_file(class thdatabase * dbp);  ///< Export compass plt file.
  
  void export_tm_file(class thdatabase * dbp);  ///< Export therion model file.

  public:
  
  thexpmodel(); ///< Default constructor.

  /**
   * Parse model export options.
   */
   
  virtual void parse_options(int & argx, int nargs, char ** args);

  
  /**
   * Dump object into file.
   */
   
  virtual void dump_body(FILE * xf);


  /**
   * Dump object into file.
   */
   
  virtual void dump_header(FILE * xf);
  

  /**
   * Make export.
   */
   
  virtual void process_db(class thdatabase * dbp);
  
};


#endif


