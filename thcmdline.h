/**
 * @file thcmdline.h
 * Command line processing module.
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
 
#ifndef thcmdline_h
#define thcmdline_h


/**
 * Print states.
 */
 
enum {
  THPS_NONE = 1000,  ///< Nothing to query.
  THPS_ENCODINGS = 1001,  ///< Print encodings.
  THPS_INIT_FILE = 1002,  ///< Print init file.
  THPS_LIB_SRC = 1003,  ///< Print library source file
  THPS_XTHERION_SRC = 1004,  ///< Print point types list.
  THPS_PATHS = 1005,  ///< Print setting of all paths
  THPS_EXTERN_LIBS = 1006,  ///< Use extern libraries
  THPS_TEX_ENCODINGS = 1007,  ///< Print tex encodings
  THPS_SYMBOLS = 1008,  ///< Print symbols
};


/**
 * Command line processing class.
 *
 * Provides functions for therion command line options analysis.
 */

class thcmdline {

  public:

  bool version_ds,  ///< Version information display state.
    help_ds,  ///< Help display state.
    m_bezier,
    extern_libs;  ///< Use extern libs.
  int print_state;  ///< Query state.


  /**
   * Standard constructor.
   */
  
  thcmdline();
  
  
  /**
   * Standard destructor.
   */
   
  ~thcmdline();
  
  
  /**
   * Process given command line.
   */
   
  void process(int argc, char * argv[]);
  
  
  /**
   * Return version disp state.
   */
   
  bool get_version_disp_state();


  /**
   * Return help disp state.
   */
   
  bool get_help_disp_state();
  
  
  /**
   * Return print state.
   */
   
  int get_print_state();

};


/**
 * Command line processing module.
 */
 
extern thcmdline thcmdln;


#endif


