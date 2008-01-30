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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#ifndef thexptable_h
#define thexptable_h


#include "thexport.h"
#include "thattr.h"

/**
 * Model export options.
 */

enum {
  TT_EXPTABLE_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPTABLE_OPT_FORMAT,  ///< Output option.
  TT_EXPTABLE_OPT_ENCODING,  ///< Output option.
  TT_EXPTABLE_OPT_ATTRIBUTES,  ///< Output option.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_exptable_opt[] = {
  {"-attr", TT_EXPTABLE_OPT_ATTRIBUTES},
  {"-attributes", TT_EXPTABLE_OPT_ATTRIBUTES},
  {"-enc", TT_EXPTABLE_OPT_ENCODING},
  {"-encoding", TT_EXPTABLE_OPT_ENCODING},
  {"-fmt", TT_EXPTABLE_OPT_FORMAT},
  {"-format", TT_EXPTABLE_OPT_FORMAT},
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

  bool expattr; ///< Whether to export user defined attributes.

  int format,  ///< Output format.
    encoding;
  
  thattr m_table;
  
  thexptable(); ///< Default constructor.

  /**
   * Parse model export options.
   */
   
  virtual void parse_options(int & argx, int nargs, char ** args);

  
  /**
   * Dump object into file.
   */
   
  virtual void dump_body(FILE * xf);


  /**
   * Dump object into file.
   */
   
  virtual void dump_header(FILE * xf);
  

  /**
   * Make export.
   */
   
  virtual void process_db(class thdatabase * dbp);
  
};


#endif


