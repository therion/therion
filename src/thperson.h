/**
 * @file thperson.h
 * Person class.
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
 
#ifndef thperson_h
#define thperson_h

/**
 * Person class.
 */
 
class thperson {

  char * n1,  ///< First name.
    * n2,  ///< Second name.
    * nn;  ///< Identification string.
    
  void reset();  ///< Reset person names.
  
  void identify(class thdatabase * dbp);   ///< Make identification string.
  
  friend bool operator < (const class thperson &, const class thperson &);
  friend bool operator == (const class thperson &, const class thperson &);
  

  public:
    
  /**
   * Standard constructor.
   */
  
  thperson();
  
  
  /**
   * Parse the person name.
   */
   
  void parse(thdatabase * dbp, char * src);
  
  
  /**
   * Return first name.
   */
   
  char * get_n1() const;


  /**
   * Return second name.
   */
   
  char * get_n2() const;
  
};


#endif


