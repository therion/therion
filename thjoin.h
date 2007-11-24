/**
 * @file thjoin.h
 * join module.
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
 
#ifndef thjoin_h
#define thjoin_h


#include "thdataobject.h"


/**
 * join command options tokens.
 */
 
enum {
  TT_JOIN_UNKNOWN = 2000,
  TT_JOIN_SMOOTH = 2001,
  TT_JOIN_COUNT = 2002,
};


/**
 * join command options parsing table.
 */
 
static const thstok thtt_join_opt[] = {
  {"count", TT_JOIN_COUNT},
  {"smooth", TT_JOIN_SMOOTH},
  {NULL, TT_JOIN_UNKNOWN},
};



/**
 * join class.
 */

class thjoin : public thdataobject {


  public:
  
  friend class thdb2d;

  class thdb2dji * first_item,
    * last_item;
    
  class thdb2dprj * proj;
  thjoin * proj_next_join,
    * proj_prev_join;

  int smooth,  ///< Smooth option.
    count; ///< Number of joins.
  
  void parse_item(char * istr);  ///< Parse join item.

  public:

  /**
   * Standard constructor.
   */
  
  thjoin();
  
  
  /**
   * Standard destructor.
   */
   
  ~thjoin();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thjoin";};
  
  
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
   * Get context for object.
   */
   
  virtual int get_context();


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
  

};


#endif


