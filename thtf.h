/**
 * @file thtf.h
 * Units transformation class.
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
 
#ifndef thtf_h
#define thtf_h


/**
 * Transformation class.
 *
 * Parent class for all units transformations.
 */
 
class thtf {

  public:

  double ufactor,  ///< Units factor.
    sfactor;  ///< Scale factor.
 
  int units;

  public:
  
  /**
   * Standard constructor.
   */
  
  thtf() : ufactor(1.0), sfactor(1.0) {}
  virtual ~thtf();
  
  
  /**
   * Parse units factor.
   */
   
  virtual void parse_units(char * ustr) = 0;



  /**
   * Return units.
   */
   
  virtual int get_units();  
  
  
  /**
   * Parse scale factor.
   */
   
  void parse_scale(char * sstr);
  
  
  /**
   * Make units transformation on number.
   */
  
  virtual double transform(double value);
  
};


#endif


