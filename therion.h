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

#include <stdio.h>
#include <stdlib.h>
#include <fmt/printf.h>

#ifdef THDEBUG
#define thprint_error_src() thprint2err(fmt::format("{}{} (" __FILE__ ":{}): error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__))
#else
#define thprint_error_src() thprint2err(fmt::format("{}{}: error -- ", (thtext_inline ? "\n" : ""), thexecute_cmd))
#endif


#ifdef THDEBUG
#define thprint_warning_src() thprint2err(fmt::format("{}{} (" __FILE__ ":{}): warning -- ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__))
#else
#define thprint_warning_src() thprint2err(fmt::format("{}{}: warning -- ", (thtext_inline ? "\n" : ""), thexecute_cmd))
#endif

/**
 * Error macro.
 *
 * Writes error message to stderr and terminates the program.
 */
 
#define therror(P) {\
  thprint_error_src();\
  thprint2err(fmt::sprintf P );\
  thprint2err("\n");\
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
  thprint2err(fmt::sprintf P );\
  thprint2err("\n");\
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
 * @brief Print to the logfile and stdout.
 * 
 * @param msg message to print
 */
void thprint(std::string_view msg);

/**
 * @brief Print to stderr.
 * 
 * This function is used for error reporting, so it catches any additional
 * exceptions in order to not interfere with error handling.
 * 
 * @param msg message to print
 */
void thprint2err(std::string_view msg) noexcept;

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

#define thassert(expr) {if (!(expr)) {thprint2err(fmt::format("{}{} (" __FILE__ ":{}): assertion failed ", (thtext_inline ? "\n" : ""), thexecute_cmd, __LINE__));exit(EXIT_FAILURE);}}

#endif


