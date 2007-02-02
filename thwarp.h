/**
 * @file thwarp.h
 * Export class.
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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
 
#ifndef thwarp_h
#define thwarp_h

#include "thpic.h"
#include "thsketch.h"


class thwarp {

  protected:

  thsketch * m_sketch;

  public:

  thwarp() : m_sketch(NULL) {}

  virtual ~thwarp();
   

  /**
   * Morph given sketch.
   */
   
  virtual thpic * morph(thsketch * sketch, double scale);


  /**
   * Delete warp object.
   */
   
  virtual void self_delete();

   
};



/**
 * Only linear picture transformation.
 */

class thwarplin : public thwarp {

  protected:

  thpic mpic;
  bool morphed;
  int method;

  public:

  thwarplin() : morphed(false), method(0) {}
   
  virtual thpic * morph(thsketch * sketch, double scale);

  virtual void self_delete();
   
};



/**
 * Inverse distance transformation.
 */

class thwarpinvdist : public thwarplin {

  public:

  thwarpinvdist() {
    this->method = 1;
  }

  virtual void self_delete();
   
};



/**
 * Inverse distance line transformation.
 */

class thwarpinvdistln : public thwarplin {

  public:

  thwarpinvdistln() {
    this->method = 2;
  }

  virtual void self_delete();
   
};


/**
 * Inverse distance line transformation.
 */

class thwarpfastinvdistln : public thwarplin {

  public:

  thwarpfastinvdistln() {
    this->method = 3;
  }

  virtual void self_delete();
   
};



#endif


