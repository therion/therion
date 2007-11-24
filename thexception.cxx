/**
 * @file thexception.cxx
 * String exception class.
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
 
#include "thexception.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef THWIN32
#define vsnprintf _vsnprintf
#endif


void thexception::appspf(const char * format, ...)
{
  char * desc = new char [8192];
  va_list args;
  va_start(args, format);
  vsnprintf(desc, 8192, format, args);
  this->strcat(desc);
  va_end(args);
  delete [] desc;
}


void thexception::insspf(const char * format, ...)
{
  char * desc = new char [8192];
  char * orig = new char [this->size];
  ::strcpy(orig, this->buff);
  va_list args;
  va_start(args, format);
  vsnprintf(desc, 8192, format, args);
  this->strcpy(desc);
  this->strcat(orig);
  va_end(args);
  delete [] orig;
  delete [] desc;
}


char * thexception::get_desc() {
  return this->get_buffer();
}


thexception thexc;


