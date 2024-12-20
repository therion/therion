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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef therion_h
#define therion_h

#include "thlogfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <fmt/printf.h>

#ifdef THDEBUG
#define thprint_error_src() thprintf2err("%s%s (" __FILE__ ":%d): error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__)
#else
#define thprint_error_src() thprintf2err("%s%s: error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd)
#endif


#ifdef THDEBUG
#define thprint_warning_src() thprintf2err("%s%s (" __FILE__ ":%d): warning -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__)
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
  thpause_exit();\
  therion_exit_state = 0;\
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
  therion_exit_state = 1;\
}


/**
 * Verbose mode id.
 */
 
extern bool thverbose_mode;


/**
 * Command that executed therion.
 */

extern char * thexecute_cmd;

/**
 * @brief Helper function for universal logging.
 * 
 * @param verbose print also to stdout
 * @param f output
 * @param format format string
 * @param args arguments to print
 */
template<typename FormatStr, typename... Args>
void thfprintf(const bool verbose, FILE* f, const FormatStr& format, const Args&... args)
{
  thlog.printf(format, args...);
  if (verbose) {
    fmt::fprintf(f, format, args...);
  }
}

/**
 * @brief Print formatted to stdout.
 * 
 * @param format format string
 * @param args arguments to print
 */
template <typename FormatStr, typename... Args>
void thprintf(const FormatStr& format, const Args&... args)
{
  thfprintf(thverbose_mode, stdout, format, args...);
}


/**
 * @brief Print formatted to stderr.
 * 
 * This function is used for error reporting, so it catches any additional
 * exceptions in order to not interfere with error handling.
 * 
 * @param format format string
 * @param args arguments to print
 */
template <typename FormatStr, typename... Args>
void thprintf2err(const FormatStr& format, const Args&... args) noexcept
{
  try
  {
    thfprintf(true, stderr, format, args...);
  }
  catch(const std::exception& e)
  {
    std::fprintf(stderr, "error occurred while reporting another error: %s\n", e.what());
  }
  catch (...)
  {
    std::fprintf(stderr, "unknown error occurred while reporting another error\n");
  }
} 


/**
 * Wait a bit.
 */
void thpause_exit();

/**
 * Exit therion program.
 *
 * Do all necessary clean up.
 * @param exit_code Exit code of the program
 */

[[noreturn]] void thexit(int exit_code);

extern bool thtext_inline;

extern int therion_exit_state;

void thprint_environment();
void thprint_xtherion();

#define thassert(expr) {if (!(expr)) {thprintf2err("%s%s (" __FILE__ ":%d): assertion failed ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__);exit(EXIT_FAILURE);}}

#endif


