/**
 * @file thdataobject.h
 * Main data class module.
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
 
#ifndef thdataobject_h
#define thdataobject_h


#include "thdatabase.h"
#include "thperson.h"
#include "thparse.h"
#include "thdate.h"
#include "thdataleg.h"
#include <stdio.h>
#include <map>

/**
 * Command tokens.
 */

enum {
  TT_UNKNOWN_CMD,
  TT_DATAOBJECT_CMD, TT_DATASTATION_CMD, TT_2DDATAOBJECT_CMD,
  TT_SURVEY_CMD, TT_ENDSURVEY_CMD,
  TT_GRADE_CMD, TT_LAYOUT_CMD,
  TT_SCRAP_CMD, TT_ENDSCRAP_CMD,
  TT_POINT_CMD, TT_LINE_CMD, TT_AREA_CMD,
  TT_JOIN_CMD, TT_MAP_CMD, TT_SURFACE_CMD,
  TT_DATA_CMD, TT_IMPORT_CMD, TT_SKETCH_CMD,
};


/**
 * Command parsing table.
 */
 
static const thstok thtt_commands[] = {
  {"area", TT_AREA_CMD},
  {"centerline", TT_DATA_CMD},
  {"centreline", TT_DATA_CMD},
  {"endscrap", TT_ENDSCRAP_CMD},
  {"endsurvey", TT_ENDSURVEY_CMD},
  {"grade", TT_GRADE_CMD},
  {"import", TT_IMPORT_CMD},
  {"join", TT_JOIN_CMD},
  {"layout", TT_LAYOUT_CMD},
  {"line", TT_LINE_CMD},
  {"map", TT_MAP_CMD},
  {"point", TT_POINT_CMD},
  {"scrap", TT_SCRAP_CMD},
  {"sketch", TT_SKETCH_CMD},
  {"surface", TT_SURFACE_CMD},
  {"survey", TT_SURVEY_CMD},
	{NULL, TT_UNKNOWN_CMD},
};


/**
 * Dataobject command options tokens.
 */
 
enum {
  TT_DATAOBJECT_UNKNOWN = 1000,
  TT_DATAOBJECT_NAME = 1001,
  TT_DATAOBJECT_TITLE = 1002,
  TT_DATAOBJECT_AUTHOR = 1003,
  TT_DATAOBJECT_COPYRIGHT = 1004,
  TT_DATAOBJECT_STNS = 1005,
  TT_DATAOBJECT_CS = 1006,
  TT_DATAOBJECT_ATTR = 1007,
};


/**
 * Dataobject command options parsing table.
 */
 
static const thstok thtt_dataobject_opt[] = {
  {"attr", TT_DATAOBJECT_ATTR},
  {"author", TT_DATAOBJECT_AUTHOR},
  {"copyright", TT_DATAOBJECT_COPYRIGHT},
  {"cs", TT_DATAOBJECT_CS},
  {"id", TT_DATAOBJECT_NAME},
  {"station-names", TT_DATAOBJECT_STNS},
  {"title", TT_DATAOBJECT_TITLE},
	{NULL, TT_DATAOBJECT_UNKNOWN},
};


/**
 * Option description class.
 */
 
class thcmd_option_desc {

  public:

  
  int id, ///< Option identifier.
      nargs; ///< Number of option arguments.
  
  /**
   * Standard constructor.
   */
   
  thcmd_option_desc() : id(TT_DATAOBJECT_UNKNOWN), nargs(1) {}
  

  /**
   * Constructor.
   *
   * @param oid Option id.
   */
   
  thcmd_option_desc(int oid) : id(oid), nargs(1) {}
  
  
  /**
   * Constructor.
   *
   * @param oid Option id.
   * @param nas Number of arguments.
   */
   
  thcmd_option_desc(int oid, int nas) : id(oid), nargs(nas) {}
    
    
};


/**
 * Dataobject author class.
 */
 
class thdataobject_author {

  /**
   * Comparison operator.
   */
   
  friend bool operator < (const thdataobject_author & a1, 
      const thdataobject_author & a2);
  
  public:

  thperson name;  ///< Author's name
  unsigned long rev;  ///< Object's revision.
  
  /**
   * Standard constructor.
   */
   
  thdataobject_author() : name(), rev(0) {}


  /**
   * Default constructor.
   */
   
  thdataobject_author(thperson an, unsigned long rv) : name(an), rev(rv) {}
  
  
};


/* *
 * Dataobject copyright class.
 */
 
class thdataobject_copyright {

  /**
   * Comparison operator.
   */
   
  friend bool operator < (const thdataobject_copyright & c1, 
      const thdataobject_copyright & c2);
  
  public:

  const char * name;  ///< Author's name
  unsigned long rev;  ///< Object's revision.
  
  /**
   * Standard constructor.
   */
   
  thdataobject_copyright() : name(""), rev(0) {}
   
  
  /**
   * Default constructor.
   */
   
  thdataobject_copyright(const char * nn, unsigned long rv) : name(nn), rev(rv) {}
  
  
};


typedef std::map <thdataobject_author, thdate> thdo_author_map_type;  ///< Author's map type
typedef std::map <thdataobject_copyright, thdate> thdo_copyright_map_type;  ///< Copyright's map type


/**
 * Main data object class.
 *
 * Father object of all data object. Main database item.
 */

class thdataobject {

  public:
  
  friend class thdatabase;
  friend class thdb1d;
  friend class thdb2d;
  friend class thselector;

  class thdatabase * db;

  const char * name,  ///< Object name.
    * title,  ///< Object title.
    * stnpref, ///< Station names prefix;
    * stnsuff; ///< Station names suffix;
  
  unsigned long id;  ///< Object identifier.
  
  bool selected,  ///< Whether object is selected.
    tmp_bool;  ///< Temporary variable for some algorithms
    
  unsigned long selected_number,  ///< Number of selection.
    tmp_ulong;  ///< Temporary variable for some algorithms
  
  thdataobject * nsptr,  ///< Next object in survey.
    * psptr;  ///< Previous object in survey.
  class thsurvey * fsptr;  ///< Father survey ptr.
  
  unsigned long revision;  ///< Object revision.
	thobjectsrc source;  ///< Object source.

  thperson dotmp_person;  ///< Temporary person.
  thdate dotmp_date;  ///< Temporary date.
  thdataobject_author dotmp_author; ///< Temporary author.
  thdataobject_copyright dotmp_copyright; ///< Temporary author.
  thdo_author_map_type author_map;  ///< Author map.
  thdo_copyright_map_type copyright_map;  ///< Copyright map.

  int cs;  ///< Coordinate system.
	thobjectsrc cs_source;  ///< Coordinate system source.

  public:

  /**
   * Standard constructor.
   */
  
  thdataobject();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thdataobject();
  
  
  /**
   * Assign database to object.
   */
   
  void assigndb(thdatabase * pdb);
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thdataobject";};
  
  
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
   * Whether multiple ends.
   */
   
  virtual bool get_cmd_ends_state();


  /**
   * Whether cmd is end.
   */
   
  virtual bool get_cmd_ends_match(char * cmd);
  
  
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
   * @param args Option arguments arry.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);
  
  
  /**
   * Returns object id.
   */
   
  virtual int get_id();
  
  
  /**
   * Return object name.
   */
   
  virtual const char * get_name();
  
  
  /**
   * Return object title.
   */
   
  const char * get_title();
  
  
  /**
   * Delete this object.
   *
   * @warn Always use this methos instead of delete function.
   */
   
  virtual void self_delete();
  
  
  /**
   * Get context for object.
   */
   
  virtual int get_context();
  
  
  /**
   * Print object contents into file.
   */
   
  void self_print(FILE * outf);
  
  
  /**
   * Print object contents into file.
   */
   
  virtual void self_print_properties(FILE * outf);
  
    
  /**
   * Write object source to exception description.
   */
   
  virtual void throw_source();
  
  
  /**
   * Complete object settings.
   *
   * This method is called, before object is inserted into database,
   * or configuration is ended.
   */
   
  virtual void start_insert();
  
  
  /**
   * Return father survey.
   */
   
  class thsurvey * get_father_survey() {return this->fsptr;}
  
  
  /**
   * Whether object is selected.
   */
   
  bool is_selected() {return this->selected;}
  
  /**
   * Is inside other survey.
   */
 
  bool is_in_survey(thsurvey * psearch);

  /**
   * Convert coordinates according to current CS specification.
   */

  void convert_cs(char * src_x, char * src_y, double & dst_x, double & dst_y);

  /**
   * Parse object attributes.
   */

  void parse_attribute(char * name, char * value);

};


#endif


