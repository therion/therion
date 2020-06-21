/**
 * @file thinit.h
 * Initialization class.
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
 
#ifndef thinit_h
#define thinit_h

#include <string>
#include "thbuffer.h"
#include "thmbuffer.h"
#include "thinput.h"

/**
 * Therion system dependent properties initializer.
 */
 
enum {
  THINIT_ENCODING_DEFAULT,
  THINIT_ENCODING_SQL,
};

enum {
	THINIT_LOOPC_UNKNOWN = 0,
	THINIT_LOOPC_THERION,
	THINIT_LOOPC_SURVEX,
};
 
class thinit {

  public:  

  int encoding_default, encoding_sql;  ///< Default encoding.
    
  thbuffer path_cavern, ///< Survex executable full path.
    path_pdftex, path_mpost,  ///< PDF tex and metapost path
    opt_mpost, path_otftotfm, 
    path_convert, path_identify,  ///< Path to ImageMagick convert and identify executables.
    tmp_path, tmp_remove_script;
    //path_3dtopos, 

  std::string font_src[5], font_dst[5];
  
  bool tex_env, fonts_ok, proj_auto;
  int lang, loopc, proj_missing_grid;
  
  thmbuffer cmb;  ///< Command parsing mbuffer.
  
  thinput ini_file;  ///< Initialization file.
    
  /**
   * Standard constructor.
   */
  
  thinit();
  
  
  /**
   * Destructor.
   */
  
  ~thinit();
  
  
  /**
   * Load initialization file.
   */
   
  void load();

  /**
   * Copy fonts to temporary folder.
   */

  void copy_fonts();
  
  /**
   * Return cavern executable path.
   */
   
  char * get_path_cavern();

  /**
   * Return ImageMagick convert executable path.
   */
   
  char * get_path_convert();

  /**
   * Return ImageMagick identify executable path.
   */
  char * get_path_identify();

  /**
   * Return metapost options.
   */

  char * get_opt_mpost();
  
  
  /**
   * Return metapost executable path.
   */
   
  char * get_path_mpost();
  char * get_path_otftotfm();

  /**
   * Return pdftex executable path.
   */
   
  char * get_path_pdftex();

  int get_lang();

  void check_font_path(const char * fname, int index);
  
  int get_encoding(int type = THINIT_ENCODING_DEFAULT);

  bool get_proj_auto();

  int get_proj_missing_grid();

  void set_proj_lib_path();

};


/**
 * Print initialization file.
 */
 
void thprint_init_file();


/**
 * Initialization module.
 */

extern thinit thini;


#endif


