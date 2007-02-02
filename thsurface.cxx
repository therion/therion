/**
 * @file thsurface.cxx
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
 
#include "thsurface.h"
#include "thexception.h"
#include "thchenc.h"
#include "thmbuffer.h"
#include "thtflength.h"
#include "thdatabase.h"
#include "thcsdata.h"
#include <math.h>
#ifdef THMSVC
#define hypot _hypot
#endif
#include "thdatareader.h"
#include "thparse.h"
#include "thdb1d.h"
#include "thinfnan.h"
#ifdef THMSVC
#include <direct.h>
#define getcwd _getcwd
#endif

enum {
  TT_SURFACE_GFLIP_UNKNOWN,
  TT_SURFACE_GFLIP_VERTICAL,
  TT_SURFACE_GFLIP_HORIZONTAL,
  TT_SURFACE_GFLIP_NONE,
};
 
static const thstok thtt_surface_gflip[] = {
  {"horiz", TT_SURFACE_GFLIP_HORIZONTAL},
  {"horizontal", TT_SURFACE_GFLIP_HORIZONTAL},
  {"none", TT_SURFACE_GFLIP_NONE},
  {"vert", TT_SURFACE_GFLIP_VERTICAL},
  {"vertical", TT_SURFACE_GFLIP_VERTICAL},
  {NULL, TT_SURFACE_GFLIP_UNKNOWN},
};


thsurface::thsurface()
{
  // replace this by setting real properties initialization
  this->pict_name = NULL;
  this->pict_type = TT_IMG_TYPE_UNKNOWN;
  this->pict_stations = false;
  this->pict_X1 = thnan;
  this->pict_Y1 = thnan;
  this->pict_X2 = thnan;
  this->pict_Y2 = thnan;
  this->pict_x1 = thnan; 
  this->pict_y1 = thnan;
  this->pict_x2 = thnan;
  this->pict_y2 = thnan;
  this->grid_ox = thnan;
  this->grid_oy = thnan; 
  this->grid_dx = thnan;
  this->grid_dy = thnan;
  this->grid_nx = 0;
  this->grid_ny = 0;
  this->grid_size = 0;
  this->grid_counter = 0;
  this->calib_x = thnan;
  this->calib_y = thnan;
  this->calib_xx = thnan;
  this->calib_xy = thnan;
  this->calib_yx = thnan;
  this->calib_yy = thnan;
  this->calib_r = thnan;
  this->calib_s = thnan;
  this->grid = NULL;
  this->grid_flip = TT_SURFACE_GFLIP_NONE;
  
  this->s1.clear();
  this->s2.clear();
  this->d3dok = false;
    
}


thsurface::~thsurface()
{
  if (this->grid != NULL)
    delete [] this->grid;
}


int thsurface::get_class_id() 
{
  return TT_SURFACE_CMD;
}


bool thsurface::is(int class_id)
{
  if (class_id == TT_SURFACE_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thsurface::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 0;
}


char * thsurface::get_cmd_end()
{
  // insert endcommand if multiline command
  return "endsurface";
}


char * thsurface::get_cmd_name()
{
  // insert command name here
  return "surface";
}


thcmd_option_desc thsurface::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_surface_opt);
  if (id == TT_SURFACE_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else switch(id) {
    case TT_SURFACE_PICTURE:
      return thcmd_option_desc(id,2);
      break;
    case TT_SURFACE_GRID:
      return thcmd_option_desc(id,6);
      break;
    default:
      return thcmd_option_desc(id);
  }
}


void thsurface::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    // replace this by real properties setting
    case 0:
      this->parse_grid(args[0]);
      break;
      
    case TT_SURFACE_GRID:
      this->parse_grid_setup(args);
      break;
      
    case TT_SURFACE_GRID_UNITS:
      this->grid_units.parse_units(args[0]);
      break;
    
    case TT_SURFACE_GRID_FLIP:
      if (this->grid != NULL)
        ththrow(("grid-flip specification after grid data not allowed"));
      this->grid_flip = thmatch_token(args[0], thtt_surface_gflip);
      if (this->grid_flip == TT_SURFACE_GFLIP_UNKNOWN)
        ththrow(("unknown surface flip mode -- %s", args[0]))
      break;
      
    case TT_SURFACE_PICTURE:
      this->parse_picture(args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thsurface::self_delete()
{
  delete this;
}

void thsurface::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thsurface:\n");
  if (this->pict_name != NULL) {
    fprintf(outf,"\tpict_name:\t%s\n", this->pict_name);
  }
}


void thsurface::parse_picture(char ** args)
{

  thbuffer pict_path;
  pict_path.guarantee(1024);
  getcwd(pict_path.get_buffer(),1024);
  
  long i;
  if (strlen(args[0]) == 0)
    ththrow(("picture name not specified"))
  pict_path += "/";
  pict_path += thdb.csrc.name;
  char * pp = pict_path.get_buffer();
  for(i = (long)strlen(pp); i >= 0; i--) {
    if ((pp[i] == '/') || (pp[i] == '\\')) {
      break;
    } else
      pp[i] = 0;
  }
  if (strlen(pp) == 0)
    pict_path = "/";
  pict_path += args[0];
  pp = pict_path.get_buffer();
  for(i = (long)strlen(pp); i >= 0; i--) {
    if (pp[i] == '\\')
      pp[i] = '/';
  }
  this->pict_name = thdb.strstore(pp);
  // thprintf("\npict name: %s\n", this->pict_name);  
  
  this->pict_dpi = 300.0;
  this->pict_width = 1000;
  this->pict_height = 1000;

  // potom obrazok skontroluje, vytiahne jeho rozmery a DPI
  try {
    thparse_image(this->pict_name, this->pict_width, this->pict_height, this->pict_dpi, this->pict_type);
  } catch (...) {
    this->pict_name = NULL;
    threwarning(("%s [%d] -- error reading bitmap",
      thdbreader.get_cinf()->get_cif_name(),
      thdbreader.get_cinf()->get_cif_line_number()));
  }
  
  // potom parsne kalibraciu
  thmbuffer calib;
  thsplit_words(&calib, args[1]);
  thtflength ltr;
  int sv;
  long ncals = calib.get_size();
  char ** cals = calib.get_buffer();
  switch (ncals) {
    case 9:
      ltr.parse_units(cals[8]);
      ncals--;
    case 8:
      // parsne jednotlive cisla
#define surfpiccaldbl(XXX,YYY) \
      thparse_double(sv, this->XXX, cals[YYY]); \
      if (sv != TT_SV_NUMBER) \
        ththrow(("number expected -- %s", cals[YYY]))
      
      surfpiccaldbl(pict_X1,0);
      surfpiccaldbl(pict_Y1,1);
      this->convert_cs(cals[2], cals[3], this->pict_x1, this->pict_y1);
      if (this->cs == TTCS_LOCAL) {
        this->pict_x1 = ltr.transform(this->pict_x1);
        this->pict_y1 = ltr.transform(this->pict_y1);
      }
      surfpiccaldbl(pict_X2,4);
      surfpiccaldbl(pict_Y2,5);
      this->convert_cs(cals[6], cals[7], this->pict_x2, this->pict_y2);
      if (this->cs == TTCS_LOCAL) {
        this->pict_x2 = ltr.transform(this->pict_x2);
        this->pict_y2 = ltr.transform(this->pict_y2);
      }
      
      if (((this->pict_X1 == this->pict_X2) && (this->pict_Y1 == this->pict_Y2)) ||
          ((this->pict_x1 == this->pict_x2) && (this->pict_y1 == this->pict_y2))) {
        ththrow(("duplicate points in picture calibration"));
      }
      break;
    
    case 6:
      surfpiccaldbl(pict_X1,0);
      surfpiccaldbl(pict_Y1,1);
      surfpiccaldbl(pict_X2,3);
      surfpiccaldbl(pict_Y2,4);
      thparse_objectname(this->s1, &(thdb.buff_stations), cals[2]);
      thparse_objectname(this->s2, &(thdb.buff_stations), cals[5]);
      this->ssurvey = thdb.get_current_survey();
      if ((this->pict_X1 == this->pict_X2) && (this->pict_Y1 == this->pict_Y2)) {
        ththrow(("duplicate points in picture calibration"));
      }
      break;

    default:
      ththrow(("invalid number of picture calibration arguments -- %d", ncals))
  }

}


void thsurface::calibrate() {
  // spocita origin, scale (pri 300 dpi) a rotaciu
  double olen, nlen, scale, ang, tang;
  olen = hypot(this->pict_X2 - this->pict_X1, 
                this->pict_Y2 - this->pict_Y1);
  nlen = hypot(this->pict_x2 - this->pict_x1, 
                this->pict_y2 - this->pict_y1);
  ang = atan2(this->pict_Y2 - this->pict_Y1, this->pict_X2 - this->pict_X1);
  tang = atan2(this->pict_y2 - this->pict_y1, this->pict_x2 - this->pict_x1);
  tang -= ang;
  this->calib_xx = cos(tang);
  this->calib_xy = -sin(tang);
  this->calib_yx = sin(tang);
  this->calib_yy = cos(tang);
  scale = nlen / olen;
  this->calib_s = scale;
  this->calib_r = - tang / 3.14159265358 * 180.0;
  this->calib_xx *= scale;
  this->calib_yy *= scale;
  this->calib_xy *= scale;
  this->calib_yx *= scale;
  this->calib_x = this->pict_x1 - this->calib_xx * pict_X1
                              - this->calib_xy * pict_Y1;
  this->calib_y = this->pict_y1 - this->calib_yx * pict_X1
                              - this->calib_yy * pict_Y1;
}


void thsurface::check_stations() 
{
  if (!this->s1.is_empty()) {
    
    thdb1ds * ds1, * ds2;
    
    // najde stations, error ak nie
    this->s1.id = thdb.db1d.get_station_id(this->s1, this->ssurvey);
    if (this->s1.id == 0) {
      this->throw_source();
      if (this->s1.survey == NULL)
        threthrow2(("station doesn't exist -- %s", this->s1.name))
      else
        threthrow2(("station doesn't exist -- %s@%s", this->s1.name, this->s1.survey))
    }
    
    this->s2.id = thdb.db1d.get_station_id(this->s2, this->ssurvey);
    if (this->s2.id == 0) {
      this->throw_source();
      if (this->s2.survey == NULL)
        threthrow2(("station doesn't exist -- %s", this->s2.name))
      else
        threthrow2(("station doesn't exist -- %s@%s", this->s2.name, this->s2.survey))
    }
    
    // priradi si x a y a skontroluje ci su roozne
    ds1 = &(thdb.db1d.station_vec[this->s1.id - 1]);
    ds2 = &(thdb.db1d.station_vec[this->s2.id - 1]);
    this->pict_x1 = ds1->x;
    this->pict_y1 = ds1->y;
    this->pict_x2 = ds2->x;
    this->pict_y2 = ds2->y;
    if ((this->pict_x1 == this->pict_x2) && (this->pict_y1 == this->pict_y2)) {
      this->throw_source();
      threthrow2(("duplicate points in picture calibration"));
    }
  }
}


void thsurface::parse_grid_setup(char ** args)
{
  if (this->grid != NULL)
    ththrow(("grid specification after grid data not allowed"));

  // nacitame vsetky premenne ktore treba
  double dblv;
  int sv;

#define parsedbl(XXX,YYY) \
      thparse_double(sv, dblv, args[YYY]); \
      if (sv != TT_SV_NUMBER) \
        ththrow(("number expected -- %s", args[YYY])) \
      XXX = this->grid_units.transform(dblv);
#define parsenum(XXX,YYY) \
      thparse_double(sv, dblv, args[YYY]); \
      if (sv != TT_SV_NUMBER) \
        ththrow(("number expected -- %s", args[YYY])) \
      if (dblv <= 0) \
        ththrow(("positive number expected -- %s", args[YYY])) \
      if (dblv != double(long(dblv))) \
        ththrow(("integer expected -- %s", args[YYY])) \
      XXX = long(dblv);
  
  this->convert_cs(args[0], args[1], this->grid_ox, this->grid_oy);
  if (this->cs == TTCS_LOCAL) {
    this->grid_ox = this->grid_units.transform(this->grid_ox);
    this->grid_oy = this->grid_units.transform(this->grid_oy);
  }

  parsedbl(this->grid_dx,2);
  if (this->grid_dx == 0.0)
    ththrow(("non-zero number expected -- %s", args[2]));
  parsedbl(this->grid_dy,3);
  if (this->grid_dy == 0.0)
    ththrow(("non-zero number expected -- %s", args[3]));
  parsenum(this->grid_nx,4);
  if (this->grid_nx < 2)
    ththrow(("number > 1 expected -- %s", args[4]));
  parsenum(this->grid_ny,5);
  if (this->grid_ny < 2)
    ththrow(("number > 1 expected -- %s", args[5]));
}

void thsurface::parse_grid(char * spec)
{
  if (this->grid_nx == 0)
    ththrow(("grid dimensions not specified"));
  if (this->grid == NULL) {
    this->grid_counter = 0;
    this->grid_size = this->grid_nx * this->grid_ny;
    this->grid = new double [this->grid_size];
  }
  
  // rozdelime na argumenty
  thsplit_args(&thdb.mbuff_tmp, spec);
  
  // kazdy parsneme ako cislo a zapiseme do pola  
  long i, ni = thdb.mbuff_tmp.get_size();
  long x, y;
  int sv;
  double alt;
  char ** heights = thdb.mbuff_tmp.get_buffer();
  for(i = 0; i < ni; i++) {
    thparse_double(sv, alt, heights[i]);
    if (sv != TT_SV_NUMBER)
      ththrow(("number expected -- %s", heights[i]))
    if (this->grid_counter == this->grid_size)
      ththrow(("too many grid data"))
    x = this->grid_counter % this->grid_nx;
    y = this->grid_ny - (this->grid_counter / this->grid_nx) - 1;
    switch (this->grid_flip) {
      case TT_SURFACE_GFLIP_VERTICAL:
        y = this->grid_ny - y - 1;
        break;
      case TT_SURFACE_GFLIP_HORIZONTAL:
        x = this->grid_nx - x - 1;
        break;
    }
    this->grid[y * this->grid_nx + x] = this->grid_units.transform(alt);
    this->grid_counter++;
  }
}

void thsurface::start_insert() {
  if (this->grid_counter < this->grid_size)
    ththrow(("missing grid data"))
}

thdb3ddata * thsurface::get_3d() {

  if (this->grid_size == 0)
    return NULL;
  if (this->d3dok)
    return &(this->d3d);

  // vytvorime 3d data
  long i, j;
  double nx, ny, nz, nl, nt;
  thdb3dvx ** surfvx = new thdb3dvx* [this->grid_size],
   * pvx, * cvx;
  
#define grd(I, J) (this->grid_nx * (J) + (I))
  for(i = 0; i < this->grid_nx; i++)
    for(j = 0; j < this->grid_ny; j++)
      surfvx[grd(i,j)] = this->d3d.insert_vertex(
        this->grid_ox + i * this->grid_dx,
        this->grid_oy + j * this->grid_dy,
        this->grid[grd(i,j)]
        );

  // zratame normaly
  for(i = 0; i < this->grid_nx; i++)
    for(j = 0; j < this->grid_ny; j++) {
      // TODO - vlozit vsetky prilahle
      if (i > 0) {
        pvx = surfvx[grd(i-1,j)];
        cvx = surfvx[grd(i,j)];
        nx = pvx->x - cvx->x;
        ny = pvx->y - cvx->y;
        nz = pvx->z - cvx->z;
        nl = hypot(nz, hypot(nx, ny));
        nx /= nl; ny /= nl; nz /= nl;
        nt = nx; nx = nz; nz = -nt;
        cvx->insert_normal(nx, ny, nz);
      }
      if (i < (this->grid_nx - 1)) {
        pvx = surfvx[grd(i+1,j)];
        cvx = surfvx[grd(i,j)];
        nx = - pvx->x + cvx->x;
        ny = - pvx->y + cvx->y;
        nz = - pvx->z + cvx->z;
        nl = hypot(nz, hypot(nx, ny));
        nx /= nl; ny /= nl; nz /= nl;
        nt = nx; nx = nz; nz = -nt;
        cvx->insert_normal(nx, ny, nz);
      }
      if (j > 0) {
        pvx = surfvx[grd(i,j-1)];
        cvx = surfvx[grd(i,j)];
        nx = pvx->x - cvx->x;
        ny = pvx->y - cvx->y;
        nz = pvx->z - cvx->z;
        nl = hypot(nz, hypot(nx, ny));
        nx /= nl; ny /= nl; nz /= nl;
        nt = ny; ny = nz; nz = -nt;
        cvx->insert_normal(nx, ny, nz);
      }
      if (j < (this->grid_ny - 1)) {
        pvx = surfvx[grd(i,j+1)];
        cvx = surfvx[grd(i,j)];
        nx = - pvx->x + cvx->x;
        ny = - pvx->y + cvx->y;
        nz = - pvx->z + cvx->z;
        nl = hypot(nz, hypot(nx, ny));
        nx /= nl; ny /= nl; nz /= nl;
        nt = ny; ny = nz; nz = -nt;
        cvx->insert_normal(nx, ny, nz);
      }
    }

  // povklada trojuholniky
  thdb3dfc * fc;
  for(i = 0; i < (this->grid_nx - 1); i++) {
    fc = this->d3d.insert_face(THDB3DFC_TRIANGLE_STRIP);
    for(j = 0; j < this->grid_ny; j++) {
      fc->insert_vertex(surfvx[grd(i,j)]);
      fc->insert_vertex(surfvx[grd(i+1,j)]);
    }
  }
  
  delete [] surfvx;
  this->d3dok = true;
  this->d3d.postprocess();
  return &(this->d3d);    

}




