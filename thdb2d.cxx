/**
 * @file thdb2d.cxx
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
 
#include "thdb2d.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thparse.h"
#include "thtfangle.h"
#include "tharea.h"
#include "thmap.h"
#include "thjoin.h"
#include "thpoint.h"
#include "thline.h"
#include "thscrap.h"
#include "thsurvey.h"
#include "thlogfile.h"
#include <math.h>
#include "thlayout.h"
#include "thexpmap.h"
#include "thconfig.h"
#include "thtrans.h"
#include "thtmpdir.h"
#include "thinit.h"
#include <list>
#include <cstdio>
#ifndef THMSVC
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#define putenv _putenv
#define hypot _hypot
#endif

#ifdef THMSVC
#define hypot _hypot
#endif

class thprjx_link {

  public:
  
  thdb2dcp * from_cp, * to_cp;
  
  class thprjx_scrap * from_scrap, * to_scrap;
  
  thprjx_link * next_link, * prev_link, * oposite_link;
  
  thprjx_link() : from_cp(NULL), to_cp(NULL), from_scrap(NULL), 
      to_scrap(NULL), next_link(NULL), prev_link(NULL), oposite_link(NULL) {}
  
};


class thprjx_scrap {
  
  public:
  
  bool is_attached;
  
  thscrap * scrap;
  
  thprjx_scrap * prev_scrap;
  
  thprjx_link * via_link;
  
  double viacpx, viacpy, viacpz;
  
  thprjx_link * first_link, * last_link;
  
  thprjx_scrap() : is_attached(false), scrap(NULL), prev_scrap(NULL), 
      via_link(NULL), first_link(NULL), last_link(NULL) {}
};


class thprjx_station {

  public:
  
  unsigned long nstations;
  
  class thprjx_station_link * first_link, * last_link;
  
  thprjx_station() : nstations(0), first_link(NULL), last_link(NULL) {}
  
};


class thprjx_station_link {

  public:
  
  thdb2dcp * scrapcp;
  
  thprjx_scrap * scrap;
  
  thprjx_station_link * next_link;
  
  thprjx_station_link() : scrapcp(NULL), scrap(NULL), next_link(NULL) {}
};


bool operator < (const struct thdb2d_udef & u1, const struct thdb2d_udef & u2)
{
  if (u1.m_command < u2.m_command) return true;
  if (u1.m_command > u2.m_command) return false;
  return (strcmp(u1.m_type, u2.m_type) < 0);
}

bool operator == (const struct thdb2d_udef & u1, const struct thdb2d_udef & u2)
{
  if ((u1.m_command == u2.m_command) && (strcmp(u1.m_type, u2.m_type) == 0))
    return true;
  else
    return false;
}





thdb2d::thdb2d()
{
  this->db = NULL;
  this->processed_area_outlines = false;
  this->prj_lid = 0;
  thdb2dprjpr dpr = this->parse_projection("plan");
  this->prj_default = dpr.prj; 
  this->m_udef_map.clear();
}


thdb2d::~thdb2d()
{
}


void thdb2d::assigndb(thdatabase * dbptr)
{
  this->db = dbptr;
}

void thdb2d_rot_align(int & align, double rot) {

  double o;

  switch (align) {
    case TT_POINT_ALIGN_T:
      o = 0.0;
      break;
    case TT_POINT_ALIGN_B:
      o = 180.0;
      break;
    case TT_POINT_ALIGN_L:
      o = 270.0;
      break;
    case TT_POINT_ALIGN_R:
      o = 90.0;
      break;
    case TT_POINT_ALIGN_TL:
      o = 315.0;
      break;
    case TT_POINT_ALIGN_BL:
      o = 225.0;
      break;
    case TT_POINT_ALIGN_TR:
      o = 45.0;
      break;
    case TT_POINT_ALIGN_BR:
      o = 135.0;
      break;
    default:
      return;
  }

  o += rot;
  if (o < 0.0) {
    o += 360.0;
  } else if (o >= 360.0) {
    o -= 360.0;
  }
  
  if ((o >= 337.5) || (o <= 22.5)) 
    align = TT_POINT_ALIGN_T;
  else if ((o >= 67.5) && (o <= 112.5)) 
    align = TT_POINT_ALIGN_R;
  else if ((o >= 157.5) && (o <= 202.5)) 
    align = TT_POINT_ALIGN_B;
  else if ((o >= 247.5) && (o <= 292.5)) 
    align = TT_POINT_ALIGN_L;
  else if ((o > 22.5) && (o < 67.5)) 
    align = TT_POINT_ALIGN_TR;
  else if ((o > 112.5) && (o < 157.5)) 
    align = TT_POINT_ALIGN_BR;
  else if ((o > 202.5) && (o < 247.5)) 
    align = TT_POINT_ALIGN_BL;
  else if ((o > 292.5) && (o < 337.5)) 
    align = TT_POINT_ALIGN_TL;    
    
}


void thdb2d_flip_align(int & align, bool horiz) {
  
  bool vert = ! horiz;

  switch (align) {
    case TT_POINT_ALIGN_T:
      if (vert) align = TT_POINT_ALIGN_B;
      break;
    case TT_POINT_ALIGN_B:
      if (vert) align = TT_POINT_ALIGN_T;
      break;
    case TT_POINT_ALIGN_L:
      if (horiz) align = TT_POINT_ALIGN_R;
      break;
    case TT_POINT_ALIGN_R:
      if (horiz) align = TT_POINT_ALIGN_L;
      break;
    case TT_POINT_ALIGN_TL:
      if (vert)  align = TT_POINT_ALIGN_BL;
      if (horiz) align = TT_POINT_ALIGN_TR;
      break;
    case TT_POINT_ALIGN_BL:
      if (vert)  align = TT_POINT_ALIGN_TL;
      if (horiz) align = TT_POINT_ALIGN_BR;
      break;
    case TT_POINT_ALIGN_TR:
      if (vert)  align = TT_POINT_ALIGN_BR;
      if (horiz) align = TT_POINT_ALIGN_TL;
      break;
    case TT_POINT_ALIGN_BR:
      if (vert)  align = TT_POINT_ALIGN_TR;
      if (horiz) align = TT_POINT_ALIGN_BL;
      break;
  }
    
}


int thdb2d_rotate_align(int align, double rot) {
  thdb2d_rot_align(align, rot);
  return(align);
}


thdb2dprjpr thdb2d::parse_projection(const char * prjstr,bool insnew) 
{

  // let's split string into type - index - param - units
  thsplit_words(& this->mbf, prjstr);
  thdb2dprjpr ret_val;
  std::string index_str_str(":");
  thdb2dprj tp;
  char ** pars = this->mbf.get_buffer(), ** pars2;
  const char * type_str, * index_str = "";
  double par = tp.pp1;
  int npar = this->mbf.get_size(), prj_type;
  if (npar < 1)
    ththrow(("missing projection type"));
  thsplit_strings(& this->mbf2, pars[0], ':');
  switch (this->mbf2.get_size()) {
    case 1:
      type_str = pars[0];
      break;
    case 2:
      pars2 = this->mbf2.get_buffer();
      type_str = pars2[0];
      index_str_str += pars2[1];
      if (!th_is_keyword(pars2[1]))
        ththrow(("projection index not a keyword -- %s", pars2[1]))
      break;
    default:
      ththrow(("only one projection index allowed -- %s", pars[0]))
  } 
  prj_type = thmatch_token(type_str,thtt_2dproj);
  if (prj_type == TT_2DPROJ_UNKNOWN)
    ththrow(("unknown projection type -- %s", pars[0]));
  
  if ((prj_type == TT_2DPROJ_NONE) && (index_str_str.length() > 1))
    ththrow(("no projection index allowed -- %s",prjstr))
  
  //parse rest arguments
  thtfangle angtf;
  int asv;
  switch (prj_type) {
    case TT_2DPROJ_ELEV:
      // parse units if necessary
      if (npar > 3)
        ththrow(("too many projection arguments"))
      if (npar > 1) {
        thparse_double(asv,par,pars[1]);
        index_str_str += ":";
        index_str_str += pars[1];
        if (asv != TT_SV_NUMBER)
          ththrow(("invalid projection parameter -- %s",pars[1]))
        if (npar > 2) {
          angtf.parse_units(pars[2]);
          par = angtf.transform(par);
        }
        if ((par < 0.0) || (par >= 360.0))
          ththrow(("elevation orientation out of range -- %s", pars[1]))
      } else {
        par = 0.0;
      }
      break;
    default:
      if (npar > 1)
        ththrow(("too many projection arguments"))
      break;
  }
  
  // let's find projection in a set or create a new one
  if (index_str_str.length() > 1)
    index_str = this->db->strstore(index_str_str.c_str(), true);
  thdb2dprjid_map::iterator pi;
  thdb2dprjid tpid(prj_type, index_str);
  bool prj_found = false;
  pi = this->prjid_map.find(tpid);
  if (pi != this->prjid_map.end())
    prj_found = true;
    
  ret_val.newprj = !prj_found;
  ret_val.parok = true;
  
  thdb2dprj_list::iterator prjli;
  
  if (prj_found) {
    ret_val.prj = pi->second;
  } else {
    if (insnew) {
      this->prj_lid++;
      tp.type = prj_type;
      tp.id = this->prj_lid;
      tp.pp1 = par;
      prjli = this->prj_list.insert(this->prj_list.end(),tp);
      ret_val.prj = &(*prjli);
      this->prjid_map[tpid] = ret_val.prj;
    } else {
      ret_val.prj = NULL;
    }
  }
  return ret_val;
}


void thdb2d::self_print(FILE * outf)
{
}



thdb2dpt * thdb2d::insert_point()
{
  thdb2dpt dumm;
  return &(* this->pt_list.insert(this->pt_list.end(), dumm));
}


thdb2dlp * thdb2d::insert_line_point()
{
  thdb2dlp dumm;
  return &(* this->lp_list.insert(this->lp_list.end(),dumm));
}

thdb2dab * thdb2d::insert_border_line()
{
  thdb2dab dumm;
  return &(* this->ab_list.insert(this->ab_list.end(),dumm));
}

thdb2dji * thdb2d::insert_join_item()
{
  thdb2dji dumm;
  return &(* this->ji_list.insert(this->ji_list.end(),dumm));
}

thdb2dmi * thdb2d::insert_map_item()
{
  thdb2dmi dumm;
  return &(* this->mi_list.insert(this->mi_list.end(),dumm));
}

thdb2dcp * thdb2d::insert_control_point()
{
  thdb2dcp dumm;
  return &(* this->cp_list.insert(this->cp_list.end(),dumm));
}

thdb2dxm * thdb2d::insert_xm()
{
  thdb2dxm dumm;
  return &(* this->xm_list.insert(this->xm_list.end(),dumm));
}

thdb2dxs * thdb2d::insert_xs()
{
  thdb2dxs dumm;
  return &(* this->xs_list.insert(this->xs_list.end(),dumm));
}

thscraplo * thdb2d::insert_scraplo()
{ 
  thscraplo dumm;
  return &(* this->scraplo_list.insert(this->scraplo_list.end(),dumm));
}

thlayoutln * thdb2d::insert_layoutln()
{
  thlayoutln dumm;
  return &(* this->layoutln_list.insert(this->layoutln_list.end(),dumm));
}

thscrapen * thdb2d::insert_scrapen()
{
  thscrapen dumm;
  return &(* this->scrapen_list.insert(this->scrapen_list.end(),dumm));
}

thscraplp * thdb2d::insert_scraplp()
{
  thscraplp dumm;
  return &(* this->scraplp_list.insert(this->scraplp_list.end(),dumm));
}

void thdb2d::process_references()
{
#ifdef THDEBUG
  thprintf("\n\nprocessing references\n");
#else
  thprintf("processing references ... ");
  thtext_inline = true;
#endif

  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_LINE_CMD:
        ((thline *)(*obi))->preprocess();
        break;
      case TT_LAYOUT_CMD:
        ((thlayout *)(*obi))->process_copy();
        break;
    }
    obi++;
  }
  
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->fsptr != NULL) {
      switch ((*obi)->get_class_id()) {
        case TT_AREA_CMD:
          this->process_area_references((tharea *) *obi);
          break;
        case TT_POINT_CMD:
          this->process_point_references((thpoint *) *obi);
          break;
        case TT_MAP_CMD:
          this->process_map_references((thmap *) *obi);
          break;
        case TT_JOIN_CMD:
          this->process_join_references((thjoin *) *obi);
          break;
        case TT_SCRAP_CMD:
          this->process_scrap_references((thscrap *) *obi);
      }
    }
    obi++;
  }


  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->fsptr != NULL) {
      switch ((*obi)->get_class_id()) {
        case TT_MAP_CMD:
          this->postprocess_map_references((thmap *) *obi);
          break;
      }
    }
    obi++;
  }

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thdb2d::process_area_references(tharea * aptr)
{
  thdb2dab* cbl = aptr->first_line;
  thdataobject * optr;
  while (cbl != NULL) {  
    optr = this->db->get_object(cbl->name, aptr->fsptr);

    if (optr == NULL) {
      if (cbl->name.survey != NULL)
        ththrow(("%s [%d] -- object does not exist -- %s@%s",
          cbl->source.name, cbl->source.line,
          cbl->name.name,cbl->name.survey))
      else
        ththrow(("%s [%d] -- object does not exist -- %s",
          cbl->source.name, cbl->source.line,
          cbl->name.name))
    }

    if (optr->get_class_id() != TT_LINE_CMD) {
      if (cbl->name.survey != NULL)
        ththrow(("%s [%d] -- object is not a line -- %s@%s",
          cbl->source.name, cbl->source.line, cbl->name.name,cbl->name.survey))
      else
        ththrow(("%s [%d] -- object is not a line -- %s",
          cbl->source.name, cbl->source.line, cbl->name.name))
    }

    cbl->line = (thline *) optr;
    if (cbl->line->fscrapptr->id != aptr->fscrapptr->id) {
      if (cbl->name.survey != NULL)
        ththrow(("%s [%d] -- line outside of current scrap -- %s@%s",
          cbl->source.name, cbl->source.line, cbl->name.name,cbl->name.survey))
      else
        ththrow(("%s [%d] -- line outside of current scrap -- %s",
          cbl->source.name, cbl->source.line, cbl->name.name))
    }
    cbl = cbl->next_line;
  }
}


void thdb2d::process_map_references(thmap * mptr)
{
  if (!mptr->asoc_survey.is_empty()) {
		thdataobject * obj = this->db->get_object(mptr->asoc_survey, mptr->asoc_survey.psurvey);
		if ((obj == NULL) || (obj->get_class_id() != TT_SURVEY_CMD)) {
			if (mptr->asoc_survey.survey != NULL) 
        ththrow(("%s [%d] -- invalid survey reference -- %s@%s",
          mptr->source.name, mptr->source.line, mptr->asoc_survey.name, mptr->asoc_survey.survey))
			else
        ththrow(("%s [%d] -- invalid survey reference -- %s",
          mptr->source.name, mptr->source.line, mptr->asoc_survey.name))
		}
		mptr->asoc_survey.psurvey = (thsurvey *) obj;
	}

  if (mptr->projection_id > 0)
    return;
  if (mptr->first_item == NULL) {
    mptr->throw_source();
    threthrow2(("empty map not allowed"))
  }
  if (mptr->projection_id == -1)
    ththrow(("recursive map reference"))
  // let's lock the current map
  mptr->projection_id = -1;
  thdb2dmi * citem = mptr->first_item, * xcitem;
  thdataobject * optr;
  int proj_id = -1;
  while (citem != NULL) {
    if (citem->type != TT_MAPITEM_NORMAL) {
      citem = citem->next_item;
      continue;
    }
    optr = this->db->get_object(citem->name, citem->psurvey);
    if (optr == NULL) {
      if (citem->name.survey != NULL)
        ththrow(("%s [%d] -- object does not exist -- %s@%s",
          citem->source.name, citem->source.line, 
          citem->name.name,citem->name.survey))
      else
        ththrow(("%s [%d] -- object does not exist -- %s",
          citem->source.name, citem->source.line, 
          citem->name.name))
    }
    
    thmap * mapp;
    thscrap * scrapp;
    thsurvey * survp;

    switch (optr->get_class_id()) {
    
      case TT_SURVEY_CMD:

        if (proj_id == -1) {
          if (mptr->expl_projection == NULL)
            ththrow(("%s [%d] -- no projection for survey", citem->source.name, citem->source.line))
          proj_id = mptr->expl_projection->id;
          mptr->is_basic = false;
        } else {
          if (mptr->is_basic) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- not a scrap reference -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- not a scrap reference -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
        }

        // skontroluje ci nie sme v inej projekcii
        switch (this->get_projection(proj_id)->type) {
          case TT_2DPROJ_EXTEND:
          case TT_2DPROJ_PLAN:
          case TT_2DPROJ_ELEV:
            break;          
          default:
            ththrow(("%s [%d] -- unsupported projection for survey", citem->source.name, citem->source.line))
            break;
        }
        survp = (thsurvey*) optr;
        
        // vytvorime specialnu mapu s jednym scrapom,
        // ktoremu nastavime centerline io a survey
        scrapp = new thscrap;
        scrapp->centerline_io = true;
        scrapp->centerline_survey = survp;
        scrapp->z = thnan;
        if ((survp->stat.station_top != NULL) && (survp->stat.station_bottom != NULL)) {
          scrapp->z = (survp->stat.station_top->z + survp->stat.station_bottom->z) / 2.0;
        }
        scrapp->fsptr = NULL;
        scrapp->db = this->db;
        scrapp->proj = this->get_projection(proj_id);
        thdb.object_list.push_back(scrapp);

        mapp = new thmap;
        mapp->projection_id = proj_id;
        mapp->id = ++this->db->objid;
        mapp->db = this->db;
        mapp->z = scrapp->z;
        mapp->fsptr = NULL;
        thdb.object_list.push_back(mapp);

        xcitem = this->db->db2d.insert_map_item();
        xcitem->itm_level = mapp->last_level;
        xcitem->source = thdb.csrc;
        xcitem->psurvey = NULL;
        xcitem->type = TT_MAPITEM_NORMAL;
        xcitem->object = scrapp;
        mapp->first_item = xcitem;
        mapp->last_item = xcitem;
        citem->object = mapp;
        break;

      case TT_MAP_CMD:
        mapp = (thmap *) optr;
        // if not defined - process recursively
        if (mapp->projection_id == 0) {
          try {
            this->process_map_references(mapp);
          }
          catch (...) {
            if (citem->name.survey != NULL)
              threthrow(("%s [%d] -- error processing map reference -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              threthrow(("%s [%d] -- error processing map reference -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
        }
        if (proj_id == -1) {
          proj_id = mapp->projection_id;
          mptr->is_basic = false;
        } else {
          // check projection
          if (mapp->projection_id != proj_id) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- incompatible map projection -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- incompatible map projection -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
          // check basic
          if (mptr->is_basic) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- not a scrap reference -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- not a scrap reference -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
        }
        if ((mptr->expl_projection != NULL) && (mptr->expl_projection->id != proj_id)) {
          if (citem->name.survey != NULL)
            ththrow(("%s [%d] -- incompatible map projection -- %s@%s",
              citem->source.name, citem->source.line, 
              citem->name.name,citem->name.survey))
          else
            ththrow(("%s [%d] -- incompatible map projection -- %s",
              citem->source.name, citem->source.line, 
              citem->name.name))
        }
        citem->object = mapp;
        break;



      case TT_SCRAP_CMD:
        scrapp = (thscrap *) optr;
        if (proj_id == -1) {
          proj_id = scrapp->proj->id;
          mptr->is_basic = true;
        } else {
          // check projection
          if (scrapp->proj->id != proj_id) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- incompatible scrap projection -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- incompatible scrap projection -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
          // check basic
          if (!mptr->is_basic) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- not a map reference -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- not a map reference -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
          if (citem->m_shift.is_active()) {
            if (citem->name.survey != NULL)
              ththrow(("%s [%d] -- shift is not allowed for scrap -- %s@%s",
                citem->source.name, citem->source.line, 
                citem->name.name,citem->name.survey))
            else
              ththrow(("%s [%d] -- shift is not allowed for scrap -- %s",
                citem->source.name, citem->source.line, 
                citem->name.name))
          }
        }
        if ((mptr->expl_projection != NULL) && (mptr->expl_projection->id != proj_id)) {
          if (citem->name.survey != NULL)
            ththrow(("%s [%d] -- incompatible scrap projection -- %s@%s",
              citem->source.name, citem->source.line, 
              citem->name.name,citem->name.survey))
          else
            ththrow(("%s [%d] -- incompatible scrap projection -- %s",
              citem->source.name, citem->source.line, 
              citem->name.name))
        }
        citem->object = scrapp;
        break;



      default:
        mptr->throw_source();
        if (citem->name.survey != NULL)
          ththrow(("%s [%d] -- invalid map object -- %s@%s",
            citem->source.name, citem->source.line, 
            citem->name.name,citem->name.survey))
        else
          ththrow(("%s [%d] -- invalid map object -- %s",
            citem->source.name, citem->source.line, 
            citem->name.name))
    }
    citem = citem->next_item;
  }
#ifdef THDEBUG
  thprintf("\nmap projection %s -> %d\n",mptr->name,proj_id);
#endif 
  mptr->projection_id = proj_id;
}


void thdb2d::postprocess_map_references(thmap * mptr)
{
  thdb2dmi * citem = mptr->first_item;
  thdataobject * optr;
  while (citem != NULL) {
    if (citem->type == TT_MAPITEM_NORMAL) {
      citem = citem->next_item;
      continue;
    }
    optr = this->db->get_object(citem->name, citem->psurvey);
    if (optr == NULL) {
      if (citem->name.survey != NULL)
        ththrow(("%s [%d] -- object does not exist -- %s@%s",
          citem->source.name, citem->source.line, 
          citem->name.name,citem->name.survey))
      else
        ththrow(("%s [%d] -- object does not exist -- %s",
          citem->source.name, citem->source.line, 
          citem->name.name))
    }
    
    thmap * mapp;
    switch (optr->get_class_id()) {
      case TT_MAP_CMD:
        mapp = (thmap *) optr;
        if (mapp->projection_id != mptr->projection_id) {
          if (citem->name.survey != NULL)
            ththrow(("%s [%d] -- incompatible map projection -- %s@%s",
              citem->source.name, citem->source.line, 
              citem->name.name,citem->name.survey))
          else
            ththrow(("%s [%d] -- incompatible map projection -- %s",
              citem->source.name, citem->source.line, 
              citem->name.name))
        }
        citem->object = optr;
        break;

      case TT_SCRAP_CMD:
        if (citem->name.survey != NULL)
          ththrow(("%s [%d] -- not a map reference -- %s@%s",
            citem->source.name, citem->source.line, 
            citem->name.name,citem->name.survey))
        else
          ththrow(("%s [%d] -- not a map reference -- %s",
            citem->source.name, citem->source.line, 
            citem->name.name))
        break;



      default:
        mptr->throw_source();
        if (citem->name.survey != NULL)
          ththrow(("%s [%d] -- invalid map object -- %s@%s",
            citem->source.name, citem->source.line, 
            citem->name.name,citem->name.survey))
        else
          ththrow(("%s [%d] -- invalid map object -- %s",
            citem->source.name, citem->source.line, 
            citem->name.name))
    }
    citem = citem->next_item;
  }
}


void thdb2d::process_join_references(thjoin * jptr)
{
  thdataobject * optr;
  thdb2dji * citem = jptr->first_item, * prev_item;
  thpoint * pointp;
  thline * linep;
  thdb2dprj * cprj;
  int otype;
  while (citem != NULL) {
    optr = this->db->get_object(citem->name, jptr->fsptr);
    if (optr == NULL) {
      jptr->throw_source();
      if (citem->name.survey != NULL)
        threthrow2(("object does not exist -- %s@%s",
          citem->name.name,citem->name.survey))
      else
        threthrow2(("object does not exist -- %s",
          citem->name.name))
    }
    otype = optr->get_class_id();

    if (optr->is(TT_2DDATAOBJECT_CMD) || optr->is(TT_SCRAP_CMD)) {
      if (jptr->proj != NULL) {
        if (optr->is(TT_SCRAP_CMD))
          cprj = ((thscrap*)optr)->proj;
        else
          cprj = ((th2ddataobject *)optr)->fscrapptr->proj;
        if (cprj->id != jptr->proj->id) {        
          jptr->throw_source();
          if (citem->name.survey != NULL)
            threthrow2(("projection mishmash -- %s@%s",
              citem->name.name,citem->name.survey))
          else
            threthrow2(("projection mishmash -- %s",
              citem->name.name))
        }
      } else {
        if (optr->is(TT_SCRAP_CMD))
          jptr->proj = ((thscrap*)optr)->proj;
        else
          jptr->proj = ((th2ddataobject *)optr)->fscrapptr->proj;
      }
    }    
    
    switch (otype) {
      case TT_SCRAP_CMD:
        prev_item = citem->prev_item;
        if (prev_item != NULL) {
          if (!(prev_item->object->is(TT_SCRAP_CMD))) {
            jptr->throw_source();
            if (citem->name.survey != NULL)
              threthrow2(("scrap can not follow not scrap -- %s@%s:%s",
                citem->name.name,citem->name.survey,citem->mark))
            else
              threthrow2(("scrap can not follow not scrap  -- %s:%s",
                citem->name.name,citem->mark))
          }
          prev_item = prev_item->prev_item;
          if (prev_item != NULL) {
            jptr->throw_source();
            if (citem->name.survey != NULL)
              threthrow2(("join of more than 2 scraps not supported -- %s@%s:%s",
                citem->name.name,citem->name.survey,citem->mark))
            else
              threthrow2(("join of more than 2 scraps not supported -- %s:%s",
                citem->name.name,citem->mark))
          }
        }
        citem->object = optr;    
        break;
      case TT_POINT_CMD:
        pointp = (thpoint*) optr;
        if (citem->mark != NULL) {
          jptr->throw_source();
          if (citem->name.survey != NULL)
            threthrow2(("mark reference with point -- %s@%s:%s",
              citem->name.name,citem->name.survey,citem->mark))
          else
            threthrow2(("mark reference with point -- %s:%s",
              citem->name.name,citem->mark))
        }
        citem->point = pointp->point;
        citem->line_point = NULL;
        citem->object = optr;    
        break;
      case TT_LINE_CMD:
        linep = (thline *) optr;
        if (citem->mark != NULL) {
          citem->line_point = linep->get_marked_station(citem->mark);
          if (citem->line_point == NULL) {
            jptr->throw_source();
            if (citem->name.survey != NULL)
              threthrow2(("marked station not found -- %s@%s:%s",
                citem->name.name,citem->name.survey,citem->mark))
            else
              threthrow2(("marked station not found -- %s:%s",
                citem->name.name,citem->mark))
          }
          citem->point = citem->line_point->point;
        }
        citem->object = optr;    
        break;
      default:
        jptr->throw_source();
        if (citem->name.survey != NULL)
          threthrow2(("invalid join object -- %s@%s",
            citem->name.name,citem->name.survey))
        else
          threthrow2(("invalid join object -- %s",
            citem->name.name))
    }
    citem = citem->next_item;
  }  
}


void thdb2d::process_point_references(thpoint * pp)
{
  thdataobject * optr;
  bool extend_error = false;
  const char * err_code = "invalid station or point reference";
  switch (pp->type) {
    case TT_POINT_TYPE_STATION:
    case TT_POINT_TYPE_CONTINUATION:
      if (! pp->station_name.is_empty()) {
        try {
          pp->station_name.id = this->db->db1d.get_station_id(pp->station_name,pp->fsptr);
        } catch (...) {
          pp->station_name.id = 0;
        }  
        if (pp->station_name.id == 0) {
          pp->throw_source();
          if (pp->station_name.survey != NULL)
            threthrow2(("station does not exist -- %s@%s",
              pp->station_name.name,pp->station_name.survey))
          else
            threthrow2(("station does not exist -- %s",
              pp->station_name.name))
        }
        if (pp->type == TT_POINT_TYPE_STATION)
          pp->fscrapptr->insert_adata(&(this->db->db1d.station_vec[pp->station_name.id - 1]));
      }      
      if (pp->type == TT_POINT_TYPE_CONTINUATION)
        break;
    case TT_POINT_TYPE_EXTRA:
      if (! pp->from_name.is_empty()) {
        try {
          pp->from_name.id = this->db->db1d.get_station_id(pp->from_name,pp->fsptr);
        } catch (...) {
          pp->from_name.id = 0;
        }  
        if (pp->from_name.id == 0) {
          pp->throw_source();
          if (pp->station_name.survey != NULL)
            threthrow2(("station does not exist -- %s@%s",
              pp->from_name.name,pp->from_name.survey))
          else
            threthrow2(("station does not exist -- %s",
              pp->from_name.name))
        }
      }  
      break;
      
    case TT_POINT_TYPE_SECTION:
      extend_error = false;
      err_code = "object does not exist";
      if (! pp->station_name.is_empty()) {
        optr = this->db->get_object(pp->station_name,pp->fsptr);
        if (optr != NULL) {
          if (optr->get_class_id() == TT_SCRAP_CMD) {
            if (((thscrap *)optr)->proj->type == TT_2DPROJ_NONE) {
              pp->text = (char *) optr;
            } else {
              extend_error = true;
              err_code = "not a none scrap projection";
            }
          }
          else {
            extend_error = true;
            err_code = "object not a scrap";
          }
        }
        else {
          extend_error = true;
        }
        if (extend_error) {
          pp->throw_source();
          if (pp->station_name.survey != NULL)
            threthrow2(("%s -- %s@%s",err_code,
              pp->station_name.name,pp->station_name.survey))
          else
            threthrow2(("%s -- %s",err_code,
              pp->station_name.name))
        }
      }  
      break;       
    default:
      break;
  }
}


void thdb2d::process_scrap_references(thscrap * sptr)
{
  thscraplp * lp = sptr->polygon_first;  
  while (lp != NULL) {
    if (!(lp->station_name.is_empty())) {
      try {
        lp->station_name.id = this->db->db1d.get_station_id(lp->station_name,lp->station_name.psurvey); //sptr->fsptr);
      } catch (...) {
        lp->station_name.id = 0;
      }  
      if (lp->station_name.id == 0) {
        sptr->throw_source();
        if (lp->station_name.survey != NULL)
          threthrow2(("invalid station reference -- %s@%s",
            lp->station_name.name,lp->station_name.survey))
        else
          threthrow2(("invalid station reference -- %s",
            lp->station_name.name))
      }
      
      lp->station = &(this->db->db1d.station_vec[lp->station_name.id - 1]);
      sptr->insert_adata(lp->station);
      if (lp->station->uid > 0)
          lp->ustation = &(this->db->db1d.station_vec[lp->station->uid - 1]);
        else
          lp->ustation = lp->station;
    }
    
    lp = lp->next_item;
  }
}

int comp_dist(const void * s1, const void * s2) {
  if ((*((thscrap**)s1))->maxdist > (*((thscrap**)s2))->maxdist)
    return -1;
  else if ((*((thscrap**)s1))->maxdist < (*((thscrap**)s2))->maxdist)
    return 1;
  else return 0;
}

void thdb2d::log_distortions() {
  thdb2dprj * prj;
  thdb2dprj_list::iterator prjli;
  prjli = this->prj_list.begin();
  thscrap ** ss;
  thscrap * ps;
  unsigned long ns = 0, i;
  
  while (prjli != this->prj_list.end()) {
    prj = &(*prjli);
    ns = 0;
    ps = prj->first_scrap;
    while(ps != NULL) {
      ps = ps->proj_next_scrap;
      ns++;
    }
    if ((ns > 0) && (prj->processed) && (prj->type != TT_2DPROJ_NONE)) {
      ss = new thscrap* [ns];
      ps = prj->first_scrap;
      i = 0;
      while(ps != NULL) {
        ss[i] = ps;
        ps = ps->proj_next_scrap;
        i++;
      }
      
      qsort(ss,ns,sizeof(thscrap*),comp_dist);
      thlog.printf("\n\n###################### scrap distortions #######################\n");
      thlog.printf(" PROJECTION: %s%s%s\n", 
        thmatch_string(prj->type,thtt_2dproj), 
        strlen(prj->index) > 0 ? ":" : "",
        strlen(prj->index) > 0 ? prj->index : "");
      thlog.printf(" AVERAGE  MAXIMAL  SCRAP\n");
      for(i = 0; i < ns; i++)
        thlog.printf(" %6.2f%%  %6.2f%%  %s@%s\n",ss[i]->avdist, ss[i]->maxdist, ss[i]->name, ss[i]->fsptr->full_name);
      thlog.printf("################### end of scrap distortions ###################\n");
      delete [] ss;
    }
    prjli++;
  }
}


void thdb2d::log_selection(thdb2dxm * maps, thdb2dprj * prj) {
  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;
  thscrap * cs;
  double z;
  thmap * cm;
  thmap * bm;
  thlog.printf("\n\n############### export maps & scraps selection #################\n");
  while (cmap != NULL) {
    cm = (thmap *) cmap->map;
    z = cm->z;
    if (prj->type == TT_2DPROJ_PLAN) z += prj->shift_z;
    if (strlen(cm->name) > 0)
    	thlog.printf("M %8.2f %s@%s (%s)\n", z, cm->name, cm->fsptr ? cm->fsptr->full_name : "",  cm->title ? cm->title : "");
	cbm = cmap->first_bm;
    while (cbm != NULL) {
   	  if (cbm->mode != TT_MAPITEM_NORMAL) {
		cbm = cbm->next_item;
		continue;
	  }
      bm = cbm->bm;
      z = bm->z;
      if (prj->type == TT_2DPROJ_PLAN) z += prj->shift_z;
      cmi = cbm->bm->first_item;
      if ((cm->id != bm->id) && (strlen(bm->name) > 0))
    	  thlog.printf("M %8.2f %s@%s (%s)\n", z, bm->name, bm->fsptr ? bm->fsptr->full_name : "",  bm->title ? bm->title : "");
      if (cbm->mode == TT_MAPITEM_NORMAL) while (cmi != NULL) {
        if (cmi->type == TT_MAPITEM_NORMAL) {
          cs = (thscrap *) cmi->object;
          z = cs->z;
          if (prj->type == TT_2DPROJ_PLAN) z += prj->shift_z;
          thlog.printf("S %8.2f  %s@%s (%s)\n",z, cs->name, cs->fsptr ? cs->fsptr->full_name : "", cs->title ? cs->title : "");
        }
        cmi = cmi->next_item;
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }
  thlog.printf("########## end of export maps & scraps selection ###############\n");
}



void thdb2d::process_projection(thdb2dprj * prj)
{
  if (prj->processed)
    return;
    
  // make sure that 1D tree is processed
  thdb.db1d.get_tree_size();  
  
  const char * prjstr;
  switch (prj->type) {
    case TT_2DPROJ_EXTEND:
      prjstr = "extended";
      break;
    case TT_2DPROJ_PLAN:
      prjstr = "plan";
      break;
    case TT_2DPROJ_ELEV:
      prjstr = "elevation";
      break;
    case TT_2DPROJ_NONE:
      prjstr = "none";
      break;
    default:
      prjstr = "unknown";
      break;
  }

  bool old_thtext_inline = thtext_inline;
  if (strlen(prj->index) > 0) {
#ifdef THDEBUG
    thprintf("\n\nprocessing projection %s:%s\n",prjstr,prj->index);
#else
    thprintf("%sprocessing projection %s:%s ... ",
      thtext_inline ? "\n": "", prjstr,prj->index);
    thtext_inline = true;
#endif 
  } else {
#ifdef THDEBUG
    thprintf("\n\nprocessing projection %s\n",prjstr);
#else
    thprintf("%sprocessing projection %s ... ",
      thtext_inline ? "\n": "", prjstr);
    thtext_inline = true;
#endif   
  }
    
  prj->processed = true;
  this->pp_find_scraps_and_joins(prj);
  this->pp_scale_points(prj);
  if (prj->type != TT_2DPROJ_NONE) {
    this->pp_calc_stations(prj);
    this->pp_adjust_points(prj); 
    //this->pp_shift_points(prj, true);
    this->pp_morph_points(prj);
    this->pp_calc_points_z(prj);
  }
  this->pp_process_joins(prj);
  this->pp_shift_points(prj);
  if (this->pp_process_adjustments(prj))
    this->pp_shift_points(prj);
  this->pp_smooth_lines(prj);
  this->pp_smooth_joins(prj);
  this->pp_calc_limits(prj);
  this->pp_calc_distortion(prj);
  
#ifdef THDEBUG
  // Suppress "unused variable" warning.
  (void)old_thtext_inline;
#else
  thprintf("done\n");
  thtext_inline = old_thtext_inline;
#endif 
  if (prj->amaxdist > 0.0) {
    thprintf("average distortion: %.2f%%\n", prj->amaxdist);
  }

}


void thdb2d::pp_calc_stations(thdb2dprj * prj)
{
  double minx = 0.0, miny = 0.0, minz = 0.0, maxx = 0.0, maxy = 0.0, 
    maxz = 0.0, shift_x, shift_y, shift_z;
  minx = thnan;
  
  // prejde vsetky objekty a urobi nasledovne veci
  thscrap * pps;
  thpoint * ppp;
  thdb2dcp * cp, * scancp; //, * rootcp;
	int max_mark;
//  bool has_root = false, some_attached;
  unsigned long // nattached, 
    numcps = 0, numscraps = 0;
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
//  unsigned long searchid;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_POINT_CMD) {
      ppp = (thpoint *)(*obi);
      if ((ppp->fscrapptr->proj->id == prj->id) && (ppp->type == TT_POINT_TYPE_STATION) && (ppp->station_name.id != 0)) {
        // let's add control point to given scrap
        cp = ppp->fscrapptr->insert_control_point();
        cp->point = ppp;
        cp->point->cpoint = cp;
        cp->pt = ppp->point;
        thdb1ds * st = & (this->db->db1d.station_vec[ppp->station_name.id - 1]);
        thdb1ds * uidst = & (this->db->db1d.station_vec[st->uid - 1]);
        numcps++;
        cp->st = st;
        // let's update station type
        switch (ppp->subtype) {
          case TT_POINT_SUBTYPE_FIXED:
            if (uidst->mark < TT_DATAMARK_FIXED) {
              st->mark = TT_DATAMARK_FIXED;
              uidst->mark = TT_DATAMARK_FIXED;
            }
            break;
          case TT_POINT_SUBTYPE_PAINTED:
            if (uidst->mark < TT_DATAMARK_PAINTED) {
              st->mark = TT_DATAMARK_PAINTED;
              uidst->mark = TT_DATAMARK_PAINTED;
            }
            break;
          case TT_POINT_SUBTYPE_NATURAL:
            if (uidst->mark < TT_DATAMARK_NATURAL) {
              st->mark = TT_DATAMARK_NATURAL;
              uidst->mark = TT_DATAMARK_NATURAL;
            }
            break;
					case TT_POINT_SUBTYPE_UNKNOWN:
					  max_mark = st->mark;
						if (st->mark > uidst->mark) {
							max_mark = st->mark;
						}
						switch (max_mark) {
							case TT_DATAMARK_FIXED:
							  ppp->subtype = TT_POINT_SUBTYPE_FIXED;
							  break;
							case TT_DATAMARK_PAINTED:
							  ppp->subtype = TT_POINT_SUBTYPE_PAINTED;
							  break;
							case TT_DATAMARK_NATURAL:
							  ppp->subtype = TT_POINT_SUBTYPE_NATURAL;
							  break;
						}
					  break;
        }
        // let's check projection station limits
        if (thisnan(minx)) {
          minx = st->x;
          maxx = st->x;
          miny = st->y;
          maxy = st->y;
          minz = st->z;
          maxz = st->z;
        } else {
          if (st->x < minx)
            minx = st->x;
          if (st->x > maxx)
            maxx = st->x;
          if (st->y < miny)
            miny = st->y;
          if (st->y > maxy)
            maxy = st->y;
          if (st->z < minz)
            minz = st->z;
          if (st->z > maxz)
            maxz = st->z;
        }
      }
    }
    obi++;
  }
  
  // now let's calculate projection coordinates for all points
  // first scrap by scrap
  double sina = 0.0, cosa = 0.0, dxy, mindxy = 1e99;
  thdb1d_tree_node * nodes = this->db->db1d.get_tree_nodes(), * cnode;
  thdb1d_tree_arrow * arrow, * tarrow;

  switch (prj->type) {
    case TT_2DPROJ_ELEV:
      sina = sin(prj->pp1 / 180.0 * 3.14159265358);
      cosa = cos(prj->pp1 / 180.0 * 3.14159265358);
      break;
  }

  if (thisnan(minx)) 
	  minx = 0.0;

  pps = prj->first_scrap;  
  while (pps != NULL) {
  
//    // no scraps without stations
//    if (pps->ncp < 1) {
//      pps->throw_source();
//      threthrow(("no reference station found in scrap -- %s@%s",
//        pps->name,pps->fsptr->get_full_name()))
//    }
    
    switch (prj->type) {

      case TT_2DPROJ_PLAN:
        cp = pps->fcpp;
        prj->shift_x = shift_x = minx; //(minx + maxx) / 2.0;
        prj->shift_y = shift_y = maxy; //(miny + maxy) / 2.0;
        prj->shift_z = shift_z = maxz; //(minz + maxz) / 2.0;
        prj->rshift_x = shift_x;
        prj->rshift_y = shift_y;
        prj->rshift_z = shift_z;
        while (cp != NULL) {
          cp->tx = cp->st->x - shift_x;
          cp->ty = cp->st->y - shift_y;
          cp->tz = cp->st->z - shift_z;
          cp->ta = cp->st->z;
          cp = cp->nextcp;
        }
        break;

      case TT_2DPROJ_ELEV:
        cp = pps->fcpp;
        prj->shift_x = shift_x = minx; //(minx + maxx) / 2.0;
        prj->shift_y = shift_y = maxy; //(miny + maxy) / 2.0;
        prj->shift_z = shift_z = maxz; //(minz + maxz) / 2.0;
        prj->rshift_x = shift_x;
        prj->rshift_y = shift_y;
        prj->rshift_z = shift_z;
        while (cp != NULL) {
          cp->tx = cosa * (cp->st->x - shift_x) - sina * (cp->st->y - shift_y);
          cp->tz = sina * (cp->st->x - shift_x) + cosa * (cp->st->y - shift_y);
          cp->ty = cp->st->z - shift_z;
          cp->ta = cp->st->z;
          cp = cp->nextcp;
        }
        break;

      case TT_2DPROJ_EXTEND:
        cp = pps->fcpp;
        prj->shift_x = shift_x = 0;
        prj->shift_y = shift_y = maxy; //(miny + maxy) / 2.0;
        prj->shift_z = shift_z = maxz; //(minz + maxz) / 2.0;
        prj->rshift_x = shift_x;
        prj->rshift_y = shift_y;
        prj->rshift_z = shift_z;
        while (cp != NULL) {
          cnode = &(nodes[cp->st->uid - 1]);
          cp->tx = cnode->extendx - shift_x;
          // if prev station specified - try to find this arrow
          arrow = NULL;
          if (!cp->point->from_name.is_empty()) {
            arrow = cnode->first_arrow;
            while (arrow != NULL) {
              if (arrow->end_node->uid == this->db->db1d.station_vec[cp->point->from_name.id - 1].uid)
                break;
              arrow = arrow->next_arrow;
            }
          }
          // if not find, find arrow with minimal dx where oposite
          // is valid cp
          if (arrow == NULL) {
            tarrow = cnode->first_arrow;
            while (tarrow != NULL) {
              scancp = pps->fcpp;
              while (scancp != NULL) {
                if (tarrow->end_node->uid == scancp->st->uid) {
                  if (arrow == NULL) {
                    arrow = tarrow;
                    mindxy = hypot(cp->pt->x - scancp->pt->x, cp->pt->y - scancp->pt->y);
                  } else {
                    dxy = hypot(cp->pt->x - scancp->pt->x, cp->pt->y - scancp->pt->y);
                    if (dxy < mindxy) {
                      mindxy = dxy;
                      arrow = tarrow;
                    }
                  }
                }
                scancp = scancp->nextcp;
              }
              tarrow = tarrow->next_arrow;
            }
          }          
          if (arrow != NULL) {
            cp->tx = (arrow->is_reversed ? arrow->leg->leg->txx : arrow->leg->leg->fxx) - shift_x;
          }
          cp->tz = 0.0;
          cp->ty = cp->st->z - shift_z;
          cp->ta = cp->st->z;
          cp = cp->nextcp;
        }
        break;
  
    } // END of projection switch

    pps = pps->proj_next_scrap;
    numscraps++;
  }

  // shift all points in scraps without control points
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  while (ii != this->pt_list.end()) {
    pps = ii->pscrap;
	if ((pps->proj->id == prj->id) && (pps->fcpp == NULL)) {
	  ii->xt -= prj->shift_x;
	  ii->yt -= prj->shift_y;
	  ii->zt -= prj->shift_z;
    }
    ii++;
  }
  
    
  // postprocess if necessary
//  if (prj->type == TT_2DPROJ_EXTEND) {
//    
//    std::list <thprjx_link> xscrap_links;
//    thprjx_link * xscrap_link, * xscrap_link2;
//    
//    std::list <thprjx_station_link> xstation_links;
//    thprjx_station_link * xstation_link, * xstation_link2;
//    
//    thprjx_scrap * xscraps = new thprjx_scrap [numscraps],
//      * xscrap;
//
//    unsigned long numstations = this->db->db1d.station_vec.size();
//    thprjx_station * xstations = new thprjx_station [numstations],
//      * xstation = NULL;
//      
//    thprjx_station_link dummxs;
//      
////    unsigned long cuid;
//    
//    pps = prj->first_scrap;
//    xscrap = xscraps;
//    while (pps != NULL) {
//      xscrap->scrap = pps;
//      pps->xscrap = xscrap;
//      cp = pps->fcpp;
//      while (cp != NULL) {
//        if (cp->is_sticky != TT_POINT_EXTEND_STICKYOFF) {
//          xstation = &(xstations[cp->st->uid - 1]);
//          xstation_link = &(*(xstation_links.insert(xstation_links.end(),dummxs)));
//          xstation_link->scrap = xscrap;
//          xstation_link->scrapcp = cp;
//          if (xstation->nstations == 0) {
//            xstation->first_link = xstation_link;
//            xstation->last_link = xstation_link;
//          } else {
//            xstation->last_link->next_link = xstation_link;
//            xstation->last_link = xstation_link;
//          }
//        }
//        xstation->nstations++;
//        
//        cp = cp->nextcp;
//      }      
//      pps = pps->proj_next_scrap;
//      xscrap++;
//    }
//    
//    // let's add scrap links from UID
//    unsigned long station_id;
//    thprjx_link dummxl;
//    station_id = 0;
//    xstation = xstations;
//    while (station_id < numstations) {
//      if (xstation->nstations > 1) {
//        xstation_link = xstation->first_link;
//        while (xstation_link != NULL) {
//          xstation_link2 = xstation_link->next_link;
//          while (xstation_link2 != NULL) {
//            if (xstation_link2->scrap->scrap->id != xstation_link->scrap->scrap->id) {
//            
//              // pridaj ->
//              xscrap_link = &(*(xscrap_links.insert(xscrap_links.end(),dummxl)));
//              xscrap_link->from_cp = xstation_link->scrapcp;
//              xscrap_link->from_scrap = xstation_link->scrap;
//              xscrap_link->to_cp = xstation_link2->scrapcp;
//              xscrap_link->to_scrap = xstation_link2->scrap;
//              if (xscrap_link->from_scrap->last_link == NULL) {
//                xscrap_link->from_scrap->last_link = xscrap_link;
//                xscrap_link->from_scrap->first_link = xscrap_link;
//              } else {
//                if ((xscrap_link->from_cp->is_sticky == TT_POINT_EXTEND_STICKYON) ||
//                    (xscrap_link->to_cp->is_sticky == TT_POINT_EXTEND_STICKYON)) {
//                  xscrap_link->from_scrap->first_link->prev_link = xscrap_link;
//                  xscrap_link->next_link = xscrap_link->from_scrap->first_link;                  
//                  xscrap_link->from_scrap->first_link = xscrap_link;
//                } else {
//                  xscrap_link->prev_link = xscrap_link->from_scrap->last_link;
//                  xscrap_link->from_scrap->last_link->next_link = xscrap_link;
//                  xscrap_link->from_scrap->last_link = xscrap_link;
//                }
//              }
//              
//              xscrap_link2 = xscrap_link;
//              
//              // pridaj <-
//              xscrap_link = &(*(xscrap_links.insert(xscrap_links.end(),dummxl)));
//              xscrap_link->from_cp = xstation_link2->scrapcp;
//              xscrap_link->from_scrap = xstation_link2->scrap;
//              xscrap_link->to_cp = xstation_link->scrapcp;
//              xscrap_link->to_scrap = xstation_link->scrap;
//              if (xscrap_link->from_scrap->last_link == NULL) {
//                xscrap_link->from_scrap->last_link = xscrap_link;
//                xscrap_link->from_scrap->first_link = xscrap_link;
//              } else {
//                if ((xscrap_link->from_cp->is_sticky == TT_POINT_EXTEND_STICKYON) ||
//                    (xscrap_link->to_cp->is_sticky == TT_POINT_EXTEND_STICKYON)) {
//                  xscrap_link->from_scrap->first_link->prev_link = xscrap_link;
//                  xscrap_link->next_link = xscrap_link->from_scrap->first_link;                  
//                  xscrap_link->from_scrap->first_link = xscrap_link;
//                } else {
//                  xscrap_link->prev_link = xscrap_link->from_scrap->last_link;
//                  xscrap_link->from_scrap->last_link->next_link = xscrap_link;
//                  xscrap_link->from_scrap->last_link = xscrap_link;
//                }
//              }
//              
//              xscrap_link2->oposite_link = xscrap_link;
//              xscrap_link->oposite_link = xscrap_link2;
//              
//            }
//            xstation_link2 = xstation_link2->next_link;
//          }
//          xstation_link = xstation_link->next_link;
//        }
//      }
//      station_id++;
//      xstation++;
//    }
//    
//    
//    
//    // let's run TREMAUX on scraps
//    unsigned long num_scrap_att = 0; 
//    double movex, movey;
//    xscrap = xscraps;
//    while(1) {
//
//      // 1 ak nie je xscrap pripojeny, pripoji ho
//      if (!xscrap->is_attached) {
//        if (xscrap->via_link != NULL) {
//          movex = xscrap->via_link->from_cp->tx - xscrap->via_link->to_cp->tx;
//          movey = xscrap->via_link->from_cp->ty - xscrap->via_link->to_cp->ty;
//          cp = xscrap->scrap->fcpp;
//          while (cp != NULL) {
//            cp->tx = cp->tx + movex;
//            cp->ty = cp->ty + movey;
//            cp->tz = double(num_scrap_att);
//            cp = cp->nextcp;
//          }
//        }
//        xscrap->is_attached = true;
//        num_scrap_att++;
//      }
//
//      // skontroluje ci mooze ist dalej
//      while ((xscrap->first_link != NULL) && (xscrap->first_link->to_scrap->is_attached))
//        xscrap->first_link = xscrap->first_link->next_link;
//
//      // ak nie ide spat ak mooze inak break.
//      if (xscrap->first_link != NULL) {
//        xscrap->first_link->to_scrap->via_link = xscrap->first_link;
//        xscrap = xscrap->first_link->to_scrap;
//        xscrap->via_link->from_scrap->first_link = xscrap->via_link->from_scrap->first_link->next_link;
//      } else {
//        if (xscrap->via_link != NULL)
//          xscrap = xscrap->via_link->from_scrap;
//        else
//          break;
//      }
//    }
//    if (num_scrap_att < numscraps) {
//      xscrap = xscraps;
//      unsigned nxscrap = 0;
//      thlog.printf("error info -- scraps not attached to extended");
//      if (strlen(prj->index) == 0)
//        thlog.printf(":%s",prj->index);
//      thlog.printf("projection:\n");
//      while(nxscrap < numscraps) {
//        if (!xscrap->is_attached)
//          thlog.printf("\t%s@%s\n",xscrap->scrap->name,xscrap->scrap->fsptr->get_full_name());
//        xscrap++;
//        nxscrap++;
//      }
//      if (strlen(prj->index) == 0)
//        ththrow(("can not connect all scraps in exteded projection"))
//      else
//        ththrow(("can not connect all scraps in exteded:%s projection", prj->index))      
//    }
//    
//    delete [] xscraps;
//    delete [] xstations;
//  } // END of extended projection post processing
  
}


void thdb2d::pp_scale_points(thdb2dprj * prj)
{
  thscrap * ps = prj->first_scrap;
  double maxdist,cdist,scale,ang,tang;
  
  
  // prejde scrapy a nastavy matice
  ps = prj->first_scrap;
  while (ps != NULL) {
    ps->reset_transformation();
    if (ps->scale_p9) {

        cdist = hypot(ps->scale_p2y - ps->scale_p1y,ps->scale_p2x - ps->scale_p1x);
        maxdist = hypot(ps->scale_r2y - ps->scale_r1y,ps->scale_r2x - ps->scale_r1x);

        ang = atan2(ps->scale_p2y - ps->scale_p1y,ps->scale_p2x - ps->scale_p1x);
        tang = atan2(ps->scale_r2y - ps->scale_r1y,ps->scale_r2x - ps->scale_r1x);
        tang -= ang;
        
        
        ps->mxx = cos(tang);
        ps->mxy = -sin(tang);
        ps->myx = sin(tang);
        ps->myy = cos(tang);
        
        scale = maxdist / cdist;
        ps->ms = scale;
        ps->mr = - tang / 3.14159265358 * 180.0;
        ps->mxx *= scale;
        ps->myy *= scale;
        ps->mxy *= scale;
        ps->myx *= scale;

        ps->mx = ps->scale_r1x - ps->mxx * ps->scale_p1x
                          - ps->mxy * ps->scale_p1y;
        ps->my = ps->scale_r1y - ps->myx * ps->scale_p1x
                          - ps->myy * ps->scale_p1y;

    }
    ps = ps->proj_next_scrap;
  }
  
  // podla nastavenia scale v scrape modifikuje suradnice bodov
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  while (ii != this->pt_list.end()) {
    ps = ii->pscrap;
    if (ps->proj->id == prj->id) {
      if (ps->scale_p9) {
        ii->xt = ps->mxx * ii->x + ps->mxy * ii->y + ps->mx;
        ii->yt = ps->myx * ii->x + ps->myy * ii->y + ps->my;
      } else {
        ii->xt = ii->x * ps->scale;
        ii->yt = ii->y * ps->scale;
      }

      // flipne ak treba
      if (ps->flip != TT_SCRAP_FLIP_NONE) {
        switch (ps->flip) {
          case TT_SCRAP_FLIP_VERT:
            ii->yt *= -1.0;
            break;
          case TT_SCRAP_FLIP_HORIZ:
            ii->xt *= -1.0;
            break;
        }
      }
      
      ii->dbgx0 = ii->xt;
      ii->dbgy0 = ii->yt;
      ii->dbgx1 = ii->xt;
      ii->dbgy1 = ii->yt;
      
    }
    ii++;
  }
  
  // transformuje rotacie a scales
  ps = prj->first_scrap;
  thline * pline;
  thpoint * ppoint;
  thdb2dlp * plp;
  th2ddataobject * p2dobj;
  while (ps != NULL) {
    p2dobj = ps->fs2doptr;
    while (p2dobj != NULL) {
      switch (p2dobj->get_class_id()) {
        case TT_POINT_CMD:
          ppoint = (thpoint *) p2dobj;
          if (!thisnan(ppoint->orient)) {
            ppoint->orient += ps->mr;
            if (ppoint->orient < 0) {
              ppoint->orient += 360.0;
            } else if (ppoint->orient >= 360.0) {
              ppoint->orient -= 360.0;
            }
            
            // flip if needed
            if (ps->flip != TT_SCRAP_FLIP_NONE) {
              switch (ps->flip) {
                case TT_SCRAP_FLIP_VERT:
                  ppoint->orient -= 90.0;
                  ppoint->orient = 360.0 - ppoint->orient;
                  ppoint->orient += 90.0;
                  break;
                case TT_SCRAP_FLIP_HORIZ:
                  ppoint->orient = 360.0 - ppoint->orient;
                  break;
              }
              if (ppoint->orient < 0) {
                ppoint->orient += 360.0;
              } else if (ppoint->orient >= 360.0) {
                ppoint->orient -= 360.0;
              }
            } // flip
            
          } else if (ppoint->align != TT_POINT_ALIGN_C) {
            thdb2d_rot_align(ppoint->align,ps->mr);
            if (ps->flip != TT_SCRAP_FLIP_NONE) {
              thdb2d_flip_align(ppoint->align, ps->flip == TT_SCRAP_FLIP_HORIZ);
            }
          }
          //ppoint->xsize *= ps->ms;
          //ppoint->ysize *= ps->ms;
          break;
        case TT_LINE_CMD:
          pline = (thline *) p2dobj;
          plp = pline->first_point;

          while (plp != NULL) {            
            if (!thisnan(plp->orient)) {
              plp->orient += ps->mr;
              if (plp->orient < 0) {
                plp->orient += 360.0;
              } else if (plp->orient >= 360.0) {
                plp->orient -= 360.0;
              }

              // flip if needed
              if (ps->flip != TT_SCRAP_FLIP_NONE) {
              
                switch (ps->flip) {
                  case TT_SCRAP_FLIP_VERT:
                    plp->orient -= 90.0;
                    plp->orient = 360.0 - plp->orient;
                    plp->orient += 90.0;
                    break;
                  case TT_SCRAP_FLIP_HORIZ:
                    plp->orient = 360.0 - plp->orient;
                    break;
                }
                if (plp->orient < 0) {
                  plp->orient += 360.0;
                } else if (plp->orient >= 360.0) {
                  plp->orient -= 360.0;
                }
              } // flip
              
            }
            //  plp->rsize *= ps->ms;
            //  plp->lsize *= ps->ms;
            switch (pline->type) {
              case TT_LINE_TYPE_SLOPE:
                if ((plp->tags & TT_LINEPT_TAG_SIZE) > 0)
                  plp->lsize *= ps->ms;
                break;
            }
            plp = plp->nextlp;
          }
          break;
      }
      p2dobj = p2dobj->nscrapoptr;
    }
    ps = ps->proj_next_scrap;
  }  
  
  
}


void thdb2d::pp_find_scraps_and_joins(thdb2dprj * prj)
{
  thscrap * pscrap = NULL, * cscrap;
  thjoin * pjoin = NULL, * cjoin;
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if (((*obi)->fsptr != NULL) && ((*obi)->get_class_id() == TT_SCRAP_CMD)) {
      if (((thscrap *)(*obi))->proj->id == prj->id) {
        cscrap = (thscrap *)(*obi);
        if (pscrap != NULL)
          pscrap->proj_next_scrap = cscrap;
        else {
          prj->first_scrap = cscrap;
          prj->last_scrap = cscrap;
        }
        cscrap->proj_prev_scrap = pscrap;
        prj->last_scrap = cscrap;
        pscrap = cscrap;
      }
    }
    
    if (((*obi)->get_class_id() == TT_JOIN_CMD) &&
        (((thjoin *)(*obi))->proj != NULL) &&
        (((thjoin *)(*obi))->proj->id == prj->id)) {    
      cjoin = (thjoin *)(*obi);
      if (pjoin != NULL)
        pjoin->proj_next_join = cjoin;
      else {
        prj->first_join = cjoin;
        prj->last_join = cjoin;
      }
      cjoin->proj_prev_join = pjoin;
      prj->last_join = cjoin;
      pjoin = cjoin;
    }
    
    obi++;
  }
  
}


void thdb2d::pp_calc_limits(thdb2dprj * prj)
{

  // vyrusi kazdy scrap
  thscrap * ps;
  ps = prj->first_scrap;
  while (ps != NULL) {
    ps->reset_limits();
    ps->calc_z();
    ps = ps->proj_next_scrap;
  }
  
  // prejde bod za bodom a nastavi limity pre dany scrap
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  while (ii != this->pt_list.end()) {
    if (ii->pscrap->proj->id == prj->id) {
      if (thisnan(ii->pscrap->lxmin)) {
        ii->pscrap->lxmin = ii->xt;
        ii->pscrap->lxmax = ii->xt;
        ii->pscrap->lymin = ii->yt;
        ii->pscrap->lymax = ii->yt;
      } else {
        if (ii->pscrap->lxmin > ii->xt)
          ii->pscrap->lxmin = ii->xt;
        if (ii->pscrap->lxmax < ii->xt)
          ii->pscrap->lxmax = ii->xt;
        if (ii->pscrap->lymin > ii->yt)
          ii->pscrap->lymin = ii->yt;
        if (ii->pscrap->lymax < ii->yt)
          ii->pscrap->lymax = ii->yt;
      }
    }
    ii++;
  }
  
}


void thdb2d::pp_adjust_points(thdb2dprj * prj)
{

  // prejde scrap za scrapom, zrata transf. maticu
  thscrap * pscrap;
  thdb2dcp * cp;

  // vycentruje scrapy v projekcii
  pscrap = prj->first_scrap;
  while (pscrap != NULL) {
    pscrap->mx = 0.0;
    pscrap->my = 0.0;
    pscrap->ms = 0.0;
    cp = pscrap->fcpp;
    while (cp != NULL) {
      pscrap->mx += cp->pt->xt;
      pscrap->my += cp->pt->yt;
      pscrap->ms += 1.0;
      cp = cp->nextcp;
    }
    if (pscrap->ms > 1.0) {
      pscrap->mx = pscrap->mx / pscrap->ms;
      pscrap->my = pscrap->my / pscrap->ms;
    } else {
      pscrap->mx = 0.0;
      pscrap->my = 0.0;
    }
    pscrap = pscrap->proj_next_scrap;
  }  

  thdb2dpt_list::iterator ii = this->pt_list.begin();
  while (ii != this->pt_list.end()) {
    pscrap = ii->pscrap;
    if (pscrap->proj->id == prj->id) {
      ii->xt -= pscrap->mx;
      ii->yt -= pscrap->my;
    }
    ii++;
  }  

#define THADJUSTLS
#ifndef THADJUSTLS
  thdb2dcp * cp2, * acp1, * acp2;
  double maxdist, cdist, scale, ang, tang;
#else  
  double sumXx, sumYy, sumYx, sumXy, sumxx, sumyy, movetx, movety, X, Y, a, b;
#endif

  pscrap = prj->first_scrap;
  while (pscrap != NULL) {
    pscrap->reset_transformation();
    if (pscrap->ncp == 1) {
      pscrap->mx = pscrap->fcpp->tx - pscrap->fcpp->pt->xt;
      pscrap->my = pscrap->fcpp->ty - pscrap->fcpp->pt->yt;
    } else if (pscrap->ncp > 1) {

#ifndef THADJUSTLS
      acp1 = pscrap->fcpp;
      acp2 = pscrap->fcpp->nextcp;
      maxdist = hypot(acp1->tx - acp2->tx, acp1->ty - acp2->ty);
      cp = pscrap->fcpp;
      while (cp->nextcp != NULL) {
        cp2 = cp->nextcp;
        while (cp2 != NULL) {
          cdist = hypot(cp->tx - cp2->tx, cp->ty - cp2->ty);
          if (cdist > maxdist) {
            maxdist = cdist;
            acp1 = cp;
            acp2 = cp2;
          }
          cp2 = cp2->nextcp;
        }
        cp = cp->nextcp;
      }
      cdist = hypot(acp1->pt->xt - acp2->pt->xt, acp1->pt->yt - acp2->pt->yt);
      if ((maxdist > 0) && (cdist > 0)) {
      
        ang = atan2(acp2->pt->yt - acp1->pt->yt, acp2->pt->xt - acp1->pt->xt);
        tang = atan2(acp2->ty - acp1->ty, acp2->tx - acp1->tx);
        tang -= ang;
        
        pscrap->mxx = cos(tang);
        pscrap->mxy = -sin(tang);
        pscrap->myx = sin(tang);
        pscrap->myy = cos(tang);
        
        scale = maxdist / cdist;
        pscrap->ms = scale;
        pscrap->mr = - tang / 3.14159265358 * 180.0;
        pscrap->mxx *= scale;
        pscrap->myy *= scale;
        pscrap->mxy *= scale;
        pscrap->myx *= scale;
        
        pscrap->mx = acp1->tx - pscrap->mxx * acp1->pt->xt
                              - pscrap->mxy * acp1->pt->yt;
        pscrap->my = acp1->ty - pscrap->myx * acp1->pt->xt
                              - pscrap->myy * acp1->pt->yt;                                                            
#else
      // 1. najdeme transformaciu targetov
      // thprintf("\n\n\nscap: %s\n",pscrap->name);
      movetx = 0.0;
      movety = 0.0;
      cp = pscrap->fcpp;
      while (cp != NULL) {
        movetx += cp->tx;
        movety += cp->ty;
        cp = cp->nextcp;
      }
      movetx = movetx / double(pscrap->ncp);
      movety = movety / double(pscrap->ncp);
      sumXx = 0.0; sumYy = 0.0; 
      sumYx = 0.0; sumXy = 0.0;
      sumxx = 0.0; sumyy = 0.0;
      cp = pscrap->fcpp;
      while (cp != NULL) {
        X = cp->tx - movetx;
        Y = cp->ty - movety;
        // thprintf("%.4f,%.4f -> %.4f,%.4f\n",X,Y,cp->pt->xt,cp->pt->yt);
        sumXx += X * cp->pt->xt;
        sumYy += Y * cp->pt->yt;
        sumXy += X * cp->pt->yt;
        sumYx += Y * cp->pt->xt;
        sumxx += cp->pt->xt * cp->pt->xt;
        sumyy += cp->pt->yt * cp->pt->yt;
        cp = cp->nextcp;
      }

      a = (sumXx + sumYy) / (sumxx + sumyy);
      b = (sumYx - sumXy) / (sumxx + sumyy);
        
      if (((sumxx + sumyy) > 0) && (hypot(a,b) > 0.0)) {
        // thprintf("a = %.4f;\nb = %.4f\n",a,b);
        pscrap->mxx = a;
        pscrap->mxy = -b;
        pscrap->myx = b;
        pscrap->myy = a;
        pscrap->mx = movetx;
        pscrap->my = movety;
        pscrap->ms = hypot(a,b);
        pscrap->mr = - atan2(b,a) / 3.14159265358 * 180.0;
        
#endif
      } else {
        pscrap->mx = pscrap->fcpp->tx - pscrap->fcpp->pt->xt;
        pscrap->my = pscrap->fcpp->ty - pscrap->fcpp->pt->yt;
      }
    }
    pscrap = pscrap->proj_next_scrap;
  }

  
  // prejde bod za bodom a spocita ich suradnice
  ii = this->pt_list.begin();
  double tmpx, tmpy;
  while (ii != this->pt_list.end()) {
    if (ii->pscrap->proj->id == prj->id) {
      pscrap = ii->pscrap;
      tmpx = ii->xt;
      tmpy = ii->yt;
      ii->xt = pscrap->mxx * tmpx + pscrap->mxy * tmpy + pscrap->mx;
      ii->yt = pscrap->myx * tmpx + pscrap->myy * tmpy + pscrap->my;
      ii->dbgx0 = ii->xt;
      ii->dbgy0 = ii->yt;
    }
    ii++;
  }

  // transformuje rotacie a scales
  pscrap = prj->first_scrap;
  thline * pline;
  thpoint * ppoint;
  thdb2dlp * plp;
  th2ddataobject * p2dobj;
  thdb1ds * neas;
  while (pscrap != NULL) {
    p2dobj = pscrap->fs2doptr;
    while (p2dobj != NULL) {
      switch (p2dobj->get_class_id()) {
        case TT_POINT_CMD:
          ppoint = (thpoint *) p2dobj;
          if (!thisnan(ppoint->orient)) {
            ppoint->orient += pscrap->mr;
            if (ppoint->orient < 0) {
              ppoint->orient += 360.0;
            } else if (ppoint->orient >= 360.0) {
              ppoint->orient -= 360.0;
            }
          } else if (ppoint->align != TT_POINT_ALIGN_C)
            thdb2d_rot_align(ppoint->align,pscrap->mr);
          //  ppoint->xsize *= pscrap->ms;
          //  ppoint->ysize *= pscrap->ms;
          switch (ppoint->type) {
            case TT_POINT_TYPE_STATION_NAME:
              if (ppoint->text == NULL) {
                neas = pscrap->get_nearest_station(ppoint->point);
                if (neas == NULL) {
                  ppoint->throw_source();
                  threthrow2(("unable to determine station name"))
                } else {
                  ppoint->station_name.name = neas->name;
                  ppoint->station_name.psurvey = neas->survey;
                  ppoint->text = neas->name;
                }
              }
              break;
            case TT_POINT_TYPE_ALTITUDE:
              if (ppoint->ysize > 0.0) {
                neas = pscrap->get_nearest_station(ppoint->point);
                if (neas == NULL) {
                  ppoint->throw_source();
                  threthrow2(("unable to determine altitude"))
                } else {
                  ppoint->xsize += neas->z;
                  ppoint->ysize = 0.0;
                }
              }
              break;
            default:
              break;
          }
          break;
        case TT_LINE_CMD:
          pline = (thline *) p2dobj;
          plp = pline->first_point;
          while (plp != NULL) {            
            if (!thisnan(plp->orient)) {
              plp->orient += pscrap->mr;
              if (plp->orient < 0) {
                plp->orient += 360.0;
              } else if (plp->orient >= 360.0) {
                plp->orient -= 360.0;
              }
            }
            
            //  plp->rsize *= pscrap->ms;
            //  plp->lsize *= pscrap->ms;            
            switch (pline->type) {
              case TT_LINE_TYPE_SLOPE:
                if ((plp->tags & TT_LINEPT_TAG_SIZE) > 0)
                  plp->lsize *= pscrap->ms;
                break;
              case TT_LINE_TYPE_WALL:
                if (((plp->tags & TT_LINEPT_TAG_ALTITUDE) > 0) &&
                  (plp->lsize > 0.0)) {
                  neas = pscrap->get_nearest_station(plp->point);
                  if (neas == NULL) {
                    pline->throw_source();
                    threthrow2(("unable to determine altitude"))
                  } else {
                    plp->rsize += neas->z;
                    plp->lsize = 0.0;
                  }
                }
                break;
            }
            plp = plp->nextlp;
          }
          break;
      }
      p2dobj = p2dobj->nscrapoptr;
    }
    pscrap = pscrap->proj_next_scrap;
  }
  
}



void thdb2d::pp_calc_points_z(thdb2dprj * prj)
{
  // let's calculate drifts
  // prejde scrap za scrapom
  thscrap * pscrap = prj->first_scrap;
  thdb2dcp * cp;
  while (pscrap != NULL) {
    cp = pscrap->fcpp;
    while (cp != NULL) {
      cp->dx = cp->tx - cp->pt->xt;
      cp->dy = cp->ty - cp->pt->yt;
      cp->oxt = cp->pt->xt;
      cp->oyt = cp->pt->yt;
      cp = cp->nextcp;
    }
    pscrap = pscrap->proj_next_scrap;
  }
  
  // prejde bod za bodom a spocita ich suradnice
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  double cpw, totalw, att, ztt, dist;
  while (ii != this->pt_list.end()) {
    if ((ii->pscrap->proj->id == prj->id) && (ii->pscrap->ncp > 0)) {
      cp = ii->pscrap->fcpp;
      totalw = 0.0;
      att = 0.0;
      ztt = 0.0;
      while (cp != NULL) {
        dist = hypot(cp->oxt - ii->xt, cp->oyt - ii->yt);
        if (dist > 0) {
          cpw = 1.0 / (dist * dist);
          att += cp->ta * cpw;
          ztt += cp->tz * cpw;
          totalw += cpw;
        } else {
          att = cp->ta;
          ztt = cp->tz;
          totalw = 1.0;
          break;
        }
        cp = cp->nextcp;
      }
      if (totalw > 0) {
        ii->at = att / totalw;
        ii->zt = ztt / totalw;
      }
    }
    ii++;
  }
    
}



void thdb2d::pp_shift_points(thdb2dprj * prj)
{
  // let's calculate drifts
  // prejde scrap za scrapom
  thscrap * pscrap = prj->first_scrap;
  thdb2dcp * cp;
  while (pscrap != NULL) {
    cp = pscrap->fcpp;
    while (cp != NULL) {
      cp->dx = cp->tx - cp->pt->xt;
      cp->dy = cp->ty - cp->pt->yt;
      cp->oxt = cp->pt->xt;
      cp->oyt = cp->pt->yt;
      cp = cp->nextcp;
    }
    pscrap = pscrap->proj_next_scrap;
  }
  
  // prejde bod za bodom a spocita ich suradnice
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  double cpw, totalw, cdx, cdy, dist;
  while (ii != this->pt_list.end()) {
    if ((ii->pscrap->proj->id == prj->id) && (ii->pscrap->ncp > 0)) {
      cp = ii->pscrap->fcpp;
      totalw = 0.0;
      cdx = 0.0;
      cdy = 0.0;
      while (cp != NULL) {
        dist = hypot(cp->oxt - ii->xt, cp->oyt - ii->yt);
        if (dist > 0) {
          cpw = 1.0 / (dist * dist);
          cdx += cp->dx * cpw;
          cdy += cp->dy * cpw;
          totalw += cpw;
        } else {
          cdx = cp->dx;
          cdy = cp->dy;
          totalw = 1.0;
          break;
        }
        cp = cp->nextcp;
      }
      if (totalw > 0.0) {
        cdx = cdx / totalw;
        cdy = cdy / totalw;
      }
      ii->xt = ii->xt + cdx;
      ii->yt = ii->yt + cdy;
    }
    ii++;
  }
    
}



void thdb2d::pp_morph_points(thdb2dprj * prj)
{
  // initialize morphing function
  thscrap * pscrap = prj->first_scrap;
  thdb2dcp * cp;
  th2ddataobject * pobj;
  thpoint * pointp;
  while (pscrap != NULL) {
    cp = pscrap->fcpp;
    while (cp != NULL) {
      pscrap->morph.insert_point(thvec2(cp->pt->xt, cp->pt->yt), thvec2(cp->tx, cp->ty), cp->st->uid);
      cp = cp->nextcp;
    }
    pobj = pscrap->fs2doptr;
    while (pobj != NULL) {
      if (pobj->get_class_id() == TT_POINT_CMD) {
        pointp = (thpoint *) pobj;
        if (pointp->type == TT_POINT_TYPE_EXTRA) {
          pointp->check_extra();
          if ((pointp->from_name.id > 0) && (!thisnan(pointp->xsize))) {
            pscrap->morph.insert_zoom_point(
              thvec2(pointp->point->xt, pointp->point->yt), 
              pointp->xsize, 
              thdb.db1d.station_vec[pointp->from_name.id - 1].uid);
          }
        }
      }
      pobj = pobj->nscrapoptr;
    }
    pscrap->morph.insert_lines_from_db();
    pscrap->morph.init();
    pscrap = pscrap->proj_next_scrap;
  }
  
  // prejde bod za bodom a spocita ich suradnice
  thvec2 it;
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  while (ii != this->pt_list.end()) {
    if ((ii->pscrap->proj->id == prj->id) && (ii->pscrap->ncp > 0)) {
      it = ii->pscrap->morph.forward(thvec2(ii->xt, ii->yt));
      ii->xt = it.m_x;
      ii->yt = it.m_y;
      ii->dbgx1 = ii->xt;
      ii->dbgy1 = ii->yt;
    }
    ii++;
  }
    
}



struct joincand {
	th2ddataobject * obj;
	thdb2dlp * lp;
	thdb2dpt * pt;
	long fileid;
};


void thdb2d::pp_process_joins(thdb2dprj * prj)
{

  thjoin * jptr, * tjptr;
  prj->first_join_list = NULL;
  prj->last_join_list = NULL;
  thscrap * sc1 = NULL, * sc2 = NULL;
  thdb2dji * ji, * tji;
  thdb2dpt * searchpt;
  thscrapen * se1, * se2, * fse1, * fse2;
  thline * l1, * l2;
  int ccount;
  double mindst, cdst;
  unsigned long nactive;

  // find autojoin candidates
	std::list<joincand> joincandlist;
	std::list<joincand>::iterator jci, njci;
	const char * cfile = "";
	long fileid = 0;
	thscrap * scp;
	th2ddataobject * o2d;
	
	thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SCRAP_CMD) {
			scp = (thscrap *)(*obi);
			if ((scp->proj->type != TT_2DPROJ_NONE) && (scp->proj->id == prj->id)) {
 			o2d = scp->fs2doptr;
			while (o2d != NULL) {
 				// check object source file
				if (strcmp(cfile, o2d->source.name) != 0) {
					cfile = o2d->source.name;
					fileid++;
				}
				// insert candidates (not label)
				thline * pln;
					switch (o2d->get_class_id()) {
						case TT_LINE_CMD:
							pln = (thline*) o2d;
							switch (pln->type) {
 								case TT_LINE_TYPE_LABEL:
 								case TT_LINE_TYPE_SECTION:
 								case TT_LINE_TYPE_ARROW:
 								case TT_LINE_TYPE_GRADIENT:
 								case TT_LINE_TYPE_SURVEY:
 									break;
 							  default:
 									if (pln->first_point != NULL) {
 										joincand jc;
 										jc.obj = pln;
 										jc.fileid = fileid;
 										jc.pt = pln->first_point->point;
 										jc.lp = pln->first_point;
 										joincandlist.insert(joincandlist.end(), jc);
 										if ((pln->first_point->point->x != pln->last_point->point->x) ||
 												(pln->first_point->point->y != pln->last_point->point->y)) {
 											jc.obj = pln;
 											jc.fileid = fileid;
 											jc.pt = pln->last_point->point;
 											jc.lp = pln->last_point;
 											joincandlist.insert(joincandlist.end(), jc);
 										}
 									}
 									break;
 							}
 							break;
 					}
 					o2d = o2d->nscrapoptr;
 				}
 			}
 		}
 		obi++;		
 	}
 
  // prida na koniec automaticke joiny
  jci = joincandlist.begin();
  while (jci != joincandlist.end()) {
    njci = jci;
    njci++;
    while ((njci != joincandlist.end()) && (jci->fileid == njci->fileid)) {
      if (
          (jci->obj->fscrapptr->id != njci->obj->fscrapptr->id) && 
          (jci->obj->fscrapptr->proj->id == njci->obj->fscrapptr->proj->id) &&
          (jci->pt->x == njci->pt->x) &&
          (jci->pt->y == njci->pt->y)) {
         // add automatic join here
        tjptr = (thjoin*) thdb.create("join",thobjectsrc());
        thdb.insert(tjptr);
        tjptr->proj = jci->obj->fscrapptr->proj;
         // TODO: check smoothness???
        tjptr->smooth = TT_FALSE;
                
        if (prj->last_join == NULL) {
          prj->first_join = tjptr;
          prj->last_join = tjptr;
        } else {
          tjptr->proj_prev_join = prj->last_join;
          prj->last_join->proj_next_join = tjptr;
          prj->last_join = tjptr;
        }
        tji = this->insert_join_item();
        tji->object = jci->obj;
        tji->line_point = jci->lp;
        tji->point = jci->pt;
        tji->next_item = this->insert_join_item();
        tji->next_item->object = njci->obj;
        tji->next_item->line_point = njci->lp;
        tji->next_item->point = njci->pt;
        tji->next_item->prev_item = tji;
        tjptr->first_item = tji;
        tjptr->last_item = tji->next_item;
      }
      njci++;
    }
    jci++;
  }
    
  // prejde join za joinom a vytovori zoznamy
  jptr = prj->first_join;
  while (jptr != NULL) {
    nactive = 0;
    ji = jptr->first_item;
    while (ji != NULL) {
      switch (ji->object->get_class_id()) {
        case TT_POINT_CMD:
          if (ji->point != NULL) {
            ji->is_active = true;
            nactive++;
          } else
            ji->is_active = false;
          break;
        case TT_LINE_CMD:
          if ((((thline *)ji->object)->first_point != NULL) &&
             (((thline *)ji->object)->first_point->point != NULL)) {
            ji->is_active = true;
            nactive++;
          } else
            ji->is_active = false;
          break;
        case TT_SCRAP_CMD:

          if (ji->next_item == NULL)
            break;
            
          for(ccount = 0; ccount < jptr->count; ccount++) {
  
            // z dvoch scrapov urobime body na ciarach
            if (ccount == 0) {
              sc1 = (thscrap *) ji->object;
              ji->is_active = false;
              ji = ji->next_item;
              sc2 = (thscrap *) ji->object;
              ji->is_active = false;
            }
            
            // mame 2 scrapy, najdeme spolocne ciary
            fse1 = sc1->get_ends();
            fse2 = sc2->get_ends();
            
#ifdef THDEBUG
            thprintf("\nsearching for join %d of %s and %s:\n\t%% OCTAVE CODE\n\thold on;\n",ccount+1,sc1->name,sc2->name);
#endif

            if (ccount == 0) {
              se1 = fse1;
              while (se1 != NULL) {
                se1->cxt = (se1->lp1->point->xt + se1->lp2->point->xt) / 2.0;
                se1->cyt = (se1->lp1->point->yt + se1->lp2->point->yt) / 2.0;
#ifdef THDEBUG
                thprintf("\tplot([%.2f,%.2f],[%.2f,%.2f],\"r;;\");hold on;\n",se1->lp1->point->xt,se1->lp2->point->xt,se1->lp1->point->yt,se1->lp2->point->yt);
                thprintf("\tplot([%.2f],[%.2f],\"xm;;\");hold on;\n",se1->cxt,se1->cyt);
#endif                
                se1->active = true;
                se1 = se1->next_end;
              }
 
              se2 = fse2;
              while (se2 != NULL) {
                se2->cxt = (se2->lp1->point->xt + se2->lp2->point->xt) / 2.0;
                se2->cyt = (se2->lp1->point->yt + se2->lp2->point->yt) / 2.0;
#ifdef THDEBUG
                thprintf("\tplot([%.2f,%.2f],[%.2f,%.2f],\"b;;\");hold on;\n",se2->lp1->point->xt,se2->lp2->point->xt,se2->lp1->point->yt,se2->lp2->point->yt);
                thprintf("\tplot([%.2f],[%.2f],\"xc;;\");hold on;\n",se2->cxt,se2->cyt);
#endif                
                se2->active = true;
                se2 = se2->next_end;
              }
            }
              
            se1 = fse1;
            while ((se1 != NULL) && (!(se1->active))) {
              se1 = se1->next_end;
              fse1 = se1;
            }

            se2 = fse2;
            while ((se2 != NULL) && (!(se2->active))) {
              se2 = se2->next_end;
              fse2 = se2;
            }
              
            if ((fse1 != NULL) && (fse2 != NULL))
              mindst = hypot(fse1->cxt - fse2->cxt, fse1->cyt - fse2->cyt);
            else {
              // nenasli sme uz dva konce, skipujeme az na koniec
              jptr->throw_source();
              threwarning2(("unable to join scraps (count %d)", ccount + 1));
              ccount = jptr->count;
              goto UNABLE_TO_JOIN;
            }
              
            se1 = fse1;
            while (se1 != NULL) {
              se2 = sc2->get_ends();
              while (se2 != NULL) {
                cdst = hypot(se1->cxt - se2->cxt, se1->cyt - se2->cyt);
//#ifdef THDEBUG
//  thprintf("\ttrying (%.2f,%.2f) - (%.2f,%.2f) = %.2f (MIN: %.2f)\n",se1->cxt,se1->cyt,se2->cxt,se2->cyt,cdst,mindst);
//#endif
                if ((cdst < mindst) && (se1->active) && (se2->active)) {
                  fse1 = se1;
                  fse2 = se2;
                  mindst = cdst;
                }
                se2 = se2->next_end;
              }
              se1 = se1->next_end;
            }
              
            fse1->active = false;
            fse2->active = false;
              
            // mame dva najblizsie, zistime ci ich netreba otocit
            if (((fse1->lp1->point->xt - fse1->cxt) * (fse2->lp1->point->xt - fse2->cxt) +
              (fse1->lp1->point->yt - fse1->cyt) * (fse2->lp1->point->yt - fse2->cyt)) < 0) {
              thline * tline;
              thdb2dlp * tlp;              
              tline = fse2->l2;
              fse2->l2 = fse2->l1;
              fse2->l1 = tline;
              tlp = fse2->lp2;
              fse2->lp2 = fse2->lp1;
              fse2->lp1 = tlp;
            }
              
            // vytvorime z nich joiny
            
#ifdef THDEBUG
            thprintf("\ncreating new 2 joins:\n");
            thprintf("\tline %ld (%.2f,%.2f) -- line %ld (%.2f,%.2f)\n",
                fse1->l1->id, fse1->lp1->point->xt, fse1->lp1->point->yt, 
                fse2->l1->id, fse2->lp1->point->xt, fse2->lp1->point->yt);
            thprintf("\tline %ld (%.2f,%.2f) -- line %ld (%.2f,%.2f)\n",
                fse1->l2->id, fse1->lp2->point->xt, fse1->lp2->point->yt, 
                fse2->l2->id, fse2->lp2->point->xt, fse2->lp2->point->yt);
#endif
            
            tjptr = (thjoin*) thdb.create("join",thobjectsrc());
            thdb.insert(tjptr);
            tjptr->proj = jptr->proj;
            tjptr->smooth = jptr->smooth;
            tjptr->proj_prev_join = jptr;
            tjptr->proj_next_join = (thjoin*) thdb.create("join",thobjectsrc());
            thdb.insert(tjptr->proj_next_join);
            tjptr->proj_next_join->proj_prev_join = tjptr;
            tjptr->proj_next_join->proj_next_join = jptr->proj_next_join;
            if (jptr->proj_next_join != NULL)
              jptr->proj_next_join->proj_prev_join = tjptr->proj_next_join;
            jptr->proj_next_join = tjptr;
            tji = this->insert_join_item();
            tji->object = fse1->l1;
            tji->line_point = fse1->lp1;
            tji->point = fse1->lp1->point;
            tji->next_item = this->insert_join_item();
            tji->next_item->object = fse2->l1;
            tji->next_item->line_point = fse2->lp1;
            tji->next_item->point = fse2->lp1->point;
            tji->next_item->prev_item = tji;
            tjptr->first_item = tji;
            tjptr->last_item = tji->next_item;
 
            tji = this->insert_join_item();
            tji->object = fse1->l2;
            tji->line_point = fse1->lp2;
            tji->point = fse1->lp2->point;
            tji->next_item = this->insert_join_item();
            tji->next_item->object = fse2->l2;
            tji->next_item->line_point = fse2->lp2;
            tji->next_item->point = fse2->lp2->point;
            tji->next_item->prev_item = tji;
            tjptr->proj_next_join->first_item = tji;
            tjptr->proj_next_join->last_item = tji->next_item;              

            UNABLE_TO_JOIN:;
          } // for count
          break;
        default:
          ji->is_active = false;
      }
      ji = ji->next_item;
    }
    
    if (nactive > 1) {
      ji = jptr->first_item;
      searchpt = NULL;
      while ((ji != NULL) && (searchpt == NULL)) {
        if ((ji->is_active) && (ji->point != NULL))
          searchpt = ji->point;
        ji = ji->next_item;
      }
      
      // let's search first two lines for search point
      if (searchpt == NULL) {
        l1 = NULL; l2 = NULL;
        ji = jptr->first_item;
        while ((ji != NULL) && (l2 == NULL)) {
          if (ji->is_active && 
              (ji->object->get_class_id() == TT_LINE_CMD)) {
            if (l1 == NULL)
              l1 = ((thline *)ji->object);
            else
              l2 = ((thline *)ji->object);
          }  
          ji = ji->next_item;
        }
        
        if (l2 == NULL) {
          jptr->throw_source();
          ththrow(("can not determine any join point"))
        }
        
        // teraz najde spolocny bod kriviek l1 a l2
        double mindist = hypot(l1->first_point->point->xt - l2->first_point->point->xt,
          l1->first_point->point->yt - l2->first_point->point->yt);
        searchpt = l1->first_point->point;
        
        double dist = hypot(l1->last_point->point->xt - l2->first_point->point->xt,
          l1->last_point->point->yt - l2->first_point->point->yt);
        if (dist < mindist) {
          searchpt = l1->last_point->point;
          mindist = dist;
        }
          
        dist = hypot(l1->first_point->point->xt - l2->last_point->point->xt,
          l1->first_point->point->yt - l2->last_point->point->yt);
        if (dist < mindist) {
          searchpt = l1->first_point->point;
          mindist = dist;
        }
        
        dist = hypot(l1->last_point->point->xt - l2->last_point->point->xt,
          l1->last_point->point->yt - l2->last_point->point->yt);
        if (dist < mindist) {
          searchpt = l1->last_point->point;
          mindist = dist;
        }
        
      } // here we have search point
      
      // let's traverse all lines, where no point is define it
      ji = jptr->first_item;
      while (ji != NULL) {
        if (ji->is_active &&
            (ji->point == NULL) &&
            (ji->object->get_class_id() == TT_LINE_CMD)) {
          if (hypot(((thline*)ji->object)->first_point->point->xt - searchpt->xt,
              ((thline*)ji->object)->first_point->point->yt - searchpt->yt) <
              hypot(((thline*)ji->object)->last_point->point->xt - searchpt->xt,
              ((thline*)ji->object)->last_point->point->yt - searchpt->yt)) {
            ji->point = ((thline*)ji->object)->first_point->point;
            ji->line_point = ((thline*)ji->object)->first_point;
          } else {
            ji->point = ((thline*)ji->object)->last_point->point;
            ji->line_point = ((thline*)ji->object)->last_point;
          }
        }
        ji = ji->next_item;
      }
      
      // now let's search for control points
      if (jptr->smooth == TT_TRUE) {
        ji = jptr->first_item;
        while (ji != NULL) {
          // && (ji->line_point->smooth == TT_TRUE)
          if ((ji->is_active) && (ji->line_point != NULL)) {
            if (ji->line_point->cp2 != NULL)
              ji->cp1 = ji->line_point->cp2;
            if ((ji->line_point->nextlp != NULL) && (ji->line_point->nextlp->cp1 != NULL))
              ji->cp2 = ji->line_point->nextlp->cp1;
          }
          ji = ji->next_item;
        }
      }
      
      // we have at all active positions point defined,
      // now let's create and insert new list      
      thdb2dji * target_item = NULL, * first_active_item = NULL,
        * ccitem, * ppitem;
      ji = jptr->first_item;
      while ((ji != NULL) && (target_item == NULL)) {
        if (ji->is_active) {
          if (ji->point->join_item != NULL)
            target_item = ji;
          if (first_active_item == NULL)
            first_active_item =ji; 
        }
        ji = ji->next_item;
      }
      if (target_item == NULL) {
        target_item = first_active_item;
        if (prj->last_join_list == NULL) {
          prj->last_join_list = target_item;
          prj->first_join_list = target_item;
        } else {
          prj->last_join_list->next_list = target_item;
          target_item->prev_list = prj->last_join_list;
          prj->last_join_list = target_item;
        }
        target_item->point->join_item = target_item;
      }
      
      // teraz pridame itemy leziace okolo target
      bool ideme_nahor = true;
      ppitem = target_item;
      // add to the end of list
      while (ppitem->next_list_item != NULL) {
        ppitem = ppitem->next_list_item;
      }
      ccitem = target_item->prev_item;
      while ((ccitem != NULL) || ideme_nahor) {
        if ((ccitem != NULL) && (ccitem->is_active)) {
          if (ccitem->point->join_item == NULL) {
            ccitem->point->join_item = target_item;
            ppitem->next_list_item = ccitem;
            ccitem->prev_list_item = ppitem;
            ppitem = ccitem;
          }
        }
        if (ideme_nahor && (ccitem != NULL))
          ccitem = ccitem->prev_item;
        if ((ccitem == NULL) && ideme_nahor) {
          ideme_nahor = false;
          ccitem = target_item;
        }
        if (!ideme_nahor)
          ccitem = ccitem->next_item;
      }      
    } // more than one active item, add join list

    jptr = jptr->proj_next_join;
  }
	
  // zredukuje viacnasobne joiny  
  


	// vyrovna joiny
  thdb2dji * jlist = prj->first_join_list;
  bool has_target;
  double tx = 0.0, ty = 0.0, tn;
  while (jlist != NULL) {
    has_target = false;
    ji = jlist;
    while ((!has_target) && (ji != NULL)) {
      if ((ji->object->get_class_id() == TT_POINT_CMD) &&
          (((thpoint*)ji->object)->cpoint != NULL)) {
        has_target = true;
        tx = ((thpoint*)ji->object)->cpoint->tx;
        ty = ((thpoint*)ji->object)->cpoint->ty;
      }
      ji = ji->next_list_item;
    }
    if (!has_target) {
      tx = 0.0;
      ty = 0.0;
      tn = 0.0;
      ji = jlist;
      while (ji != NULL) {
        tx += ji->point->xt;
        ty += ji->point->yt;
        tn += 1.0;
        ji = ji->next_list_item;
      }
      tx /= tn;
      ty /= tn;
    }
    // now let's add control points
    ji = jlist;
    thdb2dcp * cp;
    while (ji != NULL) {
      if ((ji->object->get_class_id() != TT_POINT_CMD) ||
          (((thpoint*)ji->object)->cpoint == NULL)) {
        cp = ji->point->pscrap->insert_control_point();
        cp->pt = ji->point;
        cp->tx = tx;
        cp->ty = ty;
      }
      ji = ji->next_list_item;
    }
    jlist = jlist->next_list;
  }

}


void thdb2d_pp_adjust_cp(double & xp, double & yp,
  double x, double y,
  double & xn, double & yn) {
  
  double v1x, v2x, v1y, v2y, vvx, vvy, dv, d1, d2;

  v1x = x - xp;
  v1y = y - yp;
  d1 = hypot(v1x,v1y);
  
  v2x = xn - x;
  v2y = yn - y;
  d2 = hypot(v2x,v2y);
  
  if ((d1 > 0.0) && (d2 > 0.0)) {
  
    vvx = v1x + v2x;
    vvy = v1y + v2y;
    dv = hypot(vvx,vvy);
    
    if (dv > 0.0) {
      vvx /= dv;
      vvy /= dv;
    } else {
      vvx = -v1y / d1;
      vvy = v1x / d1;
    }
    
    xp = x - d1 * vvx;
    yp = y - d1 * vvy;
    
    xn = x + d2 * vvx;
    yn = y + d2 * vvy;
    
  }
  
  return;
}


void thdb2d::pp_smooth_lines(thdb2dprj * prj) 
{
  // prejde scrap za scrapom, ciaru za ciarov a vyrovna vsetky smooth body
  thscrap * pscrap;
  th2ddataobject * optr;
  thdb2dlp * pt;
  pscrap = prj->first_scrap;
  while (pscrap != NULL) {
    optr = pscrap->fs2doptr;
    while (optr != NULL) {
      if ((optr->get_class_id() == TT_LINE_CMD)
        && (((thline *)optr)->first_point != NULL)
        && (((thline *)optr)->first_point->nextlp != NULL)) {
        
        pt = ((thline *) optr)->first_point->nextlp;
        while (pt->nextlp != NULL) {
          if ((pt->smooth == TT_TRUE) &&
              (pt->cp2 != NULL) && (pt->nextlp->cp1 != NULL))
            thdb2d_pp_adjust_cp(pt->cp2->xt, pt->cp2->yt,
                pt->point->xt, pt->point->yt,
                pt->nextlp->cp1->xt, pt->nextlp->cp1->yt);
          pt = pt->nextlp;
        }
      }
      optr = optr->nscrapoptr;
    }
    pscrap = pscrap->proj_next_scrap;
  }
  
}

void thdb2d::pp_smooth_joins(thdb2dprj * prj) 
{
  double x1,x2,y1,y2,d1,d2,tmpx,tmpy,testx = 0.0,testy = 0.0,dt;
  bool has_test;
  unsigned long ncp, ncp1, ncp2;
  thdb2dji * jlist = prj->first_join_list, * ji;
  thdb2dpt * tcp;
  while (jlist != NULL) {
    ji = jlist;
    has_test = false;
    // 1. najde testovaci vektor
    while ((!has_test) && (ji != NULL)) {
      if (ji->cp1 != NULL) {
        has_test = true;
        testx = ji->cp1->xt - ji->point->xt;
        testy = ji->cp1->yt - ji->point->yt;
      } else if (ji->cp2 != NULL) {
        has_test = true;
        testx = ji->cp2->xt - ji->point->xt;
        testy = ji->cp2->yt - ji->point->yt;
      }
      ji = ji->next_list_item;
    }
    
    // prejde vsetky CP, prislusne ich poprehadzuje do kategorii,
    // vyrata xy1 a xy2
    ncp1 = 0;
    ncp2 = 0;
    ncp = 0;
    if (has_test) {
      ji = jlist;
      while (ji != NULL) {
        if (ji->cp1 != NULL) {
          tmpx = ji->cp1->xt - ji->point->xt;
          tmpy = ji->cp1->yt - ji->point->yt;
          if ((tmpx * testx + tmpy * testy) < 0) {
            tcp = ji->cp1;
            ji->cp1 = ji->cp2;
            ji->cp2 = tcp;
          }
        } else if (ji->cp2 != NULL) {
          tmpx = ji->cp2->xt - ji->point->xt;
          tmpy = ji->cp2->yt - ji->point->yt;
          if ((tmpx * testx + tmpy * testy) > 0) {
            tcp = ji->cp1;
            ji->cp1 = ji->cp2;
            ji->cp2 = tcp;
          }
        }
        if (ji->cp1 != NULL) {
          tmpx = ji->cp1->xt - ji->point->xt;
          tmpy = ji->cp1->yt - ji->point->yt;
          ncp++;
          if (ncp1 > 0) {
            x1 += tmpx;
            y1 += tmpy;
          } else {
            x1 = tmpx;
            y1 = tmpy;
          }
          ncp1++;
        }
        if (ji->cp2 != NULL) {
          tmpx = ji->cp2->xt - ji->point->xt;
          tmpy = ji->cp2->yt - ji->point->yt;
          ncp++;
          if (ncp2 > 0) {
            x2 += tmpx;
            y2 += tmpy;
          } else {
            x2 = tmpx;
            y2 = tmpy;
          }
          ncp2++;
        }
        ji = ji->next_list_item;
      }

      if (ncp1 == 0) {
        x1 = 0.0;
        y1 = 0.0;
      }
      if (ncp2 == 0) {
        x2 = 0.0;
        y2 = 0.0;
      }
      
      // samotne vyrovnanie
      if (ncp > 1) {
        
        thdb2d_pp_adjust_cp(x1,y1,0.0,0.0,x2,y2);
        
        // znormuje dlzky cp vektorov
        d1 = hypot(x1,y1);
        d2 = hypot(x2,y2);
        if (d1 > 0) {
          x1 /= d1;
          y1 /= d1;
        }
        if (d2 > 0) {
          x2 /= d2;
          y2 /= d2;
        }
        
        ji = jlist;
        while (ji != NULL) {
          if (ji->cp1 != NULL) {
            tmpx = ji->cp1->xt - ji->point->xt;
            tmpy = ji->cp1->yt - ji->point->yt;
            dt = hypot(tmpx,tmpy);
            ji->cp1->xt = ji->point->xt + x1 * dt;
            ji->cp1->yt = ji->point->yt + y1 * dt;
          }
          if (ji->cp2 != NULL) {
            tmpx = ji->cp2->xt - ji->point->xt;
            tmpy = ji->cp2->yt - ji->point->yt;
            dt = hypot(tmpx,tmpy);
            ji->cp2->xt = ji->point->xt + x2 * dt;
            ji->cp2->yt = ji->point->yt + y2 * dt;
          }
          ji = ji->next_list_item;
        }
      } // koniec samotneho vyrovnania
    } // koniec has_test
    jlist = jlist->next_list;
  }
}

#define calcdist(p1,p2) { \
  tmp = hypot(p1->dbgx0 - p2->dbgx0, p1->dbgy0 - p2->dbgy0); \
  if (tmp > 0.0) { \
    tmp = hypot(p1->xt - p2->xt, p1->yt - p2->yt) / tmp; \
    tmp = 100.0 * (1 - ((tmp > 1.0) ? (1.0 / tmp) : tmp)); \
    cavdist += tmp; \
    if (cdist < tmp) { \
      cdist = tmp; \
      ps->maxdistp1 = p1; \
      ps->maxdistp2 = p2; \
    } \
    numcavdist += 1.0; \
  } \
}

void thdb2d::pp_calc_distortion(thdb2dprj * prj) {
  prj->amaxdist = 0.0;
  double nums = 0.0;
  th2ddataobject * so, * so2;
  thdb2dlp * lp, * lp2;
  double cdist, cavdist, numcdist, numcavdist, tmp;
  if (prj->type == TT_2DPROJ_NONE) 
    return;
  thscrap * ps = prj->first_scrap;
  if (ps == NULL)
    return;
  while (ps != NULL) {
    cdist = 0.0;
    numcdist = 0.0;
    numcavdist = 0.0;
    ps->maxdistp1 = NULL;
    ps->maxdistp2 = NULL;
    cavdist = 0.0;
    so = ps->fs2doptr;
    while (so != NULL) {
      switch (so->get_class_id()) {
        case TT_LINE_CMD:
          lp = ((thline*)so)->first_point;
          while (lp != NULL) {
            numcdist += 1.0;
            if (lp->cp1 != NULL) {
              calcdist(lp->point, lp->cp1);
              numcdist += 1.0;
            }
            if (lp->cp2 != NULL) {
              numcdist += 1.0;
              calcdist(lp->point, lp->cp2);
            }
            lp2 = lp->nextlp;
            while (lp2 != NULL) {
              calcdist(lp->point, lp2->point);
              lp2 = lp2->nextlp;
            }
            so2 = so->nscrapoptr;
            while (so2 != NULL) {
              switch(so2->get_class_id()) {
                case TT_POINT_CMD:
                  calcdist(((thpoint*)so2)->point,lp->point);
                  break;
                case TT_LINE_CMD:
                  lp2 = ((thline*)so2)->first_point;
                  while (lp2 != NULL) {
                    calcdist(lp->point, lp2->point);
                    lp2 = lp2->nextlp;
                  }
                  break;
              }
              so2 = so2->nscrapoptr;
            }
            lp = lp->nextlp;
          }
          break;
        case TT_POINT_CMD:
          numcdist += 1.0;
          so2 = so->nscrapoptr;
          while (so2 != NULL) {
            switch(so2->get_class_id()) {
              case TT_POINT_CMD:
                calcdist(((thpoint*)so)->point,((thpoint*)so2)->point);
                break;
              case TT_LINE_CMD:
                lp2 = ((thline*)so2)->first_point;
                while (lp2 != NULL) {
                  calcdist(((thpoint*)so)->point, lp2->point);
                  lp2 = lp2->nextlp;
                }
                break;
            }
            so2 = so2->nscrapoptr;
          }
          break;
      }
      so = so->nscrapoptr;
    }
    ps->maxdist = cdist;
    if (numcavdist > 0.0) {
      ps->avdist = cavdist / numcavdist;
      prj->amaxdist += ps->avdist * numcdist;
      nums += numcdist;
    }
    ps = ps->proj_next_scrap;
  }
  if (nums > 0.0)
    prj->amaxdist = prj->amaxdist / nums;
  else
    prj->amaxdist = 0;
}

bool thdb2d::pp_process_adjustments(thdb2dprj * prj)
{
  bool anycp = false;
  thscrap * ps;
  th2ddataobject * so;
  thline * ln;
  thdb2dlp * lp, * lastlp;
  thdb2dcp * cp;
  int Vadj, Hadj;
  unsigned nlp;
  double aver;
  bool lastlpin = false;

  // scrap za scrapom
  ps = prj->first_scrap;
  while (ps != NULL) {

    // ciara za ciarou
    so = ps->fs2doptr;
    while (so != NULL) {
      if (so->get_class_id() == TT_LINE_CMD) {
        
        ln = (thline*) so;
        
        // vertical adjustment
        // 1. skusi ci je nejaky adjustment
        Vadj = -1; Hadj = -1;
        lp = ln->first_point;
        nlp = 0;
        while (lp != NULL) {
          nlp++;
          switch (lp->adjust) {
            case TT_LINE_ADJUST_HORIZONTAL:
              if (Hadj < 0)
                Hadj = 2;
              else if (Hadj == 0)
                Hadj = 1;
              if (Vadj > 0)
                Vadj = 1;
              break;
            case TT_LINE_ADJUST_VERTICAL:
              if (Vadj < 0)
                Vadj = 2;
              else if (Vadj == 0)
                Vadj = 1;
              if (Hadj > 0)
                Hadj = 1;
              break;
            default:
              if (Vadj > 0)
                Vadj = 1;
              if (Hadj > 0)
                Hadj = 1;
              break;
          }
          if (Vadj < 0)
            Vadj = 0;
          if (Hadj < 0)
            Hadj = 0;
          lp = lp->nextlp;
        }
        
        // adj mame 0 (ziadny), 1 (nejaky), 2 (vsetky)
        if (nlp > 1) {
          // horizontalne
          switch (Hadj) {
            case 2:
              // zratame priemer a vlozime vsetky
              aver = 0.0;
              lp = ln->first_point;
              while (lp != NULL) {
                aver += lp->point->yt;
                lp = lp->nextlp;
              }
              aver /= (double) nlp;
              lp = ln->first_point;
              while (lp != NULL) {
                cp = ln->fscrapptr->insert_control_point();
                cp->pt = lp->point;
                cp->tx = lp->point->xt;
                cp->ty = aver;
                lp = lp->nextlp;
              }
              anycp = true;
              break;
            case 1:
              // poojdeme bod za bodom, najdeme najblizsi predchadzajuci
              // ak nie tak nasledujuci a pouzijeme jeho yt
              lp = ln->first_point;
              lastlp = NULL;
              while (lp != NULL) {
                if (lp->adjust != TT_LINE_ADJUST_HORIZONTAL) {
                  lastlpin = false;
                  lastlp = lp;
                  break;
                }
                lp = lp->nextlp;
              }
              if (lastlp == NULL)
                break;
              lp = ln->first_point;
              while (lp != NULL) {
                if (lp->adjust == TT_LINE_ADJUST_HORIZONTAL) {
                  if (!lastlpin) {
                    lastlpin = true;
                    cp = ln->fscrapptr->insert_control_point();
                    cp->pt = lastlp->point;
                    cp->tx = lastlp->point->xt;
                    cp->ty = lastlp->point->yt;
                  }
                  cp = ln->fscrapptr->insert_control_point();
                  cp->pt = lp->point;
                  cp->tx = lp->point->xt;
                  cp->ty = lastlp->point->yt;
                  anycp = true;
                } else {
                  lastlpin = false;
                  lastlp = lp;
                }
                lp = lp->nextlp;
              }
              break;
          }
          switch (Vadj) {
            case 2:
              // zratame priemer a vlozime vsetky
              aver = 0.0;
              lp = ln->first_point;
              while (lp != NULL) {
                aver += lp->point->xt;
                lp = lp->nextlp;
              }
              aver /= (double) nlp;
              lp = ln->first_point;
              while (lp != NULL) {
                cp = ln->fscrapptr->insert_control_point();
                cp->pt = lp->point;
                cp->ty = lp->point->yt;
                cp->tx = aver;
                lp = lp->nextlp;
              }
              anycp = true;
              break;
            case 1:
              // poojdeme bod za bodom, najdeme najblizsi predchadzajuci
              // ak nie tak nasledujuci a pouzijeme jeho yt
              lp = ln->first_point;
              lastlp = NULL;
              while (lp != NULL) {
                if (lp->adjust != TT_LINE_ADJUST_VERTICAL) {
                  lastlpin = false;
                  lastlp = lp;
                  break;
                }
                lp = lp->nextlp;
              }
              if (lastlp == NULL)
                break;
              lp = ln->first_point;
              while (lp != NULL) {
                if (lp->adjust == TT_LINE_ADJUST_VERTICAL) {
                  if (!lastlpin) {
                    lastlpin = true;
                    cp = ln->fscrapptr->insert_control_point();
                    cp->pt = lastlp->point;
                    cp->tx = lastlp->point->xt;
                    cp->ty = lastlp->point->yt;
                  }
                  cp = ln->fscrapptr->insert_control_point();
                  cp->pt = lp->point;
                  cp->tx = lastlp->point->xt;
                  cp->ty = lp->point->yt;
                  anycp = true;
                } else {
                  lastlpin = false;
                  lastlp = lp;
                }
                lp = lp->nextlp;
              }
              break;
          }
        }
        
        
      }
      so = so->nscrapoptr;
    } // ciara za ciarou

    ps = ps->proj_next_scrap;
  } // scrap za scrapom
  
  return anycp;
  
}

thdb2dprj * thdb2d::get_projection(int id) {
  thdb2dprj_list::iterator pi;
  for(pi = this->prj_list.begin(); pi != this->prj_list.end(); pi++) {
    if (pi->id == id)
      return &(*pi);
  }
  return NULL;
}



struct area_proc {
  tharea * area;
  double x, y, a, s;
};

void thdb2d::process_areas_in_projection(thdb2dprj * prj)
{

  if (this->processed_area_outlines)
    return;

  this->process_projection(prj);
  this->processed_area_outlines = true;

  std::list<area_proc> todo;

  double xx(0.0), xy(0.0), nx(0.0), ny(0.0), xa(0.0), na(0.0), dx(0.0), dy(0.0);
  long cnt(0), cnt2;

#define ulim(x,y,a) \
  { if (cnt == 0) {xx = x; xy = y; xa = a; nx = x; ny = y; na = a;} \
  else { \
    if (x > xx) xx = x; if (x < nx) nx = x; \
    if (y > xy) xy = y; if (y < ny) ny = y; \
    if (a > xa) xa = a; if (a < na) na = a; \
  } \
  cnt++; }

  thscrap * cscrap;
  th2ddataobject * obj;
  tharea * carea(NULL);
  thdb2dab * bln;
  area_proc itm;
  thline * cln;
  thdb2dlp * clp;
  for (cscrap = prj->first_scrap; cscrap != NULL; cscrap = cscrap->proj_next_scrap) {
    for (obj = cscrap->fs2doptr; obj != NULL; obj = obj->nscrapoptr) {
      if (obj->get_class_id() == TT_AREA_CMD) {
        carea = (tharea*) obj;
        cnt = 0;
        for (bln = carea->first_line; bln != NULL; bln = bln->next_line) {
          cln = bln->line;
          for (clp = cln->first_point; clp != NULL; clp = clp->nextlp) {
            if (clp->cp1 != NULL) ulim(clp->cp1->xt, clp->cp1->yt, clp->cp1->at);
            if (clp->cp2 != NULL) ulim(clp->cp2->xt, clp->cp2->yt, clp->cp2->at);
            ulim(clp->point->xt, clp->point->yt, clp->point->at);
          }
        }
        dx = (xx - nx);
        dy = (xy - ny);
        if ((cnt > 0) && (dx > 0.0) && (dy > 0.0)) {
          itm.area = carea;
          itm.x = 0.5 * (xx + nx);
          itm.y = 0.5 * (xy + ny);
          itm.a = 0.5 * (xa + na);
          itm.s = dy > dx ? (200.0 / dy) : (200.0 / dx);
          todo.push_back(itm);
        }
      }
    }
  }

  // export mp file
  FILE * mpf;
  mpf = fopen(thtmp.get_file_name("data.mp"),"w");
  fprintf(mpf,
"numeric area_border_errors;\narea_border_errors = 0;\n"
"string current_scrap, current_src;\n\n\n"
"vardef buildcycle(text ll) =\n"
"  save ta_, tb_, k_, i_, pp_; path pp_[];\n"
"  k_=0;\n"
"  for q=ll: pp_[incr k_]=q; endfor\n"
"  i_=k_;\n"
"  for i=1 upto k_:\n"
"    (ta_[i], length pp_[i_]-tb_[i_]) =\n"
"      pp_[i] intersectiontimes reverse pp_[i_];\n"
"    if ta_[i]<0:\n"
"      message(\"[Error: area borders \"& area_border[i] &\" and \"&\n"
"         area_border[i_] &\" don't intersect in scrap \" & current_scrap &\n"
"         \" (file \" & current_src & \")]\");\n"
"      area_border_errors := area_border_errors + 1;\n"
"    fi\n"
"    i_ := i;\n"
"  endfor\n"
"  for i=1 upto k_: subpath (ta_[i],tb_[i]) of pp_[i] .. endfor\n"
"    cycle\n"
"enddef;\n"
"\n"
"\n\nbeginfig(1);\n");

  std::list<area_proc>::iterator ti;
  thdb_revision_set_type::iterator ri;
  const char * sn;
  long blnum;

  for (ti = todo.begin(); ti != todo.end(); ti++) {
    itm = (*ti);
    carea = itm.area;
    ri = this->db->revision_set.find(threvision(carea->id, 0));
    fprintf(mpf,"\n\n\ncurrent_scrap := \"%s", carea->fscrapptr->name);
    sn = carea->fscrapptr->fsptr->get_full_name(); 
    if (strlen(sn) > 0) fprintf(mpf,"@%s", sn);
    fprintf(mpf,"\";\ncurrent_src := \"%s [%ld]\";\n", ri->srcf.name, ri->srcf.line);  
    fprintf(mpf,"string area_border[];\n");
    blnum = 1;
    for (bln = carea->first_line; bln != NULL; bln = bln->next_line) {
      fprintf(mpf,"area_border[%ld] := \"%s\";\n", blnum, bln->line->name);
      blnum++;
    }

#define mpfpt(xx,yy) fprintf(mpf, "(%.6f,%.6f)", \
    ((xx) - itm.x) * itm.s, \
    ((yy) - itm.y) * itm.s)

    fprintf(mpf,"\ndraw (buildcycle(");
    for (cnt = 0, bln = carea->first_line; bln != NULL; bln = bln->next_line, cnt++) {
      cln = bln->line;      
      if (cnt > 0)
        fprintf(mpf,",(");
      else
        fprintf(mpf,"(");
      for (clp = cln->first_point, cnt2 = 0; clp != NULL; clp = clp->nextlp, cnt2++) {
        if (cnt2 > 0) {
          if ((clp->cp1 != NULL) && (clp->cp2 != NULL)) {
            fprintf(mpf," .. controls ");
            mpfpt(clp->cp1->xt, clp->cp1->yt);
            fprintf(mpf," and ");
            mpfpt(clp->cp2->xt, clp->cp2->yt);
            fprintf(mpf," .. ");
          } else {
            fprintf(mpf," -- ");
          }
        }
        mpfpt(clp->point->xt, clp->point->yt);
        fprintf(mpf,"\n");
      }
      fprintf(mpf,")");
    }
    fprintf(mpf,"));\n");
  }
  fprintf(mpf,"endfig;\nend;\n");
  fclose(mpf);

  // run metapost
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

  thbuffer com, wdir;
  wdir.guarantee(1024);
  getcwd(wdir.get_buffer(),1024);
  chdir(thtmp.get_dir_name());
  int retcode;

  com = "\"";
  com += thini.get_path_mpost();
  com += "\" ";
  com += thini.get_opt_mpost();
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

  // load data back to therion
  FILE * af;
  af = fopen("data.1","r");
  double n[6];
  com.guarantee(256);
  cln = NULL;
  char * buff = com.get_buffer();
  ti = todo.begin();
  while ((fscanf(af,"%32s",buff) > 0) && (ti != todo.end())) {
    if (cnt < 6) {
      thparse_double(retcode, n[cnt], buff);
      if (retcode == TT_SV_NUMBER) {
        cnt++;
      }
    }
    if ((cnt == 6) && (strcmp(buff,"curveto") == 0) && (cln != NULL)) {
      cln->insert_line_point(6, NULL, n);
      cnt = 0;
    }
    if ((cnt == 2) && ((strcmp(buff,"lineto") == 0) || (strcmp(buff,"moveto") == 0)) && (cln != NULL)) {
      cln->insert_line_point(2, NULL, n);
      cnt = 0;
    }
    if (strcmp(buff,"newpath") == 0) { 
      cln = new thline;
      cln->db = this->db;
      cnt = 0;
    }
    if ((cln != NULL) && (cln->first_point != NULL) && (cln->first_point->nextlp != NULL) && (strcmp(buff,"closepath") == 0)) {
      
      // close path if not closed
      if ((cln->last_point->point->x != cln->first_point->point->x) ||
        (cln->last_point->point->y != cln->first_point->point->y)) {
        n[0] = cln->first_point->point->x;
        n[1] = cln->first_point->point->y;
        cln->insert_line_point(2, NULL, n);
      }

#define mpfptback(pt) if (pt != NULL) { \
  pt->xt = pt->x / ti->s + ti->x; \
  pt->yt = pt->y / ti->s + ti->y; \
  pt->zt = ti->a; \
  pt->at = ti->a; \
  pt->pscrap = carea->fscrapptr; \
}

      for (clp = cln->first_point; clp != NULL; clp = clp->nextlp) {
        mpfptback(clp->cp1);
        mpfptback(clp->cp2);
        mpfptback(clp->point);
      }
      
      // add line
      //cln->reverse = true;
      cln->is_closed = true;
      cln->fscrapptr = carea->fscrapptr;
      cln->fsptr = carea->fsptr;
      cln->type = TT_LINE_TYPE_BORDER;
      ti->area->m_outline_line = cln;
      //cln->preprocess();
      cln = NULL;

      // increase area counter
      ti++;

    }
  }

  delete cln;

  chdir(wdir.get_buffer());

}



void thdb2d::register_u_symbol(int cmd, const char * type)
{
  thdb2d_udef x(cmd, type);
  thdb2d_udef_map::iterator it = this->m_udef_map.find(x);
  if (it == this->m_udef_map.end()) {
    thdb2d_udef_prop * p;
    p = &(*this->m_udef_list.insert(this->m_udef_list.end(), thdb2d_udef_prop()));
    this->m_udef_map[x] = p;
  }
}


thdb2d_udef_prop * thdb2d::get_u_symbol_prop(int cmd, const char * type)
{
  thdb2d_udef x(cmd, type);
  thdb2d_udef_map::iterator it = this->m_udef_map.find(x);
  if (it == this->m_udef_map.end())
    return NULL;
  else return it->second;
}


void thdb2d_udef_prop::reset() {
  this->m_assigned = true;
  this->m_used = false;
}


void thdb2d::use_u_symbol(int cmd, const char * type)
{
  thdb2d_udef_prop * p = get_u_symbol_prop(cmd, type);
  p->m_used = true;
}


void thdb2d::reset_u_symbols()
{
  thdb2d_udef_map::iterator it;
  for(it = this->m_udef_map.begin(); it != this->m_udef_map.end(); it++) {
    it->second->reset();
  }
}


bool thdb2d::is_u_symbol_used(int cmd, const char * type)
{
  thdb2d_udef_prop * p = get_u_symbol_prop(cmd, type);
  return p->m_used;
}


void thdb2d::export_mp_header(FILE * f)
{
  thdb2d_udef_map::iterator it;
  fprintf(f, "\n%% user defined symbols defaults\n");
  for(it = this->m_udef_map.begin(); it != this->m_udef_map.end(); it++) {
    switch (it->first.m_command) {
      case TT_POINT_CMD:
        fprintf(f, "let p_u_%s = p_u;\n", it->first.m_type);
        fprintf(f, "def p_u_%s_legend = p_u_%s((0.5,0.5) inscale,0.0,1.0,(0,0)) enddef;\n", it->first.m_type, it->first.m_type);
        break;
      case TT_LINE_CMD:
        fprintf(f, "let l_u_%s = l_u;\n", it->first.m_type);
        fprintf(f, "def l_u_%s_legend = l_u_%s(((-.3,0.5) .. (.3,.3) .. (.7,.7) .. (1.3,.5)) inscale) enddef;\n", it->first.m_type, it->first.m_type);
        break;
      case TT_AREA_CMD:
        fprintf(f, "let a_u_%s = a_u;\n", it->first.m_type);
        fprintf(f, "def a_u_%s_legend = a_u_%s(buildcycle((((-1,0) -- (1,0) -- (1,1) -- (0,1) -- (0,-1))  inscale))) enddef;\n", it->first.m_type, it->first.m_type);
        break;
    }
  }
  fprintf(f, "%% end of user defined symbols\n\n");
}



