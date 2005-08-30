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

thexpmodel::thexpmodel() {
  this->format = TT_EXPMODEL_FMT_UNKNOWN;
  this->items = TT_EXPMODEL_ITEM_ALL;
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


void thexpmodel::dump_body(FILE * xf)
{
  thexport::dump_body(xf);
  if (this->format != TT_EXPMODEL_FMT_UNKNOWN)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expmodel_fmt));
}


void thexpmodel::process_db(class thdatabase * dbp) 
{
  if (this->format == TT_EXPMODEL_FMT_UNKNOWN) {
    this->format = TT_EXPMODEL_FMT_THERION;
    thexp_set_ext_fmt(".plt", TT_EXPMODEL_FMT_COMPASS)
    thexp_set_ext_fmt(".3d", TT_EXPMODEL_FMT_SURVEX)
    thexp_set_ext_fmt(".wrl", TT_EXPMODEL_FMT_VRML)
    thexp_set_ext_fmt(".vrml", TT_EXPMODEL_FMT_VRML)
    thexp_set_ext_fmt(".3dmf", TT_EXPMODEL_FMT_3DMF)
    thexp_set_ext_fmt(".dxf", TT_EXPMODEL_FMT_DXF)
    thexp_set_ext_fmt(".tlx", TT_EXPMODEL_FMT_TLX)
  }  
  switch (this->format) {
    case TT_EXPMODEL_FMT_SURVEX:
      this->export_3d_file(dbp);
      break;
    case TT_EXPMODEL_FMT_COMPASS:
      this->export_plt_file(dbp);
      break;
    case TT_EXPMODEL_FMT_THERION:
      this->export_thm_file(dbp);
      break;
    case TT_EXPMODEL_FMT_3DMF:
      this->export_3dmf_file(dbp);
      break;
    case TT_EXPMODEL_FMT_VRML:
      this->export_vrml_file(dbp);
      break;
    case TT_EXPMODEL_FMT_DXF:
      this->export_dxf_file(dbp);
      break;
    case TT_EXPMODEL_FMT_TLX:
      this->export_tlx_file(dbp);
      break;
  }
}


void thexpmodel::export_3d_file(class thdatabase * dbp)
{

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = dbp->db1d.station_vec.size();
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();

  if (nlegs == 0)
    return;
  
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.3d";
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  unsigned long i;
  img * pimg;

  pimg = img_open_write(fnm, "cave", 1);
     
  if (!pimg) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  unsigned long last_st = nstat, cur_st;
  int * s_exp = new int [nstat], * cis_exp, leg_flag, x_exp;
  cis_exp = s_exp; 
  bool is_surface, is_duplicate;
  for(i = 0; i < nstat; i++, *cis_exp = 0, cis_exp++);
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
//      thprintf("EXP: %s@%s - %s@%s\n", (*tlegs)->leg->from.name, dbp->db1d.station_vec[(*tlegs)->leg->from.id -1].survey->full_name,  (*tlegs)->leg->to.name, dbp->db1d.station_vec[(*tlegs)->leg->to.id -1].survey->full_name);
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      is_surface = (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0);
      is_duplicate = (((*tlegs)->leg->flags & TT_LEGFLAG_DUPLICATE) != 0);
      if (is_surface)
        s_exp[cur_st] |= img_SFLAG_SURFACE;
      else
        s_exp[cur_st] |= img_SFLAG_UNDERGROUND;
      if (cur_st != last_st) {
        img_write_item(pimg, img_MOVE, 0, NULL, 
          dbp->db1d.station_vec[cur_st].x, dbp->db1d.station_vec[cur_st].y, dbp->db1d.station_vec[cur_st].z);
        //thprintf("move to %d\n",cur_st);
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
      
      img_write_item(pimg, img_LINE, leg_flag, (*tlegs)->survey->get_reverse_full_name(),
          dbp->db1d.station_vec[last_st].x, dbp->db1d.station_vec[last_st].y, dbp->db1d.station_vec[last_st].z);
      //thprintf("line to %d\n",last_st);
    }
  }

  cis_exp = s_exp;
  thbuffer stnbuf;
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
      if ((dbp->db1d.station_vec[i].flags & TT_STATIONFLAG_ENTRANCE) != 0)
        x_exp |= img_SFLAG_ENTRANCE;
      if ((dbp->db1d.station_vec[i].flags & TT_STATIONFLAG_FIXED) != 0)
        x_exp |= img_SFLAG_FIXED;
      stnbuf.strcpy(dbp->db1d.station_vec[i].survey->get_reverse_full_name());
      stnbuf.strcat(".");
      stnbuf.strcat(dbp->db1d.station_vec[i].name);
      img_write_item(pimg, img_LABEL, x_exp, stnbuf,
          dbp->db1d.station_vec[i].x, dbp->db1d.station_vec[i].y, dbp->db1d.station_vec[i].z);
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
 
void thexpmodel::export_plt_file(class thdatabase * dbp)
{

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = dbp->db1d.station_vec.size();
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();

  if (nlegs == 0)
    return;
  
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.plt";
  
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
    zmin = 0.0, zmax = 0.0, avx, avy, avz;
  
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
  
  avx = (xmin + xmax) / 2.0;
  avy = (ymin + ymax) / 2.0;
  avz = (zmin + zmax) / 2.0;

#define copy_station_name(stid) \
strncpy(station_name,dbp->db1d.station_vec[stid].name,8); \
station_name[8] = 0

#define toft(x) ((x)/0.3048)

#define exppltdim(ffd,ttd) (thisnan((*tlegs)->reverse ? (*tlegs)->leg->ffd : (*tlegs)->leg->ttd) ? -9999.0 : toft((*tlegs)->reverse ? (*tlegs)->leg->ffd : (*tlegs)->leg->ttd))
  
  // now let's print header
  fprintf(pltf,"Z\t%f\t%f\t%f\t%f\t%f\t%f\n\n",toft(ymin-avy),toft(ymax-avy),toft(xmin-avx),toft(xmax-avx),toft(zmin),toft(zmax));
//  fprintf(pltf,"Z\t%f\t%f\t%f\t%f\t%f\t%f\n",toft(ymin-avy),toft(ymax-avy),toft(xmin-avx),toft(xmax-avx),toft(zmin-avz),toft(zmax-avz));
  fprintf(pltf,"STHERION\n");
  fprintf(pltf,"NX\tD\t3\t12\t1997\tCtherion export\n");
  last_st = nstat;
  tlegs = dbp->db1d.get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      if (cur_st != last_st) {
        copy_station_name(cur_st);
        fprintf(pltf,"M\t%f\t%f\t%f\tS%s\tP\t%f\t%f\t%f\t%f\n",
          toft(dbp->db1d.station_vec[cur_st].y - avy), toft(dbp->db1d.station_vec[cur_st].x - avx), 
          toft(dbp->db1d.station_vec[cur_st].z),station_name,
          exppltdim(to_left, from_left), 
          exppltdim(to_up, from_up), 
          exppltdim(to_down, from_down), 
          exppltdim(to_right, from_right));
//          toft(dbp->db1d.station_vec[cur_st].z - avz),station_name);
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      copy_station_name(last_st);
      fprintf(pltf,"D\t%f\t%f\t%f\tS%s\tP\t%f\t%f\t%f\t%f\n",
        toft(dbp->db1d.station_vec[last_st].y - avy), toft(dbp->db1d.station_vec[last_st].x - avx), 
        toft(dbp->db1d.station_vec[last_st].z),station_name,
        exppltdim(from_left, to_left), 
        exppltdim(from_up, to_up), 
        exppltdim(from_down, to_down), 
        exppltdim(from_right, to_right));
//        toft(dbp->db1d.station_vec[last_st].z - avz),station_name);
    }
  }

  fprintf(pltf,"X\t%f\t%f\t%f\t%f\t%f\t%f\n\x1A",toft(ymin-avy),toft(ymax-avy),toft(xmin-avx),toft(xmax-avx),toft(zmin),toft(zmax));
//  fprintf(pltf,"X\t%f\t%f\t%f\t%f\t%f\t%f\n",toft(ymin-avy),toft(ymax-avy),toft(xmin-avx),toft(xmax-avx),toft(zmin-avz),toft(zmax-avz));
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}
  
void thexpmodel::export_thm_file(class thdatabase * dbp)
{

  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.thm";
  
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

    d3d = dbp->db1d.get_3d_walls();
    finlim.update(&(d3d->limits));
    d3d->exp_shift_x = avx;
    d3d->exp_shift_y = avy;
    d3d->exp_shift_z = avz;
    d3d->export_thm(pltf);

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


void thexpmodel::export_vrml_file(class thdatabase * dbp) {
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
#ifdef THWIN32
    fnm = "cave.wrl";
#else
    fnm = "cave.vrml";
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




  double avx, avy, avz;
  thdb_object_list_type::iterator obi;
  thdb3ddata * pgn = dbp->db1d.get_3d(), 
    * surf_pgn = dbp->db1d.get_3d_surface(),
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
  fprintf(pltf,"#VRML V2.0 utf8\n\nNavigationInfo {\n\theadlight TRUE\n}\nBackground {\n\tskyColor 0 0 0\n}\n");

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
            fprintf(pltf,
              "Shape {\nappearance Appearance {\n" \
              "\tmaterial Material {\n\t\tdiffuseColor 0.3 1.0 0.1\n\t\ttransparency 0.5\n\t}" \
              "\n}\ngeometry IndexedFaceSet {\n\tsolid FALSE\n\tcreaseAngle 3.0\n");
            tmp3d->export_vrml(pltf);
            finlim.update(&(tmp3d->limits));
            fprintf(pltf,"\n}\n}\n");
          }
          break;
      }
      obi++;
    }
  }



  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {
  
    // 3D DATA 
    thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
    thscrap * cs;
    thdb3ddata * d3d;
    
    d3d = dbp->db1d.get_3d_walls();
    finlim.update(&(d3d->limits));
    d3d->exp_shift_x = avx;
    d3d->exp_shift_y = avy;
    d3d->exp_shift_z = avz;
    fprintf(pltf,
      "Shape {\nappearance Appearance {\n" \
      "\tmaterial Material {\n\t\tdiffuseColor 1.0 1.0 1.0\n\t}" \
      "\n}\ngeometry IndexedFaceSet {\n");
    d3d->export_vrml(pltf);
    fprintf(pltf,"creaseAngle 3.0\n}\n}\n");
    
    
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
    
  } // WALLS
  

  fprintf(pltf,
     "Shape {\nappearance Appearance {\n" \
     "\tmaterial Material {\n\t\temissiveColor 1.0 0.0 0.0\n\t}" \
     "\n}\ngeometry IndexedLineSet {\n");

  fprintf(pltf,"coord Coordinate {\n\tpoint [\n");
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx - avx, finlim.maxy - avy, finlim.minz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx - avx, finlim.maxy - avy, finlim.minz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx - avx, finlim.miny - avy, finlim.minz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx - avx, finlim.miny - avy, finlim.maxz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.minx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx - avx, finlim.maxy - avy, finlim.maxz - avz);
  fprintf(pltf,"%8.2f %8.2f %8.2f,\n",finlim.maxx - avx, finlim.miny - avy, finlim.maxz - avz);
  fprintf(pltf,"\t]\n}\n");
  fprintf(pltf,"\tcoordIndex [\n");
  fprintf(pltf,"0, 1, 2, 3, 0, 4, 5, 6, 7, 4, -1,\n");
  fprintf(pltf,"1, 5, -1,\n");
  fprintf(pltf,"2, 6, -1,\n");
  fprintf(pltf,"3, 7, -1,\n");
  fprintf(pltf,"\t]\n");

  fprintf(pltf,"\n}\n}\n");



  fprintf(pltf,"\n");
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpmodel::export_3dmf_file(class thdatabase * dbp) {
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.3dmf";
  
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
          tmp3d = ((thsurface*)(*obi))->get_3d();
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

    thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
    thscrap * cs;
    thdb3ddata * d3d;

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
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.dxf";
  
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
  if ((this->items & TT_EXPMODEL_ITEM_SURFACECENTERLINE) != 0) {
    surf_pgn->export_dxf(pltf,"SURFACE_CENTERLINE");
  }

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
    thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
    thscrap * cs;
    thdb3ddata * d3d;
    
    d3d = dbp->db1d.get_3d_walls();
    finlim.update(&(d3d->limits));
    d3d->exp_shift_x = avx;
    d3d->exp_shift_y = avy;
    d3d->exp_shift_z = avz;
    d3d->export_dxf(pltf,"WALLS");
    
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



void thexpmodel::export_tlx_file(class thdatabase * dbp) {
  char * fnm;  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.tlx";

  thdb_object_list_type::iterator obi;
  
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

  thsurvey * sptr, * tsptr;

  // vsetkym surveyom nastavi num1 na -1 ak nie su, 1 ak
  // su oznacene   
  
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi);
      if (sptr->is_selected()) {
        sptr->num1 = 1;
      } else {
        sptr->num1 = -1;
      }
    }
    obi++;
  }

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = dbp->db1d.station_vec.size(), i;
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();
  long * stnum = NULL;
  if (nstat > 0) {
    stnum = new long[nstat];
    for (i = 0; i < nstat; i++)
      stnum[i] = (dbp->db1d.station_vec[i].survey->is_selected() ? 1 : -1);
  }

  // prejde vsetky zamery, ktore ideme 
  // exportovat a parent surveyom bodov tychto zamer nastavi num1 na 1
  // rovnako aj bodom num1 na 1
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      stnum[(*tlegs)->leg->from.id - 1] = 1;
      dbp->db1d.station_vec[(*tlegs)->leg->from.id - 1].survey->num1 = 1;
      stnum[(*tlegs)->leg->to.id - 1] = 1;
      dbp->db1d.station_vec[(*tlegs)->leg->to.id - 1].survey->num1 = 1;
    }
  }
  
  // opat prejde vsetky surveye - doplni medziclanky a nastavi num1 
  // nastavi od 0 po n - exportuje ich
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
    }
    obi++;
  }


  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi);
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


  long survnum = 0;
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi);
      if (sptr->num1 > 0) {
        sptr->num1 = survnum++;
        fprintf(pltf,"S %d %d %s\n", sptr->num1, (sptr->fsptr != NULL) ? sptr->fsptr->num1 : -1, sptr->name);
        if (strlen(sptr->title)) {
          fprintf(pltf,"C %s\n", sptr->title);
        }
      }
    }
    obi++;
  }


  // prejde vsetky body a nastavi im num od 0 po n a exportuje ich
  survnum = 0;
  thdb1ds * pst;
  for (i = 0; i < nstat; i++) {
    if (stnum[i] > 0) {
      stnum[i] = survnum;
      pst = &(dbp->db1d.station_vec[i]);
      fprintf(pltf,"T %d %d %s %.3f %.3f %.3f G%s%s%s\n", survnum, pst->survey->num1, pst->name, pst->x, pst->y, pst->z, (pst->flags & TT_STATIONFLAG_ENTRANCE) != 0 ? "E" : "", (pst->flags & TT_STATIONFLAG_FIXED) != 0 ? "F" : "", (pst->flags & TT_STATIONFLAG_CONT) != 0 ? "C" : "");
      if (pst->comment != NULL) {
        fprintf(pltf,"C %s\n", pst->comment);
      }
      survnum++;
    }    
  }

  // exportuje vsetky zamery ktorych psurveye su oznacene, ak
  // walls tak aj dimensions
  tlegs = dbp->db1d.get_tree_legs();
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      fprintf(pltf,"H %d %d %d G%s%s\n", 
          stnum[(*tlegs)->leg->from.id - 1],
          stnum[(*tlegs)->leg->to.id - 1],
          (*tlegs)->survey->num1,
          ((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0 ? "S" : "", 
          ((*tlegs)->leg->flags & TT_LEGFLAG_DUPLICATE) != 0 ? "D" : ""
        );
      if (((*tlegs)->leg->walls != TT_FALSE) && ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0))
        fprintf(pltf,"D %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
          (*tlegs)->leg->from_up, (*tlegs)->leg->from_down, (*tlegs)->leg->from_left, (*tlegs)->leg->from_right,
          (*tlegs)->leg->to_up, (*tlegs)->leg->to_down, (*tlegs)->leg->to_left, (*tlegs)->leg->to_right);
    }
  }

  // export povrchov (vsetkych - ak chceme)
  thdb3ddata * tmp3d;
  if ((this->items & TT_EXPMODEL_ITEM_SURFACE) != 0) {
    // prejde secky surfaces a exportuje z nich povrchy
    obi = dbp->object_list.begin();
    while (obi != dbp->object_list.end()) {
      switch ((*obi)->get_class_id()) {
        case TT_SURFACE_CMD:
          tmp3d = ((thsurface*)(*obi))->get_3d();
          if ((tmp3d != NULL) && (tmp3d->nfaces > 0)) {
            fprintf(pltf,"X 1 %d\n", ((thsurface*)(*obi))->fsptr->num1);
            tmp3d->exp_shift_x = 0.0;
            tmp3d->exp_shift_y = 0.0;
            tmp3d->exp_shift_z = 0.0;
            tmp3d->export_tlx(pltf);
          }
          break;
      }
      obi++;
    }
  }


  // export stien
  if ((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) {
  
    // 3D DATA 
    thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
    thscrap * cs;
    thdb3ddata * d3d;
    
    if (!prjid.newprj) {
      thdb.db2d.process_projection(prjid.prj);
      cs = prjid.prj->first_scrap;
      while(cs != NULL) {
        if (cs->fsptr->is_selected()) {
          d3d = cs->get_3d_outline();
          if ((d3d != NULL) && (d3d->nfaces > 0)) {
            d3d->exp_shift_x = 0.0;
            d3d->exp_shift_y = 0.0;
            d3d->exp_shift_z = 0.0;
            fprintf(pltf,"X 0 %d\n", cs->fsptr->num1);
            d3d->export_tlx(pltf);
          }
        }
        cs = cs->proj_next_scrap;
      }
    }
    
  } // WALLS
  
  fclose(pltf);
  
  if (stnum != NULL)
    delete [] stnum;
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


