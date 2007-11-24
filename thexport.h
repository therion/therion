/**
 * @file thexport.h
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
 
#ifndef thexport_h
#define thexport_h

#include <stdio.h>
#ifndef THMSVC
#include <strings.h>
#endif
#include "thparse.h"
#include "thobjectsrc.h"

/**
 * General export options.
 */

enum {
  TT_EXP_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXP_OPT_OUTPUT,  ///< Output option.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_exp_opt[] = {
  {"-o", TT_EXP_OPT_OUTPUT},
  {"-output", TT_EXP_OPT_OUTPUT},
  {NULL, TT_EXP_OPT_UNKNOWN}
};


/**
 * Main export class.
 */
 
class thexport {

  public:

  friend class thexporter;
  class thconfig * cfgptr;  ///< Current config pointer.
  int export_mode;  ///< Export mode.
  thobjectsrc src; ///< Export source.
  class thdatabase * db; ///< Exported database.
  
  const char * outpt;  ///< Output file name.
  thbuffer cfgpath;  ///< Config path.
  bool outpt_def;  ///< Whether output file defined.  

  public:
  
  thexport();  ///< Default constructor.
  virtual ~thexport();
  
  void assign_config(class thconfig * cptr); ///< ???
  
  /**
   * Parse format and options.
   */
  
  virtual void parse(int nargs, char ** args);
  
  /**
   * Parse export arguments.
   */
   
  virtual void parse_arguments(int & argx, int nargs, char ** args);
  
  
  /**
   * Parse export options.
   */
   
  virtual void parse_options(int & argx, int nargs, char ** args);
  

  /**
   * Dump object into file.
   */
   
  virtual void dump(FILE * xf);


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
   
  virtual void process_db(class thdatabase * dbp) = 0;


  /**
   * Return path to output file.
   */

  virtual const char * get_output(const char * defname);
  
};


#ifdef THMSVC
#define strcasecmp _stricmp
#endif

#define thexp_set_ext_fmt(extension,cformat) { \
    if (strlen(this->outpt) > strlen(extension)) { \
      if (strcasecmp(&(this->outpt[strlen(this->outpt) - strlen(extension)]), extension) == 0) { \
        this->format = cformat; \
      } \
    } \
  }


#endif


