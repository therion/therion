/**
 * @file thcmdline.cxx
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

#include "thcmdline.h"
#include "therion.h"
#include "thlogfile.h"
#include "thconfig.h"
//#include <getopt.h>
#include "thtmpdir.h"

#ifdef THMSVC
#define __STDC__ 1
#endif
#include "extern/getopt.h"


thcmdline::thcmdline()
{
  this->version_ds = false;
  this->help_ds = false;
  this->m_bezier = false;
  this->extern_libs = false;
  this->print_state = THPS_NONE;
}


thcmdline::~thcmdline()
{
}


bool thcmdline::get_version_disp_state()
{
  return(this->version_ds);
}


bool thcmdline::get_help_disp_state()
{
  return(this->help_ds);
}


void thcmdline::process(int argc, char * argv[])
{

  // 1. let's search for options
  int oc;
  int oindex = 0;
  static struct option thlong_options[] = 
  {
    {"help",no_argument,NULL,'h'},
    {"print-encodings",no_argument,NULL,THPS_ENCODINGS},
    {"print-environment",no_argument,NULL,THPS_PATHS},
    {"print-init-file",no_argument,NULL,THPS_INIT_FILE},
    {"print-library-src",no_argument,NULL,THPS_LIB_SRC},
    {"print-symbols",no_argument,NULL,THPS_SYMBOLS},
    {"print-tex-encodings",no_argument,NULL,THPS_TEX_ENCODINGS},
    {"print-xtherion-src",no_argument,NULL,THPS_XTHERION_SRC},
    {"use-extern-libs",no_argument,NULL,THPS_EXTERN_LIBS},
    {"version",no_argument,NULL,'v'},
    {"bezier",no_argument,NULL,'b'},
    {NULL, 0, NULL, 0}
  };

  while(1) {

    oc = getopt_long (argc, argv, "dxs:l:qLvbhp:",
      thlong_options, &oindex);
    
    // no other options detected
    if (oc == -1)
      break;  
      
    switch (oc)
    {
    
      case 'd':
        thtmp.debug = true;
        thtmp.delete_all = false;
        break;
        
      case 'h':
        this->help_ds = true;
        break;
      
      case 'v':
        this->version_ds = true;
        break;
      
      case 'b':
        this->m_bezier = true;
        break;
        
      case 'q':
        thverbose_mode = false;
        break;

      case 'L':
        thlog.logging_off();
        break;
        
      case 'l':
        thlog.logging_on();
        thlog.set_file_name(optarg);
        break;
        
      //case 'g':
      //  thcfg.set_file_state(THCFG_GENERATE);
      //  break;
        
      case 'x':
        thcfg.generate_xthcfg = true;
        break;
        
      //case 'u':
      //  thcfg.set_file_state(THCFG_UPDATE);
      //  break;
        
      //case 'i':
      //  thcfg.comments_skip_on();
      //  break;
        
      case 's':     
        thcfg.append_source(optarg);
        break;
      
      case 'p':  
        thcfg.set_search_path(optarg);
        break;
        
      case THPS_ENCODINGS:
        this->print_state = THPS_ENCODINGS;
        break;

      case THPS_TEX_ENCODINGS:
        this->print_state = THPS_TEX_ENCODINGS;
        break;

      case THPS_SYMBOLS:
        this->print_state = THPS_SYMBOLS;
        break;

      case THPS_XTHERION_SRC:
        this->print_state = THPS_XTHERION_SRC;
        break;

      case THPS_INIT_FILE:
        this->print_state = THPS_INIT_FILE;
        break;

      case THPS_LIB_SRC:
        this->print_state = THPS_LIB_SRC;
        break;
     
      case THPS_PATHS:
        this->print_state = THPS_PATHS;
        break;
        
      case THPS_EXTERN_LIBS:
        this->extern_libs = true;
        break;
        
    }
      
  }


  // 2. let's search for program arguments
  if (optind < argc)
    thcfg.set_file_name(argv[optind++]);
  if (optind < argc)
    thwarning(("too many input arguments"));


}


int thcmdline::get_print_state()
{
  return this->print_state;
}


thcmdline thcmdln;


