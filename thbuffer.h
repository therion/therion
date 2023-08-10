/**
 * @file thbuffer.h
 * String buffer class.
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
 
#ifndef thbuffer_h
#define thbuffer_h

#include <cstddef>

/**
 * String buffer class.
 */
 
class thbuffer {

  public:

  char * buff;  ///< Buffer.
  size_t size;  ///< Buffer size.

  /**
   * Enlarge the buffer at least to given size.
   *
   * @param min_size Minimal buffer size.
   */
   
  void enlarge(size_t min_size);

  
  public:
  
  
  /**
   * Standard constructor.
   */
  
  thbuffer();
  
  // Only copy assignment is implemented.
  thbuffer(const thbuffer&) = delete;
  thbuffer(thbuffer&&) = delete;
  thbuffer& operator=(thbuffer&&) = delete;
  
  /**
   * Destructor.
   */
  
  ~thbuffer();
  
  
  /**
   * Copy given string to buffer and return pointer to it.
   * @param src Given string.
   */
   
  char * strcpy(const char * src);


  /**
   * Copy n characters from given string to buffer and return pointer to it.
   *
   * Also add 0 to the end.
   * @param src Given string.
   * @param n Source length.
   */
   
  char * strncpy(const char * src, size_t n);
  
  
  /**
   * Concatenate given string and buffer and return pointer to it.
   * @param src Given string.
   */
   
  char * strcat(const char * src);
  
  
  /**
   * Concatenate n characters from given string and 
   * buffer and return pointer to it.
   * @param src Given string.
   */
   
  char * strncat(const char * src, size_t n);
  

  /**
   * Guarantee buffer size.
   *
   * @param bs Minimal buffer size.  
   */
   
  void guarantee(size_t bs);
  
  
  /**
   * Return pointer to the buffer.
   */
   
  char * get_buffer();
  
  
  /**
   * Type conversion to char*.
   */
   
  operator char* ();
  
  
  /**
   * Type conversion to char*.
   */
   
  operator char** ();
  
  
  /**
   * "=" operator.
   */
   
  thbuffer & operator=(const char * src);


  /**
   * "=" operator.
   */
   
  thbuffer & operator=(thbuffer const & srcbf);


  /**
   * "+=" operator.
   */
   
  thbuffer & operator+=(const char * src);
  

};

#endif


