/**
 * @file thexpmap.cxx
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
 
#include "thexpmap.h"
#include "thexporter.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdb2d.h"
#include "thdb2dmi.h"
#include "thtmpdir.h"
#include "thscrap.h"
#include "thpoint.h"
#include "thline.h"
#include "thlayout.h"
#include "thmap.h"
#include "thsketch.h"
#include "thconfig.h"
#include <stdio.h>
#include "thtmpdir.h"
#ifndef THMSVC
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#define putenv _putenv
#define hypot _hypot
#endif
#include "thchenc.h"
#include "thdb1d.h"
#include "thinit.h"
#include "thlogfile.h"
#include "thconvert.h"
#include "thpdf.h"
#include "thpdfdata.h"
#include "thmpost.h"
#include "thtex.h"   
#include "thcmdline.h"
#include "thtexfonts.h"
#include "thsurvey.h"
#include "thlegenddata.h"
#include <fstream>
#include <map>
#include <set>
#include "thmapstat.h"
#include "thsurface.h"
#include <stdlib.h>
#include "extern/lxMath.h"
#include "thsvg.h"
#include "extern/img.h"

#ifdef THMSVC
#define snprintf _snprintf
#define strcasecmp _stricmp
#endif

thexpmap::thexpmap() {
  this->format = TT_EXPMAP_FMT_UNKNOWN;
  this->projstr = "plan";
  this->layoutstr = "";
  this->layout = new thlayout;
  this->layout->assigndb(&thdb);
  this->layout->id = ++thdb.objid;
  this->projptr = NULL;
  this->encoding = TT_UTF_8;
}

thexpmap::~thexpmap() {
  delete this->layout;
}

void thexpmap_log_log_file(char * logfpath, char * on_title, char * off_title, bool mpbug = false) {
  char * lnbuff = new char [4097];
//  unsigned long lnum = 0;
  thlog.printf("%s",on_title);
  std::ifstream lf(logfpath);
  if (!(lf.is_open())) {{
    thwarning(("can't open %s file for input", logfpath));
    }
    thlog.printf("can't open %s file for input",logfpath);
    thlog.printf("%s",off_title);
    delete [] lnbuff;
    return;
  }
  // let's read line by line and print to log file
  bool skip_next = false, skip_this = false, peoln = false;
  while (!(lf.eof())) {
    lf.getline(lnbuff,4096);
    if (mpbug && (!skip_this)) {
      if (strncmp(lnbuff,"write",5) == 0) {
        skip_next = true;
        skip_this = true;
        peoln = false;
      }
    }
    if (!skip_this) {
      if (!skip_next) {
        thlog.printf("%s%s", (peoln ? "\n" : ""), lnbuff);
        peoln = true;
      } else {
        skip_next = false;
      }
    } else {
      skip_this = false;
    } 
  }
  if (peoln) 
    thlog.printf("\n");
  lf.close();
  delete [] lnbuff;
  thlog.printf("%s",off_title);
}


void thexpmap::parse_options(int & argx, int nargs, char ** args)
{

  int optid, optx; //,sv;
//  double dv;
  bool supform;
  char * fmt;
  //if (argx == 0) {
  //  optid = TT_EXPMAP_OPT_PROJECTION;
  //  optx = argx;
  //}
  //else {
  optid = thmatch_token(args[argx], thtt_expmap_opt);
  optx = argx;
  //}
  switch (optid) {
    case TT_EXPMAP_OPT_FORMAT:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing format -- \"%s\"",args[optx]))
      this->format = thmatch_token(args[argx], thtt_expmap_fmt);
      fmt = args[argx];
      if (this->format == TT_EXPMAP_FMT_UNKNOWN)
        ththrow(("unknown format -- \"%s\"", args[argx]))
      argx++;
      supform = false;
      switch (this->export_mode) {
        case TT_EXP_MAP:
          switch (this->format) {
            case TT_EXPMAP_FMT_PDF:
            case TT_EXPMAP_FMT_SVG:
            case TT_EXPMAP_FMT_XHTML:
            case TT_EXPMAP_FMT_XVI:
            case TT_EXPMAP_FMT_3D:
            case TT_EXPMAP_FMT_KML:
            case TT_EXPMAP_FMT_SHP:
              supform = true;
          }
          break;
        case TT_EXP_ATLAS:
          switch (this->format) {
            case TT_EXPMAP_FMT_PDF:
              supform = true;
          }
          break;
      }
      if (!supform)
        ththrow(("format not supported -- %s",fmt))
      break;
    case TT_EXPMAP_OPT_ENCODING:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing encoding -- \"%s\"",args[optx]))
      this->encoding = thmatch_token(args[argx], thtt_encoding);
      if (this->encoding == TT_UNKNOWN_ENCODING)
        ththrow(("unknown encoding -- \"%s\"", args[argx]))
      argx++;
      break;
    case TT_EXPMAP_OPT_PROJECTION:
      argx++;
      if (argx >= nargs)
        ththrow(("missing projection specification -- \"%s\"",args[optx]))
      this->projstr = thdb.strstore(args[argx],true);
      argx++;
      break;
    case TT_EXPMAP_OPT_LAYOUT:
      argx++;
      if (argx >= nargs)
        ththrow(("missing layout -- \"%s\"",args[optx]))
      this->layout->set(thcmd_option_desc(TT_LAYOUT_COPY), &(args[argx]), this->cfgptr->cfg_file.get_cif_encoding(), 0); // = thdb.strstore(args[argx],true);
      this->layoutopts += " -layout ";
      thencode(&(this->cfgptr->bf1), args[argx], this->cfgptr->cfg_file.get_cif_encoding());
      thdecode_arg(&(this->cfgptr->bf2), this->cfgptr->bf1.get_buffer());
      this->layoutopts += this->cfgptr->bf2.get_buffer();
      argx++;
      break;
    default:
      // skusi ci je to -layout-xxx
      if (strncmp(args[optx],"-layout-",8) == 0)
        this->parse_layout_option(argx, nargs, args);
      else
        thexport::parse_options(argx, nargs, args);
      break;
  }
}

void thexpmap::parse_layout_option(int & argx, int nargs, char ** args) {

  // ak ano, tak skontroluje ich pocet
  // pouzije set na nastavenie layoutu
  // ulozi ich - pozor na dekodovanie
  int i;
  char * opts = &((args[argx])[8]);
  this->layout->m_pconfig = this->cfgptr;
  thcmd_option_desc o = this->layout->get_cmd_option_desc(opts);
  
  if (o.id == TT_DATAOBJECT_UNKNOWN)
    ththrow(("unknown layout option -- -%s", opts));
    
  if (argx + o.nargs >= nargs)
    ththrow(("not enough layout option arguments -- -%s", opts));
  
  argx++;  
  
  this->layout->set(o, &(args[argx]), this->cfgptr->cfg_file.get_cif_encoding(), 0);
  this->layoutopts += " -layout-";
  this->layoutopts += opts;

  for (i = 0; i < o.nargs; i++) {
    thencode(&(this->cfgptr->bf1), args[argx], this->cfgptr->cfg_file.get_cif_encoding());
    thdecode_arg(&(this->cfgptr->bf2), this->cfgptr->bf1.get_buffer());
    this->layoutopts += " ";
    this->layoutopts += this->cfgptr->bf2.get_buffer();
    argx++;
  }  
 
}

void thexpmap::dump_header(FILE * xf)
{
  thexport::dump_header(xf);
}


void thexpmap::dump_body(FILE * xf)
{
  if (strcmp(this->projstr,"plan") != 0)
    fprintf(xf," -projection %s",this->projstr);
  thexport::dump_body(xf);
  if (this->format != TT_EXPMAP_FMT_UNKNOWN)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expmap_fmt));
  thdecode(&(this->cfgptr->bf1), this->cfgptr->cfg_fenc, this->layoutopts.get_buffer());
  fprintf(xf,"%s",this->cfgptr->bf1.get_buffer());
}


void thexpmap::parse_projection(class thdatabase * dbp) {

  thdb2dprjpr prjid = dbp->db2d.parse_projection(this->projstr,true);
/*  if (prjid.newprj) {
    thwarning(("%s [%d] -- no projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }*/
  this->projptr = prjid.prj;
  
}



void thexpmap::process_db(class thdatabase * dbp) 
{
  
  if (this->projptr == NULL)
    return;

  this->db = dbp;

  if (this->format == TT_EXPMAP_FMT_UNKNOWN) {
    this->format = TT_EXPMAP_FMT_PDF;
    thexp_set_ext_fmt(".pdf", TT_EXPMAP_FMT_PDF)
    if (this->export_mode == TT_EXP_MAP) {
      thexp_set_ext_fmt(".svg", TT_EXPMAP_FMT_SVG)
      thexp_set_ext_fmt(".xhtml", TT_EXPMAP_FMT_XHTML)
      thexp_set_ext_fmt(".xvi", TT_EXPMAP_FMT_XVI)
      thexp_set_ext_fmt(".3d", TT_EXPMAP_FMT_3D)
      thexp_set_ext_fmt(".shp", TT_EXPMAP_FMT_SHP)
      thexp_set_ext_fmt(".kml", TT_EXPMAP_FMT_KML)
    }
  }

  if (this->format != TT_EXPMAP_FMT_XVI)
    thdb.db2d.process_projection(this->projptr);
  this->layout->process_copy();
  
  // the export it self
  switch (this->format) {
    case TT_EXPMAP_FMT_3D:
      this->export_uni(thdb.db2d.select_projection(this->projptr),this->projptr);
      break;
    case TT_EXPMAP_FMT_PDF:
    case TT_EXPMAP_FMT_XHTML:
    case TT_EXPMAP_FMT_SVG:
      this->export_pdf(thdb.db2d.select_projection(this->projptr),this->projptr);
      break;
    case TT_EXPMAP_FMT_SHP:
      this->export_shp(thdb.db2d.select_projection(this->projptr),this->projptr);
      break;
    case TT_EXPMAP_FMT_KML:
      this->export_kml(thdb.db2d.select_projection(this->projptr),this->projptr);
      break;
    case TT_EXPMAP_FMT_XVI:
      this->export_xvi(this->projptr);
      break;
  }
  
  //if (tmp != NULL) {
  //  tmp->self_delete();
  //}
}



char * thexpmap_u2string(unsigned u) {
  static char a [8];
  a[7] = 0;
  unsigned v = 1;
  for(int i = 6; i >= 0; i--) {
    a[i] = 'a' + ((u / v) % 10);
    v *= 10;
  }
  return (&(a[0]));
}

		

#define layoutnan(XXX,VVV) \
  if (thisnan(this->layout->XXX)) this->layout->XXX = (VVV)    


void thexpmap::export_xvi(class thdb2dprj * prj)
{
  char * fnm = this->get_output("cave.xvi");  
    
  switch (prj->type) {
    case TT_2DPROJ_PLAN:
    case TT_2DPROJ_ELEV:
    case TT_2DPROJ_EXTEND:
      this->db->db2d.process_projection(prj);
      break;
    default:
      thwarning(("projection type not supported in XVI export"));
      return;
  }
  
  if (thdb.db1d.station_vec.size() == 0) {
    thwarning(("no data to export"));
    return;
  }


  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  size_t i, nst = thdb.db1d.station_vec.size(), nsh = thdb.db1d.leg_vec.size();
  thdb1ds * cs;
  thdataleg * cl;
  lxVec * stvec;
  bool * isexp;
  FILE * pltf;

  pltf = fopen(fnm,"w");
     
  if (pltf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

  isexp = new bool [nst];
  size_t nstvec;
  if (prj->type == TT_2DPROJ_EXTEND)
    nstvec = 2 * nsh;
  else
    nstvec = nst;
  stvec = new lxVec [nstvec == 0 ? 1 : nstvec];

  layoutnan(gxs, 1.0);
  layoutnan(gys, 1.0);
  layoutnan(gzs, 1.0);
  layoutnan(gox, 0.0);
  layoutnan(goy, 0.0);
  layoutnan(goz, 0.0);
  
  double xx, yx, zx, xy, yy, zy, xmin = thnan, xmax = thnan, ymin = thnan, ymax = thnan, cx, cy, sf;
  double shx, shy, gxs, gxo, gyo, gxoo, gyoo, alpha;
  long gxn, gyn;

  // scale factor: assume 100dpi
  sf = 3937.00787402 * this->layout->scale;
  gxs = this->layout->gxs;
  fprintf(pltf,"set XVIgrids {%.1f m}\n", gxs);
  gxs *= sf;
    
  switch (prj->type) {
    case TT_2DPROJ_ELEV:
    case TT_2DPROJ_EXTEND:
      if (thisnan(prj->pp1))
        alpha = 0.0;
      else
        alpha = prj->pp1;
      xx = sf * cos(alpha / 180.0 * THPI);  xy = 0.0;
      yx = sf * -sin(alpha / 180.0 * THPI);  yy = 0.0;
      zx = 0.0; zy = sf;
      gxo = sf * this->layout->gox * xx + this->layout->goy * yx;
      gyo = sf * this->layout->goz;
      break;
    default:
      xx = sf; xy = 0.0;
      yx = 0.0; yy = sf;
      zx = 0.0; zy = 0.0;
      gxo = sf * this->layout->gox;
      gyo = sf * this->layout->goy;
  }
  
  // select stations from selected surveys
  long survid = -1;
  unsigned long ff, tt;

#define check_cxy_minmax() \
  if (thisnan(xmin)) { \
    xmin = cx; \
    ymin = cy; \
    xmax = cx; \
    ymax = cy; \
  } else { \
    if (xmin > cx) xmin = cx; \
    if (ymin > cy) ymin = cy; \
    if (xmax < cx) xmax = cx; \
    if (ymax < cy) ymax = cy; \
  }

  for(i = 0; i < nst; i++) {
    cs = &(thdb.db1d.station_vec[i]);
    if (cs->survey->is_selected()) {
      if ((survid > -2) && (long(cs->survey->id) != survid)) {
        if (survid == -1)
          survid = (long) cs->survey->id;
        else
          survid = -2;
      }
      isexp[i] = true;
      cx = cs->x * xx + cs->y * yx + cs->z * zx;
      cy = cs->x * xy + cs->y * yy + cs->z * zy;
      check_cxy_minmax();
      stvec[i].x = cx;
      stvec[i].y = cy;
      stvec[i].z = 0.0;
    } else {
      isexp[i] = false;
    }
  }

  if (prj->type == TT_2DPROJ_EXTEND) {
    xmin = thnan;
    for(i = 0; i < nsh; i++) {
      cl = thdb.db1d.leg_vec[i].leg;
      ff = cl->from.id - 1;
      tt = cl->to.id - 1;

      if (isexp[ff] && isexp[tt]) {

        cs = &(thdb.db1d.station_vec[ff]);
        cx = sf * cl->fxx;
        cy = sf * cs->z;
        check_cxy_minmax();
        stvec[2*i].x = cx;
        stvec[2*i].y = cy;
        stvec[2*i].z = 0.0;

        cs = &(thdb.db1d.station_vec[tt]);
        cy = sf * cs->z;
        cx = sf * cl->txx;
        check_cxy_minmax();
        stvec[2*i+1].x = cx;
        stvec[2*i+1].y = cy;
        stvec[2*i+1].z = 0.0;
      }
    }
  }
  
  shx = (xmin + xmax) / 2.0;
  shy = (ymin + ymax) / 2.0;
  xmin -= shx; xmax -= shx; gxo -= shx;
  ymin -= shy; ymax -= shy; gyo -= shy;

  thbuffer stname;
  thsurvey * css;
  fprintf(pltf,"set XVIstations {\n");
  for(i = 0; i < nstvec; i++) {

# define export_xvi_station(i,j)  \
    if (isexp[(i)]) { \
      cs = &(thdb.db1d.station_vec[(i)]); \
      stname = cs->name; \
      css = cs->survey; \
      if (survid == -2) { \
        while ((css != NULL) && (css->id != thdb.fsurveyptr->id) && (css->is_selected())) { \
          if (css->id == cs->survey->id) \
            stname += "@"; \
          else \
            stname += "."; \
          stname += css->name; \
          css = css->fsptr; \
        } \
      } \
      stvec[(j)].x -= shx; \
      stvec[(j)].y -= shy; \
      fprintf(pltf,"  {%12.2f %12.2f %s}\n", stvec[(j)].x, stvec[(j)].y, stname.get_buffer()); \
    }
    if (prj->type == TT_2DPROJ_EXTEND) {
      if (i % 2 == 0) {
        cl = thdb.db1d.leg_vec[i / 2].leg;
        ff = cl->from.id - 1;
        tt = cl->to.id - 1;
        export_xvi_station(ff,i);
        export_xvi_station(tt,i+1);
      }
    } else
      export_xvi_station(i,i);
  }
  fprintf(pltf,"}\n");

  // export sketches
  if (this->layout->sketches) {
    fprintf(pltf,"set XVIimages {\n");
    thdb_object_list_type::iterator obi;
    thsketch_list::iterator skit;
    thpic * skpic;
    thscrap * scrap;
    for (obi = thdb.object_list.begin(); obi != thdb.object_list.end(); obi++) {
      if (((*obi)->get_class_id() == TT_SCRAP_CMD) && (*obi)->fsptr->is_selected() && (((thscrap *)(*obi))->proj->id == prj->id)) {
        scrap = (thscrap *)(*obi);
        skit = scrap->sketch_list.begin();
        while (skit != scrap->sketch_list.end()) {
          skpic = skit->morph(sf);
          if (skpic != NULL) {
            double nx, ny, ns;
            char * srcgif;
            nx = sf * (skpic->x) - shx; 
            ny = sf * (skpic->y + skpic->scale * double(skpic->height)) - shy;
            ns = skpic->scale * sf;

            char * fn;
            size_t fnx, fnl;
            fn = skit->m_pic.fname;
            fnl = strlen(skit->m_pic.fname);
            for(fnx = 0; fnx < fnl; fnx++) {
              if (((skit->m_pic.fname[fnx] == '/') || (skit->m_pic.fname[fnx] == '\\')) && (fnx < fnl - 1)) {
                fn = &(skit->m_pic.fname[fnx + 1]);
              }
            }
            if (thtext_inline) thprintf("\n");
            thprintf("converting %s ", fn);
            thprintf("(%.1f Mpix) ...", double(ns * ns * skpic->width * skpic->height) / 1000000.0);
            thtext_inline = true;

            if (fabs(ns - 1.0) < 1e-8) {
              srcgif = skpic->convert("GIF", "gif", "");            
            } else {
              srcgif = skpic->convert("GIF", "gif", "-resize %d", 
                long(ns * double(skpic->width) + 0.5));            
            }
            
            thprintf(" done\n");
            thtext_inline = false;

            if (srcgif != NULL) {
              fprintf(pltf,"{%.2f %.2f\n{", nx, ny);
              thbase64_encode(srcgif, pltf);
              fprintf(pltf,"}\n}\n");
            }
          }
          skit++;
        }
      }
    }
    fprintf(pltf,"}\n");
  }


  // export shots
  fprintf(pltf,"set XVIshots {\n");
  lxVec vff, vf1, vf2, vtt, vt1, vt2, rvec;
  double fl, fr, tl, tr;
  bool vertical;
  lxVecLimits vlim;
  for(i = 0; i < nsh; i++) {
    cl = thdb.db1d.leg_vec[i].leg;
    ff = cl->from.id - 1;
    tt = cl->to.id - 1;
    if (prj->type == TT_2DPROJ_EXTEND) {
      vff = stvec[2*i];
      vtt = stvec[2*i+1];
    } else {
      vff = stvec[ff];
      vtt = stvec[tt];
    }
    if (isexp[ff] && isexp[tt]) {
      fprintf(pltf,"  {%12.2f %12.2f %12.2f %12.2f", vff.x, vff.y, vtt.x, vtt.y);
      // calculate and export LRUD if needed
      if (cl->walls != TT_FALSE) {
        if ((vff.x == vtt.x) && (vff.y == vtt.y)) {
          vlim.valid = false;
          vlim.Add(-sf * cl->from_left,-sf * cl->from_down,0.0);
          vlim.Add(sf * cl->from_right, sf * cl->from_up  ,0.0);
          vlim.Add(-sf * cl->to_left,-sf * cl->to_down,0.0);
          vlim.Add(sf * cl->to_right, sf * cl->to_up  ,0.0);
          vf1 = vff + vlim.min;
          vf2 = vff + lxVec(vlim.min.x, vlim.max.y, 0.0);
          vt2 = vff + vlim.max;
          vt1 = vff + lxVec(vlim.max.x, vlim.min.y, 0.0);
        } else {
          vertical = false;
          switch (prj->type) {
            case TT_2DPROJ_ELEV:
            case TT_2DPROJ_EXTEND:
              if (fabs(cl->total_gradient) < cl->vtresh)
                vertical = true;
              if ((vertical) || (vff.x < vtt.x) || ((vff.x == vtt.x) && (vff.y < vtt.y))) {
                fl = sf * cl->from_up; 
                fr = sf * cl->from_down; 
                tl = sf * cl->to_up;
                tr = sf * cl->to_down;
              } else {
                fl = sf * cl->from_down; 
                fr = sf * cl->from_up; 
                tl = sf * cl->to_down;
                tr = sf * cl->to_up;
              }
              break;
            default:
              fl = sf * cl->from_left; 
              fr = sf * cl->from_right; 
              tl = sf * cl->to_left;
              tr = sf * cl->to_right;
          }
          if (vertical) {
            rvec = lxVec(0.0, 1.0, 0.0);
          } else {
            rvec = vtt - vff;
            rvec.Normalize();
            rvec = rvec.Rotated(90.0,0.0);
          }
          vf1 = vff + fl * rvec;
          vf2 = vff - fr * rvec;
          vt1 = vtt + tl * rvec;
          vt2 = vtt - tr * rvec;
        }
        cx = vf1.x; cy = vf1.y; check_cxy_minmax();
        cx = vt1.x; cy = vt1.y; check_cxy_minmax();
        cx = vf2.x; cy = vf2.y; check_cxy_minmax();
        cx = vt2.x; cy = vt2.y; check_cxy_minmax();
        fprintf(pltf," %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f %12.2f", 
          vf1.x, vf1.y, vt1.x, vt1.y, vt2.x, vt2.y, vf2.x, vf2.y);
      }
      fprintf(pltf,"}\n");
    }
  }
  fprintf(pltf,"}\n");
 
  // calculate grid
  double goverlap = 0.1 * hypot(xmax - xmin, ymax - ymin);
  thset_grid(gxo, gxs, xmin - goverlap, xmax + goverlap, gxoo, gxn);
  thset_grid(gyo, gxs, ymin - goverlap, ymax + goverlap, gyoo, gyn);
  fprintf(pltf,"set XVIgrid  {%g %g %g 0.0 0.0 %g %ld %ld}\n", gxoo, gyoo, gxs, gxs, gxn+1, gyn+1);
  
  delete [] isexp;
  delete [] stvec;
  fclose(pltf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}





void thexpmap::export_pdf(thdb2dxm * maps, thdb2dprj * prj) {

  // poojde kapitolu za kapitolou a exportuje scrapy a 
  // vytvori hashe -> layer a scrap
  // spusti mp - thpdf - pdftex

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }
    

  double meridian_conv = thcfg.get_outcs_convergence();
  double rotate_plus = 0.0;
  FILE * mpf, * plf, *tf;
  unsigned sfig = 1;
  unsigned sscrap = 0;
  thexpmap_xmps exps;
  char * chtitle;
  thbuffer tit;
  bool quick_map_exp = false;
  double origin_shx, origin_shy, new_shx, new_shy, srot = 0.0, crot = 1.0, rrot = 0.0;
  thexpmapmpxs out;
  th2ddataobject * op2;
  bool export_sections, export_outlines_only;
  double shx, shy;
  unsigned long sclevel, bmlevel;
  legenddata ldata;

  bool anyprev, anyprevabove = false, anyprevbelow = false;
  char * prevbf;
  prevbf = new char [128];
  prevbf[127] = 0;
  
  thbuffer aboveprev, belowprev;

  list<scraprecord>::iterator SCRAPITEM;
  scraprecord dummsr;
  
  char * fnm = this->get_output("cave.pdf");  

  layerrecord L;
  map<int, layerrecord>::iterator LAYER_ITER;

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;
  thbuffer encb, texb;
  texb.guarantee(128);
  thscrap * cs;

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ...\n", fnm);
//  thtext_inline = true;
#endif 

  // scalebar length
  double sblen;
  int sbtest;

	this->layout->units.lang = this->layout->lang;

  if (this->layout->scale_bar <= 0.0) {
    sbtest = int(log(this->layout->units.convert_length(0.10 / this->layout->scale)) / log(pow(10.0,1.0/3.0)));
    sblen = pow(10.0,double(sbtest/3));
    switch (sbtest % 3) {
      case 0: sblen *= 1.0; break;
      case 1: sblen *= 2.5; break;
      default: sblen *= 5.0; break;
    }
  } else 
    sblen = this->layout->units.convert_length(this->layout->scale_bar);

  layoutnan(gxs, sblen / 5.0);
  layoutnan(gys, sblen / 5.0);
  layoutnan(gzs, sblen / 5.0);
  layoutnan(gox, 0.0);
  layoutnan(goy, 0.0);
  layoutnan(goz, 0.0);

  out.symset = &(this->symset);
  out.layout = this->layout;
  out.ms = this->layout->scale * 2834.64566929;
  // korekcia shiftu na layout origin
  if (!thisnan(this->layout->ox)) {
    switch (prj->type) {
      case TT_2DPROJ_EXTEND:
      case TT_2DPROJ_ELEV:
        origin_shx = (prj->shift_x - this->layout->ox) * out.ms;
        origin_shy = (prj->shift_z - this->layout->oz) * out.ms;
        break;
      default:
        origin_shx = (prj->shift_x - this->layout->ox) * out.ms;
        origin_shy = (prj->shift_y - this->layout->oy) * out.ms;
      break;
    }
  } else {
    origin_shx = 0.0;
    origin_shy = 0.0;
  }
  // koniec korekcie shiftu na layout origin

	if ((this->layout->def_overlap > 0) && (this->layout->def_size == 0)) {
		double nhs, nvs;
		nhs = this->layout->hsize + 0.02 - 2.0 * this->layout->overlap;
		nvs = this->layout->vsize + 0.02 - 2.0 * this->layout->overlap;
		if (nhs > 0.0)
			this->layout->hsize = nhs;
		if (nvs > 0.0)
			this->layout->vsize = nvs;
	} else if ((this->layout->def_size > 0) && (this->layout->def_page_setup == 0)) {
	  this->layout->paghs = this->layout->hsize + 2.0 * this->layout->overlap;
	  this->layout->pagvs = this->layout->vsize + 2.0 * this->layout->overlap + 0.065;
	  this->layout->paphs = this->layout->paghs + 2.0 * this->layout->marls;
	  this->layout->papvs = this->layout->pagvs + 2.0 * this->layout->marts;
	}
  
  switch (this->layout->north) {
    case TT_LAYOUT_NORTH_GRID:
      rotate_plus = meridian_conv;
      break;
  }
      
//  tf = fopen(thtmp.get_file_name("Config"),"w");
//  this->layout->export_config(tf,prj,out.ms,origin_shx,origin_shy);
  this->layout->set_thpdf_layout(prj,out.ms,origin_shx,origin_shy);
//  fclose(tf);

  tf = fopen(thtmp.get_file_name("data.tex"),"w");
  if (thcmdln.extern_libs)
    fprintf(tf,"\\input therion.tex\n");
  else
    fprintf(tf,"%s\n",thtex_library);
  this->layout->export_pdftex(tf,prj,
    (this->export_mode == TT_EXP_MAP ? TT_LAYOUT_CODE_TEX_MAP : TT_LAYOUT_CODE_TEX_ATLAS)
  );     
  fprintf(tf,"\\end\n");
  fclose(tf);
  

  // mooze byt zmena v definovanych symboloch, natoceni atd... :(((
  //if (thexporter_quick_map_export && 
  //    (thexpmap_quick_map_export_scale == this->layout->scale)) {
  //  quick_map_exp = true;
  //  goto QUICK_MAP_EXPORT;
  //}

  plf = fopen(thtmp.get_file_name("data.pl"),"w");

  SCRAPLIST.clear();     
  
  mpf = fopen(thtmp.get_file_name("data.mp"),"w");     

  out.file = mpf;
  out.proj = prj;
  out.attr_last_survey = "";
  out.attr_last_id = "";
  out.attr_last_scrap = "";
  out.attr_last_scrap_centerline = false;
  
  thexpmap_quick_map_export_scale = this->layout->scale;
  fprintf(mpf,"Scale:=%.2f;\n",0.01 / this->layout->scale);
  if (this->layout->def_base_scale > 0)
    fprintf(mpf,"BaseScale:=%.2f;\n",0.01 / this->layout->base_scale);
  fprintf(mpf,"color HelpSymbolColor;\nHelpSymbolColor := (0.8, 0.8, 0.8);\n");
  fprintf(mpf,"background:=(%.5f,%.5f,%.5f);\n",
    this->layout->color_map_fg.R,
    this->layout->color_map_fg.G,
    this->layout->color_map_fg.B);
  fprintf(mpf,"verbatimtex \\input th_enc.tex etex;\n");
//  fprintf(mpf,"def user_initialize = enddef;\n");
//this->layout->export_mpost(mpf);
  if (thcmdln.extern_libs)
    fprintf(mpf,"input therion;\n");
  else
    fprintf(mpf,"%s\n",thmpost_library);
  fprintf(mpf,"lang:=\"%s\";\n",thlang_getid(thlang_getlang(this->layout->lang)));

  // System attributes definition.
  fprintf(mpf,"string ATTR__id, ATTR__survey, ATTR__scrap;\n");
  fprintf(mpf,"boolean ATTR__scrap_centerline;\nATTR__scrap_centerline = false;\n");
  
  this->db->attr.export_mp_header(out.file);

//  fprintf(mpf,"verbatimtex \\def\\updown#1#2{\\vbox{%%\n");
//  fprintf(mpf,"    \\offinterlineskip\n");
//  fprintf(mpf,"    \\setbox100=\\hbox{#1}\n");
//  fprintf(mpf,"    \\setbox101=\\hbox{#2}\n");
//  fprintf(mpf,"    \\ifnum\\wd100>\\wd101\\hsize=\\wd100\\else\\hsize=\\wd101\\fi\n");
//  fprintf(mpf,"    \\centerline{\\box100}\\vskip4pt\n");
//  fprintf(mpf,"    \\centerline{\\box101}}}\n");
//  fprintf(mpf,"  \\def\\thnormalsize{\\size[10]}\n");
//  fprintf(mpf,"  \\def\\thsmallsize{\\size[8]}\n");
//  fprintf(mpf,"  \\def\\thlabel{\\thnormalsize}\n");
//  fprintf(mpf,"  \\def\\thremark{\\thsmallsize\\si}\n");
//  fprintf(mpf,"  \\def\\thaltitude{\\thsmallsize}\n");
//  fprintf(mpf,"  \\def\\thstationname{\\thsmallsize}\n");
//  fprintf(mpf,"  \\def\\thdate{\\thsmallsize}\n");
//  fprintf(mpf,"  \\def\\thheight{\\thsmallsize}\n");
//  fprintf(mpf,"  \\def\\thheightpos{\\thsmallsize+\\ignorespaces}\n");
//  fprintf(mpf,"  \\def\\thheightneg{\\thsmallsize-\\ignorespaces}\n");
//  fprintf(mpf,"  \\def\\thframed{\\thsmallsize}\n");
//  fprintf(mpf,"  \\def\\thwallaltitude{\\thsmallsize}\n");
//  fprintf(mpf,"etex;\n");

  fprintf(mpf,"defaultfont:=\"%s\";\n",FONTS.begin()->ss.c_str());
  
//fprintf(mpf,"defaultscale:=0.8;\n\n");

  this->layout->export_mpost(mpf);
  
  this->layout->export_mpost_symbols(mpf, &(this->symset));
  
  fprintf(mpf,"transparency:=%s;\n", (this->layout->transparency ? "true" : "false"));
  
//fprintf(mpf,"write EOF to \"missed.dat\";\n\n");

  // prida nultu figure
  // fprintf(mpf,"beginfig(0);\nendfig;\n");
  
  this->export_pdf_set_colors(maps, prj);
  double ascR = 1.0, ascG = 1.0, ascB = 1.0,
    pscR = -1.0, pscG = -1.0, pscB = -1.0;

  SURFPICTLIST.clear();
  surfpictrecord srfpr;
  thsurface * surf;
  double surfscl = this->layout->scale * 11811.023622472446117783;
    
  if (COLORLEGENDLIST.size() > 0) {
    fprintf(plf,"# COLOR LEGEND\n");
    for (list<colorlegendrecord>::iterator cli = COLORLEGENDLIST.begin();
      cli != COLORLEGENDLIST.end(); cli++) {
      fprintf(plf,"# %4.0f %4.0f %4.0f %s\n", 100.0 * cli->R, 100.0 * cli->G, 100.0 * cli->B, cli->texname.c_str());
    }
    fprintf(plf,"\n\n\n");
  }
   
  fprintf(plf,"%%SCRAP = (\n");
  while (cmap != NULL) {
    cbm = cmap->first_bm;
    bmlevel = 0;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      sclevel = 0;
      // !!! Tu pridat aj ine druhy - teda ABOVE a BELOW
      export_outlines_only = ((cbm->mode == TT_MAPITEM_ABOVE) ||
        (cbm->mode == TT_MAPITEM_BELOW)) && (!cbm->m_target->previewed)
        && (cbm->m_target->fmap->output_number != cbm->m_target->preview_output_number);
      if ((cbm->mode == TT_MAPITEM_NORMAL) || export_outlines_only) {
        if (export_outlines_only) {
          cbm->m_target->previewed = true;
        }
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            cs = (thscrap *) cmi->object;
            export_sections = false;
            op2 = NULL;
            while ((!export_sections) || (op2 != NULL)) {
              if (export_sections) {
                if ((op2->get_class_id() == TT_POINT_CMD) &&
                  (((thpoint *)op2)->type == TT_POINT_TYPE_SECTION) &&
                  (((((thpoint *)op2)->context < 0) && this->symset.assigned[SYMP_SECTION]) || ((((thpoint *)op2)->context > -1) && this->symset.assigned[((thpoint *)op2)->context])) &&
                  (((thpoint *)op2)->text != NULL)) {
                    cs = (thscrap *)((thpoint *)op2)->text;
                    thdb.db2d.process_projection(cs->proj);
                }
                else
                  cs = NULL;
              }
              if (cs != NULL) {
                cs->get_polygon();
                if (export_sections) {
                  if (thisnan(cs->lxmin)) {
                    out.mx = 0.0;
                    out.my = 0.0;
                  } else {
                    out.mx = (cs->lxmax + cs->lxmin) / 2.0;
                    out.my = (cs->lymax + cs->lymin) / 2.0;
                  }
                  out.sr = 0.0;
                  out.cr = 1.0;
                  out.rr = 0.0;
                  shx = ((thpoint *)op2)->point->xt;
                  shy = ((thpoint *)op2)->point->yt;
                  shx *= out.ms;
                  shy *= out.ms;

                } else {
  
                  ascR = cs->R;
                  ascG = cs->G;
                  ascB = cs->B;
                  if ((ascR != pscR) || (ascG != pscG) || (ascB != pscB)) {
                    fprintf(mpf,"background:=(%.5f,%.5f,%.5f);\n", ascR, ascG, ascB);
                    fprintf(mpf,"def_transparent_rgb(tr_bg, %.5f, %.5f, %.5f);\n", ascR, ascG, ascB);
                    pscR = ascR;
                    pscG = ascG;
                    pscB = ascB;
                  }
                  
                  if (thisnan(cs->lxmin)) {
                    out.mx = 0.0;
                    out.my = 0.0;
                  } else {
                    out.mx = (cs->lxmax + cs->lxmin) / 2.0;
                    out.my = (cs->lymax + cs->lymin) / 2.0;
                  }
                  switch (prj->type) {
                    case TT_2DPROJ_ELEV:
                    case TT_2DPROJ_EXTEND:
                      break;
                    default:
                      out.rr = out.layout->rotate - meridian_conv + rotate_plus;
                      out.sr = sin(out.rr / 180.0 * THPI);
                      out.cr = cos(out.rr / 180.0 * THPI);
                      break;
                  }
                  shx = out.mx * out.ms;
                  shy = out.my * out.ms;
                  if (!export_outlines_only) {
                    cs->exported = true;
                  }
                } 

                shx += cbm->m_shift.m_x * out.ms;
                shy += cbm->m_shift.m_y * out.ms;
                out.m_shift_x = cbm->m_shift.m_x - cbm->m_shift.m_prev_x;
                out.m_shift_y = cbm->m_shift.m_y - cbm->m_shift.m_prev_y; 
                
                switch (prj->type) {
                  case TT_2DPROJ_ELEV:
                  case TT_2DPROJ_EXTEND:
                    rrot = 0.0;
                    srot = 0.0;
                    crot = 1.0;
                    break;
                  default:
                    rrot = this->layout->rotate - meridian_conv + rotate_plus;
                    srot = sin(rrot / 180.0 * THPI);
                    crot = cos(rrot / 180.0 * THPI);
                    new_shx = shx * crot + shy * srot;
                    new_shy = shy * crot - shx * srot;
                    shx = new_shx;
                    shy = new_shy;
                }
                
                shx += origin_shx;
                shy += origin_shy;
      
                if (export_sections) {
                  switch (thdb2d_rotate_align(((thpoint *)op2)->align, rrot)) {
                    case TT_POINT_ALIGN_B:
                      shy -= (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_T:
                      shy += (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_R:
                      shx += (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_L:
                      shx -= (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_BR:
                      shy -= (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      shx += (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_BL:
                      shy -= (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      shx -= (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_TR:
                      shy += (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      shx += (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                    case TT_POINT_ALIGN_TL:
                      shy += (cs->lymax - cs->lymin) / 2.0 * out.ms;
                      shx -= (cs->lxmax - cs->lxmin) / 2.0 * out.ms;
                      break;
                  }
                }
                
                exps = this->export_mp(& out, cs, sfig, export_outlines_only);
                // naozaj ho exportujeme
                if (exps.flags != TT_XMPS_NONE) {
                  fprintf(plf,"\t# scrap: %s\n",cs->name);
                  fprintf(plf,"\t%s => {\n",thexpmap_u2string(sscrap));

                  if (this->layout->sketches) {
                    thsketch_list::iterator skit;
                    thpic * skpic;
                    skit = cs->sketch_list.begin();
                    while (skit != cs->sketch_list.end()) {
                      skpic = skit->morph(out.ms);
                      if (skpic != NULL) { 
                        srfpr.filename = skpic->texfname;
                        srfpr.width  = skpic->width / 300.0 * 72.0;
                        srfpr.height = skpic->height / 300.0 * 72.0;
                        srfpr.dx = ( - prj->shift_x + skpic->x ) * out.ms;
                        srfpr.dy = ( - prj->shift_y + skpic->y ) * out.ms;
                        srfpr.xx = skpic->scale * surfscl;
                        srfpr.xy = 0.0;
                        srfpr.yx = 0.0;
                        srfpr.yy = skpic->scale * surfscl;
                        srfpr.type = "png";

                        // otocenie o rotaciu v layoute
                        double tdx = srfpr.dx, tdy = srfpr.dy,
                          txx = srfpr.xx, txy = srfpr.xy, tyx = srfpr.yx, tyy = srfpr.yy;
                        srfpr.dx =   tdx * crot + tdy * srot + origin_shx;
                        srfpr.dy = - tdx * srot + tdy * crot + origin_shy;
                        srfpr.xx =  crot * txx + srot * tyx;
                        srfpr.xy =  crot * txy + srot * tyy;
                        srfpr.yx = -srot * txx + crot * tyx;
                        srfpr.yy = -srot * txy + crot * tyy;
                        SURFPICTLIST.insert(SURFPICTLIST.end(), srfpr);
                        fprintf(plf,"\n\n# PICTURE: %s\n", srfpr.filename);
                        fprintf(plf,    "#  origin: %g %g\n", srfpr.dx, srfpr.dy);
                        fprintf(plf,    "#  matrix: %g %g %g %g\n\n", srfpr.xx, srfpr.xy, srfpr.yx, srfpr.yy);
                      }
                      skit++;
                    }
                  }


                  SCRAPITEM = SCRAPLIST.insert(SCRAPLIST.end(),dummsr);
                  SCRAPITEM->sect = 0;
                  SCRAPITEM->name = thexpmap_u2string(sscrap);

                  SCRAPITEM->r = ascR; //this->layout->color_map_fg.R;
                  SCRAPITEM->g = ascG; //this->layout->color_map_fg.G;
                  SCRAPITEM->b = ascB; //this->layout->color_map_fg.B;
                  
                  if (export_sections) {
                    fprintf(plf,"\t\t Z => 1,\n");    
                    SCRAPITEM->sect = 1;
                  }
                  // pred orezanim
                  if (exps.F > 0) {
                    fprintf(plf,"\t\t F => \"data.%ld\",\n",exps.F);
                    sprintf(texb.get_buffer(),"data.%ld",exps.F);
                    SCRAPITEM->F = texb.get_buffer();
                  }
//                  else
//                    fprintf(plf,"\t\t F => \"data.0\",\n");
    
                  // orezavacia cesta a outlines
                  if (exps.B > 0) {
                    fprintf(plf,"\t\t B => \"data.%ld\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ld",exps.B);
                    SCRAPITEM->B = texb.get_buffer();
                    fprintf(plf,"\t\t I => \"data.%ldbg\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ldbg",exps.B);
                    SCRAPITEM->I = texb.get_buffer();
                    fprintf(plf,"\t\t C => \"data.%ldclip\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ldclip",exps.B);
                    SCRAPITEM->C = texb.get_buffer();
                  }
//                  else {
//                    fprintf(plf,"\t\t B => \"data.0\",\n");
//                    fprintf(plf,"\t\t I => \"data.0\",\n");
//                    fprintf(plf,"\t\t C => \"data.0\",\n");
//                  }
    
                  // po orezani
                  if (exps.E > 0) {
                    fprintf(plf,"\t\t E => \"data.%ld\",\n",exps.E);
                    sprintf(texb.get_buffer(),"data.%ld",exps.E);
                    SCRAPITEM->E = texb.get_buffer();
                  }
//                  else
//                    fprintf(plf,"\t\t E => \"data.0\",\n");
    
                  if (exps.X > 0) {
                    fprintf(plf,"\t\t X => \"data.%ld\",\n",exps.X);
                    sprintf(texb.get_buffer(),"data.%ld",exps.X);
                    SCRAPITEM->X = texb.get_buffer();
                    fprintf(plf,"\t\t P => \"data.%ldbbox\",\n",exps.X);
                    sprintf(texb.get_buffer(),"data.%ldbbox",exps.X);
                    SCRAPITEM->P = texb.get_buffer();
                  }

                  if (export_outlines_only) {
                    fprintf(plf,"\t\t Y => %ld,\n",cbm->m_target->preview_output_number);
                    SCRAPITEM->layer = (int) cbm->m_target->preview_output_number;
                    fprintf(plf,"\t\t V => -1,\n");
                    SCRAPITEM->level = -1;
                  } else {
                    fprintf(plf,"\t\t Y => %ld,\n",cmap->output_number);
                    SCRAPITEM->layer = (int) cmap->output_number;
                    fprintf(plf,"\t\t V => -%ld,\n",cmi->itm_level + bmlevel);
                    SCRAPITEM->level = - ((int) (cmi->itm_level + bmlevel));
                  }
                  
                  fprintf(plf,"\t\t S => \"%.2f %.2f\",\n\t},\n",shx,shy);
                  SCRAPITEM->S1 = shx;
                  SCRAPITEM->S2 = shy;
                  
                  if ((!export_outlines_only) && (!export_sections) &&
                      (cbm->m_target->fmap->output_number != cbm->m_target->preview_output_number)
      	              && (!cbm->m_target->previewed)		      
                      && (exps.B > 0)) {
            		    fprintf(plf,"\t# scrap: %s\n",cs->name);
                    fprintf(plf,"\t%s => {\n",thexpmap_u2string(sscrap + 1));
                    SCRAPITEM = SCRAPLIST.insert(SCRAPLIST.end(),dummsr);
                    SCRAPITEM->sect = 0;
                    SCRAPITEM->name = thexpmap_u2string(sscrap + 1);
      
                    SCRAPITEM->r = ascR; //this->layout->color_map_fg.R;
                    SCRAPITEM->g = ascG; //this->layout->color_map_fg.G;
                    SCRAPITEM->b = ascB; //this->layout->color_map_fg.B;
      
                    fprintf(plf,"\t\t B => \"data.%ld\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ld",exps.B);
                    SCRAPITEM->B = texb.get_buffer();
                    fprintf(plf,"\t\t I => \"data.%ldbg\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ldbg",exps.B);
                    SCRAPITEM->I = texb.get_buffer();
                    fprintf(plf,"\t\t C => \"data.%ldclip\",\n",exps.B);
                    sprintf(texb.get_buffer(),"data.%ldclip",exps.B);
                    SCRAPITEM->C = texb.get_buffer();
                    //fprintf(plf,"\t\t B => \"data.%ld\",\n",exps.B);
                    //fprintf(plf,"\t\t I => \"data.%ldbg\",\n",exps.B);
                    //fprintf(plf,"\t\t C => \"data.%ldclip\",\n",exps.B);
                    fprintf(plf,"\t\t Y => %ld,\n",cbm->m_target->preview_output_number);
                    SCRAPITEM->layer = (int) cbm->m_target->preview_output_number;
                    fprintf(plf,"\t\t V => -1,\n");
                    SCRAPITEM->level = -1;
                    fprintf(plf,"\t\t S => \"%.2f %.2f\",\n\t},\n",shx,shy);
                    SCRAPITEM->S1 = shx;
                    SCRAPITEM->S2 = shy;
                    sscrap++;
                  }
                  sscrap++;
                }
              } // if cs != NULL
              if (!export_sections) {
                export_sections = true;
//                if ((export_outlines_only) || (!this->symset.assigned[SYMP_SECTION]))
                if (export_outlines_only)
                  // rezy sa neexportuju
                  op2 = NULL;
                else
                  op2 = cs->ls2doptr;
              } else {
                op2 = op2->pscrapoptr;
              }
            } // koniec exportovania priecnych rezov 
          }
          cmi = cmi->prev_item;  
        }
        bmlevel += cbm->bm->last_level;
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  sprintf(texb.get_buffer(),"data.%d",sfig);
  LAYOUT.northarrow = texb.get_buffer();
  fprintf(mpf,"beginfig(%d);\ns_northarrow(%g);\nendfig;\n",sfig++,this->layout->rotate + rotate_plus);

  sprintf(texb.get_buffer(),"data.%d",sfig);
  LAYOUT.scalebar = texb.get_buffer();
  //snprintf(prevbf,127,"%g",sblen);
  fprintf(mpf,"beginfig(%d);\ns_scalebar(%g, %g, \"%s\");\nendfig;\n",
    sfig++, sblen, 1.0 / this->layout->units.convert_length(1.0), utf2tex(this->layout->units.format_i18n_length_units()));

  // sem pride zapisanie legendy do MP suboru
  if (this->layout->def_base_scale > 0)
    fprintf(mpf,"Scale:=%.2f;\ninitialize(Scale);\n",0.01 / this->layout->base_scale);
  fprintf(mpf,"background:=(%.5f,%.5f,%.5f);\n",
    this->layout->color_map_fg.R,
    this->layout->color_map_fg.G,
    this->layout->color_map_fg.B);
  //fprintf(mpf,"background:=white;\n");
  fprintf(mpf,"transparency:=false;\n");
  
  LEGENDLIST.clear();
  if ((this->layout->legend != TT_LAYOUT_LEGEND_OFF) && 
      ((this->export_mode == TT_EXP_ATLAS) || (this->layout->map_header != TT_LAYOUT_MAP_HEADER_OFF))) {
    this->symset.export_pdf(this->layout,mpf,sfig);
  }
  
  
  // export all surface pictures

  switch (prj->type) {
    case TT_2DPROJ_ELEV:
    case TT_2DPROJ_EXTEND:
      rrot = 0.0;
      srot = 0.0;
      crot = 1.0;
      break;
    default:
      rrot = this->layout->rotate - meridian_conv + rotate_plus;
      srot = sin(rrot / 180.0 * THPI);
      crot = cos(rrot / 180.0 * THPI);
  }

  thdb_object_list_type::iterator obi;
  if ((prj->type == TT_2DPROJ_PLAN) && (this->layout->surface != TT_LAYOUT_SURFACE_OFF)) {
    // prejde vsetky objekty a exportuje vsetku surveye
    obi = thdb.object_list.begin();
    while (obi != thdb.object_list.end()) {
      if ((*obi)->get_class_id() == TT_SURFACE_CMD) {
        surf = (thsurface *)(*obi);
        if (surf->pict_name != NULL) {
          surf->calibrate();
          srfpr.filename = surf->pict_name;
          srfpr.dx = (surf->calib_x - prj->shift_x) * out.ms;
          srfpr.dy = (surf->calib_y - prj->shift_y) * out.ms;
          srfpr.xx = surf->calib_xx * surfscl * surf->pict_dpi / 300.0;
          srfpr.xy = surf->calib_xy * surfscl * surf->pict_dpi / 300.0;
          srfpr.yx = surf->calib_yx * surfscl * surf->pict_dpi / 300.0;
          srfpr.yy = surf->calib_yy * surfscl * surf->pict_dpi / 300.0;
          srfpr.width = surf->pict_width / surf->pict_dpi * 72.0;
          srfpr.height = surf->pict_height / surf->pict_dpi * 72.0;
          srfpr.type = "unknown";
          switch (surf->pict_type) {
            case TT_IMG_TYPE_JPEG:
              srfpr.type = "jpeg";
              break;
            case TT_IMG_TYPE_PNG:
              srfpr.type = "png";
              break;
          }
					
	// otocenie o rotaciu v layoute
	double tdx = srfpr.dx, tdy = srfpr.dy,
	txx = srfpr.xx, txy = srfpr.xy, tyx = srfpr.yx, tyy = srfpr.yy;
	srfpr.dx =   tdx * crot + tdy * srot + origin_shx;
	srfpr.dy = - tdx * srot + tdy * crot + origin_shy;
	srfpr.xx =  crot * txx + srot * tyx;
	srfpr.xy =  crot * txy + srot * tyy;
	srfpr.yx = -srot * txx + crot * tyx;
	srfpr.yy = -srot * txy + crot * tyy;
					
          SURFPICTLIST.insert(SURFPICTLIST.end(), srfpr);
          fprintf(plf,"\n\n# PICTURE: %s\n", srfpr.filename);
          fprintf(plf,    "#  origin: %g %g\n", srfpr.dx, srfpr.dy);
          //fprintf(plf,    "#   scale: %g\n", srfpr.ss);
          //fprintf(plf,    "#  rotate: %g\n", srfpr.rr);
          fprintf(plf,    "#  matrix: %g %g %g %g\n\n", srfpr.xx, srfpr.xy, srfpr.yx, srfpr.yy);
        }
      }
      obi++;
    }
  }

  // nakoniec grid
  double ghs, gvs, gox, goy;
  char * grid_macro = "s_hgrid";
  switch (prj->type) {
  case TT_2DPROJ_ELEV:
  case TT_2DPROJ_EXTEND:
    ghs = this->layout->gxs * out.ms;
    gvs = this->layout->gzs * out.ms;
    gox = 0.0;
    goy = (this->layout->goz - prj->shift_z) * out.ms;
    LAYOUT.gridrot = 0.0;
    LAYOUT.proj = 1;
    grid_macro = "s_vgrid";
    break;
  default:
    ghs = this->layout->gxs * out.ms;
    gvs = this->layout->gys * out.ms;
    gox = (this->layout->gox - prj->shift_x) * out.ms;
    goy = (this->layout->goy - prj->shift_y) * out.ms;
    LAYOUT.gridrot = rrot;
    LAYOUT.proj = 0;
    break;
  }
  
  LAYOUT.hgridsize = ghs;
  LAYOUT.vgridsize = gvs;
  LAYOUT.hgridorigin = gox * crot + goy * srot + origin_shx;
	LAYOUT.vgridorigin = - gox * srot + goy * crot + origin_shy;
  
  switch (this->layout->grid) {
    case TT_LAYOUT_GRID_TOP:
      LAYOUT.grid = 2;
      break;
    case TT_LAYOUT_GRID_BOTTOM:
      LAYOUT.grid = 1;
      break;
    default:
      LAYOUT.grid = 0;
      break;
  }
  
  if (this->layout->grid != TT_LAYOUT_GRID_OFF) {

#define expgridscrap(varname,Xpos,Ypos) \
    sprintf(texb.get_buffer(),"data.%d",sfig); \
    LAYOUT.varname = texb.get_buffer(); \
    fprintf(mpf,"beginfig(%d);\n%s(%d, %d, %.5f, %.5f);\nendfig;\n", \
    sfig++, grid_macro, Xpos, Ypos, ghs, gvs);
    
    expgridscrap(gridAA, -1, -1);
    expgridscrap(gridAB,  0, -1);
    expgridscrap(gridAC,  1, -1);
    expgridscrap(gridBA, -1,  0);
    expgridscrap(gridBB,  0,  0);
    expgridscrap(gridBC,  1,  0);
    expgridscrap(gridCA, -1,  1);
    expgridscrap(gridCB,  0,  1);
    expgridscrap(gridCC,  1,  1);
    
  }
  
  fprintf(mpf,"end;\n");
  fprintf(plf,");\n");
  fclose(mpf);

  cmap = maps;
  while (cmap != NULL) {
    if (cmap->expand) {      
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        cbm->m_target->previewed = false;
        cbm = cbm->next_item;
      }
    }
    cmap = cmap->next_item;
  }
  
  cmap = maps;  

  LAYERHASH.clear();
  MAP_PREVIEW_UP.clear();
  MAP_PREVIEW_DOWN.clear();
  
  chtitle = NULL;
  fprintf(plf,"\n\n%%LAYER = (\n");
  while (cmap != NULL) {
    if (cmap->title)
      chtitle = (strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name);
    if (cmap->expand) {      
    
      fprintf(plf,"\t# expanded map: %s\n",cmap->map->name);
      fprintf(plf,"\t%ld => {\n",cmap->output_number);
      LAYERHASH.insert(make_pair(cmap->output_number,L));
      LAYER_ITER = LAYERHASH.find(cmap->output_number);
      LAYER_ITER->second.Z = 0;
      
      anyprev = false;
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if (cbm->mode == TT_MAPITEM_ABOVE) {
          LAYER_ITER->second.U.insert(cbm->m_target->preview_output_number);
          if (!anyprev) {
            fprintf(plf,"\t\tU => \"%ld",cbm->m_target->preview_output_number);
            anyprev = true;
          } else
            fprintf(plf," %ld",cbm->m_target->preview_output_number);
        }
        cbm = cbm->next_item;
      }
      if (anyprev)
            fprintf(plf,"\",\n");      
      

      thdecode(& texb,TT_ISO8859_2,(strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name));      
      thdecode_tex(& encb, texb.get_buffer());
      fprintf(plf,"\t\tN => '%s',\n",encb.get_buffer());
      LAYER_ITER->second.N = (strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name);
      if ((chtitle != NULL) && ((cmap->next_item == NULL) || (cmap->next_item->title))) {
        thdecode(& texb,TT_ISO8859_2,chtitle);      
        thdecode_tex(& encb, texb.get_buffer());
        fprintf(plf,"\t\tT => '%s',\n",encb.get_buffer());
        LAYER_ITER->second.T = chtitle;
        chtitle = NULL;
      }
      
      
      anyprev = false;
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if (cbm->mode == TT_MAPITEM_BELOW) {
          LAYER_ITER->second.D.insert(cbm->m_target->preview_output_number);
          if (!anyprev) {
            fprintf(plf,"\t\tD => \"%ld",cbm->m_target->preview_output_number);
            anyprev = true;
          } else
            fprintf(plf," %ld",cbm->m_target->preview_output_number);
        }
        cbm = cbm->next_item;
      }
      if (anyprev)
            fprintf(plf,"\",\n");      
      
      
      fprintf(plf,"\t},\n");

      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if ((!cbm->m_target->previewed) && 
            (cbm->m_target->fmap->output_number != cbm->m_target->preview_output_number)) {
          cbm->m_target->previewed = true;
          fprintf(plf,"\t# basic map: %s\n",cbm->bm->name);
          fprintf(plf,"\t%ld => {\n",cbm->m_target->preview_output_number);
          fprintf(plf,"\t\tZ => 1,\n");
      	  fprintf(plf,"\t\tA => %ld,\n",cbm->m_target->fmap->output_number);
          fprintf(plf,"\t},\n");

          LAYERHASH.insert(make_pair(cbm->m_target->preview_output_number,L));
          LAYER_ITER = LAYERHASH.find(cbm->m_target->preview_output_number);
          LAYER_ITER->second.Z = 1;
          LAYER_ITER->second.AltJump = cbm->m_target->fmap->output_number;

          switch (cbm->m_target->mode) {
            case TT_MAPITEM_BELOW:
              MAP_PREVIEW_DOWN.insert(cbm->m_target->preview_output_number);
              if (!anyprevbelow) {
                snprintf(prevbf,127,"%ld",cbm->m_target->preview_output_number);
                belowprev += prevbf;
                anyprevbelow = true;
              } else {
                snprintf(prevbf,127," %ld",cbm->m_target->preview_output_number);
                belowprev += prevbf;
              }
              break;
            case TT_MAPITEM_ABOVE:
              MAP_PREVIEW_UP.insert(cbm->m_target->preview_output_number);
              if (!anyprevabove) {
                snprintf(prevbf,127,"%ld",cbm->m_target->preview_output_number);
                aboveprev += prevbf;
                anyprevabove = true;
              } else {
                snprintf(prevbf,127," %ld",cbm->m_target->preview_output_number);
                aboveprev += prevbf;
              }
              break;
          }
        }
        cbm = cbm->next_item;
      }
    }
    cmap = cmap->next_item; 
  }
  fprintf(plf,");\n");
  
  // map preview hash
  if (anyprevabove) {
    fprintf(plf,"\n\n$MAP_PREVIEW{U} = \"%s\";",aboveprev.get_buffer());
  }
  if (anyprevbelow) {
    fprintf(plf,"\n\n$MAP_PREVIEW{D} = \"%s\";",belowprev.get_buffer());
  }
  
  fprintf(plf,"\n\n\n$PATTERN_DEFS = \"./\";\n");
  fclose(plf);
  
  delete [] prevbf;

  //QUICK_MAP_EXPORT:

  //if (strlen(this->layout->doc_title) == 0) {
  tit.strcpy(thdb.db2d.get_projection_title(prj));
    //LAYOUT.doc_title = tit.get_buffer();
  //} else
  //  tit.strcpy(LAYOUT.doc_title.c_str());
    
  tf = fopen(thtmp.get_file_name("th_texts.tex"),"w");
  fprintf(tf,"\\legendtitle={%s}\n",utf2tex(thT("title legend",this->layout->lang)));
  ldata.legendtitle = thT("title legend",this->layout->lang);

  ldata.colorlegendtitle = "";
  if ((this->layout->color_crit != TT_LAYOUT_CCRIT_UNKNOWN) && (this->layout->color_crit != TT_LAYOUT_CCRIT_SCRAP)) {
    fprintf(tf,"\\colorlegendtitle={");
    switch (this->layout->color_crit) {
      case TT_LAYOUT_CCRIT_ALTITUDE:
        fprintf(tf,"%s", utf2tex(thT("title color-legend-altitude",this->layout->lang)));
        ldata.colorlegendtitle = thT("title color-legend-altitude",this->layout->lang);
        break;
      default: //TT_LAYOUT_CCRIT_MAP:
        fprintf(tf,"%s", utf2tex(thT("title color-legend-map",this->layout->lang)));
        ldata.colorlegendtitle = thT("title color-legend-map",this->layout->lang);
    }
    fprintf(tf,"}\n");
  }

  // ak neni atlas, tak nastavi legendcavename
  fprintf(tf,"\\cavename={%s}\n",utf2tex(tit.get_buffer()));
  ldata.cavename = tit.get_buffer();
  ldata.comment = "";

  if (strlen(this->layout->doc_comment) > 0) {
    fprintf(tf,"\\comment={%s}\n",utf2tex(this->layout->doc_comment));
    ldata.comment = this->layout->doc_comment;
  }
  
  if ((prj->type != TT_2DPROJ_PLAN) || (!this->symset.assigned[SYMS_NORTHARROW])) {
    fprintf(tf,"\\northarrowfalse\n");
    ldata.northarrow = false;
  } else {
    fprintf(tf,"\\northarrowtrue\n");
    ldata.northarrow = true;
  }
  if (!this->symset.assigned[SYMS_SCALEBAR]) {
    fprintf(tf,"\\scalebarfalse\n");
    ldata.scalebar = false;
  } else {
    fprintf(tf,"\\scalebartrue\n");
    ldata.scalebar = true;
  }
  prj->stat.export_pdftex(tf, this->layout, &ldata);
  fclose(tf);

  // teraz sa hodi do temp adresara - spusti metapost, thpdf, a pdftex a skopiruje vysledok
  thbuffer com, wdir;
  wdir.guarantee(1024);
  getcwd(wdir.get_buffer(),1024);
  chdir(thtmp.get_dir_name());
  
  // vypise kodovania
  print_fonts_setup();
  
  int retcode;
  
#ifdef THWIN32
  if (!thini.tex_env) {
    putenv("TEXMFCNF=");
    putenv("DVIPSHEADERS=");
    putenv("GFFONTS=");
    putenv("GLYPHFONTS=");
    putenv("MFBASES=");
    putenv("MFINPUTS=");
    putenv("MFPOOL=");
#ifdef THMSVC
    putenv("MPINPUTS=../mpost;.");
#else
    putenv("MPINPUTS=");
#endif
    putenv("MPMEMS=");
    putenv("MPPOOL=");
    putenv("MPSUPPORT=");
    putenv("PKFONTS=");
    putenv("PSHEADERS=");
    putenv("T1FONTS=");
    putenv("T1INPUTS=");
    putenv("T42FONTS=");
    putenv("TEXCONFIG=");
    putenv("TEXDOCS=");
    putenv("TEXFONTMAPS=");
    putenv("TEXFONTS=");
    putenv("TEXFORMATS=");
    putenv("TEXINPUTS=");
    putenv("TEXMFDBS=");
    putenv("TEXMFINI=");
    putenv("TEXPICTS=");
    putenv("TEXPKS=");
    putenv("TEXPOOL=");
    putenv("TEXPSHEADERS=");
    putenv("TEXSOURCES=");
    putenv("TFMFONTS=");
    putenv("TTFONTS=");
    putenv("VFFONTS=");
    putenv("WEB2C=");
  }
#endif  
  
  if (!quick_map_exp) {
    com = "\"";
    com += thini.get_path_mpost();
    com += "\"";
//    com += " --interaction nonstopmode data.mp";
    com += " data.mp";
#ifdef THDEBUG
    thprintf("running metapost\n");
#endif
    retcode = system(com.get_buffer());
    thexpmap_log_log_file("data.log",
    "####################### metapost log file ########################\n",
    "#################### end of metapost log file ####################\n",true);
    if (retcode != EXIT_SUCCESS) {
      chdir(wdir.get_buffer());
      ththrow(("metapost exit code -- %d", retcode))
    }
  }

/*
  if (quick_map_exp)
    com = "thpdf -q";
  else
    com = "thpdf";
*/


#ifdef THWIN32
      unsigned long cpch;
      char * cpcmd;
#endif


  switch (this->format) {

    case TT_EXPMAP_FMT_PDF:
      if (!quick_map_exp)
        thconvert();

      thpdf((this->export_mode == TT_EXP_MAP ? 1 : 0));

      com = "\"";
      com += thini.get_path_pdftex();
      com += "\"";
    //  com += " --interaction nonstopmode data.tex";
      com += " data.tex";
#ifdef THDEBUG
      thprintf("running pdftex\n");
#endif
      retcode = system(com.get_buffer());
      thexpmap_log_log_file("data.log",
      "######################## pdftex log file #########################\n",
      "##################### end of pdftex log file #####################\n",false);
      if (retcode != EXIT_SUCCESS) {
        chdir(wdir.get_buffer());
        ththrow(("pdftex exit code -- %d", retcode))
      }

      // Let's copy results and log-file to working directory
      chdir(wdir.get_buffer());
#ifdef THWIN32
      com = "copy ";
#else
      com = "cp ";
#endif
      com += thtmp.get_file_name("data.pdf");
      com += " ";
      com += fnm;

#ifdef THWIN32
      cpcmd = com.get_buffer();
      for(cpch = 0; cpch < strlen(cpcmd); cpch++) {
       if (cpcmd[cpch] == '/') {
         cpcmd[cpch] = '\\';
       }
     }
#endif

#ifdef THDEBUG
      thprintf("copying results\n");
#endif
      retcode = system(com.get_buffer());
      if (retcode != EXIT_SUCCESS)
        ththrow(("cp exit code -- %d", retcode))
      break;
      // END OF PDF POSTPROCESSING

    case TT_EXPMAP_FMT_SVG:
      thconvert_new();
      chdir(wdir.get_buffer());
      thsvg(fnm, 0, ldata);
      break;
      

    case TT_EXPMAP_FMT_XHTML:
      thconvert_new();
      chdir(wdir.get_buffer());
      thsvg(fnm, 1, ldata);
      break;

  }


#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
  
}

struct vec2d {
	double x;
	double y;
	vec2d() : x(0.0), y(0.0) {}
	vec2d(double xx, double yy) : x(xx), y(yy) {}
	void set(double xx, double yy);
};

void vec2d::set(double xx, double yy) {
	this->x = xx;
	this->y = yy;
}


thexpmap_xmps thexpmap::export_mp(thexpmapmpxs * out, class thscrap * scrap, 
    unsigned & startnum, bool outline_mode)
{
  th2ddataobject * obj;
  thpoint * ptp;
  thexpmapmpxs nooutc, * noout;
  nooutc = *out;
  noout = &(nooutc);
  noout->file = NULL;
  thexpmap_xmps result;
  bool clip_polygon;
  clip_polygon = (!scrap->centerline_io) && (scrap->get_outline() != NULL);
  unsigned from, to;
  int placeid;
  thscraplo * lo, * lo2;
  bool somex, first, vis;
  thscraplp * slp;
  thdb2dlp * lp;
  thexception dbg_stnms;
  bool map_shift = (hypot(out->m_shift_x, out->m_shift_y) > 1e-2);
	
	// check scrap limits
  fprintf(out->file, "warningcheck := 1;\n");
	if (!thisnan(scrap->lxmin)) {
		vec2d v1, v2;
		v1.set(thxmmxst(out, scrap->lxmin, scrap->lymin)); 
		v2.set(thxmmxst(out, scrap->lxmax, scrap->lymax));
		double max, minscale;
		max = 0;
#define mmmmcx(v) if (fabs(v) > max) max = fabs(v);
		mmmmcx(v1.x);
		mmmmcx(v1.y);
		mmmmcx(v2.x);
		mmmmcx(v2.y);
		if (max > 4095.0) {
		  minscale = (1.0 / out->layout->scale) * (max / 4095.0);
			if (scrap->centerline_io) {
			  fprintf(out->file, "warningcheck := 0;\n");
				if (max > 32767.0) {
				  minscale = (1.0 / out->layout->scale) * (max / 32767.0);
					ththrow(("scale too large -- maximal scale for this configuration is 1 : %.0f", minscale + 1));
				}
			} else {
				ththrow(("scrap %s@%s defined at %s [%d] is too large to process in metapost in this scale -- maximal scale for this scrap is 1 : %.0f", 
					scrap->name, 
					scrap->fsptr->full_name, 
					scrap->source.name, 
					scrap->source.line,
					minscale + 1));
			}
		}
	}

	
  if (scrap->fsptr != NULL) {  
    fprintf(out->file, "current_scrap := \"%s@%s\";\n", scrap->name, 
      scrap->fsptr->full_name);
  }
  if (strcmp(out->attr_last_scrap, scrap->name) != 0) {
    fprintf(out->file, "ATTR__scrap := \"%s\";\n", scrap->name);
    out->attr_last_scrap = scrap->name;
  }
  if (out->attr_last_scrap_centerline != scrap->centerline_io) {
    fprintf(out->file, "ATTR__scrap_centerline := %s;\n", (scrap->centerline_io ? "true" : "false"));
    out->attr_last_scrap_centerline = scrap->centerline_io;
  }
  scrap->db->attr.export_mp_object(out->file, (long) scrap->id);

  // preskuma vsetky objekty a ponastavuje im clip tagy
  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    if ((obj->tags & TT_2DOBJ_TAG_CLIP_AUTO) > 0) {
        obj->tags &= ~(TT_2DOBJ_TAG_CLIP_AUTO | TT_2DOBJ_TAG_CLIP_ON);
      switch (obj->get_class_id()) {
        case TT_POINT_CMD:
          switch (((thpoint*)obj)->type) {
            case TT_POINT_TYPE_DIMENSIONS:
              obj->tags &= ~TT_2DOBJ_TAG_VISIBILITY_ON;
              break;

            case TT_POINT_TYPE_MAP_CONNECTION:
              if (!map_shift) {
                obj->tags &= ~TT_2DOBJ_TAG_VISIBILITY_ON;
              }
              break;

            case TT_POINT_TYPE_BEDROCK:
            case TT_POINT_TYPE_SAND:
            case TT_POINT_TYPE_RAFT:
            case TT_POINT_TYPE_CLAY:
            case TT_POINT_TYPE_PEBBLES:
            case TT_POINT_TYPE_DEBRIS:
            case TT_POINT_TYPE_BLOCKS:
            case TT_POINT_TYPE_WATER:
            case TT_POINT_TYPE_ICE:
            case TT_POINT_TYPE_GUANO:
              obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
              break;
          }
          break;
        case TT_LINE_CMD:
          if ((((thline*)obj)->outline == TT_LINE_OUTLINE_NONE)) { 
            switch (((thline*)obj)->type) {
              case TT_LINE_TYPE_SECTION:
              case TT_LINE_TYPE_ARROW:
              case TT_LINE_TYPE_LABEL:
              case TT_LINE_TYPE_WATER_FLOW:
              case TT_LINE_TYPE_GRADIENT:
              case TT_LINE_TYPE_MAP_CONNECTION:
                break;
              default:
                obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
                break;
            }
          }  
          break;
        case TT_AREA_CMD:
          obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
          break;
      }
    }  
    obj = obj->pscrapoptr;
  }
  
  
  // vypise objekty pred orezanim
  somex = false;
#define thexpmap_export_mp_bgif if (!somex) {\
  result.F = startnum++;\
  fprintf(out->file,"beginfig(%ld);\n",result.F);} \
  somex = true

#define thxmempiov ((obj->tags & TT_2DOBJ_TAG_VISIBILITY_ON) > 0)
  // najprv vyplne
  for (placeid = TT_2DOBJ_PLACE_BOTTOM; placeid <= TT_2DOBJ_PLACE_TOP; placeid++) {
    obj = scrap->ls2doptr;
    if (outline_mode) obj = NULL;
    while (obj != NULL) {
      if (thxmempiov && ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) > 0) && (obj->place == placeid)) {
        if (obj->export_mp(noout)) {
          thexpmap_export_mp_bgif;
          obj->export_mp(out);
        }
      }
      obj = obj->pscrapoptr;
    }
  }

  // a polygon
  if (clip_polygon) {
    slp = scrap->get_polygon();
    if (outline_mode) slp = NULL;
    while (slp != NULL) {
      if (slp->lnio) {
        if (out->symset->assigned[slp->type]) {
          thexpmap_export_mp_bgif;
          slp->export_mp(out, scrap);
          //fprintf(out->file,"%s(((%.2f,%.2f) -- (%.2f,%.2f)));\n",
          //  out->symset->get_mp_macro(slp->type),
          //  thxmmxst(out, slp->lnx1, slp->lny1),
          //  thxmmxst(out, slp->lnx2, slp->lny2));
        }
      }
      slp = slp->next_item;
    }
  }


  if (somex) {
    fprintf(out->file,"endfig;\n");
    result.flags |= TT_XMPS_F;
  }
  
  
  // vypise outline
  somex = false;
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) {\
  result.B = startnum++;\
  fprintf(out->file,"beginfig(%ld);\n",result.B);} \
  somex = true
  
  lo = scrap->get_outline();
  //if ((lo != NULL) && (lo->line->outline == TT_LINE_OUTLINE_OUT)) {
  if (true) {  /// ;-) uz mame podporovanych viac vonkajsich a viac vnut. obrysov 
    while (lo != NULL) {
      thexpmap_export_mp_bgif;
      fprintf(out->file,"draw_upscrap(%d)(",((lo->line->outline == TT_LINE_OUTLINE_OUT) ? 1 : 0));
      // prejde celu krivku a rozlozi ju na viditelne a neviditelne casti
      first = true;
      lo2 = lo;
      while (lo2 != NULL) {              
        
        if (lo2->mode == TT_OUTLINE_REVERSED) {
          lp = lo2->line->first_point;
          from = 0;
          while (lp != NULL) {
            lp = lp->nextlp;
            from++;
          }
          from = from - 1;
          lp = lo2->line->last_point;
          to = from;
        } else {
          lp = lo2->line->first_point;
          from = 0;
          to = from;
        }
        
        bool cvis = false;
        while (lp != NULL) {

          // najde viditelnost prveho bodu
          if (lo2->mode == TT_OUTLINE_REVERSED) {
            if (lp->prevlp != NULL)
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
              (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
            else
              vis = cvis;
          } else {
            vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
              (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
          }
          cvis = vis;
          
          // pokracuje pokial nemame bod z inou viditelnostou
          while (lp != NULL) {
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              if (lp->prevlp != NULL)
                vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
              else
                vis = cvis;
            } else {
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
            }
            
            if (vis != cvis)
              break;
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              lp = lp->prevlp;
              if (lp != NULL)
                from--;
            } else {
              lp = lp->nextlp;
              if (lp != NULL)
                to++;
            }
          }

          if (!first) {
            fprintf(out->file,",\n");
          }
          first = false;
                    
          if (cvis) {
            fprintf(out->file,"1,");
          } else {
            fprintf(out->file,"0,");
          }
          
          if (lo2->mode == TT_OUTLINE_REVERSED) {
            fprintf(out->file,"reverse ");
            lo2->line->export_path_mp(out,from,to);
            to = from;
          } else {
            lo2->line->export_path_mp(out,from,to);
            from = to;
          }
          
        }
        
        lo2 = lo2->next_line;
      }
      fprintf(out->file,");\n");
      lo = lo->next_outline;
    }
  }
  if (somex) {
    fprintf(out->file,"draw_downscrap;\n");
    fprintf(out->file,"endfig;\n");
    result.flags |= TT_XMPS_B;
  }
  
  // vypise objekty po orezani
  somex = false;
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) {\
  result.E = startnum++;\
  fprintf(out->file,"beginfig(%ld);\n",result.E);} \
  somex = true
  
  for (placeid = TT_2DOBJ_PLACE_BOTTOM; placeid <= TT_2DOBJ_PLACE_TOP; placeid++) {
    obj = scrap->ls2doptr;
    if (outline_mode) obj = NULL;
    while (obj != NULL) {
      if (thxmempiov && ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) == 0) && (obj->place == placeid)) {
        switch (obj->get_class_id()) {
          case TT_LINE_CMD:
            switch (((thline*)obj)->type) {
              case TT_LINE_TYPE_LABEL:
                break;
              default:
                if (obj->export_mp(noout)) {
                  thexpmap_export_mp_bgif;
                  obj->export_mp(out);
                }
                break;
            }
            break;
          case TT_POINT_CMD:
            ptp = ((thpoint*)obj);
            switch (ptp->type) {
              case TT_POINT_TYPE_STATION:
              case TT_POINT_TYPE_STATION_NAME:
              case TT_POINT_TYPE_LABEL:
              case TT_POINT_TYPE_REMARK:
              case TT_POINT_TYPE_DATE:
              case TT_POINT_TYPE_ALTITUDE:
              case TT_POINT_TYPE_HEIGHT:
              case TT_POINT_TYPE_PASSAGE_HEIGHT:
                break;
              case TT_POINT_TYPE_MAP_CONNECTION:
                if (out->symset->assigned[SYML_MAPCONNECTION]) {
                  thexpmap_export_mp_bgif;
                  fprintf(out->file,"%s(((%.2f,%.2f) -- (%.2f,%.2f)));\n",
                  out->symset->get_mp_macro(SYML_MAPCONNECTION),
                  thxmmxst(out, ptp->point->xt, ptp->point->yt),
                  thxmmxst(out, ptp->point->xt - out->m_shift_x, ptp->point->yt - out->m_shift_y));
                }
                break;
              default:
                if (obj->export_mp(noout)) {
                  thexpmap_export_mp_bgif;
                  obj->export_mp(out);
                }
                break;
            }
            break;
          case TT_AREA_CMD:
            if (obj->export_mp(noout)) {
              thexpmap_export_mp_bgif;
              obj->export_mp(out);
            }
            break;
        }
      }
      obj = obj->pscrapoptr;
    }
  }

  // a polygon
  if (!clip_polygon) {
    slp = scrap->get_polygon();
    if (outline_mode) slp = NULL;
    while (slp != NULL) {
      if (slp->lnio) {
        if (out->symset->assigned[slp->type]) {
          thexpmap_export_mp_bgif;
          slp->export_mp(out, scrap);
        }
      }
      slp = slp->next_item;
    }
  }

    
  // nakoniec meracske body
  // najprv z polygonu
  int macroid, typid;
  thdb1ds * tmps;
  slp = scrap->get_polygon();
  if (outline_mode) slp = NULL;
  while (slp != NULL) {
    if (!slp->lnio) {
#define thexpmat_station_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;
      macroid = SYMP_STATION_TEMPORARY;
      switch(slp->station->mark) {
        thexpmat_station_type_export_mp(TT_DATAMARK_FIXED,SYMP_STATION_FIXED)
        thexpmat_station_type_export_mp(TT_DATAMARK_NATURAL,SYMP_STATION_NATURAL)
        thexpmat_station_type_export_mp(TT_DATAMARK_PAINTED,SYMP_STATION_PAINTED)
      }
      if (slp->station->flags & TT_STATIONFLAG_UNDERGROUND)
        typid = SYMP_CAVESTATION;
      else
        typid = SYMP_SURFACESTATION;
      if (out->symset->assigned[macroid] && out->symset->assigned[typid]) {
        thexpmap_export_mp_bgif;
        fprintf(out->file,"%s((%.2f,%.2f));\n",
          out->symset->get_mp_macro(macroid),
          thxmmxst(out, slp->stx, slp->sty));
        if (out->layout->is_debug_stationnames() && (slp->station_name.id != 0)) {
          tmps = &(thdb.db1d.station_vec[slp->station_name.id - 1]);
          dbg_stnms.appspf("p_label.urt(btex \\thstationname %s etex, (%.2f, %.2f), 0.0, 7);\n",
            (char *) utf2tex(thobjectname__print_full_name(tmps->name, tmps->survey, layout->survey_level)), 
            thxmmxst(out, slp->stx, slp->sty));
        }
      }
    }
    slp = slp->next_item;
  }
  
  // potom zo scrapu
  bool expstation;
  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    if (thxmempiov && ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) == 0)) {
      switch (obj->get_class_id()) {
        case TT_POINT_CMD:
          if (((thpoint*)obj)->type == TT_POINT_TYPE_STATION) {
            ptp = (thpoint *) obj;
            expstation = true;
            if (ptp->station_name.id != 0) {
              thdb1ds * station = &(thdb.db1d.station_vec[ptp->station_name.id - 1]);
              if (station->flags & TT_STATIONFLAG_UNDERGROUND)
                typid = SYMP_CAVESTATION;
              else
                typid = SYMP_SURFACESTATION;
              expstation = out->symset->assigned[typid];
            }
            if (expstation) {
              if (obj->export_mp(noout)) {
                thexpmap_export_mp_bgif;
                obj->export_mp(out);
                if (out->layout->is_debug_stationnames() && (ptp->station_name.id != 0)) {
                  tmps = &(thdb.db1d.station_vec[ptp->station_name.id - 1]);
                  dbg_stnms.appspf("p_label.urt(btex \\thstationname %s etex, (%.2f, %.2f), 0.0, 7);\n",
                    (char *) utf2tex(thobjectname__print_full_name(tmps->name, tmps->survey, layout->survey_level)), 
                    thxmmxst(out, ptp->point->xt, ptp->point->yt));
                }
              }
            }
          }
          break;
      }
    }
    obj = obj->pscrapoptr;
  }

  if ((scrap->maxdistp1 != NULL) && (out->layout->is_debug_stations())) {
    thexpmap_export_mp_bgif;
    fprintf(out->file,"p_debug(-1,1,");
    scrap->maxdistp1->export_mp(out);
    fprintf(out->file,");\np_debug(-1,1,");
    scrap->maxdistp2->export_mp(out);
    fprintf(out->file,");\nl_debug(-1,1,(");
    scrap->maxdistp1->export_mp(out);
    fprintf(out->file," -- ");
    scrap->maxdistp2->export_mp(out);
    fprintf(out->file,"));\np_debug(-2,1,");
    scrap->maxdistp1->export_mp(out,0);
    fprintf(out->file,");\np_debug(-2,1,");
    scrap->maxdistp2->export_mp(out,0);
    fprintf(out->file,");\nl_debug(-2,1,(");
    scrap->maxdistp1->export_mp(out);
    fprintf(out->file," -- ");
    scrap->maxdistp1->export_mp(out,0);
    fprintf(out->file," -- ");
    scrap->maxdistp2->export_mp(out,0);
    fprintf(out->file," -- ");
    scrap->maxdistp2->export_mp(out);
    fprintf(out->file,"));\n");
  }
  
  if (somex) {
    fprintf(out->file,"endfig;\n");  
    result.flags |= TT_XMPS_E;
  }
  
  // teraz spracuje labels
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) {\
  result.X = startnum++;\
  fprintf(out->file,"beginfig(%ld);\n",result.X);} \
  somex = true
  somex = false;
  
  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    if (!thxmempiov) goto DO_NOT_EXPORT_LABEL;
    switch (obj->get_class_id()) {
    
      // altitude labels zo steny
      case TT_LINE_CMD:
        switch (((thline*)obj)->type) {
          case TT_LINE_TYPE_WALL:
            if (!out->symset->assigned[SYMP_WALLALTITUDE])
              break;
            // prescanuje stenu
            lp = ((thline*)obj)->first_point;
            while(lp != NULL) {
              if (((lp->tags | TT_LINEPT_TAG_ALTITUDE) > 0) &&
                  (!thisnan(lp->rsize))) {
                thexpmap_export_mp_bgif;
                fprintf(out->file,"%s(",out->symset->get_mp_macro(SYMP_WALLALTITUDE));
                lp->export_prevcp_mp(out);
                fprintf(out->file,",");
                lp->point->export_mp(out);
                fprintf(out->file,",");
                lp->export_nextcp_mp(out);
                thdb.buff_enc.guarantee(4096);
                //sprintf(thdb.buff_enc.get_buffer(),"%.0f",lp->rsize - out->layout->goz);
                fprintf(out->file,",btex \\thwallaltitude %s etex);\n",utf2tex(out->layout->units.format_length(lp->rsize - out->layout->goz)));
//                fprintf(out->file,",\"%.0f\");\n",lp->rsize);
              }
              lp = lp->nextlp;
            }
            break;
          case TT_LINE_TYPE_LABEL:
            if (((thline*)obj)->export_mp(noout)) {
              thexpmap_export_mp_bgif;
              ((thline*)obj)->export_mp(out);
            }
            break;
        }
        break;
      case TT_POINT_CMD:
        switch (((thpoint*)obj)->type) {
          case TT_POINT_TYPE_LABEL:
          case TT_POINT_TYPE_REMARK:
          case TT_POINT_TYPE_STATION_NAME:
          case TT_POINT_TYPE_DATE:
          case TT_POINT_TYPE_ALTITUDE:
          case TT_POINT_TYPE_HEIGHT:
          case TT_POINT_TYPE_PASSAGE_HEIGHT:
            if (((thpoint*)obj)->export_mp(noout)) {
              thexpmap_export_mp_bgif;
              ((thpoint*)obj)->export_mp(out);
            }
            break;
        }
        break;
    }
    DO_NOT_EXPORT_LABEL:
    obj = obj->pscrapoptr;
  }

  if (out->layout->is_debug_stationnames() && (strlen(dbg_stnms.get_buffer()) > 0)) {
    thexpmap_export_mp_bgif;
    fprintf(out->file, "%s", dbg_stnms.get_buffer());
  }
  
  // nakoniec scrap name, ak mame zapnuty dany debug mod
  if (out->layout->is_debug_scrapnames() && (scrap->fsptr != NULL)) {
    thexpmap_export_mp_bgif;
    thdb2dpt tmppt;
    tmppt.xt = (scrap->lxmin + scrap->lxmax) / 2.0;
    tmppt.yt = (scrap->lymin + scrap->lymax) / 2.0;
    thdb.buff_tmp = utf2tex(thobjectname__print_full_name(scrap->name, scrap->fsptr, layout->survey_level));
    fprintf(out->file,"p_label(btex \\thlargesize %s etex,",thdb.buff_tmp.get_buffer());
    tmppt.export_mp(out);
    fprintf(out->file,",0.0,6);\n");
  }


  if (somex) {
    fprintf(out->file,"endfig;\n");  
    result.flags |= TT_XMPS_X;
  }

  return result;
}



#define alpha_correction(AA,RR,GG,BB) { \
  RR = AA * RR + (1 - AA) * 1.0; \
  GG = AA * GG + (1 - AA) * 1.0; \
  BB = AA * BB + (1 - AA) * 1.0; \
}

#define opacity_correction(RR,GG,BB) { \
  if (this->layout->transparency) alpha_correction(this->layout->opacity, RR, GG, BB); \
}

#define tmp_alpha 0.75
#define tmp_set_color(r,g,b) cR = r; cG = g; cB = b

void thexpmap::export_pdf_set_colors(class thdb2dxm * maps, class thdb2dprj * prj)
{

  // prejde vsetky scrapy a nastavi im farbu na color_map_fg
  // farba priecnych rezov sa nastavuje pri exporte, zrata minz/maxz
  // a pocet kapitol

  COLORLEGENDLIST.clear();
  
  thdb2dxm * cmap;
  thdb2dxs * cbm;
  thdb2dmi * cmi;
  thscrap * cs;
  
  double minz = 0.0, maxz = -1.0;
  long cmn = 0, nmap = 0;
  double curz;
  bool firstmapscrap;
  double cR, cG, cB;
  
  // najprv to nascanuje  
  cmap = maps;
  while (cmap != NULL) {
    cbm = cmap->first_bm;
    firstmapscrap = true;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
//      if ((cbm->mode == TT_MAPITEM_NORMAL) && (strlen(cbm->bm->name) > 0)) while (cmi != NULL) {
      if (cbm->mode == TT_MAPITEM_NORMAL) while (cmi != NULL) {
        if (cmi->type == TT_MAPITEM_NORMAL) {
          if (firstmapscrap) {
            nmap++;
            firstmapscrap = false;
          }
          cs = (thscrap *) cmi->object;
          curz = cs->z;
          if ((!thisnan(curz)) && (cs->fsptr != NULL))
            curz += prj->shift_z;
          if (!thisnan(curz)) {
            if (minz > maxz) {
              minz = curz;
              maxz = curz;
            } else {
//              if (minz > curz)
//                thprintf("new min: %8.2f %s @ %s\n", curz, cs->name, (cs->fsptr != NULL ? cs->fsptr->full_name : ""));
//              if (maxz < curz)
//                thprintf("new max: %8.2f %s @ %s\n", curz, cs->name, (cs->fsptr != NULL ? cs->fsptr->full_name : ""));
              if (minz > curz)
                minz = curz;
              if (maxz < curz)
                maxz = curz;
            }
          }
          cs->R = this->layout->color_map_fg.R;
          cs->G = this->layout->color_map_fg.G;
          cs->B = this->layout->color_map_fg.B;
        }
        cmi = cmi->prev_item;  
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }
  
  if (this->layout->color_crit == TT_LAYOUT_CCRIT_UNKNOWN)
    return;

  bool addleg = (this->layout->color_legend == TT_TRUE);
  colorlegendrecord clrec;
  
  // urobi altitude legendu
  long xalt;
  thbuffer tmpb;
  tmpb.guarantee(2048);
  if (addleg && (maxz > minz) && (this->layout->color_crit == TT_LAYOUT_CCRIT_ALTITUDE)) {
    for (xalt = 5; xalt >= 0; xalt--) {
      curz = double(xalt) / 5.0 * (maxz - minz) + minz;
      thset_color(0, double(5 - xalt), 5.0, cR, cG, cB);
      alpha_correction(tmp_alpha, cR, cG, cB);
      clrec.R = cR;
      clrec.G = cG; 
      clrec.B = cB;
      opacity_correction(clrec.R, clrec.G, clrec.B);
      //sprintf(tmpb.get_buffer(), "%.0f", curz - this->layout->goz);
      clrec.texname = utf2tex(this->layout->units.format_length(curz - this->layout->goz));
      clrec.texname += "\\thinspace ";			
      clrec.texname += utf2tex(this->layout->units.format_i18n_length_units());
      clrec.name = this->layout->units.format_length(curz - this->layout->goz);
      clrec.name += " ";			
      clrec.name += this->layout->units.format_i18n_length_units();
      COLORLEGENDLIST.insert(COLORLEGENDLIST.end(), clrec);
    }
  }
  
  // potom to nastavi
//  thprintf("\n");
  cmn = 0;
  nmap = nmap - 1;
  cmap = maps;
  while (cmap != NULL) {
    cbm = cmap->first_bm;
    firstmapscrap = true;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
//      if ((cbm->mode == TT_MAPITEM_NORMAL) && (strlen(cbm->bm->name) > 0)) while (cmi != NULL) {
      if (cbm->mode == TT_MAPITEM_NORMAL) while (cmi != NULL) {
        if (cmi->type == TT_MAPITEM_NORMAL) {
          cs = (thscrap *) cmi->object;
          curz = cs->z;
          if ((!thisnan(curz)) && (cs->fsptr != NULL))
            curz += prj->shift_z;
          switch (this->layout->color_crit) {
            case TT_LAYOUT_CCRIT_MAP:
              // vsetkym scrapom v kazdej priradi farbu
              if (firstmapscrap) {
                thset_color(0, (double) (nmap - cmn), (double) nmap, cR, cG, cB);
                clrec.texname = utf2tex(strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name);
                clrec.name = (strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name);
                alpha_correction(tmp_alpha, cR, cG, cB);
                clrec.R = cR;
                clrec.G = cG; 
                clrec.B = cB;
                opacity_correction(clrec.R, clrec.G, clrec.B);
                COLORLEGENDLIST.insert(COLORLEGENDLIST.begin(), clrec);
                firstmapscrap = false;
                cmn++;
              }
              cs->R = cR;
              cs->G = cG;
              cs->B = cB;
//              thprintf("%s@%s->%.2f,%.2f,%.2f\n",cs->name,cs->fsptr->full_name,cs->R,cs->G,cs->B);
            break;
            case TT_LAYOUT_CCRIT_SCRAP:
              // vsetkym scrapom v kazdej priradi farbu
							switch (cmn % 6) {
								case 0:  tmp_set_color(1.0, 0.5, 0.5); break;
								case 1:  tmp_set_color(0.5, 1.0, 0.5); break;
								case 2:  tmp_set_color(0.5, 0.5, 1.0); break;
								case 3:  tmp_set_color(1.0, 1.0, 0.0); break;
								case 4:  tmp_set_color(0.0, 1.0, 1.0); break;
								default: tmp_set_color(1.0, 0.0, 1.0); break;
							}
              alpha_correction(tmp_alpha, cR, cG, cB);
              cs->R = cR;
              cs->G = cG;
              cs->B = cB;
              cmn++;
            break;
            case TT_LAYOUT_CCRIT_ALTITUDE:
              // priradi farbu podla (z - min) z (max - min)
              if (!thisnan(curz)) {
                thset_color(0, (maxz - curz), (maxz - minz), cs->R, cs->G, cs->B);
                alpha_correction(tmp_alpha, cs->R, cs->G, cs->B);
              } else {
                cs->R = this->layout->color_map_fg.R;
                cs->G = this->layout->color_map_fg.G;
                cs->B = this->layout->color_map_fg.B;
              }
            break;
            default:
              cs->R = this->layout->color_map_fg.R;
              cs->G = this->layout->color_map_fg.G;
              cs->B = this->layout->color_map_fg.B;
          }
        }
        cmi = cmi->prev_item;  
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

}


void thexpmap::export_uni(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  char * fnm = this->get_output("cave.3d");

  img * pimg;
  img_output_version = 4;
  pimg = img_open_write(fnm, "cave", 1);
  if (pimg == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;

  while (cmap != NULL) {
    cbm = cmap->first_bm;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      if (cbm->mode == TT_MAPITEM_NORMAL) {
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL)
						this->export_uni_scrap((FILE *) pimg, (thscrap *) cmi->object);
          cmi = cmi->prev_item;  
        }
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  img_close(pimg);
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpmap_line_svx3d(img * pimg, thline * pln)
{
	thdb2dlp * plp = pln->first_point, * lp;
	if (plp != NULL) 
		lp = plp->nextlp;
	else 
		return;
	bool move_to = true;
	double t, tt, ttt, t_, tt_, ttt_, nx, ny, nz, px(0.0), py(0.0);
	while (lp != NULL) {
		if (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE) {
			if (move_to) {
	      img_write_item(pimg, img_MOVE, 0, NULL, plp->point->xt, plp->point->yt, plp->point->zt);
				px = plp->point->xt;
				py = plp->point->yt;
				move_to = false;
			}
			if ((lp->cp1 != NULL) && (lp->cp2 != NULL)) {
				for(t = 0.05; t < 1.0; t += 0.05) {
				  tt = t * t;
					ttt = tt * t;
					t_ = 1.0 - t;
					tt_ = t_ * t_;
					ttt_ = tt_ * t_;				
					nx = ttt_ * plp->point->xt + 
							 3.0 * t * tt_ * lp->cp1->xt + 
							 3.0 * tt * t_ * lp->cp2->xt + 
							 ttt * lp->point->xt;
					ny = ttt_ * plp->point->yt + 
							 3.0 * t * tt_ * lp->cp1->yt + 
							 3.0 * tt * t_ * lp->cp2->yt + 
							 ttt * lp->point->yt;
					nz = t_ * plp->point->zt + t * lp->point->zt;
					if (hypot(nx - px, ny - py) > 0.5) {
			      img_write_item(pimg, img_LINE, 0, NULL, nx, ny, nz);
						px = nx;
						py = ny;
					}
				}			
			}
      img_write_item(pimg, img_LINE, 0, NULL, lp->point->xt, lp->point->yt, lp->point->zt);
			px = lp->point->xt;
			py = lp->point->yt;
		} else {
			move_to = true;
		}
		plp = lp;
		lp = lp->nextlp;
	}
}


void thexpmap::export_uni_scrap(FILE * out, class thscrap * scrap)
{
	
	img * pimg;
	pimg = (img *) out;
	thbuffer stnbuff;
	
	double avx = 0.0, avy = 0.0, avz = 0.0, avn = 0.0;
#define avadd(x,y,z) {avx	+= x; avy += y; avz += z; avn += 1.0;}
	// export shots
  thscraplp * slp;
  slp = scrap->get_polygon();
  while (slp != NULL) {
    if (slp->lnio) {
      img_write_item(pimg, img_MOVE, 0, NULL, slp->lnx1, slp->lny1, slp->lnz1);
      img_write_item(pimg, img_LINE, img_FLAG_SURFACE, NULL, slp->lnx2, slp->lny2, slp->lnz2);
    } else {
			stnbuff = slp->station->survey->get_reverse_full_name();
			stnbuff += ".";
			stnbuff += slp->station->name;
//			img_write_item(pimg, img_LABEL, 0, stnbuff, slp->stx,  slp->sty,  slp->stz);
			avadd(slp->stx,  slp->sty,  slp->stz);
		}
    slp = slp->next_item;
  }
	
	// export objects
  th2ddataobject * obj;
	thpoint * ppt;
	thline * pln;
  obj = scrap->ls2doptr;
  while (obj != NULL) {
		switch (obj->get_class_id()) {
			case TT_POINT_CMD:
				ppt = ((thpoint*)obj);
	      switch (ppt->type) {
	        case TT_POINT_TYPE_STATION:
						if ((ppt->cpoint != NULL) && (ppt->cpoint->st != NULL)) {
							stnbuff = ppt->cpoint->st->survey->get_reverse_full_name();
							stnbuff += ".";
							stnbuff += ppt->cpoint->st->name;
//							img_write_item(pimg, img_LABEL, img_SFLAG_SURFACE, stnbuff, ppt->point->xt, ppt->point->yt, ppt->point->zt);
							avadd(ppt->point->xt, ppt->point->yt, ppt->point->zt);
						}
  		      break;
				}
				break;
		  case TT_LINE_CMD:
				pln = ((thline*)obj);
	      switch (pln->type) {
					case TT_LINE_TYPE_WALL:
						thexpmap_line_svx3d(pimg, pln);
						thdb2dlp * lp = pln->first_point;
						while (lp != NULL) {
							avadd(lp->point->xt,lp->point->yt,lp->point->zt);
							lp = lp->nextlp;
						}
						break;
				}
				break;
    }
    obj = obj->pscrapoptr;
  }

	if (avn > 0.0) {
//		stnbuff = "SCRAP.";
		stnbuff = scrap->fsptr->get_reverse_full_name();
		stnbuff += ".";
		stnbuff += scrap->name;
		img_write_item(pimg, img_LABEL, 0, stnbuff, avx/avn, avy/avn, avz/avn);
	}
		
}

bool th2ddataobject::export_mp(thexpmapmpxs * out)
{
  if (out->file != NULL) {
    if (strcmp(out->attr_last_id, this->name) != 0) {
      fprintf(out->file,"ATTR__id := \"%s\";\n",this->name);
      out->attr_last_id = this->name;
    }
    if (strcmp(out->attr_last_survey, this->fsptr->full_name) != 0) {
      fprintf(out->file,"ATTR__survey := \"%s\";\n",this->fsptr->full_name);
      out->attr_last_survey = this->fsptr->get_full_name();
    }
    this->db->attr.export_mp_object(out->file, (long) this->id);
  }
  return true;
}


double thexpmap_quick_map_export_scale;
