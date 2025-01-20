/**
 * @file thdatastation.h
 * datastation module.
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
 
#ifndef thdatastation_h
#define thdatastation_h


#include "thdataobject.h"

/**
 * Datastation class.
 *
 * Only virtual class.
 */

class thdatastation : public thdataobject {

  public:

  /**
   * Return class identifier.
   */
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thdatastation";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  bool is(int class_id) override;
};


#endif


