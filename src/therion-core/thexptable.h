/**
 * @file thexptable.h
 * Export class.
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
 
#ifndef thexptable_h
#define thexptable_h


#include "thexport.h"
#include "thattr.h"
#include <list>
#include <string>

/**
 * Model export options.
 */

enum {
  TT_EXPTABLE_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPTABLE_OPT_FORMAT,  ///< Output option.
  TT_EXPTABLE_OPT_ENCODING,  ///< Output option.
  TT_EXPTABLE_OPT_ATTRIBUTES,  ///< Output option.
  TT_EXPTABLE_OPT_LOCATION,
  TT_EXPTABLE_OPT_FILTER,
  TT_EXPTABLE_OPT_SURVEYS,
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_exptable_opt[] = {
  {"-attr", TT_EXPTABLE_OPT_ATTRIBUTES},
  {"-attributes", TT_EXPTABLE_OPT_ATTRIBUTES},
  {"-enc", TT_EXPTABLE_OPT_ENCODING},
  {"-encoding", TT_EXPTABLE_OPT_ENCODING},
  {"-filter", TT_EXPTABLE_OPT_FILTER},
  {"-fmt", TT_EXPTABLE_OPT_FORMAT},
  {"-format", TT_EXPTABLE_OPT_FORMAT},
  {"-location", TT_EXPTABLE_OPT_LOCATION},
  {"-surveys", TT_EXPTABLE_OPT_SURVEYS},
  {NULL, TT_EXPTABLE_OPT_UNKNOWN}
};



/**
 * Model export formats.
 */

enum {
  TT_EXPTABLE_FMT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPTABLE_FMT_DBF, 
  TT_EXPTABLE_FMT_TXT, 
  TT_EXPTABLE_FMT_HTML,
  TT_EXPTABLE_FMT_KML, 
};


/**
 * Format parsing table.
 */
 
static const thstok thtt_exptable_fmt[] = {
  {"dbf", TT_EXPTABLE_FMT_DBF},
  {"html", TT_EXPTABLE_FMT_HTML},
  {"kml", TT_EXPTABLE_FMT_KML},
  {"text", TT_EXPTABLE_FMT_TXT},
  {"txt", TT_EXPTABLE_FMT_TXT},  
  {NULL, TT_EXPTABLE_FMT_UNKNOWN}
};


/**
 * Main export class.
 */
 
class thexptable : public thexport {

  public:

  bool expattr, exploc, filter, surveys; ///< Whether to export user defined attributes.

  thattr m_table;

  std::list<std::string> m_str_list;
  
  thexptable(); ///< Default constructor.

  /**
   * Parse model export options.
   */
   
  void parse_options(int & argx, int nargs, char ** args) override;

  
  /**
   * Dump object into file.
   */
   
  void dump_body(FILE * xf) override;


  /**
   * Dump object into file.
   */
   
  void dump_header(FILE * xf) override;
  

  /**
   * Make export.
   */
   
  void process_db(class thdatabase * dbp) override;

  /**
   * Export entrances from survey.
   */

  void export_survey_entrances(class thsurvey * survey);

  /**
   * Return string for temporary use.
   */

  std::string * get_tmp_string();

  /**
   * Add coordinates to table.
   */

  void add_coordinates(double x, double y, double z, const char * xlabel = "", const char * ylabel = "", const char * zlabel = "");
  
};


#endif


