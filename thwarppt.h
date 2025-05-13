/** @file thwarppt.h
 * 
 * @author marco corvi
 * @date   nov 2006 - mar 2007
 * 
 * @brief warping plaquette algorithm
 *
 * ---------------------------------------------------
 */
/* Copyright (C) 2006-2007 marco corvi
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

#ifndef thwarppt_h
#define thwarppt_h
#define THERION

#include "thtrans.h"
#include "thwarppme.h"
#include "thwarppdef.h"

#include <memory>
#include <string>
#include <vector>

namespace therion
{
  namespace warp
  {

  /**
   * therion warping transformation.
   *
   * The "from" data are in the sketch image reference frame
   * The "to" data are in the survey reference frame
   * The "warped image" is scaled/translated with respect to the survey frame
   *
   * The X and U centers and units are computed when the mapping is
   * initialized.  The warped image center and units are computed when a
   * source image is mapped (before that point they have the same values as the
   * U center and units).  Until the map has been initialized the centers and
   * units have undefined values.
   */
    class plaquette_algo : public inserter
    {
      private:
        bool m_initialized;          //!< whether the map has been initialized
        std::vector<std::unique_ptr<therion::warp::basic_pair>> mPlaquettes; //!< vector of 4-plaquettes
        std::vector<std::unique_ptr<therion::warp::point_pair>> mPairs;         //!< vector of point pairs
        std::vector<std::unique_ptr<therion::warp::line>> mLines;         //!< vector of legs (lines)
       
        double m_bound3;   //!< bound for triangles
        double m_bound4;   //!< bound for plaquettes
    
      public:
        thvec2 mX0;    //!< X-Y center
        thvec2 mU0;    //!< U-V center
        double mXUnit = 0.0; //!< X-Y unit
        double mUUnit = 0.0; //!< U-V unit
        thvec2 mUC;    //!< U-V center in the warped image
        double mUCUnit = 0.0; //!< U-V unit in the warped image
        
      public:
        /** default cstr
         */
        plaquette_algo( );
    
    
        /** cstr
         * @param bound3   bound for triangles
         * @param bound4   bound for plaquettes
         */
        plaquette_algo( double bound3, double bound4 );
    
        /** accessor to the "from" origin 
         * @return origin in the "from" image
         */
        thvec2 from_origin() const { return mX0; }
        
        /** accessor to the "to" origin 
         * @return origin in the "to" image
         */
        thvec2 to_origin() const { return mU0; }
        
        /** accessor to the "from" unit 
         * @return unit in the "from" image
         */
        double from_unit() const { return mXUnit; }
        
        /** accessor to the "to" unit 
         * @return unit in the "to" image
         */
        double to_unit() const { return mUUnit; }
        
        /** accessor to the warped image origin 
         * @return origin in the warped image (zero at the upper-left corner)
         */
        thvec2 warped_origin() const { return mUC; }
        
        /** accessor to the warped image unit 
         * @return unit in the warped image
         */
        double warped_unit() const { return mUCUnit; }
    
    
        
        /** insert a point and a line of extra type
         * @param p1  first line point
         * @param index index used to make the new point name
         * @param x3    new point X (sketch frame)
         * @param u3    new point U (survey frame)
         */
        line * add_extra_line( point_pair * p1, size_t index, 
                                       thvec2 & x3, thvec2 & u3 ) override;
    
    
        /** insert a point
         * @param t    point morph type
         * @param name point name
         * @param src  point coords in the source (from) frame (sketch)
         * @param dst  point coords in the destination (to) frame (survey)
         * @return pointer to the new point-pair
         */
        point_pair * insert_point( morph_type t, std::string & name, 
                                     const thvec2 & src, const thvec2 & dst );

	point_pair * insert_zoom_point( morph_type t, std::string & name, 
	                                const thvec2 src, std::string & from, double size );
    
        /** insert a line
         * @param t    line morph type
         * @param from name of the first line endpoint 
         * @param to   name of the second line endpoint 
         */
        void insert_line( morph_type t, std::string & from, std::string & to );
        
        /** initialize internal data for the mapping
         * @param proj  projection type
         * @return true if initialized successfully
         */
        bool initialize( warp_proj proj = THWARP_PLAN );
        
        /** map a 2D point forward 
         * @param src     point in the source ("from") frame
         * @param dst     point in the "to" frame
         * @return true if successful
         */
        bool forward( const thvec2 & src, thvec2 & dst )
        {
            return forward( src, dst, mU0, mUUnit );
        }
        
        /** map a 2D point backward 
         * @param dst     point in the "to" frame
         * @param src     point in the "from" frame
         * @return true if successful
         */
        bool backward( const thvec2 & dst, thvec2 & src )
        {
            return backward( dst, src, mU0, mUUnit );
        }
        
        /** map a 2D point backward 
         * @param dst     point in the "to" frame (survey)
         * @param src     point in the "from" frame (sketch)
         * @param origin  origin in the "to" frame
         * @param unit    unit in the "to" frame
         * @return true if successful
         */
        bool backward( const thvec2 & dst, thvec2 & src, const thvec2 & origin, double unit );
        
        /** map a 2D point forward
         * @param src     point in the "from" frame
         * @param dst     point in the "to" frame
         * @param origin  origin in the "to" frame
         * @param unit    unit in the "to" frame
         * @return true if successful
         */
        bool forward( const thvec2 & src, thvec2 & dst, const thvec2 & origin, double unit );
        
        /** map an image. The result is centered in the destination image
         * @param src  source image (RGB)
         * @param ws   source image width
         * @param hs   source image height 
         * @param dst  destination image (RGB) must be preallocated by the caller
         * @param wd   destination image width
         * @param hd   destination image height
         * @param origin target origin in the warped image frame
         * @param unit   target units
         * @param depth image depth (bytes per pixel).
         * @param proj  sketch projection type
         * @return true if successful
         *
         * @note this method, as a side effect, sets the warped image origin and units
         */
        bool map_image( const unsigned char * src, unsigned int ws, unsigned int hs,
                    unsigned char * dst, unsigned int wd, unsigned int hd,
    		    thvec2 const & origin, double unit,
                    int depth = 3, 
    		    warp_proj proj = THWARP_PLAN );
    
        /** debug: print the content of this warptrans
         */
        void print();
    
      private:
        /** update line features - compute the lines' coeffs.
         */
        void update_lines();
        
        /** prepare the plaquettes
         * @param proj  projection type
         */
        void make_plaquettes( warp_proj proj );
        
        /** set up the neighbors
         */
        void make_neighbors( );
    
        /** find the neighbors of a warping pair
         * @param me warping basic_pair
         */
        void find_neighbors( basic_pair * me );
    
        /** fidn the warping pair with a given oriented side
         * @param p1 first side point
         * @param p2 second side point
         */
        basic_pair * find_plaquette( point_pair * p1, point_pair * p2 );
        
        /** map by the plaquette
         * @param k   plaquette index
         * @param w   from point
         * @param z   to point
         * @return true if successful
         */
        bool map_backward_plaquette( size_t k, const thvec2 & w, thvec2 & z );
    
        #ifdef USE_SEGMENT
        /** add a segment
         * @param A   first endpoint
         * @param B   second endpoint
         */
        void add_segment( point_pair * A, point_pair * B );
        #endif
    
        /** add a triangle
         * @param A   first point (left)
         * @param B   second point (vertex)
         * @param C   third point (right)
         * @param proj warp projection type
         */
        void add_triangle( point_pair * A, point_pair * B, point_pair * C,
          warp_proj proj = THWARP_PLAN );
        
        /** add a quadrilateral
         * @param A   first point 
         * @param B   second point
         * @param C   third point 
         * @param D   fourth point 
         * @param proj warp projection type
         */
        void add_quadrilateral( point_pair * A, point_pair * B, point_pair * C, point_pair * D,
          warp_proj proj = THWARP_PLAN );
    };

  } // namespace warp

} // name space therion

// *******************************************************************
// backward compatibility
//
#ifdef THERION
  typedef therion::warp::plaquette_algo thwarptrans;
#endif

#endif
