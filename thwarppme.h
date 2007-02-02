/**
 * @file thmorphelement.h
 * Transformation structures.
 */
  
/* Copyright (C) 2006 marco corvi
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
#ifndef thmorph_element_h
#define thmorph_element_h

#include "therion.h"  // thprintf
// #define thprintf printf

#include "thtrans.h"


/** type of the morphing control element
 * overused for point-pairs and lines
 */
enum thmorph_type 
{
  THMORPH_STATION,   /* station point | centerline shot */
  THMORPH_LEFT,      /* left point    | left shot */
  THMORPH_RIGHT,     /* right point   | right shot */
  THMORPH_SEGMENT,   /* -             | straight end segment */
  THMORPH_EXTRA,
};

/** warping projection type
 */
enum thwarp_proj 
{
  THWARP_PLAN,       /* plan */
  THWARP_EXTENDED,   /* extended elevation */
};

struct thmorph_line; // forward declaration
struct thmorph_pair; // forward declaration

/** extra point and line inserter
 */
class thinserter
{
  public:
    virtual ~thinserter() {}

    /** add and extra line
     * @param p1    first line endpoint
     * @param index ...
     * @param x3    "from" image coords of the second endpoint
     * @param u3    "to" image coords of the second endpoint
     * @return pointer to the new line
     */
    virtual thmorph_line * 
    add_extra_line( thmorph_pair * p1, unsigned int index, thvec2 & x3, thvec2 & u3 ) = 0;

};


/** 
 * a pair of points in the sketch and in the survey
 * into correspondence
 */
struct thmorph_pair
{
  thmorph_type m_type;  //!< point pair type
  bool m_used;          //!< whether this pair has been used in making the plaquettes
  std::string m_name; //!< station (or whatever) name
  thvec2 x;           //!< sketch coords
  thvec2 u;           //!< survey coords
  thvec2 z;         //!< sketch reduced coords
  thvec2 w;         //!< survey reduced coords

  private:
    std::vector< thmorph_line * > mLines; //!< lines joining this point
    unsigned int m_legs;                  //!< number of legs

  public:
  /** cstr
   * @param t    type of the pair
   * @param n    name of the pair (naming must follow a strict convention)
   * @param x1   X coord in the sketch image
   * @param y1   Y coord in the sketch image
   * @param u1   X coord in the survey 
   * @param v1   Y coord in the survey
   */
  thmorph_pair( thmorph_type t, std::string & n, double x1, double y1 , double u1, double v1 )
    : m_type( t ) 
    , m_name( n )
    , x( x1, y1 )
    , u( u1, v1 )
    , m_legs( 0 )
  { }
  
  thmorph_pair( thmorph_type t, const char * n, double x1, double y1 , double u1, double v1 )
    : m_type( t )
    , m_name( n )
    , x( x1, y1 )
    , u( u1, v1 )
    , m_legs( 0 )
  { } 

  /** add a line (pointer) to the array of lines
   * @param line line (pointer)
   */
  void add_line( thmorph_line * line );

  /** reorder (counter)clockwise the lines around the point
   * @param morpher    new pair/line inserter
   * @param x_u        ratio X_unit / U_unit
   * @param proj       projection type
   */
  void order_lines( thinserter * morpher, double x_u, thwarp_proj proj );

  /** update: compute scaled coordinates
   * @param ...
   */
  void update( thvec2 & x0, double xunit, thvec2 & u0, double uunit );

  /** get the index of the node (the number of lines)
   * @return the number of lines at this node
   */
  unsigned int size() const { return (unsigned int) mLines.size(); }

  /** get the number of centerline legs at the point
   * @return the number of centerline legs at this node
   */
  unsigned int legs() const { return m_legs; }

  /** get the "first" leg line
   * return the first leg line (NULL if none)
   */
  thmorph_line * first_leg();
  

  /** get the next line around the point
   * @param line   given line (NULL to get the first line)
   * @return the line next to the given one (NULL if at the end of the lines)
   */
  thmorph_line * next_line( thmorph_line * line ) 
  {
    unsigned int sz = (unsigned int) mLines.size();
    if ( line == NULL ) {
      return ( sz > 0 ) ? mLines[0] : NULL;
    }
    for (unsigned int j=0; j<sz; ++j) {
      if (mLines[j] == line ) return mLines[(j+1)%sz];
    }
    return NULL;
  }

  /** get the previous line around the point
   * @param line   given line
   * @return the line previous to the given one
   */
  thmorph_line * prev_line( thmorph_line * line ) 
  {
    unsigned int sz = (unsigned int)mLines.size();
    for (unsigned int j=0; j<sz; ++j) {
      if (mLines[j] == line ) return mLines[(j+sz-1)%sz];
    }
    return NULL;
  }

};

/** 
 * a line joining two points used in the morphing
 */
struct thmorph_line
{
  thmorph_type m_type;     //!< morphing type of the line
  thmorph_pair * m_p1;     //!< first point pair
  thmorph_pair * m_p2;     //!< second point pair
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
  
  thmat2 R;                //!< rotation matrix R(from->to)
  thmat2 S;                //!< rotation matrix R(to->from)
  
  /** cstr
   * @param p1  first point pair
   * @param p2  second point pair
   *
   * @note as a side effect this line (pointer) is added to the two points
   */
  thmorph_line( thmorph_type t, thmorph_pair * p1, thmorph_pair * p2 );

  /** update reduced parameters of the line
   */
  void update();

  /** get the other end of the line
   * @param end    one end of the line
   * @return the other end (NULL if end is not an end of this line)
   */
  thmorph_pair * other_end( thmorph_pair * end )
  {
    if ( end == m_p1 ) return m_p2;
    if ( end == m_p2 ) return m_p1;
    return NULL;
  }

};

// --------------------------------------------------------------
// morphing feature elements

/** 
 * abstract class of a morphing feature element.
 * All morphing elements have one "missing" side and are
 * "extends" across that side by an amount specified be the
 * element's "bound"
 */
class thmorph_element
{
  protected:
    double m_bound;   //!< the element's bound

  public:
    /** cstr
     * @param bound   bound
     */
    thmorph_element( double bound ) 
      : m_bound( bound )
    { }

    /** dstr
     */
    virtual ~thmorph_element() {}

    virtual int id() const { return 0; }

    /** map p in the "from" image to a point in the "to" img
     * @param p   2D point in the "from" image 
     * @return corresponding 2D point in the "to" image
     */
    virtual thvec2 forward( const thvec2 & p ) = 0;

    /** map p in the "to" image to a point in the "from" img
     * @param p   2D point in the "to" image
     * @return corresponding 2D point in the "from" image
     */
    virtual thvec2 backward( const thvec2 & p ) = 0;

    /** check if a point in the "from" image is inside 
     * @param p   2D point in the "from" image
     * @return true if the point is inside 
     */
    virtual bool is_inside_from( const thvec2 & p ) = 0;

    /** check if a point in the "to" image is inside 
     * @param p   2D point in the "to" image
     * @return true if the point is inside
     */
    virtual bool is_inside_to( const thvec2 & p ) = 0;

    /** get the distance of apoint from the element
     * @param p   2D point in the "to" image
     * @return the distance of the point P from the element
     */
    virtual double distance_to( const thvec2 & p ) = 0;
    virtual double distance_from( const thvec2 & p ) = 0;

    /** squared distance from the segment
     * @param p   point in the "to" image
     * @return squared distance from the segment
     */
    virtual double distance2_to( const thvec2 & p ) = 0;
    virtual double distance2_from( const thvec2 & p ) = 0;


    /** get the element bounding-box
     * @param t1   upper left corner of the bounding-box in the "to" image
     * @param t2   lower right corner of the bounding-box in the "to" image
     */
    virtual void bounding_box_to( thvec2 & t1, thvec2 & t2 ) = 0;

    /** debug: print
     */
    virtual void print() const = 0;

};

/** a segment is a half-plane portion on the (right) side of a line
 * Use Berie Neely formula
 */
struct thmorph_segment
{
  thvec2 m_A, m_B;  //!< endpoints of the segment (from A to B)
  thvec2 m_AB;      //!< (B-A)
  thvec2 m_ABn;     //!< (B-A)/|B-A|^2
  thvec2 m_ABh;     //!< (B-A)^ / |B-A|

  /** cstr
   * @param a       point A
   * @param b       point B
   * @param reverse if true reverse the segment orientation (ie, use from B to A)
   */
  thmorph_segment( thvec2 & a, thvec2 & b, bool reverse = false )
    : m_A( a )
    , m_B( b )
  {
    init( reverse );
  }

  /** map a 2D point to BT coords
   * @param p  input point 
   * @return the mapped point
   *
   * <PRE>
   *    x = (P-A) * (B-A) / |B-A|^2
   *    y = (P-A) ^ (B-A) / |B-A|
   * </PRE>
   */
  thvec2 map( const thvec2 & p )
  {
    thvec2 pa = p - m_A;
    return thvec2( pa * m_ABn, pa * m_ABh );
  }

  private:
    /** initialize
     * @param reverse   whether to reverse orientation
     */
    void init( bool reverse );

};

/** a pair of segments. in correspondence
 * a segment is oriented from A to B. The geometry is
 * <PRE>
 *     + - A ----------- B - +
 *     |                     |
 *     + - - - - - - - - - - +   
 * </PRE>
 * The bound applies on y and -x and (x-1) for is_inside
 */
struct thmorph_segment_pair : public thmorph_element
{
  thmorph_segment from;   //!< segment in the "from" image
  thmorph_segment to;     //!< segment in the "to" image

  /** cstr
   * @param a    first endpoint pair
   * @param b    second endpoint pair 
   * @param bound size of the segment region
   * @param reverse whether to reverse the from segment orientation
   */
  thmorph_segment_pair( thmorph_pair * a, thmorph_pair * b, double bound = 1.0, 
                        bool reverse = false )
    : thmorph_element( bound * (a->z - b->z).length() )
    , from( a->z, b->z, reverse )
    , to  ( a->w, b->w )
  { }

  int id() const { return 2; }

  /** map p in the "from" image to a point in the "to" img
   * @param p   2D point in the "from" image 
   * @return corresponding 2D point in the "to" image
   */
  thvec2 forward( const thvec2 & p )
  {
    thvec2 v = from.map( p );
    return to.m_A + v.m_x * to.m_AB + v.m_y * to.m_ABh;
  }

  /** map p in the "to" image to a point in the "from" img
   * @param p   2D point in the "to" image
   * @return corresponding 2D point in the "from" image
   */
  thvec2 backward( const thvec2 & p );

  /** check if a point in the "from" image is inside the segment pos. half-plane
   * @param p   2D point in the "from" image
   * @return true if the point is inside ( orthogonal >= 0 )
   */
  bool is_inside_from( const thvec2 & p );

  /** check if a point in the "to" image is inside the segment pos. half-plane
   * @param p   2D point in the "to" image
   * @return true if the point is inside ( orthogonal >= 0 )
   */
  bool is_inside_to( const thvec2 & p );

  /** distance of a point from the segment 
   * @param p   point in the "to" image
   * @return distance from the segment
   */
  double distance_to( const thvec2 & p );
  double distance_from( const thvec2 & p );

  /** squared distance from the segment
   * @param p   point in the "to" image
   * @return squared distance from the segment
   */
  double distance2_to( const thvec2 & p );
  double distance2_from( const thvec2 & p );


  /** get the element bounding-box
   * @param t1   upper left corner of the bounding-box in the "to" image
   * @param t2   lower right corner of the bounding-box in the "to" image
   */
  void bounding_box_to( thvec2 & t1, thvec2 & t2 );

  /** debug: print
   */
  void print() const
  {
    thprintf("Segment  A %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_A.m_x, from.m_A.m_y, to.m_A.m_x, to.m_A.m_y );
    thprintf("         B %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_B.m_x, from.m_B.m_y, to.m_B.m_x, to.m_B.m_y );
  }


};



/** a triangle
 * The barycentric coords are defined as
 * <PRE>
 *    Px = a * Ax + b * Bx + c * Cx
 *    Py = a * Ay + b * By + c * Cy
 *    1  = a      + b      + c     
 * </PRE>
 * The mapping matrix M is the inverse of 
 * <PRE>
 *    |  Ax  Bx  Cx  |
 *    |  Ay  By  Cy  |
 *    |  1   1   1   |
 * </PRE>
 * so that the map of a point P is M * P
 */
struct thmorph_triangle
{
  thvec2 m_A, m_B, m_C;  //!< corners of the triangle (vertex, right, left)
  thmat3 M;              //!< map matrix

  /** cstr
   * @note there is aproblem if the three points are aligned,
   * ie, A ^ (B-C) = 0, because the matrix M1 is not invertible.
   */
  thmorph_triangle( thvec2 & a, thvec2 & b, thvec2 & c )
    : m_A( a )
    , m_B( b )
    , m_C( c )
  { 
    thmat3 M1( a.m_x, b.m_x, c.m_x,
               a.m_y, b.m_y, c.m_y,
               1.0,   1.0,   1.0 );
    M = M1.inverse();
  }

  /** map a 2D point to barycentric coords
   * @param p   input point
   * @return barycentric coords of the point
   */
  thline2 map( const thvec2 p )
  {
    return thline2( M.m_xx * p.m_x + M.m_xy * p.m_y + M.m_xz,
                    M.m_yx * p.m_x + M.m_yy * p.m_y + M.m_yz,
                    M.m_zx * p.m_x + M.m_zy * p.m_y + M.m_zz );
  }

};

/** a pair of triangles (in the two images) in correspondence 
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
 * The bounds applies to the extension across the side CB
 */
struct thmorph_triangle_pair : public thmorph_element
{
  thmorph_triangle from;   //!< triangle in the first image ("from")
  thmorph_triangle to;     //!< triangle in the second image ("to")

  /** cstr
   * @param a     A-corner point pair ("vertex" of the triangle)
   * @param b     B-corner point pair (right side)
   * @param c     C-corner point pair (left side)
   * @param bound bound on "a" for is_inside test
   */
  thmorph_triangle_pair( thmorph_pair * a, thmorph_pair * b, 
                         thmorph_pair * c, double bound = 4.0 )
    : thmorph_element( bound )
    , from( a->z, b->z, c->z )
    , to  ( a->w, b->w, c->w )
  { } 

  /** id of this morphing element
   * @return the number of points of the element
   */ 
  int id() const { return 3; }

  /** map p in the "from" image to a point in the "to" img
   * @param p   2D point in the "from" image 
   * @return corresponding 2D point in the "to" image
   */
  thvec2 forward( const thvec2 & p );

  /** map p in the "to" image to a point in the "from" img
   * @param p   2D point in the "to" image
   * @return corresponding 2D point in the "from" image
   */
  thvec2 backward( const thvec2 & p );

  /** check if a point in the "from" image is inside the triangle's angle
   * @param p   2D point in the "from" image
   * @return true if the point is inside ( 0<=s<=1 and 0<=t<=bound )
   */
  bool is_inside_from( const thvec2 & p );
 
  /** check if a point in the "to" image is inside the triangle's angle
   * @param p   2D point in the "to" image
   * @return true if the point is inside ( 0<=s<=1 and 0<=t<=bound )
   */
  bool is_inside_to( const thvec2 & p );

  /** distance of a point from the triangle
   * @param p   point in the "to" image
   * @return distance from the vertex of the triangle
   */
  double distance_to( const thvec2 & p );
  double distance_from( const thvec2 & p );
  
  /** squared distance from the segment
   * @param p   point in the "to" image
   * @return squared distance from the segment
   */
  double distance2_to( const thvec2 & p );
  double distance2_from( const thvec2 & p );

  /** get the element bounding-box
   * @param t1   upper left corner of the bounding-box in the "to" image
   * @param t2   lower right corner of the bounding-box in the "to" image
   */
  void bounding_box_to( thvec2 & t1, thvec2 & t2 );

  /** debug: print
   */
  void print() const
  {
    thprintf("Triangle A %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_A.m_x, from.m_A.m_y, to.m_A.m_x, to.m_A.m_y );
    thprintf("         B %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_B.m_x, from.m_B.m_y, to.m_B.m_x, to.m_B.m_y );
    thprintf("         C %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_C.m_x, from.m_C.m_y, to.m_C.m_x, to.m_C.m_y );
  }

};



/** four corners plaquette (quadrilater)
 * TRANSFORMATION selects the transform, either "interpolation" or
 * "bilinear". they are actually the same.
 *
 * @see P.S. Heckbert "Fundamentals of Texture Mapping and Image Warping"
 *      UCA Berkeley, 1989
 *
 * @note projective transform is not fine because it is not linear on the
 *       sides and this introduces 0-order discontinuities on the plaquette
 *       borders.
 */
#define TRANSFORMATION 1

struct thmorph_plaquette
{
  thvec2 m_A, m_B, m_C, m_D;  //!< vertices of the plaquette

  thvec2 m_AD;
  thvec2 m_BC;
#if TRANSFORMATION == 0
  // intersection interpolation
  double m_ab;    //!< A ^ B
  double m_ac;    //!< A ^ C
  double m_ad;    //!< A ^ D
  double m_db;    //!< D ^ B
  double m_bd;    //!< B ^ D
  double m_dc;    //!< D ^ C
  double m_bc;    //!< B ^ C
  thvec2 m_AC_BD;  //!<  m_A + m_C - m_B - m_D;
  thvec2 m_AB;     //!<  m_A - m_B;
  thvec2 m_DC;
  double m_at;      //!<  m_ab - m_ac - m_db + m_dc;
  double m_bt;      //!<  m_ac + m_db  - 2 * m_ab;
  double m_as;
  double m_bs;
#elif TRANSFORMATION == 1
  // bilinear interpolation
  double m_a, m_b, m_c, m_d, m_e, m_f, m_g, m_h; 
  double m_A0, m_B0, m_C0;
  // double m_D0, m_E0, m_F0;
#endif

  thmorph_plaquette( thvec2 & a, thvec2 & b, thvec2 & c, thvec2 & d )
    : m_A( a )
    , m_B( b )
    , m_C( c )
    , m_D( d )
  { 
    init();
  }

  /** map a point to plaquette coordinates
   * @param p 2D point
   * @return plaquette coordinates of the point
   *
   * @pre the point should lie inside the plaquette
   */
  thvec2 map( const thvec2 & p );

  private:
    /** initialize the plaquette
     */
    void init();
};

/** the plaquette of four point pairs. The geometry is
 * <PRE>  
 *        + - - - - - - +
 *        |             |
 *        D - - - - - - C
 *        |             |
 *        A ----------- B
 * <\PRE>
 * The bound applies to the extension across the side CD
 */
struct thmorph_plaquette_pair : public thmorph_element
{
  thmorph_plaquette from;   //!< plaquette in the "from" image
  thmorph_plaquette to;     //!< plaquette in the "to" image
  // thhomography m_H; 

  /** cstr
   * @param a    A-corner point pair
   * @param b    B-corner point pair
   * @param c    C-corner point pair
   * @param d    D-corner point pair
   * @param bound bound of "t" for the is_inside test
   */
  thmorph_plaquette_pair( thmorph_pair * a, thmorph_pair * b, 
                          thmorph_pair * c, thmorph_pair * d,
                          double bound = 2.0 )
    : thmorph_element( bound )
    , from( a->z, b->z, c->z, d->z )
    , to  ( a->w, b->w, c->w, d->w )
    // , m_H( a, b, c, d )
  { }
 
  int id() const { return 4; }

  /** map p in the "from" image to a point in the "to" img
   * @param p   2D point in the "from" image 
   * @return corresponding 2D point in the "to" image
   */
  thvec2 forward( const thvec2 & p ); 
  // thvec2 h_forward( const thvec2 & p ); 

  /** map p in the "to" image to a point in the "from" img
   * @param p   2D point in the "to" image
   * @return corresponding 2D point in the "from" image
   */
  thvec2 backward( const thvec2 & p );
  // thvec2 h_backward( const thvec2 & p );

  /** check if a point in the "from" image is inside the plaquette
   * @param p   2D point in the "from" image
   * @return true if the point is inside ( 0<=s<=1 and 0<=t<=bound )
   */
  bool is_inside_from( const thvec2 & p );
 
  /** check if a point in the "to" image is inside the plaquette
   * @param p   2D point in the "to" image
   * @return true if the point is inside ( 0<=s<=1 and 0<=t<=bound )
   */
  bool is_inside_to( const thvec2 & p );

  /** distance of a point from the plaquette
   * @param p   point in the "to" image
   * @return distance from the base (AB) of the plaquette
   */
  double distance_to( const thvec2 & p );
  double distance_from( const thvec2 & p );

  /** squared distance from the segment
   * @param p   point in the "to" image
   * @return squared distance from the segment
   */
  double distance2_to( const thvec2 & p );
  double distance2_from( const thvec2 & p );

  /** get the element bounding-box
   * @param t1   upper left corner of the bounding-box in the "to" image
   * @param t2   lower right corner of the bounding-box in the "to" image
   */
  void bounding_box_to( thvec2 & t1, thvec2 & t2 );

  /** debug: print
   */
  void print() const
  {
    thprintf("Plaquete A %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_A.m_x, from.m_A.m_y, to.m_A.m_x, to.m_A.m_y );
    thprintf("         B %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_B.m_x, from.m_B.m_y, to.m_B.m_x, to.m_B.m_y );
    thprintf("         C %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_C.m_x, from.m_C.m_y, to.m_C.m_x, to.m_C.m_y );
    thprintf("         D %6.2f %6.2f <-> %6.2f %6.2f\n",
      from.m_D.m_x, from.m_D.m_y, to.m_D.m_x, to.m_D.m_y );
  }


};

#endif


