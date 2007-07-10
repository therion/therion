/**
* @file thwarp.h
* Plaquette morphing.
*/

/* Copyright (C) 2007 Marco Corvi, Stacho Mudrak
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

#ifndef thwarpp_h
#define thwarpp_h
#define THERION

#include "thwarp.h"
#include "thwarppme.h"
#include "thwarppt.h"
#include "thdataobject.h"

/**
 * Sketch station structure.
 */

struct thsketchst {
  thmorph_type code;     //!< station code
  double x, y;           //!< station image coords
  thobjectname station;  //!< station survey name
  thobjectsrc source;
};

/**
 * Sketch leg structure.
 */

struct thsketchlg {
  thmorph_type code;    //!< leg code
  thobjectname from;    //!< station survey name
  thobjectname to;      //!< station survey name
  thobjectsrc source;
};



typedef std::list<thsketchst> thsketchst_list;
typedef std::list<thsketchlg> thsketchlg_list;


class thwarpp : public thwarp {

private:

  thpic mpic, upic;  ///< Morphed picture.
  thvec2 m_origin;  //!< Survey origin
  thsketchst_list stations;  //!< List of sketch stations.
  thsketchlg_list legs;      //!< List of legs  

private:

  /** get the scrap (pointer) for this sketch
   * @return the scrap pointer (NULL on failure)
   */
  thscrap * get_scrap();

  /** process the points and insert them in the warp transform
   * @param tw  the warp transform
   * @param ph  this picture height
   */
  void process_station_vector( thwarptrans & tw, double ph );

  /** process the legs and insert them in the warp transform
   * @param tw  the warp transform
   */
  void process_leg_vector( thwarptrans & tw );

public:

  virtual ~thwarpp();

  virtual thpic * morph(thsketch * sketch, double scale);

  virtual void self_delete();

};


#endif


