/**
 * @file thconfig.h
 * Configuration module.
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
 
#ifndef thconfig_h
#define thconfig_h

#include "thbuffer.h"
#include "thmbuffer.h"
#include "thinput.h"
#include "thexporter.h"
#include "thselector.h"

/**
 * What to do with configuration file?
 */

typedef enum {THCFG_GENERATE,  ///< Generate config file.
  THCFG_UPDATE,  ///< Update config file.
  THCFG_READ  ///< Read only config file.
  } thcfg_fstate;


/**
 * Configuration class.
 *
 * Provides functions for therion configuration.
 */

class thconfig {

  public:

  thbuffer fname,  ///< Configuration file name.
    search_path,  ///< Lib files search path.
    bf1,  ///< TMP buffer.
    bf2;  ///< TMP buffer.
  thmbuffer strmbuff,  ///< String storage buffer.
    src_fnames,  ///< Source file name.
    cfg_dblines,  ///< Lines with database commands.
    mbf1;  ///< TMP buffer.
  bool skip_comments,  ///< Skip comments when writing config file.
    generate_xthcfg;  ///< Generate xtherion log file.
  thcfg_fstate fstate;  ///< What to do with cfg file.
  thinput cfg_file;  ///< Configuration file input.
  int cfg_fenc;  ///< Configuration file encoding.

  class thdatabase * dbptr;  ///< Associated db.
  thexporter exporter;  ///< Data exporter.
  thselector selector;  ///< Database selector.

  public:

  /**
   * Standard constructor.
   */
  
  thconfig();
  
  
  /**
   * Standard destructor.
   */
   
  ~thconfig();
  
  
  /**
   * Set config file name.
   */
 
  void set_file_name(char * fn);
  
  
  /**
   * Retrieve config file name.
   */
   
  char * get_file_name();
  
  
  /**
   * Set input file name.
   */
  
  void set_source_file_name(char * fn);
  
  
  /**
   * Retrieve input file name.
   */
   
  thmbuffer * get_source_file_names();
  
  
  /**
   * Set/get skip comments state.
   */
   
  void set_comments_skip(bool state);
  
  void comments_skip_on();
  
  void comments_skip_off();
  
  bool get_comments_skip();


  /**
   * Set/get config file state.
   */
   
  void set_file_state(thcfg_fstate fs);
  
  thcfg_fstate get_file_state();


  /**
   * Set/get search path.
   */
   
  void set_search_path(char * pth);
  
  char * get_search_path();
  
  
  /**
   * Load input from configuration file.
   */
   
  void load();
  
  
  /**
   * Load database command.
   */
  
  void load_dbcommand(thmbuffer * valmb);
  
  /**
   * Save input into configuration file.
   */
   
  void save();

  /**
   * Write configuration to xth file.
   */
   
  void xth_save();
  
  
  /**
   * Return string storage buffer.
   */
   
  thmbuffer * get_str_buff() {return &(this->strmbuff);}
  
  
  /**
   * Assign db.
   */
   
  void assign_db(class thdatabase * dp);
  
  
  /**
   * Return assigned db.
   */
   
  class thdatabase * get_db() {return this->dbptr;}
  
  
  /**
   * Select objects in database.
   */
   
  void select_data();
  
  
  /**
   * Export data from database.
   */
   
  void export_data();
  
  
  /**
   * Return input ptr.
   */
   
  thinput * get_cfg_file() {return &(this->cfg_file);}
   
  
};


/**
 * Configuration module.
 */
 
extern thconfig thcfg;


#endif


