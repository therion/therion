/**
 * @file thdatareader.h
 * datareader module.
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
 
#ifndef thdatareader_h
#define thdatareader_h


#include "thinput.h"

class thdatabase;

/**
 * Main data reader class.
 *
 * Read and import therion text files.
 */

class thdatareader {

  thinput inp;   ///< Input files.
  thbuffer bf1, bf2;  ///< Buffers.
  thmbuffer mbf1;  ///< Multiple buffers.

  public:


  /**
   * Read input to database.
   *
   * @param ifname Input file name.
   * @param lnstart Start line where to start reading.
   * @param lnend End line where to stop reading.
   * @param spath Search path for input files.
   * @param dbptr Destination database.
   */
   
  void read(const char * ifname, long lnstart, long lnend, const char * spath, thdatabase * dbptr);


  /**
   * Return current input file.
   */
   
  thinput * get_cinf() {return &(this->inp);}
  
    
};


/**
 * Database reader module.
 */
 
extern thdatareader thdbreader;


/**
 * Return option position.
 */
   
unsigned long thdatareader_get_opos(bool inlineid, bool cfgid);


#endif


