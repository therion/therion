/** @file thwarppme.cxx
 *
 * @author marco corvi
 * @date nov 2006 - mar 2007
 *
 * @brief Warping plaquette algo structures
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
#include <assert.h>
#include <math.h>

#include "thinfnan.h"
#include "thwarppme.h"

int therion::warp::basic_pair::basic_pair_nr = 0;

namespace therion
{
  namespace warp 
  {



/** compute vertical-ness
 * @param a   X-coordinate factor
 * @param x   X vector
 * @param u   U vector
 * @return the min vertical-ness between X and U
 *
 * vertical-ness is
 *   - positive if Y-coord is positive
 *   - negative if Y-coord is negative
 *   - the closer to 0 the more vertical is the vector
 */
double
compute_vertical( double a, thvec2 & x, thvec2 & u )
{
  double vx = ( fabs(x.m_x * a) < fabs(x.m_y) )? 0.0 
              : ( (x.m_y > 0.0)? (1.0 - x.m_y/fabs( x.m_x * a ))
                               : (x.m_y/fabs( x.m_x * a ) -1.0) );
  double vu = ( fabs(u.m_x * a) < fabs(u.m_y) )? 0.0 
              : ( (u.m_y > 0.0)? (1.0 - u.m_y/fabs( u.m_x * a ))
                               : (u.m_y/fabs( u.m_x * a ) - 1.0) );
  return ( fabs(vx) < fabs(vu) )? vx : vu;
}

/** compute the squared distance from a segment
 * @param x   input vector
 * @param b   first endpoint of the segment
 * @param c   second endpoint of the segment
 */
double
compute_segment_distance2( const thvec2 & x, const thvec2 & b, const thvec2 & c )
{
  double xcb = c.m_x - b.m_x;
  double ycb = c.m_y - b.m_y;
  double x0b = x.m_x - b.m_x;
  double y0b = x.m_y - b.m_y;
  double xcb2 = xcb*xcb;
  double ycb2 = ycb*ycb;

  double cb2 = xcb2 + ycb2;
  double hx = (xcb2 * x.m_x + ycb2 * b.m_y + xcb * ycb * y0b)/cb2;
  double hy = (ycb2 * x.m_y + xcb2 * b.m_x + xcb * ycb * x0b)/cb2;

  bool inside = true;
  if (fabs(xcb) > fabs(ycb) ) {
    inside = ( c.m_x > b.m_x ) ? ( hx > b.m_x && hx < c.m_x ) 
                               : ( hx > c.m_x && hx < b.m_x );
  } else {
    inside = ( c.m_y > b.m_y ) ? ( hy > b.m_y && hy < c.m_y )
                               : ( hy > c.m_y && hy < b.m_y );
  }
  if ( inside ) {
    hx -= x.m_x;
    hy -= x.m_y;
    return hx*hx + hy*hy;
  } // else {
  hx = x.m_x - c.m_x;
  hy = x.m_y - c.m_y;
  double dc = hx*hx + hy*hy;
  double db = x0b*x0b + y0b*y0b;
  return ( dc < db )? dc : db;
}



} // namespace warp

} // namespace therion

void 
therion::warp::point_pair::add_line( line * line )
{
  // thprintf("%s add_line %s-%s\n", 
  //   m_name.c_str(), line->m_p1->m_name.c_str(), line->m_p2->m_name.c_str() );
  if ( line->m_type == THMORPH_STATION ) m_legs ++;
  mLines.push_back( line );
}

therion::warp::line::line( morph_type t, point_pair * p1, point_pair * p2 )
  : m_type( t ) 
  , m_p1( p1 )
  , m_p2( p2 )
{ 
  // thprintf("New line: P1 %s P2 %s\n", p1->m_name.c_str(), p2->m_name.c_str() );
  if ( p1 != NULL ) p1->add_line( this );
  if ( p2 != NULL ) p2->add_line( this );
}

void
therion::warp::point_pair::update( thvec2 & x0, double xunit, thvec2 & u0, double uunit )
{
  z = ( x - x0 ) / xunit;
  w = ( u - u0 ) / uunit;
}

void
therion::warp::point_pair::order_lines( inserter * warper, double x_u, warp_proj proj )
{
  size_t sz = mLines.size();
  // thprintf("order_lines(): Point %s has %d/%d lines\n", m_name.c_str(), sz, m_legs );
  if ( sz <= 1 ) {
    return;
  }

  bool repeat = true;
  while ( repeat ) {
    sz = mLines.size();

    therion::warp::line * l1 = mLines[0];
    thvec2 v1 = ( this == l1->m_p1 ) ? l1->vz : l1->vz * (-1);
    for ( size_t i=1; i<sz-1; ++i) {
      double theta_min = 2*THPI;
      size_t jmin = i;
      for (size_t j=i; j<sz; ++j) {
        therion::warp::line * l2 = mLines[j];
        thvec2 v2 = ( this == l2->m_p1 ) ? l2->vz : l2->vz * (-1);
        double ct = v1 * v2;
        double st = v1 ^ v2;
        double theta = atan2( st, ct );
        if ( theta < 0 ) theta += 2*THPI;
        if ( theta < theta_min ) { jmin = j; theta_min = theta; }
      }
      l1 = mLines[jmin];
      if ( jmin > i ) {
        mLines[jmin] = mLines[i];
        mLines[i] = l1;
      }
      v1 = ( this == l1->m_p1 ) ? l1->vz : l1->vz * (-1);
    }
 
    repeat = false; 
    // now check that between any two STATION lines there is a non-STATION line
    // thprintf("Point %s: %6.2f %6.2f has %d lines\n", m_name.c_str(), x.m_x, x.m_y, sz );
    for ( size_t i=0; i<sz; ++i) {
      size_t j = (i+1)%sz;
      if ( m_legs >= 2 ) {
        therion::warp::line * l1 = mLines[i];
        therion::warp::line * l2 = mLines[j];
        if ( l1->m_type != THMORPH_STATION ) continue;
        if ( l2->m_type != THMORPH_STATION ) continue;
        therion::warp::point_pair * p1 = l1->other_end( this );
        therion::warp::point_pair * p2 = l2->other_end( this );
        // thprintf("point %s (%s - %s) sizes %d %d \n",
        //   m_name.c_str(),  p1->m_name.c_str(), p2->m_name.c_str(), p1->size(), p2->size() );
        if ( p1->size() == 1 && p2->size() == 1 ) continue;

      } else if ( m_legs == 0 ) {
        continue;
      } else { // m_legs == 1
        // angle between line[i] and line[j]
        therion::warp::line * l1 = mLines[i];
        therion::warp::line * l2 = mLines[j];
        therion::warp::point_pair * p1 = l1->other_end( this );
        therion::warp::point_pair * p2 = l2->other_end( this );
        // thprintf("point %s (%s - %s) legs 1 angle %6.2f\n", 
        //   m_name.c_str(), p1->m_name.c_str(), p2->m_name.c_str(), 
        //   (p2->u - u) ^ (p1->u -u ) );
        if ( ((p2->u - u) ^ (p1->u -u )) < 0 )
          continue;
      }
      repeat = true;
      // thprintf("Point %s: insert %d/%d lines (j=%d)\n", m_name.c_str(), i, sz, j );
      therion::warp::line * l1 = mLines[i];
      therion::warp::line * l2 = mLines[j];
      therion::warp::point_pair * p1 = l1->other_end( this ); // ( this == l1->m_p1 ) ? l1->m_p2 : l1->m_p1;
      therion::warp::point_pair * p2 = l2->other_end( this ); // ( this == l2->m_p1 ) ? l2->m_p2 : l2->m_p1;
      thvec2 x1 = p1->x - x;
      thvec2 x2 = p2->x - x;
      thvec2 u1 = p1->u - u;
      thvec2 u2 = p2->u - u;
      // thprintf("must insert between %s %s at %s (m_legs %d) proj %s\n",
      //   p1->m_name.c_str(), p2->m_name.c_str(), m_name.c_str(), m_legs,
      //   (proj == THWARP_PLAN)? "plan" : "elev" );
      // try a non-STATION line that reflected is between l1 and l2
      size_t k = 0;
      if ( m_legs <= 2 ) {
        for ( ; k<sz; ++k) {
          if ( mLines[k]->m_type == THMORPH_STATION ) continue;
          therion::warp::point_pair * p0 = mLines[k]->other_end( this );
          thvec2 x0 = x - p0->x;
          thvec2 u0 = u - p0->u;
          if ( (x2 ^ x0) < 0 && (x0 ^ x1) < 0 && (u2 ^ u0) < 0 && (u0 ^ u1) ) {
            // opposite is OK both for PLAN and for EXTENDED proj
            // thprintf("adding extra as opposite of %s\n", p0->m_name.c_str() );
            thvec2 x3 = x + x0;
            thvec2 u3 = u + u0;
            warper->add_extra_line( this, i, x3, u3 );
            break;
          }
        }
      } else { 
        k = sz; // still need to insert
      }

      if ( k == sz ) {
        thvec2 x3, u3;
        if ( proj == THWARP_PLAN ) {
	  // this is a wild guess. there is no generic way to guess
	  // where the tranverse point(s) are ...
          double d1 = u1.length(); 
          double d2 = u2.length(); 
          double du = (u1 - u2).length();
          thvec2 vu = u2/d2 - u1/d1;
	  vu.normalize();
	  // thprintf("adding extra as bisector at %s [lengths %.2f %.2f %.2f]\n",
	  //   this->m_name.c_str(), du, d1, d2 );
          if ( d1 < du ) du = d1;
          if ( d2 < du ) du = d2;
          u3.m_x = u.m_x + vu.m_y * du;
          u3.m_y = u.m_y - vu.m_x * du;

          thvec2 vx = x2/x2.length() - x1/x1.length();
	  vx.normalize();
          /*
          double dx = (x1 - x2).length() / 2.0;
          double d1 = x1.length()/2.0; 
          double d2 = x2.length()/2.0; 
          if ( d1 < dx ) dx = d1;
          if ( d2 < dx ) dx = d2;
          */
          double dx = du * x_u;
          x3.m_x = x.m_x + vx.m_y * dx;
          x3.m_y = x.m_y - vx.m_x * dx;
        } else /* if ( proj == THWARP_EXTENDED ) */ {
          #define TH_VERT_ALPHA 32.0
          double v1 = compute_vertical( TH_VERT_ALPHA, x1, u1 );
          double v2 = compute_vertical( TH_VERT_ALPHA, x2, u2 );
          thvec2 vu = u2/u2.length() - u1/u1.length();
          double du = (u1 - u2).length();
          double d1 = u1.length();
          double d2 = u2.length();
          if ( d1 < du ) du = d1;
          if ( d2 < du ) du = d2;
          double dx = du * x_u;
          // thprintf("v1 %f v2 %f \n", v1, v2 );
          if ( fabs(v1) < 0.5 || fabs(v2) < 0.5 ) {
            if ( fabs(v1) < 0.1 && fabs(v2) < 0.1 ) {
              if ( v1 * v2 < 0.0 ) { // vertical opposite
                // thprintf("horizontal\n");
                u3.m_x = u.m_x + ((v2>0.0)? 1.0 : -1.0) * du;
                u3.m_y = u.m_y;
                x3.m_x = x.m_x + ((v2>0.0)? 1.0 : -1.0) * dx;
                x3.m_y = x.m_y;
              } else {
                thprintf("warning: closed up vertical angle\n");
                u3.m_x = u.m_x;
                u3.m_y = (u1.m_y + u2.m_y)/2.0;
                x3.m_x = x.m_x;
                x3.m_y = (x1.m_y + x2.m_y)/2.0;
              }
            } else {
              // thprintf("bisector\n");
              u3.m_x = u.m_x + vu.m_y * du;
              u3.m_y = u.m_y - vu.m_x * du;
              thvec2 vx = x2/x2.length() - x1/x1.length();
              x3.m_x = x.m_x + vx.m_y * dx;
              x3.m_y = x.m_y - vx.m_x * dx;
            }
          } else {
            // thprintf("vertical\n");
            u3.m_x = u.m_x;
            u3.m_y = u.m_y + ((v2>0.0)? 1.0 : -1.0) * du;
            x3.m_x = x.m_x;
            x3.m_y = x.m_y + ((v2>0.0)? 1.0 : -1.0) * dx;
          }
        }
        warper->add_extra_line( this, i, x3, u3 );
      }
    } // for ( ...; i<sz; ...)
  } // while ( repeat )

  sz = mLines.size();
  /*
  thprintf("Point %s: %6.2f %6.2f has %d/%d lines: ", m_name.c_str(), x.m_x, x.m_y, sz, m_legs );
  for ( size_t i=0; i<sz; ++i) {
    point_pair * p2 = mLines[i]->other_end( this );
    thprintf("%s ", p2->m_name.c_str() );
  }
  thprintf("\n");
  */

  // check that all the lines from this point that are not centerlines have a "single"
  // node at the other end
  // this check could be restricted to 
  //    if ( m_type == THMORPH_STATION )
  for ( size_t i=0; i<sz; ++i) {
    therion::warp::line * l2 = mLines[i];
    if ( l2->m_type != THMORPH_STATION ) {
      assert( l2->other_end(this)->mLines.size() == 1 );
    }
  }
}


therion::warp::line * 
therion::warp::point_pair::first_leg()
{
  size_t sz = mLines.size();
  for (size_t j=0; j<sz; ++j) {
    if ( mLines[j]->m_type == THMORPH_STATION ) return mLines[j];
  }
  return NULL;
}


void
therion::warp::line::update()
{
  // thprintf("Update line %s %s\n", m_p1->m_name.c_str(), m_p2->m_name.c_str() );
  vz = m_p2->z - m_p1->z;
  vw = m_p2->w - m_p1->w;

  z.m_a =   vz.m_y;
  z.m_b = - vz.m_x;
  z.m_c = - ( z.m_a * m_p1->z.m_x + z.m_b * m_p1->z.m_y);
  zab = sqrt(z.m_a*z.m_a + z.m_b*z.m_b);

  w.m_a =   vw.m_y;
  w.m_b = - vw.m_x;
  w.m_c = - ( w.m_a * m_p1->w.m_x + w.m_b * m_p1->w.m_y);
  wab = sqrt(w.m_a*w.m_a + w.m_b*w.m_b);

  double dz = vz.length2();
  // R.m_xx = R.m_yy = ( vw * vz ) / dz;
  // R.m_xy = ( vw ^ vz ) / dz;
  // R.m_yx = - R.m_xy;

  double dw = vw.length2();
  // S.m_xx = S.m_yy = ( vz * vw ) / dw;
  // S.m_xy = ( vz ^ vw ) / dw;
  // S.m_yx = - S.m_xy;

  dz = sqrt( dz );
  dw = sqrt( dw );
  vz /= dz;
  vw /= dw;
  assert( fabs( vz.length() - 1.0 ) < 0.01 );
  assert( fabs( vw.length() - 1.0 ) < 0.01 );
  z_w = dz / dw;
  w_z = dw / dz;
}

// ---------------------------------------------------------------
// therion::warp::triangle
//
// TOCHECK from .h

double
therion::warp::triangle::sm_map( const thvec2 & p, thvec2 & ret ) const
{
  ret.m_x = p.m_x - m_A.m_x;
  ret.m_y = p.m_y - m_A.m_y;
  double d2 = ret.length2();
  return d2*d2;
}

void
therion::warp::triangle::inv_sm_map( const thvec2 & p, thvec2 & ret, double d4 ) const
{
  ret.m_x = d4 * ( m_A.m_x + p.m_x );
  ret.m_y = d4 * ( m_A.m_y + p.m_y );
}

// ---------------------------------------------------------------
// therion::warp::plaquette

void therion::warp::plaquette::init()
{
  m_AD = m_D - m_A;
  m_BC = m_C - m_B;
  m_AB = m_B - m_A;
  m_BA = m_A - m_B;

  m_AB_len = m_AB.length();   // length of AB (was m_abd)
  m_AD_len = m_AD.length();   // length of AD
  m_BC_len = m_BC.length();   // length of BC

  m_adn = m_AD;  m_adn.normalize();
  m_bcn = m_BC;  m_bcn.normalize();
  m_abn = m_AB;  m_abn.normalize();
  m_abh.m_x =   m_abn.m_y;
  m_abh.m_y = - m_abn.m_x;

  m_a =   m_A.m_x - m_B.m_x - m_D.m_x  + m_C.m_x;
  m_b = - m_A.m_x + m_B.m_x;
  m_c = - m_A.m_x           + m_D.m_x;
  m_d =   m_A.m_x;
  m_e =   m_A.m_y - m_B.m_y - m_D.m_y  + m_C.m_y;
  m_f = - m_A.m_y + m_B.m_y;
  m_g = - m_A.m_y           + m_D.m_y;
  m_h =   m_A.m_y;

  m_A0 = m_a * m_f - m_b * m_e;
  m_B0 = m_a * m_h - m_d * m_e + m_c * m_f - m_b * m_g;
  m_C0 = m_c * m_h - m_d * m_g;
  m_D0 = m_a * m_g - m_c * m_e;
  m_E0 = m_a * m_h - m_d * m_e - m_c * m_f + m_b * m_g;
  m_F0 = m_b * m_h - m_d * m_f;

  // angles + "line"
  // thvec2 adn = m_AD; adn.normalize();
  // m_theta_l = acos( m_abn * adn );
  m_theta_l = angle( m_abn, m_adn );
  // thvec2 bcn = m_BC; bcn.normalize();
  // m_theta_r = acos( - m_abn * bcn );
  m_theta_r = angle( m_bcn, - m_abn );
  
  // thprintf("Plaquette theta L %.2f R %.2f \n", m_theta_l, m_theta_r );
  
  // C1 is the projection (parallel to AB) of C on AD.
  // [1] line thru C parallel to AB:
  //      y = C.y + ( x - C.x ) * AB.y / AB.x
  // [2] intersection with 
  //      y = A.y + ( x - A.x ) * AD.y / AD.x
  //
  //      x ( AB.y AD.x - AB.x AD.y ) = AD.x AB.x ( A.y - C.y ) + AD.x AB.y C.x - AB.x AD.y A.x
  // i.e.
  //      - adab * x = AB.x ( AD.x A.y - AD.y A.x ) - AD.x ( AB.x C.y - AB.y C.x )
  //      adab * x = AB.x * ( A ^ AD ) - AD.x * ( C ^ AB )

  m_adab = - m_adn.m_x * m_abn.m_y + m_adn.m_y * m_abn.m_x; // abn ^ adn
  m_bcab = - m_bcn.m_x * m_abn.m_y + m_bcn.m_y * m_abn.m_x; // abn ^ bcn
  m_adA = - m_adn.m_x * m_A.m_y + m_adn.m_y * m_A.m_x;      // A ^ adn
  m_bcB = - m_bcn.m_x * m_B.m_y + m_bcn.m_y * m_B.m_x;      // B ^ bcn

  double cab = m_C.m_x * m_abn.m_y - m_C.m_y * m_abn.m_x;   // C ^ abn
  m_C1 = ( m_abn * m_adA - m_adn * cab ) / m_adab;

  // check align A-D-C1
  assert( fabs( m_A.m_x * m_D.m_y - m_A.m_y * m_D.m_x
              + m_D.m_x * m_C1.m_y - m_D.m_y * m_C1.m_x
              + m_C1.m_x * m_A.m_y - m_C1.m_y * m_A.m_x ) < 0.001 );

  m_C1C = m_C - m_C1;
  // assert m_C1C || m_AB
  assert( fabs( m_C1C.m_x * m_AB.m_y - m_C1C.m_y * m_AB.m_x ) < 0.001 );

  m_sin = m_AB.m_x / m_AB_len;
  m_cos = m_AB.m_y / m_AB_len;
  if ( fabs(m_AB.m_x) * 1.73 > fabs(m_AB.m_y) ) { // horizontal up to 60 deg.
    if ( fabs(m_AB.m_x) * 1.19 > fabs(m_AB.m_y) ) { // really horizontal
      m_sin = 0.0;
      m_cos = 1.0;
    } 
  } else {
    if ( fabs(m_AB.m_y) * 0.58 > fabs(m_AB.m_x) ) {
      m_sin = 1.0;
      m_cos = 0.0;
    }
  }
  m_tan = m_sin/m_cos;
  m_ctg = m_cos/m_sin;
}

double
therion::warp::plaquette::s_map_straight( const thvec2 & p ) const
{
  double s;

  double ex_ay = m_e * p.m_x - m_a * p.m_y;
  double A = m_A0;
  double B = (ex_ay + m_B0)/2.0;
  double C = m_g * p.m_x - m_c * p.m_y + m_C0;
  double det = B*B - A * C;
  if ( det < 0.0 )
    return thnan;
  if ( det > 0.0 ) det = sqrt( det );
  if ( fabs(A) > 1.e-6 ) {
    s = (- B + det )/A;
    double s2 = (-B-det)/A;
    if ( s2 > 0.0 && fabs(s2) < fabs(s) ) s = s2;
  } else {
    s = ( B == 0.0 ) ? thinf : - C / (2*B);
  }
  return s;
}

double
therion::warp::plaquette::s_map_slant( const thvec2 & p ) const
{
  double x0 = p.m_x;
  double y0 = p.m_y;
  double pab = p.m_x * m_abn.m_y - p.m_y * m_abn.m_x;
  // P1 = [ (ad^A) ab + (P^ab) ad ] / [ad^ab]
  // P2 = [ (bc^B) ab + (P^ab) bc ] / [ad^bc]
  double x1 = ( m_abn.m_x * m_adA - m_adn.m_x * pab ) / m_adab;
  double y1 = ( m_abn.m_y * m_adA - m_adn.m_y * pab ) / m_adab;
  double x2 = ( m_abn.m_x * m_bcB - m_bcn.m_x * pab ) / m_bcab;
  double y2 = ( m_abn.m_y * m_bcB - m_bcn.m_y * pab ) / m_bcab;
  // check alignment
  // double align = (x1*y2 - x2*y1) + (x2*y0 - x0*y2) + (x0*y1 - x1*y0);
  // assert( fabs(align) < 0.001 );

  double d12 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  double d10 = (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1);
  return sqrt( d10/d12 );
}

double
therion::warp::plaquette::t_map( const thvec2 & p ) const
{
  double t;

  double ex_ay = m_e * p.m_x - m_a * p.m_y;
  // double A = m_A0;
  double D = m_D0;
  // double B = (ex_ay + m_B0)/2.0;
  double E = (ex_ay + m_E0)/2.0;
  // double C = m_g * p.m_x - m_c * p.m_y + m_C0;
  double F = m_f * p.m_x - m_b * p.m_y + m_F0;
  double det = E*E - D * F;
  if ( det < 0.0 )
    return thnan;
  if ( det > 0.0 ) det = sqrt( det );
  if ( fabs(D) > 1.e-6 ) {
    t = (- E + det )/D;
    double t2 = (-E-det)/D;
    if ( t2 > 0.0 && fabs(t2) < fabs(t) ) t = t2;
  } else {
    t = ( E == 0.0 ) ? thinf : - F / (2*E);
  }
  return t;
}

// --------------------------------------------------------------
// Horizontal + Vertical stuff
//
// X:horizontal - Y:vertical
void
therion::warp::plaquette::hv_map( const thvec2 & p, thvec2 & ret ) const
{
  ret.m_x = ( p.m_x - m_A.m_x )/m_AB.m_x;
  // Qy = Ay + (By-Ay)/(Bx-Ax) * (Px - Ax)
  double yq = m_A.m_y + m_AB.m_y * ret.m_x;
  ret.m_y = p.m_y - yq;
}

void
therion::warp::plaquette::inv_hv_map( const thvec2 & p, thvec2 & ret ) const
{
  ret.m_x = m_A.m_x + p.m_x * m_AB.m_x;
  double yq = m_A.m_y + p.m_x * m_AB.m_y;
  ret.m_y = yq + p.m_y;
}

// X:vertical - Y:horizontal
void
therion::warp::plaquette::vh_map( const thvec2 & p, thvec2 & ret ) const
{
  ret.m_x = ( p.m_y - m_A.m_y )/m_AB.m_y;
  // Qx = Ax + (Bx-Ax)/(By-Ay) * (Py - Ay)
  double xq = m_A.m_x + m_AB.m_x * ret.m_x;
  ret.m_y = p.m_x - xq;
}

void
therion::warp::plaquette::inv_vh_map( const thvec2 & p, thvec2 & ret ) const
{
  ret.m_y = m_A.m_y + p.m_x * m_AB.m_y;
  double xq = m_A.m_x + p.m_x * m_AB.m_x;
  ret.m_x = xq + p.m_y;
}

// horizontal-vertical at an angle (m1)
//   X = Px + m1 ( Y - Py )
//
// X = Qx + t * cos 
// Y = Qy + t * sin 
//
// t = ( Y - Qy ) / sin
// X = Qx + cos/sin * ( Y - Qy )
//   = Px + cos/sin * ( Y - Py )
// 
void
therion::warp::plaquette::vhm_map( const thvec2 & p, thvec2 & ret ) const
{ 
  double yq = ( m_AB.m_x * m_A.m_y + (p.m_x - m_A.m_x - m_ctg * p.m_y ) * m_AB.m_y ) 
            / ( m_AB.m_x - m_ctg * m_AB.m_y );
  ret.m_x = ( yq - m_A.m_y )/m_AB.m_y;
  double xq = m_A.m_x + ret.m_x * m_AB.m_x;
  double dx = p.m_x - xq;
  double dy = p.m_y - yq;
  ret.m_y = sqrt( dx*dx + dy*dy );
} 

void
therion::warp::plaquette::inv_vhm_map( const thvec2 & p, thvec2 & ret ) const
{ 
  double xq = m_A.m_x + p.m_x * m_AB.m_x;
  double yq = m_A.m_y + p.m_x * m_AB.m_y;
  ret.m_x = xq + m_cos * p.m_y;
  ret.m_y = yq + m_sin * p.m_y;
}

// Y = Py + m ( X - Px )
void
therion::warp::plaquette::hvm_map( const thvec2 & p, thvec2 & ret ) const
{ 
  double xq = ( m_AB.m_y * m_A.m_x + (p.m_y - m_A.m_y - m_tan * p.m_x ) * m_AB.m_x ) 
            / ( m_AB.m_y - m_tan * m_AB.m_x );
  ret.m_x = ( xq - m_A.m_x )/m_AB.m_x;
  double yq = m_A.m_y + ret.m_x * m_AB.m_y;
  double dx = p.m_x - xq;
  double dy = p.m_y - yq;
  ret.m_y = sqrt( dx*dx + dy*dy );
} 

void
therion::warp::plaquette::inv_hvm_map( const thvec2 & p, thvec2 & ret ) const
{ 
  double xq = m_A.m_x + p.m_x * m_AB.m_x;
  double yq = m_A.m_y + p.m_x * m_AB.m_y;
  ret.m_x = xq + m_cos * p.m_y;
  ret.m_y = yq + m_sin * p.m_y;
}

// --------------------------------------------------------------



/** solve the system
 *    | A+t(D-A)   B+t(C-B)   P  | = 0
 */
void
therion::warp::plaquette::st_map( const thvec2 & p, thvec2 & ret ) const
{
  double s;

  double ex_ay = m_e * p.m_x - m_a * p.m_y;
  double A = m_A0;
  double B = (ex_ay + m_B0)/2.0;
  double C = m_g * p.m_x - m_c * p.m_y + m_C0;
  if ( fabs(A) > 1.e-6 ) {
    double det = B*B - A * C;
    if ( det < 0.0 ) {
      ret.m_x = ret.m_y = thnan;
      return;
    }
    if ( det > 0.0 ) det = sqrt( det );
    s = (- B + det )/A;
    double s2 = (-B-det)/A;
    if ( s2 > 0.0 && fabs(s2) < fabs(s) ) s = s2;
  } else {
    s = ( B == 0.0 ) ? thinf : - C / (2*B);
  }
  ret.m_x = s;
  ret.m_y = ( p.m_x - m_b * s - m_d ) / ( m_a * s + m_c );
}

void
therion::warp::plaquette::inv_st_map( const thvec2 & v, thvec2 & ret ) const
{
  ret.m_x = m_a * v.m_x * v.m_y + m_b * v.m_x + m_c * v.m_y + m_d;
  ret.m_y = m_e * v.m_x * v.m_y + m_f * v.m_x + m_g * v.m_y + m_h;
}

// this are Beier-Neely type maps
// "Feature-based image metamorphosis" Computer Graphics, 26 35-42, 1992
//
// TODO 
// Taking into account different end-size ratios can be done here
// or by the caller. The plaquette does not have the ratios, 
// and the relative weight could be different from the return 
// X coordinate (ret.m_x)

// straight BN map
void
therion::warp::plaquette::bn_map_straight( const thvec2 & p, thvec2 & ret ) const
{
  thvec2 ap( p.m_x - m_A.m_x, p.m_y - m_A.m_y );
  ret.m_x = ( ap * m_abn ) / m_AB_len;
  ret.m_y = ap * m_abh;
}

void 
therion::warp::plaquette::inv_bn_map_straight( const thvec2 & p, thvec2 & ret ) const
{
  // ret = m_A + m_AB * p.m_x + m_abh * p.m_y;
  ret.m_x = m_A.m_x + m_AB.m_x * p.m_x + m_abh.m_x * p.m_y;
  ret.m_y = m_A.m_y + m_AB.m_y * p.m_x + m_abh.m_y * p.m_y;
}

  
// slanted BN map
void
therion::warp::plaquette::bn_map_slant( const thvec2 & p, thvec2 & ret ) const
{
  double x0 = p.m_x;
  double y0 = p.m_y;
  double pab = p.m_x * m_abn.m_y - p.m_y * m_abn.m_x;  // P ^ ab
  // P1 = [ (A^ad) ab - (P^ab) ad ] / [ab^ad]  projection of P parallel to AB on AD
  // P2 = [ (B^bc) ab - (P^ab) bc ] / [ab^bc]  projection of P parallel to AB on BC
  double x1 = ( m_abn.m_x * m_adA - m_adn.m_x * pab ) / m_adab;
  double y1 = ( m_abn.m_y * m_adA - m_adn.m_y * pab ) / m_adab;
  double x2 = ( m_abn.m_x * m_bcB - m_bcn.m_x * pab ) / m_bcab;
  double y2 = ( m_abn.m_y * m_bcB - m_bcn.m_y * pab ) / m_bcab;
  // check alignment
  // double align = (x1*y2 - x2*y1) + (x2*y0 - x0*y2) + (x0*y1 - x1*y0);
  // assert( fabs(align) < 0.001 );

  double d12 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  double d10 = (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1);
  ret.m_x = sqrt( d10/d12 );

  // slant distance
  // x1 = m_A.m_x + ret.m_x * m_AB.m_x;
  // y1 = m_A.m_y + ret.m_x * m_AB.m_y;
  // ret.m_y = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) );
  // 
  // straight distance
  ret.m_y = (p - m_A) * m_abh;
}

void 
therion::warp::plaquette::inv_bn_map_slant( const thvec2 & p, thvec2 & ret ) const
{
  // slanted BN map
  /*
  thvec2 ps = m_A + p.m_x * m_AB;
  thvec2 p1 = m_C1 + p.m_x * m_C1C;
  double x = p1.m_x - ps.m_x;
  double y = p1.m_y - ps.m_y;
  ret = ps + (p1-ps)*(p.m_y /sqrt(x*x+y*y) );
  */
  double d1 = fabs(p.m_y/m_adab);
  double d2 = fabs(p.m_y/m_bcab);
  double x1 = 1.0 - p.m_x;
  ret.m_x = x1 * ( m_A.m_x + d1 * m_adn.m_x ) + p.m_x * ( m_B.m_x + d2 * m_bcn.m_x );
  ret.m_y = x1 * ( m_A.m_y + d1 * m_adn.m_y ) + p.m_x * ( m_B.m_y + d2 * m_bcn.m_y );
}


// TODO fix this
double
therion::warp::plaquette::sm_map( const thvec2 & p, thvec2 & ret ) const
{
  thvec2 ap( p.m_x - m_A.m_x, p.m_y - m_A.m_y );
  ret.m_x = (ap * m_abn) / m_AB_len;
  ret.m_y = ap * m_abh;
  double d2;
  if ( ret.m_x < 0.0 ) { // left point
    d2 = ap.length2();
  } else if ( ret.m_x > 1.0 ) { // right point
    thvec2 bp( p.m_x - m_B.m_x, p.m_y - m_B.m_y );
    d2 = bp.length2();
  } else { // middle point
    d2 = ret.m_y * ret.m_y;
  }
  return d2*d2;
}

void
therion::warp::plaquette::inv_sm_map( const thvec2 & p, thvec2 & ret, double d4 ) const
{
  ret.m_x = d4 * ( m_A.m_x + p.m_x * m_AB.m_x + p.m_y * m_abh.m_x );
  ret.m_y = d4 * ( m_A.m_y + p.m_x * m_AB.m_y + p.m_y * m_abh.m_y );
}


// ****************************************************************
// TRIANGLE PAIRS
//

namespace therion 
{
  namespace warp 
  {
    /** cstr
     * @param a     A-corner point pair ("vertex" of the triangle)
     * @param b     B-corner point pair (right side)
     * @param c     C-corner point pair (left side)
     * @param bound bound on "a" for is_inside test
     * @param cubic  whether to use the cubic radial correction
     * @param border inner border
     */
    template< > 
    item_pair<triangle>::item_pair( point_pair * a, point_pair * b, point_pair * c
                     , double bound /* = BOUND_TRIANGLE */
		     )
        : basic_pair( bound )
        , from( a->z, b->z, c->z )
        , to  ( a->w, b->w, c->w )
    {
      m_pair[0] = c;
      m_pair[1] = a;
      m_pair[2] = b;

      m_kl = ( from.theta_left() - to.theta_left() ) * MORPH_ANGLE_FACTOR;
      m_kr = ( from.theta_right() - to.theta_right() ) * MORPH_ANGLE_FACTOR;
      // m_kl = fabs(from.theta_left()-M_PI_2) - fabs(to.theta_left()-M_PI_2);
      // m_kr = fabs(from.theta_right()-M_PI_2) - fabs(to.theta_right()-M_PI_2);
      
      m_dl = from.m_ac / to.m_ac;
      m_dr = from.m_ab / to.m_ab;

      // thprintf("Triangle [%d] Theta L %.2f R %.2f Ratios L %.2f R %.2f \n", 
      //   nr(), m_kl, m_kr, m_dl, m_dr );
    } 
    
    /** type of this warping basic_pair
     * @return the number of points of the basic_pair
     */ 
    template< > 
    warp_type item_pair<triangle>::type() const { return THWARP_TRIANGLE; }
    
    /** maximum number of neighbors
     * @return the maximum number of neighbors
     */
    template< >
    int item_pair<triangle>::ngbh_nr() const { return 2; }

    template< >
    void item_pair<triangle>::set_projection( warp_proj /*proj*/ )
    {
      // thprintf("item_pair<triangle>::set_projection(%d)\n", proj );
      // TODO
    }


    template< >
    void item_pair<triangle>::backward_normal_bd( const thvec2 & p, thvec2 & ret ) const
    {
      static thvec2 v0, r1, pa;
      to.bn_map( p, v0 );
      v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
      from.inv_bn_map( v0, ret );

      if ( v0.m_x < MORPH_BD ) {
	    double x = v0.m_x/MORPH_BD;             // x  = 0 .. 1
	    // double x1 = 1.0 - 2*x + x*x;
	    double x1 = 1 - x;
	    /*
            #if MORPH_ROTATE != 0
              double t = left_ngbh()->m_kr * x1;
              left_ngbh()->pto()->right_rotate( p, pa, cos(t), -sin(t) );
	      left_ngbh()->pto()->bn_map( pa, v0 );
            #else
	      left_ngbh()->pto()->bn_map( p, v0 );
            #endif
	    */
	    left_ngbh()->right_rotate_to( p, v0, x1 );

	    // double s = left_ngbh()->pto()->s_map( pa );
	    // v0.m_y *= s * left_ngbh()->m_dr + (1.0-s) * left_ngbh()->m_dl;
	    v0.m_y *= v0.m_x * left_ngbh()->m_dr + (1.0-v0.m_x) * left_ngbh()->m_dl;
	    left_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	    // ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	    ret.m_x = ( ret.m_x + x1 * r1.m_x ) / ( 1 + x1 );
	    ret.m_y = ( ret.m_y + x1 * r1.m_y ) / ( 1 + x1 );
      } else if ( 1.0 - v0.m_x < MORPH_BD ) {
	    double x = (1.0-v0.m_x)/MORPH_BD;
	    // double x1 = 1.0 - 2*x + x*x;
	    double x1 = 1 - x;
	    /*
            #if MORPH_ROTATE != 0
	      double t = right_ngbh()->m_kl * x1;
	      right_ngbh()->pto()->left_rotate( p, pa, cos(t), -sin(t) );
	      right_ngbh()->pto()->bn_map( pa, v0 );
            #else
	      right_ngbh()->pto()->bn_map( p, v0 );
            #endif
	    */
	    right_ngbh()->left_rotate_to( p, v0, x1 );

	    // s = right_ngbh()->pto()->s_map( pa );
	    // v0.m_y *= s * right_ngbh()->m_dr + (1.0-s) * right_ngbh()->m_dl;
	    v0.m_y *= v0.m_x * right_ngbh()->m_dr + (1.0-v0.m_x) * right_ngbh()->m_dl;
	    right_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	    // ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	    ret.m_x = ( ret.m_x + x1 * r1.m_x ) / ( 1 + x1 );
	    ret.m_y = ( ret.m_y + x1 * r1.m_y ) / ( 1 + x1 );
      } else {
	    /* nothing */
      }
    }

    /*
     * as in therion
    template< >
    void item_pair<triangle>::forward( const thvec2 & p, thvec2 & ret ) const
    {
      static thvec2 v0;
      from.bn_map( p, v0 );
      v0.m_y /= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
      to.inv_bn_map( v0, ret );
    }
     */

    #ifdef DEBUG
    /** debug: print
     */
    template< >
    void item_pair<triangle>::print() const
    {
        thprintf("[%d] Triangle A %6.2f %6.2f <-> %6.2f %6.2f\n",
          nr(), from.m_A.m_x, from.m_A.m_y, to.m_A.m_x, to.m_A.m_y );
        thprintf("              B %6.2f %6.2f <-> %6.2f %6.2f\n",
          from.m_B.m_x, from.m_B.m_y, to.m_B.m_x, to.m_B.m_y );
        thprintf("              C %6.2f %6.2f <-> %6.2f %6.2f\n",
          from.m_C.m_x, from.m_C.m_y, to.m_C.m_x, to.m_C.m_y );
        print_ngbhs();
    }
    #endif

// ****************************************************************
// PLAQUETTE PAIRS
// 

    // TODO FIXME 
    template< >
    void item_pair<plaquette>::set_projection( warp_proj /*proj*/ )
    {
      // thprintf("item_pair<plaquette>::set_projection(%d)\n", proj );
      #ifdef MORPH_EXPERIMENTAL
      if ( proj == THWARP_EXTENDED ) {
        // TODO
	if ( fabs(from.m_sin) > fabs(from.m_cos) ) { // horizontal
	  if ( fabs(from.m_cos) > 0.0 ) {
            from.bn_map_impl = &therion::warp::plaquette::hvm_map;
            from.inv_bn_map_impl = &therion::warp::plaquette::inv_hvm_map;
            to.bn_map_impl = &therion::warp::plaquette::hvm_map;
            to.inv_bn_map_impl = &therion::warp::plaquette::inv_hvm_map;
	  } else {
            from.bn_map_impl = &therion::warp::plaquette::hv_map;
            from.inv_bn_map_impl = &therion::warp::plaquette::inv_hv_map;
            to.bn_map_impl = &therion::warp::plaquette::hv_map;
            to.inv_bn_map_impl = &therion::warp::plaquette::inv_hv_map;
          }
	} else {
	  if ( fabs(from.m_sin) > 0.0 ) {
            from.bn_map_impl = &therion::warp::plaquette::vhm_map;
            from.inv_bn_map_impl = &therion::warp::plaquette::inv_vhm_map;
            to.bn_map_impl = &therion::warp::plaquette::vhm_map;
            to.inv_bn_map_impl = &therion::warp::plaquette::inv_vhm_map;
	  } else {
            from.bn_map_impl = &therion::warp::plaquette::vh_map;
            from.inv_bn_map_impl = &therion::warp::plaquette::inv_vh_map;
            to.bn_map_impl = &therion::warp::plaquette::vh_map;
            to.inv_bn_map_impl = &therion::warp::plaquette::inv_vh_map;
          }
	}
      } else 
      #endif
      {
        if ( m_kl + m_kr < 0 ) 
        {
          from.s_map_impl = &therion::warp::plaquette::s_map_slant;
          from.bn_map_impl = &therion::warp::plaquette::bn_map_slant;
          from.inv_bn_map_impl = &therion::warp::plaquette::inv_bn_map_slant;
        } else if ( false ) {
          to.s_map_impl = &therion::warp::plaquette::s_map_slant;
          to.bn_map_impl = &therion::warp::plaquette::bn_map_slant;
          to.inv_bn_map_impl = &therion::warp::plaquette::inv_bn_map_slant;
        }
      }
    }

    /** cstr
     * @param a    A-corner point pair
     * @param b    B-corner point pair
     * @param c    C-corner point pair
     * @param d    D-corner point pair
     * @param bound bound of "t" for the is_inside test
     * @param cubic  whether to use the cubic correction
     * @param border inner border 
     */
    template< >
    item_pair<plaquette>::item_pair( 
          point_pair * a, point_pair * b, point_pair * c, point_pair * d
          , double bound /* = BOUND_PLAQUETTE */
	)
      : basic_pair( bound )
      , from( a->z, b->z, c->z, d->z )
      , to  ( a->w, b->w, c->w, d->w )
    {
      m_pair[0] = d;
      m_pair[1] = a;
      m_pair[2] = b;
      m_pair[3] = c;

      m_kl = ( from.theta_left() - to.theta_left() ) * MORPH_ANGLE_FACTOR;
      m_kr = ( from.theta_right() - to.theta_right() ) * MORPH_ANGLE_FACTOR;
      // m_kl = fabs(from.theta_left()-M_PI_2) - fabs(to.theta_left()-M_PI_2);
      // m_kr = fabs(from.theta_right()-M_PI_2) - fabs(to.theta_right()-M_PI_2);
      
      m_dl = from.m_AD_len / to.m_AD_len;
      m_dr = from.m_BC_len / to.m_BC_len;
      // thprintf("Plaquette [%d] Theta L %.2f R %.2f Ratios L %.2f R %.2f \n", 
      //   nr(), m_kl, m_kr, m_dl, m_dr );
      //

    }
    
    /** warp type
     * @return the warp type of the warping basic_pair
     */
    template< >
    warp_type item_pair<plaquette>::type() const { return THWARP_PLAQUETTE; }
    
    /** maximum number of neighbors
     * @return the maximum number of neighbors
     */
    template< >
    int item_pair<plaquette>::ngbh_nr() const { return 3; }
    

    template< >
    void item_pair<plaquette>::backward_normal_bd( const thvec2 & p, thvec2 & ret ) const
    {
      static thvec2 v0, r1, pa;
      double s = to.s_map( p );
      // assert( s >= 0 && s <= 1.0 );
      if ( s < MORPH_BD ) {
	double x = s/MORPH_BD;             // x  = 0 .. 1
	double x1 = 1.0 - 2*x + x*x;     // x1 = 1 .. 0
	this->left_rotate_to( p, v0, x1 );

	// v0.m_y *= s * m_dr + (1.0-s) * m_dl;
	v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	from.inv_bn_map( v0, ret );
       
	left_ngbh()->right_rotate_to( p, v0, x1 );

	s = v0.m_x;
	v0.m_y *= s * left_ngbh()->m_dr + (1.0-s) * left_ngbh()->m_dl;
	left_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	// ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	ret.m_x = ( (2-x1)* ret.m_x + x1 * r1.m_x ) / 2; // ( 1 + x1 );
	ret.m_y = ( (2-x1)* ret.m_y + x1 * r1.m_y ) / 2; // ( 1 + x1 );
      } else if ( 1.0 - s < MORPH_BD ) {
	double x = (1.0-s)/MORPH_BD;
	// double x1 = 1.0 - x;
	double x1 = 1.0 - 2*x + x*x;
	this->right_rotate_to( p, v0, x1 );

	// v0.m_y *= s * m_dr + (1.0-s) * m_dl;
	v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	from.inv_bn_map( v0, ret );

	right_ngbh()->left_rotate_to( p, v0, x1 );
	s = v0.m_x;
	v0.m_y *= s * right_ngbh()->m_dr + (1.0-s) * right_ngbh()->m_dl;
	right_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	// ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	ret.m_x = ( (2-x1)* ret.m_x + x1 * r1.m_x ) / 2; // ( 1 + x1 );
	ret.m_y = ( (2-x1)* ret.m_y + x1 * r1.m_y ) / 2; // ( 1 + x1 );
      } else {
	to.bn_map( p, v0 );
	// v0.m_y *= s * m_dr + (1.0-s) * m_dl;
	v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	from.inv_bn_map( v0, ret );
	// to.st_map( p, v0 );
	// from.inv_st_map( v0, ret );
      }
    }

    /** map p in the "from" image to a point in the "to" img
     * @param p   2D point in the "from" image 
     * @param ret corresponding 2D point in the "to" image
     *
     * The backward() map is not invertibel in close form, therefore
     * the forward() map is carried out by guessing s first point and
     * searching around it for a point that satisfies the backward() map.
     * This way forward() takes 30-40 times longer than backward().
     *
     * The forward() code must come after backward() has been instantiated.
     */
    template< typename T >
    void item_pair<T>::forward( const thvec2 & p, thvec2 & ret ) const
    {
	  static thvec2 v0, r1; 
	  thvec2 p1;
	  double d, d0, x, y;
	  double dx = 0.1, dy=0.1;
	  from.bn_map( p, v0 );
	  // v0.m_y *= s * m_dr + (1.0-s) * m_dl;
	  v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	  to.inv_bn_map( v0, r1 );
	  this->backward( r1, p1 );
	  x = p.m_x - p1.m_x;
	  y = p.m_y - p1.m_y;
          d0 = x*x + y*y;
	  do { 
	    r1.m_x += dx;
	    this->backward( r1, p1 );
	    x = p.m_x - p1.m_x;
	    y = p.m_y - p1.m_y;
	    d = x*x + y*y;
	    if ( d < d0 ) {
	      d0 = d;
	    } else {
	      r1.m_x -= dx;
	      dx = -dx/2;
	    }
	    r1.m_y += dy;
	    this->backward( r1, p1 );
	    x = p.m_x - p1.m_x;
	    y = p.m_y - p1.m_y;
	    d = x*x + y*y;
	    if ( d < d0 ) {
	      d0 = d;
	    } else {
	      r1.m_y -= dy;
	      dy = -dy/2;
	    }
          } while ( d0 > 1.e-6 && fabs(dx) > 1.e-3 && fabs(dy) > 1.e-3);
          ret = r1;
    }

    #ifdef DEBUG
    /** debug: print
     */
    template< >
    void plaquette_pair::print() const
    {
        thprintf("[%d] Plaquette A %6.2f %6.2f <-> %6.2f %6.2f\n",
          nr(), from.m_A.m_x, from.m_A.m_y, to.m_A.m_x, to.m_A.m_y );
        thprintf("              B %6.2f %6.2f <-> %6.2f %6.2f\n",
          from.m_B.m_x, from.m_B.m_y, to.m_B.m_x, to.m_B.m_y );
        thprintf("              C %6.2f %6.2f <-> %6.2f %6.2f\n",
          from.m_C.m_x, from.m_C.m_y, to.m_C.m_x, to.m_C.m_y );
        thprintf("              D %6.2f %6.2f <-> %6.2f %6.2f\n",
          from.m_D.m_x, from.m_D.m_y, to.m_D.m_x, to.m_D.m_y );
        print_ngbhs();
    }
    #endif
    

  } // namespace warp

} // namespace therion



