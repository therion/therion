/**
 * @file thobjectname.h
 * Object name class.
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
 
#ifndef thobjectname_h
#define thobjectname_h

#include "thmbuffer.h"
#include "therion.h"

/**
 * Survey station class.
 */

class thobjectname {

  public:
  
  const char * name,  ///< Object name.
      * survey;  ///< Survey name.
       
  class thsurvey * psurvey; ///< Parent survey.
       
  unsigned long id;  ///< Object identifier.

  /**
   * Standard constructor.
   */
  
  thobjectname();
  
  
  /**
   * Two argument constructor.
   */
   
  thobjectname(char * n, char * s) : name(n), survey(s), id(0) {}
  
  /**
   * Clear station.
   */
   
  void clear();
  
  
  /**
   * Return true if empty.
   */
 
  bool is_empty();
  

  /**
   * Print object name with survey into str.
   */

  char * print_name();
  

  /**
   * Print object name with survey up to given level into str.
   */

  char * print_full_name(int slevel = -1);
    
};

void thparse_objectname(thobjectname & ds, thmbuffer * sstore, char * src, class thdataobject * psobj = NULL);

char * thobjectname__print_full_name(const char * oname, class thsurvey * psrv, int slevel = -1);

void fprintf(FILE * fh, thobjectname & ds);

#endif


