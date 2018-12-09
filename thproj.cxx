/**
 * @file proj.cxx
 */
  
/* Copyright (C) 2006 Martin Budaj
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

#include "thexception.h"
#include <proj_api.h>
#include <string>
#include <cassert>

using namespace std;

void thcs2utm(string s, signed int zone,
              double a, double b, double c, double &x, double &y, double &z) {
  projPJ P1, P2;
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  char ch[50];
  sprintf(ch, "+proj=utm +datum=WGS84 +zone=%d", zone);
  if ((P2 = pj_init_plus(ch))==NULL) 
     therror(("Can't initialize default projection!"));
//  if (s.find("+proj=latlong") != s.npos) {
//    // prepocet na radiany tu alebo skoor?
//  }
  if (pj_transform(P1,P2,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
  x = a; 
  y = b;
  z = c;
  pj_free(P1);
  pj_free(P2);
}

void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z) {
  projPJ P1, P2;
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  if ((P2 = pj_init_plus(t.c_str()))==NULL) 
     therror(("Can't initialize output projection!"));
//  if (s.find("+proj=latlong") != s.npos) {
//    // prepocet na radiany tu alebo skoor?
//  }
//  assert (t.find("+proj=latlong") == t.npos);
  if (pj_transform(P1,P2,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
  x = a; 
  y = b;
  z = c;
  pj_free(P1);
  pj_free(P2);
}

signed int thcs2zone(string s, double a, double b, double c) {
  projPJ P1, P2;
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  if ((P2 = pj_init_plus("+proj=latlong +datum=WGS84"))==NULL) 
     therror(("Can't initialize default projection!"));
  if (pj_transform(P1,P2,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
  pj_free(P1);
  pj_free(P2);
  return (int) (a*180/3.1415926536+180)/6 + 1; 
}

double thcsconverg(string s, double a, double b) {
  projPJ P1, P2;
  double c = 0;
  double x = a, y = b;
  assert (s.find("+proj=latlong") == s.npos);
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  if ((P2 = pj_init_plus("+proj=latlong +datum=WGS84"))==NULL) 
     therror(("Can't initialize default projection!"));
//cout << a << " " << b << endl;
  if (pj_transform(P1,P2,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
  b += 1e-6;
  if (pj_transform(P2,P1,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
//cout << a << " " << b << endl;
  pj_free(P1);
  pj_free(P2);
  return atan2(a-x,b-y)/3.1415926536*180;
}


