/**
 * @file thimport.h
 * import module.
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
 
#ifndef thimport_h
#define thimport_h


#include "thdataobject.h"

/**
 * import command options tokens.
 */
 
enum {
  TT_IMPORT_UNKNOWN = 2000,
  TT_IMPORT_FORMAT = 2001,
  TT_IMPORT_FILTER = 2002,
  TT_IMPORT_SURVEYS = 2003,
  TT_IMPORT_CALIB = 2004,
};


/**
 * import command options parsing table.
 */
 
static const thstok thtt_import_opt[] = {
  {"calibrate", TT_IMPORT_CALIB},
  {"filter", TT_IMPORT_FILTER},
  {"fmt", TT_IMPORT_FORMAT},
  {"format", TT_IMPORT_FORMAT},
  {"surveys", TT_IMPORT_SURVEYS},
  {NULL, TT_IMPORT_UNKNOWN},
};


enum {
  TT_IMPORT_FMT_UNKNOWN,
  TT_IMPORT_FMT_3D,
  TT_IMPORT_FMT_PLT,
  TT_IMPORT_FMT_XYZ,
};


/**
 * import types parsing table.
 */
 
static const thstok thtt_import_fmts[] = {
  {"3d", TT_IMPORT_FMT_3D},
  {"plt", TT_IMPORT_FMT_PLT},
  {"xyz", TT_IMPORT_FMT_XYZ},
  {NULL, TT_IMPORT_FMT_UNKNOWN},
};


enum {
  TT_IMPORT_SURVEYS_UNKNOWN,
  TT_IMPORT_SURVEYS_CREATE,
  TT_IMPORT_SURVEYS_USE,
  TT_IMPORT_SURVEYS_IGNORE,
};


/**
 * import types parsing table.
 */
 
static const thstok thtt_import_surveys[] = {
  {"create", TT_IMPORT_SURVEYS_CREATE},
  {"ignore", TT_IMPORT_SURVEYS_IGNORE},
  {"use", TT_IMPORT_SURVEYS_USE},
  {NULL, TT_IMPORT_SURVEYS_UNKNOWN},
};





/**
 * import class.
 */

class thimport : public thdataobject {

  public:

  // insert here real properties
  int format, surveys;  
  const char * fname, * filter;
  thobjectsrc mysrc;
  double calib_x, calib_y, calib_z;

  /**
   * Standard constructor.
   */
  
  thimport();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thimport();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thimport";};
  
  
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
   * Get context for object.
   */
   
  virtual int get_context();

  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  
  
  void set_file_name(char * fnm);
  
  void import_file();

  void import_file_img();

  void parse_calib(char * spec, int enc);
  
  const char * station_name(const char * sn, const char separator, struct thsst * sst);

};


#endif


