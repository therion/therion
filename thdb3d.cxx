/**
 * @file thdb3d.cxx
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
 
#include "thdb3d.h"
#include <math.h>
 

void thdb3dlim::update(double x, double y, double z) {
  if (this->ok) {
    if (x < this->minx) this->minx = x;
    if (y < this->miny) this->miny = y;
    if (z < this->minz) this->minz = z;
    if (x > this->maxx) this->maxx = x;
    if (y > this->maxy) this->maxy = y;
    if (z > this->maxz) this->maxz = z;
  } else {
    this->minx = x;
    this->miny = y;
    this->minz = z;
    this->maxx = x;
    this->maxy = y;
    this->maxz = z;
    this->ok = true;
  }
}


void thdb3dlim::update(thdb3dlim * limits) {
  if (limits->ok) {
    this->update(limits->minx, limits->miny, limits->minz);
    this->update(limits->maxx, limits->maxy, limits->maxz);
  }
}

void thdb3dnm::normalize() {
  double nl;
  if (!this->norm) {
    nl = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    if (nl > 0.0) {
      this->x /= nl;
      this->y /= nl;
      this->z /= nl;
    } else {
      this->z = 1.0;
    }
    this->norm = true;
  }
}
 

thdb3dfc * thdb3ddata::insert_face(int type) {
  thdb3dfc * face;
  face = &(*thdatabase3d.face_list.insert(thdatabase3d.face_list.end(),thdb3dfc()));
  face->type = type;
  this->nfaces++;
  if (this->lastfc == NULL) {
    this->firstfc = face;
    this->lastfc = face;
  } else {
    this->lastfc->next = face;
    this->lastfc = face;
  }
  return face;
}
  
thdb3dvx * thdb3ddata::insert_vertex(double vxx, double vxy, double vxz, void * dt) {
  thdb3dvx * vertex;
  vertex = &(*thdatabase3d.vertex_list.insert(thdatabase3d.vertex_list.end(),thdb3dvx()));
  vertex->id = this->nvertices;
  vertex->x = vxx;
  vertex->y = vxy;
  vertex->z = vxz;
  this->limits.update(vxx, vxy, vxz);
  vertex->data = dt;
  this->nvertices++;
  if (this->lastvx == NULL) {
    this->firstvx = vertex;
    this->lastvx = vertex;
  } else {
    this->lastvx->next = vertex;
    this->lastvx = vertex;
  }
  return vertex;
}

thdb3dfx * thdb3dfc::insert_vertex(thdb3dvx * vx, void * dt) {
  thdb3dfx * fx;
  fx = &(*thdatabase3d.face_vertex_list.insert(thdatabase3d.face_vertex_list.end(),thdb3dfx()));
  this->nvx++;
  if (this->lastfx == NULL) {
    this->firstfx = fx;
    this->lastfx = fx;
  } else {
    this->lastfx->next = fx;
    this->lastfx = fx;
  }
  fx->vertex = vx;
  fx->data = dt;
  return fx;
}

thdb3dnm * thdb3dfx::insert_normal(double nx, double ny, double nz) {
  if (this->normal == NULL) {
    this->normal = &(*thdatabase3d.normal_list.insert(thdatabase3d.normal_list.end(),thdb3dnm()));
    this->normal->x = nx;
    this->normal->y = ny;
    this->normal->z = nz;
  } else {
    this->normal->x += nx;
    this->normal->y += ny;
    this->normal->z += nz;
  }
  return this->normal;
}

thdb3dnm * thdb3dvx::insert_normal(double nx, double ny, double nz) {
  if (this->normal == NULL) {
    this->normal = &(*thdatabase3d.normal_list.insert(thdatabase3d.normal_list.end(),thdb3dnm()));
    this->normal->x = nx;
    this->normal->y = ny;
    this->normal->z = nz;
  } else {
    this->normal->x += nx;
    this->normal->y += ny;
    this->normal->z += nz;
  }
  return this->normal;
}

void thdb3ddata::export_thm(FILE * out) {
  thdb3dfc * fc;
  thdb3dfx * fx;
  char * typestr;
  for (fc = this->firstfc; fc != NULL; fc = fc->next) {
    switch (fc->type) {
      case THDB3DFC_LINES:
        typestr = "GL_LINES";
        break;      
      case THDB3DFC_LINE_STRIP:
        typestr = "GL_LINE_STRIP";
        break;      
      case THDB3DFC_LINE_LOOP:
        typestr = "GL_LINE_LOOP";
        break;      
      case THDB3DFC_TRIANGLES:
        typestr = "GL_TRIANGLES";
        break;      
      case THDB3DFC_TRIANGLE_STRIP:
        typestr = "GL_TRIANGLE_STRIP";
        break;      
      case THDB3DFC_TRIANGLE_FAN:
        typestr = "GL_TRIANGLE_FAN";
        break;      
      case THDB3DFC_QUADS:
        typestr = "GL_QUADS";
        break;      
      case THDB3DFC_QUAD_STRIP:
        typestr = "GL_QUAD_STRIP";
        break;      
      case THDB3DFC_POLYGON:
        typestr = "GL_POLYGON";
        break;      
      default:
        typestr = "GL_POINTS";
        break;      
    }
    fprintf(out,"glBegin $GL::%s\n", typestr);
    for (fx = fc->firstfx; fx != NULL; fx = fx->next) {
      if (fx->vertex->normal != NULL) {
        fprintf(out,"glNormal3f %8.4f %8.4f %8.4f\n", 
          fx->vertex->normal->x, fx->vertex->normal->y, fx->vertex->normal->z);
      } else if (fx->normal != NULL) {
        fprintf(out,"glNormal3f %8.4f %8.4f %8.4f\n", 
          fx->normal->x, fx->normal->y, fx->normal->z);
      }
      fprintf(out,"glVertex3f %8.2f %8.2f %8.2f\n", 
        fx->vertex->x - this->exp_shift_x, 
        fx->vertex->y - this->exp_shift_y, 
        fx->vertex->z - this->exp_shift_z);
    }
    fprintf(out,"glEnd\n");
  }
}


void thdb3ddata::export_vrml(FILE * out) {
  thdb3dfc * fc;
  thdb3dfx * fx, * fx1, * fx2, * fx3;
  thdb3dvx * vx;
  bool reverse;  
  fprintf(out,"coord Coordinate {\n\tpoint [\n");
  for(vx = this->firstvx; vx != NULL; vx = vx->next)
    fprintf(out,"\t%8.2f %8.2f %8.2f,\n", 
      vx->x - this->exp_shift_x, 
      vx->y - this->exp_shift_y, 
      vx->z - this->exp_shift_z);
  fprintf(out,"\t]\n}\n");

  fprintf(out,"\tcoordIndex [\n");
  for (fc = this->firstfc; fc != NULL; fc = fc->next) {
    switch (fc->type) {
      case THDB3DFC_TRIANGLES:
        fx = fc->firstfx;
        while (fx != NULL) {
          fx1 = fx; fx = fx->next; fx2 = NULL; fx3 = NULL;
          if (fx != NULL) {
            fx2 = fx; fx = fx->next;
          }
          if (fx != NULL) {
            fx3 = fx; fx = fx->next;
          }
          if (fx3 != NULL) {
            fprintf(out,"\t%6ld,%6ld,%6ld,    -1,\n",
              fx1->vertex->id,
              fx2->vertex->id,
              fx3->vertex->id);
          }
        }
        break;      
      case THDB3DFC_TRIANGLE_STRIP:
        reverse = true;
        fx = fc->firstfx;
        if (fx != NULL) {
          fx2 = fx; fx = fx->next;
        }
        if (fx != NULL) {
          fx3 = fx; fx = fx->next;
        }
        while (fx != NULL) {
          fx1 = fx2;
          fx2 = fx3;
          fx3 = fx; 
          fx = fx->next;
          fprintf(out, "\t%6ld,%6ld,%6ld,    -1,\n",
            reverse ? fx1->vertex->id : fx2->vertex->id,
            reverse ? fx2->vertex->id : fx1->vertex->id,
            fx3->vertex->id);
          reverse = ! reverse;
        }
        break;      
      case THDB3DFC_TRIANGLE_FAN:
      case THDB3DFC_QUADS:
      case THDB3DFC_QUAD_STRIP:
      case THDB3DFC_POLYGON:
      case THDB3DFC_LINES:
      case THDB3DFC_LINE_STRIP:
      case THDB3DFC_LINE_LOOP:
      default:
        break;      
    }
  }
  fprintf(out,"\t]\n");
}

void thdb3ddata::export_3dmf(FILE * out) {
  thdb3dfc * fc;
  thdb3dfx * fx, * fx1, * fx2, * fx3;
  thdb3dvx * vx;
  bool reverse;
  
  fprintf(out,"\t%ld # nVertices\n", this->nvertices);
  for(vx = this->firstvx; vx != NULL; vx = vx->next)
    fprintf(out,"\t%.2f %.2f %.2f # %ld\n", 
      vx->x - this->exp_shift_x, 
      vx->y - this->exp_shift_y, 
      vx->z - this->exp_shift_z,
      vx->id);
  
  unsigned long nfaces = 0;
  for (fc = this->firstfc; fc != NULL; fc = fc->next) {
    switch (fc->type) {
      case THDB3DFC_TRIANGLES:
      nfaces += fc->nvx / 3;
      break;
      case THDB3DFC_TRIANGLE_STRIP:
      nfaces += fc->nvx - 2;
      break;
    }
  }
  
  fprintf(out,"\t%ld # nFaces\n\t0 # nContours\n", nfaces);
  nfaces = 0;
  for (fc = this->firstfc; fc != NULL; fc = fc->next) {
    switch (fc->type) {
      case THDB3DFC_TRIANGLES:
        fx = fc->firstfx;
        while (fx != NULL) {
          fx1 = fx; fx = fx->next; fx2 = NULL; fx3 = NULL;
          if (fx != NULL) {
            fx2 = fx; fx = fx->next;
          }
          if (fx != NULL) {
            fx3 = fx; fx = fx->next;
          }
          if (fx3 != NULL) {
            fprintf(out,"\t3 %ld %ld %ld # %ld\n",
              fx1->vertex->id,
              fx2->vertex->id,
              fx3->vertex->id,
              nfaces++);
          }
        }
        break;      
      case THDB3DFC_TRIANGLE_STRIP:
        fx = fc->firstfx;
        reverse = true;
        if (fx != NULL) {
          fx2 = fx; fx = fx->next;
        }
        if (fx != NULL) {
          fx3 = fx; fx = fx->next;
        }
        while (fx != NULL) {
          fx1 = fx2;
          fx2 = fx3;
          fx3 = fx; 
          fx = fx->next;
          fprintf(out,"\t3 %ld %ld %ld # %ld\n",
            reverse ? fx1->vertex->id : fx2->vertex->id,
            reverse ? fx2->vertex->id : fx1->vertex->id,
            fx3->vertex->id,
            nfaces++);
          reverse = ! reverse;
        }
        break;      
      case THDB3DFC_TRIANGLE_FAN:
      case THDB3DFC_QUADS:
      case THDB3DFC_QUAD_STRIP:
      case THDB3DFC_POLYGON:
      case THDB3DFC_LINES:
      case THDB3DFC_LINE_STRIP:
      case THDB3DFC_LINE_LOOP:
      default:
        break;      
    }
  }
}




void thdb3ddata::postprocess() {
  // znormalizuje vsetky normaly
  thdb3dvx * vx;
  thdb3dfx * fx;
  thdb3dfc * fc;
  for(vx = this->firstvx; vx != NULL; vx = vx->next) {
    if (vx->normal != NULL)
      vx->normal->normalize();
  }
  for(fc = this->firstfc; fc != NULL; fc = fc->next) {
    for(fx = fc->firstfx; fx != NULL; fx = fx->next) {
      if (fx->normal != NULL)
        fx->normal->normalize();
    }
  }
}


thdb3d thdatabase3d;


