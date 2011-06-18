/**
 * @file thdb3d.h
 * 3D processing class.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
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
 
#ifndef thdb3d_h
#define thdb3d_h

#include <list>
#include <stdio.h>
#include "loch/lxMath.h"

enum {
  THDB3DFC_POINTS,
  THDB3DFC_LINES,
  THDB3DFC_LINE_STRIP,
  THDB3DFC_LINE_LOOP,
  THDB3DFC_TRIANGLES,
  THDB3DFC_TRIANGLE_STRIP,
  THDB3DFC_TRIANGLE_FAN,
  THDB3DFC_QUADS,
  THDB3DFC_QUAD_STRIP,
  THDB3DFC_POLYGON,
};


struct thdb3dlim {
  bool ok;
  double minx, miny, minz,
    maxx, maxy, maxz;
  thdb3dlim() : ok(false), minx(0.0), miny(0.0), minz(0.0), 
    maxx(0.0), maxy(0.0), maxz(0.0) {}
  void update(double x, double y, double z);
  void update(thdb3dlim * limits);
};



struct thdb3dnm {
  double x, y, z;
  bool norm;
  thdb3dnm * next;
  thdb3dnm() : x(0.0), y(0.0), z(0.0), norm(false), next(NULL) {}
  void normalize();
};


struct thdb3dvx {
  unsigned long id;
  double x, y, z;
  void * data;
  thdb3dnm * normal;
  thdb3dvx * next;
  thdb3dvx() : id(0), x(0.0), y(0.0), z(0.0), 
    data(NULL), normal(NULL), next(NULL) {}
  lxVec get_vector() {return lxVec(this->x, this->y, this->z);}  
  thdb3dnm * insert_normal(double nx, double ny, double nz);
  thdb3dnm * insert_normal(lxVec n) {return this->insert_normal(n.x, n.y, n.z);};
  
};


struct thdb3dfx {
  void * data;
  thdb3dnm * normal;
  thdb3dvx * vertex;
  thdb3dfx * next;
  thdb3dfx() : data(NULL), normal(NULL), vertex(NULL), next(NULL) {}
  
  thdb3dnm * insert_normal(double nx, double ny, double nz);
  thdb3dnm * insert_normal(lxVec n) {return this->insert_normal(n.x, n.y, n.z);};
  
};


struct thdb3dfc {
  int type;
  unsigned long nvx;
  thdb3dfx * firstfx, * lastfx;  
  thdb3dfc * next;
  
  thdb3dfc() : type(THDB3DFC_TRIANGLES), nvx(0), firstfx(NULL), lastfx(NULL),
    next(NULL) {}
  
  thdb3dfx * insert_vertex(thdb3dvx * vx = NULL, void * dt = NULL);
  
};


struct thdb3ddata {

  unsigned long nvertices,
    nfaces;
  double exp_shift_x, exp_shift_y, exp_shift_z;
    
  thdb3dfc * firstfc, * lastfc;
  thdb3dvx * firstvx, * lastvx;
  thdb3ddata * next;
  
  thdb3dlim limits;
  
  thdb3ddata() : nvertices(0), nfaces(0),
    exp_shift_x(0.0), exp_shift_y(0.0), exp_shift_z(0.0),
    firstfc(NULL), lastfc(NULL), 
    firstvx(NULL), lastvx(NULL),
    next(NULL), limits() {}
    
  thdb3dfc * insert_face(int type);
  
  thdb3dvx * insert_vertex(double vxx, double vxy, double vxz, void * dt = NULL);
  thdb3dvx * insert_vertex(lxVec v, void * dt = NULL);
  
  void export_thm(FILE * out);
  void export_vrml(FILE * out);
  void export_3dmf(FILE * out);
  void export_dxf(FILE * out, const char * LAYER);
  
  void postprocess();
  
};


struct thdb3d {

  std::list <thdb3dnm> normal_list;
  std::list <thdb3dvx> vertex_list;
  std::list <thdb3dfx> face_vertex_list;
  std::list <thdb3dfc> face_list;

};


extern thdb3d thdatabase3d;

#endif


