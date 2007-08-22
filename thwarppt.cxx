/** @file thwarppt.cxx
 * 
 * @author marco corvi
 * @date   nov 2006 - mar 2007
 * 
 * @brief warping plaquette algorithm transform implementation
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
#include <assert.h>
#include <sstream>

#include "thwarppt.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

typedef unsigned int warpp_t;
const warpp_t ngbh_mask = ((warpp_t)(0x7))<<(8*sizeof(warpp_t)-3);
const warpp_t indx_mask = ~ngbh_mask;


therion::warp::plaquette_algo::plaquette_algo( )
  : m_initialized( false )
  , m_bound3( BOUND_TRIANGLE )
  , m_bound4( BOUND_PLAQUETTE )
{ 
}

therion::warp::plaquette_algo::plaquette_algo( double bound3, double bound4 )
  : m_initialized( false )
  , m_bound3( bound3 )
  , m_bound4( bound4 )
{ 
}

therion::warp::plaquette_algo::~plaquette_algo( )
{
  reset();
}

void 
therion::warp::plaquette_algo::reset( )
{
  for (size_t i=0; i<mLines.size(); ++i) {
    delete mLines[i];
  }
  mLines.clear();

  for (size_t i=0; i<mPairs.size(); ++i) {
    delete mPairs[i];
  }
  mPairs.clear();

  for (size_t i=0; i<mPlaquettes.size(); ++i) {
    delete mPlaquettes[i];
  }
  mPlaquettes.clear();

  m_initialized = false;
}

therion::warp::point_pair *
therion::warp::plaquette_algo::insert_point( 
    therion::warp::morph_type t, 
    std::string & name, 
    const thvec2 & src, 
    const thvec2 & dst )
{
  point_pair * pair = new point_pair( t, name, src.m_x, src.m_y, dst.m_x, dst.m_y );
  mPairs.push_back( pair );
  return pair;
}

void 
therion::warp::plaquette_algo::insert_line(
     therion::warp::morph_type t, 
     std::string & from, 
     std::string & to )
{
  point_pair * p1 = NULL;
  point_pair * p2 = NULL;
  for ( size_t i=0; i<mPairs.size(); ++i ) {
    if ( mPairs[i]->m_name == from ) { p1 = mPairs[i]; if (p2) break; }
    if ( mPairs[i]->m_name == to   ) { p2 = mPairs[i]; if (p1) break; }
  }
  if ( p1 == NULL || p2 == NULL ) throw;
  mLines.push_back( new therion::warp::line( t, p1, p2 ) );
}

#ifdef DEBUG
void
therion::warp::plaquette_algo::print()
{
  printf("Plaquette algo: P.nr %d L.nr %d\n", mPairs.size(), mLines.size() );
  printf("  X origin %.2f %.2f units %.4f\n", mX0.m_x, mX0.m_y, mXUnit );
  printf("  U origin %.2f %.2f units %.4f\n", mU0.m_x, mU0.m_y, mUUnit );
  printf("  UC origin %.2f %.2f units %.2f\n", mUC.m_x, mUC.m_y, mUCUnit);

  for ( size_t i=0; i<mPairs.size(); ++i ) {
    printf("P %2d: %s (T %d) survey %6.2f %6.2f sketch %6.2f %6.2f\n", 
      i, mPairs[i]->m_name.c_str(), mPairs[i]->m_type, 
      mPairs[i]->u.m_x, mPairs[i]->u.m_y, mPairs[i]->x.m_x, mPairs[i]->x.m_y );
  }
  for ( size_t i=0; i<mLines.size(); ++i ) {
    printf("L %2d: %s %s (T %d)\n", i, mLines[i]->m_p1->m_name.c_str(),
      mLines[i]->m_p2->m_name.c_str(), mLines[i]->m_type );
  }
  for ( size_t i=0; i<mPlaquettes.size(); ++i ) 
    mPlaquettes[i]->print();
}
#endif

bool
therion::warp::plaquette_algo::initialize( warp_proj proj )
{
  if ( mPairs.size() < 2 )
    return false;

  if ( ! m_initialized ) {
    // thprintf("plaquette_algo::initialize() \n");
    size_t sz = mPairs.size();

    // bounding box
    thvec2 xmin( mPairs[0]->x );
    thvec2 xmax( xmin );
    thvec2 umin( mPairs[0]->u );
    thvec2 umax( umin );

    for ( size_t k=1; k<sz; ++k) {
      xmin.minimize( mPairs[k]->x );
      xmax.maximize( mPairs[k]->x );
      umin.minimize( mPairs[k]->u );
      umax.maximize( mPairs[k]->u );
    }
    mX0 = (xmin + xmax) / 2.0;
    mU0 = (umin + umax) / 2.0;

    // units
    mXUnit = (xmax - xmin).length() / 2.0;
    mUUnit = (umax - umin).length() / 2.0;

    // scaled coordinates (relative to the centers)
    for ( size_t k=0; k<sz; ++k ) {
        mPairs[k]->z = ( mPairs[k]->x - mX0 ) / mXUnit;
        mPairs[k]->w = ( mPairs[k]->u - mU0 ) / mUUnit;
    }
    mUC = mU0;
    mUCUnit = mUUnit;

    update_lines();
    make_plaquettes( proj );

    make_neighbors( );

    /* print topology
    for (unsigned int k=0; k<mPlaquettes.size(); ++k ) {
      thprintf("Neighbor %2d left %2d right %2d\n",
        mPlaquettes[k]->nr(), mPlaquettes[k]->left_ngbh()->nr(),
        mPlaquettes[k]->right_ngbh()->nr() );
    }
    */
  
    m_initialized = true;
  }
  return m_initialized;
}

therion::warp::point_pair * 
therion::warp::plaquette_algo::insert_zoom_point(
  morph_type t, std::string & name, 
  const thvec2 src, std::string & from, double size )
{
  size_t k;
  size_t sz = mPairs.size();
  for (k=0; k<sz; ++k) {
    if ( mPairs[k]->m_name == from ) break;
  }
  if ( k == sz ) {
    thprintf("warning insert_zoom_point() from point \"%s\" not found\n", 
      from.c_str() );
    return NULL;
  }
  double x0 = mPairs[k]->x.m_x;
  double y0 = mPairs[k]->x.m_y;
  thprintf("insert_zoom_point() %s (%.2f %.2f) from %s (%.2f %.2f) distance %.2f\n",
    name.c_str(), src.m_x, src.m_y, from.c_str(), x0, y0, size );

  double x1 = src.m_x - x0;
  double y1 = src.m_y - y0;
  double dd = sqrt( x1*x1 + y1*y1 );
  x1 /= dd;
  y1 /= dd;

  double a1=7.0, a2=7.0;
  point_pair * p1 = NULL; // to the left
  point_pair * p2 = NULL; // to the right
  size_t sl = mLines.size();
  double x2, y2;
  double sa, ca, a;
  for (size_t h=0; h<sl; ++h) {
    if ( mLines[h]->m_type != THMORPH_STATION ) continue;
    point_pair * p;
    if ( mLines[h]->m_p1 == mPairs[k] ) {
      p = mLines[h]->m_p2;
    } else if ( mLines[h]->m_p2 == mPairs[k] ) {
      p = mLines[h]->m_p1;
    } else { 
      continue;
    }
    x2 = p->x.m_x - x0;
    y2 = p->x.m_y - x0;
    dd = sqrt(x2*x2 + y2*y2);
    x2 /= dd;
    y2 /= dd;
    sa = x2*y1 - y2*x1;
    ca = x1*x2 + y1*y2;
    a = atan2( sa, ca );
    if ( a < 0.0 ) a += 2*M_PI;
    if ( a < a1 ) { a1=a; p1=p; }
    a = 2*M_PI - a;
    if ( a < a2 ) { a2=a; p2=p; }
  }
  if ( p1 == NULL || p2 == NULL || p1 == p2 ) return NULL;
  x0 = mPairs[k]->u.m_x;
  y0 = mPairs[k]->u.m_y;

  x1 = p1->u.m_x - x0;
  y1 = p1->u.m_y - y0;
  dd = sqrt( x1*x1 + y1*y1 );
  x1 /= dd;
  y1 /= dd;

  x2 = p2->u.m_x - x0;
  y2 = p2->u.m_y - y0;
  double d2 = sqrt( x2*x2 + y2*y2 );
  x2 /= d2;
  y2 /= d2;
  if ( dd < d2 ) d2 = dd;
  if ( size < d2 / 10 ) return NULL;

  ca = x1*x2 + y1*y2;
  sa = sqrt( 1 - ca );
  ca = sqrt( 1 + ca );

  x0 += ( x2*ca + y2*sa) * size;
  y0 += (-x2*sa + y2*ca) * size;

  thprintf("left %s (%.2f %.2f)  right %s (%.2f %.2f) ==> %.2f %.2f\n",
    p1->m_name.c_str(), p1->u.m_x, p1->u.m_y, p2->m_name.c_str(), p2->u.m_x, p2->u.m_y, x0, y0 );

  point_pair * pair = new point_pair( t, name, src.m_x, src.m_y, x0, y0 );
  mPairs.push_back( pair );
  mLines.push_back( new therion::warp::line( t, mPairs[k], pair ) );
  return pair;
}
    

therion::warp::line *
therion::warp::plaquette_algo::add_extra_line( 
    point_pair * p1, 
    size_t index, 
    thvec2 & x3, 
    thvec2 & u3 )
{
  std::ostringstream ost;
  ost << p1->m_name << "_E_" << index;
  std::string name = ost.str();
  // thprintf("Extra line() point %s %6.2f %6.2f %6.2f %6.2f \n",
  //    name.c_str(), x3.m_x, x3.m_y, u3.m_x, u3.m_y );

  point_pair * p3 = insert_point( THMORPH_EXTRA, name, x3, u3 );
  p3->update( mX0, mXUnit, mU0, mUUnit );
  
  therion::warp::line * l3 = new therion::warp::line( THMORPH_EXTRA, p1, p3 );
  l3->update();
  mLines.push_back( l3 );
  return l3;
}

// ========================================================================

bool
therion::warp::plaquette_algo::forward( 
    const thvec2 & src, 
    thvec2 & dst, 
    const thvec2 & origin, 
    double unit )
{
  thvec2 z = ( src - mX0) / mXUnit;
  size_t szp = mPlaquettes.size();
  double dmin = -1.0;
  size_t imin = (size_t)(-1);
  for ( size_t i=0; i<szp; ++i) {
    if ( mPlaquettes[i]->is_inside_from( z ) ) {
      double d = mPlaquettes[i]->distance_from( z );
      if ( thisnan( d ) ) continue;
      if ( imin == (size_t)(-1) || d < dmin ) {
        dmin = d;
        imin = i;
      }
    }
  }
  if ( imin == (size_t)(-1) ) 
    return false;
  thvec2 w;
  mPlaquettes[imin]->forward( z, w );
  if ( w.is_nan() )
    return false;
  dst = ( origin + w * unit );
  return true;
}

bool
therion::warp::plaquette_algo::backward(
    const thvec2 & dst, 
    thvec2 & src, 
    const thvec2 & origin, 
    double unit )
{
  thvec2 w = (dst - origin) / unit;
  size_t szp = mPlaquettes.size();
  double dmin = -1.0;
  size_t imin = (size_t)(-1);
  for ( size_t i=0; i<szp; ++i) {
    if ( mPlaquettes[i]->is_inside_to( w ) ) {
      double d = mPlaquettes[i]->distance_to( w );
      if ( thisnan( d ) ) continue;
      if ( imin == (size_t)(-1) || d < dmin ) {
        dmin = d;
        imin = i;
      }
    }
  }
  if ( imin == (size_t)(-1) ) 
    return false;
    thvec2 z;
    mPlaquettes[imin]->backward( w, z );
    if ( z.is_nan( ) ) 
      return false;
    src = mX0 + z * mXUnit;
  return true;
}

void 
therion::warp::plaquette_algo::update_lines()
{
  for (size_t i=0; i<mLines.size(); ++i) {
    mLines[i]->update();
  }
}

void 
therion::warp::plaquette_algo::make_neighbors( )
{
  // thprintf("plaquette_algo::make_neighbors() plaquettes %d \n", mPlaquettes.size() );
  for (size_t k=0; k<mPlaquettes.size(); ++k) {
    find_neighbors( mPlaquettes[k] );
    // mPlaquettes[k]->compute_slopes();
  }
}

void
therion::warp::plaquette_algo::find_neighbors( therion::warp::basic_pair * me )
{
  
  switch ( me->type() ) {
    case THWARP_TRIANGLE:
    case THWARP_PLAQUETTE:
      for (int k=0; k<me->ngbh_nr(); ++k) 
        me->m_ngbh[k] = find_plaquette( me->m_pair[k+1], me->m_pair[k] );
      break;
    default:
      thprintf("plaquette_algo::make_neighbors() wrong pair warp-type %d\n",
        me->type() );
  }
}

therion::warp::basic_pair * 
therion::warp::plaquette_algo::find_plaquette( 
    point_pair * p1, 
    point_pair * p2 )
{
  // thprintf("find_plaquette() %s %s \n", p1->m_name.c_str(), p2->m_name.c_str() );
  for (size_t k=0; k<mPlaquettes.size(); ++k ) {
    int i2 = mPlaquettes[k]->ngbh_nr();
    if ( i2 >= 1 && i2 < 4 ) {
      for (int i=0; i<i2; ++i) {
        if ( mPlaquettes[k]->m_pair[i] == p1 && mPlaquettes[k]->m_pair[i+1] == p2 ) {
	  // thprintf("found %d\n", mPlaquettes[k]->nr() );
	  return mPlaquettes[k];
	}
      }
    }
  }
  return NULL;
}



void 
therion::warp::plaquette_algo::make_plaquettes( warp_proj proj )
{
  for (size_t k=0; k<mPlaquettes.size(); ++k) {
    delete mPlaquettes[k];
  }
  mPlaquettes.clear();

  // print();
  size_t szp = mPairs.size();
  for (size_t i=0; i<szp; ++i) {
      mPairs[i]->order_lines( this, mXUnit/mUUnit, proj );
  }
  // print();

  szp = mPairs.size();
  for (size_t i=0; i<szp; ++i) {
    mPairs[i]->m_used = false;
  }

  for ( ; ; ) {
    therion::warp::point_pair * node1 = NULL;
    therion::warp::line * line = NULL;
    for (size_t i=0; i<szp; ++i) {
      /*
      if ( mPairs[i]->legs() == 1 && ! mPairs[i]->m_used ) {
        node1 = mPairs[i];
        line  = node1->first_leg();
        for ( ++i; i<szp; ++i) {
          if ( mPairs[i]->size() == 1 ) {
            therion::warp::line * ln = mPairs[i]->next_line(NULL);
            if ( ln && ln->m_type == THMORPH_STATION ) 
              ln->m_type = THMORPH_EXTRA;
          }
        }
        break; // not really necessary
      }
      */
      if ( mPairs[i]->size() == 1 && ! mPairs[i]->m_used ) {
        node1 = mPairs[i];
        line  = node1->next_line(NULL);
        line->m_type = THMORPH_STATION; // make the line into a station line
        break;
      }
    }
    if ( node1 == NULL ) { 
      // thprintf("Error: cannot find a 1-leg node\n");
      return;
    }
  
    therion::warp::point_pair * node2 = line->other_end( node1 );
    therion::warp::point_pair * node10 = node1; // save starting nodes
    therion::warp::point_pair * node20 = node2;
    // thprintf("make_plaquettes()");
    // thprintf(" node1 %s node2 %s\n", node1->m_name.c_str(), node2->m_name.c_str() );
  
    do {
      therion::warp::point_pair * A = node1;
      therion::warp::point_pair * B = node2;
      if ( line->m_type == THMORPH_STATION ) {
        size_t iA = A->size();
        size_t iB = B->size();
        if ( iA == 1 ) // mark the node as used
          A->m_used = true;
        if ( iB == 1 ) 
          B->m_used = true;

        // thprintf("  A %s (%d)  B %s (%d) line type S\n", A->m_name.c_str(), iA, B->m_name.c_str(), iB );
        if ( iA == 1 && iB == 1 ) { 
	  thprintf("ERROR: segments are no longer supported\n");
        } else if ( iA == 1 ) {
          //                      /
          //           A ======= B ... 
          //                     | 
          //                     C
          //
          // do not do anything: B will do it
          therion::warp::line * line2 = B->next_line( line );
          therion::warp::point_pair * C = line2->other_end( B );
          if ( line2->m_type != THMORPH_STATION )
            add_triangle( A, B, C, proj );
          node1 = B;
          node2 = C;
          line = line2;
        } else if ( iB == 1 ) {
          //
          //           | 
          //       ... A ======= B  
          //          / 
          //         D
          therion::warp::line * line1 = A->prev_line( line );
          therion::warp::point_pair * D = line1->other_end( A );
          add_triangle( D, A, B, proj );
          node1 = B;
          node2 = A;
          // line = line;
        } else {
          //    ... A ========= B ...
          //        |           |
          //        D           C
          //
          therion::warp::line * line1 = A->prev_line( line );
          therion::warp::point_pair * D = line1->other_end( A );
          therion::warp::line * line2 = B->next_line( line );
          therion::warp::point_pair * C = line2->other_end( B );
          add_quadrilater( A, B, C, D, proj );
          node1 = B;
          node2 = C;
          line = line2;
        }
      } else { // line->m_type != THMORPH_STATION 
        size_t iA = A->size();
        size_t iB = B->size();
        if ( iA == 1 ) // mark the node as used
          A->m_used = true;
        if ( iB == 1 ) 
          B->m_used = true;

        // thprintf("  A %s B %s line type non-S\n", A->m_name.c_str(), B->m_name.c_str() );
        //      ?
        //      |
        //      A ======== B
        //
        therion::warp::line * line2 = A->next_line( line );
        if ( line2->m_type == THMORPH_STATION ) {
          //    node_2
          //      |
          //      A ======== B
          //
          line = line2;
          node2 = line->other_end( A );
          // insertion is handled at the next iteration
        } else {
          //      D
          //      |
          //      A ======== B
          //
          therion::warp::point_pair * D = line2->other_end( A );
          add_triangle( B, A, D, proj );
          node2 = D;
          line = line2;
        }
      }
    } while ( node1 != node10 );
    // back to start
    while ( node2 != node20 ) {
      therion::warp::line * line2 = node1->next_line( line );
      therion::warp::point_pair * C = line2->other_end( node1 );
      add_triangle( node2, node1, C, proj );
      node2 = C;
      line = line2;
    }
  } // end big for ( ; ; )
}


void
therion::warp::plaquette_algo::add_triangle( point_pair * A, point_pair * B, point_pair * C,
  warp_proj proj )
{
  // N.B. bad triangles are only warned; 
  // they are used anyways.
  if ( ((C->x - B->x) ^ (A->x - B->x)) >= 0.0 ) {
    thprintf("WARNING: bad triangle (%s %s %s) X-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } else if ( ((C->u - B->u) ^ (A->u - B->u)) >= 0.0 ) {
    thprintf("WARNING: bad triangle (%s %s %s) U-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } // else {
    therion::warp::triangle_pair * tri = new therion::warp::triangle_pair( B, C, A, this->m_bound3 );
    tri->set_projection( proj );
    mPlaquettes.push_back( tri );
  // }
}

void
therion::warp::plaquette_algo::add_quadrilater( 
     point_pair * A, point_pair * B, point_pair * C, point_pair * D,
     warp_proj proj )
{
  // thprintf("  ADD Plaquette: %s %s %s %s\n",
  //   A->m_name.c_str(), B->m_name.c_str(), C->m_name.c_str(), D->m_name.c_str() );
  // check orientation
  if ( ((B->x - A->x) ^ (D->x - A->x)) >= 0.0 ) {
    thprintf("WARNING: bad plaquette (%s %s %s) X-orientation\n",
      B->m_name.c_str(), A->m_name.c_str(), D->m_name.c_str() );
  } else if ( ((C->x - B->x) ^ (A->x - B->x)) >= 0.0 ) {
    thprintf( "WARNING: bad plaquette (%s %s %s) X-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } else if ( ((B->u - A->u) ^ (D->u - A->u)) >= 0.0 ) {
    thprintf("WARNING: bad plaquette (%s %s %s) U-orientation\n",
      B->m_name.c_str(), A->m_name.c_str(), D->m_name.c_str() );
  } else if ( ((C->u - B->u) ^ (A->u - B->u)) >= 0.0 ) {
    thprintf("WARNING: bad plaquette (%s %s %s) U-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } else {
    therion::warp::plaquette_pair * plaq = new therion::warp::plaquette_pair( A, B, C, D, this->m_bound4 );
    plaq->set_projection( proj );
    mPlaquettes.push_back( plaq );
  }
}




bool
therion::warp::plaquette_algo::map_backward_plaquette( size_t k, const thvec2 & w, thvec2 & z )
{
  if ( mPlaquettes[k]->is_inside_to( w ) ) {
    mPlaquettes[k]->backward( w, z );
    return true;
  }
  return false;
}


bool
therion::warp::plaquette_algo::map_image( const unsigned char * src, unsigned int ws, unsigned int hs,
                       unsigned char * dst, unsigned int wd, unsigned int hd,
		       thvec2 const & origin, double unit,
                       int depth, 
		       warp_proj proj )
{
  if ( ! initialize( proj ) ) return false;

  mUC     = origin;
  mUCUnit = unit;

  double res[4];
  assert( depth <= 4 ); // max 4 components
  unsigned int wsd = ws * depth;

  thvec2 z;

  // print();

  // thprintf("X origin %6.2f %6.2f units %6.2f\n", mX0.m_x, mX0.m_y, mXUnit);
  // thprintf("U origin %6.2f %6.2f units %6.2f\n", mU0.m_x, mU0.m_y, mUUnit);
  // thprintf("UC origin %6.2f %6.2f units %6.2f\n", mUC.m_x, mUC.m_y, mUCUnit);
  // new way
  warpp_t * pi = (warpp_t *)malloc( wd * hd * sizeof(warpp_t) );
  if ( pi != NULL ) {
    memset( pi, 0xff, wd*hd*sizeof(warpp_t) );
    double * pf = (double *)malloc( wd * hd * sizeof(double) );
    if ( pf == NULL ) 
      thprintf("warning: failed to allocate temporary distance image\n");

    for (size_t k=0; k<mPlaquettes.size(); ++k ) {
      assert( ( ( (warpp_t)k) & indx_mask) == (warpp_t)k );
      // int cnt = 0;
      // thprintf("mapping plaquette %d\n", k );
      thvec2 t1, t2;
      mPlaquettes[k]->bounding_box_to( t1, t2 );
      t1 = mUC + mUCUnit * t1;
      t2 = mUC + mUCUnit * t2;
      t1.maximize( thvec2(0,0) );
      t2.minimize( thvec2(wd, hd) );
      int i1 = (int)t1.m_x;
      int i2 = (int)t2.m_x;
      int j2 = (int)t2.m_y;
      for (int j=(int)t1.m_y; j<j2; ++j) {
        warpp_t * piwd = pi + j*wd;
        double  * pfwd = pf + j*wd;
        thvec2 u( i1, j );
        for (int i=i1; i<i2; ++i) {
          // thvec2 u( i, j );
          // thvec2 w = (u - mUC) / mUCUnit;
	  thvec2 w( (u.m_x - mUC.m_x)/mUCUnit, (u.m_y - mUC.m_y)/mUCUnit );
          // if ( map_backward_plaquette( k, w, z ) ) {
	  if ( mPlaquettes[k]->is_inside_to( w ) ) {
	    // ++cnt;
            if ( ( piwd[i] & ngbh_mask ) == ngbh_mask ) {
              piwd[i] = (warpp_t)k;
	      if ( pf ) pfwd[i] = -1.0; // delay distance computation
	      // pfwd[i] = mPlaquettes[k]->distance2_to( w );
            } else {
              double d1 = mPlaquettes[k]->distance2_to( w );
	      if ( pf ) {
	        // delayed distance computation
	        if ( pfwd[i] < 0.0 ) pfwd[i] = mPlaquettes[piwd[i]]->distance2_to( w );
                double d0 = pfwd[i];
                if ( d1 < d0 ) { 
		  piwd[i] = (warpp_t)k;
		  pfwd[i] = d1; 
		}
	      } else {
	        double d0 = mPlaquettes[piwd[i]]->distance2_to( w );
		if ( d1 < d0 ) { 
		  piwd[i] = (warpp_t)k; 
		}
	      }
            }
          }
	  u.m_x += 1.0;
        }
      }
      // thprintf("plaquette %2d: points %d\n", k, cnt );
    }
    warpp_t * pij = pi;

    // not sure if it's better to random-access or scan-through
    // if the output image is dense a scan-through should be better
    // if it is "sparse" random-access should be better
    // in the following i opt for scan-through on the destination image
    // but random-access of the (i,j) vector
    //
    // unsigned int wdd = wd * depth;
    unsigned char * dst1 = dst;
    for (unsigned int j=0; j<hd; ++j) {
      for (unsigned int i=0; i<wd; ++i) {
        warpp_t kp = *pij++;
        // if ( kp < mPlaquettes.size() ) {
	if ( (kp & ngbh_mask) != ngbh_mask ) {
          thvec2 u( i, j );
          // thvec2 w = (u - mUC) / mUCUnit;
	  thvec2 w( (u.m_x - mUC.m_x)/mUCUnit, (u.m_y - mUC.m_y)/mUCUnit );
          map_backward_plaquette( kp, w, z );
          // thvec2 x = mX0 + z * mXUnit;
          thvec2 x( mX0.m_x + z.m_x * mXUnit, mX0.m_y + z.m_y * mXUnit );
          int ix1 = (int)x.m_x;
          int ix2 = ix1 + 1;
          if ( ix1 >= 0 && ix2 < (int)ws ) {
            int iy1 = (int)x.m_y;
            int iy2 = iy1 + 1;
	    if ( iy1 >= 0 && iy2 < (int)hs ) {
	      // bilinear interpolation: does not cost much more than nearest-pixel
              double dx = x.m_x - ix1;  
              double dy = x.m_y - iy1;
	      double dx1dy1 = (1-dx) * (1-dy);
	      double dx1dy0 = (1-dx) * dy;
	      double dx0dy1 = dx * (1-dy);
	      double dx0dy0 = dx * dy;
              // unsigned char * dst1 = dst + (j * wdd + i * depth);
              const unsigned char * src00 = src + iy1*wsd + ix1 * depth;
              const unsigned char * src01 = src00 + wsd;
              const unsigned char * src10 = src00 + depth;
              const unsigned char * src11 = src01 + depth;
              for (int c=0; c<depth; ++c) {
                res[c] = src00[c] * dx1dy1 + src10[c] * dx0dy1
                       + src01[c] * dx1dy0 + src11[c] * dx0dy0;
                dst1[c] = (unsigned char)(( res[c] >= 255 ) ? 255 : res[c]);
              }
	    }
	  }
        }
	dst1 += depth;
      }
    }
    if ( pf ) free( pf );
    free( pi );
  } else { // old way (used if pi malloc failed ...
    thprintf("warning: failed to allocate temporary index image\n");
    for (size_t k=0; k<mPlaquettes.size(); ++k ) {
      // thprintf("mapping plaquette %d\n", k );
      thvec2 t1, t2;
      mPlaquettes[k]->bounding_box_to( t1, t2 );
      t1 = mUC + mUCUnit * t1;
      t2 = mUC + mUCUnit * t2;
      t1.maximize( thvec2(0,0) );
      t2.minimize( thvec2(wd, hd) );
      int i2 = (int)t2.m_x;
      int j2 = (int)t2.m_y;
      for (int j=(int)t1.m_y; j<j2; ++j) {
        for (int i=(int)t1.m_x; i<i2; ++i) {
          thvec2 u( i, j );
          thvec2 z;
          thvec2 w = (u - mUC) / mUCUnit;
          if ( map_backward_plaquette( k, w, z ) ) {
            thvec2 x = mX0 + z * mXUnit;
            int ix1 = (int)x.m_x;
            int ix2 = ix1 + 1;
            int iy1 = (int)x.m_y;
            int iy2 = iy1 + 1;
            if ( ix1 >= 0 && ix2 < (int)ws && iy1 >= 0 && iy2 < (int)hs ) {
              double dx = x.m_x - ix1;  // bilinear interpolation
              double dy = x.m_y - iy1;
              unsigned char * dst1 = dst + depth * (j * wd + i);
              const unsigned char * src00 = src + depth * ( iy1*ws + ix1 );
              const unsigned char * src01 = src00 + depth * ws;
              const unsigned char * src10 = src00 + depth;
              const unsigned char * src11 = src01 + depth;
              for (int k=0; k<depth; ++k) {
                double res = ( src00[k] * (1-dx) +  src10[k] * dx ) * (1-dy)
                           + ( src01[k] * (1-dx) +  src11[k] * dx ) * dy;
                dst1[k] = (unsigned char)(( res >= 255 ) ? 255 : res);
              }
            }
          }
        }
      }
    }
  }

  return true;
}
