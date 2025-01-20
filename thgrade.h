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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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

  double dls_length = {}, dls_gradient = {}, dls_bearing = {}, dls_counter = {}, dls_depth = {},
    dls_dx = {}, dls_dy = {}, dls_dz = {}, dls_x = {}, dls_y = {}, dls_z = {};
  
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
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thgrade";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  bool is(int class_id) override;
  
  
  /**
   * Return number of command arguments.
   */
   
  int get_cmd_nargs() override;
  
  
  /**
   * Return command name.
   */
   
  const char * get_cmd_name() override;
  
  
  /**
   * Return command end option.
   */
   
  const char * get_cmd_end() override;
  
  
  /**
   * Return option description.
   */
   
  thcmd_option_desc get_cmd_option_desc(const char * opts) override;
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */
   
  void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline) override;


  /**
   * Print object properties.
   */
   
  void self_print_properties(FILE * outf) override; 
  

  /**
   * Print object in C++ format.
   */
   
  void self_print_library(); 
  

  /**
   * Get context for object.
   */
   
  int get_context() override;
  
  /**
   * Update data SD.
   */
  
  void update_data_sd(thdata * dp);
  
  /**
   * Start insertion into database.
   */
   
  void start_insert() override;
  
};

#endif


