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
#include "thlogfile.h"
#include <string>
#include <cmath>
#include <map>
#include <set>
#include <regex>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

using namespace std;

thcs_config::thcs_config() {
  proj_auto = false;
  proj_auto_grid = GRID_WARN;
}

thcs_config thcs_cfg;

#if PROJ_VER < 5

#include <proj_api.h>

void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z) {
  projPJ P1, P2;
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  if ((P2 = pj_init_plus(t.c_str()))==NULL) 
     therror(("Can't initialize output projection!"));
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
  thassert (s.find("+proj=latlong") == s.npos);
  if ((P1 = pj_init_plus(s.c_str()))==NULL) 
     therror(("Can't initialize input projection!"));
  if ((P2 = pj_init_plus("+proj=latlong +datum=WGS84"))==NULL) 
     therror(("Can't initialize default projection!"));
  if (pj_transform(P1,P2,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
  b += 1e-6;
  if (pj_transform(P2,P1,1,0,&a,&b,&c) != 0)
     therror(("Can't transform projections!"));
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

string thcs_get_proj_version() {
  string s = pj_get_release();
  smatch match;
  regex_search(s, match, regex(R"(\d+\.\d+\.\d+)"));
  return match[0];
}

string thcs_get_proj_version_headers() {
  ostringstream s;
  string ver = to_string(PJ_VERSION);
  for(int i = 0; i < 3; i++) {
    s << ver[i];
    if (i < 2) s << ".";
  }
  return s.str();
}

#else    // PROJ 5 and newer

  #include <proj.h>
  #include <math.h>
  #include <sstream>
  #include <iomanip>

#if PROJ_VER > 5
  regex reg_init(R"(^\+init=(epsg|esri):(\d+)$)");
  regex reg_epsg_ok(R"(^(epsg|esri):\d+$)");
  regex reg_type(R"(\+type\s*=\s*crs\b)");
  regex reg_space(R"(\s+)");
  regex reg_czech(R"(\s+\+czech\b)");
#endif

  string sanitize_crs(string s) {
#if PROJ_VER > 5
    s = regex_replace(s, reg_space, " ");
    if (thcs_get_proj_version() == "7.1.0") {  // fix a bug in axes order in 7.1.0 also for user-defined CSs
      s = regex_replace(s, reg_czech, " +axis=wsu");
    }
    if (regex_match(s,reg_epsg_ok)) return s;
    else if (regex_match(s,reg_init)) return regex_replace(s, reg_init, "$1:$2");   // get epsg:nnnn format
    else if (!regex_search(s,reg_type)) return s + " +type=crs";                    // add +type=crs to explicitly declare CRS
    else return s;
#else
    return s;
#endif
  }

#if PROJ_VER >= 6
class proj_cache {
    map<tuple<string,string,vector<double> >, PJ*> transf_cache;
    set<PJ*> PJ_cache;
  public:
    bool contains(PJ*);
    PJ* get(string,string,vector<double>);
    bool add(string,string,vector<double>, PJ*);
    string log();
    ~proj_cache();
};

PJ* proj_cache::get(string s, string t, vector<double> bbox) {
  tuple<string,string,vector<double> > tr_c {sanitize_crs(s), sanitize_crs(t), bbox};
  if (transf_cache.count(tr_c) == 1)
    return transf_cache[tr_c];
  else
    return nullptr;
}

bool proj_cache::add(string s, string t, vector<double> bbox, PJ* P) {
  tuple<string,string,vector<double> > tr_c {sanitize_crs(s), sanitize_crs(t), bbox};
  if (transf_cache.count(tr_c) == 1) {
    return false;
  } else {
    transf_cache[tr_c] = P;
    PJ_cache.insert(P);
    return true;
  }
}

bool proj_cache::contains(PJ* P) {
  return (PJ_cache.count(P) > 0);
}

string proj_cache::log() {
    if (transf_cache.size() == 0) return "";
    ostringstream s;
    s << setprecision(3) << std::fixed;
    s << endl << "############# CRS transformations chosen by PROJ ###############" << endl;
    if (thcs_cfg.bbox.size() == 4)
      s << "  Area of Use (AoU): (" << thcs_cfg.bbox[0] << ", " << thcs_cfg.bbox[1] << ") (" <<
                                     thcs_cfg.bbox[2] << ", " << thcs_cfg.bbox[3] << ")" << endl;
    for (const auto & i : transf_cache) {
      PJ_PROJ_INFO pinfo = proj_pj_info(i.second);
      s << "  from: [" << std::get<0>(i.first).c_str() <<
              "] to: [" << std::get<1>(i.first).c_str() <<
              "] AoU: [" << (std::get<2>(i.first).size()>0 ? "yes" : "no") <<
              "] transformation: [" << pinfo.description <<
              "] definition: [" << pinfo.definition <<
              "] accuracy: [" << pinfo.accuracy << " m]" << endl;
    }
    s << "########## end of CRS transformations chosen by PROJ ###########" << endl;
    return s.str();
}

proj_cache::~proj_cache() {
    for (const auto & i : PJ_cache) proj_destroy(i);
}

proj_cache cache;
#endif

  void th_init_proj(PJ * &P, string s) {
#if PROJ_VER > 5
    proj_context_use_proj4_init_rules(PJ_DEFAULT_CTX, true);
#endif
    P = proj_create(PJ_DEFAULT_CTX, s.c_str());
    if (P==0) {
      ostringstream u;
      u << "PROJ library: " << proj_errno(P) << " (" << proj_errno_string(proj_errno(P)) << "): " << s;
      proj_destroy(P);
      therror((u.str().c_str()));
    }
  }

#if PROJ_VER >= 6
  void th_init_proj_auto(PJ * &P, string s, string t) {

    // check the cache first
    if ((P = cache.get(s,t,thcs_cfg.bbox)) != nullptr) return;

    if
#if PROJ_VER >= 7
    (thcs_cfg.proj_auto_grid == GRID_CACHE && !proj_context_is_network_enabled(PJ_DEFAULT_CTX)) {
      if (!proj_context_set_enable_network(PJ_DEFAULT_CTX, 1)) {
        therror(("couldn't enable network access for Proj"));
      }
      proj_grid_cache_set_enable(PJ_DEFAULT_CTX, 1);
      thprintf("network access for Proj library enabled...\n");
    } else if
#endif
    (thcs_cfg.proj_auto_grid == GRID_WARN || thcs_cfg.proj_auto_grid == GRID_FAIL || thcs_cfg.proj_auto_grid == GRID_DOWNLOAD) {
      // start of missing grid detection
      PJ_OPERATION_FACTORY_CONTEXT *operation_factory_context = proj_create_operation_factory_context(PJ_DEFAULT_CTX, nullptr);
      // allow PROJ to find more potential transformations
      proj_operation_factory_context_set_spatial_criterion(PJ_DEFAULT_CTX, operation_factory_context, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
      // set area of interest
      if (thcs_cfg.bbox.size() == 4) {
        proj_operation_factory_context_set_area_of_interest(PJ_DEFAULT_CTX, operation_factory_context,
            thcs_cfg.bbox[0], thcs_cfg.bbox[1], thcs_cfg.bbox[2], thcs_cfg.bbox[3]);
      }
      // find if a grid is missing; see https://north-road.com/wp-content/uploads/2020/01/on_gda2020_proj6_and_qgis_lessons_learnt_and_recommendations.pdf
      proj_operation_factory_context_set_grid_availability_use(PJ_DEFAULT_CTX, operation_factory_context, PROJ_GRID_AVAILABILITY_IGNORED);
      PJ* P_1 = proj_create(PJ_DEFAULT_CTX, sanitize_crs(s).c_str());
      PJ* P_2 = proj_create(PJ_DEFAULT_CTX, sanitize_crs(t).c_str());
      PJ_OBJ_LIST *ops = proj_create_operations(PJ_DEFAULT_CTX, P_1, P_2, operation_factory_context);
      proj_destroy(P_1);
      proj_destroy(P_2);
      proj_operation_factory_context_destroy(operation_factory_context);
      const char * short_name, * url;
      if (proj_list_get_count(ops) < 1) {
        proj_list_destroy(ops);
        therror(("no usable coordinate transformation found"));
      }
      for (int i = 0; i < 1; i++) {   // let's look just at the first operation instead of up to proj_list_get_count(ops)
        PJ* P_tmp = proj_list_get(PJ_DEFAULT_CTX, ops, i);
  //      if (proj_coordoperation_has_ballpark_transformation(PJ_DEFAULT_CTX, P_tmp))
  //        therror(("no reasonably precise coordinate transformation found"));
        if (!proj_coordoperation_is_instantiable(PJ_DEFAULT_CTX, P_tmp)) {
          for (int j = 0; j < proj_coordoperation_get_grid_used_count(PJ_DEFAULT_CTX, P_tmp); j++) {
            proj_coordoperation_get_grid_used(PJ_DEFAULT_CTX, P_tmp, j,
                &short_name, nullptr, nullptr, &url, nullptr, nullptr, nullptr);
            string s_tmp = (string) "missing PROJ transformation grid '" + short_name + "'; you can download it from " +
                          url + " and install it to a location where PROJ finds it";
            switch (thcs_cfg.proj_auto_grid) {
              case GRID_WARN:
                thwarning((s_tmp.c_str()));
                break;
              case GRID_FAIL:
                proj_destroy(P_tmp);
                proj_list_destroy(ops);
                therror((s_tmp.c_str()));
                break;
#if PROJ_VER >= 7
              case GRID_DOWNLOAD:
                if (!proj_context_set_enable_network(PJ_DEFAULT_CTX, 1)) {
                  proj_destroy(P_tmp);
                  proj_list_destroy(ops);
                  therror(("couldn't enable network access for Proj"));
                }
                thprintf("downloading the grid %s... ", url);
                if (!proj_download_file(PJ_DEFAULT_CTX, url, 0, NULL, NULL)) {
                  proj_destroy(P_tmp);
                  proj_list_destroy(ops);
                  therror(("couldn't download the grid"));
                }
                if (proj_context_set_enable_network(PJ_DEFAULT_CTX, 0)) { // disable the network to prevent Proj from automatic caching
                  proj_destroy(P_tmp);
                  proj_list_destroy(ops);
                  therror(("couldn't disable network access for Proj"));
                }
                thprintf("done\n", url);
                break;
#endif
            }
          }
        }
        proj_destroy(P_tmp);
      }
      proj_list_destroy(ops);
      // end of missing grid handling
    }

    PJ_AREA* PA = proj_area_create();
    if (thcs_cfg.bbox.size() == 4) {
      proj_area_set_bbox(PA, thcs_cfg.bbox[0], thcs_cfg.bbox[1], thcs_cfg.bbox[2], thcs_cfg.bbox[3]);
    }
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, sanitize_crs(s).c_str(), sanitize_crs(t).c_str(), PA);
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

    if (!cache.add(s,t,thcs_cfg.bbox,P))
      therror(("could not add projection to the cache, it's already there -- should not happen"));
  }
#endif

  void thcs2cs(string s, string t,
              double a, double b, double c, double &x, double &y, double &z) {

    // TODO: support user-defined pipelines for a combination of CRSs
    // for high-precision transformations

    //  Proj (at least 5.2.0) doesn't accept custom proj strings in
    //  proj_create_crs_to_crs(); just +init=epsg:NNN and similar init strings
    //  PJ* P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, s.c_str(), t.c_str(), NULL);

    //  Proj v.6 supports them

    double undo_radians = 1.0, redo_radians = 1.0;
    PJ* P = NULL;
#if PROJ_VER > 5
    if (thcs_cfg.proj_auto) {  // let PROJ find the best transformation
      th_init_proj_auto(P, s, t);
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
#if PROJ_VER >= 6
    if (!cache.contains(P))   // cached Ps are destroyed in proj_cache's destructor
#endif
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

  string thcs_get_proj_version() {
    PJ_INFO info = proj_info();
    return string(info.version);
  }

  string thcs_get_proj_version_headers() {
    return to_string(PROJ_VERSION_MAJOR)+"."+to_string(PROJ_VERSION_MINOR)+"."+to_string(PROJ_VERSION_PATCH);
  }

#endif   // end of Proj 5+ branch

map<string,int> grid_map {
  {"ignore", GRID_IGNORE},
  {"warn", GRID_WARN},
  {"fail", GRID_FAIL},
  {"cache", GRID_CACHE},
  {"download", GRID_DOWNLOAD},
};
int thcs_parse_gridhandling(const char * s) {
  auto i = grid_map.find(string(s));
  if (i != grid_map.end()) {
    int res = i->second;
#if PROJ_VER < 7
    if (res == GRID_CACHE || res == GRID_DOWNLOAD)
      res = GRID_FAIL;
#endif
    return res;
  } else
    return GRID_INVALID;
}

void thcs_log_transf_used() {
#if PROJ_VER >= 6
  thlog.printf(cache.log().c_str());
#endif
}

string thcs_get_label(string s) {
#if PROJ_VER >= 6
    PJ* P;
    string res;
    th_init_proj(P, sanitize_crs(s));
    PJ_PROJ_INFO pinfo = proj_pj_info(P);
    res = (string) pinfo.description;
    proj_destroy(P);
    return res;
#else
    return "";
#endif
}
