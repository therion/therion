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

thexpmodel::thexpmodel() {
  this->format = TT_EXPMODEL_FMT_THERION;
}


void thexpmodel::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_expmodel_opt);
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
  if (this->format != TT_EXPMODEL_FMT_THERION)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expmodel_fmt));
}


void thexpmodel::process_db(class thdatabase * dbp) 
{
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
  int * s_exp = new int [nstat], * cis_exp, leg_flag;
  cis_exp = s_exp; bool is_surface, is_duplicate;
  for(i = 0; i < nstat; i++, *cis_exp = 0, cis_exp++);
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
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
    if (*cis_exp != 0 || 
        ((dbp->db1d.station_vec[i].flags & 
        (TT_STATIONFLAG_ENTRANCE | TT_STATIONFLAG_FIXED)) != 0)) {
      if ((dbp->db1d.station_vec[i].flags & TT_STATIONFLAG_ENTRANCE) != 0)
        *cis_exp |= img_SFLAG_ENTRANCE;
      if ((dbp->db1d.station_vec[i].flags & TT_STATIONFLAG_FIXED) != 0)
        *cis_exp |= img_SFLAG_FIXED;
      stnbuf.strcpy(dbp->db1d.station_vec[i].survey->get_reverse_full_name());
      stnbuf.strcat(".");
      stnbuf.strcat(dbp->db1d.station_vec[i].name);
      img_write_item(pimg, img_LABEL, *cis_exp, stnbuf,
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
    zmin = 0.0, zmax = 0.0, avx, avy;
  
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

#define copy_station_name(stid) \
strncpy(station_name,dbp->db1d.station_vec[stid].name,8); \
station_name[8] = 0

#define toft(x) ((x)/0.3048)
  
  // now let's print header
  fprintf(pltf,"Z\t%f\t%f\t%f\t%f\t%f\t%f\n",xmin-avx,xmax-avx,ymin-avy,ymax-avy,zmin,zmax);
  fprintf(pltf,"STHERION\n");
  fprintf(pltf,"NEXPORT\tD\t3\t12\t1997\tCtherion export\n");
  last_st = nstat;
  tlegs = dbp->db1d.get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected()) {
      cur_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      if (cur_st != last_st) {
        copy_station_name(cur_st);
        fprintf(pltf,"M\t%f\t%f\t%f\tS%s\tP\t-9.9\t-9.9\t-9.9\t-9.9\n",
          toft(dbp->db1d.station_vec[cur_st].y - avy), toft(dbp->db1d.station_vec[cur_st].x - avx), 
          toft(dbp->db1d.station_vec[cur_st].z),station_name);
      }
      last_st = dbp->db1d.station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      copy_station_name(last_st);
      fprintf(pltf,"D\t%f\t%f\t%f\tS%s\tP\t-9.9\t-9.9\t-9.9\t-9.9\n",
        toft(dbp->db1d.station_vec[last_st].y - avy), toft(dbp->db1d.station_vec[last_st].x - avx), 
        toft(dbp->db1d.station_vec[last_st].z),station_name);
    }
  }

  fprintf(pltf,"X\t%f\t%f\t%f\t%f\t%f\t%f\n",xmin - avx,xmax - avx,ymin - avy,ymax - avy,zmin,zmax);
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
  
  thdb3ddata * pgn = dbp->db1d.get_3d();
  avx = (pgn->limits.minx + pgn->limits.maxx) / 2.0;
  avy = (pgn->limits.miny + pgn->limits.maxy) / 2.0;
  avz = (pgn->limits.minz + pgn->limits.maxz) / 2.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;

  // now let's print header
  
  fprintf(pltf,"set xthmvv(model,maxx) %.2f\n",pgn->limits.maxx - avx);
  fprintf(pltf,"set xthmvv(model,maxy) %.2f\n",pgn->limits.maxy - avy);
  fprintf(pltf,"set xthmvv(model,maxz) %.2f\n",pgn->limits.maxz - avz);
  fprintf(pltf,"glDeleteLists $xthmvv(list,model) 1\n");
  fprintf(pltf,"glNewList $xthmvv(list,model) $GL::GL_COMPILE\n");
  fprintf(pltf,"xth_mv_gl_wireframe\n");
  fprintf(pltf,"glColor3f 1.0 1.0 1.0\n");
  pgn->export_thm(pltf);

  
  // 3D DATA 
  fprintf(pltf,"\n\n\n");
  fprintf(pltf,"xth_mv_gl_surface\n");
  thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
  thscrap * cs;
  thdb3ddata * d3d;
  if (!prjid.newprj) {
    thdb.db2d.process_projection(prjid.prj);
    cs = prjid.prj->first_scrap;
    while(cs != NULL) {
      if (cs->fsptr->is_selected()) {
        thprintf("[%s] ", cs->name);
        d3d = cs->get_3d_outline();
        d3d->exp_shift_x = avx;
        d3d->exp_shift_y = avy;
        d3d->exp_shift_z = avz;
        d3d->export_thm(pltf);
      }
      cs = cs->proj_next_scrap;
    }
  }

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
  
  thdb3ddata * pgn = dbp->db1d.get_3d();
  avx = (pgn->limits.minx + pgn->limits.maxx) / 2.0;
  avy = (pgn->limits.miny + pgn->limits.maxy) / 2.0;
  avz = (pgn->limits.minz + pgn->limits.maxz) / 2.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;

  // now let's print header
  fprintf(pltf,"#VRML V2.0 utf8\n\nNavigationInfo {\n\theadlight TRUE\n}\nBackground {\n\tskyColor 0 0 0\n}\n");
  //pgn->export_vrml(pltf);

  
  // 3D DATA 
  thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
  thscrap * cs;
  thdb3ddata * d3d;
  if (!prjid.newprj) {
    thdb.db2d.process_projection(prjid.prj);
    cs = prjid.prj->first_scrap;
    while(cs != NULL) {
      if (cs->fsptr->is_selected()) {
        thprintf("[%s] ", cs->name);
        d3d = cs->get_3d_outline();
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
  
  thdb3ddata * pgn = dbp->db1d.get_3d();
  avx = (pgn->limits.minx + pgn->limits.maxx) / 2.0;
  avy = (pgn->limits.miny + pgn->limits.maxy) / 2.0;
  avz = (pgn->limits.minz + pgn->limits.maxz) / 2.0;
  pgn->exp_shift_x = avx;
  pgn->exp_shift_y = avy;
  pgn->exp_shift_z = avz;

  // now let's print header
  
  fprintf(pltf,"3DMetafile ( 1 0 Stream nextTOC> ) \n");
  //pgn->export_thm(pltf);

  
  // 3D DATA 
  thdb2dprjpr prjid = dbp->db2d.parse_projection("plan",false);
  thscrap * cs;
  thdb3ddata * d3d;
  if (!prjid.newprj) {
    thdb.db2d.process_projection(prjid.prj);
    cs = prjid.prj->first_scrap;
    while(cs != NULL) {
      if (cs->fsptr->is_selected()) {
        thprintf("[%s] ", cs->name);
        d3d = cs->get_3d_outline();
        d3d->exp_shift_x = avx;
        d3d->exp_shift_y = avy;
        d3d->exp_shift_z = avz;
        fprintf(pltf,"BeginGroup ( \n\tDisplayGroup ( ) \n) \n"
          "Container ( \n\tMesh ( \n");
        d3d->export_3dmf(pltf);
        fprintf(pltf,"\t) \nContainer ( \n\tAttributeSet ( ) \n"
          "\tDiffuseColor ( 1 1 1 ) \n) \n) \nEndGroup ( ) \n");
      }
      cs = cs->proj_next_scrap;
    }
  }
  fprintf(pltf,"\n");
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}

