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
};


/**
 * import command options parsing table.
 */
 
static const thstok thtt_import_opt[] = {
  {"fmt", TT_IMPORT_FORMAT},
  {"format", TT_IMPORT_FORMAT},
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





/**
 * import class.
 */

class thimport : public thdataobject {

  public:

  // insert here real properties
  int format;  
  char * fname;
  class thdata * data;
  thobjectsrc mysrc;

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
   
  virtual char * get_class_name() {return "thimport";};
  
  
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
  
  
  void set_file_name(char * fnm);
  
  void import_file();

  void import_file_img();
  
  const char * station_name(const char * sn, const char separator);

};


#endif


