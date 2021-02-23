/**
 * @file thexpdb.h
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
 
#ifndef thexpdb_h
#define thexpdb_h


#include "thexport.h"
#include <string>


/**
 * Model export options.
 */

enum {
  TT_EXPDB_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPDB_OPT_FORMAT,  ///< Output option.
  TT_EXPDB_OPT_ENCODING,  ///< Output option.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_expdb_opt[] = {
  {"-enc", TT_EXPDB_OPT_ENCODING},
  {"-encoding", TT_EXPDB_OPT_ENCODING},
  {"-fmt", TT_EXPDB_OPT_FORMAT},
  {"-format", TT_EXPDB_OPT_FORMAT},
  {NULL, TT_EXPDB_OPT_UNKNOWN}
};



/**
 * Model export formats.
 */

enum {
  TT_EXPDB_FMT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPDB_FMT_SQL,  ///< sql
  TT_EXPDB_FMT_CSV,  ///< list of shots in CSV format
  TT_EXPDB_FMT_QTH,  ///< qtherion JSON format
};


/**
 * Format parsing table.
 */
 
static const thstok thtt_expdb_fmt[] = {
  {"csv", TT_EXPDB_FMT_CSV},
  {"qth", TT_EXPDB_FMT_QTH},
  {"sql", TT_EXPDB_FMT_SQL},
  {NULL, TT_EXPDB_FMT_UNKNOWN}
};


/**
 * Main export class.
 */
 
class thexpdb : public thexport {

  public:

  int format,  ///< Output format.
    encoding;
  
  void export_sql_file(class thdatabase * dbp);
  
  void export_csv_file(class thdatabase * dbp);

  void export_qth_file(class thdatabase * dbp);
  
  void export_qth_survey(std::string fpath, thsurvey * srv);  
  
  public:
  
  thexpdb(); ///< Default constructor.

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
  
};


#endif


