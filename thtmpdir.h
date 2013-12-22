/**
 * @file thtmpdir.h
 * Temporary directory module.
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
 
#ifndef thtmpdir_h
#define thtmpdir_h

#include "thbuffer.h"


/**
 * Temporary directory module.
 *
 * Creates and remove temporary directory.
 */
 
class thtmpdir {

  public:

  bool exist;  ///< ID whether temp directory exist.  
  bool tried;  ///< ID, if we've tried to create temp directory.  
  bool delete_all;  ///< ID whether to delete temporary directory.
  bool debug;  ///< ID, whether debugging mode is on.  
  thbuffer name,  ///< Name of temp dir.  
    file_name,  ///< Name of temporary file.
    tmp_remove_script;

  /**
   * Creates temporary directory.
   */
   
  void create();
  
  
  /**
   * Removes temporaty directory.
   */
   
  void remove();


  /**
   * Standard constructor.
   */
   
  thtmpdir();
  
  
  /**
   * Standard destructor.
   */
   
  ~thtmpdir();
  
  
  /**
   * Retrieve temporary directory name.
   */
   
  const char* get_dir_name();
  
  
  /**
   * Form valid path from temporary directory name and given file name.
   */
   
  const char* get_file_name(const char *fname);
  
  
  /**
   * Set deleting state.
   */
  
  void set_delete(bool delete_id);
  
  /**
   * Retrieve deleting state.
   */
   
  bool get_delete();
  
  
  /**
   * Turn deleting temp directory on.
   */
  
  void delete_on();


  /**
   * Turn deleting temp directory off.
   */
  
  void delete_off();
  
};

extern thtmpdir thtmp;

#endif


