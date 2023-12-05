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

#include <memory>

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
  TT_AREA_TYPE_UNDERWATERCEILING,
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
  {"underwater-ceiling", TT_AREA_TYPE_UNDERWATERCEILING},
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

  std::unique_ptr<class thline> m_outline_line;

  void insert_border_line(int npars, char ** pars);  ///< Insert border line.

  public:

  /**
   * Standard constructor.
   */

  tharea();

  /**
   * Return class identifier.
   */

  int get_class_id() override;


  /**
   * Return class name.
   */

  const char * get_class_name() override {return "tharea";};


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
   * Export to metapost file.
   */

  bool export_mp(class thexpmapmpxs * out) override;


  void parse_type(char * tstr);  ///< Parse area type.

  void parse_subtype(char * ststr);  ///< Parse area subtype.

  void start_insert() override;


};


#endif


