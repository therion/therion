/**
 * @file therion.cxx
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
 
#include "therion.h"
#include "thlogfile.h"
#include "thtmpdir.h"
#include "thcmdline.h"
#include "thconfig.h"
#include "thinput.h"
#include "thchenc.h"
#include "thdatabase.h"
#include "thdatareader.h"
#include "thexception.h"
#include "thlibrary.h"
#include "thinit.h"
#include "thgrade.h"
#include "thlayout.h"
#include "thobjectsrc.h"
#include "thpoint.h"
#include "thline.h"
#include "thversion.h"
#include "thtexfonts.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

bool thverbose_mode = true;

bool thtext_inline = false;

char * thexecute_cmd = NULL;

const char * thhelp_text =
      "\ntherion [-q] [-L] [-l log-file]\n"
      "\t[-s source-file] [-p search-path]\n"
      "\t[-g|-u] [-i] [-d] [-x] [cfg-file]\n\n"
      "therion [-h|--help]\n"
      "        [-v|--version]\n"
      "        [--print-encodings]\n"
      "        [--print-init-file]\n"
      "        [--print-tex-encodings]\n\n";

const char * thversion_text = THVERSION;
const char * thversion_format = "therion %s";


void thprintf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  thlog.vprintf(format, &args);
  va_end(args);
  if (thverbose_mode) {
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
  }
}
  

void thprintf2err(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  thlog.vprintf(format, &args);
  va_end(args);
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}


void thprint_environment() {
  thprintf("INIT=%s\n",thcfg.get_initialization_path());
  thprintf("SOURCE=%s\n",thcfg.get_search_path());
  thprintf("CAVERN=%s\n",thini.get_path_cavern());
  thprintf("METAPOST=%s\n",thini.get_path_mpost());
  thprintf("PDFTEX=%s\n",thini.get_path_pdftex());
}


void thprint_xtherion() {
  thprintf("set xth(point_types) {\n");
  for(int i = 0; thtt_point_types[i].tok != TT_POINT_TYPE_UNKNOWN; i++) {
    thprintf("\t%s\n",thtt_point_types[i].s);
  }
  thprintf("}\n\nset xth(line_types) {\n");
  for(int i = 0; thtt_line_types[i].tok != TT_POINT_TYPE_UNKNOWN; i++) {
    thprintf("\t%s\n",thtt_line_types[i].s);
  }
  thprintf("}\n");
}


void thprint_xth_lines() {
}


void thpause_exit() {
#ifdef THWIN32
  if (thverbose_mode && (!(thcfg.generate_xthcfg))) {
    thprintf("Press ENTER to exit!");
    getchar();
  }
#endif
}


void thexit(int exit_code)
{
  exit(exit_code);
}  


int main(int argc, char * argv[]) {

  try {

    time_t tmUserStart = time(NULL);
    
    // turn off buffering
    setvbuf(stdout,NULL,_IONBF,0);
    thcfg.assign_db(&thdb);

    // set some system parameters
    thexecute_cmd = argv[0];
  
    // process command line
    thcmdln.process(argc, argv);
    
    
    // print some help messages and exit if demanded
    if (thcmdln.get_version_disp_state())
    {
      thprintf(thversion_format, thversion_text);
      thprintf("\n");
      thexit(EXIT_SUCCESS);
    }
    
    if (thcmdln.get_help_disp_state()) 
    {
      thprintf(thhelp_text);
      thexit(EXIT_SUCCESS);
    }
    
    switch (thcmdln.get_print_state()) {
      case THPS_ENCODINGS:
        thprint_encodings();
        thexit(EXIT_SUCCESS);
      case THPS_TEX_ENCODINGS:
        print_tex_encodings();
        thexit(EXIT_SUCCESS);
      case THPS_INIT_FILE:
        thprint_init_file();
        thexit(EXIT_SUCCESS);
      case THPS_XTHERION_SRC:
        thprint_xtherion();
        thexit(EXIT_SUCCESS);
    }
    
    // print version information
    thprintf(thversion_format, thversion_text);
    thprintf("\n");
    
    // load initialization file
#ifdef THDEBUG
    thprintf("\nloading initialization file\n");
#else
    thprintf("reading initialization file ...");
    thtext_inline = true;
#endif 
    thini.load();
#ifdef THDEBUG
    thprintf("\n");
#else
    thprintf(" done\n");
    thtext_inline = false;
#endif 
    
    // initialize database
    thdb.clear();
    // load therion library
    thlibrary_init();

    // load configuration from file
#ifdef THDEBUG
    thprintf("\nloading configuration -- %s\n", thcfg.get_file_name());
#else
    thprintf("reading configuration file ...");
    thtext_inline = true;
#endif 
    thcfg.load();
#ifdef THDEBUG
    thprintf("configuration loaded\n\n");
#else
    thprintf(" done\n");
    thtext_inline = false;
#endif    
    
    // load input
    long sid, maxsid = thcfg.get_source_file_names()->get_size();
    if (maxsid == 0)
      therror(("source files not specified"));
    char ** srcn = thcfg.get_source_file_names()->get_buffer();
#ifndef THDEBUG
    thprintf("reading source files ... ");
    thtext_inline = true;
#endif 


    for(sid = 0; sid < maxsid; sid++, srcn++) {

#ifdef THDEBUG
      thprintf("\nreading input -- %s\n", *srcn);
#endif 

      thdbreader.read(*srcn, 
        thcfg.get_search_path(), &thdb);

#ifdef THDEBUG
    thprintf("input read\n");
#endif 

    }

#ifndef THDEBUG
    thprintf("done\n");
    thtext_inline = false;
#endif 

    // After reading printing
    switch (thcmdln.get_print_state()) {
      case THPS_LIB_SRC:
        thdb.self_print_library();
        thexit(EXIT_SUCCESS);
      case THPS_PATHS:
        thprint_environment();
        thexit(EXIT_SUCCESS);
    }

    // process survey data
    thdb.db1d.process_data();

    // process 2D references
    thdb.db2d.process_references();

    // selecting objects for output
#ifdef THDEBUG
    thprintf("\n\nselecting export objects\n");
#else
    thprintf("selecting export objects ... ");
    thtext_inline = true;
#endif 
    thcfg.select_data();
#ifdef THDEBUG
#else
    thprintf("done\n");
    thtext_inline = false;
#endif

    // make export
    thcfg.export_data();

    // save configuration
    thcfg.save();
    thcfg.xth_save();

#ifdef THDEBUG
    thprintf("\n\nlisting database\n");
    if (thverbose_mode)
      thdb.self_print(stdout);
    thdb.self_print(thlog.get_fileh());
    thprintf("\ndatabase listed\n\n");
#endif   

    // write the CPU time
    thprintf("compilation time: %.0f sec\n", difftime(time(NULL), tmUserStart));
#ifdef THDEBUG
    thprintf("\n");
#endif   
    
    //exit the program
    return(EXIT_SUCCESS);
    
  }
  catch(...)
  {
    if (strlen(thexc.get_buffer()) == 0) {
      therror(("unknown exception"));  
    } else {
      therror((thexc.get_desc()));
    }
  }
}


