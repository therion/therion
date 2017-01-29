/**
 * @file thendsurvey.h
 * Endsurvey module.
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
 
#ifndef thendsurvey_h
#define thendsurvey_h


#include "thdataobject.h"

/**
 * endsurvey class.
 */

class thendsurvey : public thdataobject {

  public:

  /**
   * Standard constructor.
   */
  
  thendsurvey();
  
  
  /**
   * Standard destructor.
   */
   
  ~thendsurvey();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  
  /**
   * Return number of command arguments.
   */
   
  virtual int get_cmd_nargs();
  
  
  /**
   * Return command end option.
   */
   
  virtual const char * get_cmd_end();
  
  
  /**
   * Return command name.
   */
   
  virtual const char * get_cmd_name();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(const char * opts);
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Delete this object.
   *
   * @warn Always use this method instead of delete function.
   */
   
  virtual void self_delete();
 
  
  /**
   * Get context for object.
   */
   
  virtual int get_context();


  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thendsurvey";};  
  

  /**
   * Print object contents into file.
   */
   
  virtual void self_print_properties(FILE * outf);  
    
  
};


#endif


