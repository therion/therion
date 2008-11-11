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
#include "extern/lxFile.h"
#include "thsurface.h"
#include "thchenc.h"
#include "thconfig.h"
#include "thcsdata.h"
#include "thproj.h"


thexpmodel::thexpmodel() {
  this->format = TT_EXPMODEL_FMT_UNKNOWN;
  this->items = TT_EXPMODEL_ITEM_ALL;
  this->wallsrc = TT_WSRC_ALL;
  this->encoding = TT_UTF_8;
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
  img_output_version = 4;
  thbuffer fnmb;
  fnmb.strcpy("cave");
  pimg = img_open_write(fnm, fnmb.get_buffer(), 1);
     
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
          srfc = ((thsurface*)(*obi));
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
                    fread((void *) cdata, 1, fsz, xf);
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
  diamxy = hypot(diamx, diamy);
  diamxz = hypot(diamx, diamz);
  diamyz = hypot(diamy, diamz);
  diam = hypot(diamxy, diamz);
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
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif

  lxFile expf;
      
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
    nstat = (unsigned long)dbp->db1d.station_vec.size(), i, j;
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();
  long * stnum = NULL;
  if (nstat > 0) {
    stnum = new long[nstat];
    for (i = 0; i < nstat; i++)
      stnum[i] = -1; //(dbp->db1d.station_vec[i].survey->is_selected() ? 1 : -1);
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

  lxFileSurvey expf_survey;

  long survnum = 0;
  obi = dbp->object_list.begin();
  while (obi != dbp->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      sptr = (thsurvey*)(*obi);
      if (sptr->num1 > 0) {
        sptr->num1 = survnum++;

        expf_survey.m_id = sptr->num1;
        expf_survey.m_parent = (sptr->fsptr != NULL) ? sptr->fsptr->num1 : sptr->num1;
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
          csrf = ((thsurface*)(*obi));
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
  survnum = 0;

  // export stien
  if (((this->items & TT_EXPMODEL_ITEM_WALLS) != 0) && ((this->wallsrc & TT_WSRC_MAPS) != 0)) {
  
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
  
  if (stnum != NULL)
    delete [] stnum;

  expf.ExportLOX(fnm);

  if (expf.m_error.size() > 0) {
    thwarning(("error writing %s",fnm))
    return;
  }

  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}






void thexpmodel::export_kml_file(class thdatabase * dbp)
{

  unsigned long nlegs = dbp->db1d.get_tree_size(),
    nstat = (unsigned long)dbp->db1d.station_vec.size();
  thdb1dl ** tlegs = dbp->db1d.get_tree_legs();

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

  fprintf(out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.0\">\n<Document>\n");
  fprintf(out,"<name>Therion KML export</name>\n<description>Therion KML export.</description>\n");


  unsigned long last_st = nstat, cur_st, numst, i;
  double x, y, z;

  fprintf(out,"<Placemark>\n");
  fprintf(out,"<Style>\n<LineStyle>\n<color>ffffff00</color>\n<width>1</width>\n</LineStyle>\n</Style>\n");
  fprintf(out,"<MultiGeometry>\n");

  numst = 0;
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) == 0)) {
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      if (cur_st != last_st) {
        if (numst > 0)
          fprintf(out,"</coordinates>\n</LineString>\n");
        fprintf(out,"<LineString>\n<coordinates>\n");
        thcs2cs(thcsdata_table[thcfg.outcs].params, thcsdata_table[TTCS_LONG_LAT].params, 
          dbp->db1d.station_vec[cur_st].x, dbp->db1d.station_vec[cur_st].y, dbp->db1d.station_vec[cur_st].z,
          x, y, z);
        fprintf(out, "\t%20.14f,%20.14f,%20.14f\n", x / THPI * 180.0, y / THPI * 180.0, z);
        numst = 1;
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      thcs2cs(thcsdata_table[thcfg.outcs].params, thcsdata_table[TTCS_LONG_LAT].params, 
        dbp->db1d.station_vec[last_st].x, dbp->db1d.station_vec[last_st].y, dbp->db1d.station_vec[last_st].z,
        x, y, z);
      fprintf(out, "\t%20.14f,%20.14f,%20.14f\n", x / THPI * 180.0, y / THPI * 180.0, z);
      numst++;
    }
  }

  if (numst > 0)
    fprintf(out,"</coordinates>\n</LineString>\n");
  fprintf(out,"</MultiGeometry>\n");
  fprintf(out,"</Placemark>\n</Document>\n</kml>\n");
  fclose(out);
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}





