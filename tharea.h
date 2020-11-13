/**
 * @file tharea.h
 * Area module.
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

#ifndef tharea_h
#define tharea_h


#include "th2ddataobject.h"
#include "thdb2dab.h"

/**
 * Area command options tokens.
 */

enum {
  TT_AREA_UNKNOWN = 3000,
  TT_AREA_TYPE = 3001,
};


/**
 * Area command options parsing table.
 */

static const thstok thtt_area_opt[] = {
  {NULL, TT_AREA_UNKNOWN},
};


/**
 * Area type tokens.
 */

enum {
  TT_AREA_TYPE_UNKNOWN,
  TT_AREA_TYPE_U,
  TT_AREA_TYPE_WATER,
  TT_AREA_TYPE_SUMP,
  TT_AREA_TYPE_SAND,
  TT_AREA_TYPE_DEBRIS,
  TT_AREA_TYPE_BLOCKS,
  TT_AREA_TYPE_SNOW,
  TT_AREA_TYPE_ICE,
  TT_AREA_TYPE_CLAY,
  TT_AREA_TYPE_PEBBLES,
  TT_AREA_TYPE_BEDROCK,
  TT_AREA_TYPE_FLOWSTONE,
  TT_AREA_TYPE_MOONMILK,
  TT_AREA_TYPE_DIMENSIONS,
  TT_AREA_TYPE_MUDCRACK,
  TT_AREA_TYPE_PILLAR,
  TT_AREA_TYPE_PILLARWITHCURTAINS,
  TT_AREA_TYPE_STALACTITE,
  TT_AREA_TYPE_STALACTITESTALAGMITE,
  TT_AREA_TYPE_STALAGMITE,
};


/**
 * Area types parsing table.
 */

static const thstok thtt_area_types[] = {
  {"bedrock", TT_AREA_TYPE_BEDROCK},
  {"blocks", TT_AREA_TYPE_BLOCKS},
  {"clay", TT_AREA_TYPE_CLAY},
  {"debris", TT_AREA_TYPE_DEBRIS},
  {"dimensions", TT_AREA_TYPE_DIMENSIONS},
  {"flowstone", TT_AREA_TYPE_FLOWSTONE},
  {"ice", TT_AREA_TYPE_ICE},
  {"moonmilk", TT_AREA_TYPE_MOONMILK},
  {"mudcrack", TT_AREA_TYPE_MUDCRACK},
  {"pebbles", TT_AREA_TYPE_PEBBLES},
  {"pillar", TT_AREA_TYPE_PILLAR},
  {"pillar-with-curtains", TT_AREA_TYPE_PILLARWITHCURTAINS},
  {"sand", TT_AREA_TYPE_SAND},
  {"snow", TT_AREA_TYPE_SNOW},
  {"stalactite", TT_AREA_TYPE_STALACTITE},
  {"stalactite-stalagmite", TT_AREA_TYPE_STALACTITESTALAGMITE},
  {"stalagmite", TT_AREA_TYPE_STALAGMITE},
  {"sump", TT_AREA_TYPE_SUMP},
  {"u",TT_AREA_TYPE_U},
  {"water", TT_AREA_TYPE_WATER},
  {NULL, TT_AREA_TYPE_UNKNOWN},
};


/**
 * Area class.
 */

class tharea : public th2ddataobject {

  public:

  friend class thdb2d;

  int type;  ///< Area type.

  thdb2dab * first_line,  ///< First border line.
    * last_line;  ///< Last border line.

  class thline * m_outline_line;

  void insert_border_line(int npars, char ** pars);  ///< Insert border line.

  public:

  /**
   * Standard constructor.
   */

  tharea();


  /**
   * Standard destructor.
   */

  ~tharea();


  /**
   * Return class identifier.
   */

  virtual int get_class_id();


  /**
   * Return class name.
   */

  virtual const char * get_class_name() {return "tharea";};


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
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */

  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Print object properties.
   */

  virtual void self_print_properties(FILE * outf);


  /**
   * Export to metapost file.
   */

  virtual bool export_mp(class thexpmapmpxs * out);


  void parse_type(char * tstr);  ///< Parse area type.

  void parse_subtype(char * ststr);  ///< Parse area subtype.

  virtual void start_insert();


};


#endif


