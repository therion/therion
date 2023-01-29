/**
 * @file thmesh.h
 * mesh module.
 */
  
/* Copyright (C) 2023 Stacho Mudrak
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
 
#ifndef thmesh_h
#define thmesh_h

#include "thdataobject.h"
#include "thobjectname.h"
#include "thobjectsrc.h"
#include "thtflength.h"
#include "loch/lxMath.h"

#include <list>


/**
 * mesh command options tokens.
 */
 
enum {
  TT_MESH_UNKNOWN = 2000,
  TT_MESH_FILE = 2001,
};


/**
 * mesh command options parsing table.
 */
 
static const thstok thtt_mesh_opt[] = {
  {"file", TT_MESH_FILE},
  {NULL, TT_MESH_UNKNOWN},
};


/**
 * Calibration station structure.
 */

struct thmesh_calibst {
	thobjectname name;  ///< Station name.
	lxVec coords; ///< File station coordinates.
	thobjectsrc src; ///< Station source.
	class thsurvey * ssurvey = nullptr; ///< Station survey.
};


/**
 * mesh class.
 */

class thmesh : public thdataobject {

  public:

  const char * file_name;
  std::list<thmesh_calibst> cal_stations;

  /**
   * Store station & coordinates.
   */

  void parse_station(char ** args);

  /**
   * Parse file name.
   */

  void parse_file(char ** args);

  /**
   * Calibrate transformation function.
   */

  void calibrate();

  /**
   * Calibrate transformation function.
   */

  void check_stations();

  /**
   * Standard constructor.
   */
  
  thmesh();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thmesh();
  
  
  /**
   * Return class identifier.
   */
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thmesh";};
  
  
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
  
  void start_insert() override;

};


#endif


