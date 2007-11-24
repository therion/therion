/**
 * @file thexception.h
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
 
#ifndef thexception_h
#define thexception_h


#include "therion.h"
#include "thbuffer.h"


/**
 * Exception throwing macro.
 */
//  thexc.strcpy("");

#ifdef THDEBUG
#define ththrow(P) {\
  thexc.strcpy("");\
  thexc.appspf("("__FILE__":%d): ", __LINE__);\
  thexc.appspf P;\
  throw(0);\
  }
#else
#define ththrow(P) {\
  thexc.strcpy("");\
  thexc.appspf P;\
  throw(0);\
  }
#endif


/**
 * Exception rethrowing macro.
 */

#ifdef THDEBUG
#define threthrow(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.insspf(" -- ");\
  thexc.insspf P;\
  thexc.insspf("("__FILE__":%d): ", __LINE__);\
  throw(0);\
  }
#else
#define threthrow(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.insspf(" -- ");\
  thexc.insspf P;\
  throw(0);\
  }
#endif


/**
 * Exception throwing macro without buffer reset.
 */

#ifdef THDEBUG
#define threthrow2(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.appspf(" -- ");\
  thexc.appspf("("__FILE__":%d): ", __LINE__);\
  thexc.appspf P;\
  throw(0);\
  }
#else
#define threthrow2(P) {\
  if (*(thexc.get_buffer()) == 0)\
    thexc.strcpy("unknown exception");\
  thexc.appspf(" -- ");\
  thexc.appspf P;\
  throw(0);\
  }
#endif





/**
 * Therion exception class.
 */
 
class thexception : public thbuffer {

  public:
  
  /**
   * Append sprinted string.
   */
   
  void appspf(const char * format, ...);
  
    
  /**
   * Insert sprinted string.
   */
   
  void insspf(const char * format, ...);
  
    
  /**
   * Return exception description.
   */
   
  char * get_desc();
  

};


extern thexception thexc;

#endif


