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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
#include <assert.h>
#include <math.h>

#include "thwarppme.h"
#include "thinfnan.h"

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
      point_pair * p2 = l2->other_end( this );
      assert( p2->mLines.size() == 1 );
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
// therion::warp::plaquette

void therion::warp::plaquette::init()
{
  m_AD = m_A - m_D;
  m_BC = m_B - m_C;

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
  m_abn = m_B - m_A;
  m_abd = m_abn.length(); 
  m_abn.normalize();
  m_abh.m_x =   m_abn.m_y;
  m_abh.m_y = - m_abn.m_x;

  thvec2 ad = m_D - m_A;
  ad.normalize();
  m_theta_l = acos( m_abn * ad );
  m_theta_l = angle( m_abn, ad );
  thvec2 bc = m_C - m_B;
  bc.normalize();
  m_theta_r = acos( - m_abn * bc );
  m_theta_r = angle( bc, - m_abn );
  
  // thprintf("Plaquette Theta L %.2f R %.2f \n", m_theta_l, m_theta_r );
}

double
therion::warp::plaquette::s_map( const thvec2 & p ) const
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

// this is  Beier-Neely map
// "Feature-based image metamorphosis" Computer Graphics, 26 35-42, 1992
//
// TODO 
// Taking into account different end-size ratios can be done here
// or by the caller. The plaquette does not have the ratios, 
// and the relative weight could be different from the return 
// X coordinate (ret.m_x)
//
void
therion::warp::plaquette::bn_map( const thvec2 & p, thvec2 & ret ) const
{
  thvec2 ap = p - m_A;
  ret.m_x = ( ap * m_abn ) / m_abd;
  ret.m_y = ap * m_abh;
}

void 
therion::warp::plaquette::inv_bn_map( const thvec2 & p, thvec2 & ret ) const
{
  double d = p.m_x * m_abd;
  // ret = m_A + m_abn * d + m_abh * p.m_y;
  ret.m_x = m_A.m_x + m_abn.m_x * d + m_abh.m_x * p.m_y;
  ret.m_y = m_A.m_y + m_abn.m_y * d + m_abh.m_y * p.m_y;
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

      m_kl = from.theta_left() - to.theta_left();
      m_kr = from.theta_right() - to.theta_right();
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
    
    /** map p in the "from" image to a point in the "to" img
     * @param p   2D point in the "from" image 
     * @return corresponding 2D point in the "to" image
     */
    template< >
    void item_pair<triangle>::forward( const thvec2 & p, thvec2 & ret ) const
    {
      static thvec2 v0;
      from.bn_map( p, v0 );
      to.inv_bn_map( v0, ret );
    }
    
    /** map p in the "to" image to a point in the "from" img
     * @param p   2D point in the "to" image
     * @return corresponding 2D point in the "from" image
     */
    template< >
    void item_pair<triangle>::backward( const thvec2 & p, thvec2 & ret ) const
    {
      static thvec2 v0;
      to.bn_map( p, v0 );
      from.inv_bn_map( v0, ret );
    }

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
    plaquette_pair::item_pair( point_pair * a, point_pair * b, point_pair * c, point_pair * d
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

      m_kl = from.theta_left() - to.theta_left();
      m_kr = from.theta_right() - to.theta_right();
      m_dl = from.m_AD.length() / to.m_AD.length();
      m_dr = from.m_BC.length() / to.m_BC.length();
      // thprintf("Plaquette [%d] Theta L %.2f R %.2f Ratios L %.2f R %.2f \n", 
      //   nr(), m_kl, m_kr, m_dl, m_dr );
    }
    
    /** warp type
     * @return the warp type of the warping basic_pair
     */
    template< >
    warp_type plaquette_pair::type() const { return THWARP_PLAQUETTE; }
    
    /** maximum number of neighbors
     * @return the maximum number of neighbors
     */
    template< >
    int plaquette_pair::ngbh_nr() const { return 3; }
    
    #define BD 0.5

    /** map p in the "to" image to a point in the "from" img
     * @param p   2D point in the "to" image
     * @return corresponding 2D point in the "from" image
     */
    template< >
    void plaquette_pair::backward( const thvec2 & p, thvec2 & ret ) const
    {
        #if 1
	  static thvec2 v0, r1;
          double s   = to.s_map( p );
	  if ( s < BD ) {
	    double x = s/BD;             // x  = 0 .. 1
	    double x1 = 1.0 - 2*x + x*x;     // x1 = 1 .. 0
	    double t = m_kl*x1;
	    thvec2 pa = to.left_rotate(p, cos(t), -sin(t) );
	    to.bn_map( pa, v0 );
	    // v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	    from.inv_bn_map( v0, ret );
       
  	    double x2 = 1.0 + 2*x - x*x;     //      1 .. 2
            t = left_ngbh()->m_kr * x2;
            pa = left_ngbh()->pto()->right_rotate( p, cos(t), sin(t) );
	    left_ngbh()->pto()->bn_map( pa, v0 );
	    // v0.m_y *= v0.m_x * left_ngbh()->m_dr + (1.0-v0.m_x) * left_ngbh()->m_dl;
	    left_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	    // ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	    ret.m_x = ( ret.m_x + x1 * r1.m_x ) / ( 1 + x1 );
	    ret.m_y = ( ret.m_y + x1 * r1.m_y ) / ( 1 + x1 );
	  } else if ( 1.0 - s < BD ) {
	    double x = (1.0-s)/BD;
	    double x1 = 1.0 - 2*x + x*x;
	    double t = m_kr * x1;
	    thvec2 pa = to.right_rotate(p, cos(t), sin(t) );
	    to.bn_map( pa, v0 );
	    // v0.m_y *= v0.m_x * m_dr + (1.0-v0.m_x) * m_dl;
	    from.inv_bn_map( v0, ret );

	    double x2 = 1.0 + 2*x - x*x;
	    t = right_ngbh()->m_kl * x2;
	    pa = right_ngbh()->pto()->left_rotate( p, cos(t), -sin(t) );
	    right_ngbh()->pto()->bn_map( pa, v0 );
	    // v0.m_y *= v0.m_x * right_ngbh()->m_dr + (1.0-v0.m_x) * right_ngbh()->m_dl;
	    right_ngbh()->pfrom()->inv_bn_map( v0, r1 );
	    // ret = ( ret + r1 * x1 ) / ( 1.0 + x1 );
	    ret.m_x = ( ret.m_x + x1 * r1.m_x ) / ( 1 + x1 );
	    ret.m_y = ( ret.m_y + x1 * r1.m_y ) / ( 1 + x1 );
	  } else {
	    to.bn_map( p, v0 );
	    from.inv_bn_map( v0, ret );
	  }
        #else
	  // too curly
	  thvec2 v0, r;
	  to.bn_map( p, v0 );
	  from.inv_bn_map( v0, r );
          double x1  = 1.0 - v.m_x / BD;
	  thvec2 r1;
	  left_ngbh()->pto()->bn_map( p, v0 );
	  left_ngbh()->pfrom()->inv_bn_map( v0, r1 );
          double x2  = 1.0 - v.m_x;
	  x2 = 1.0 - x2 / BD;
	  thvec2 r2;
	  right_ngbh()->pto()->bn_map( p, v0 );
	  right_ngbh()->pfrom()->inv_bn_map( v0, r2 );
	  ret = ( r + r1 * x1+ r2 * x2 ) / ( 1.0 + x1 + x2 );
        #endif
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
    template< >
    void plaquette_pair::forward( const thvec2 & p, thvec2 & ret ) const
    {
	  static thvec2 v0, r1; 
	  thvec2 p1;
	  double d, d0, x, y;
	  double dx = 0.1, dy=0.1;
	  from.bn_map( p, v0 );
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



