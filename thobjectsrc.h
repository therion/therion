/**
 * @file thobjectsrc.h
 * Object source class.
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
 
#ifndef thobjectsrc_h
#define thobjectsrc_h

#include <cstdlib>


/**
 * Object source class.
 */

class thobjectsrc {

  public:
  
  char * name;  ///< File name.
       
  unsigned long line;  ///< File line.

  class thdataobject * context;  ///< Source context.

  /**
   * Standard constructor.
   */
  
  thobjectsrc() : name(""), line(0), context(NULL) {}
  
  /**
   * Parametric constructor.
   */
  
  thobjectsrc(char * n, unsigned long ln) : name(n), line(ln), context(NULL) {}
    
};

#endif


