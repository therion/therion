/**
 * @file thlogfile.h
 * Log file module.
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
 
#ifndef thlogfile_h
#define thlogfile_h

#include "thbuffer.h"
#include <stdio.h>
#include <stdarg.h>

/**
 * Log file module.
 *
 * Provides work with log file.
 */
 
class thlogfile {

  thbuffer file_name;  ///< Name of log file.
  FILE* fileh;  ///< File handler.
  bool is_open;  ///< File is open ID.
  bool is_warned;  ///< File couldn't be opened ID.
  bool is_logging;  ///< Loggind ID.
  
  /**
   * Open log file for output.
   */
   
  void open_file();


  /**
   * Close log file.
   */
   
  void close_file();
  
  
  public:
  
  /**
   * Standard constructor.
   */
   
  thlogfile();
  
  
  /**
   * Standard destructor.
   */
   
  ~thlogfile();
  
  
  /**
   * Print formatted into log file.
   */
   
  void vprintf(const char *format, va_list *args);


  /**
   * Print formatted into log file.
   */
   
  void printf(const char * format, ...);
  
  
  /**
   * Set log file name.
   *
   * @param fname File name.
   */
   
  void set_file_name(char *fname);
  
  
  /**
   * Retrieve log file name.
   */
   
  const char* get_file_name();
  
  
  /**
   * Set logging state.
   */
   
  void set_logging(bool log_io);


  /**
   * Retrieve logging state.
   */
   
  bool get_logging();
  
  
  /**
   * Turn on logging.
   */
   
  void logging_on();
  
  
  /**
   * Turn off logging.
   */
   
  void logging_off();
	

	/**
	 * Logging error.
	 */ 	
	void log_error();
  
  
  /**
   * Return log file handler.
   */
   
  FILE * get_fileh();
  
};


/**
 * Log file module.
 */
 
extern thlogfile thlog;

#endif


