/**
 * @file thendscrap.h
 * endscrap module.
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
 
#ifndef thendscrap_h
#define thendscrap_h


#include "thdataobject.h"

/**
 * endscrap command options tokens.
 */
 
enum {
  TT_ENDSCRAP_UNKNOWN = 2000,
};


/**
 * endscrap command options parsing table.
 */
 
static const thstok thtt_endscrap_opt[] = {
  {NULL, TT_ENDSCRAP_UNKNOWN},
};


/**
 * endscrap class.
 */

class thendscrap : public thdataobject {

  public:

  /**
   * Standard constructor.
   */
  
  thendscrap();
  
  
  /**
   * Standard destructor.
   */
   
  ~thendscrap();
  
  
  /**
   * Return class identifier.
   */
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thendscrap";};
  
  
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
   * Get context for object.
   */
   
  int get_context() override;

};


#endif


