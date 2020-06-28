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
#include "thcmdline.h"
#include "thconfig.h"
#include "thdatareader.h"
#include "thexception.h"
#include "thlibrary.h"
#include "thinit.h"
#include "thversion.h"
#include "thtexfonts.h"
#include "thbezier.h"
#include "thlogfile.h"
#include "thproj.h"

extern const thstok thtt__texts [];

const char * thhelp_text =
      "\ntherion [-q] [-L] [-l log-file]\n"
      "\t[-s source-file] [-p search-path]\n"
      "\t[-b|--bezier]\n"
      "\t[-d] [-x] [cfg-file]\n\n"
      "therion [-h|--help]\n"
      "        [-v|--version]\n"
      "        [--print-encodings]\n"
      "        [--print-environment]\n"
      "        [--print-init-file]\n"
      "        [--print-library-src]\n"
      "        [--print-symbols]\n"
      "        [--print-tex-encodings]\n"
      "        [--print-xtherion-src]\n"
      "        [--reproducible-output]\n\n";

const char * thversion_text = THVERSION;
const char * thversion_format = "therion %s";

int main(int argc, char * argv[]) {

#ifndef THDEBUG
#ifndef THMSVC
  try {
#endif
#endif  

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
      thprintf("  - using Proj %s\n", thcs_get_proj_version().c_str());
      thexit(EXIT_SUCCESS);
    }

    if (thcmdln.m_bezier)
    {
      thbezier_main();
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
    thlog.printf("  - using Proj %s\n", thcs_get_proj_version().c_str());
    
    // load initialization file
    thini.load();
    // initialize database
    thdb.clear();
    // load therion library
    thlibrary_init();


    switch (thcmdln.get_print_state()) {
      case THPS_SYMBOLS:
        export_all_symbols();
        thexit(EXIT_SUCCESS);
    }



    // load configuration from file
    thcfg.load();

    switch (thcmdln.get_print_state()) {
      case THPS_PATHS:
        thprint_environment();
        thexit(EXIT_SUCCESS);
    }
    
    // load input
    thconfig_src_list * sources = thcfg.get_sources();
    thconfig_src_list::iterator srcit;
    if (sources->size() == 0)
      therror(("source files not specified"));
#ifndef THDEBUG
    thprintf("reading source files ... ");
    thtext_inline = true;
#endif 


    for(srcit = sources->begin(); srcit != sources->end(); srcit++) {

#ifdef THDEBUG
      thprintf("\nreading input -- %s\n", srcit->fname);
#endif 

      thdbreader.read(srcit->fname, srcit->startln, srcit->endln, 
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
    }

    // process 2D references
    thdb.preprocess();

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

    // log statistics
    thdb.db1d.print_loops();
    thdb.db2d.log_distortions();
    // log CRS transformations
    thcs_log_transf_used();

//#ifdef THMSVC
//    getchar();
//#endif

    //exit the program
    return(EXIT_SUCCESS);
    
#ifndef THDEBUG
#ifndef THMSVC
  }
  catch(...)
  {
    if (strlen(thexc.get_buffer()) == 0) {
      therror(("unknown exception"));  
    } else {
      therror((thexc.get_desc()));
    }
  }
#endif
#endif

}
