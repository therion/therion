/**
 * @file thexpmodel.cxx
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
 
#include "thexpmodel.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdb1d.h"
#include "thdata.h"
#include "thsurvey.h"
#include <stdio.h>
#include "extern/img.h"
#include "thchenc.h"
#include "thscrap.h"
#include <map>
#include "thsurface.h"
#include "loch/lxFile.h"
#include "thsurface.h"
#include "thchenc.h"
#include "thconfig.h"
#include "thcsdata.h"
#include "thproj.h"
#include "thcs.h"
#include "thtexfonts.h"
#include "thlang.h"


thexpmodel::thexpmodel() {
  this->format = TT_EXPMODEL_FMT_UNKNOWN;
  this->items = TT_EXPMODEL_ITEM_ALL;
  this->wallsrc = TT_WSRC_ALL;
  this->encoding = TT_UTF_8;
}

thexpmodel::~thexpmodel() {
}

void thexpmodel::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_expmodel_opt);
  unsigned utmp;
  int optx = argx;
  switch (optid) {

    case TT_EXPMODEL_OPT_FORMAT:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing format -- \"%s\"",args[optx]))
      this->format = thmatch_token(args[argx], thtt_expmodel_fmt);
      if (this->format == TT_EXPMODEL_FMT_UNKNOWN)
        ththrow(("unknown format -- \"%s\"", args[argx]))
      argx++;
      break;

    case TT_EXPMODEL_OPT_ENCODING:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing encoding -- \"%s\"",args[optx]))
      this->encoding = thmatch_token(args[argx], thtt_encoding);
      if (this->encoding == TT_UNKNOWN_ENCODING)
        ththrow(("unknown encoding -- \"%s\"", args[argx]))
      argx++;
      break;

    case TT_EXPMODEL_OPT_ENABLE:
    case TT_EXPMODEL_OPT_DISABLE:
      argx++;
      if (argx >= nargs)
        ththrow(("missing model entity -- \"%s\"",args[optx]))
      utmp = thmatch_token(args[argx], thtt_expmodel_items);
      if (utmp == TT_EXPMODEL_ITEM_UNKNOWN)
        ththrow(("unknown model entity -- \"%s\"", args[argx]))
      if (optid == TT_EXPMODEL_OPT_ENABLE) {
        this->items |= utmp;
      } else {
        this->items &= (~utmp);
      }
      argx++;
      break;

    case TT_EXPMODEL_OPT_WALLSRC:
      argx++;
      if (argx >= nargs)
        ththrow(("missing wall source -- \"%s\"",args[optx]))
      utmp = thmatch_token(args[argx], thtt_expmodel_wallsrc);
      if (utmp == TT_WSRC_UNKNOWN)
        ththrow(("unknown wall source -- \"%s\"", args[argx]))
      this->wallsrc = utmp;
      argx++;
      break;

    default:
      thexport::parse_options(argx, nargs, args);
      break;
  }
}

void thexpmodel::dump_header(FILE * xf)
{
  thexport::dump_header(xf);
  //fprintf(xf,"export\tmodel");
}


bool thexpmodel::is_leg_exported(thdb1dl * l)
{
  if (!l->survey->is_selected()) return false;
  if (((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) == 0) && ((l->leg->flags & TT_LEGFLAG_SPLAY) != 0))
    return false;
  if (((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) == 0) && ((l->leg->flags & TT_LEGFLAG_SURFACE) != 0))
    return false;
  if (((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) == 0) && (!((l->leg->flags & TT_LEGFLAG_SURFACE) != 0)))
    return false;
  return true;
}


void thexpmodel::dump_body(FILE * xf)
{
  thexport::dump_body(xf);
  if (this->format != TT_EXPMODEL_FMT_UNKNOWN)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expmodel_fmt));
}


void thexpmodel::process_db(class thdatabase * dbp) 
{
 this->db = dbp;
 if (this->format == TT_EXPMODEL_FMT_UNKNOWN) {
    this->format = TT_EXPMODEL_FMT_LOCH;
    thexp_set_ext_fmt(".plt", TT_EXPMODEL_FMT_COMPASS)
    thexp_set_ext_fmt(".3d", TT_EXPMODEL_FMT_SURVEX)
    thexp_set_ext_fmt(".wrl", TT_EXPMODEL_FMT_VRML)
    thexp_set_ext_fmt(".vrml", TT_EXPMODEL_FMT_VRML)
    thexp_set_ext_fmt(".3dmf", TT_EXPMODEL_FMT_3DMF)
    thexp_set_ext_fmt(".dxf", TT_EXPMODEL_FMT_DXF)
    thexp_set_ext_fmt(".lox", TT_EXPMODEL_FMT_LOCH)
    thexp_set_ext_fmt(".shp", TT_EXPMODEL_FMT_SHP)
    thexp_set_ext_fmt(".kml", TT_EXPMODEL_FMT_KML)
  }  
  switch (this->format) {
    case TT_EXPMODEL_FMT_LOCH:
      this->export_lox_file(dbp);
      break;
    case TT_EXPMODEL_FMT_SURVEX:
      this->export_3d_file(dbp);
      break;
    case TT_EXPMODEL_FMT_COMPASS:
      this->export_plt_file(dbp);
      break;
    //case TT_EXPMODEL_FMT_THERION:
    //  this->export_thm_file(dbp);
    //  break;
    case TT_EXPMODEL_FMT_3DMF:
      this->export_3dmf_file(dbp);
      break;
    case TT_EXPMODEL_FMT_VRML:
      this->export_vrml_file(dbp);
      break;
    case TT_EXPMODEL_FMT_DXF:
      this->export_dxf_file(dbp);
      break;
    case TT_EXPMODEL_FMT_SHP:
      this->export_shp_file(dbp);
      break;
    case TT_EXPMODEL_FMT_KML:
      this->export_kml_file(dbp);
      break;
  }
}


void thexpmodel::export_3d_file(class thdatabase * dbp)
{

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = (unsigned long)dbp->db1d.station_vec.size();
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();

  if (nlegs == 0)
    return;
  
  const char * fnm = this->get_output("cave.3d");
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  unsigned long i;
  img * pimg;
  img_output_version = 8;
  thbuffer fnmb;
  #ifdef THWIN32
    char title[_MAX_FNAME];
    _splitpath(this->outpt, NULL, NULL, title, NULL);
  #elif THLINUX
    thbuffer bnb;
    bnb.strcpy(this->outpt);
    const char * title = basename(bnb.get_buffer());
  #else
    const char * title = "cave";
  #endif
  fnmb.strcpy(title);  // VG 290316: Set the filename as a cave name instead of "cave". The top-level survey name will be even better
  if ((thcfg.outcs >= 0) || (thcfg.outcs < TTCS_UNKNOWN))  // Export the coordinate system data if one is set
    pimg = img_open_write_cs(fnm, fnmb.get_buffer(), thcs_get_params(thcfg.outcs).c_str(), 1);
  else
    pimg = img_open_write(fnm, fnmb.get_buffer(), 1);
     
  if (!pimg) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  unsigned long last_st = nstat, cur_st, cnlegs = 0;
  bool check_traverses = (dbp->db1d.traverse_list.size() > 0);
  thdb1d_loop * last_loop = NULL;
  thdb1d_traverse * last_traverse = NULL;
  int * s_exp = new int [nstat], * cis_exp, leg_flag, x_exp;
  cis_exp = s_exp; 
  bool is_surface, is_duplicate, is_splay, newtraverse;
  for(i = 0; i < nstat; i++, *cis_exp = 0, cis_exp++);
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
//      thprintf("EXP: %s@%s - %s@%s\n", (*tlegs)->leg->from.name, dbp->db1d.station_vec[(*tlegs)->leg->from.id -1].survey->full_name,  (*tlegs)->leg->to.name, dbp->db1d.station_vec[(*tlegs)->leg->to.id -1].survey->full_name);
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      is_surface = (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0);
      is_duplicate = (((*tlegs)->leg->flags & TT_LEGFLAG_DUPLICATE) != 0);
      is_splay = (((*tlegs)->leg->flags & TT_LEGFLAG_SPLAY) != 0);

      if (((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) == 0) && is_splay)
        continue;
      if (((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) == 0) && is_surface)
        continue;
      if (((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) == 0) && (!is_surface))
        continue;

      // Set start and end date of the survey
      if ((*tlegs)->data->date.syear != -1) {
        pimg->days1 = (*tlegs)->data->date.get_start_days1900();
        if ((*tlegs)->data->date.eyear == -1)
          pimg->days2 = (*tlegs)->data->date.get_start_days1900();
        else
          pimg->days2 = (*tlegs)->data->date.get_end_days1900();
      } else {
        pimg->days1 = -1;  // Unknown dates
        pimg->days2 = -1;
      }

      if (is_surface)
        s_exp[cur_st] |= img_SFLAG_SURFACE;
      else
        s_exp[cur_st] |= img_SFLAG_UNDERGROUND;

      newtraverse = false;
      if (cur_st != last_st) newtraverse = true;
      if (check_traverses) {
        if ((*tlegs)->leg->traverse != last_traverse) newtraverse = true;
      } else {
        if ((*tlegs)->leg->loop != last_loop) newtraverse = true;
      }
      if (newtraverse) {
        if (check_traverses) {
          if ((cnlegs > 0) && (last_traverse != NULL)) {
            img_write_errors(pimg, cnlegs, last_traverse->src_length, last_traverse->E, last_traverse->H, last_traverse->V);
          }
        } else {
          if ((cnlegs > 0) && (last_loop != NULL)) {
            img_write_errors(pimg, cnlegs, last_loop->src_length, last_loop->err, last_loop->err, last_loop->err);
          }
        }
        img_write_item(pimg, img_MOVE, 0, NULL, 
          dbp->db1d.station_vec[cur_st].x, dbp->db1d.station_vec[cur_st].y, dbp->db1d.station_vec[cur_st].z);
        //thprintf("move to %d\n",cur_st);
        cnlegs = 0;
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      if (is_surface)
        s_exp[last_st] |= img_SFLAG_SURFACE;
      else
        s_exp[last_st] |= img_SFLAG_UNDERGROUND;
      leg_flag = 0;
      if (is_surface)
        leg_flag |= img_FLAG_SURFACE;
      if (is_duplicate)
        leg_flag |= img_FLAG_DUPLICATE;
      if (is_splay)
        leg_flag |= img_FLAG_SPLAY;

      img_write_item(pimg, img_LINE, leg_flag, (*tlegs)->survey->get_reverse_full_name(),
          dbp->db1d.station_vec[last_st].x, dbp->db1d.station_vec[last_st].y, dbp->db1d.station_vec[last_st].z);
      cnlegs++;
      last_loop = (*tlegs)->leg->loop;
      last_traverse = (*tlegs)->leg->traverse;
      //thprintf("line to %d\n",last_st);
    }
  }

  cis_exp = s_exp;
  thbuffer stnbuf;
  thdb1ds * tmps;
  for(i = 0; i < nstat; i++, cis_exp++) {
    if ((*cis_exp != 0) || (s_exp[dbp->db1d.station_vec[i].uid - 1] != 0) || 
        (((dbp->db1d.station_vec[i].flags & 
        (TT_STATIONFLAG_ENTRANCE | TT_STATIONFLAG_FIXED)) != 0) &&
        (dbp->db1d.station_vec[i].survey->is_selected() ||
         dbp->db1d.station_vec[dbp->db1d.station_vec[i].uid - 1].survey->is_selected()))) {
      if ((*cis_exp == 0) && (s_exp[dbp->db1d.station_vec[i].uid - 1] != 0))	
        x_exp = s_exp[dbp->db1d.station_vec[i].uid - 1];
      else
        x_exp = *cis_exp;
      tmps = &(dbp->db1d.station_vec[i]);
      if ((tmps->flags & TT_STATIONFLAG_ENTRANCE) != 0)
        x_exp |= img_SFLAG_ENTRANCE;
      if (((tmps->flags & TT_STATIONFLAG_FIXED) != 0) && ((tmps->flags & TT_STATIONFLAG_NOTFIXED) == 0))
        x_exp |= img_SFLAG_FIXED;
      stnbuf.strcpy(dbp->db1d.station_vec[i].survey->get_reverse_full_name());
      if (strlen(stnbuf.get_buffer()) > 0) stnbuf.strcat(".");
      stnbuf.strcat(dbp->db1d.station_vec[i].name);
      if (!tmps->is_temporary())
        img_write_item(pimg, img_LABEL, x_exp, stnbuf,
          dbp->db1d.station_vec[i].x, dbp->db1d.station_vec[i].y, dbp->db1d.station_vec[i].z);
      else {
        x_exp |= img_SFLAG_ANON;
        if (tmps->temps == TT_TEMPSTATION_WALL)
          x_exp |= img_SFLAG_WALL;        
        img_write_item(pimg, img_LABEL, x_exp, "",
          dbp->db1d.station_vec[i].x, dbp->db1d.station_vec[i].y, dbp->db1d.station_vec[i].z);
      
      }
    }
  }
  
  delete [] s_exp;
  
  img_close(pimg);

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
  
}


// 8 - survey name
// 12 - station name
// 40 - survey comment
// -9.9 - no dimension

#define toft(x) ((x)/0.3048)
 
void thexpmodel::export_plt_file(class thdatabase * dbp)
{

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = (unsigned long)dbp->db1d.station_vec.size();
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();

  if (nlegs == 0)
    return;
  
  const char * fnm = this->get_output("cave.plt");
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  unsigned long i;
  FILE * pltf;
  char station_name[14];

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  unsigned long last_st = nstat, cur_st;
  double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0, 
    zmin = 0.0, zmax = 0.0;
  
#define minmaxvar(min,max,var) \
if (var < min) min = var;\
if (var > max) max = var
  
  // OK, let's calculate limits
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      if (cur_st != last_st) {
        if (last_st == nstat) {
          xmax = dbp->db1d.station_vec[cur_st].x;
          xmin = xmax;
          ymax = dbp->db1d.station_vec[cur_st].y;
          ymin = ymax;
          zmax = dbp->db1d.station_vec[cur_st].z;
          zmin = zmax;
        } else {
          minmaxvar(xmin,xmax,dbp->db1d.station_vec[cur_st].x);
          minmaxvar(ymin,ymax,dbp->db1d.station_vec[cur_st].y);
          minmaxvar(zmin,zmax,dbp->db1d.station_vec[cur_st].z);
        }
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      minmaxvar(xmin,xmax,dbp->db1d.station_vec[last_st].x);
      minmaxvar(ymin,ymax,dbp->db1d.station_vec[last_st].y);
      minmaxvar(zmin,zmax,dbp->db1d.station_vec[last_st].z);
    }
  }
  
#define copy_station_name(stid) \
strncpy(station_name,dbp->db1d.station_vec[stid].name,8); \
station_name[8] = 0

#define exppltdim(ffd,ttd) (((*tlegs)->reverse ? thisnan((*tlegs)->leg->ffd) : thisnan((*tlegs)->leg->ttd)) ? -9999.0 : toft((*tlegs)->reverse ? (*tlegs)->leg->ffd : (*tlegs)->leg->ttd))
  
  // now let's print header
  fprintf(pltf,"Z\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\n",toft(ymin),toft(ymax),toft(xmin),toft(xmax),toft(zmin),toft(zmax));
  fprintf(pltf,"STHERION\n");
  fprintf(pltf,"NX\tD\t3\t12\t1997\tCtherion export\n");
  last_st = nstat;
  tlegs = dbp->db1d.get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      if (cur_st != last_st) {
        copy_station_name(cur_st);
        fprintf(pltf,"M\t%.2f\t%.2f\t%.2f\tS%s\tP\t%.2f\t%.2f\t%.2f\t%.2f\n",
          toft(dbp->db1d.station_vec[cur_st].y), toft(dbp->db1d.station_vec[cur_st].x), 
          toft(dbp->db1d.station_vec[cur_st].z),station_name,
          exppltdim(to_left, from_left), 
          exppltdim(to_up, from_up), 
          exppltdim(to_down, from_down), 
          exppltdim(to_right, from_right));
//          toft(dbp->db1d.station_vec[cur_st].z - avz),station_name);
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      copy_station_name(last_st);
      if (strcmp(station_name,"VA09") == 0) 
        fflush(pltf);
      fprintf(pltf,"D\t%.2f\t%.2f\t%.2f\tS%s\tP\t%.2f\t%.2f\t%.2f\t%.2f\n",
        toft(dbp->db1d.station_vec[last_st].y), toft(dbp->db1d.station_vec[last_st].x), 
        toft(dbp->db1d.station_vec[last_st].z),station_name,
        exppltdim(from_left, to_left), 
        exppltdim(from_up, to_up), 
        exppltdim(from_down, to_down), 
        exppltdim(from_right, to_right));
    }
  }

  fprintf(pltf,"X\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\x1A",toft(ymin),toft(ymax),toft(xmin),toft(xmax),toft(zmin),toft(zmax));
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}

/*
void thexpmodel::export_thm_file(class thdatabase * dbp)
{

  const char * fnm = this->get_output("cave.thm");
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  FILE * pltf;

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  double avx, avy, avz;
  thdb_object_list_type::iterator obi;
  thdb3ddata * pgn = dbp->db1d.get_3d(), 
    * surf_pgn = dbp->db1d.get_3d_surface(),
    * splay_pgn = dbp->db1d.get_3d_splay(),
    * tmp3d;
  thdb3dlim pgnlimits, finlim;
  switch (this->items & TT_EXPMODEL_ITEM_CENTERLINE) {
    case TT_EXPMODEL_ITEM_SURFACECENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      break;
    case TT_EXPMODEL_ITEM_CENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      pgnlimits.update(&(pgn->limits));
      break;
    default:    
      pgnlimits.update(&(pgn->limits));
  }
	if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0)
		pgnlimits.update(&(splay_pgn->limits));
  finlim.update(&(pgnlimits));

  // update limits from surfaces
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          tmp3d = ((thsurface*)(*obi))->get_3d();
          if (tmp3d != NULL) {
            pgnlimits.update(&(tmp3d->limits));
          }
          break;
      }
      obi++;
    }
  }

  avx = (pgnlimits.minx + pgnlimits.maxx) / 2.0;
  avy = (pgnlimits.miny + pgnlimits.maxy) / 2.0;
  avz = (pgnlimits.minz + pgnlimits.maxz) / 2.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;
  surf_pgn->exp_shift_x = avx;
  surf_pgn->exp_shift_y = avy;
  surf_pgn->exp_shift_z = avz;


  // now let's print header
//  thprintf("\nLIMITS: %10.2f%10.2f%10.2f%10.2f%10.2f%10.2f\n", 
//          finlim.minx, finlim.maxx, 
//          finlim.miny, finlim.maxy, 
//          finlim.minz, finlim.maxz);

  
  fprintf(pltf,"glDeleteLists $xthmvv(list,model) 1\n");
  fprintf(pltf,"glNewList $xthmvv(list,model) $GL::GL_COMPILE\n");
  fprintf(pltf,"xth_mv_gl_wireframe\n");

  if ((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) != 0) {
    fprintf(pltf,"glColor4f 0.3 0.7 1.0 1.0\n");
    pgn->export_thm(pltf);
  }
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    fprintf(pltf,"glColor4f 0.5 0.5 0.5 1.0\n");
    surf_pgn->export_thm(pltf);
  }

  
  // WALLS
  fprintf(pltf,"\n#WALLS\n\n");
  fprintf(pltf,"xth_mv_gl_walls\n");
  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {
    thdb3ddata * d3d;

    if ((this->wallsrc & TT_WSRC_CENTERLINE) != 0) {
      d3d = dbp->db1d.get_3d_walls();
      finlim.update(&(d3d->limits));
      d3d->exp_shift_x = avx;
      d3d->exp_shift_y = avy;
      d3d->exp_shift_z = avz;
      d3d->export_thm(pltf);
    }

    if ((this->wallsrc & TT_WSRC_MAPS) != 0) {
      thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
      thscrap * cs;
      if (!prjid.newprj) {
        thdb.db2d.process_projection(prjid.prj);
        cs = prjid.prj->first_scrap;
        while(cs != NULL) {
          if (cs->fsptr->is_selected() && (cs->d3 != TT_FALSE)) {
            d3d = cs->get_3d_outline();
    //        thprintf("\nLIMITS: %10.2f%10.2f%10.2f%10.2f%10.2f%10.2f\n", 
    //          d3d->limits.minx, d3d->limits.maxx, 
    //          d3d->limits.miny, d3d->limits.maxy, 
    //          d3d->limits.minz, d3d->limits.maxz);
            finlim.update(&(d3d->limits));
            d3d->exp_shift_x = avx;
            d3d->exp_shift_y = avy;
            d3d->exp_shift_z = avz;
            d3d->export_thm(pltf);
          }
          cs = cs->proj_next_scrap;
        }
      }
    }
  
  } // WALLS

  fprintf(pltf,"\n#SURFACE\n\n");
  fprintf(pltf,"xth_mv_gl_surface\n");
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          tmp3d = ((thsurface*)(*obi))->get_3d();
          if (tmp3d != NULL) {
            tmp3d->exp_shift_x = avx;
            tmp3d->exp_shift_y = avy;
            tmp3d->exp_shift_z = avz;
            tmp3d->export_thm(pltf);
            finlim.update(&(tmp3d->limits));
          }
          break;
      }
      obi++;
    }
  }
  
  fprintf(pltf,"set xthmvv(model,maxx) %.2f\n",finlim.maxx - avx);
  fprintf(pltf,"set xthmvv(model,maxy) %.2f\n",finlim.maxy - avy);
  fprintf(pltf,"set xthmvv(model,maxz) %.2f\n",finlim.maxz - avz);
  fprintf(pltf,"set xthmvv(model,minx) %.2f\n",finlim.minx - avx);
  fprintf(pltf,"set xthmvv(model,miny) %.2f\n",finlim.miny - avy);
  fprintf(pltf,"set xthmvv(model,minz) %.2f\n",finlim.minz - avz);
  fprintf(pltf,"glEndList\n");


  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}
*/

void thexpmodel::export_vrml_file(class thdatabase * dbp) {
  const char * fnm;  
#ifdef THWIN32
  fnm = this->get_output("cave.wrl");
#else
  fnm = this->get_output("cave.vrml");
#endif
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  FILE * pltf;

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  thdb_object_list_type::iterator obi;
  thdb3ddata * pgn = dbp->db1d.get_3d(), 
    * surf_pgn = dbp->db1d.get_3d_surface(),
    * splay_pgn = dbp->db1d.get_3d_splay(),
    * tmp3d;
  thdb3dlim pgnlimits, finlim;
  switch (this->items & TT_EXPMODEL_ITEM_CENTERLINE) {
    case TT_EXPMODEL_ITEM_SURFACECENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      break;
    case TT_EXPMODEL_ITEM_CENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      pgnlimits.update(&(pgn->limits));
      break;
    default:    
      pgnlimits.update(&(pgn->limits));
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0)
    pgnlimits.update(&(splay_pgn->limits));

  finlim.update(&(pgnlimits));
  // now let's print header
  fprintf(pltf,"#VRML V2.0 utf8\n\n");

  fprintf(pltf,"Transform { children [\n");
  
  if ((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) != 0) {
    fprintf(pltf,
       "Shape {\nappearance Appearance {\n" \
       "\tmaterial Material {\n\t\temissiveColor 0.3 0.7 1.0\n\t}" \
       "\n}\ngeometry IndexedLineSet {\n");
       pgn->export_vrml(pltf);
    fprintf(pltf,"\n}\n}\n");
  }
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    fprintf(pltf,
       "Shape {\nappearance Appearance {\n" \
       "\tmaterial Material {\n\t\temissiveColor 0.5 0.5 0.5\n\t}" \
       "\n}\ngeometry IndexedLineSet {\n");
       surf_pgn->export_vrml(pltf);
    fprintf(pltf,"\n}\n}\n");
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0) {
    fprintf(pltf,
       "Shape {\nappearance Appearance {\n" \
       "\tmaterial Material {\n\t\temissiveColor 0.22 0.22 0.22\n\t}" \
       "\n}\ngeometry IndexedLineSet {\n");
       splay_pgn->export_vrml(pltf);
    fprintf(pltf,"\n}\n}\n");
  }

  bool has_texture;
  int imgn = 0;
  double txx, txy, tyx, tyy, tinv, tdx, tdy;
  thsurface * srfc;
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    fprintf(pltf, "Group {\n\tchildren[\n");
    fprintf(pltf, "DirectionalLight {\n\ton TRUE\n\tintensity  1.0\n\tdirection  0 0 -1\n}");
    fprintf(pltf, "DirectionalLight {\n\ton TRUE\n\tintensity  1.0\n\tdirection  0 0  1\n}");
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          srfc = ((thsurface*)(*obi).get());
          tmp3d = srfc->get_3d();
          srfc->calibrate();
          tinv = srfc->calib_yy*srfc->calib_xx - srfc->calib_xy*srfc->calib_yx;
          has_texture = ((srfc->pict_name != NULL) && (srfc->pict_height > 0) && (srfc->pict_width > 0) && (tinv != 0.0));
          if (tmp3d != NULL) {
            fprintf(pltf,
              "Shape {\nappearance Appearance {\n" \
              "\tmaterial Material {\n\t\tdiffuseColor 0.3 1.0 0.1\n\t\ttransparency 0.5\n\t}\n");
            if (has_texture) {
              thbuffer tifn;
              tifn.guarantee(2048);
              sprintf(tifn.get_buffer(), "%s.img%d.%s", fnm, imgn++, srfc->pict_type == TT_IMG_TYPE_JPEG ? "jpg" : "png");
              FILE * texf, * xf;
              texf = fopen(tifn.get_buffer(), "wb");
              xf = fopen(srfc->pict_name, "rb");
              if (texf != NULL) {
                if (xf != NULL) {
                  fseek(xf, 0, SEEK_END);
                  size_t fsz = ftell(xf);
                  fseek(xf, 0, SEEK_SET);
                  if (fsz > 0) {
                    char * cdata = new char [fsz];
                    thassert(fread((void *) cdata, 1, fsz, xf) == fsz);
                    fwrite((void *) cdata, 1, fsz, texf);
                    delete [] cdata;
                  }
                  fclose(xf);
                }
                fclose(texf);
                fprintf(pltf,
                  "\ttexture ImageTexture {\n\t\turl [\"%s\"]\n\t}\n", tifn.get_buffer());
              }
            }
            fprintf(pltf,
              "\n}\ngeometry IndexedFaceSet {\n\tsolid FALSE\n\tcreaseAngle 3.0\n");
            tmp3d->export_vrml(pltf);
            if (has_texture) {
              txx = srfc->calib_yy / tinv;
              txy = -srfc->calib_xy / tinv;
              tyx = -srfc->calib_yx / tinv;
              tyy = srfc->calib_xx / tinv;
              tdx = -1.0 * (txx * srfc->calib_x + txy * srfc->calib_y);
              tdy = -1.0 * (tyx * srfc->calib_x + tyy * srfc->calib_y);
              thdb3dvx * vx;
              fprintf(pltf,"texCoord TextureCoordinate {\n\tpoint [\n");
              for(vx = tmp3d->firstvx; vx != NULL; vx = vx->next)
                fprintf(pltf,"\t%8.6f %8.6f,\n", 
                  (vx->x * txx + vx->y * txy + tdx) / srfc->pict_width, 
                  (vx->x * tyx + vx->y * tyy + tdy) / srfc->pict_height);
              fprintf(pltf,"\t]\n}\n");
            }
            finlim.update(&(tmp3d->limits));
            fprintf(pltf,"\n}\n}\n");
          }
          break;
      }
      obi++;
    }
    fprintf(pltf, "\t]\n}\n");
  }



  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {
  
    thscrap * cs;
    thdb3ddata * d3d;
    
    if ((this->wallsrc & TT_WSRC_CENTERLINE) != 0) {
      d3d = dbp->db1d.get_3d_walls();
      finlim.update(&(d3d->limits));
      fprintf(pltf,
        "Shape {\nappearance Appearance {\n" \
        "\tmaterial Material {\n\t\tdiffuseColor 1.0 1.0 1.0\n\t}" \
        "\n}\ngeometry IndexedFaceSet {\n");
      d3d->export_vrml(pltf);
      fprintf(pltf,"creaseAngle 3.0\n}\n}\n");
    }
    
    // 3D DATA 
    if ((this->wallsrc & TT_WSRC_MAPS) != 0) {
      thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
      if (!prjid.newprj) {
        thdb.db2d.process_projection(prjid.prj);
        cs = prjid.prj->first_scrap;
        while(cs != NULL) {
          if (cs->fsptr->is_selected()) {
            d3d = cs->get_3d_outline();
            finlim.update(&(d3d->limits));
            fprintf(pltf,
              "Shape {\nappearance Appearance {\n" \
              "\tmaterial Material {\n\t\tdiffuseColor 1.0 1.0 1.0\n\t}" \
              "\n}\ngeometry IndexedFaceSet {\n");
            d3d->export_vrml(pltf);
            fprintf(pltf,"creaseAngle 3.0\n}\n}\n");
          }
          cs = cs->proj_next_scrap;
        }
      }
    }
    
  } // WALLS
  

  fprintf(pltf,
     "Shape {\nappearance Appearance {\n" \
     "\tmaterial Material {\n\t\temissiveColor 1.0 0.0 0.0\n\t}" \
     "\n}\ngeometry IndexedLineSet {\n");

  fprintf(pltf,"coord Coordinate {\n\tpoint [\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx , finlim.miny , finlim.minz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx , finlim.maxy , finlim.minz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx , finlim.maxy , finlim.minz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx , finlim.miny , finlim.minz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx , finlim.miny , finlim.maxz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx , finlim.maxy , finlim.maxz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx , finlim.maxy , finlim.maxz );
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx , finlim.miny , finlim.maxz );
  fprintf(pltf,"\t]\n}\n");
  fprintf(pltf,"\tcoordIndex [\n");
  fprintf(pltf,"0, 1, 2, 3, 0, 4, 5, 6, 7, 4, -1,\n");
  fprintf(pltf,"1, 5, -1,\n");
  fprintf(pltf,"2, 6, -1,\n");
  fprintf(pltf,"3, 7, -1,\n");
  fprintf(pltf,"\t]\n");

  fprintf(pltf,"\n}\n}\n");

  fprintf(pltf,"\n]\ntranslation %8.2f %8.2f %8.2f} # end of transformation to center\n",
    (finlim.maxx + finlim.minx) / -2.0,
    (finlim.maxy + finlim.miny) / -2.0,
    (finlim.maxz + finlim.minz) / -2.0
  );


  double diamx, diamy, diamz, diamxy, diamxz, diamyz, diam;
  diamx = (finlim.maxx - finlim.minx) / 2.0;
  diamy = (finlim.maxy - finlim.miny) / 2.0;
  diamz = (finlim.maxz - finlim.minz) / 2.0;
  diamxy = std::hypot(diamx, diamy);
  diamxz = std::hypot(diamx, diamz);
  diamyz = std::hypot(diamy, diamz);
  diam = std::hypot(diamxy, diamz);
  fprintf(pltf,"Viewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 0.0 %8.2f\n\tdescription \"Down\"\n}\n",
    diamxy / 0.21818181818181 + diamz);
  fprintf(pltf,"Viewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 0.0 %8.2f\n\torientation 1 0 0 3.14159\n\tdescription \"Up\"\n}\n",
    diamxy / -0.21818181818181 - diamz);
  fprintf(pltf,"Viewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 %8.2f 0.0\n\torientation 1 0 0 1.570796\n\tdescription \"North\"\n}\n",
    diamxz / -0.21818181818181 - diamy);
  fprintf(pltf,"Transform {\n\trotation 0 0 1 1.570796\n\tchildren\nViewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 %8.2f 0.0\n\torientation 1 0 0 1.570796\n\tdescription \"West\"\n}\n}\n",
    diamyz / -0.21818181818181 - diamx);
  fprintf(pltf,"Transform {\n\trotation 0 0 1 -1.570796\n\tchildren\nViewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 %8.2f 0.0\n\torientation 1 0 0 1.570796\n\tdescription \"East\"\n}\n}\n",
    diamyz / -0.21818181818181 - diamx);
  fprintf(pltf,"Transform {\n\trotation 0 0 1 3.1415927\n\tchildren\nViewpoint {\n\tfieldOfView 0.42\n\tjump TRUE\n\tposition 0.0 %8.2f 0.0\n\torientation 1 0 0 1.570796\n\tdescription \"South\"\n}\n}\n",
    diamxz / -0.21818181818181 - diamy);
  fprintf(pltf,"\nNavigationInfo {\n\theadlight TRUE\n\tspeed %.1f\n}", diam * 0.1);
  fprintf(pltf,"\nBackground {\n\tskyColor 0 0 0\n}\n");
  fprintf(pltf,"\n");
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpmodel::export_3dmf_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave.3dmf");
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  FILE * pltf;

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  double avx, avy, avz;
  thdb_object_list_type::iterator obi;
  thdb3ddata * pgn = dbp->db1d.get_3d(), 
    * surf_pgn = dbp->db1d.get_3d_surface(),
    * splay_pgn = dbp->db1d.get_3d_splay(),
    * tmp3d;
  thdb3dlim pgnlimits, finlim;
  switch (this->items & TT_EXPMODEL_ITEM_CENTERLINE) {
    case TT_EXPMODEL_ITEM_SURFACECENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      break;
    case TT_EXPMODEL_ITEM_CENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      pgnlimits.update(&(pgn->limits));
      break;
    default:    
      pgnlimits.update(&(pgn->limits));
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0)
    pgnlimits.update(&(splay_pgn->limits));
  finlim.update(&(pgnlimits));
  avx = (pgnlimits.minx + pgnlimits.maxx) / 2.0;
  avy = (pgnlimits.miny + pgnlimits.maxy) / 2.0;
  avz = (pgnlimits.minz + pgnlimits.maxz) / 2.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;
  surf_pgn->exp_shift_x = avx;
  surf_pgn->exp_shift_y = avy;
  surf_pgn->exp_shift_z = avz;

  // now let's print header
  
  fprintf(pltf,"3DMetafile ( 1 0 Stream nextTOC> ) \n");
  //pgn->export_thm(pltf);

  if ((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) != 0) {
    if (pgn->firstfc != NULL) {
      fprintf(pltf,"BeginGroup (\n\tDisplayGroup ( ) \n)\n"
              "Container ( \n\tAttributeSet ( ) \n"
              "\tDiffuseColor ( 0.3 0.7 1.0)\n)\n");
      pgn->export_3dmf(pltf);
      fprintf(pltf,"EndGroup ( )\n");
    }
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0) {
    if (pgn->firstfc != NULL) {
      fprintf(pltf,"BeginGroup (\n\tDisplayGroup ( ) \n)\n"
              "Container ( \n\tAttributeSet ( ) \n"
              "\tDiffuseColor ( 0.22 0.22 0.22)\n)\n");
      splay_pgn->export_3dmf(pltf);
      fprintf(pltf,"EndGroup ( )\n");
    }
  }
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    if (surf_pgn->firstfc != NULL) {
      fprintf(pltf,"BeginGroup (\n\tDisplayGroup ( ) \n)\n"
              "Container ( \n\tAttributeSet ( ) \n"
              "\tDiffuseColor ( 0.5 0.5 0.5) \n)\n");
      surf_pgn->export_3dmf(pltf);
      fprintf(pltf,"EndGroup ( )\n");
    }
  }


  
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          tmp3d = ((thsurface*)(*obi).get())->get_3d();
          if (tmp3d != NULL) {
            tmp3d->exp_shift_x = avx;
            tmp3d->exp_shift_y = avy;
            tmp3d->exp_shift_z = avz;
            fprintf(pltf,"BeginGroup ( \n\tDisplayGroup ( ) \n) \n"
              "Container ( \n\tMesh ( \n");
            tmp3d->export_3dmf(pltf);
            finlim.update(&(tmp3d->limits));
            fprintf(pltf,"\t) \nContainer ( \n\tAttributeSet ( ) \n"
              "\tDiffuseColor ( 0.3 1.0 0.1 ) \n\tTransparencyColor (0.5 0.5 0.5)\n) \n) \nEndGroup ( ) \n");
          }
          break;
      }
      obi++;
    }
  }



  // 3D DATA 
  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {

    thscrap * cs;
    thdb3ddata * d3d;

    if ((this->wallsrc & TT_WSRC_CENTERLINE) != 0) {
      d3d = dbp->db1d.get_3d_walls();
      finlim.update(&(d3d->limits));
      d3d->exp_shift_x = avx;
      d3d->exp_shift_y = avy;
      d3d->exp_shift_z = avz;
      if ((d3d->firstfc != NULL) && (d3d->firstvx != NULL)) {
        fprintf(pltf,"BeginGroup ( \n\tDisplayGroup ( ) \n) \n"
          "Container ( \n\tMesh ( \n");
        d3d->export_3dmf(pltf);
        fprintf(pltf,"\t) \nContainer ( \n\tAttributeSet ( ) \n"
          "\tDiffuseColor ( 1 1 1 ) \n) \n) \nEndGroup ( ) \n");
      }
    }


    if ((this->wallsrc & TT_WSRC_MAPS) != 0) {
      thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
      if (!prjid.newprj) {
        thdb.db2d.process_projection(prjid.prj);
        cs = prjid.prj->first_scrap;
        while(cs != NULL) {
          if (cs->fsptr->is_selected()) {
            d3d = cs->get_3d_outline();
            finlim.update(&(d3d->limits));
            d3d->exp_shift_x = avx;
            d3d->exp_shift_y = avy;
            d3d->exp_shift_z = avz;
            if ((d3d->firstfc != NULL) && (d3d->firstvx != NULL)) {
              fprintf(pltf,"BeginGroup ( \n\tDisplayGroup ( ) \n) \n"
                "Container ( \n\tMesh ( \n");
              d3d->export_3dmf(pltf);
              fprintf(pltf,"\t) \nContainer ( \n\tAttributeSet ( ) \n"
                "\tDiffuseColor ( 1 1 1 ) \n) \n) \nEndGroup ( ) \n");
            }
          }
          cs = cs->proj_next_scrap;
        }
      }
    }
  
  } // WALLS
  

  fprintf(pltf,"BeginGroup ( \n\tDisplayGroup ( ) \n) \n"
    "Container ( \n\tAttributeSet ( ) \n"
    "\tDiffuseColor ( 1 0 0 ) \n)\n");
  fprintf(pltf,"\tPolyLine ( 10\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.minz - avz, avy - finlim.miny );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.minz - avz, avy - finlim.maxy );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.minz - avz, avy - finlim.maxy );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.minz - avz, avy - finlim.miny );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.minz - avz, avy - finlim.miny );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.maxz - avz, avy - finlim.miny );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.maxz - avz, avy - finlim.maxy );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.maxz - avz, avy - finlim.maxy );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.maxz - avz, avy - finlim.miny );
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.maxz - avz, avy - finlim.miny );
  fprintf(pltf,")\n");
  fprintf(pltf,"\tPolyLine ( 2\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.minz - avz, avy - finlim.maxy);
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.minx - avx, finlim.maxz - avz, avy - finlim.maxy);
  fprintf(pltf,")\n");
  fprintf(pltf,"\tPolyLine ( 2\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.minz - avz, avy - finlim.maxy);
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.maxz - avz, avy - finlim.maxy);
  fprintf(pltf,")\n");
  fprintf(pltf,"\tPolyLine ( 2\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.minz - avz, avy - finlim.miny);
  fprintf(pltf,"%8.2f %8.2f %8.2f\n",finlim.maxx - avx, finlim.maxz - avz, avy - finlim.miny);
  fprintf(pltf,")\n");
  fprintf(pltf,"EndGroup ( )\n");


  fprintf(pltf,"\n");
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpmodel::export_dxf_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave.dxf");
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  FILE * pltf;

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }




  double avx, avy, avz;
  thdb_object_list_type::iterator obi;
  thdb3ddata * pgn = dbp->db1d.get_3d(), 
    * surf_pgn = dbp->db1d.get_3d_surface(),
    * splay_pgn = dbp->db1d.get_3d_splay(),
    * tmp3d;
  thdb3dlim pgnlimits, finlim, extlim;
  switch (this->items & TT_EXPMODEL_ITEM_CENTERLINE) {
    case TT_EXPMODEL_ITEM_SURFACECENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      break;
    case TT_EXPMODEL_ITEM_CENTERLINE:
      pgnlimits.update(&(surf_pgn->limits));
      pgnlimits.update(&(pgn->limits));
      break;
    default:    
      pgnlimits.update(&(pgn->limits));
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0)
    pgnlimits.update(&(splay_pgn->limits));
  finlim.update(&(pgnlimits));
  avx = 0.0;
  avy = 0.0;
  avz = 0.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;
  surf_pgn->exp_shift_x = avx;
  surf_pgn->exp_shift_y = avy;
  surf_pgn->exp_shift_z = avz;


  extlim.update(&(surf_pgn->limits));
  extlim.update(&(pgn->limits));

  // now let's print header
  fprintf(pltf,"999\n" \
    "Therion DXF export\n" \
    "0\n" \
    "SECTION\n" \
    "2\n" \
    "HEADER\n" \
    "9\n" \
    "$ACADVER\n" \
    "1\n" \
    "AC1006\n" \
    "9\n" \
    "$INSBASE\n" \
    "10\n" \
    "0.0\n" \
    "20\n" \
    "0.0\n" \
    "30\n" \
    "0.0\n" \
    "9\n" \
    "$EXTMIN\n" \
    "10\n" \
    "%.3f\n" \
    "20\n" \
    "%.3f\n" \
    "30\n" \
    "%.3f\n" \
    "9\n" \
    "$EXTMAX\n" \
    "10\n" \
    "%.3f\n" \
    "20\n" \
    "%.3f\n" \
    "30\n" \
    "%.3f\n" \
    "0\n" \
    "ENDSEC\n" \
    "0\n" \
    "SECTION\n" \
    "2\n" \
    "TABLES\n" \
    "0\n" \
    "TABLE\n" \
    "2\n" \
    "LTYPE\n" \
    "70\n" \
    "1\n" \
    "0\n" \
    "LTYPE\n" \
    "2\n" \
    "CONTINUOUS\n" \
    "70\n" \
    "64\n" \
    "3\n" \
    "Solid line\n" \
    "72\n" \
    "65\n" \
    "73\n" \
    "0\n" \
    "40\n" \
    "0.000000\n" \
    "0\n" \
    "ENDTAB\n" \
    "0\n" \
    "TABLE\n" \
    "2\n" \
    "LAYER\n" \
    "70\n" \
    "6\n" \
    "0\n" \
    "LAYER\n" \
    "2\n" \
    "CENTERLINE\n" \
    "70\n" \
    "64\n" \
    "62\n" \
    "4\n" \
    "6\n" \
    "CONTINUOUS\n" \
    "0\n" \
    "LAYER\n" \
    "2\n" \
    "WALLS\n" \
    "70\n" \
    "64\n" \
    "62\n" \
    "7\n" \
    "6\n" \
    "CONTINUOUS\n" \
    "0\n" \
    "LAYER\n" \
    "2\n" \
    "SURFACE\n" \
    "70\n" \
    "64\n" \
    "62\n" \
    "3\n" \
    "6\n" \
    "CONTINUOUS\n" \
    "0\n" \
    "LAYER\n" \
    "2\n" \
    "SURFACE_CENTERLINE\n" \
    "70\n" \
    "64\n" \
    "62\n" \
    "8\n" \
    "6\n" \
    "CONTINUOUS\n" \
    "0\n" \
    "LAYER\n" \
    "2\n" \
    "BBOX\n" \
    "70\n" \
    "64\n" \
    "62\n" \
    "1\n" \
    "6\n" \
    "CONTINUOUS\n" \
    "0\n" \
    "ENDTAB\n" \
    "0\n" \
    "TABLE\n" \
    "2\n" \
    "STYLE\n" \
    "70\n" \
    "0\n" \
    "0\n" \
    "ENDTAB\n" \
    "0\n" \
    "ENDSEC\n" \
    "0\n" \
    "SECTION\n" \
    "2\n" \
    "BLOCKS\n" \
    "0\n" \
    "ENDSEC\n" \
    "0\n" \
    "SECTION\n" \
    "2\n" \
    "ENTITIES\n",
    extlim.minx, extlim.miny, extlim.minz,
    extlim.maxx, extlim.maxy, extlim.maxz);

//    "9\n" 
//    "$LIMMIN\n" 
//    "10\n" 
//    "%.3f\n" 
//    "20\n" 
//    "%.3f\n" 
//    "9\n" 
//    "$LIMMAX\n" 
//    "10\n" 
//    "%.3f\n" 
//    "20\n" 
//    "%.3f\n" 


  if ((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) != 0) {
    pgn->export_dxf(pltf,"CENTERLINE");
  }
  if ((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0) {
    splay_pgn->export_dxf(pltf,"SPLAY_SHOTS");
  }
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    surf_pgn->export_dxf(pltf,"SURFACE_CENTERLINE");
  }

  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          tmp3d = ((thsurface*)(*obi).get())->get_3d();
          if (tmp3d != NULL) {
            tmp3d->exp_shift_x = avx;
            tmp3d->exp_shift_y = avy;
            tmp3d->exp_shift_z = avz;
            tmp3d->export_dxf(pltf,"SURFACE");
            finlim.update(&(tmp3d->limits));
          }
          break;
      }
      obi++;
    }
  }



  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {
  
    // 3D DATA 
    thscrap * cs;
    thdb3ddata * d3d;
    
    if ((this->wallsrc & TT_WSRC_CENTERLINE) != 0) {
      d3d = dbp->db1d.get_3d_walls();
      finlim.update(&(d3d->limits));
      d3d->exp_shift_x = avx;
      d3d->exp_shift_y = avy;
      d3d->exp_shift_z = avz;
      d3d->export_dxf(pltf,"WALLS");
    }
    
    if ((this->wallsrc & TT_WSRC_MAPS) != 0) {
      thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
      if (!prjid.newprj) {
        thdb.db2d.process_projection(prjid.prj);
        cs = prjid.prj->first_scrap;
        while(cs != NULL) {
          if (cs->fsptr->is_selected()) {
            d3d = cs->get_3d_outline();
            finlim.update(&(d3d->limits));
            d3d->exp_shift_x = avx;
            d3d->exp_shift_y = avy;
            d3d->exp_shift_z = avz;
            d3d->export_dxf(pltf,"WALLS");
          }
          cs = cs->proj_next_scrap;
        }
      }
    }
    
  } // WALLS
  

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.minz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.minz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.maxz - avz);
  
  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.maxz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.maxz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.minz - avz);


  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.maxz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.miny - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.maxz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.maxz - avz);
  
  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.minz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.maxx - avx, finlim.maxy - avy, finlim.minz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.maxx - avx, finlim.miny - avy, finlim.minz - avz);

  fprintf(pltf,"0\nLINE\n8\nBBOX\n");
  fprintf(pltf,"10\n%.3f\n20\n%.3f\n30\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"11\n%.3f\n21\n%.3f\n31\n%.3f\n",finlim.minx - avx, finlim.maxy - avy, finlim.minz - avz);

  fprintf(pltf,"0\nENDSEC\n0\nEOF\n");
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}



void thexpmodel::export_lox_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave.lox");

  thdb_object_list_type::iterator obi;
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ...", fnm);
  thtext_inline = true;
#endif

  lxFile expf;
      
  thsurvey * sptr, * tsptr;

  // vsetkym surveyom nastavi num1 na -1 ak nie su, 1 ak
  // su oznacene   
  
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi).get();
      if (sptr->is_selected()) {
        sptr->num1 = 1;
      } else {
        sptr->num1 = -1;
      }
    }
    obi++;
  }

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = (unsigned long)dbp->db1d.station_vec.size(), i, j;
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();
  long * stnum = NULL, * stnum_orig = NULL;
  if (nstat > 0) {
    stnum = new long[nstat];
    stnum_orig = new long[nstat];
    for (i = 0; i < nstat; i++)
      stnum[i] = (dbp->db1d.station_vec[i].survey->is_selected() ? 1 : -1); //;-1
  }

  // prejde vsetky zamery, ktore ideme 
  // exportovat a parent surveyom bodov tychto zamer nastavi num1 na 1
  // rovnako aj bodom num1 na 1
  bool txsfc, txcav, issfc;
  txsfc = ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0);
  txcav = ((this->items & TT_EXPMODEL_ITEM_CAVECENTERLINE) != 0);
  for(i = 0; i < nlegs; i++, tlegs++) {
    issfc = (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0);
    if ((*tlegs)->survey->is_selected() && ((txsfc && issfc) || (txcav && !issfc))) {
      stnum[(*tlegs)->leg->from.id - 1] = 1;
      dbp->db1d.station_vec[(*tlegs)->leg->from.id - 1].survey->num1 = 1;
      stnum[(*tlegs)->leg->to.id - 1] = 1;
      dbp->db1d.station_vec[(*tlegs)->leg->to.id - 1].survey->num1 = 1;
    }
  }
  
  // opat prejde vsetky surveye - doplni medziclanky a nastavi num1 
  // nastavi od 0 po n - exportuje ich
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi).get();
      if ((sptr->num1 > 0) && (sptr->fsptr != NULL)) {
        tsptr = sptr->fsptr;
        if (tsptr->num1 > 0)
          tsptr = NULL;
        if (tsptr != NULL)
          tsptr = tsptr->fsptr;
        while ((tsptr != NULL) && (tsptr->num1 < 0))
          tsptr = tsptr->fsptr;
        if (tsptr != NULL) {
          sptr = sptr->fsptr;
          while (sptr->id != tsptr->id) {
            sptr->num1 = 1;
            sptr = sptr->fsptr;
          }
        }
      }
    }
    obi++;
  }

  lxFileSurvey expf_survey;

  long survnum = 0;
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi).get();
      if (sptr->num1 > 0) {
        sptr->num1 = survnum++;
        expf_survey.m_id = sptr->num1;
        expf_survey.m_parent = ((sptr->fsptr == NULL) || (sptr->fsptr->num1 < 0)) ? sptr->num1 : sptr->fsptr->num1;
        expf_survey.m_namePtr = expf.m_surveysData.AppendStr(sptr->name);
        expf_survey.m_titlePtr = expf.m_surveysData.AppendStr(sptr->title);
        expf.m_surveys.push_back(expf_survey);
      }
    }
    obi++;
  }

  // prejde vsetky body a nastavi im num od 0 po n a exportuje ich
  lxFileStation expf_station;

  survnum = 0;
  thdb1ds * pst;
  for (i = 0; i < nstat; i++) {
    if (stnum[i] > 0) {
    	stnum_orig[i] = stnum[i];
      stnum[i] = survnum;
      pst = &(dbp->db1d.station_vec[i]);
      //fprintf(pltf,"T %ld %ld %s %.3f %.3f %.3f G%s%s%s\n", survnum, pst->survey->num1, pst->name, pst->x, pst->y, pst->z, (pst->flags & TT_STATIONFLAG_ENTRANCE) != 0 ? "E" : "", (pst->flags & TT_STATIONFLAG_FIXED) != 0 ? "F" : "", (pst->flags & TT_STATIONFLAG_CONT) != 0 ? "C" : "");
      
      expf_station.m_c[0] = pst->x;
      expf_station.m_c[1] = pst->y;
      expf_station.m_c[2] = pst->z;
      expf_station.m_id = survnum;
      expf_station.m_surveyId = pst->survey->num1;
      expf_station.m_namePtr = expf.m_stationsData.AppendStr(pst->name);
      expf_station.m_commentPtr = expf.m_stationsData.AppendStr(pst->comment);
      expf_station.m_flags = 0;
      if ((pst->flags & TT_STATIONFLAG_ENTRANCE) != 0)
        expf_station.m_flags |= LXFILE_STATION_FLAG_ENTRANCE;
      if ((pst->flags & TT_STATIONFLAG_FIXED) != 0)
        expf_station.m_flags |= LXFILE_STATION_FLAG_FIXED;
      if ((pst->flags & TT_STATIONFLAG_CONT) != 0)
        expf_station.m_flags |= LXFILE_STATION_FLAG_CONTINUATION;
      if ((pst->flags & TT_STATIONFLAG_UNDERGROUND) == 0)
        expf_station.m_flags |= LXFILE_STATION_FLAG_SURFACE;
      expf.m_stations.push_back(expf_station);

      survnum++;
    }    
  }

  
  // exportuje vsetky zamery ktorych psurveye su oznacene, ak
  // walls tak aj dimensions
  lxFileShot expf_shot;
  tlegs = dbp->db1d.get_tree_legs();
  for(i = 0; i < nlegs; i++, tlegs++) {
    issfc = (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0);
    if ((*tlegs)->survey->is_selected() && ((txsfc && issfc) || (txcav && !issfc))) {
      expf_shot.m_from = stnum[(*tlegs)->leg->from.id - 1];
      expf_shot.m_to = stnum[(*tlegs)->leg->to.id - 1];
      expf_shot.m_surveyId = (*tlegs)->survey->num1;
      expf_shot.m_flags = 0;
      if (((*tlegs)->leg->flags & TT_LEGFLAG_SPLAY) != 0)
        expf_shot.m_flags |= LXFILE_SHOT_FLAG_SPLAY;
      if (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0)
        expf_shot.m_flags |= LXFILE_SHOT_FLAG_SURFACE;
      if (((*tlegs)->leg->flags & TT_LEGFLAG_DUPLICATE) != 0)
        expf_shot.m_flags |= LXFILE_SHOT_FLAG_DUPLICATE;
      if (((*tlegs)->leg->walls != TT_FALSE) && ((this->wallsrc & TT_WSRC_CENTERLINE) != 0)) {
        expf_shot.m_sectionType = LXFILE_SHOT_SECTION_OVAL;
        expf_shot.m_fLRUD[2] = (*tlegs)->leg->from_up;
        expf_shot.m_fLRUD[3] = (*tlegs)->leg->from_down;
        expf_shot.m_fLRUD[0] = (*tlegs)->leg->from_left;
        expf_shot.m_fLRUD[1] = (*tlegs)->leg->from_right;
        expf_shot.m_tLRUD[2] = (*tlegs)->leg->to_up;
        expf_shot.m_tLRUD[3] = (*tlegs)->leg->to_down;
        expf_shot.m_tLRUD[0] = (*tlegs)->leg->to_left;
        expf_shot.m_tLRUD[1] = (*tlegs)->leg->to_right;
        expf_shot.m_threshold = (*tlegs)->leg->vtresh;
      }
      else
        expf_shot.m_sectionType = LXFILE_SHOT_SECTION_NONE;
      expf.m_shots.push_back(expf_shot);
    }
  }
  lxFileSurface expf_sfc;
  lxFileSurfaceBitmap expf_sfcBmp;


  // export povrchov (vsetkych - ak chceme)
  thdb3ddata * tmp3d;
  thsurface * csrf;
  survnum = 0;
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          csrf = ((thsurface*)(*obi).get());
          tmp3d = csrf->get_3d();
          if ((tmp3d != NULL) && (tmp3d->nfaces > 0)) {
            expf_sfc.m_id = survnum;
            expf_sfc.m_width = csrf->grid_nx;
            expf_sfc.m_height = csrf->grid_ny;
            expf_sfc.m_calib[0] = csrf->grid_ox;
            expf_sfc.m_calib[1] = csrf->grid_oy;
            expf_sfc.m_calib[2] = csrf->grid_dx;
            expf_sfc.m_calib[3] = 0.0;
            expf_sfc.m_calib[4] = 0.0;
            expf_sfc.m_calib[5] = csrf->grid_dy;

            lxFileDbl * cdata = new lxFileDbl[csrf->grid_size];
            for(i = 0; i < (unsigned long) csrf->grid_size; i++) {
              cdata[i] = csrf->grid[i];
            }
            expf_sfc.m_dataPtr = expf.m_surfacesData.AppendData((void *) cdata, csrf->grid_size * sizeof(lxFileDbl));
            expf.m_surfaces.push_back(expf_sfc);
            delete [] cdata;
            
            if (csrf->pict_name != NULL) {
              csrf->calibrate();
              expf_sfcBmp.m_surfaceId = survnum;
              switch (csrf->pict_type) {
                case TT_IMG_TYPE_PNG:
                  expf_sfcBmp.m_type = LXFILE_BITMAP_PNG;
                default:
                  expf_sfcBmp.m_type = LXFILE_BITMAP_JPEG;
              }
              expf_sfcBmp.m_dataPtr = expf.m_surfaceBitmapsData.AppendFile(csrf->pict_name);
              expf_sfcBmp.m_calib[0] = csrf->calib_x;
              expf_sfcBmp.m_calib[1] = csrf->calib_y;
              expf_sfcBmp.m_calib[2] = csrf->calib_xx;
              expf_sfcBmp.m_calib[3] = csrf->calib_xy;
              expf_sfcBmp.m_calib[4] = csrf->calib_yx;
              expf_sfcBmp.m_calib[5] = csrf->calib_yy;
              expf.m_surfaceBitmaps.push_back(expf_sfcBmp);
            }
            survnum++;
          }
          break;
      }
      obi++;
    }
  }

  lxFileScrap expf_scrap;
	thdb3ddata * d3d;
  survnum = 0;

  // export stien
  if (((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) && ((this->wallsrc & TT_WSRC_MAPS) != 0)) {
  
    // 3D DATA 
    thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
    thscrap * cs;
    if (!prjid.newprj) {
      thdb.db2d.process_projection(prjid.prj);
      cs = prjid.prj->first_scrap;
      while(cs != NULL) {
        if (cs->fsptr->is_selected()) {
          d3d = cs->get_3d_outline();
          if ((d3d != NULL) && (d3d->nfaces > 0)) {
            expf_scrap.m_id = survnum;
            expf_scrap.m_surveyId = cs->fsptr->num1;
            // points & triangles

            lxFile3Point * pdata = new lxFile3Point [d3d->nvertices];
            thdb3dvx * vxp;
            std::list<lxFile3Angle> tlist;
            lxFile3Angle t3;
            for(i = 0, vxp = d3d->firstvx; vxp != NULL; vxp = vxp->next, i++) {
              pdata[i].m_c[0] = vxp->x;
              pdata[i].m_c[1] = vxp->y;
              pdata[i].m_c[2] = vxp->z;
            }
            expf_scrap.m_numPoints = d3d->nvertices;
            expf_scrap.m_pointsPtr = expf.m_scrapsData.AppendData(pdata, i * sizeof(lxFile3Point));
            thdb3dfc * fcp;
            thdb3dfx * fxp;
            for(i = 0, fcp = d3d->firstfc; fcp != NULL; fcp = fcp->next, i++) {
              switch (fcp->type) {
                case THDB3DFC_TRIANGLE_STRIP:
                  for(j = 0, fxp = fcp->firstfx; fxp->next->next != NULL; j++, fxp = fxp->next) {
                    t3.m_v[0] = fxp->vertex->id;
                    switch (j % 2) {
                      case 0:
                        t3.m_v[1] = fxp->next->vertex->id;
                        t3.m_v[2] = fxp->next->next->vertex->id;
                      default:
                        t3.m_v[2] = fxp->next->vertex->id;
                        t3.m_v[1] = fxp->next->next->vertex->id;
                    }
                    tlist.insert(tlist.end(), t3);
                  }
                  break;
                case THDB3DFC_TRIANGLES:
                  for(j = 0, fxp = fcp->firstfx; fxp != NULL; j++, fxp = fxp->next->next->next) {
                    t3.m_v[0] = fxp->vertex->id;
                    t3.m_v[1] = fxp->next->vertex->id;
                    t3.m_v[2] = fxp->next->next->vertex->id;
                    tlist.insert(tlist.end(), t3);
                  }
                  break;
              }
            }
            lxFile3Angle * tdata = new lxFile3Angle[tlist.size()];
            std::list<lxFile3Angle>::iterator tli;
            for(i = 0, tli = tlist.begin(); tli != tlist.end(); tli++, i++) {
              tdata[i] = *tli;
            }
            expf_scrap.m_num3Angles = tlist.size();
            expf_scrap.m_3AnglesPtr = expf.m_scrapsData.AppendData(tdata, i * sizeof(lxFile3Angle));
            delete [] pdata;
            delete [] tdata;
            expf.m_scraps.push_back(expf_scrap);
            survnum++;
          }
        }
        cs = cs->proj_next_scrap;
      }
    }
    
  } // WALLS  
  
  // SPLAY walls export
  if (((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) && ((this->wallsrc & TT_WSRC_SPLAYS) != 0)) {
    for (size_t ii = 0; ii < nstat; ii++) {
      if (stnum_orig[ii] > 0) {
        pst = &(dbp->db1d.station_vec[ii]);
        d3d = pst->get_3d_outline();
        if ((d3d != NULL) && (d3d->nfaces > 0)) {
          expf_scrap.m_id = survnum;
          expf_scrap.m_surveyId = pst->survey->num1;
          // points & triangles
          lxFile3Point * pdata = new lxFile3Point [d3d->nvertices];
          thdb3dvx * vxp;
          std::list<lxFile3Angle> tlist;
          lxFile3Angle t3;
          for(i = 0, vxp = d3d->firstvx; vxp != NULL; vxp = vxp->next, i++) {
            pdata[i].m_c[0] = vxp->x;
            pdata[i].m_c[1] = vxp->y;
            pdata[i].m_c[2] = vxp->z;
          }
          expf_scrap.m_numPoints = d3d->nvertices;
          expf_scrap.m_pointsPtr = expf.m_scrapsData.AppendData(pdata, i * sizeof(lxFile3Point));
          thdb3dfc * fcp;
          thdb3dfx * fxp;
          for(i = 0, fcp = d3d->firstfc; fcp != NULL; fcp = fcp->next, i++) {
            switch (fcp->type) {
              case THDB3DFC_TRIANGLE_STRIP:
                for(j = 0, fxp = fcp->firstfx; fxp->next->next != NULL; j++, fxp = fxp->next) {
                  t3.m_v[0] = fxp->vertex->id;
                  switch (j % 2) {
                    case 0:
                      t3.m_v[1] = fxp->next->vertex->id;
                      t3.m_v[2] = fxp->next->next->vertex->id;
                    default:
                      t3.m_v[2] = fxp->next->vertex->id;
                      t3.m_v[1] = fxp->next->next->vertex->id;
                  }
                  tlist.insert(tlist.end(), t3);
                }
                break;
              case THDB3DFC_TRIANGLES:
                for(j = 0, fxp = fcp->firstfx; fxp != NULL; j++, fxp = fxp->next->next->next) {
                  t3.m_v[0] = fxp->vertex->id;
                  t3.m_v[1] = fxp->next->vertex->id;
                  t3.m_v[2] = fxp->next->next->vertex->id;
                  tlist.insert(tlist.end(), t3);
                }
                break;
            }
          }
          lxFile3Angle * tdata = new lxFile3Angle[tlist.size()];
          std::list<lxFile3Angle>::iterator tli;
          for(i = 0, tli = tlist.begin(); tli != tlist.end(); tli++, i++) {
            tdata[i] = *tli;
          }
          expf_scrap.m_num3Angles = tlist.size();
          expf_scrap.m_3AnglesPtr = expf.m_scrapsData.AppendData(tdata, i * sizeof(lxFile3Angle));
          delete [] pdata;
          delete [] tdata;
          expf.m_scraps.push_back(expf_scrap);
          survnum++;
        }
      }
    }
  }


  delete [] stnum;
  delete [] stnum_orig;

  expf.ExportLOX(fnm);

  if (expf.m_error.size() > 0) {
    thwarning(("error writing %s",fnm))
    return;
  }

  
#ifdef THDEBUG
#else
  thprintf(" done\n");
  thtext_inline = false;
#endif
}


void thexpmodel::export_kml_file(class thdatabase * dbp)
{
  if (thcfg.outcs == TTCS_LOCAL) {
    thwarning(("data not georeferenced -- unable to export KML file"));
    return;
  }

  FILE * out;
  const char * fnm = this->get_output("cave.kml");
  out = fopen(fnm, "w");
  if (out == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.0\">\n");
  fprintf(out, "<Folder>\n");
  fprintf(out, "<Style id=\"ThSurveyLine\"> <LineStyle> <color>ffffff00</color> <width>1</width> </LineStyle> </Style>\n");
  fprintf(out, "<Style id=\"ThSurveyLineSurf\"> <LineStyle> <color>ffcccccc</color> <width>1</width> </LineStyle> </Style>\n");
  fprintf(out, "<Style id=\"ThEntranceIcon\"> <IconStyle> <Icon> <href>https://therion.speleo.sk/downloads/KmlIconEntrance.png</href> </Icon> <hotSpot x=\"0.5\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\" /> </IconStyle> </Style>\n");
  fprintf(out, "<Icon> <href>https://therion.speleo.sk/downloads/KmlIconModel.png</href> </Icon>\n");
  // VG 250616: TODO change icons above, maybe upload to therion website after testing

  // Get the main survey, which is at level 2 and is different from the fsurveyptr at level 1
  thsurvey * mainsrv = dbp->fsurveyptr;
  thdataobject * obj;
  for(obj = mainsrv->foptr; obj != NULL; obj = obj->nsptr) 
    if (obj->get_class_id() == TT_SURVEY_CMD) {
      mainsrv = (thsurvey *) obj;
      break;
    }

  // Export cave name and description in the selected language
  std::string cavename = ths2txt((strlen(mainsrv->title) > 0) ? mainsrv->title : mainsrv->name, layout->lang);
  cavename = replace_all(cavename, "<br>", "-");
  fprintf(out, "<name><![CDATA[%s]]></name>\n", cavename.c_str());
  double cavedepth = 0;
  if (mainsrv->stat.station_top != NULL) {
    cavedepth = mainsrv->stat.station_top->z - mainsrv->stat.station_bottom->z;
    if (cavedepth > mainsrv->stat.length)
      cavedepth = mainsrv->stat.length;
  }
  layout->units.lang = layout->lang;
  fprintf(out, "<description><![CDATA[%s %s %s<br>%s %s %s]]></description>\n",
      thT("title cave length", layout->lang), layout->units.format_length(mainsrv->stat.length), layout->units.format_i18n_length_units(),
      thT("title cave depth", layout->lang), layout->units.format_length(cavedepth), layout->units.format_i18n_length_units());

  double x, y, z;
  thdb1ds * station;
  size_t nstat = db->db1d.station_vec.size(), i;

  // Export entrances
  if ((this->items & TT_EXPMODEL_ITEM_ENTRANCES) != 0) {
    for(i = 0; i < nstat; i++) {
      station = &(db->db1d.station_vec[i]);
      if ((station->flags & TT_STATIONFLAG_ENTRANCE) != 0) {
        thcs2cs(thcs_get_params(thcfg.outcs), thcs_get_params(TTCS_LONG_LAT),
          station->x, station->y, station->z, x, y, z);
        fprintf(out, "<Placemark>\n");
        fprintf(out, "<styleUrl>#ThEntranceIcon</styleUrl>");
        fprintf(out, "<name><![CDATA[%s]]></name>\n", ths2txt(station->comment, layout->lang).c_str());
        fprintf(out, "<Point> <coordinates>%.14f,%.14f,%.14f</coordinates> </Point>\n", x / THPI * 180.0, y / THPI * 180.0, z);
        fprintf(out, "</Placemark>\n");
      }
    }
  }

  // Export only surface shots in a separate subfolder (placemark)) with distinct color
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    thdb1dl ** tlegs = dbp->db1d.get_tree_legs();
    unsigned long nlegs, last_st, cur_st, numst;
    nlegs = dbp->db1d.get_tree_size();
    nstat = (unsigned long)dbp->db1d.station_vec.size();
    last_st = nstat;
    numst = 0;
    for(i = 0; i < nlegs; i++, tlegs++) {
      if (this->is_leg_exported(*tlegs) && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0)) {
        if (numst == 0) {
          fprintf(out, "<Placemark>\n");
          fprintf(out, "<name>%s</name>\n", thT("surface legs", layout->lang));
          fprintf(out, "<styleUrl>#ThSurveyLineSurf</styleUrl>\n");
          fprintf(out, "<MultiGeometry>\n");
        }
        cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
        if (cur_st != last_st) {
          if (numst > 0)
            fprintf(out,"</coordinates></LineString>\n");
          fprintf(out,"<LineString><coordinates>\n");
          thcs2cs(thcs_get_params(thcfg.outcs), thcs_get_params(TTCS_LONG_LAT),
            dbp->db1d.station_vec[cur_st].x, dbp->db1d.station_vec[cur_st].y, dbp->db1d.station_vec[cur_st].z,
            x, y, z);
          fprintf(out, "\t%.14f,%.14f,%.14f ", x / THPI * 180.0, y / THPI * 180.0, z);
          numst = 1;
        }
        last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
        thcs2cs(thcs_get_params(thcfg.outcs), thcs_get_params(TTCS_LONG_LAT),
          dbp->db1d.station_vec[last_st].x, dbp->db1d.station_vec[last_st].y, dbp->db1d.station_vec[last_st].z,
          x, y, z);
        fprintf(out, "\t%.14f,%.14f,%.14f ", x / THPI * 180.0, y / THPI * 180.0, z);
        numst++;
      }
    }
    if (numst > 0) {
      fprintf(out,"</coordinates></LineString>\n");
      fprintf(out,"</MultiGeometry>\n</Placemark>\n");
    }
  }

  // Export the survey and subsurvey data
  export_kml_survey_file(out, mainsrv);

  fprintf(out, "</Folder>\n</kml>\n");
  fclose(out);

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpmodel::export_kml_survey_file(FILE * out, thsurvey * surv)
{
  if (!surv || (strlen(surv->name) == 0) || !(surv->is_selected()))
    return;

  thdataobject * obj;
  for(obj = surv->foptr; obj != NULL; obj = obj->nsptr) {
    switch (obj->get_class_id()) {
      case TT_SURVEY_CMD: {
        thsurvey * subsurv = (thsurvey *) obj;

        if ((strlen(subsurv->name) == 0) || !(subsurv->is_selected()))
          break;
        std::string surveyname = ths2txt((strlen(subsurv->title) > 0) ? subsurv->title : subsurv->name, layout->lang);
        fprintf(out, "<Folder>\n");
        fprintf(out, "<name><![CDATA[%s]]></name>\n", surveyname.c_str());
        export_kml_survey_file(out, subsurv);
        fprintf(out, "</Folder>\n");  
        break;
      }

      case TT_DATA_CMD: {
        unsigned long last_st, cur_st, numst = 0;
        double x, y, z;
        thdataleg_list::iterator legs;
        thdata * survdata = (thdata *) obj;

        if (survdata->leg_list.empty())  // skip empty data blocks
          break;
        fprintf(out, "<Placemark>\n");
        fprintf(out, "<name>%s</name>\n", survdata->leg_list.front().from.name);
        fprintf(out, "<styleUrl>#ThSurveyLine</styleUrl>\n");
        fprintf(out, "<MultiGeometry>\n");

        // Export underground legs here. Surface ones are already exported in export_kml_file
        last_st = db->db1d.station_vec[survdata->leg_list.back().to.id - 1].uid - 1;
        for(legs = survdata->leg_list.begin(); legs != survdata->leg_list.end(); legs++) {
          bool is_surface = (legs->flags & TT_LEGFLAG_SURFACE) != 0;
          bool is_splay = (legs->flags & TT_LEGFLAG_SPLAY) != 0;

          if ((legs->is_valid && !is_surface) &&
              (((this->items & TT_EXPMODEL_ITEM_SPLAYSHOTS) != 0) || !is_splay)) {
            cur_st = db->db1d.station_vec[legs->from.id - 1].uid - 1;
            if (cur_st != last_st) {
              if (numst > 0)
                fprintf(out, "\n</coordinates></LineString>\n");
              fprintf(out, "<LineString><coordinates>\n");
              thcs2cs(thcs_get_params(thcfg.outcs), thcs_get_params(TTCS_LONG_LAT),
                db->db1d.station_vec[cur_st].x, db->db1d.station_vec[cur_st].y, db->db1d.station_vec[cur_st].z,
                x, y, z);
              fprintf(out, "\t%.14f,%.14f,%.14f ", x / THPI * 180.0, y / THPI * 180.0, z);
              numst = 1;
            }
            if (numst == 0)
              fprintf(out,"<LineString><coordinates>\n");
            last_st = db->db1d.station_vec[legs->to.id - 1].uid - 1;
            thcs2cs(thcs_get_params(thcfg.outcs), thcs_get_params(TTCS_LONG_LAT),
              db->db1d.station_vec[last_st].x, db->db1d.station_vec[last_st].y, db->db1d.station_vec[last_st].z,
              x, y, z);
            fprintf(out, "\t%.14f,%.14f,%.14f ", x / THPI * 180.0, y / THPI * 180.0, z);
            numst++;
          }
        }  // for legs
        if (numst > 0)
          fprintf(out, "\n</coordinates></LineString>\n");
        fprintf(out, "</MultiGeometry>\n</Placemark>\n");
        break;
      }
    } // switch
  } // for object list
}
