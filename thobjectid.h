/**
 * @file thobjectid.h
 * Object identifier.
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
 
#ifndef thobjectid_h
#define thobjectid_h

/**
 * Therion object identifier.
 */
 
class thobjectid {

  public:  
  
  const char * name;  ///< Object name.
  unsigned long sid;  ///< Survey id.
  
  thobjectid() : name(""), sid(0) {};  ///< Standard constructor.
  
  /**
   * Default contructor.
   *
   * @param n Object name.
   * @param svid Survey identifier.
   */
   
  thobjectid(const char * n, unsigned long svid) : name(n), sid(svid) {}; 
  
  
  /**
   * Comparison operator.
   */
   
  friend bool operator < (const class thobjectid & o1, 
    const class thobjectid &o2);
};


#endif
