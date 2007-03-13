/** @file thwarppdef.h
 * 
 * @author marco corvi
 * @date   mar 2007
 * 
 * @brief warping transformation definitions
 */
/* Copyright (C) 2007 marco corvi
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
#ifndef thwarppdef_h
#define thwarppdef_h

// #define BOUND_SEGMENT 1.5
#define BOUND_TRIANGLE 1.5
#define BOUND_PLAQUETTE 1.5

#define THERION


namespace therion
{
  namespace warp
  {
    
    /** type of the warping control objects
     * overused for point-pairs and lines
     */
    enum morph_type 
    {
      THMORPH_STATION,   /* station point | centerline shot */
      THMORPH_LEFT,      /* left point    | left shot */
      THMORPH_RIGHT,     /* right point   | right shot */
      THMORPH_SEGMENT,   /* -             | straight end segment */
      THMORPH_EXTRA,
    };
    
    /** warping projection type
     */
    enum warp_proj 
    {
      THWARP_PLAN,       /* plan */
      THWARP_EXTENDED,   /* extended elevation */
    };
    
    /** warping pair types
     */
    enum warp_type
    {
      THWARP_NONE = 0,
      THWARP_POINT = 1,
      THWARP_SEGMENT = 2,
      THWARP_TRIANGLE = 3,
      THWARP_PLAQUETTE = 4
    };
  } // namespace warp
} // namespace therion

// *****************************************************************
// backward compatibility
//
#ifdef THERION
  typedef therion::warp::morph_type thmorph_type;
  typedef therion::warp::warp_proj  thwarp_proj;

  using therion::warp::THMORPH_STATION;

#endif

#endif // thwarppdef_h

