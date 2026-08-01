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
 
#pragma once

#include <source_location>
#include <string_view>

/**
 * Report error.
 *
 * Writes error message to stderr and terminates the program.
 */
[[noreturn]] void therror(std::string_view message, std::source_location loc = std::source_location::current());

/**
 * Report warning.
 *
 * Writes warning message to stderr.
 */
void thwarning(std::string_view message, std::source_location loc = std::source_location::current());

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

/**
 * Assert condition.
 *
 * If the condition is false, prints error message and exits the program.
 * @param cond Condition to check.
 * @param loc Source location for better debugging.
 */
void thassert(bool cond, std::source_location loc = std::source_location::current());
