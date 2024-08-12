/**
 * @file thinput.h
 * Input file processing class.
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
 
#ifndef thinput_h
#define thinput_h

#include "thbuffer.h"
#include "thmbuffer.h"
#include <fstream>
#include <memory>


/**
 * Input file processing class.
 *
 * This class provides standard functions, that work with therion input
 * and configuration files.
 */
 
class thinput {

  public:

  bool cmd_sensitivity,  ///< Command sensitivity ID.
    input_sensitivity,  /// < Whether dive into subfiles.
    scan_search_path,  ///< Whether to scan also search path for input files.
    pifo, ///< Print if open.
    * pifoid, ///< Identifier.
    report_missing; 
  thbuffer file_name;  ///< Main input file name.
  thmbuffer search_path,  ///< Search paths.
    file_suffix,  ///< File suffixes.
    tmpmb;   ///< Temporary multi buffer.
  void (* pifoproc)(char *);  ///< Function to call if file was opened.
  thbuffer linebf,  ///< Line buffer.
    cmdbf,  ///< Command buffer.
    valuebf;  ///< Value buffer.
    
  static const long max_line_size;
  std::unique_ptr<char[]> lnbuffer;
  

  /**
   * Therion input file structure.
   */
  class ifile {
  
    public:
    
    std::ifstream sh;   ///< file stream.
    thbuffer name,  ///< Input file name buffer.
      path;  ///< Input file path buffer.
    unsigned long lnumber;  /// Position at the file.
    int encoding;  /// Current file encoding.
    ifile * prev_ptr;  /// Pointer to the upper file.
    std::unique_ptr<ifile> next_ptr;  /// Pointer to the lower file.
      
    
    /**
     * Standard constructor.
     */
    
    ifile(ifile * fp);
    
    /**
     * Close file if it's open.
     */
     
    void close();
    
    
    /**
     * Check if file statistics are equal.
     */
     
    bool is_equal(ifile* f);
    
  };
  
  
  std::unique_ptr<ifile> first_ptr;  ///< Pointer to the first file.
  ifile * last_ptr;  ///< Pointer to the last file.


  bool is_first_file();


  /**
   * Open new file for input.
   */
   
  void open_file(char * fname);
  
  
  /**
   * Close file open for input.
   */
   
  void close_file();
  
  
  public:  
  
  /**
   * Standard constructor.
   */
  
  thinput();

  /**
   * Set command sensitivity state.
   */
   
  void set_cmd_sensitivity(bool cs);
  
  
  /**
   * Turn on command sensitivity.
   */
   
  void cmd_sensitivity_on();
  
  
  /**
   * Turn off command sensitivity.
   */
   
  void cmd_sensitivity_off();
  
  
  /**
   * Retrieve command sensitivity.
   */
   
  bool get_cmd_sensitivity();
  
  
  /**
   * Set search path scanning state.
   */
   
  void set_sp_scan(bool spss);
  
  
  /**
   * Turn on search path scanning for input files.
   */
   
  void sp_scan_on();
  
  
  /**
   * Turn off search path scanning for input files.
   */
   
  void sp_scan_off();
  
  
  /**
   * Retrieve search path scanning state.
   */
   
  bool get_sp_scan();
  
  
  /**
   * Set main input file name.
   */
   
  void set_file_name(const char * fname);
  
  
  /**
   * Retrieve main input file name.
   */
   
  char * get_file_name();
  
  
  /**
   * Set search path.
   */
   
  void set_search_path(const char * spath);

  /**
   * Set possible file suffixes.
   *
   * Used, when unable to open input file.
   */
   
  void set_file_suffix(const char * fsx);

  bool get_input_sensitivity();
  void set_input_sensitivity(bool s);
  
  
  /**
   * Reset input.
   */
   
  void reset();
	
	
	/**
	 * Reads line from input and return pointer to it.
   *
   * In fact, it does following:
   * - ignore comment lines (starting with #)
   * - join lines ended with backslash
   * - if command sensitivity is on, process also <TT>input</TT> and
   *   <TT>encoding</TT> commands.
   * - if also search path is enabled, program search input files
   *   also there
   * - if some suffixes are set, program tries to find files
   *   by adding these suffixes.
	 */
	 
	char * read_line();
	
	
	/**
	 * Return command string.
	 *
	 * Applicable only in when cmd_sensitivity is on.
	 */
	 
	char * get_cmd();
	
	/**
	 * Return line string.
   */
   
	char * get_line();
	
	/**
	 * Return value string.
	 *
	 * Applicable only in when cmd_sensitivity is on.
	 */
	 
  char * get_value();
  

  /**
   * Return full current input file name.
   */  
   
  char * get_cif_name();


  /**
   * Return current input file path.
   */  
   
  char * get_cif_path();
  
  /**
   * Return current absolute input file path.
   */  
   
  std::string get_cif_abspath(const char * fname_ptr = "");
  
  /**
   * Return current input line number.
   */
   
  unsigned long get_cif_line_number();
  
  
  /**
   * Return current input encoding.
   */
  
  int get_cif_encoding();
  
  /**
   * Text to print some file was opened.
   */
   
  void print_if_opened(void (* pifop)(char *), bool * printed);
   
};

#endif


