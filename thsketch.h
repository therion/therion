/**
 * @file thsketch.h
 * sketch module.
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
 
#ifndef thsketch_h
#define thsketch_h


#include "thdataobject.h"
#include "thpic.h"
#include <list>
#include <map>

/**
 * sketch command options tokens.
 */
 
enum {
  TT_SKETCH_UNKNOWN = 2000,
  TT_SKETCH_PICTURE = 2001,
  TT_SKETCH_PROJECTION = 2002,
};


/**
 * sketch command options parsing table.
 */
 
static const thstok thtt_sketch_opt[] = {
  {"pict", TT_SKETCH_PICTURE},
  {"picture", TT_SKETCH_PICTURE},
  {"proj", TT_SKETCH_PROJECTION},
  {"projection", TT_SKETCH_PROJECTION},
  {NULL, TT_SKETCH_UNKNOWN},
};


/**
 * Sketch station structure.
 */

struct thsketchst {
  double x, y;
  thobjectname station;
  thobjectsrc source;
};

typedef std::list<thsketchst> thsketchst_list;


/**
 * sketch class.
 */

class thsketch : public thdataobject {

  public:

  thpic pic, ///< Picture.
    mpic;  ///< Morphed picture.

  bool morphed;

  class thdb2dprj * proj;  ///< Sketch projection.

  thsketchst_list stations;  ///< List of sketch stations.

  /**
   * Standard constructor.
   */
  
  thsketch();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thsketch();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thsketch";};
  
  
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
   * Parse picture name.
   */
  
  void parse_picture(char ** args);


  /**
   * Parse station image coordinates.
   */

  void parse_station(char ** args);


  /// Check station references.
  void check_stations();


  /// Morph image if possible.
  bool morph();

};


#endif


