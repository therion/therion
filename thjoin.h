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
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thjoin";};
  
  
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
   * Get context for object.
   */
   
  int get_context() override;


  /**
   * Print object properties.
   */
   
  void self_print_properties(FILE * outf) override; 
  

};


#endif


