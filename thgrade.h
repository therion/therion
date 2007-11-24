/**
 * @file thgrade.h
 * Survey grade module.
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
 
#ifndef thgrade_h
#define thgrade_h


#include "thdataobject.h"
#include "thdata.h"

/**
 * grade command options tokens.
 */
 
enum {
  TT_GRADE_UNKNOWN = 2000,
  TT_GRADE_OPTION = 2001,
};


/**
 * grade command options parsing table.
 */
 
static const thstok thtt_grade_opt[] = {
  {NULL, TT_GRADE_UNKNOWN},
};


/**
 * grade class.
 */

class thgrade : public thdataobject {

  static thdata data;
    
  void update_sd_from_data(bool alsonan = false); ///< Update these data.

  public:
    
  void set_sd(int nargs, char ** args);  ///< Set SD.
  
  public:

  double dls_length, dls_gradient, dls_bearing, dls_counter, dls_depth,
    dls_dx, dls_dy, dls_dz, dls_x, dls_y, dls_z;
  
  /**
   * Standard constructor.
   */
  
  thgrade();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thgrade();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thgrade";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  
  /**
   * Return number of command arguments.
   */
   
  virtual int get_cmd_nargs();
  
  
  /**
   * Return command name.
   */
   
  virtual const char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual const char * get_cmd_end();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(const char * opts);
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments arry.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Delete this object.
   *
   * @warn Always use this methos instead of delete function.
   */
   
  virtual void self_delete();


  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  

  /**
   * Print object in C++ format.
   */
   
  void self_print_library(); 
  

  /**
   * Get context for object.
   */
   
  virtual int get_context();
  
  /**
   * Update data SD.
   */
  
  void update_data_sd(thdata * dp);
  
  /**
   * Start insertion into database.
   */
   
  void start_insert();
  
};

#endif


