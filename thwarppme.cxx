/**
 * @file thmorphelement.cxx
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
#include <assert.h>
#include <math.h>

#include "thwarppme.h"
#include "thinfnan.h"

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
thcompute_vertical( double a, thvec2 & x, thvec2 & u )
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
thcompute_segment_distance2( const thvec2 & x, const thvec2 & b, const thvec2 & c )
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


void 
thmorph_pair::add_line( thmorph_line * line )
{
  // thprintf("%s add_line %s-%s\n", 
  //   m_name.c_str(), line->m_p1->m_name.c_str(), line->m_p2->m_name.c_str() );
  if ( line->m_type == THMORPH_STATION ) m_legs ++;
  mLines.push_back( line );
}

thmorph_line::thmorph_line( thmorph_type t, thmorph_pair * p1, thmorph_pair * p2 )
  : m_type( t ) 
  , m_p1( p1 )
  , m_p2( p2 )
{ 
  // thprintf("New line: P1 %s P2 %s\n", p1->m_name.c_str(), p2->m_name.c_str() );
  if ( p1 != NULL ) p1->add_line( this );
  if ( p2 != NULL ) p2->add_line( this );
}

void
thmorph_pair::update( thvec2 & x0, double xunit, thvec2 & u0, double uunit )
{
  z = ( x - x0 ) / xunit;
  w = ( u - u0 ) / uunit;
}

void
thmorph_pair::order_lines( thinserter * morpher, double x_u, thwarp_proj proj )
{
  unsigned int sz = mLines.size();
  // thprintf("order_lines(): Point %s has %d/%d lines\n", m_name.c_str(), sz, m_legs );
  if ( sz <= 1 ) {
    return;
  }

  bool repeat = true;
  while ( repeat ) {
    sz = mLines.size();

    thmorph_line * l1 = mLines[0];
    thvec2 v1 = ( this == l1->m_p1 ) ? l1->vz : l1->vz * (-1);
    for ( unsigned int i=1; i<sz-1; ++i) {
      double theta_min = 2*THPI;
      unsigned int jmin = i;
      for (unsigned int j=i; j<sz; ++j) {
        thmorph_line * l2 = mLines[j];
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
    for ( unsigned int i=0; i<sz; ++i) {
      unsigned int j = (i+1)%sz;
      if ( m_legs >= 2 ) {
        thmorph_line * l1 = mLines[i];
        thmorph_line * l2 = mLines[j];
        if ( l1->m_type != THMORPH_STATION ) continue;
        if ( l2->m_type != THMORPH_STATION ) continue;
        thmorph_pair * p1 = l1->other_end( this );
        thmorph_pair * p2 = l2->other_end( this );
        // printf("point %s (%s - %s) sizes %d %d \n",
        //   m_name.c_str(),  p1->m_name.c_str(), p2->m_name.c_str(), p1->size(), p2->size() );
        if ( p1->size() == 1 && p2->size() == 1 ) continue;

      } else if ( m_legs == 0 ) {
        continue;
      } else { // m_legs == 1
        // angle between line[i] and line[j]
        thmorph_line * l1 = mLines[i];
        thmorph_line * l2 = mLines[j];
        thmorph_pair * p1 = l1->other_end( this );
        thmorph_pair * p2 = l2->other_end( this );
        // printf("point %s (%s - %s) legs 1 angle %6.2f\n", 
        //   m_name.c_str(), p1->m_name.c_str(), p2->m_name.c_str(), 
        //   (p2->u - u) ^ (p1->u -u ) );
        if ( ((p2->u - u) ^ (p1->u -u )) < 0 )
          continue;
      }
      repeat = true;
      // thprintf("Point %s: insert %d/%d lines (j=%d)\n", m_name.c_str(), i, sz, j );
      thmorph_line * l1 = mLines[i];
      thmorph_line * l2 = mLines[j];
      thmorph_pair * p1 = l1->other_end( this ); // ( this == l1->m_p1 ) ? l1->m_p2 : l1->m_p1;
      thmorph_pair * p2 = l2->other_end( this ); // ( this == l2->m_p1 ) ? l2->m_p2 : l2->m_p1;
      thvec2 x1 = p1->x - x;
      thvec2 x2 = p2->x - x;
      thvec2 u1 = p1->u - u;
      thvec2 u2 = p2->u - u;
      // printf("must insert between %s %s at %s (m_legs %d) proj %s\n",
      //   p1->m_name.c_str(), p2->m_name.c_str(), m_name.c_str(), m_legs,
      //   (proj == THWARP_PLAN)? "plan" : "elev" );
      // try a non-STATION line that reflected is between l1 and l2
      unsigned int k = 0;
      if ( m_legs <= 2 ) {
        for ( ; k<sz; ++k) {
          if ( mLines[k]->m_type == THMORPH_STATION ) continue;
          thmorph_pair * p0 = mLines[k]->other_end( this );
          thvec2 x0 = x - p0->x;
          thvec2 u0 = u - p0->u;
          if ( (x2 ^ x0) < 0 && (x0 ^ x1) < 0 && (u2 ^ u0) < 0 && (u0 ^ u1) ) {
            // opposite is OK both for PLAN and for EXTENDED proj
            // thprintf("can use opposite of %s\n", p0->m_name.c_str() );
            thvec2 x3 = x + x0;
            thvec2 u3 = u + u0;
            morpher->add_extra_line( this, i, x3, u3 );
            break;
          }
        }
      } else { 
        k = sz; // still need to insert
      }

      if ( k == sz ) {
        thvec2 x3, u3;
        if ( proj == THWARP_PLAN ) {
          thvec2 vu = u2/u2.length() - u1/u1.length();
          // thline2 lu( vu.m_x, vu.m_y, - u.m_x * vu.m_x - u.m_y * vu.m_y );
          // double au = vu.length();
          double du = (u1 - u2).length() / 2.0;
          double d1 = u1.length()/2.0; // fabs( lu.eval( u1 ) / au );
          double d2 = u2.length()/2.0; // fabs( lu.eval( u2 ) / au );
          if ( d1 < du ) du = d1;
          if ( d2 < du ) du = d2;
          // du /= 2*au;
          // double du = dx / x_u;
          u3.m_x = u.m_x + vu.m_y * du;
          u3.m_y = u.m_y - vu.m_x * du;

          thvec2 vx = x2/x2.length() - x1/x1.length();
          // thline2 lx( vx.m_x, vx.m_y, - x.m_x * vx.m_x - x.m_y * vx.m_y );
          // double ax = vx.length();
          /*
          double dx = (x1 - x2).length() / 2.0;
          double d1 = x1.length()/2.0; // fabs( lx.eval( x1 ) / ax );
          double d2 = x2.length()/2.0; // fabs( lx.eval( x2 ) / ax );
          if ( d1 < dx ) dx = d1;
          if ( d2 < dx ) dx = d2;
          // dx /= 2*ax;
          */
          double dx = du * x_u;
          x3.m_x = x.m_x + vx.m_y * dx;
          x3.m_y = x.m_y - vx.m_x * dx;
        } else /* if ( proj == THWARP_EXTENDED ) */ {
          #define TH_VERT_ALPHA 32.0
          double v1 = thcompute_vertical( TH_VERT_ALPHA, x1, u1 );
          double v2 = thcompute_vertical( TH_VERT_ALPHA, x2, u2 );
          thvec2 vu = u2/u2.length() - u1/u1.length();
          double du = (u1 - u2).length() / 2.0;
          double d1 = u1.length()/2.0; // fabs( lu.eval( u1 ) / au );
          double d2 = u2.length()/2.0; // fabs( lu.eval( u2 ) / au );
          if ( d1 < du ) du = d1;
          if ( d2 < du ) du = d2;
          double dx = du * x_u;
          // printf("v1 %f v2 %f \n", v1, v2 );
          if ( fabs(v1) < 0.5 || fabs(v2) < 0.5 ) {
            if ( fabs(v1) < 0.1 && fabs(v2) < 0.1 ) {
              if ( v1 * v2 < 0.0 ) { // vertical opposite
                // printf("horizontal\n");
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
              // printf("bisector\n");
              u3.m_x = u.m_x + vu.m_y * du;
              u3.m_y = u.m_y - vu.m_x * du;
              thvec2 vx = x2/x2.length() - x1/x1.length();
              x3.m_x = x.m_x + vx.m_y * dx;
              x3.m_y = x.m_y - vx.m_x * dx;
            }
          } else {
            // printf("vertical\n");
            u3.m_x = u.m_x;
            u3.m_y = u.m_y + ((v2>0.0)? 1.0 : -1.0) * du;
            x3.m_x = x.m_x;
            x3.m_y = x.m_y + ((v2>0.0)? 1.0 : -1.0) * dx;
          }
        }
        morpher->add_extra_line( this, i, x3, u3 );
      }
    } // for ( ...; i<sz; ...)
  } // while ( repeat )

  sz = mLines.size();
  /*
  thprintf("Point %s: %6.2f %6.2f has %d/%d lines: ", m_name.c_str(), x.m_x, x.m_y, sz, m_legs );
  for ( unsigned int i=0; i<sz; ++i) {
    thmorph_pair * p2 = mLines[i]->other_end( this );
    thprintf("%s ", p2->m_name.c_str() );
  }
  thprintf("\n");
  */

  // check that all the lines from this point that are not centerlines have a "single"
  // node at the other end
  // this check could be restricted to 
  //    if ( m_type == THMORPH_STATION )
  for ( unsigned int i=0; i<sz; ++i) {
    thmorph_line * l2 = mLines[i];
    if ( l2->m_type != THMORPH_STATION ) {
      thmorph_pair * p2 = l2->other_end( this );
      assert( p2->mLines.size() == 1 );
    }
  }
}


thmorph_line * 
thmorph_pair::first_leg()
{
  unsigned int sz = mLines.size();
  for (unsigned int j=0; j<sz; ++j) {
    if ( mLines[j]->m_type == THMORPH_STATION ) return mLines[j];
  }
  return NULL;
}


void
thmorph_line::update()
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
  R.m_xx = R.m_yy = ( vw * vz ) / dz;
  R.m_xy = ( vw ^ vz ) / dz;
  R.m_yx = - R.m_xy;

  double dw = vw.length2();
  S.m_xx = S.m_yy = ( vz * vw ) / dw;
  S.m_xy = ( vz ^ vw ) / dw;
  S.m_yx = - S.m_xy;

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
// thmorph_segment

void thmorph_segment::init( bool reverse )
{
  m_AB = m_B - m_A;
  double n = m_AB.length2();
  m_ABh = m_AB.orthogonal();
  m_ABn = m_AB / n;
  m_ABh /= sqrt(n);
  if (reverse) m_ABh *= -1.0;
  /*
  thprintf("Segment: A %6.2f %6.2f   B %6.2f %6.2f\n", m_A.m_x, m_A.m_y, m_B.m_x, m_B.m_y );
  thprintf("        AB %6.2f %6.2f ABn %6.2f %6.2f ABh %6.2f %6.2f\n", 
         m_AB.m_x, m_AB.m_y, m_ABn.m_x, m_ABn.m_y, m_ABh.m_x, m_ABh.m_y );
  */
}

thvec2 
thmorph_segment_pair::backward( const thvec2 & p )
{
    thvec2 v = to.map( p );
    return from.m_A + v.m_x * from.m_AB + v.m_y * from.m_ABh;
}

bool 
thmorph_segment_pair::is_inside_from( const thvec2 & p )
{
    thvec2 v = from.map( p );
    return v.m_y >= 0 && v.m_y < m_bound
        && v.m_x > - m_bound && v.m_x < 1 + m_bound;
}

bool 
thmorph_segment_pair::is_inside_to( const thvec2 & p )
{
    thvec2 v = to.map( p );
    return v.m_y >= 0 && v.m_y < m_bound 
        && v.m_x > - m_bound && v.m_x < 1 + m_bound;
}

double 
thmorph_segment_pair::distance_from( const thvec2 & p ) 
{
    thvec2 v = from.map( p );
    return v.m_y;
}

double 
thmorph_segment_pair::distance_to( const thvec2 & p ) 
{
    thvec2 v = to.map( p );
    return v.m_y;
}

double
thmorph_segment_pair::distance2_from( const thvec2 & p )
{
  return thcompute_segment_distance2( p, from.m_A, from.m_B);
}

double
thmorph_segment_pair::distance2_to( const thvec2 & p )
{
  return thcompute_segment_distance2( p, to.m_A, to.m_B);
}

void 
thmorph_segment_pair::bounding_box_to( thvec2 & t1, thvec2 & t2 )
{
  thvec2 a = to.m_A - m_bound * to.m_AB;
  thvec2 b = to.m_B + m_bound * to.m_AB;
  t1 = a;
  t1.minimize( b );
  t2 = a;
  t2.maximize( b );
  a = a + m_bound * to.m_ABh;
  b = b + m_bound * to.m_ABh;
  t1.minimize( a );
  t1.minimize( b );
  t2.maximize( a );
  t2.maximize( b );
}

// ---------------------------------------------------------------
// thmorph_plaquette

void thmorph_plaquette::init()
{
  m_AD = m_A - m_D;
  m_BC = m_B - m_C;
#if TRANSFORMATION == 0 
  // thprintf("init() intersection interpolation\n");
  m_AC_BD = m_A + m_C - m_B - m_D;
  m_ac = m_A ^ m_C;  
  m_ab = m_A ^ m_B;  // for t
  m_db = m_D ^ m_B;
  m_dc = m_D ^ m_C;
  m_AB = m_A - m_B;
  m_DC = m_D - m_C;
  m_at = m_ab - m_ac - m_db + m_dc;
  m_bt = m_ac + m_db  - 2 * m_ab;
  m_ad = m_A ^ m_D;  // for s
  m_bc = m_B ^ m_C;
  m_bd = - m_db;
  m_as = m_ad - m_ac - m_bd + m_bc;
  m_bs = m_ac + m_bd  - 2 * m_ad;
#elif TRANSFORMATION == 1
  // thprintf("init() bilinear transformation\n");
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
  // m_D0 = m_a * m_g - m_c * m_e;
  // m_E0 = m_a * m_h - m_d * m_e - m_c * m_f + m_b * m_g;
  // m_F0 = m_b * m_h - m_d * m_f;
#endif
}


/** solve the system
 *    | A+t(D-A)   B+t(C-B)   P  | = 0
 */
thvec2 thmorph_plaquette::map( const thvec2 & p )
{
  double t, s;
#if TRANSFORMATION == 0
  // intersection interpolation
  double a = m_at;
  double b = m_bt + ( m_AC_BD ^ p );
  double c = m_ab - ( m_AB ^ p );

  if ( fabs(a) > 1.e-6 ) { // 2-nd order eq.
    double det = b*b - 4 * a * c;
    if ( det < 0.0 ) {
      t = NAN;
    } else if ( det < 1.e-12 ) {
      t =  -b / (2 * a);
    } else {
      t = (-b - sqrt(det) ) / (2 * a);
      double t2 = (-b + sqrt(det) ) / (2 * a);
      if ( t2 > 0.0 && fabs(t2) < fabs(t) ) t = t2;
    }
  } else if ( fabs(b) != 0 ) { // 1-st order eq.
    t = -c / b;
  } else {
    t = ( c == 0.0 ) ? INFINITY : NAN;
  }

  a = m_as;
  b = m_bs + ( m_AC_BD ^ p );
  c = m_ad - ( m_AD ^ p );
  if ( fabs(a) > 1.e-6 ) { // 2-nd order eq.
    double det = b*b - 4 * a * c;
    if ( det < 0.0 ) {
      s = NAN;
    } else if ( det < 1.e-12 ) {
      s = -b / (2 * a);
    } else {
      s = (-b - sqrt(det) ) / (2 * a);
      double s2 = (-b + sqrt(det) ) / (2 * a);
      if ( s2 > 0.0 && fabs(s2) < fabs(s) ) s = s2;
    }
  } else if ( fabs(b) > 1.e-6 ) { // 1-st order eq.
    s = -c / b;
  } else {
    s = ( c == 0.0 ) ? INFINITY : NAN;
  }
#elif TRANSFORMATION == 1
  // bilinear interpolation
  double ex_ay = m_e * p.m_x - m_a * p.m_y;
  double A = m_A0;
  // double D = m_D0;
  double B = (ex_ay + m_B0)/2.0;
  // double E = ex_ay + m_E0;
  double C = m_g * p.m_x - m_c * p.m_y + m_C0;
  // double F = m_f * p.m_x - m_b * p.m_y + m_F0;
  double det = B*B - A * C;
  if ( det < 0.0 )
    return thvec2( thnan, thnan );
  if ( det > 0.0 ) det = sqrt( det );
  if ( fabs(A) > 1.e-6 ) {
    s = (- B + det )/A;
    double s2 = (-B-det)/A;
    if ( s2 > 0.0 && fabs(s2) < fabs(s) ) s = s2;
  } else {
    s = ( B == 0.0 ) ? thinf : - C / (2*B);
  }
  t = ( p.m_x - m_b * s - m_d ) / ( m_a * s + m_c );
#endif
  return thvec2( s, t ); 
}

// ----------------------------------------------------
// thmorph_triangle_pair

thvec2 
thmorph_triangle_pair::forward( const thvec2 & p )
{
  thline2 l = from.map( p );
  return l.m_a * to.m_A + l.m_b * to.m_B + l.m_c * to.m_C;
}

thvec2
thmorph_triangle_pair::backward( const thvec2 & p )
{
  thline2 l = to.map( p );
  return l.m_a * from.m_A + l.m_b * from.m_B + l.m_c * from.m_C;
}

bool 
thmorph_triangle_pair::is_inside_from( const thvec2 & p )
{
  thline2 l = from.map( p );
  return l.m_b >= 0 && l.m_c >= 0 && l.m_a >= - m_bound;
}

bool 
thmorph_triangle_pair::is_inside_to( const thvec2 & p )
{
  thline2 l = to.map( p );
  return l.m_b >= 0 && l.m_c >= 0 && l.m_a >= - m_bound;
}

double 
thmorph_triangle_pair::distance_from( const thvec2 & p ) 
{
  thline2 l = from.map( p );
  return 1.0 - l.m_a;
}

double 
thmorph_triangle_pair::distance_to( const thvec2 & p ) 
{
  thline2 l = to.map( p );
  return 1.0 - l.m_a;
}

double
thmorph_triangle_pair::distance2_from( const thvec2 & p )
{
  thline2 l = from.map( p );
  if ( l.m_a > 0 ) return 0.0;
  if ( l.m_b < 0.0 || l.m_c < 0.0 ) return 1.e+12; // INFINITY;
  return thcompute_segment_distance2( p, from.m_B, from.m_C );
}

double
thmorph_triangle_pair::distance2_to( const thvec2 & p )
{
  thline2 l = to.map( p );
  if ( l.m_a > 0 ) return 0.0;
  if ( l.m_b < 0.0 || l.m_c < 0.0 ) return 1.e+12; // INFINITY;
  return thcompute_segment_distance2( p, to.m_B, to.m_C );
}

void 
thmorph_triangle_pair::bounding_box_to( thvec2 & t1, thvec2 & t2 )
{
  thvec2 b = to.m_B - m_bound * (to.m_A - to.m_B);
  thvec2 c = to.m_C - m_bound * (to.m_A - to.m_C);
  t1 = to.m_A;
  t1.minimize( b );
  t1.minimize( c );
  t2 = to.m_A;
  t2.maximize( b );
  t2.maximize( c );
}

// ---------------------------------------------------------------
// thmorph_plaquette_pair

thvec2 thmorph_plaquette_pair::forward( const thvec2 & p )
{
  thvec2 v = from.map( p );
#if TRANSFORMATION == 0
  // intersection interpolation
  thvec2 x1 = to.m_A - v.m_x * to.m_AB;
  thvec2 x2 = to.m_D - v.m_x * to.m_DC;
  thline2 ly( x1, x2 );
  thvec2 y1 = to.m_A - v.m_y * to.m_AD;
  thvec2 y2 = to.m_B - v.m_y * to.m_BC;
  thline2 lx( y1, y2 );
  return thvec2( lx, ly );
#elif TRANSFORMATION == 1
  // bilinear interpolation
  return thvec2( to.m_a * v.m_x * v.m_y + to.m_b * v.m_x + to.m_c * v.m_y + to.m_d,
                 to.m_e * v.m_x * v.m_y + to.m_f * v.m_x + to.m_g * v.m_y + to.m_h );
#endif
}


thvec2 
thmorph_plaquette_pair::backward( const thvec2 & p )
{
  thvec2 v = to.map( p );
#if TRANSFORMATION == 0
  // intersection interpolation
  thvec2 x1 = from.m_A - v.m_x * from.m_AB;
  thvec2 x2 = from.m_D - v.m_x * from.m_DC;
  thline2 ly( x1, x2 );
  thvec2 y1 = from.m_A - v.m_y * from.m_AD;
  thvec2 y2 = from.m_B - v.m_y * from.m_BC;
  thline2 lx( y1, y2 );
  return thvec2( lx, ly );
#elif TRANSFORMATION == 1
  // bilinear interpolation
  return thvec2( from.m_a * v.m_x * v.m_y + from.m_b * v.m_x + from.m_c * v.m_y + from.m_d,
                 from.m_e * v.m_x * v.m_y + from.m_f * v.m_x + from.m_g * v.m_y + from.m_h );
#endif
}



bool 
thmorph_plaquette_pair::is_inside_from( const thvec2 & p )
{
  thvec2 v = from.map( p );
  if ( v.is_nan() ) return false;
  return v.m_x >= 0.0 && v.m_x <= 1.0 && v.m_y >= 0.0 && v.m_y < m_bound;
}

bool 
thmorph_plaquette_pair::is_inside_to( const thvec2 & p )
{
  thvec2 v = to.map( p );
  if ( v.is_nan() ) return false;
  return v.m_x >= 0.0 && v.m_x <= 1.0 && v.m_y >= 0.0 && v.m_y < m_bound;
}

double 
thmorph_plaquette_pair::distance_from( const thvec2 & p ) 
{
  thvec2 v = from.map( p );
  return v.m_y;
}

double 
thmorph_plaquette_pair::distance_to( const thvec2 & p ) 
{
  thvec2 v = to.map( p );
  return v.m_y;
}

double
thmorph_plaquette_pair::distance2_from( const thvec2 & p )
{
  thvec2 v = from.map( p );
  if ( v.m_y < 0.0 || v.m_x < 0.0 || v.m_x > 1.0 ) return 1.e+12; // INFINITY;
  if ( v.m_y < 1.0 ) return 0.0;
  return thcompute_segment_distance2( p, from.m_D, from.m_C );
}

double
thmorph_plaquette_pair::distance2_to( const thvec2 & p )
{
  thvec2 v = to.map( p );
  if ( v.m_y < 0.0 || v.m_x < 0.0 || v.m_x > 1.0 ) return 1.e+12; // INFINITY;
  if ( v.m_y < 1.0 ) return 0.0;
  return thcompute_segment_distance2( p, to.m_D, to.m_C );
}


void 
thmorph_plaquette_pair::bounding_box_to( thvec2 & t1, thvec2 & t2 )
{
  thvec2 c = to.m_B - m_bound * to.m_BC;
  thvec2 d = to.m_A - m_bound * to.m_AD;
  t1 = to.m_A;
  t1.minimize( to.m_B );
  t1.minimize( c );
  t1.minimize( d );
  t2 = to.m_A;
  t2.maximize( to.m_B );
  t2.maximize( c );
  t2.maximize( d );
}
