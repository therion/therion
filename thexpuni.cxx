/**
 * @file thexpuni.cxx
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
#include "thlayout.h"
#include "thmap.h"
#include "thsketch.h"
#include "thconfig.h"
#include <stdio.h>
#include "thtmpdir.h"
#include "thcsdata.h"

#ifdef THMSVC
#define hypot _hypot
#define snprintf _snprintf
#define strcasecmp _stricmp
#endif

#include "thchenc.h"
#include "thdb1d.h"
#include "thinit.h"
#include "thlogfile.h"
#include "thcmdline.h"
#include "thsurvey.h"
#include "thchenc.h"
#include <fstream>
#include "thmapstat.h"
#include "thsurface.h"
#include <stdlib.h>
#include "extern/lxMath.h"
#include "extern/shapefil.h"
#include "thexpmodel.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "thexpuni.h"
#include "thproj.h"


thexpuni::thexpuni()
{
  this->clear();
}

void thexpuni::clear()
{
  this->m_cpart = NULL;
  this->m_part_list.clear();
}


void thexpuni::polygon_start_ring(bool outer)
{
  this->m_ring_list.clear();
  thexpuni_part p;
  p.m_outer = outer;
  this->m_cpart = &(*this->m_part_list.insert(this->m_part_list.end(), p));
}



// insert points from bezier curve
void insert_line_segment(thline * ln, bool reverse, std::list<thexpuni_data> & lst, long startp = 0, long endp = -1)
{
  thdb2dlp * cpt, * prevpt;
  thdb2dpt * cp1, * cp2;
	double t, tt, ttt, t_, tt_, ttt_, nx, ny, nz, na, px, py;
  long pnum;

  prevpt = NULL;
  if (reverse)
    cpt = ln->last_point;
  else
    cpt = ln->first_point;

  pnum = 0;
  while (cpt != NULL) {

    // insert bezier curve
    if (pnum == startp)
      prevpt = NULL;

    if ((pnum >= startp) && ((endp < 0) || (pnum <= endp))) {
      if (prevpt != NULL) {
        px = prevpt->point->xt;
        py = prevpt->point->yt;
        if (reverse) {
          cp1 = prevpt->cp2;
          cp2 = prevpt->cp1;
        } else {
          cp1 = cpt->cp1;
          cp2 = cpt->cp2;
        }
        if ((cp1 != NULL) && (cp2 == NULL)) cp2 = cp1;
        if ((cp1 == NULL) && (cp2 != NULL)) cp1 = cp2;
        if ((cp1 != NULL) && (cp2 != NULL)) {
				  for(t = 0.05; t < 1.0; t += 0.05) {
				    tt = t * t;
					  ttt = tt * t;
					  t_ = 1.0 - t;
					  tt_ = t_ * t_;
					  ttt_ = tt_ * t_;				
					  nx = ttt_ * prevpt->point->xt + 
							  3.0 * t * tt_ * cp1->xt + 
							  3.0 * tt * t_ * cp2->xt + 
							  ttt * cpt->point->xt;
					  ny = ttt_ * prevpt->point->yt + 
							  3.0 * t * tt_ * cp1->yt + 
							  3.0 * tt * t_ * cp2->yt + 
							  ttt * cpt->point->yt;
					  nz = t_ * cpt->point->zt + t * prevpt->point->zt;
					  na = t_ * cpt->point->at + t * prevpt->point->at;
            // resolution 0.1 m
					  if (hypot(nx - px, ny - py) > 0.304) {
			        lst.push_back(thexpuni_data(nx + ln->fscrapptr->proj->rshift_x, ny + ln->fscrapptr->proj->rshift_y, nz + ln->fscrapptr->proj->rshift_y, na));
						  px = nx;
						  py = ny;
					  }
          }
        }
      }
    }

    // insert point it self
    if ((pnum >= startp) && ((endp < 0) || (pnum <= endp)))
      lst.push_back(thexpuni_data(cpt->point->xt + ln->fscrapptr->proj->rshift_x, cpt->point->yt + ln->fscrapptr->proj->rshift_y, cpt->point->zt + ln->fscrapptr->proj->rshift_z, cpt->point->at));

    // next point
    prevpt = cpt;
    if (reverse)
      cpt = cpt->prevlp;
    else
      cpt = cpt->nextlp;
    pnum++;

  }

}


void thexpuni::polygon_insert_line(thline * ln, bool reverse)
{
  insert_line_segment(ln, reverse, this->m_ring_list);
}


void thexpuni::polygon_close_ring()
{

  // check polygon orientation
  double area;
  std::list<thexpuni_data>::iterator i, iprev;

  if (this->m_ring_list.size() < 2)
    return;

  iprev = this->m_ring_list.end();
  iprev--;
  area = 0.0;
  for (i = this->m_ring_list.begin(); i != this->m_ring_list.end(); i++) {
    area += iprev->m_x * i->m_y - iprev->m_y * i->m_x;
    iprev = i;
  }
  area *= 0.5;

  bool reverse = ((area > 0) && (this->m_cpart->m_outer)) || ((area < 0) && (!this->m_cpart->m_outer));

  // according to orientation, insert points to point list
  if (reverse) {
    for(i = this->m_ring_list.end(); i != this->m_ring_list.begin(); ) {
      i--;
      this->m_cpart->m_point_list.push_back(*i);
    } 
  } else {
    for(i = this->m_ring_list.begin(); i != this->m_ring_list.end(); i++) {
      this->m_cpart->m_point_list.push_back(*i);
    } 
  }
}



void thexpuni::parse_scrap(thscrap * scrap)
{
	
	thbuffer stnbuff;

  // export scrap outline
  this->clear();
  thscraplo * lo = scrap->get_outline(), * lo2;
  while (lo != NULL) {
    if (lo->line->outline != TT_LINE_OUTLINE_NONE) {
      lo2 = lo;
      this->polygon_start_ring(lo->line->outline == TT_LINE_OUTLINE_OUT);
      this->m_cpart->m_lo = lo;
      while (lo2 != NULL) {
        this->polygon_insert_line(lo2->line, lo2->mode == TT_OUTLINE_REVERSED);
        lo2 = lo2->next_line;
      }
      this->polygon_close_ring();
    }    
    lo = lo->next_outline;
  }

}


void thexpuni::parse_line(thline * line)
{
  long sp = 0, cp = 1;
  int csubtype;
  this->clear();
  thdb2dlp * lp = line->first_point, * slp;
  if (lp == NULL)
    return;
  csubtype = lp->subtype;
  slp = lp;
  lp = lp->nextlp;
  while (lp != NULL) {
    if ((lp->subtype != csubtype) || (lp->nextlp == NULL)) {
      thexpuni_part p;
      this->m_cpart = &(*this->m_part_list.insert(this->m_part_list.end(), p));
      this->m_cpart->m_lp = slp;
      insert_line_segment(line, false, this->m_cpart->m_point_list, sp, cp);
      csubtype = lp->subtype; 
      sp = cp;
      slp = lp;
    }
    lp = lp->nextlp;
    cp++;
  }
}






void thexpmap::export_kml(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (thcfg.outcs == TTCS_LOCAL) {
    thwarning(("data not georeferenced -- unable to export KML file"));
    return;
  }

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  FILE * out;
  char * fnm = this->get_output("cave.kml");
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

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;


  thscrap * scrap;
  thexpuni xu;

  fprintf(out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.0\">\n<Document>\n");
  fprintf(out,"<name>Therion KML export</name>\n<description>Therion KML export.</description>\n");

  int cA, cR, cG, cB;
  cR = int (255.0 * this->layout->color_map_fg.R + 0.5);
  cG = int (255.0 * this->layout->color_map_fg.G + 0.5);
  cB = int (255.0 * this->layout->color_map_fg.B + 0.5);
  if (this->layout->transparency) {
    cA = int (255.0 * this->layout->opacity + 0.5);
  } else {
    cA = 255;
  }

#define checkc(c) if (c < 0) c = 0; if (c > 255) c = 255;
  checkc(cA);
  checkc(cR);
  checkc(cG);
  checkc(cB);

  fprintf(out,"<Placemark>\n");
  fprintf(out,"<Style>\n<PolyStyle>\n<color>");
  fprintf(out,"%02x%02x%02x%02x",cA,cB,cG,cR);
  fprintf(out,"</color>\n<fill>1</fill>\n<outline>0</outline>\n</PolyStyle>\n</Style>\n");
  fprintf(out,"<MultiGeometry>\n");

  while (cmap != NULL) {
    cbm = cmap->first_bm;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      if (cbm->mode == TT_MAPITEM_NORMAL) {
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            scrap = (thscrap*) cmi->object;
            xu.parse_scrap(scrap);
            if (xu.m_part_list.size() > 0) {
              fprintf(out,"<Polygon>\n");
              std::list<thexpuni_part>::iterator it;
              std::list<thexpuni_data>::iterator ip;
              double x,y,z;
              for(it = xu.m_part_list.begin(); it != xu.m_part_list.end(); it++) {
                if (it->m_outer)
                  fprintf(out,"<outerBoundaryIs>\n");
                else
                  fprintf(out,"<innerBoundaryIs>\n");

                fprintf(out,"<LinearRing>\n<coordinates>\n");
                for(ip = it->m_point_list.begin(); ip != it->m_point_list.end(); ip++) {
                  thcs2cs(thcsdata_table[thcfg.outcs].params, thcsdata_table[TTCS_LONG_LAT].params, 
                    ip->m_x, ip->m_y, scrap->z, x, y, z);
                  fprintf(out, "\t%20.14f,%20.14f,%20.14f\n", x / THPI * 180.0, y / THPI * 180.0, 0.0);
                }
                fprintf(out,"</coordinates>\n</LinearRing>\n");

                if (it->m_outer)
                  fprintf(out,"</outerBoundaryIs>\n");
                else
                  fprintf(out,"</innerBoundaryIs>\n");
              }
              fprintf(out,"</Polygon>\n");
            }
          }
          cmi = cmi->prev_item;  
        }
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  fprintf(out,"</MultiGeometry>\n</Placemark>\n");
  fprintf(out,"</Document>\n</kml>\n");
  fclose(out);
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}



