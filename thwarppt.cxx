/** @file thwarptrans.cxx
 * 
 * @author marco corvi
 * @date   nov 2006
 * 
 * @brief warping transformation
 *
 * ---------------------------------------------------
 * GNU GPL licence ...
 */
#include <sstream>

#include "thwarppt.h"

#define BOUND_SEGMENT 1.5
#define BOUND_TRIANGLE 1.5
#define BOUND_PLAQUETTE 2.0

thwarptrans::thwarptrans( )
  : m_initialized( false )
  , m_bound2( BOUND_SEGMENT )
  , m_bound3( BOUND_TRIANGLE )
  , m_bound4( BOUND_PLAQUETTE )
{ }

thwarptrans::thwarptrans( double bound2, double bound3, double bound4 )
  : m_initialized( false )
  , m_bound2( bound2 )
  , m_bound3( bound3 )
  , m_bound4( bound4 )
{ }

thwarptrans::~thwarptrans()
{
  reset();
}

void 
thwarptrans::reset()
{
  for (unsigned int i=0; i<mLines.size(); ++i) {
    delete mLines[i];
  }
  mLines.clear();

  for (unsigned int i=0; i<mPairs.size(); ++i) {
    delete mPairs[i];
  }
  mPairs.clear();

  for (unsigned int i=0; i<mPlaquettes.size(); ++i) {
    delete mPlaquettes[i];
  }
  mPlaquettes.clear();

  m_initialized = false;
}

thmorph_pair *
thwarptrans::insert_point( thmorph_type t, std::string & name, 
                           const thvec2 & src, const thvec2 & dst )
{
  thmorph_pair * pair = new thmorph_pair( t, name, src.m_x, src.m_y, dst.m_x, dst.m_y );
  mPairs.push_back( pair );
  return pair;
}

void 
thwarptrans::insert_line( thmorph_type t, std::string & from, std::string & to )
{
  thmorph_pair * p1 = NULL;
  thmorph_pair * p2 = NULL;
  for ( unsigned int i=0; i<mPairs.size(); ++i ) {
    if ( mPairs[i]->m_name == from ) { p1 = mPairs[i]; if (p2) break; }
    if ( mPairs[i]->m_name == to   ) { p2 = mPairs[i]; if (p1) break; }
  }
  if ( p1 == NULL || p2 == NULL ) throw;
  mLines.push_back( new thmorph_line( t, p1, p2 ) );
}

void
thwarptrans::print()
{
  printf("Warp trans: P %d L %d\n", mPairs.size(), mLines.size() );
  for ( unsigned int i=0; i<mPairs.size(); ++i ) {
    printf("P %2d: %s (T %d) survey %6.2f %6.2f sketch %6.2f %6.2f\n", 
      i, mPairs[i]->m_name.c_str(), mPairs[i]->m_type, 
      mPairs[i]->u.m_x, mPairs[i]->u.m_y, mPairs[i]->x.m_x, mPairs[i]->x.m_y );
  }
  for ( unsigned int i=0; i<mLines.size(); ++i ) {
    printf("L %2d: %s %s (T %d)\n", i, mLines[i]->m_p1->m_name.c_str(),
      mLines[i]->m_p2->m_name.c_str(), mLines[i]->m_type );
  }
}

bool
thwarptrans::initialize( thwarp_proj proj )
{
  if ( mPairs.size() < 2 )
    return false;

  if ( ! m_initialized ) {
    unsigned int sz = mPairs.size();

    // bounding box
    thvec2 xmin( mPairs[0]->x );
    thvec2 xmax( xmin );
    thvec2 umin( mPairs[0]->u );
    thvec2 umax( umin );

    for ( unsigned int k=1; k<sz; ++k) {
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
    for ( unsigned int k=0; k<sz; ++k ) {
        mPairs[k]->z = ( mPairs[k]->x - mX0 ) / mXUnit;
        mPairs[k]->w = ( mPairs[k]->u - mU0 ) / mUUnit;
    }
    mUC = mU0;
    mUCUnit = mUUnit;

    update_lines();
    make_plaquettes( proj );
  
    m_initialized = true;
  }
  return m_initialized;
}

thmorph_line *
thwarptrans::add_extra_line( thmorph_pair * p1, unsigned int index, thvec2 & x3, thvec2 & u3 )
{
  std::ostringstream ost;
  ost << p1->m_name << "_E_" << index;
  std::string name = ost.str();
  // thprintf("Extra line() point %s %6.2f %6.2f %6.2f %6.2f \n",
  //   name.c_str(), x3.m_x, x3.m_y, u3.m_x, u3.m_y );
  thmorph_pair * p3 = insert_point( THMORPH_EXTRA, name, x3, u3 );
  p3->update( mX0, mXUnit, mU0, mUUnit );
  
  thmorph_line * l3 = new thmorph_line( THMORPH_EXTRA, p1, p3 );
  l3->update();
  mLines.push_back( l3 );
  return l3;
}

// ========================================================================

bool
thwarptrans::forward( const thvec2 & src, thvec2 & dst, 
                      const thvec2 & origin, double unit )
{
  thvec2 z = ( src - mX0) / mXUnit;
  unsigned int szp = mPlaquettes.size();
  double dmin = -1.0;
  unsigned int imin = (unsigned int)(-1);
  for ( unsigned int i=0; i<szp; ++i) {
    if ( mPlaquettes[i]->is_inside_from( z ) ) {
      double d = mPlaquettes[i]->distance_from( z );
      if ( thisnan( d ) ) continue;
      if ( imin == (unsigned int)(-1) || d < dmin ) {
        dmin = d;
        imin = i;
      }
    }
  }
  if ( imin == (unsigned int)(-1) ) 
    return false;
  thvec2 w = mPlaquettes[imin]->forward( z );
  if ( w.is_nan() )
    return false;
  dst = ( origin + w * unit );
  return true;
}

bool
thwarptrans::backward( const thvec2 & dst, thvec2 & src, 
                       const thvec2 & origin, double unit )
{
  thvec2 w = (dst - origin) / unit;
  unsigned int szp = mPlaquettes.size();
  double dmin = -1.0;
  unsigned int imin = (unsigned int)(-1);
  for ( unsigned int i=0; i<szp; ++i) {
    if ( mPlaquettes[i]->is_inside_to( w ) ) {
      double d = mPlaquettes[i]->distance_to( w );
      if ( thisnan( d ) ) continue;
      if ( imin == (unsigned int)(-1) || d < dmin ) {
        dmin = d;
        imin = i;
      }
    }
  }
  if ( imin == (unsigned int)(-1) ) 
    return false;
    thvec2 z = mPlaquettes[imin]->backward( w );
    if ( z.is_nan( ) ) 
      return false;
    src = mX0 + z * mXUnit;
  return true;
}

void 
thwarptrans::update_lines()
{
  for (unsigned int i=0; i<mLines.size(); ++i) {
    mLines[i]->update();
  }
}


void 
thwarptrans::make_plaquettes( thwarp_proj proj )
{
  for (unsigned int k=0; k<mPlaquettes.size(); ++k) {
    delete mPlaquettes[k];
  }
  mPlaquettes.clear();

  // print();
  unsigned int szp = mPairs.size();
  for (unsigned int i=0; i<szp; ++i) {
      mPairs[i]->order_lines( this, mXUnit/mUUnit, proj );
  }
  // print();

  szp = mPairs.size();
  for (unsigned int i=0; i<szp; ++i) {
    mPairs[i]->m_used = false;
  }

  for ( ; ; ) {
    thmorph_pair * node1 = NULL;
    thmorph_line * line = NULL;
    for (unsigned int i=0; i<szp; ++i) {
      /*
      if ( mPairs[i]->legs() == 1 && ! mPairs[i]->m_used ) {
        node1 = mPairs[i];
        line  = node1->first_leg();
        for ( ++i; i<szp; ++i) {
          if ( mPairs[i]->size() == 1 ) {
            thmorph_line * ln = mPairs[i]->next_line(NULL);
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
  
    thmorph_pair * node2 = line->other_end( node1 );
    thmorph_pair * node10 = node1; // save starting nodes
    thmorph_pair * node20 = node2;
    // thprintf("make_plaquettes()");
    // thprintf(" node1 %s node2 %s\n", node1->m_name.c_str(), node2->m_name.c_str() );
  
    do {
      thmorph_pair * A = node1;
      thmorph_pair * B = node2;
      if ( line->m_type == THMORPH_STATION ) {
        unsigned int iA = A->size();
        unsigned int iB = B->size();
        if ( iA == 1 ) // mark the node as used
          A->m_used = true;
        if ( iB == 1 ) 
          B->m_used = true;

        // thprintf("  A %s (%d)  B %s (%d) line type S\n", A->m_name.c_str(), iA, B->m_name.c_str(), iB );
        if ( iA == 1 && iB == 1 ) { 
          // segment AB:
          //              A =========== B
          //
          add_segment( A, B );
          node1 = B;
          node2 = A;
        } else if ( iA == 1 ) {
          //                      /
          //           A ======= B ... 
          //                     | 
          //                     C
          //
          // do not do anything: B will do it
          thmorph_line * line2 = B->next_line( line );
          thmorph_pair * C = line2->other_end( B );
          if ( line2->m_type != THMORPH_STATION )
            add_triangle( A, B, C );
          node1 = B;
          node2 = C;
          line = line2;
        } else if ( iB == 1 ) {
          //
          //           | 
          //       ... A ======= B  
          //          / 
          //         D
          thmorph_line * line1 = A->prev_line( line );
          thmorph_pair * D = line1->other_end( A );
          add_triangle( D, A, B );
          node1 = B;
          node2 = A;
          // line = line;
        } else {
          //    ... A ========= B ...
          //        |           |
          //        D           C
          //
          thmorph_line * line1 = A->prev_line( line );
          thmorph_pair * D = line1->other_end( A );
          thmorph_line * line2 = B->next_line( line );
          thmorph_pair * C = line2->other_end( B );
          add_quadrilater( A, B, C, D );
          node1 = B;
          node2 = C;
          line = line2;
        }
      } else { // line->m_type != THMORPH_STATION 
        unsigned int iA = A->size();
        unsigned int iB = B->size();
        if ( iA == 1 ) // mark the node as used
          A->m_used = true;
        if ( iB == 1 ) 
          B->m_used = true;

        // thprintf("  A %s B %s line type non-S\n", A->m_name.c_str(), B->m_name.c_str() );
        //      ?
        //      |
        //      A ======== B
        //
        thmorph_line * line2 = A->next_line( line );
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
          thmorph_pair * D = line2->other_end( A );
          add_triangle( B, A, D );
          node2 = D;
          line = line2;
        }
      }
    } while ( node1 != node10 );
    // back to start
    while ( node2 != node20 ) {
      thmorph_line * line2 = node1->next_line( line );
      thmorph_pair * C = line2->other_end( node1 );
      add_triangle( node2, node1, C );
      node2 = C;
      line = line2;
    }
  } // end big for ( ; ; )
}

void
thwarptrans::add_segment( thmorph_pair * A, thmorph_pair * B )
{
  // thprintf("  ADD Segment %s %s\n", A->m_name.c_str(), B->m_name.c_str() );
  thmorph_segment_pair * sgm = new thmorph_segment_pair( A, B, this->m_bound2, false );
  mPlaquettes.push_back( sgm );
}

void
thwarptrans::add_triangle( thmorph_pair * A, thmorph_pair * B, thmorph_pair * C )
{
  double vol_x = (C->x - B->x) ^ (A->x - B->x);
  double vol_u = (C->u - B->u) ^ (A->u - B->u);
  // thprintf("  ADD Triangle: %s %s %s (vol. %8.4f %8.4f)\n",
  //   A->m_name.c_str(), B->m_name.c_str(), C->m_name.c_str(), vol_x, vol_u );
  if ( fabs(vol_u) < 1.E-4 ) {
    add_segment( A, C );
    return;
  }
  if ( vol_x >= 0.0 ) {
    thprintf("WARNING: bad triangle (%s %s %s) X-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } else if ( ((C->u - B->u) ^ (A->u - B->u)) >= 0.0 ) {
    thprintf("WARNING: bad triangle (%s %s %s) U-orientation\n",
      C->m_name.c_str(), B->m_name.c_str(), A->m_name.c_str() );
  } else {
    thmorph_triangle_pair * tri = new thmorph_triangle_pair( B, C, A, this->m_bound3  );
    mPlaquettes.push_back( tri );
  }
}

void
thwarptrans::add_quadrilater( thmorph_pair * A, thmorph_pair * B, 
                              thmorph_pair * C, thmorph_pair * D )
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
    thmorph_plaquette_pair * plaq = new thmorph_plaquette_pair( A, B, C, D, this->m_bound4 );
    mPlaquettes.push_back( plaq );
  }
}




bool
thwarptrans::map_backward_plaquette( unsigned int k, const thvec2 & w, thvec2 & z )
{
  if ( mPlaquettes[k]->is_inside_to( w ) ) {
    z = mPlaquettes[k]->backward( w );
    return true;
  }
  return false;
}


bool
thwarptrans::map_image( const unsigned char * src, unsigned int ws, unsigned int hs,
                       unsigned char * dst, unsigned int wd, unsigned int hd,
		       thvec2 const & origin, double unit,
                       int depth, 
		       thwarp_proj proj )
{
  if ( ! initialize( proj ) ) return false;

  mUC     = origin;
  mUCUnit = unit;

  // thprintf("X origin %6.2f %6.2f units %6.2f\n", mX0.m_x, mX0.m_y, mXUnit);
  // thprintf("U origin %6.2f %6.2f units %6.2f\n", mU0.m_x, mU0.m_y, mUUnit);
  // thprintf("UC origin %6.2f %6.2f units %6.2f\n", mUC.m_x, mUC.m_y, mUCUnit);
  // new way
  unsigned int * pi = (unsigned int *)malloc( wd * hd * sizeof(unsigned int) );
  if ( pi != NULL ) {
    memset( pi, 0xff, wd*hd*sizeof(unsigned int) );
    double * pf = (double *)malloc( wd * hd * sizeof(double) );
    if ( pf == NULL ) 
      thprintf("warning: failed to allocate temporary distance image\n");
  
    for (unsigned int k=0; k<mPlaquettes.size(); ++k ) {
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
        unsigned int * piwd = pi + j*wd;
        double *       pfwd = pf + j*wd;
        for (int i=(int)t1.m_x; i<i2; ++i) {
          thvec2 u( i, j );
          thvec2 w = (u - mUC) / mUCUnit;
          thvec2 z;
          if ( map_backward_plaquette( k, w, z ) ) {
            if ( piwd[i] == 0xffffffff ) {
              piwd[i] = k;
	      if ( pf ) pfwd[i] = -1.0;
            } else {
              // thvec2 x = mX0 + z * mXUnit;
              double d1 = mPlaquettes[k]->distance2_to( w );
	      if ( pf ) {
	        if ( pfwd[i] < 0.0 ) pfwd[i] = mPlaquettes[piwd[i]]->distance2_to( w );
                double d0 = pfwd[i];
                if ( d1 <d0 ) { piwd[i] = k; pfwd[i] = d1; }
	      } else {
	        double d0 = mPlaquettes[piwd[i]]->distance2_to( w );
		if ( d1 <d0 ) { piwd[i] = k; }
	      }
            }
          }
        }
      }
    }
    for (unsigned int j=0; j<hd; ++j) {
      for (unsigned int i=0; i<wd; ++i) {
        unsigned int k = pi[j*wd+i];
        if ( k < mPlaquettes.size() ) {
          thvec2 u( i, j );
          thvec2 z;
          thvec2 w = (u - mUC) / mUCUnit;;
          map_backward_plaquette( k, w, z );
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
    if ( pf ) free( pf );
    free( pi );
  } else { // old way (used if pi malloc failed ...
    thprintf("warning: failed to allocate temporary index image\n");
    for (unsigned int k=0; k<mPlaquettes.size(); ++k ) {
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

  //thprintf("warp done\n");
  /* debug: check points 
  unsigned int szp = mPairs.size();
  for (unsigned int i=0; i<szp; ++i) {
    // thprintf("%s: X %.2f %.2f Z %.2f %.2f U %.2f %.2f W %.2f %.2f\n",
    thprintf("%s: X %.2f %.2f U %.2f %.2f \n",
      mPairs[i]->m_name.c_str(),
      mPairs[i]->x.m_x, mPairs[i]->x.m_y,
      // mPairs[i]->z.m_x, mPairs[i]->z.m_y,
      mPairs[i]->u.m_x, mPairs[i]->u.m_y );
      // mPairs[i]->w.m_x, mPairs[i]->w.m_y );
    thvec2 u1, u2;
    forward( mPairs[i]->x, u1, thvec2(0.0,0.0), 1.0 );
    forward( mPairs[i]->x, u2, mUC, mUCUnit );
    thprintf("    U1 %.2f %.2f U2 %.2f %.2f\n",
      u1.m_x, u1.m_y, u2.m_x, u2.m_y );
  }
  */

  return true;
}
