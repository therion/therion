/**
 * @file therion.h
 * Main therion executable.
 *
 * Therion currently does following:
 *    - analyze the command line arguments
 *    - process the configuration file
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
 
#ifndef therion_h
#define therion_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef THDEBUG
#define thprint_error_src() thprintf2err("%s%s ("__FILE__":%d): error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__)
#else
#define thprint_error_src() thprintf2err("%s%s: error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd)
#endif


#ifdef THDEBUG
#define thprint_warning_src() thprintf2err("%s%s ("__FILE__":%d): warning -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__)
#else
#define thprint_warning_src() thprintf2err("%s%s: warning -- ", (thtext_inline ? "\n" : ""), thexecute_cmd)
#endif

/**
 * Error macro.
 *
 * Writes error message to stderr and terminates the program.
 */
 
#define therror(P) {\
  thprint_error_src();\
  thprintf2err P;\
  thprintf2err("\n");\
  thexit(EXIT_FAILURE);\
}


/**
 * Warning macro.
 *
 * Writes warning message to stderr.
 */
 
#define thwarning(P) {\
  thprint_warning_src();\
  thprintf2err P;\
  thprintf2err("\n");\
}

/**
 * Rewarning macro.
 */

#ifdef THDEBUG
#define threwarning2(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.appspf(" -- ");\
  thexc.appspf("("__FILE__":%d): ", __LINE__);\
  thexc.appspf P;\
  thwarning((thexc.get_desc()));\
  thexc.strcpy("");\
  }
#else
#define threwarning2(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.appspf(" -- ");\
  thexc.appspf P;\
  thwarning((thexc.get_desc()));\
  thexc.strcpy("");\
  }
#endif




/**
 * Verbose mode id.
 */
 
extern bool thverbose_mode;


/**
 * Command that executed therion.
 */

extern char * thexecute_cmd;

/**
 * Print formatted to stdout.
 */
 
void thprintf(const char *format, ...);


/**
 * Print formatted to stderr.
 */
 
void thprintf2err(const char *format, ...);


/**
 * Exit therion program.
 *
 * Do all necessary clean up.
 * @param exit_code Exit code of the program
 */

void thexit(int exit_code);

extern bool thtext_inline;

#endif


