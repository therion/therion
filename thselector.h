/**
 * @file thselector.h
 * Database selector class.
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
 
#ifndef thselector_h
#define thselector_h

#include <list>
#include <stdio.h>
#include "thlayoutclr.h"


/**
 * Selection.
 */
 
class thselector_item {

  public:

  const char * name, *src_name;
  unsigned long src_ln;
  unsigned long number = 0;
  bool unselect, recursive;  
  long map_level, chapter_level;
  thlayout_color m_color;
  class thdataobject * optr;
  
  thselector_item () : name(""), src_name(""), src_ln(0), unselect(false), recursive(true),
      map_level(0), chapter_level(0), optr(NULL) {}
  
};


typedef std::list <thselector_item> thselector_list;


/**
 * Selector class.
 */
 
class thselector {

  public:
  
  thselector();  ///< Default constructor.

  class thconfig * cfgptr;  ///< Current config pointer.
  
  thselector_list data;  ///< Selection data.
  
  void select_object(thselector_item * pitm, class thdataobject * optr);  ///< Select object.
  
  void select_all(thselector_item * pitm, class thdatabase * db);  ///< Select all objects in db.
  
  void select_survey(thselector_item * pitm, class thsurvey * srv);  ///< Select survey.

  unsigned long number;
    
  void assign_config(class thconfig * cptr);  ///< Assign configuration object.
  

  /**
   * Parse select option from config file.
   */
   
  void parse_selection (bool usid, int nargs, char ** args);
  
  
  /**
   * Dump selection into file.
   */
   
  void dump_selection (FILE * cf);


  /**
   * Dump database selection options into file.
   */
   
  void dump_selection_db (FILE * cf, class thdatabase * db);
  
  
  /**
   * Select objects in database.
   */
  
  void select_db(class thdatabase * db);
  
};


#endif


