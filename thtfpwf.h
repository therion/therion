/**
 * @file thtfpwf.h
 * Piece wise function.
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
 
#ifndef thtfpwf_h
#define thtfpwf_h
#include <cstddef>
#include <vector>


/**
 * Piecewise function element.
 */


struct thtfpwfxy {
  double x, y;
};


/**
 * Piecewise function class.
 *
 * If two elements are defined, makes makes (x - b) * a transformation.
 */
 
class thtfpwf {

  double a,  ///< Multiplier.
    b;  ///< Addition.
    
  std::vector<thtfpwfxy> values;

  public:
  
  /**
   * Standard constructor.
   */
  
  thtfpwf() : a(1.0), b(0.0) {}
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thtfpwf() = default;
  
  
  /**
   * Parse function.
   */
   
  virtual void parse(int nfact, char ** sfact);
  
  
  /**
   * Make units transformation on number.
   */
  
  double evaluate(double value);
  
  
  /**
   * Set function values.
   */
   
  void set(size_t nv, double * pv);
  

  /**
   * Return function values.
   */
  
  thtfpwfxy * get_values();
  
  
  /**
   * Return number of function values.
   */
   
  size_t get_size();
  
};


#endif


