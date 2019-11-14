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
#include "thproj.h"
#include <string>
#include <cassert>

using namespace std;

#if PROJ_VER < 5

#include <proj_api.h>

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

bool thcs_islatlong(string s) {
  projPJ P;
  if ((P = pj_init_plus(s.c_str()))==NULL)
     therror(("Can't initialize input projection!"));
  bool res = pj_is_latlong(P);
  pj_free(P);
  return res;
}

bool thcs_check(string s) {
  projPJ P;
  if ((P = pj_init_plus(s.c_str()))==NULL)
    ththrow(("invalid proj4 identifier -- %s", s.c_str()));
  pj_free(P);
  return true;
}

#else

  #include <proj.h>
  #include <math.h>
  #include <sstream>

  void th_init_proj(PJ * &P, string s) {
#if PROJ_VER > 5
    proj_context_use_proj4_init_rules(PJ_DEFAULT_CTX, true);
#endif
    P = proj_create(PJ_DEFAULT_CTX, s.c_str());
    if (P==0) {
      ostringstream u;
      u << "PROJ4 library: " << proj_errno(P);
      PJ_INFO info = proj_info();
      if (!(info.major==5 && info.minor==0))
          u << " (" << proj_errno_string(proj_errno(P)) << ")";
      proj_destroy(P);
      therror((u.str().c_str()));
    }
  }

  void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z) {
    // TODO: support user-defined pipelines for a combination of CRSs
    // for high-precision transformations

    //  Proj (at least 5.2.0) doesn't accept custom proj strings in
    //  proj_create_crs_to_crs(); just +init=epsg:NNN and similar init strings
    //  PJ* P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, s.c_str(), t.c_str(), NULL);

    ostringstream p;
    p << "+proj=pipeline +step +inv " << s.c_str() << " +step " << t.c_str();
    PJ* P = NULL;
    th_init_proj(P, p.str().c_str());
    PJ_COORD res;
    res = proj_trans(P, PJ_FWD, proj_coord(a, b, c, 0));
    x = res.xyz.x;
    y = res.xyz.y;
    z = res.xyz.z;
    proj_destroy(P);
  }

  void thcs2utm(string s, signed int zone,
              double a, double b, double c, double &x, double &y, double &z) {
    char ch[50];
    sprintf(ch, "+proj=utm +datum=WGS84 +zone=%d", zone);
    thcs2cs(s,ch,a,b,c,x,y,z);
  }

  signed int thcs2zone(string s, double a, double b, double c) {
    double x, y, z;
    thcs2cs(s,"+proj=latlong +datum=WGS84",a,b,c,x,y,z);
    return (int) (x*180/3.1415926536+180)/6 + 1;
  }

  double thcsconverg(string s, double a, double b) {
    double c = 0, x, y, z, x2, y2, z2;
    if (thcs_islatlong(s))
      therror(("can't determine meridian convergence for lat-long systems"));
    thcs2cs(s,"+proj=latlong +datum=WGS84",a,b,c,x,y,z);
    y += 1e-6;
    thcs2cs("+proj=latlong +datum=WGS84",s,x,y,z,x2,y2,z2);
    return atan2(x2-a,y2-b)/3.1415926536*180;
  }

  bool thcs_islatlong(string s) {
    PJ* P;
    th_init_proj(P, s.c_str());
    int angular = proj_angular_output(P,PJ_FWD);
    proj_destroy(P);
    return angular;
  }

  bool thcs_check(string s) {
    PJ* P;
    th_init_proj(P, s.c_str());
    proj_destroy(P);
    return true;
  }

#endif
