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

#include "thbuffer.h"
#include "thmbuffer.h"
#include "thinput.h"

/**
 * Therion system dependend properties initializer.
 */
 
enum {
  THINIT_ENCODING_DEFAULT,
  THINIT_ENCODING_SQL,
};
 
class thinit {

  public:  

  int encoding_default, encoding_sql;  ///< Default encoding.
    
  thbuffer path_cavern, ///< Survex executable full path.
    path_pdftex, path_mpost,  ///< PDF tex and metapost path
    tmp_path, tmp_remove_script;
    //path_3dtopos, 
    
  int lang;
  
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
   * Return cavern executable path.
   */
   
  char * get_path_cavern();

  /**
   * Return 3dtopos executable path.
   */
   
  //char * get_path_3dtopos();
  
  
  /**
   * Return metapost executable path.
   */
   
  char * get_path_mpost();

  /**
   * Return pdftex executable path.
   */
   
  char * get_path_pdftex();
  
  int get_encoding(int type = THINIT_ENCODING_DEFAULT);
  
  
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


