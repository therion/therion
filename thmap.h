/**
 * @file thmap.h
 * 2D map module.
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
 
#ifndef thmap_h
#define thmap_h


#include "thdataobject.h"
#include "thmapstat.h"

/**
 * map command options tokens.
 */
 
enum {
  TT_MAP_UNKNOWN = 2000,
  TT_MAP_PREVIEW = 2001,
  TT_MAP_BREAK = 2002,
};


/**
 * map command options parsing table.
 */
 
static const thstok thtt_map_opt[] = {
  {"break", TT_MAP_BREAK},
  {"preview", TT_MAP_PREVIEW},
  {NULL, TT_MAP_UNKNOWN},
};


/**
 * map class.
 */

class thmap : public thdataobject {

  public:

  class thdb2dmi * first_item,  ///< First map item.
    * last_item;  ///< Last map item.
    
  int projection_id;  ///< ID of map projection.
  bool is_basic;  ///< Whether map is basic.
  int selection_mode;  ///< How map is maximally selected.
  class thdb2dxs * selection_xs; ///< Maximal ....
    
  void parse_item(int npar, char ** pars);  ///< Parse map item.
  void parse_preview(char ** pars);  ///< Parse preview.
  
  void calc_z();
  
  double z;
  long nz;
  unsigned last_level;
  
  thmapstat stat;

  /**
   * Standard constructor.
   */
  
  thmap();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thmap();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thmap";};
  
  
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
   
  virtual char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual char * get_cmd_end();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(char * opts);
  
  
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


