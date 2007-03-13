/**
* @file thgeomag.cxx
*/

/* Copyright (C) 2006 Martin Budaj
* 
* based on GPL-licensed code by
* Copyright (C) 2000  Edward A Williams <Ed_Williams@compuserve.com>
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

// #include	<stdlib.h>
#include <cmath>
#include "thgeomagdata.h"

#define	max(a,b)	(((a) > (b)) ? (a) : (b))

/*struct magfield_ {
double X, Y, Z;
};
magfield_ magfield;*/

#define nmax thgeomag_maxdeg
#define nmaxl thgeomag_maxdeg

#define pi 3.14159265358979
#define a 6378.137
#define b 6356.7523142
#define r_0 6371.2

double thgeomag(double lat, double lon, double h, double dat) {

  int n,m;

  double P[nmax+1][nmax+1];
  double DP[nmax+1][nmax+1];
  double gnm[nmax+1][nmax+1];
  double hnm[nmax+1][nmax+1];
  double sm[nmax+1];
  double cm[nmax+1];

  static double root[nmax+1];
  static double roots[nmax+1][nmax+1][2];


  double yearfrac,sr,r,theta,c,s,psi,fn,fn_0,B_r,B_theta,B_phi,X,Y,Z;
  double sinpsi, cospsi, inv_s;

  static int been_here = 0;

  double sinlat = sin(lat);
  double coslat = cos(lat);

  h = h / 1000;

  /* convert to geocentric */ 
  sr = sqrt(a*a*coslat*coslat + b*b*sinlat*sinlat);
  /* sr is effective radius */
  theta = atan2(coslat * (h*sr + a*a), sinlat * (h*sr + b*b));

  /* theta is geocentric co-latitude */

  r = h*h + 2.0*h * sr +
    (a*a*a*a - ( a*a*a*a - b*b*b*b ) * sinlat*sinlat ) / 
    (a*a - (a*a - b*b) * sinlat*sinlat );

  r = sqrt(r);

  /* r is geocentric radial distance */
  c = cos(theta);
  s = sin(theta);
  /* protect against zero divide at geographic poles */
  inv_s =  1.0 / (s + (s == 0.)*1.0e-8); 

  /*zero out arrays */
  for ( n = 0; n <= nmax; n++ ) {
    for ( m = 0; m <= n; m++ ) {
      P[n][m] = 0;
      DP[n][m] = 0;
    }
  }

  /* diagonal elements */
  P[0][0] = 1;
  P[1][1] = s;
  DP[0][0] = 0;
  DP[1][1] = c;
  P[1][0] = c ;
  DP[1][0] = -s;

  /* these values will not change for subsequent function calls */
  if( !been_here ) {
    for ( n = 2; n <= nmax; n++ ) {
      root[n] = sqrt((2.0*n-1) / (2.0*n));
    }

    for ( m = 0; m <= nmax; m++ ) {
      double mm = m*m;
      for ( n = max(m + 1, 2); n <= nmax; n++ ) {
        roots[m][n][0] = sqrt((n-1)*(n-1) - mm);
        roots[m][n][1] = 1.0 / sqrt( n*n - mm);
      }
    }
    been_here = 1;
  }

  for ( n=2; n <= nmax; n++ ) {
    /*  double root = sqrt((2.0*n-1) / (2.0*n)); */
    P[n][n] = P[n-1][n-1] * s * root[n];
    DP[n][n] = (DP[n-1][n-1] * s + P[n-1][n-1] * c) * root[n];
  }

  /* lower triangle */
  for ( m = 0; m <= nmax; m++ ) {
    /*  double mm = m*m;  */
    for ( n = max(m + 1, 2); n <= nmax; n++ ) {
      /* double root1 = sqrt((n-1)*(n-1) - mm); */
      /* double root2 = 1.0 / sqrt( n*n - mm);  */
      P[n][m] = (P[n-1][m] * c * (2.0*n-1) -
        P[n-2][m] * roots[m][n][0]) * roots[m][n][1];
      DP[n][m] = ((DP[n-1][m] * c - P[n-1][m] * s) *
        (2.0*n-1) - DP[n-2][m] * roots[m][n][0]) * roots[m][n][1];
    }
  }

  /* compute gnm, hnm at dat */

  int mindex = int((dat - thgeomag_minyear) / thgeomag_step);
  if (mindex < 0) mindex = 0;
  if (mindex > thgeomag_maxmindex) mindex = thgeomag_maxmindex;
  yearfrac = dat - thgeomag_step*mindex - thgeomag_minyear;

  for (n=1;n<=nmaxl;n++) {
    for (m = 0;m<=nmaxl;m++) {
      if (mindex == thgeomag_maxmindex) {
        gnm[n][m] = thgeomag_GNM[mindex][n][m] + yearfrac * thgeomag_GNMD[n][m];
        hnm[n][m] = thgeomag_HNM[mindex][n][m] + yearfrac * thgeomag_HNMD[n][m];
      } else {
        gnm[n][m] = thgeomag_GNM[mindex][n][m] + yearfrac / thgeomag_step * (thgeomag_GNM[mindex+1][n][m] - thgeomag_GNM[mindex][n][m]);
        hnm[n][m] = thgeomag_HNM[mindex][n][m] + yearfrac / thgeomag_step * (thgeomag_HNM[mindex+1][n][m] - thgeomag_HNM[mindex][n][m]);
      }
    }
  }

  /* compute sm (sin(m lon) and cm (cos(m lon)) */
  for (m = 0;m<=nmaxl;m++) {
    sm[m] = sin(m * lon);
    cm[m] = cos(m * lon);
  }

  /* compute B fields */
  B_r = 0.0;
  B_theta = 0.0;
  B_phi = 0.0;
  fn_0 = r_0/r;
  fn = fn_0 * fn_0;

  for ( n = 1; n <= nmaxl; n++ ) {
    double c1_n=0;
    double c2_n=0;
    double c3_n=0;
    for ( m = 0; m <= n; m++ ) {
      double tmp = (gnm[n][m] * cm[m] + hnm[n][m] * sm[m]); 
      c1_n += tmp * P[n][m];
      c2_n += tmp * DP[n][m];
      c3_n +=  m * (gnm[n][m] * sm[m] - hnm[n][m] * cm[m]) * P[n][m];
    }
    /* fn=pow(r_0/r,n+2.0);   */
    fn *= fn_0;
    B_r += (n + 1) * c1_n * fn;
    B_theta -= c2_n * fn;
    B_phi += c3_n * fn * inv_s;
  }



  /* Find geodetic field components: */
  psi = theta - (pi / 2.0 - lat);
  sinpsi = sin(psi);
  cospsi = cos(psi);
  X = -B_theta * cospsi - B_r * sinpsi;
  Y = B_phi;
  Z = B_theta * sinpsi - B_r * cospsi;

  /*    field[0]=B_r;
  field[1]=B_theta;
  field[2]=B_phi;
  field[3]=X;
  field[4]=Y;
  field[5]=Z;*/   /* output fields */
  /* find variation in radians */
  /* return zero variation at magnetic pole X=Y=0. */
  /* E is positive */

  /*    magfield.X = X;
  magfield.Y = Y;
  magfield.Z = Z; */

  return (X != 0. || Y != 0.) ? atan2(Y, X) * 180 / pi : (double) 0.; 
}

