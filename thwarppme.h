/** * @file thwarppme.h
 *
 * @author marco corvi <marco.corvi@geocities.com>
 * @date   mar 2007
 *
 * Transformation structures for warping plaquette algorithm
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
#ifndef thwarppme_h
#define thwarppme_h
#define THERION

#include "thwarppdef.h"

#ifdef THERION
  #include "therion.h"  // thprintf
#else
  #define thprintf printf
#endif 

#include "thtrans.h"

#define THWARP_NGBH_DIM 3
#define THWARP_PAIR_DIM 4

// apparently it takes the same time ...
// #define MORPH_USE_IS_INSIDE
    
namespace therion
{
  namespace warp
  {
    struct line; // forward declaration
    struct point_pair; // forward declaration

    #ifdef MORPH_USE_IS_INSIDE
    /** check if a vector is inside other two
     * @param sbp  sign of the angle between the first vector and the given vector
     * @param scp  sign of the angle between the second vector and the given vector
     * @param less_then_pi whether the angle between the two vectors is less than PI
     * @return true if the given vector is inside the angle of the two vectors
     *
<PRE>
  dots (false) and commas (true) for the position of C 

      ,,,,,P                      P ..... <-- less_then_pi
      ,,,,,|                      | .....
 [1]  -----+------ B    [2]  -----+------- B
      ,,,,,|                      | ,,,,, 
      ,,,,,|                      | ,,,,, <-- not lss_than_pi
 
      .....|                      | ..... <-- less_than_pi
      .....|                      | .....
 [3]  -----+------ B    [4]  -----+------- B
      .....|                      | ,,,,, 
      .....P                      P ,,,,, <-- not less_than_pi

</PRE>
     */
    inline bool 
    is_inside( double sbp, double scp, bool less_then_pi = true )
    {
      if ( sbp > 0.0 ) {
        if ( scp <= 0.0 ) return true; // [1]
        return ! less_then_pi;         // [2]
      } else {
        if ( scp > 0.0 ) return false; // [3]
        return ! less_then_pi;         // [4]
      }
      return true;
    }
    #endif  // MORPH_USE_IS_INSIDE

    /** compute the angle between two vectors
     * @param v1 first vector
     * @param v2 second vector
     * @return vector between v2 and v1 [radians]
     */
    inline double angle( const thvec2 & v1, const thvec2 & v2 )
    {
      double c = v1 * v2;
      double s = v2 ^ v1;
      return atan2( s, c );
    }

    /** compute ...
     * @param a ...
     * @param x ...
     * @param u ...
     */
    double compute_vertical( double a, thvec2 & x, thvec2 & u );

    /** compute the squared distance from a segment
     * @param x   input vector
     * @param b   first endpoint of the segment
     * @param c   second endpoint of the segment
     */
    double compute_segment_distance2( const thvec2 & x, const thvec2 & b, const thvec2 & c );
   
    /** extra point and line inserter
     */
    class inserter
    {
      public:
        /** dstr 
	 */
        virtual ~inserter() {}
    
        /** add and extra line
         * @param p1    first line endpoint
         * @param index ...
         * @param x3    "from" image coords of the second endpoint
         * @param u3    "to" image coords of the second endpoint
         * @return pointer to the new line
         */
        virtual line * 
        add_extra_line( point_pair * p1, size_t index, thvec2 & x3, thvec2 & u3 ) = 0;
    
    };
    
    
    /** 
     * a pair of points in the sketch and in the survey
     * into correspondence
     */
    struct point_pair
    {
      morph_type m_type;  //!< point pair type
      bool m_used;        //!< whether this pair has been used in making the plaquettes
      std::string m_name; //!< station (or whatever) name
      thvec2 x;           //!< sketch coords
      thvec2 u;           //!< survey coords
      thvec2 z;           //!< sketch reduced coords
      thvec2 w;           //!< survey reduced coords
    
      private:
        std::vector< line * > mLines; //!< lines joining this point
        size_t m_legs;                //!< number of legs
    
      public:
      /** cstr
       * @param t    type of the point_pair
       * @param n    name of the point_pair (naming must follow a strict convention)
       * @param x1   X coord in the sketch image
       * @param y1   Y coord in the sketch image
       * @param u1   X coord in the survey 
       * @param v1   Y coord in the survey
       */
      point_pair( morph_type t, std::string & n, double x1, double y1 , double u1, double v1 )
        : m_type( t ) 
        , m_name( n )
        , x( x1, y1 )
        , u( u1, v1 )
        , m_legs( 0 )
      { }
      
      /** cstr
       * @param t    type of the point_pair
       * @param n    name of the point_pair (naming must follow a strict convention)
       * @param x1   X coord in the sketch image
       * @param y1   Y coord in the sketch image
       * @param u1   X coord in the survey 
       * @param v1   Y coord in the survey
       */
      point_pair( morph_type t, const char * n, double x1, double y1 , double u1, double v1 )
        : m_type( t )
        , m_name( n )
        , x( x1, y1 )
        , u( u1, v1 )
        , m_legs( 0 )
      { } 
    
      /** add a line (pointer) to the array of lines
       * @param line line (pointer)
       */
      void add_line( line * line );
    
      /** reorder (counter)clockwise the lines around the point
       * @param warper    new point_pair/line inserter
       * @param x_u        ratio X_unit / U_unit
       * @param proj       projection type
       */
      void order_lines( inserter * warper, double x_u, warp_proj proj );
    
      /** update: compute scaled coordinates
       * @param ...
       */
      void update( thvec2 & x0, double xunit, thvec2 & u0, double uunit );
    
      /** get the index of the node (the number of lines)
       * @return the number of lines at this node
       */
      size_t size() const { return mLines.size(); }
    
      /** get the number of centerline legs at the point
       * @return the number of centerline legs at this node
       */
      size_t legs() const { return m_legs; }
    
      /** get the "first" leg line
       * return the first leg line (NULL if none)
       */
      line * first_leg();
      
    
      /** get the next line around the point
       * @param line   given line (NULL to get the first line)
       * @return the line next to the given one (NULL if at the end of the lines)
       */
      line * next_line( line * line ) 
      {
        size_t sz = mLines.size();
        if ( line == NULL ) {
          return ( sz > 0 ) ? mLines[0] : NULL;
        }
        for (size_t j=0; j<sz; ++j) {
          if (mLines[j] == line ) return mLines[(j+1)%sz];
        }
        return NULL;
      }
    
      /** get the previous line around the point
       * @param line   given line
       * @return the line previous to the given one
       */
      line * prev_line( line * line ) 
      {
        size_t sz = mLines.size();
        for (size_t j=0; j<sz; ++j) {
          if (mLines[j] == line ) return mLines[(j+sz-1)%sz];
        }
        return NULL;
      }
    
    }; // class point_pair
    
    /** 
     * a line joining two points used in the warping
     */
    struct line
    {
      morph_type m_type;     //!< morph type of the line
      point_pair * m_p1;     //!< first point pair
      point_pair * m_p2;     //!< second point pair
      double dz;             //!< distance P1-P2 (Z coords)
      double dw;             //!< distance P1-P2 (W coords)
    
      thline2 z;             //!< line (first image - Z coords)
      thline2 w;             //!< line (second image - W coords)
      double zab;            //!< z-line sqrt(a*a + b*b)
      double wab;            //!< w-line sqrt(a*a + b*b)
      
      thvec2 vz; //!< unit vector in Z (P1->P2)
      thvec2 vw; //!< unit vector in W 
      
      double z_w; // dz / dw
      double w_z; // dw / dz
      
      // thmat2 R;                //!< rotation matrix R(from->to)
      // thmat2 S;                //!< rotation matrix R(to->from)
      
      /** cstr
       * @param t   morph type of the line
       * @param p1  first point pair
       * @param p2  second point pair
       *
       * @note as a side effect this line (pointer) is added to the two points
       */
      line( morph_type t, point_pair * p1, point_pair * p2 );
    
      /** update reduced parameters of the line
       */
      void update();
    
      /** get the other end of the line
       * @param end    one end of the line
       * @return the other end (NULL if end is not an end of this line)
       */
      point_pair * other_end( point_pair * end )
      {
        if ( end == m_p1 ) return m_p2;
        if ( end == m_p2 ) return m_p1;
        return NULL;
      }
    };
    
    // --------------------------------------------------------------
    
    /** base class for single-image warping items
     */
    struct item 
    {
      protected:
        double m_theta_l;  //!< left angle
	double m_theta_r;  //!< right angle

      public:
        /** default cstr is fine */
    
        /** dstr must be virtual
         */
        virtual ~item() 
	{ }
    

	/** get the value of the left angle
	 * @return the left angle [radians]
	 */
	double theta_left() const { return m_theta_l; }

	/** get the value of the right angle
	 * @return the right angle [radians]
	 */
	double theta_right() const { return m_theta_r; }
   
        /** check if a point is inside
         * @param p   2D point
         * @param bound   boundary size
         * @return true if the point is inside
         */
        virtual bool is_inside( const thvec2 & p, double bound ) const = 0;

        /** compute the distance of a point from the item
         * @param p 2D point
         * @return the distance
         */
        virtual double distance( const thvec2 & p ) const = 0;

        /** B-N distance from the item
	 * @param p 2D point
	 * @return the B-N distance
	 *
        virtual double bn_distance( const thvec2 & p ) const = 0;
	 */
   
        /**
	 */
        virtual double distance2( const thvec2 & p ) const = 0;
    
        /** bounding box for the item
	 * @param t1   minimum values (output)
	 * @param t2   maximum values (output)
	 * @param bound item boundary size
	 */
        virtual void bounding_box( thvec2 & t1, thvec2 & t2, double bound ) const = 0;

        virtual double s_map( const thvec2 & p ) const = 0;

        /** S-T map
	 * @param p input point
	 * @return transformed coordinates
	 */
        virtual void st_map( const thvec2 & p, thvec2 & ret ) const = 0;

	/** inverse S-T map
	 * @param coordinates in trasformed domain
	 * @return 2D point
	 */
        virtual void inv_st_map( const thvec2 & p, thvec2 & ret ) const = 0;

        virtual double sm_map( const thvec2 & p, thvec2 & ret ) const = 0;
        virtual void inv_sm_map( const thvec2 & p, thvec2 & ret, 
          double d4 ) const = 0;

        /** B-N map
	 * @param p input point
	 * @return transformed coordinates
	 */
        virtual void bn_map( const thvec2 & p, thvec2 & ret ) const = 0;

	/** inverse B-N map
	 * @param coordinates in trasformed domain
	 * @return 2D point
	 */
        virtual void inv_bn_map( const thvec2 & p, thvec2 & ret ) const = 0;

        /** rotate a point around the left corner
	 * @param p  input point
	 * @param ret rotated point
	 * @param c  cosine of the rotation angle
	 * @param s  sine of the rotation angle
	 */
	virtual void left_rotate( const thvec2 & p, thvec2 & ret, double c, double s ) const = 0;

	/** rotate a point around the right corner
	 * @param p  input point
	 * @param ret rotated point
	 * @param c  cosine of the rotation angle
	 * @param s  sine of the rotation angle
	 */
	virtual void right_rotate( const thvec2 & p, thvec2 & ret, double c, double s ) const = 0;

    
    }; // class item
    
    
    // --------------------------------------------------------------
    // warping feature elements
    
    /** 
     * abstract class of a warping feature basic_pair.
     * All warping basic_pair have one "missing" side and are
     * "extends" across that side by an amount specified be the
     * basic_pair's "bound"
     */
    class basic_pair
    {
      private:
        int m_nr;                   //!< the basic_pair nr
    
        static int basic_pair_nr;      //!< global counter for the basic_pairs
    
      protected: 
        double m_bound;             //!< this basic_pair bound
    
      public:
        basic_pair * m_ngbh[THWARP_NGBH_DIM];     //!< this basic_pair neighbors
        point_pair * m_pair[THWARP_PAIR_DIM];  //!< pointer to the point_pairs
        double m_ndist[THWARP_NGBH_DIM];       //!< neighbors distances
        double m_kl;                     //!< left angle differences (from - to)
        double m_kr;                     //!< right angle differences (from - to)
	double m_dl;                     //!< left end-size ratio (from/to)
	double m_dr;                     //!< right end-size ration (from/to)
    
      public:
        /** cstr
         * @param bound   boundary size
         */
        basic_pair( double bound ) 
          : m_nr( ++basic_pair_nr )
          , m_bound( bound )
	  , m_kl( 0.0 )
	  , m_kr( 0.0 )
	  , m_dl( 1.0 )
	  , m_dr( 1.0 )
        { 
          for (int k=0; k<THWARP_PAIR_DIM; ++k) 
            m_pair[k] = NULL;
          for (int k=0; k<THWARP_NGBH_DIM; ++k)
            m_ngbh[k] = NULL;
        }
    
        /** dstr
         */
        virtual ~basic_pair() {}


        /** get the left neighbor
         * @return pointer to the left neighbor
         */
        const basic_pair * left_ngbh()  const { return m_ngbh[0]; }

        /** get the right neighbor
         * @return pointer to the right neighbor
         */
        const basic_pair * right_ngbh()  const { return m_ngbh[ ngbh_nr() - 1 ]; }

	/** retrieve the "from" item
	 * @return pointer to the "from" item
	 */
	virtual const item * pfrom() const = 0;
	virtual item * pfrom() = 0;

	/** retrieve the "to" item
	 * @return pointer to the "to" item
	 */
	virtual const item * pto() const = 0;
	virtual item * pto() = 0;

        /** retrieve the id number of the warping basic_pair
         * @return this basic_pair' id number
         */
        int nr() const { return m_nr; }
    
        /** retrieve the warp type of the warping basic_pair
         * @return this basic_pair' type
         */
        virtual warp_type  type() const { return THWARP_NONE; }

        /** maximum number of neighbors
         * @return the maximum number of neighbors
         */
        virtual int ngbh_nr() const = 0;
    
        /** map p in the "from" image to a point in the "to" img
         * @param p   2D point in the "from" image 
         * @param ret corresponding 2D point in the "to" image
         */
        virtual void forward( const thvec2 & p, thvec2 & ret ) const = 0;
    
        /** map p in the "to" image to a point in the "from" img
         * @param p   2D point in the "to" image
         * @param ret corresponding 2D point in the "from" image
         */
        virtual void backward( const thvec2 & p, thvec2 & ret ) const = 0;
    
        /** check if a point in the "from" image is inside 
         * @param p   2D point in the "from" image
         * @return true if the point is inside 
         */
        virtual bool is_inside_from( const thvec2 & p ) const = 0;
    
        /** check if a point in the "to" image is inside 
         * @param p   2D point in the "to" image
         * @return true if the point is inside
         */
        virtual bool is_inside_to( const thvec2 & p ) const = 0;
    
        /** get the distance of a point from the basic_pair
         * @param p   2D point in the "to" image
         * @return the distance of the point P from the basic_pair
         */
        virtual double distance_to( const thvec2 & p ) const = 0;
        virtual double distance_from( const thvec2 & p ) const = 0;
    
        /** get the B-N distance of a point from the basic_pair
         * @param p   2D point in the "to" image
         * @return the B-N distance of the point P from the basic_pair
         *
        virtual double bn_distance_to( const thvec2 & p ) const = 0;
        virtual double bn_distance_from( const thvec2 & p ) const = 0;
	 */
    
        /** squared distance from the segment
         * @param p   point in the "to" image
         * @return squared distance from the segment
         */
        virtual double distance2_to( const thvec2 & p ) const = 0;
        // virtual double distance2_from( const thvec2 & p ) const = 0;

        /** get the basic_pair bounding-box
         * @param t1   upper left corner of the bounding-box in the "to" image
         * @param t2   lower right corner of the bounding-box in the "to" image
         */
        virtual void bounding_box_to( thvec2 & t1, thvec2 & t2 ) const = 0;
  
        /** rotate around the left corner (0.5.2)
	 * @oaram p   input 2D point
	 * @param ret return point
	 * @param x   rotation angle
	 */
        virtual void left_rotate_to( const thvec2 & p, thvec2 & ret, double x ) const = 0;

        /** rotate around the right corner (0.5.2)
	 * @oaram p   input 2D point
	 * @param ret return point
	 * @param x   rotation angle
	 */
        virtual void right_rotate_to( const thvec2 & p, thvec2 & ret, double x ) const = 0;


        #ifdef DEBUG
        /** debug: print
         */
        virtual void print() const = 0;
    
    
        /** debug: print the neighbors
         */
        void print_ngbhs() const
        {
          thprintf("    ngbh: ");
          for (int k=0; k<THWARP_NGBH_DIM; ++k) {
            if ( ! m_ngbh[k] ) thprintf(" 0 " );
            else thprintf(" %d ", m_ngbh[k]->nr() );
          }
          thprintf("\n");
        }
	#endif
    
    }; // class basic_pair

    // ----------------------------------------------------------------
    /** a pair of items, one in the "from" image, one in the "to" image
     */
    template< typename T >
    struct item_pair : public basic_pair
    {
      T from;    //!< item in the "from" image
      T to;      //!< item in the "to" image

      // double m_kl, m_kr;  // of basic_pair

      /** cstr from three point pairs
       * @param a  first point pair
       * @param b  second point pair
       * @param c  third point pair
       * @param bound  boundary size (default TRIANGLE boundary size)
       */
      item_pair( point_pair * a, point_pair * b, point_pair * c,
                 double bound = BOUND_TRIANGLE
               );

      /** cstr from four point pairs
       * @param a  first point pair
       * @param b  second point pair
       * @param c  third point pair
       * @param d  fourth point pair
       * @param bound  boundary size (default PLAQUETTE boundary size)
       */
      item_pair( point_pair * a, point_pair * b, point_pair * c, point_pair * d,
                 double bound = BOUND_PLAQUETTE
               );

      /** override basic_pair::pfrom()
       * @return pointer to the "from" item
       */
      const item * pfrom() const  
      { return dynamic_cast<const item *>(&from); }
      item * pfrom() 
      { return dynamic_cast<item *>(&from); }

      /** override basic_pair::pto()
       * @return pointer to the "to" item
       */
      const item * pto() const  
      { return dynamic_cast<const item *>(&to); }
      item * pto() 
      { return dynamic_cast<item *>(&to); }

      /** override basic_pair::type()
       * @return the warp type of this basic_pair
       */
      warp_type type() const;

      /** override basic_pair::ngbh_nr()
       * @return the number of neighbors of this basic_pair
       */
      int ngbh_nr() const;

      #ifdef DEBUG
      /** override basic_pair::print()
       */
      void print() const;
      #endif

      /** set the projection info
       * @param proj projection type
       */
      void set_projection( warp_proj proj );

      /** compute the forward map of a point
       * @param p  2D point in the "from" image
       * @return point coords in the "to" image
       *
       * @note in VERSION 0.5.1:
       * for triangle:
       *   from.bn_map()
       *   to.inv_bn_map()
       * now it is an iterative process
       * for plaquette: an iterative process (same as now)
       */
      void forward( const thvec2 & p, thvec2 & ret ) const;

      /** compute the backward map of a point
       * @param p  2D point in the "to" image
       * @param ret point coords in the "from" image
       *
       * @note in VERSION 0.5.1: 
       * for triangle:
       *    to.bn_map()
       *    from.inv_bn_map()
       *   the closest to this is backward_normal_no_bd
       * for plaquette:
       *   backward_normal_bd() with MORPH_BD == 0.5
       */
      void backward( const thvec2 & p, thvec2 & ret ) const
      {
	backward_normal_bd( p, ret );
      }

      /** backward() methods (0.5.2)
       */
      void backward_normal_bd( const thvec2 & p, thvec2 & ret ) const;

      /** check if a point is inside this basic_pair in the "from" image
       * @param p   2D point in the "from" image
       * @return true if the point is in this basic_pair
       */
      bool is_inside_from( const thvec2 & p ) const 
      { return from.is_inside( p, m_bound ); }

      /** check if a point is inside this basic_pair in the "to" image
       * @param p   2D point in the "to" image
       * @return true if the point is in this basic_pair
       */
      bool is_inside_to( const thvec2 & p ) const
      { return to.is_inside( p, m_bound ); }

      /** get the distance of a point in the "from" domain
       * @param p 2d point
       * @return distance of the point in the "from" domain
       */
      double distance_from( const thvec2 & p ) const
      { return from.distance( p ); }

      /** get the distance of a point in the "to" domain
       * @param p 2d point
       * @return distance of the point in the "to" domain
       */
      double distance_to( const thvec2 & p ) const
      { return to.distance( p ); }

      /**
      double bn_distance_from( const thvec2 & p ) const
      { return from.bn_distance( p ); }

      double bn_distance_to( const thvec2 & p ) const
      { return to.bn_distance( p ); }
       */

      /** sqaured distance in the "to" image
       * @param p input 2D point
       * @return squared distance
       */
      double distance2_to( const thvec2 & p ) const
      { return to.distance2( p ); }

      /** compute the bounding box in the "to" image
       * @param t1  upper left corner (output)
       * @param t2  lower right corner (output)
       */
      void bounding_box_to( thvec2 & t1, thvec2 & t2 ) const
      {
        to.bounding_box( t1, t2, m_bound );
      }

      /** left rotate in the "to" item (0.5.2)
       * @param p   input 2D vector 
       * @param ret return vector
       * @param x   rotation angle
       */
      void left_rotate_to( const thvec2 & p, thvec2 & ret, double x ) const
      {
	  thvec2 pa;
	  double t = m_kl * x;
	  to.left_rotate( p, pa, cos(t), -sin(t) );
	  to.bn_map( pa, ret );
      }

      /** right rotate in the "to" item (0.5.2)
       * @param p   input 2D vector 
       * @param ret return vector
       * @param x   rotation angle
       */
      void right_rotate_to( const thvec2 & p, thvec2 & ret, double x ) const
      {
	  thvec2 pa;
	  double t = m_kr * x;
	  to.right_rotate( p, pa, cos(t), sin(t) );
	  to.bn_map( pa, ret );
      }

    }; // class item_pair

    // ---------------------------------------------------------------------
    /** a triangle.
     *
     * The geometry is
     * <PRE>
     *     + - - - - - +
     *      \         /
     *       C - - - B
     *        \     /
     *         \   /
     *          \ /
     *           A
     * </PRE>
     * The bounds applies to the extension across the side CB.
     * C is the left vertex, B is the right vertex.
     * The order of the points is C-A-B
     *
     * A triangle is an "angle". The mapping is a linear map in polar
     * coordinates:
     * <PRE>
     *   theta --> theta' = (theta/theta_0) * theta'_0
     *   rho   --> rho'   = (rho/rho_0) * rho'_0
     * </PRE>
     * theta_0 and theta'_0 are the size of the two angles in the "from" and "to"
     * image, respectively. 
     * rho_0 and rho'_0 are the radial dimensions of the angles. They depend on
     * theta, as they are linearly interpolated between the two angle sides.
     */

    struct triangle : public item
    {
      thvec2 m_A, m_B, m_C;  //!< corners of the triangle (vertex, right, left)
      double m_ab;           //!< distance AB
      double m_ac;           //!< distance AC
      double m_abc;          //!< max between m_ab and m_ac
      double m_d;            //!< distances difference: AB - AC
      double m_t;            //!< theta = angle from AC to AB
      double m_ct;           //!< cosine of theta
      double m_st;           //!< sine of theta
      double m_dt;           //!< m_d/m_t;
      thvec2 m_AB;           //!< unit vector along AB
      thvec2 m_AC;           //!< unit vector along AC
      thvec2 m_AB1;          //!< unit vector orthogonal to AB
      thvec2 m_AC1;          //!< unit vector orthogonal to AC

      bool m_ltpi;           //!< "less than pi": whether theta is less than pi
    
      /** cstr
       * @note there is aproblem if the three points are aligned,
       * ie, A ^ (B-C) = 0, because the matrix M1 is not invertible.
       */
      triangle( thvec2 & a, thvec2 & b, thvec2 & c )
	: m_A( a )
        , m_B( b )
        , m_C( c )
        , m_AB( b.m_x - a.m_x, b.m_y - a.m_y )
        , m_AC( c.m_x - a.m_x, c.m_y - a.m_y )
      { 
        double xb = m_AB.m_x;
        double yb = m_AB.m_y;
        double xc = m_AC.m_x;
        double yc = m_AC.m_y;
        m_ab = sqrt(xb*xb + yb*yb);
        m_ac = sqrt(xc*xc + yc*yc);
	m_abc = (m_ab > m_ac)? m_ab : m_ac;
        m_d = m_ab - m_ac;
        m_st = yb * xc - xb * yc;
        m_ct = xb * xc + yb * yc;
        m_t = atan2( m_st, m_ct );
	m_dt = m_d / m_t;
        m_AB.normalize();
        m_AC.normalize();
        m_AB1.m_x =   m_AB.m_y;
        m_AB1.m_y = - m_AB.m_x;
        m_AC1.m_x =   m_AC.m_y;
        m_AC1.m_y = - m_AC.m_x;

        m_ltpi = m_st > 0.0;

	m_theta_l = m_theta_r = m_t;
	// m_theta_l = m_theta_r = angle( m_AB, m_AC );
    
        // thprintf("Triangle A %.2f %.2f B %.2f %.2f C %.2f %.2f\n",
        //    a.m_x, a.m_y, b.m_x, b.m_y, c.m_x, c.m_y );
        // thprintf("         AB %.2f AC %.2f Theta %.2f\n", m_ab, m_ac, m_t );
      }
    
      /** map a point to polar coordinates (AB is the X-axis)
       * @param p input point
       * @return polar coords (theta, rho) 
       *
       * rho is the normalized radial coordinate
       * theta is the normalized angular coordinate
       *
       * @note in VERSION 0.5.1: MORPH_USE_TRI_F
       */
      void to_polar( const thvec2 & p, thvec2 & ret ) const
      {
        double xp = p.m_x - m_A.m_x;
        double yp = p.m_y - m_A.m_y;
        double st = m_AC.m_x * yp - m_AC.m_y * xp;
        double ct = m_AC.m_x * xp + m_AC.m_y * yp;
        ret.m_x  = atan2(st,ct) / m_t;
	ret.m_y = sqrt(xp*xp + yp*yp);
      }
    
      /** compute a point from its polar coords
       * @param p   input point (polar coords)
       * @return cartesiona coord of the point
       *
       * @note in VERSION 0.5.1: MORPH_USE_TRI_F
       */
      void from_polar( const thvec2 & p, thvec2 & ret ) const
      {
        double r = p.m_y;                     // rho
        double t = m_t * p.m_x;                   // theta
        double c = r * cos(t);  // along AB
        double s = r * sin(t);  // across AB
	ret.m_x = m_A.m_x + m_AC.m_x * c - m_AC1.m_x * s;
	ret.m_y = m_A.m_y + m_AC.m_y * c - m_AC1.m_y * s;
      }
    
        /** check if a point is inside the triangle
         * @param p   2D point
         * @param bound  size of the item
         * @return true if the point is inside
	 *
	 * @note in VERSION 0.5.1: not MORPH_USE_IS_INSIDE and MORPH_USE_TRI_F
         */
        bool is_inside( const thvec2 & p, double bound ) const 
        { 
          #ifdef MORPH_USE_IS_INSIDE
            thvec2 ap( p.m_x - m_A.m_x, p.m_y - m_A.m_y );
            double sbp = - m_AB.m_x * ap.m_y + m_AB.m_y * ap.m_x;
            double scp = - m_AC.m_x * ap.m_y + m_AC.m_y * ap.m_x;
            if ( ! therion::warp::is_inside( sbp, scp, m_ltpi ) ) return false;
	    double t = ap.m_x*ap.m_x + ap.m_y*ap.m_y;
          #else
	    // the following is adapted from to_polar() code
	    //
            // thvec2 p1;
	    // to_polar( p, p1 );
            // return p1.m_x >= 0.0 && p1.m_x <= 1 && p1.m_y < bound;
	    //
            double xp = p.m_x - m_A.m_x;
            double yp = p.m_y - m_A.m_y;
            double st = m_AC.m_x * yp - m_AC.m_y * xp;
            double ct = m_AC.m_x * xp + m_AC.m_y * yp;
            // double s  = atan2(st,ct);
	    // if ( s > m_t ) return false;
	    if ( st < 0 ) return false;
	    if ( m_st*ct < m_ct*st ) return false;
	    double t = xp*xp + yp*yp;
          #endif
   
          double f = m_abc*bound; 
	  return ( t >= 0.0 && t < f*f );
        }

        /** distance of a 2D point from the triangle
         * @param p 2D point
         * @return the distance
         */
        double distance( const thvec2 & p ) const
        {
          thvec2 p1 = p - m_A;
          return p1.length();
        }

        /** line distance of apoint from the triangle
         * @param p 2D point
         * @return the line distance
         *
        double bn_distance( const thvec2 & p ) const
        {
	  double x = p.m_x - m_A.m_x;
	  double y = p.m_y - m_A.m_y;
	  return x*x + y*y;
	}
	 */
   
        /** square distance of a point
	 * @param p   input 2D point
	 * @return square distance of the point from the vertex
	 *
	 * @note in VERSION 0.5.1: MORPH_USE_TRI_F
	 */
        double distance2( const thvec2 & p ) const
        {
          thvec2 p1;
	  to_polar( p, p1 );
          if ( p.m_x < 0.0 || p.m_x > 1.0 ) return 1.e+12; // INFINITY;
          double d = p.m_y;
          return d*d;
        }

        /** compute the bounding box of the triangle
	 * @param t1 lower-left corner of the bounding box
	 * @param t2 upper-right corner of the bounding box
	 * @param bound  "vertical" size of the bounding box
	 */
        void bounding_box( thvec2 & t1, thvec2 & t2, double bound ) const
	{
          #if 1
            thvec2 b = m_B - bound * (m_A - m_B);
            thvec2 c = m_C - bound * (m_A - m_C);
            t1 = m_A;
            t1.minimize( b );
            t1.minimize( c );
            t2 = m_A;
            t2.maximize( b );
            t2.maximize( c );
          #else
            // TODO
          #endif
	}

        /** (0.5.2)
	 * @param p   input 2D point
	 * @return S value in st_map
	 */
	double s_map( const thvec2 & p ) const
        {
          double xp = p.m_x - m_A.m_x;
          double yp = p.m_y - m_A.m_y;
          double st = m_AC.m_x * yp - m_AC.m_y * xp;
          double ct = m_AC.m_x * xp + m_AC.m_y * yp;
          return atan2(st,ct) / m_t;
        }

        /** s-t map for the triangle
	 * @param p input 2D point
	 * @param ret return s-t coordinates (polar coordinates)
	 */
        void st_map( const thvec2 & p, thvec2 & ret ) const
	{ to_polar(p, ret); }

        /** inverse s-t map for the triangle
	 * @param p input s-t (= polar) coordinates
	 * @param ret return 2D point
	 */
        void inv_st_map( const thvec2 & p, thvec2 & ret ) const
	{ from_polar(p, ret); }

        /** (0.5.2)
	 */
        double sm_map( const thvec2 & p, thvec2 & ret ) const;

        /** (0.5.2)
	 */
        void inv_sm_map( const thvec2 & p, thvec2 & ret, double d4 ) const;

        /** Beier-Neely map for the triangle
	 * @param p input 2D point
	 * @param ret return B-N coordinates (polar coordinates)
	 */
        void bn_map( const thvec2 & p, thvec2 & ret ) const
	{ to_polar(p, ret); }

        /** inverse Beier-Neely map for the triangle
	 * @param p input B-N (= polar) coordinates
	 * @param ret return 2D point
	 */
        void inv_bn_map( const thvec2 & p, thvec2 & ret ) const
	{ from_polar(p, ret); }

        /** rotate a point about the "left" corner 
	 * @param p   input point
	 * @param ret rotated point (actually p itself)
	 * @param c   cosine of the rotation angle
	 * @param s   sine of the rotation angle
	 *
	 * @note in VERSION 0.5.1: ret was return instead of param
	 */
	void left_rotate( const thvec2 & p, thvec2 & ret, double /* c */, double /* s */) const
	{
	  ret = p;
	}

        /** rotate a point about the "right" corner 
	 * @param p   input point
	 * @param ret rotated point (actually p itself)
	 * @param c   cosine of the rotation angle
	 * @param s   sine of the rotation angle
	 *
	 * @note in VERSION 0.5.1: ret was return instead of param
	 */
	void right_rotate( const thvec2 & p, thvec2 & ret, double /* c */, double /* s */) const
	{
	  ret = p;
	}

    }; // class triangle

    // ----------------------------------------------------------------------
    /** four corners plaquette (quadrilater)
     * 
     * The geometry is
     * <PRE>  
     *        + - - - - - - +
     *        |             |
     *     <- D - - - - - - C ->
     *        |             |
     *        A ----------- B
     * <\PRE>
     * The bound applies to the extension across the side CD
     * The order of the pairs is D-A-B-C
     *
     * @see P.S. Heckbert "Fundamentals of Texture Mapping and Image Warping"
     *      UCA Berkeley, 1989
     *
     * @note projective transform is not fine because it is not linear on the
     *       sides and this introduces 0-order discontinuities on the plaquette
     *       borders.
     */
    
    struct plaquette : public item
    {
      public:
        thvec2 m_A, m_B, m_C, m_D;  //!< vertices of the plaquette
    
        thvec2 m_AD;                //!< vector AD: A - D
        thvec2 m_BC;                //!< vector BC: B - C
	thvec2 m_adn;               //!< unit vector AD
	thvec2 m_bcn;               //!< unit vector BC
	thvec2 m_abn;               //!< unit vector from A to B
        thvec2 m_abh;               //!< unit orthogonal vector
        double m_AB_len;            //!< length of AB

        double m_a, m_b, m_c, m_d, m_e, m_f, m_g, m_h; 
        double m_A0, m_B0, m_C0;
        double m_D0, m_E0, m_F0;

        double m_adab;   //!< ad ^ ab
        double m_bcab;   //!< bc ^ ab
        double m_adA;    //!< ad ^ A
        double m_bcB;    //!< bc ^ B
        thvec2 m_AB;     //!< vector AB = B - A
	thvec2 m_BA;     //!< vector BA (= -m_AB)
        thvec2 m_C1;
        thvec2 m_C1C;

        double m_AD_len;  //!< length of AD
        double m_BC_len;  //!< length of BC

        double m_tan, m_ctg, m_cos, m_sin; // coeff for the "normal" direction

	double (plaquette::* s_map_impl)( const thvec2 & p ) const;
        void (plaquette::* bn_map_impl)( const thvec2 & p, thvec2 & ret ) const;
        void (plaquette::* inv_bn_map_impl)( const thvec2 & p, thvec2 & ret ) const;
   
      public:
        /** cstr
	 * @param a   A vertex
	 * @param b   B vertex
	 * @param c   C vertex
	 * @param d   D vertex
	 */
        plaquette( thvec2 & a, thvec2 & b, thvec2 & c, thvec2 & d )
          : m_A( a )
          , m_B( b )
          , m_C( c )
          , m_D( d )
        { 
	  s_map_impl = &therion::warp::plaquette::s_map_straight;
	  bn_map_impl = &therion::warp::plaquette::bn_map_straight;
	  inv_bn_map_impl = &therion::warp::plaquette::inv_bn_map_straight;

          init();
        }

	/** compute the S coordinate of the ST map
	 * @param p input 2d point
	 * @return s coordinate value
	 *
         * @pre the point should lie inside the plaquette
	 * @note in VERSION 0.5.1: s_map == s_map_straight
	 */
	double s_map( const thvec2 & p ) const
	{
	  return (this ->* s_map_impl)(p);
	}

        /** s-maps() (0.5.2)
	 */
	double s_map_straight( const thvec2 & p ) const;
	double s_map_slant( const thvec2 & p ) const;


	/** compute the T coordinate of the ST map
	 * @param p input 2d point
	 * @return t coordinate value
	 *
         * @pre the point should lie inside the plaquette
	 * @note in VERSION 0.5.1: unchanged
	 */
	double t_map( const thvec2 & p ) const;
    
        /** map a point to plaquette (s,t) coordinates
         * @param p 2D point
         * @return plaquette (s,t) coordinates of the point
         *
         * @pre the point should lie inside the plaquette
	 * @note in VERSION 0.5.1: unchanged
         */
        void st_map( const thvec2 & p, thvec2 & ret ) const;

        /** inverse map plaquette (s,t) coordinates to point
         * @param p plaquette (s,t) coordinates of the point
         * @return 2D point
	 *
	 * @note in VERSION 0.5.1: unchanged
         */
        void inv_st_map( const thvec2 & p, thvec2 & ret ) const;

        /** sm-maps (0.5.2)
         * @return fourth power of the distance
         */
        double sm_map( const thvec2 & p, thvec2 & ret ) const;
        void inv_sm_map( const thvec2 & p, thvec2 & ret, double d4 ) const;

        /** h-v map
         * @param  p 2D point
         * @param ret 2D return point (x: coord. along AB, y: vertical)
         */
        void hv_map( const thvec2 & p, thvec2 & ret ) const;
        void inv_hv_map( const thvec2 & p, thvec2 & ret ) const;

        void vh_map( const thvec2 & p, thvec2 & ret ) const;
        void inv_vh_map( const thvec2 & p, thvec2 & ret ) const;

        void hvm_map( const thvec2 & p, thvec2 & ret ) const;
        void inv_hvm_map( const thvec2 & p, thvec2 & ret ) const;

        void vhm_map( const thvec2 & p, thvec2 & ret ) const;
        void inv_vhm_map( const thvec2 & p, thvec2 & ret ) const;


        /** Beier-Neely map
	 * @param p input point
	 * @return B-N transformed point
	 *
	 * map a point P into 
	 * - x = normalized projection along AB (A=0, B=1)
	 * - y = un-normalized distance from the line AB
	 *
	 * @note in VERSION 0.5.1: bn_map == bn_map_straight
	 */
        void bn_map( const thvec2 & p, thvec2 & ret ) const
	{
	  (this->*bn_map_impl)( p, ret );
	}

        /** inverse Beier-Neely map
	 * @param p input point (B-N coordinates)
	 * @return 2D point
	 *
	 * @note in VERSION 0.5.1: inv_bn_map == inv_bn_map_straight
	 */
        void inv_bn_map( const thvec2 & p, thvec2 & ret ) const
	{
	  (this->*inv_bn_map_impl)( p, ret );
	}

        /** bn-maps() (0.5.2)
	 */
        void bn_map_straight( const thvec2 & p, thvec2 & ret ) const;
        void bn_map_slant( const thvec2 & p, thvec2 & ret ) const;

        void inv_bn_map_straight( const thvec2 & p, thvec2 & ret ) const;
        void inv_bn_map_slant( const thvec2 & p, thvec2 & ret ) const;

        /** rotate a point around the "left" (A) corner
	 * @param p input point
	 * @param ret rotated point
	 * @param c cosine of the rotation angle
	 * @param s sine of the rotation angle
	 */
	void left_rotate( const thvec2 & p, thvec2 & ret, double c, double s ) const
	{
	  double x = p.m_x - m_A.m_x;
	  double y = p.m_y - m_A.m_y;
	  ret.m_x = m_A.m_x + x * c - y * s;
	  ret.m_y = m_A.m_y + x * s + y * c;
	}

        /** rotate a point around the "right" (B) corner
	 * @param p input point
	 * @param ret rotated point
	 * @param c cosine of the rotation angle
	 * @param s sine of the rotation angle
	 */
	void right_rotate( const thvec2 & p, thvec2 & ret, double c, double s ) const
	{
	  double x = p.m_x - m_B.m_x;
	  double y = p.m_y - m_B.m_y;
	  ret.m_x = m_B.m_x + x * c - y * s;
	  ret.m_y = m_B.m_y + x * s + y * c;
	}
    
        /** check if a point is inside the plaquette
         * @param p   2D point
         * @param bound  size of the item
         * @return true if the point is inside
	 *
	 * @note in VERSION 0.5.1: st_map (ie not MORPH_USE_IS_INSIDE)
         */
        bool is_inside( const thvec2 & p, double bound ) const 
        {
          // thvec2 v;
	  // st_map( p, v );
          // if ( v.is_nan() ) return false;
          // return v.m_x >= 0.0 && v.m_x <= 1.0 && v.m_y >= 0.0 && v.m_y < bound;

#ifdef MORPH_USE_IS_INSIDE
          thvec2 ap( p.m_x - m_A.m_x, p.m_y - m_A.m_y );
          double sbp = - m_AB.m_x * ap.m_y + m_AB.m_y * ap.m_x;
          double scp = - m_AD.m_x * ap.m_y + m_AD.m_y * ap.m_x;
	  if ( ! therion::warp::is_inside( sbp, scp ) ) return false;
	  scp = - sbp;
	  thvec2 bp( p.m_x - m_B.m_x, p.m_y - m_B.m_y );
	  sbp = - m_BC.m_x * bp.m_y + m_BC.m_y * bp.m_x;
	  if ( ! therion::warp::is_inside( sbp, scp ) ) return false;
	  double c = ap * m_abn;
	  // ap -= c * m_abn;
	  ap.m_x -= c * m_abn.m_x;
	  ap.m_y -= c * m_abn.m_y;
	  bound *= m_AB_len;
	  double t = ap.m_x*ap.m_x + ap.m_y*ap.m_y - bound*bound;
          return ( t < 0.0 );
#else
          // the following is the st_map code
          double s;
          double ex_ay = m_e * p.m_x - m_a * p.m_y;
          double A = m_A0;
          double B = (ex_ay + m_B0)/2.0;
          double C = m_g * p.m_x - m_c * p.m_y + m_C0;
          if ( fabs(A) < 1.e-6 ) {
	    if ( B == 0.0 ) return false;
            s = - C / (2*B);
            if ( s < 0 || s > 1 ) return false;
	  } else {
	    if ( A < 0.0 ) { A=-A; B=-B; C=-C; }
	    // now A > 0
	    if ( C > 0 ) { // det < B
	      if ( B > 0 ) return false;     // neg. solutions
	      if ( B*B < A*C ) return false; // neg. det
	      // s = (-B-det)/A > 1
	      //     |B|-det > A
	      //     |B|-A > det
	      //     B*B - 2 |B| A + A*A > det^2 = B*B - A C
	      //     A * ( A + 2 B + C ) > 0
	      if ( A+2*B+C > 0 ) return false;
              s = (- B - sqrt(B*B-A*C) )/A;
	    } else { // det > B
	      // s = (det - B)/A > 0
	      if ( A + 2*B + C < 0 ) return false;
              s = (sqrt(B*B-A*C) - B)/A;
	    }
          }
          double t = ( p.m_x - m_b * s - m_d ) / ( m_a * s + m_c );
	  return ( t >= 0.0 && t < bound );
#endif
        }

        /** distance of a point from the plaquette
         * @param p 2D point
         * @return the distance
	 *
	 * @pre p is inside the plaquette
	 * this could be any reasonable distance function
         */
        double distance( const thvec2 & p ) const
        {
          return t_map( p );
        }

	/** Beier-Neely distance
	 * @param p 2D point
	 * @return the B-N distance
	 *
	double bn_distance( const thvec2 & p ) const
	{
	  thvec2 ap = p - m_A;
	  double d = ap * m_abn;
	  if ( d >= 0 ) {
	    if ( d < m_AB_len ) 
	      return ( ap.length2() - d*d );
	    double x = p.m_x - m_B.m_x;
	    double y = p.m_y - m_B.m_y;
	    return x*x + y*y;
	  }
	  double x = p.m_x - m_A.m_x;
	  double y = p.m_y - m_A.m_y;
	  return x*x + y*y;
	}
	 */
    
        double distance2( const thvec2 & p ) const
        {
          thvec2 v;
	  st_map( p, v );
          if ( v.m_y < 0.0 || v.m_x < 0.0 || v.m_x > 1.0 ) return 1.e+12; // INFINITY;
          if ( v.m_y < 1.0 ) return 0.0;
          return compute_segment_distance2( p, m_D, m_C );
        }
   
        /** compute the bounding box of the plaquette
	 * @param t1 lower-left corner of the bounding box
	 * @param t2 upper-right corner of the bounding box
	 * @param bound  "vertical" size of the bounding box
	 */
        void bounding_box( thvec2 & t1, thvec2 & t2, double bound ) const
	{
          thvec2 c = m_B + bound * m_BC;
          thvec2 d = m_A + bound * m_AD;
          t1 = m_A;
          t1.minimize( m_B );
          t1.minimize( c );
          t1.minimize( d );
          t2 = m_A;
          t2.maximize( m_B );
          t2.maximize( c );
          t2.maximize( d );
	}

      private:
        /** initialize the plaquette
         */
        void init();

    }; // class plaquette

// ****************************************************************
// PAIRS
//
    typedef item_pair< triangle > triangle_pair;
    typedef item_pair< plaquette > plaquette_pair;

    
  } // namespace warp

} // namespace therion

// *****************************************************************
// backward compatibility
//
#ifdef THERION
  typedef therion::warp::point_pair thmorph_pair;
  typedef therion::warp::line thmorph_line;
#endif


#endif
