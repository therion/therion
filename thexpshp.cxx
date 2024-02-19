/**
 * @file thexpshp.cxx
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
#include "thdatabase.h"
#include "thdb2d.h"
#include "thdb2dmi.h"
#include "thlayout.h"
#include "thmap.h"
#include "thconfig.h"
#include <stdio.h>
#include "thdb1d.h"
#include "thsurvey.h"
#include "thmapstat.h"
#include <stdlib.h>
#include "shapefil.h"
#include "thexpmodel.h"
#include "thcsdata.h"
#include <stdlib.h>
#include <time.h>
#include "thcs.h"
#include <filesystem>

#include "thexpshp.h"

thexpshpf::thexpshpf(struct thexpshp * xshp, const char * fnm, int type):
  m_fnm(fnm), m_xshp(xshp), m_type(type)
{}


bool thexpshpf::open()
{
  if (this->m_is_open)
    return true;

  // set file path
  this->m_is_open = false;
  thbuffer fp;
  fp = this->m_xshp->m_dirname;
  fp += "/";
  fp += this->m_fnm;
  this->m_fpath = thdb.strstore(fp);

  this->m_hndl = SHPCreate(this->m_fpath, this->m_type);
  if (this->m_hndl == NULL)
    return false;

  this->m_is_open = true;
  return true;
}


void thexpshpf::close()
{
  if (this->m_is_open) {
    SHPClose(this->m_hndl);
    std::string dbfname(this->m_fpath);
    dbfname += ".dbf";
    int enc = TT_UTF_8;
    if (this->m_xshp != NULL) {
      if (this->m_xshp->m_expmap != NULL)
        enc = this->m_xshp->m_expmap->encoding;
      if (this->m_xshp->m_expmodel != NULL)
        enc = this->m_xshp->m_expmodel->encoding;
    }
    this->m_attributes.export_dbf(dbfname.c_str(), enc);
    if ((thcfg.outcs != TTCS_LOCAL) && (strlen(thcs_get_data(thcfg.outcs)->prjspec) > 0)) {
      FILE * prjf;
      std::string prjname(this->m_fpath);
      prjname += ".prj";
      prjf = fopen(prjname.c_str(), "w");
      fprintf(prjf, "%s", thcs_get_data(thcfg.outcs)->prjspec);
      fclose(prjf);
    }
  }
}



thexpshp::thexpshp():
  m_fscrap(this, "outline2d", SHPT_POLYGONZ),
  m_fpoints(this, "points2d", SHPT_POINTZ),
  m_flines(this, "lines2d", SHPT_ARCZ),
  m_fareas(this, "areas2d", SHPT_POLYGONZ),
  m_fstations3D(this, "stations3d", SHPT_POINTZ),
  m_fshots3D(this, "shots3d", SHPT_ARCZ),
  m_fwalls3D(this, "walls3d", SHPT_MULTIPATCH)
{}



bool thexpshp::open(const char * dirname)
{
  try {
    std::filesystem::create_directory(dirname);
    this->m_dirname = dirname;
    return true;
  } catch (const std::exception&) {
    return false;
  }
}


void thexpshp::close()
{
  this->m_fscrap.close();
  this->m_fstations3D.close();
  this->m_fshots3D.close();
  this->m_fwalls3D.close();
  this->m_fpoints.close();
  this->m_flines.close();
  this->m_fareas.close();
}



void thexpshpf::object_clear()
{
  this->m_point_list.clear();
  this->m_part_list.clear();
}


void thexpshpf::object_insert()
{
  this->m_object_id = -1;
  if (!this->open())
    return;

  size_t l = this->m_point_list.size();
  if (l == 0)
    return;

  size_t lp = this->m_part_list.size();

  int * apstart, * aptype;
  double * ax, * ay, * az, * am;
  ax = new double [l];
  ay = new double [l];
  az = new double [l];
  am = new double [l];

  size_t i;
  std::list<thexpshpf_data>::iterator ipoint;
  std::list<thexpshpf_part>::iterator ipart;

  for (i = 0, ipoint = this->m_point_list.begin();
    ipoint != this->m_point_list.end(); ipoint++, i++) 
  {
    ax[i] = ipoint->m_x;
    ay[i] = ipoint->m_y;
    az[i] = ipoint->m_z;
    am[i] = ipoint->m_m;
  }

  if (lp > 0) {
    apstart = new int [lp];
    aptype = new int [lp];
    for (i = 0, ipart = this->m_part_list.begin();
      ipart != this->m_part_list.end(); ipart++, i++) 
    {
      apstart[i] = ipart->m_start;
      aptype[i] = ipart->m_type;
    }
  } else {
    apstart = NULL;
    aptype = NULL;
  }
  
  SHPObject * obj;
  obj = SHPCreateObject( this->m_type, -1, (int) lp, apstart,
    aptype, (int) l, ax, ay, az, am);
  this->m_object_id = SHPWriteObject(this->m_hndl, -1, obj);
  if (this->m_object_id > -1)
    this->m_attributes.insert_object(nullptr, this->m_object_id);
  SHPDestroyObject(obj);

  if (lp > 0) {
    delete [] apstart;
    delete [] aptype;
  }

  delete [] ax;
  delete [] ay;
  delete [] az;
  delete [] am;

  this->m_num_objects++;
  this->object_clear();
}



void thexpshpf::polygon_start_ring(bool outer)
{
  this->m_ring_list.clear();
  this->m_ring_outer = outer;
  thexpshpf_part p;
  p.m_start = (int) this->m_point_list.size();
  if (outer)
    p.m_type = SHPP_OUTERRING;
  else
    p.m_type = SHPP_INNERRING;
  this->m_part_list.push_back(p);
}


void thexpshpf::tristrip_start()
{
  this->m_ring_list.clear();
  thexpshpf_part p;
  p.m_start = (int) this->m_point_list.size();
  p.m_type = SHPP_TRISTRIP;
  this->m_part_list.push_back(p);
}



// insert points from bezier curve
void insert_line_segment(thline * ln, bool reverse, std::list<thexpshpf_data> & lst, long startp = 0, long endp = -1)
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
					  if (std::hypot(nx - px, ny - py) > 0.1) {
			        lst.push_back(thexpshpf_data(nx + ln->fscrapptr->proj->rshift_x, ny + ln->fscrapptr->proj->rshift_y, nz + ln->fscrapptr->proj->rshift_z, na));
						  px = nx;
						  py = ny;
					  }
          }
        }
      }
    }

    // insert point it self
    if ((pnum >= startp) && ((endp < 0) || (pnum <= endp)))
      lst.push_back(thexpshpf_data(cpt->point->xt + ln->fscrapptr->proj->rshift_x, cpt->point->yt + ln->fscrapptr->proj->rshift_y, cpt->point->zt + ln->fscrapptr->proj->rshift_z, cpt->point->at));

    // next point
    prevpt = cpt;
    if (reverse)
      cpt = cpt->prevlp;
    else
      cpt = cpt->nextlp;
    pnum++;

  }

}


void thexpshpf::polygon_insert_line(thline * ln, bool reverse)
{
  insert_line_segment(ln, reverse, this->m_ring_list);
}


void thexpshpf::point_insert(double x, double y, double z, double m)
{
  this->m_point_list.push_back(thexpshpf_data(x,y,z,m));
}

void thexpshpf::polygon_close_ring()
{

  // check polygon orientation
  double area;
  std::list<thexpshpf_data>::iterator i, iprev;

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

  bool reverse = ((area > 0) && m_ring_outer) || ((area < 0) && (!m_ring_outer));

  // according to orientation, insert points to point list
  if (reverse) {
    for(i = this->m_ring_list.end(); i != this->m_ring_list.begin(); ) {
      i--;
      this->m_point_list.push_back(*i);
    } 
  } else {
    for(i = this->m_ring_list.begin(); i != this->m_ring_list.end(); i++) {
      this->m_point_list.push_back(*i);
    } 
  }
}



void thexpshp::xscrap2d(thscrap * scrap, thdb2dxm * xmap, thdb2dxs * /*xbasic*/) // TODO unused parameter xbasic
{
	
	thbuffer stnbuff;

  // export scrap outline
  this->m_fscrap.object_clear();
  thscraplo * lo = scrap->get_outline(), * lo2;
  while (lo != NULL) {
    if (lo->line->outline != TT_LINE_OUTLINE_NONE) {
      lo2 = lo;
      this->m_fscrap.polygon_start_ring(lo->line->outline == TT_LINE_OUTLINE_OUT);
      while (lo2 != NULL) {
        this->m_fscrap.polygon_insert_line(lo2->line, lo2->mode == TT_OUTLINE_REVERSED);
        lo2 = lo2->next_line;
      }
      this->m_fscrap.polygon_close_ring();
    }    
    lo = lo->next_outline;
  }
  this->m_fscrap.object_insert();
  if (this->m_fscrap.m_object_id > -1) {
    // system attributes
    this->m_fscrap.m_attributes.insert_attribute("_ID", (long) scrap->id);
    this->m_fscrap.m_attributes.insert_attribute("_NAME", scrap->name);
    this->m_fscrap.m_attributes.insert_attribute("_TITLE", scrap->title);
    this->m_fscrap.m_attributes.insert_attribute("_MAP_ID", (long) xmap->map->id);
    this->m_fscrap.m_attributes.insert_attribute("_MAP_LEVEL", (long) xmap->output_number);
    this->m_fscrap.m_attributes.insert_attribute("_MAP_TITLE", xmap->map->title);
    this->m_fscrap.m_attributes.insert_attribute("_SURVEY", scrap->fsptr->get_full_name());
    if (this->m_xproj->type == TT_2DPROJ_PLAN)
      this->m_fscrap.m_attributes.insert_attribute("_Z", scrap->z + this->m_xproj->shift_z);

    // user defined attributes
    this->m_fscrap.m_attributes.copy_attributes(thdb.attr.get_object(scrap->id));
  }


	// export centerline
  thscraplp * slp;
  slp = scrap->get_polygon();
  while (slp != NULL) {
    if (slp->lnio) {
      this->m_flines.point_insert(slp->lnx1 + scrap->proj->rshift_x, slp->lny1 + scrap->proj->rshift_y, slp->lnz1 + scrap->proj->rshift_z);
      this->m_flines.point_insert(slp->lnx2 + scrap->proj->rshift_x, slp->lny2 + scrap->proj->rshift_y, slp->lnz2 + scrap->proj->rshift_z);
      this->m_flines.object_insert();
	  this->m_flines.m_attributes.insert_attribute("_SCRAP_ID",(long) scrap->id);
      this->m_flines.m_attributes.insert_attribute("_TYPE","centerline");
    } else {
      this->m_fpoints.point_insert(slp->stx + scrap->proj->rshift_x,  slp->sty + scrap->proj->rshift_y,  slp->stz + scrap->proj->rshift_z);
      this->m_fpoints.object_insert();
	  this->m_fpoints.m_attributes.insert_attribute("_SCRAP_ID",(long) scrap->id);
	  this->m_fpoints.m_attributes.insert_attribute("_TYPE","station");
      this->m_fpoints.m_attributes.insert_attribute("_NAME",slp->station->name);
      this->m_fpoints.m_attributes.insert_attribute("_COMMENT",slp->station->comment);
		}
    slp = slp->next_item;
  }
	
	// export objects
  th2ddataobject * obj;
	thpoint * ppt;
	thline * pln;
  tharea * parea;
  obj = scrap->ls2doptr;
  long sp, cp, symid;
  int csubtype;
  thdb2dlp * lp;
  char typefc[2];
  typefc[1] = 0;
  std::string tststr, tstr;
  const char * ststr;


  while (obj != NULL) {
		switch (obj->get_class_id()) {
			case TT_POINT_CMD:
				ppt = dynamic_cast<thpoint*>(obj);
        this->m_fpoints.point_insert(ppt->point->xt + scrap->proj->rshift_x,  ppt->point->yt + scrap->proj->rshift_y, ppt->point->zt + scrap->proj->rshift_z, ppt->point->at);
        this->m_fpoints.object_insert();
		this->m_fpoints.m_attributes.insert_attribute("_SCRAP_ID",(long) ppt->fscrapptr->id);
        this->m_fpoints.m_attributes.insert_attribute("_TYPE",thmatch_string(ppt->type, thtt_point_types));
        this->m_fpoints.m_attributes.insert_attribute("_CLIP",
            ((obj->tags & TT_2DOBJ_TAG_CLIP_AUTO) ? "auto" : ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) ? "on" : "off")));
		tstr = thmatch_string(ppt->type, thtt_point_types);
		tststr = tstr;
		ststr = thmatch_string(ppt->subtype, thtt_point_subtypes);
		if ((ppt->type != TT_POINT_TYPE_U) && (ststr != NULL)) {
			tststr += ":";
			tststr += ststr;
		}
		symid = thsymbolset_get_id("point", tststr.c_str());
		if (symid < 0)
			symid = thsymbolset_get_id("point", tstr.c_str());
		if (symid < 0)
			symid = 0;
		typefc[0] = thsymbolset_fontchar[symid];
		this->m_fpoints.m_attributes.insert_attribute("_TYPEFC",(const char *)typefc);
		this->m_fpoints.m_attributes.insert_attribute("_TYPEFCR", thisnan(ppt->orient) ? 0.0 : 360.0 - ppt->orient);
        this->m_fpoints.m_attributes.insert_attribute("_SUBTYPE", ppt->type != TT_POINT_TYPE_U ?
              (thmatch_string(ppt->subtype, thtt_point_subtypes)) : ppt->m_subtype_str);
        this->m_fpoints.m_attributes.copy_attributes(thdb.attr.get_object(ppt->id));
        switch (ppt->type) {
          case TT_POINT_TYPE_LABEL:
          case TT_POINT_TYPE_REMARK:
            if ((ppt->text != NULL) && (strlen(ppt->text) > 0))
              this->m_fpoints.m_attributes.insert_attribute("_TEXT",ppt->text);
            break;
          case TT_POINT_TYPE_STATION:
            if (ppt->station_name.id > 0) {
              thdb1ds * st = &(thdb.db1d.station_vec[ppt->station_name.id - 1]);
              this->m_fpoints.m_attributes.insert_attribute("_STNAME",st->name);
              this->m_fpoints.m_attributes.insert_attribute("_STSURVEY",st->survey->get_full_name());
            }
            break;
        }
				break;
		  case TT_LINE_CMD:
				pln = dynamic_cast<thline*>(obj);
        sp = 0;
        cp = 1;
  			lp = pln->first_point;
        if (lp == NULL)
          break;
        csubtype = lp->subtype;
        lp = lp->nextlp;
        while (lp != NULL) {
          if ((lp->subtype != csubtype) || (lp->nextlp == NULL)) {
            insert_line_segment(pln, false, this->m_flines.m_point_list, sp, cp);
            this->m_flines.object_insert();
			this->m_flines.m_attributes.insert_attribute("_SCRAP_ID",(long) pln->fscrapptr->id);
            this->m_flines.m_attributes.insert_attribute("_TYPE",thmatch_string(pln->type, thtt_line_types));
            this->m_flines.m_attributes.insert_attribute("_CLIP",
                ((obj->tags & TT_2DOBJ_TAG_CLIP_AUTO) ? "auto" : ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) ? "on" : "off")));
            this->m_flines.m_attributes.insert_attribute("_SUBTYPE", pln->type != TT_LINE_TYPE_U ?
              (thmatch_string(csubtype, thtt_line_subtypes)) : pln->m_subtype_str);
            this->m_flines.m_attributes.copy_attributes(thdb.attr.get_object(pln->id));
            switch (pln->type) {
              case TT_LINE_TYPE_LABEL:
                if ((pln->text != NULL) && (strlen(pln->text) > 0))
                  this->m_flines.m_attributes.insert_attribute("_TEXT",pln->text);
                break;
            }
            csubtype = lp->subtype; 
            sp = cp;
          }
          lp = lp->nextlp;
          cp++;
  			}
				break;
		  case TT_AREA_CMD:
				parea = dynamic_cast<tharea*>(obj);
        if (parea->m_outline_line == NULL)
          break;
        this->m_fareas.object_clear();
        this->m_fareas.polygon_start_ring(true);
        this->m_fareas.polygon_insert_line(parea->m_outline_line.get(), false);
        this->m_fareas.polygon_close_ring();
        this->m_fareas.object_insert();
        if (this->m_fareas.m_object_id > -1) {
          // system attributes
          this->m_fareas.m_attributes.insert_attribute("_SCRAP_ID",(long) parea->fscrapptr->id);
          this->m_fareas.m_attributes.insert_attribute("_TYPE",thmatch_string(parea->type, thtt_area_types));
          this->m_fareas.m_attributes.insert_attribute("_CLIP",
              ((obj->tags & TT_2DOBJ_TAG_CLIP_AUTO) ? "auto" : ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) ? "on" : "off")));
          this->m_fareas.m_attributes.insert_attribute("_SUBTYPE",parea->m_subtype_str);
          this->m_fareas.m_attributes.copy_attributes(thdb.attr.get_object(parea->id));
        }
        break;
    }
    obj = obj->pscrapoptr;
  }
	
}


void thexpmap::export_shp(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (maps == NULL) {
    thwarning(("%s [%lu] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  thdb.db2d.process_areas_in_projection(prj);

  const char * fnm = this->get_output("cave.shp");
  thexpshp xs;
  xs.m_xproj = prj;
  xs.m_expmap = this;
  if (!xs.open(fnm)) {
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
            xs.xscrap2d(dynamic_cast<thscrap*>(cmi->object), cmap, cbm);
          cmi = cmi->prev_item;  
        }
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  xs.close();
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}



void thexpmodel::export_shp_file(class thdatabase * dbp)
{

  const char * fnm = this->get_output("cave.shp");
  thexpshp xs;
  xs.m_expmodel = this;
  if (!xs.open(fnm)) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  // stations
  thdb1ds * st;
  size_t nst = dbp->db1d.station_vec.size(), i;
  for(i = 0; i < nst; i++) {
    st = &(dbp->db1d.station_vec[i]);
    xs.m_fstations3D.point_insert(st->x, st->y, st->z);
    xs.m_fstations3D.object_insert();
    xs.m_fstations3D.m_attributes.insert_attribute("_ID", (long) (i+1));
    xs.m_fstations3D.m_attributes.insert_attribute("_UID", (long) st->uid);
    xs.m_fstations3D.m_attributes.insert_attribute("_NAME", st->name);
    xs.m_fstations3D.m_attributes.insert_attribute("_SURVEY", st->survey->get_full_name());
    xs.m_fstations3D.m_attributes.insert_attribute("_SURFACE", long(((st->flags & TT_STATIONFLAG_UNDERGROUND) == 0) ? 1 : 0));
    xs.m_fstations3D.m_attributes.insert_attribute("_FIXED", long(((st->flags & TT_STATIONFLAG_FIXED) != 0) ? 1 : 0));
    xs.m_fstations3D.m_attributes.insert_attribute("_ENTRANCE", long(((st->flags & TT_STATIONFLAG_ENTRANCE) != 0) ? 1 : 0));
    xs.m_fstations3D.m_attributes.insert_attribute("_CONTINUATION", long(((st->flags & TT_STATIONFLAG_CONT) != 0) ? 1 : 0));
    if ((st->comment != NULL) && (strlen(st->comment) > 0))
      xs.m_fstations3D.m_attributes.insert_attribute("_COMMENT", st->comment);
    xs.m_fstations3D.m_attributes.copy_attributes(dbp->db1d.m_station_attr.get_object(i+1));
  }

  // shots
  thdb1dl * lg;
  thdb1ds * st2;
  // stations
  size_t nleg = dbp->db1d.leg_vec.size();
  for(i = 0; i < nleg; i++) {
    lg = &(dbp->db1d.leg_vec[i]);
    st = &(dbp->db1d.station_vec[lg->leg->from.id - 1]);
    st2 = &(dbp->db1d.station_vec[lg->leg->to.id - 1]);
    xs.m_fshots3D.point_insert(st->x, st->y, st->z);
    xs.m_fshots3D.point_insert(st2->x, st2->y, st2->z);
    xs.m_fshots3D.object_insert();
    xs.m_fshots3D.m_attributes.insert_attribute("_FROM", (long) lg->leg->from.id);
    xs.m_fshots3D.m_attributes.insert_attribute("_TO", (long) lg->leg->to.id);
    xs.m_fshots3D.m_attributes.insert_attribute("_SURVEY", lg->leg->psurvey->get_full_name());
    xs.m_fshots3D.m_attributes.insert_attribute("_SURFACE", long(((lg->leg->flags & TT_LEGFLAG_SURFACE) != 0) ? 1 : 0));
    xs.m_fshots3D.m_attributes.insert_attribute("_DUPLICATE", long(((lg->leg->flags & TT_LEGFLAG_DUPLICATE) != 0) ? 1 : 0));
    xs.m_fshots3D.m_attributes.insert_attribute("_APPROXIMATE", long(((lg->leg->flags & TT_LEGFLAG_APPROXIMATE) != 0) ? 1 : 0));
    xs.m_fshots3D.m_attributes.insert_attribute("_SPLAY", long(((lg->leg->flags & TT_LEGFLAG_SPLAY) != 0) ? 1 : 0));
  }


  // walls
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
            thdb3dfc * fcp;
            thdb3dfx * fxp;
            for(i = 0, fcp = d3d->firstfc; fcp != NULL; fcp = fcp->next, i++) {
              switch (fcp->type) {
                case THDB3DFC_TRIANGLE_STRIP:
                  xs.m_fwalls3D.tristrip_start();
                  for(fxp = fcp->firstfx; fxp->next != NULL; fxp = fxp->next) {
                    xs.m_fwalls3D.point_insert(fxp->vertex->x,fxp->vertex->y,fxp->vertex->z);
                  }
                  break;
                case THDB3DFC_TRIANGLES:
                  for(fxp = fcp->firstfx; fxp != NULL; fxp = fxp->next->next->next) {
                    xs.m_fwalls3D.tristrip_start();
                    xs.m_fwalls3D.point_insert(fxp->vertex->x,fxp->vertex->y,fxp->vertex->z);
                    xs.m_fwalls3D.point_insert(fxp->next->vertex->x,fxp->next->vertex->y,fxp->next->vertex->z);
                    xs.m_fwalls3D.point_insert(fxp->next->next->vertex->x,fxp->next->next->vertex->y,fxp->next->next->vertex->z);
                  }
                  break;
              }
            }
            xs.m_fwalls3D.object_insert();
            xs.m_fwalls3D.m_attributes.insert_attribute("_ID",(long)cs->id);
            xs.m_fwalls3D.m_attributes.insert_attribute("_NAME",cs->name);
            xs.m_fwalls3D.m_attributes.insert_attribute("_SURVEY",cs->fsptr->get_full_name());
          }
        }
        cs = cs->proj_next_scrap;
      }
    }    
  } // WALLS  

  xs.close();
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}



