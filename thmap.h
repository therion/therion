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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thmap_h
#define thmap_h


#include "thdataobject.h"
#include "thlayoutclr.h"
#include "thmapstat.h"
#include "thobjectname.h"

/**
 * map command options tokens.
 */
 
enum {
  TT_MAP_UNKNOWN = 2000,
  TT_MAP_PREVIEW = 2001,
  TT_MAP_BREAK = 2002,
  TT_MAP_PROJECTION = 2003,
  TT_MAP_SURVEY = 2004,
//  TT_MAP_COLOUR = 2005,
};


/**
 * map command options parsing table.
 */
 
static const thstok thtt_map_opt[] = {
  {"break", TT_MAP_BREAK},
//  {"color", TT_MAP_COLOUR},
//  {"colour", TT_MAP_COLOUR},
  {"preview", TT_MAP_PREVIEW},
  {"proj", TT_MAP_PROJECTION},
  {"projection", TT_MAP_PROJECTION},
  {"survey", TT_MAP_SURVEY},
  {NULL, TT_MAP_UNKNOWN},
};


/**
 * map class.
 */

class thmap : public thdataobject {

  public:

  class thdb2dmi * first_item,  ///< First map item.
    * last_item;  ///< Last map item.
  class thdb2dprj * expl_projection;
  int projection_id;  ///< ID of map projection.
  bool is_basic;  ///< Whether map is basic (is a map of scraps).
  int selection_mode;  ///< How map is maximally selected.
  class thdb2dxs * selection_xs; ///< Maximal ....
  thobjectname asoc_survey; ///< Associated survey.
    
  void parse_item(int npar, char ** pars);  ///< Parse map item.
  void parse_preview(char ** pars);  ///< Parse preview.
  
  void calc_z();
  
  double z;
  long nz;
  unsigned last_level;
  
  thmapstat stat;

//  thlayout_color colour;

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
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thmap";};
  
  
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


