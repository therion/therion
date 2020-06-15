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
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

using namespace std;

#if PROJ_VER < 5

#include <proj_api.h>

/*void thcs2utm(string s, signed int zone,
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
} */

void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z, double unused1[], bool unused2) {
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
  return (int) (a*180/M_PI+180)/6 + 1; 
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
  return atan2(a-x,b-y)/M_PI*180;
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
  #include <regex>
  #include "thinit.h"

  string sanitize_crs(string s) {
#if PROJ_VER > 5
    regex reg_init(R"(^\+init=(epsg|esri):(\d+)$)");
    regex reg_epsg_ok(R"(^(epsg|esri):\d+$)");
    regex reg_type(R"(\+type\s*=\s*crs\b)");
    if (regex_match(s,reg_epsg_ok)) return s;
    else if (regex_match(s,reg_init)) return regex_replace(s, reg_init, "$1:$2");   // get epsg:nnnn format
    else if (!regex_search(s,reg_type)) return s + " +type=crs";                    // add +type=crs to explicitly declare CRS
    else return s;
#else
    return s;
#endif
  }

  void th_init_proj(PJ * &P, string s) {
#if PROJ_VER > 5
    proj_context_use_proj4_init_rules(PJ_DEFAULT_CTX, true);
#endif
    P = proj_create(PJ_DEFAULT_CTX, s.c_str());
    if (P==0) {
      ostringstream u;
      u << "PROJ library: " << proj_errno(P);
      PJ_INFO info = proj_info();
      if (!(info.major==5 && info.minor==0))
          u << " (" << proj_errno_string(proj_errno(P)) << ")";
      u << ": " << s;
      proj_destroy(P);
      therror((u.str().c_str()));
    }
  }

#if PROJ_VER > 5
//#include <iostream>

  void th_init_proj_auto(PJ * &P, string s, string t, double bbox[]) {

    PJ_OPERATION_FACTORY_CONTEXT *operation_factory_context = proj_create_operation_factory_context(PJ_DEFAULT_CTX, nullptr);
    // allow PROJ to find more potential transformations
    proj_operation_factory_context_set_spatial_criterion(PJ_DEFAULT_CTX, operation_factory_context, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
    // set area of interest
    if (bbox != nullptr) {
      proj_operation_factory_context_set_area_of_interest(PJ_DEFAULT_CTX, operation_factory_context, 
           bbox[0], bbox[1], bbox[2], bbox[3]);
    }
    // find if a grid is missing; see https://north-road.com/wp-content/uploads/2020/01/on_gda2020_proj6_and_qgis_lessons_learnt_and_recommendations.pdf
    proj_operation_factory_context_set_grid_availability_use(PJ_DEFAULT_CTX, operation_factory_context, PROJ_GRID_AVAILABILITY_IGNORED);
    PJ_OBJ_LIST *ops = proj_create_operations(PJ_DEFAULT_CTX,
         proj_create(PJ_DEFAULT_CTX, sanitize_crs(s).c_str()),
         proj_create(PJ_DEFAULT_CTX, sanitize_crs(t).c_str()), operation_factory_context);
    const char * short_name, * url;
    int c5 = 0, c6 = 0, c7 = 0;
    int proj_auto_grid = GRID_WARN;  // TODO

    if (proj_list_get_count(ops) < 1)
      therror(("no usable coordinate transformation found"));

//    for (int i = 0; i < proj_list_get_count(ops); i++) {
    for (int i = 0; i < 1; i++) {   // let's look just at the first operation
      PJ* P_tmp = proj_list_get(PJ_DEFAULT_CTX, ops, i);
      if (proj_coordoperation_has_ballpark_transformation(PJ_DEFAULT_CTX, P_tmp))
        therror(("no reasonably precise coordinate transformation found"));
//      PJ_PROJ_INFO pinfo = proj_pj_info(P_tmp);
//      cout << endl << "i " << i << " " << pinfo.description << " " << pinfo.definition << " [" << pinfo.accuracy << "m]" << endl;
      if (!proj_coordoperation_is_instantiable(PJ_DEFAULT_CTX, P_tmp)) {
        for (int j = 0; j < proj_coordoperation_get_grid_used_count(PJ_DEFAULT_CTX, P_tmp); j++) {
          proj_coordoperation_get_grid_used(PJ_DEFAULT_CTX, P_tmp, j, 
              &short_name, nullptr, nullptr, &url, &c5, &c6, &c7);
//          cout << " j " << j << "  grid: 1=" << short_name << " 4="<< url << " 5="<< c5 
//                   << " 6="<< c6 << " 7=" << c7 << endl;
          string s_tmp = (string) "Missing PROJ transformation grid '" + short_name + "'; you can download it from " + 
                         url + " and install it to a location where PROJ finds it\n";
          switch (proj_auto_grid) {
            case GRID_WARN:
              thwarning((s_tmp.c_str()));
              break;
            case GRID_FAIL:
              therror((s_tmp.c_str()));
              break;
            case GRID_DOWNLOAD:
              therror((s_tmp.c_str()));  // TODO
              break;
          }
        }
      }

      proj_destroy(P_tmp);
    }
    // end of grid handling

    PJ_AREA* PA = proj_area_create();
    if (bbox != nullptr) {
      proj_area_set_bbox(PA, bbox[0], bbox[1], bbox[2], bbox[3]);
    }
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, sanitize_crs(s).c_str(), sanitize_crs(t).c_str(), PA);
    proj_operation_factory_context_destroy(operation_factory_context);
    proj_area_destroy(PA);
    if (P==0) {
      ostringstream u;
      u << "PROJ library: " << proj_errno(P) << " (" << proj_errno_string(proj_errno(P)) << ")";
      proj_destroy(P);
      therror((u.str().c_str()));
    }
    PJ* P_for_GIS = proj_normalize_for_visualization(PJ_DEFAULT_CTX, P);
    if( 0 == P_for_GIS )  {
      ostringstream u;
      u << "PROJ library: " << proj_errno(P_for_GIS) << " (" << proj_errno_string(proj_errno(P_for_GIS)) << ")";
      proj_destroy(P);
      proj_destroy(P_for_GIS);
      therror((u.str().c_str()));
    }
    proj_destroy(P);
    P = P_for_GIS;
  }
#endif

  void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z, 
              double bbox[], bool proj_auto) {
              // proj_auto is used for automated tests and pressuposes that proj-auto in the init file is false

    // TODO: support user-defined pipelines for a combination of CRSs
    // for high-precision transformations

    //  Proj (at least 5.2.0) doesn't accept custom proj strings in
    //  proj_create_crs_to_crs(); just +init=epsg:NNN and similar init strings
    //  PJ* P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, s.c_str(), t.c_str(), NULL);

    //  Proj v.6 supports them

    double undo_radians = 1.0, redo_radians = 1.0;
    PJ* P = NULL;
#if PROJ_VER > 5
    if (proj_auto || thini.get_proj_auto()) {  // let PROJ find the best transformation
      th_init_proj_auto(P, s, t, bbox);
      if (thcs_islatlong(s) && !proj_angular_input(P, PJ_FWD)) {
        undo_radians = 180.0 / M_PI;
      }
      if (thcs_islatlong(t) && !proj_angular_output(P, PJ_FWD)) {
        redo_radians = M_PI / 180.0;
      }
    } else {
#endif
      th_init_proj(P, (string("+proj=pipeline +step +inv ") + s + " +step " + t).c_str());
#if PROJ_VER > 5
    }
#endif
    PJ_COORD res;
    res = proj_trans(P, PJ_FWD, proj_coord(a*undo_radians, b*undo_radians, c, 0));
    x = res.xyz.x*redo_radians;
    y = res.xyz.y*redo_radians;
    z = res.xyz.z;
    proj_destroy(P);
  }

  signed int thcs2zone(string s, double a, double b, double c) {
    double x, y, z;
    thcs2cs(s,"+proj=latlong +datum=WGS84",a,b,c,x,y,z);
    return (int) (x*180/M_PI+180)/6 + 1;
  }

  double thcsconverg(string s, double a, double b) {
    double c = 0, x, y, z, x2, y2, z2;
    if (thcs_islatlong(s))
      therror(("can't determine meridian convergence for lat-long systems"));
    thcs2cs(s,"+proj=latlong +datum=WGS84",a,b,c,x,y,z);
    y += 1e-6;
    thcs2cs("+proj=latlong +datum=WGS84",s,x,y,z,x2,y2,z2);
    return atan2(x2-a,y2-b)/M_PI*180;
  }

  bool thcs_islatlong(string s) {
    PJ* P;
    th_init_proj(P, sanitize_crs(s).c_str());
#if PROJ_VER > 5
    int type = proj_get_type(P);
    bool angular = (type == PJ_TYPE_GEOGRAPHIC_CRS || type == PJ_TYPE_GEOGRAPHIC_2D_CRS || type == PJ_TYPE_GEOGRAPHIC_3D_CRS);
#else
    bool angular = proj_angular_output(P,PJ_FWD);  // in PROJ v6 it returns just that input is in radians
#endif
    proj_destroy(P);
    return angular;
  }

  bool thcs_check(string s) {
    PJ* P;
    th_init_proj(P, s);
    proj_destroy(P);
    return true;
  }

#endif
