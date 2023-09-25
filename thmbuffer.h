/**
 * @file thmbuffer.h
 * Multiple string buffer class.
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
 
#ifndef thmbuffer_h
#define thmbuffer_h

#include <cstddef>

/**
 * Multiple string buffer class.
 */
 
class thmbuffer {

  long size,  ///< Number of items in buffer.
    max_size,  ///< Maximal number of items in buffer.
    free_size;
  char * free_ptr;  ///< Pointer to the free string position.
  char ** buf;  ///< Access array.

  /**
   * Memory allocation class.
   */
   
  class mblock {
  
    public:
    
    char * data;  ///< Pointer to the memory block.
    size_t size;  ///< Size of the block.
    mblock * next_ptr;  ///< Pointer to the next block.
    
    
    /**
     * Constructor.
     *
     * Allocates also necessary memory.
     */
     
    mblock(size_t min_size, size_t last_size);
    
    
    /**
     * Destructor.
     *
     * Deallocates data memory.
     */
     
    ~mblock();

    // These operations are not implemented.
    mblock(const mblock&) = delete;
    mblock(mblock&&) = delete;
    mblock& operator=(const mblock&) = delete; 
    mblock& operator=(mblock&&) = delete; 
  };

  mblock * first_ptr,  ///< Pointer to the first memory block.
    * last_ptr;  ///< Pointer to the last memory block.

  public:
  
  
  /**
   * Standard constructor.
   */
  
  thmbuffer();
  
  // These operations are not implemented.
  thmbuffer(const thmbuffer&) = delete;
  thmbuffer(thmbuffer&&) = delete;
  thmbuffer& operator=(const thmbuffer&) = delete; 
  thmbuffer& operator=(thmbuffer&&) = delete; 
  
  /**
   * Destructor.
   */
  
  ~thmbuffer();
  
  
  /**
   * Clear the buffer contents.
   */
   
  void clear();
  
  
  /**
   * Append new string to the buffer.
   *
   * @param src Source string.
   */
   
  char * append(const char * src);
  
  
  /**
   * Append n characters from string as a new string to the buffer.
   *
   * @param src Source string.
   * @param n Source size.
   */
   
  char * appendn(const char * src, size_t n);
  
  
  /**
   * Return number of items in buffer.
   */
   
  long get_size();
  
  
  /**
   * Return pointer to the array of items.
   */
   
  char ** get_buffer();
  

};

#endif


