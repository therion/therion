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
#include <list>


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




thdb2d::thdb2d()
{
  this->db = NULL;
  this->prj_lid = 0;
  thdb2dprjpr dpr = this->parse_projection("plan");
  this->prj_default = dpr.prj; 
}


thdb2d::~thdb2d()
{
}


void thdb2d::assigndb(thdatabase * dbptr)
{
  this->db = dbptr;
}


void thdb2d_rotate_align(int & align, double rot) {

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

thdb2dprjpr thdb2d::parse_projection(char * prjstr,bool insnew) 
{

  // let's split string into type - index - param - units
  thsplit_words(& this->mbf, prjstr);
  thdb2dprjpr ret_val;
  thdb2dprj tp;
  char ** pars = this->mbf.get_buffer(), ** pars2;
  char * type_str, * index_str = "";
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
      index_str = this->db->strstore(pars2[1], true);
      if (!th_is_keyword(index_str))
        ththrow(("projection index not a keyword -- %s", index_str))
      break;
    default:
      ththrow(("only one projection index allowed -- %s", pars[0]))
  } 
  prj_type = thmatch_token(type_str,thtt_2dproj);
  if (prj_type == TT_2DPROJ_UNKNOWN)
    ththrow(("unknown projection type -- %s", pars[0]));
  
  if ((prj_type == TT_2DPROJ_NONE) && (strlen(index_str) > 0))
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
        if (asv != TT_SV_NUMBER)
          ththrow(("invalid projection parameter -- %s",pars[1]))
        if (npar > 2) {
          angtf.parse_units(pars[2]);
          par = angtf.transform(par);
        }
        if ((par < 0.0) || (par >= 360.0))
          ththrow(("elevation orientation out of range -- %s", pars[1]))
      }
      break;
    default:
      if (npar > 1)
        ththrow(("too many projection arguments"))
      break;
  }
  
  // let's find projection in a set or create a new one
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
    // check parameters
    switch (prj_type) {
      case TT_2DPROJ_ELEV:
        if (par != pi->second->pp1) {
          ret_val.parok = false;
        }
    }
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
  return &(* this->pt_list.insert(this->pt_list.end()));
}


thdb2dlp * thdb2d::insert_line_point()
{
  return &(* this->lp_list.insert(this->lp_list.end()));
}

thdb2dab * thdb2d::insert_border_line()
{
  return &(* this->ab_list.insert(this->ab_list.end()));
}

thdb2dji * thdb2d::insert_join_item()
{
  return &(* this->ji_list.insert(this->ji_list.end()));
}

thdb2dmi * thdb2d::insert_map_item()
{
  return &(* this->mi_list.insert(this->mi_list.end()));
}

thdb2dcp * thdb2d::insert_control_point()
{
  return &(* this->cp_list.insert(this->cp_list.end()));
}

thdb2dxm * thdb2d::insert_xm()
{
  return &(* this->xm_list.insert(this->xm_list.end()));
}

thdb2dxs * thdb2d::insert_xs()
{
  return &(* this->xs_list.insert(this->xs_list.end()));
}

thscraplo * thdb2d::insert_scraplo()
{
  return &(* this->scraplo_list.insert(this->scraplo_list.end()));
}

thlayoutln * thdb2d::insert_layoutln()
{
  return &(* this->layoutln_list.insert(this->layoutln_list.end()));
}

thscrapen * thdb2d::insert_scrapen()
{
  return &(* this->scrapen_list.insert(this->scrapen_list.end()));
}

thscraplp * thdb2d::insert_scraplp()
{
  return &(* this->scraplp_list.insert(this->scraplp_list.end()));
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
    obi++;
  }


  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_MAP_CMD:
        this->postprocess_map_references((thmap *) *obi);
        break;
    }
    obi++;
  }

#ifdef THDEBUG
#else
  thprintf("done.\n");
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
  thdb2dmi * citem = mptr->first_item;
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
    switch (optr->get_class_id()) {
      case TT_MAP_CMD:
        mapp = (thmap *) optr;
        // if not defined - process recursively
        if (mapp->projection_id == -1) {
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
    thscrap * scrapp;
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
        scrapp = (thscrap *) optr;
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
  thscrap * scrapp;
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
        scrapp = (thscrap*) optr;
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
  char * err_code = "invalid station or point reference";
  switch (pp->type) {
    case TT_POINT_TYPE_STATION:
      if (! pp->station_name.is_empty()) {
        pp->station_name.id = this->db->db1d.get_station_id(pp->station_name,pp->fsptr);
        if (pp->station_name.id == 0) {
          pp->throw_source();
          if (pp->station_name.survey != NULL)
            threthrow2(("invalid station reference -- %s@%s",
              pp->station_name.name,pp->station_name.survey))
          else
            threthrow2(("invalid station reference -- %s",
              pp->station_name.name))
        }
      }
      
      if (! pp->extend_name.is_empty()) {
        pp->extend_name.id = this->db->db1d.get_station_id(pp->extend_name,pp->fsptr);
        if (pp->extend_name.id == 0) {
          optr = this->db->get_object(pp->extend_name, pp->fsptr);
          extend_error = true;
          if (optr != NULL)
            if (optr->get_class_id() == TT_POINT_CMD) {
              pp->extend_point = (thpoint *) optr;
              if (pp->extend_point->type == TT_POINT_TYPE_STATION) {
                if (pp->extend_point->fscrapptr->id == pp->fscrapptr->id) {
                  if (!(pp->extend_point->station_name.is_empty()))
                    extend_error = false;
                  else
                    err_code = "no station reference for given point reference";
                }
                else
                  err_code = "referenced point not within the same scrap";
              }
              else
                err_code = "referenced point type is not station";
            }
            else
              err_code = "not a point reference";
        }
        if (extend_error) {
          pp->throw_source();
          if (pp->station_name.survey != NULL)
            threthrow2(("%s -- %s@%s",err_code,
              pp->extend_name.name,pp->extend_name.survey))
          else
            threthrow2(("%s -- %s",err_code,
              pp->extend_name.name))
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
  if ((lp != NULL) && (sptr->proj->type == TT_2DPROJ_EXTEND)) {
    sptr->throw_source();
    threthrow2(("scrap option -stations not allowed in extended projection"))
  }
  
  while (lp != NULL) {
    if (!(lp->station_name.is_empty())) {
      lp->station_name.id = this->db->db1d.get_station_id(lp->station_name,lp->station_name.psurvey); //sptr->fsptr);
      if (lp->station_name.id == 0) {
        sptr->throw_source();
        if (lp->station_name.survey != NULL)
          threthrow2(("invalid station reference -- %s@%s",
            lp->station_name.name,lp->station_name.survey))
        else
          threthrow2(("invalid station reference -- %s",
            lp->station_name.name))
      } else
        lp->station = &(this->db->db1d.station_vec[lp->station_name.id - 1]);
        if (lp->station->uid > 0)
          lp->ustation = &(this->db->db1d.station_vec[lp->station->uid - 1]);
        else
          lp->ustation = lp->station;
    }
    lp = lp->next_item;
  }
}



void thdb2d::process_projection(thdb2dprj * prj)
{
  if (prj->processed)
    return;
    
  // make sure that 1D tree is processed
  thdb.db1d.get_tree_size();  
  
  char * prjstr;
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

  if (strlen(prj->index) > 0) {
#ifdef THDEBUG
    thprintf("\n\nprocessing projection %s:%s\n",prjstr,prj->index);
#else
    thprintf("processing projection %s:%s ... ",prjstr,prj->index);
    thtext_inline = true;
#endif 
  } else {
#ifdef THDEBUG
    thprintf("\n\nprocessing projection %s\n",prjstr);
#else
    thprintf("processing projection %s ... ",prjstr);
    thtext_inline = true;
#endif   
  }
    
  prj->processed = true;
  this->pp_find_scraps_and_joins(prj);
  this->pp_scale_points(prj);
  if (prj->type != TT_2DPROJ_NONE) {
    this->pp_calc_stations(prj);
    this->pp_adjust_points(prj); 
    this->pp_shift_points(prj, true);
  }
  this->pp_process_joins(prj);
  this->pp_smooth_lines(prj);
  this->pp_smooth_joins(prj);
  this->pp_calc_limits(prj);
  
#ifdef THDEBUG
#else
  thprintf("done.\n");
  thtext_inline = false;
#endif 

}


void thdb2d::pp_calc_stations(thdb2dprj * prj)
{
  double minx, miny, minz, maxx, maxy, maxz, shift_x, shift_y, shift_z;
  minx = thnan;
  
  // prejde vsetky objekty a urobi nasledovne veci
  thscrap * pps;
  thpoint * ppp;
  thdb2dcp * cp, * scancp, * rootcp;
  bool has_root = false, some_attached;
  unsigned long nattached, numcps = 0, numscraps = 0;
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  unsigned long searchid;
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
        thdb1ds * uidst = & (this->db->db1d.station_vec[st->uid - 1]);;
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
  double sina, cosa, dxy;
  switch (prj->type) {
    case TT_2DPROJ_ELEV:
      sina = sin(prj->pp1 / 180.0 * 3.14159265358);
      cosa = cos(prj->pp1 / 180.0 * 3.14159265358);
      break;
  }

  pps = prj->first_scrap;  
  while (pps != NULL) {
  
    if (pps->ncp < 1) {
      pps->throw_source();
      threthrow(("no reference station found in scrap -- %s@%s",
        pps->name,pps->fsptr->get_full_name()))
    }

    switch (prj->type) {

      case TT_2DPROJ_PLAN:
        cp = pps->fcpp;
        prj->shift_x = shift_x = minx; //(minx + maxx) / 2.0;
        prj->shift_y = shift_y = maxy; //(miny + maxy) / 2.0;
        prj->shift_z = shift_z = maxz; //(minz + maxz) / 2.0;
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
        while (cp != NULL) {
          cp->tx = cosa * (cp->st->x - shift_x) - sina * (cp->st->y - shift_y);
          cp->tz = sina * (cp->st->x - shift_x) + cosa * (cp->st->y - shift_y);
          cp->ty = cp->st->z - shift_z;
          cp->ta = cp->st->z;
          cp = cp->nextcp;
        }
        break;

      case TT_2DPROJ_EXTEND:
        // overi vsetky existujuce referencie predchadzajucich bodov
        cp = pps->fcpp;
        if (cp == NULL)
          break;
        while (cp != NULL) {
          if (cp->point->extend_point == NULL) {
            if (cp->point->extend_name.is_empty()) {
              if (cp->prevcp != NULL)
                cp->point->extend_point = cp->prevcp->point;
            } 
            else {                
              searchid = cp->point->extend_name.id;
              scancp = pps->fcpp;
              while (scancp != NULL) {
                if (scancp->point->station_name.id == searchid) {
                  cp->point->extend_point = scancp->point;
                  break;
                }
                scancp = scancp->nextcp;
              }                

              if (cp->point->extend_point == NULL) {
                searchid = this->db->db1d.station_vec[cp->point->extend_name.id - 1].uid;
                scancp = pps->fcpp;
                while (scancp != NULL) {
                  if (scancp->st->uid == searchid) {
                    cp->point->extend_point = scancp->point;
                    break;
                  }
                  scancp = scancp->nextcp;
                }
              }

              if (cp->point->extend_point == NULL) {
                cp->point->throw_source();
                if (cp->point->station_name.survey != NULL)
                  threthrow2(("no referenced station found -- %s@%s",
                    cp->point->extend_name.name,cp->point->extend_name.survey))
                else
                  threthrow2(("no referenced station found -- %s",
                    cp->point->extend_name.name))
              }
            }
          }
          cp = cp->nextcp;
        }
        
        // OK, let's find the root CP
        has_root = false;
        bool has_first_root = false;
        rootcp = pps->fcpp;
        cp = pps->fcpp;
        while (cp != NULL) {
          if (cp->point->extend_point == NULL) {
            rootcp = cp;
            has_root = true;
          }
          if ((!has_root) && (!has_first_root) && (cp->point->extend_name.is_empty())) {
            rootcp = cp;
            has_first_root = true;
          }
          if ((!has_root) && ((cp->point->extend_opts & TT_POINT_EXTEND_ROOT) != 0)) {
            has_root = true;
            rootcp = cp;
          }
          if ((cp->point->extend_opts & 
              (TT_POINT_EXTEND_LEFT | TT_POINT_EXTEND_RIGHT)) == 0) {
            if (cp->prevcp != NULL)
              cp->point->extend_opts |= (cp->prevcp->point->extend_opts & 
              (TT_POINT_EXTEND_LEFT | TT_POINT_EXTEND_RIGHT));
            else 
              cp->point->extend_opts |= TT_POINT_EXTEND_RIGHT;
          }
          cp = cp->nextcp;
        }
        
        // OK, let's calculate coordinates
        nattached = 1;
        rootcp->is_attached = true;
        rootcp->tx = 0.0;
        prj->shift_x = rootcp->st->x;
        rootcp->ty = 0.0;
        prj->shift_y = rootcp->st->y;
        rootcp->tz = 0.0;
        prj->shift_z = rootcp->st->z;
        rootcp->ta = rootcp->st->z;
        rootcp->is_sticky = rootcp->point->extend_opts & 
                (TT_POINT_EXTEND_STICKYON | TT_POINT_EXTEND_STICKYOFF);
        
        some_attached = true;
        while (some_attached) {
          some_attached = false;
          cp = pps->fcpp;
          while (cp != NULL) {
            if ((!cp->is_attached) &&
                (cp->point->extend_point->cpoint->is_attached)) {
              cp->is_attached = true;
              cp->used_in_attachement = true;
              //dx = (cp->st->x - cp->point->extend_point->cpoint->st->x);
              //dy = (cp->st->y - cp->point->extend_point->cpoint->st->y);
              dxy = hypot((cp->st->x - cp->point->extend_point->cpoint->st->x),
                (cp->st->y - cp->point->extend_point->cpoint->st->y));
              //sqrt(dx * dx + dy * dy);
              if (cp->point->extend_opts & TT_POINT_EXTEND_RIGHT)
                cp->tx = cp->point->extend_point->cpoint->tx + dxy;
              else
                cp->tx = cp->point->extend_point->cpoint->tx - dxy;
              cp->ty = cp->point->extend_point->cpoint->ty + 
                (cp->st->z - cp->point->extend_point->cpoint->st->z);
              cp->tz = 0.0;
              cp->ta = cp->st->z;
    
              cp->is_sticky = cp->point->extend_opts & 
                (TT_POINT_EXTEND_STICKYON | TT_POINT_EXTEND_STICKYOFF);
                 
              nattached++;
              some_attached = true;
            }
            cp = cp->nextcp;
          }
        }

        if (nattached < pps->ncp) {
          pps->throw_source();
          threthrow(("unable extend survey stations -- scrap %s@%s",
            pps->name,pps->fsptr->get_full_name()))
        }
        
        break;
        
    } // END of projection switch

    pps = pps->proj_next_scrap;
    numscraps++;
  }
    
  // postprocess if necessary
  if (prj->type == TT_2DPROJ_EXTEND) {
    
    std::list <thprjx_link> xscrap_links;
    thprjx_link * xscrap_link, * xscrap_link2;
    
    std::list <thprjx_station_link> xstation_links;
    thprjx_station_link * xstation_link, * xstation_link2;
    
    thprjx_scrap * xscraps = new thprjx_scrap [numscraps],
      * xscrap;

    unsigned long numstations = this->db->db1d.station_vec.size();
    thprjx_station * xstations = new thprjx_station [numstations],
      * xstation;
      
//    unsigned long cuid;
    
    pps = prj->first_scrap;
    xscrap = xscraps;
    while (pps != NULL) {
      xscrap->scrap = pps;
      pps->xscrap = xscrap;
      cp = pps->fcpp;
      while (cp != NULL) {
        if (cp->is_sticky != TT_POINT_EXTEND_STICKYOFF) {
          xstation = &(xstations[cp->st->uid - 1]);
          xstation_link = &(*(xstation_links.insert(xstation_links.end())));
          xstation_link->scrap = xscrap;
          xstation_link->scrapcp = cp;
          if (xstation->nstations == 0) {
            xstation->first_link = xstation_link;
            xstation->last_link = xstation_link;
          } else {
            xstation->last_link->next_link = xstation_link;
            xstation->last_link = xstation_link;
          }
        }
        xstation->nstations++;
        
        cp = cp->nextcp;
      }      
      pps = pps->proj_next_scrap;
      xscrap++;
    }
    
    // let's add scrap links from UID
    unsigned long station_id;
    station_id = 0;
    xstation = xstations;
    while (station_id < numstations) {
      if (xstation->nstations > 1) {
        xstation_link = xstation->first_link;
        while (xstation_link != NULL) {
          xstation_link2 = xstation_link->next_link;
          while (xstation_link2 != NULL) {
            if (xstation_link2->scrap->scrap->id != xstation_link->scrap->scrap->id) {
            
              // pridaj ->
              xscrap_link = &(*(xscrap_links.insert(xscrap_links.end())));
              xscrap_link->from_cp = xstation_link->scrapcp;
              xscrap_link->from_scrap = xstation_link->scrap;
              xscrap_link->to_cp = xstation_link2->scrapcp;
              xscrap_link->to_scrap = xstation_link2->scrap;
              if (xscrap_link->from_scrap->last_link == NULL) {
                xscrap_link->from_scrap->last_link = xscrap_link;
                xscrap_link->from_scrap->first_link = xscrap_link;
              } else {
                if ((xscrap_link->from_cp->is_sticky == TT_POINT_EXTEND_STICKYON) ||
                    (xscrap_link->to_cp->is_sticky == TT_POINT_EXTEND_STICKYON)) {
                  xscrap_link->from_scrap->first_link->prev_link = xscrap_link;
                  xscrap_link->next_link = xscrap_link->from_scrap->first_link;                  
                  xscrap_link->from_scrap->first_link = xscrap_link;
                } else {
                  xscrap_link->prev_link = xscrap_link->from_scrap->last_link;
                  xscrap_link->from_scrap->last_link->next_link = xscrap_link;
                  xscrap_link->from_scrap->last_link = xscrap_link;
                }
              }
              
              xscrap_link2 = xscrap_link;
              
              // pridaj <-
              xscrap_link = &(*(xscrap_links.insert(xscrap_links.end())));
              xscrap_link->from_cp = xstation_link2->scrapcp;
              xscrap_link->from_scrap = xstation_link2->scrap;
              xscrap_link->to_cp = xstation_link->scrapcp;
              xscrap_link->to_scrap = xstation_link->scrap;
              if (xscrap_link->from_scrap->last_link == NULL) {
                xscrap_link->from_scrap->last_link = xscrap_link;
                xscrap_link->from_scrap->first_link = xscrap_link;
              } else {
                if ((xscrap_link->from_cp->is_sticky == TT_POINT_EXTEND_STICKYON) ||
                    (xscrap_link->to_cp->is_sticky == TT_POINT_EXTEND_STICKYON)) {
                  xscrap_link->from_scrap->first_link->prev_link = xscrap_link;
                  xscrap_link->next_link = xscrap_link->from_scrap->first_link;                  
                  xscrap_link->from_scrap->first_link = xscrap_link;
                } else {
                  xscrap_link->prev_link = xscrap_link->from_scrap->last_link;
                  xscrap_link->from_scrap->last_link->next_link = xscrap_link;
                  xscrap_link->from_scrap->last_link = xscrap_link;
                }
              }
              
              xscrap_link2->oposite_link = xscrap_link;
              xscrap_link->oposite_link = xscrap_link2;
              
            }
            xstation_link2 = xstation_link2->next_link;
          }
          xstation_link = xstation_link->next_link;
        }
      }
      station_id++;
      xstation++;
    }
    
    
//    // let's add scrap links infront of all from JOINS
//    bool scan_first_one = true;
//    thjoin * cj = prj->first_join;
//    thdb2dcp * jcp1, * jcp2;
//    thdb2dji * jitm, * jitm2;
//    while (cj != NULL) {
//      jitm = cj->first_item;
//      while ((jitm != NULL) && (scan_first_one)) {
//        if ((jitm->object->get_class_id() == TT_POINT_CMD) &&
//            (((thpoint *)(jitm->object))->cpoint != NULL)) {
//          scan_first_one = false;
//          jcp1 = ((thpoint *)(jitm->object))->cpoint;
//          jitm2 = jitm->next_item;
//          while (jitm2 != NULL) {
//          
//            if ((jitm2->object->get_class_id() == TT_POINT_CMD) &&
//                (((thpoint *)(jitm2->object))->cpoint != NULL)) {
//              jcp2 = ((thpoint *)(jitm2->object))->cpoint;
//              
//              // mame 2 roozne xscrapy -> teraz ci su rozlicne
//              if (jcp1->point->fscrapptr->id != jcp2->point->fscrapptr->id) {
//
//                // 0. let's create link from 1 to 2
//                xscrap_link = &(*(xscrap_links.insert(xscrap_links.end())));
//                xscrap_link->from_cp = jcp1;
//                xscrap_link->from_scrap = jcp1->point->fscrapptr->xscrap;
//                xscrap_link->to_cp = jcp2;
//                xscrap_link->to_scrap = jcp2->point->fscrapptr->xscrap;
//
//                // 1. let's remove all links pointing to scrap 2
//                xscrap_link2 = xscrap_link->from_scrap->first_link;
//                while (xscrap_link2 != NULL) {
//                
//                  // remove oposite links
//                  if (xscrap_link2->oposite_link != NULL) {
//
//                    if (xscrap_link2->oposite_link->prev_link == NULL)
//                      xscrap_link2->oposite_link->from_scrap->first_link = xscrap_link2->oposite_link->next_link;
//                    else
//                      xscrap_link2->oposite_link->prev_link->next_link = xscrap_link2->oposite_link->next_link;
//
//                    if (xscrap_link2->oposite_link->next_link == NULL)
//                      xscrap_link2->oposite_link->from_scrap->last_link = xscrap_link2->oposite_link->prev_link;
//                    else
//                      xscrap_link2->oposite_link->next_link->prev_link = xscrap_link2->oposite_link->prev_link;                      
//                      
//                    xscrap_link2->oposite_link = NULL;
//
//                  }
//                  
//                  xscrap_link2 = xscrap_link2->next_link;
//                }
//                
//                // 2. let's add link to the first position
//                if (xscrap_link->from_scrap->last_link == NULL) {
//                  xscrap_link->from_scrap->last_link = xscrap_link;
//                  xscrap_link->from_scrap->first_link = xscrap_link;
//                } else {
//                  xscrap_link->from_scrap->first_link->prev_link = xscrap_link;
//                  xscrap_link->next_link = xscrap_link->from_scrap->first_link;                  
//                  xscrap_link->from_scrap->first_link = xscrap_link;
//                }
//                      
//              } // END OF LINK TRANSFORMATION
//            }
//            jitm2 = jitm2->next_item;
//          }
//        }
//        jitm = jitm->next_item;
//      }
//      cj = cj->proj_next_join;
//    }    
    
    // let's run TREMAUX on scraps
    unsigned long num_scrap_att = 0; 
    double movex, movey;
    xscrap = xscraps;
    while(1) {

      // 1 ak nie je xscrap pripojeny, pripoji ho
      if (!xscrap->is_attached) {
        if (xscrap->via_link != NULL) {
          movex = xscrap->via_link->from_cp->tx - xscrap->via_link->to_cp->tx;
          movey = xscrap->via_link->from_cp->ty - xscrap->via_link->to_cp->ty;
          cp = xscrap->scrap->fcpp;
          while (cp != NULL) {
            cp->tx = cp->tx + movex;
            cp->ty = cp->ty + movey;
            cp->tz = double(num_scrap_att);
            cp = cp->nextcp;
          }
        }
        xscrap->is_attached = true;
        num_scrap_att++;
      }

      // skontroluje ci mooze ist dalej
      while ((xscrap->first_link != NULL) && (xscrap->first_link->to_scrap->is_attached))
        xscrap->first_link = xscrap->first_link->next_link;

      // ak nie ide spat ak mooze inak break.
      if (xscrap->first_link != NULL) {
        xscrap->first_link->to_scrap->via_link = xscrap->first_link;
        xscrap = xscrap->first_link->to_scrap;
        xscrap->via_link->from_scrap->first_link = xscrap->via_link->from_scrap->first_link->next_link;
      } else {
        if (xscrap->via_link != NULL)
          xscrap = xscrap->via_link->from_scrap;
        else
          break;
      }
    }
    if (num_scrap_att < numscraps) {
      xscrap = xscraps;
      unsigned nxscrap = 0;
      thlog.printf("error info -- scraps not attached to extended");
      if (strlen(prj->index) == 0)
        thlog.printf(":%s",prj->index);
      thlog.printf("projection:\n");
      while(nxscrap < numscraps) {
        if (!xscrap->is_attached)
          thlog.printf("\t%s@%s\n",xscrap->scrap->name,xscrap->scrap->fsptr->get_full_name());
        xscrap++;
        nxscrap++;
      }
      if (strlen(prj->index) == 0)
        ththrow(("can not connect all scraps in exteded projection"))
      else
        ththrow(("can not connect all scraps in exteded:%s projection", prj->index))      
    }
    
    delete [] xscraps;
    delete [] xstations;
  } // END of extended projection post processing
  
}


void thdb2d::pp_scale_points(thdb2dprj * prj)
{
  // najprv prejde scrapy a nastavy matice
  thscrap * ps = prj->first_scrap;
  double maxdist,cdist,scale,ang,tang;
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
//  double tx,ty;
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
          } else if (ppoint->align != TT_POINT_ALIGN_C)
            thdb2d_rotate_align(ppoint->align,ps->mr);
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
    if ((*obi)->get_class_id() == TT_SCRAP_CMD) {
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
  thscrap * pscrap = prj->first_scrap;
  thdb2dcp * cp, * cp2, * acp1, * acp2;
  double maxdist, cdist, scale, ang, tang;
  while (pscrap != NULL) {
    pscrap->reset_transformation();
    if (pscrap->ncp == 1) {
      pscrap->mx = pscrap->fcpp->tx - pscrap->fcpp->pt->xt;
      pscrap->my = pscrap->fcpp->ty - pscrap->fcpp->pt->yt;
    } else {
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
      } else {
        pscrap->mx = pscrap->fcpp->tx - pscrap->fcpp->pt->xt;
        pscrap->my = pscrap->fcpp->ty - pscrap->fcpp->pt->yt;
      }
    }
    pscrap = pscrap->proj_next_scrap;
  }
  
  // prejde bod za bodom a spocita ich suradnice
  thdb2dpt_list::iterator ii = this->pt_list.begin();
  double tmpx, tmpy;
  while (ii != this->pt_list.end()) {
    if (ii->pscrap->proj->id == prj->id) {
      pscrap = ii->pscrap;
      tmpx = ii->xt;
      tmpy = ii->yt;
      ii->xt = pscrap->mxx * tmpx + pscrap->mxy * tmpy + pscrap->mx;
      ii->yt = pscrap->myx * tmpx + pscrap->myy * tmpy + pscrap->my;
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
            thdb2d_rotate_align(ppoint->align,pscrap->mr);
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


void thdb2d::pp_shift_points(thdb2dprj * prj, bool calc_az)
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
  double cpw, totalw, cdx, cdy, att, ztt, dist;
  while (ii != this->pt_list.end()) {
    if ((ii->pscrap->proj->id == prj->id) && (ii->pscrap->ncp > 0)) {
      cp = ii->pscrap->fcpp;
      totalw = 0.0;
      cdx = 0.0;
      cdy = 0.0;
      att = 0.0;
      ztt = 0.0;
      while (cp != NULL) {
        dist = hypot(cp->oxt - ii->xt, cp->oyt - ii->yt);
        if (dist > 0) {
          cpw = pow(dist,-2.0);
          cdx += cp->dx * cpw;
          cdy += cp->dy * cpw;
          if (calc_az) {
            att += cp->ta * cpw;
            ztt += cp->tz * cpw;
          }
          totalw += cpw;
        } else {
          cdx = cp->dx;
          cdy = cp->dy;
          if (calc_az) {
            att = cp->ta;
            ztt = cp->tz;
          }
          totalw = 1.0;
          break;
        }
        cp = cp->nextcp;
      }
      cdx = cdx / totalw;
      cdy = cdy / totalw;
      ii->xt = ii->xt + cdx;
      ii->yt = ii->yt + cdy;
      if (calc_az) {
        ii->at = att / totalw;
        ii->zt = ztt / totalw;
      }
    }
    ii++;
  }
    
}



void thdb2d::pp_process_joins(thdb2dprj * prj)
{
  // prejde join za joinom a vytovori zoznamy
  thjoin * jptr = prj->first_join, * tjptr;
  prj->first_join_list = NULL;
  prj->last_join_list = NULL;
  thscrap * sc1, * sc2;
  thdb2dji * ji, * tji;
  thdb2dpt * searchpt;
  thscrapen * se1, * se2, * fse1, * fse2;
  thline * l1, * l2;
  double mindst, cdst;
  unsigned long nactive;
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
          // z dvoch scrapov urobime body na ciarach
          sc1 = (thscrap *) ji->object;
          ji->is_active = false;
          if (ji->next_item != NULL) {
            ji = ji->next_item;
            sc2 = (thscrap *) ji->object;
            ji->is_active = false;
            // mame 2 scrapy, najdeme spolocne ciary
            fse1 = sc1->get_ends();
            fse2 = sc2->get_ends();
            if ((fse1 == NULL) || (fse2 == NULL))
              break;
              
            se1 = fse1;
            while (se1 != NULL) {
              se1->cxt = (se1->lp1->point->xt + se1->lp2->point->xt) / 2.0;
              se1->cyt = (se1->lp1->point->yt + se1->lp2->point->yt) / 2.0;
              se1 = se1->next_end;
            }

            se2 = fse2;
            while (se2 != NULL) {
              se2->cxt = (se2->lp1->point->xt + se2->lp2->point->xt) / 2.0;
              se2->cyt = (se2->lp1->point->yt + se2->lp2->point->yt) / 2.0;
              se2 = se2->next_end;
            }
            
            mindst = hypot(fse1->cxt - fse2->cxt, fse1->cyt - fse2->cyt);
            se1 = fse1;
            
            while (se1 != NULL) {
              se2 = sc2->get_ends();
              while (se2 != NULL) {
                cdst = hypot(se1->cxt - se2->cxt, se1->cyt - se2->cyt);
                if (cdst < mindst) {
                  fse1 = se1;
                  fse2 = se2;
                  mindst = cdst;
                }
                se2 = se2->next_end;
              }
              se1 = se1->next_end;
            }
            
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
            
          }
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
      thdb2dji * newlist;
      ppitem = target_item;
      ccitem = target_item->prev_item;
      while ((ccitem != NULL) || ideme_nahor) {
        if ((ccitem != NULL) && (ccitem->is_active)) {
          if (ccitem->point->join_item == NULL) {
            ccitem->point->join_item = target_item;
            ppitem->next_list_item = ccitem;
            ccitem->prev_list_item = ppitem;
            ppitem = ccitem;
          } else {
            newlist = ccitem->point->join_item;
            // najprv odstranime novy list zo zoznamu listov
            if (newlist->next_list == NULL)
              prj->last_join_list = newlist->prev_list;
            else
              newlist->next_list->prev_list = newlist->prev_list;
            newlist->next_list = NULL;            
            if (newlist->prev_list == NULL)
              prj->first_join_list = newlist->next_list;
            else
              newlist->prev_list->next_list = newlist->next_list;
            newlist->prev_list = NULL;
            // teraz ho prilinkujeme na ppitem
            newlist->prev_list_item = ppitem;
            ppitem->next_list_item = newlist;
            ppitem = newlist;
            ppitem->point->join_item = target_item;
            while (ppitem->next_list_item != NULL) {
              ppitem->next_item->point->join_item = target_item;
              ppitem = ppitem->next_item;
            }
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
  
  thdb2dji * jlist = prj->first_join_list;
  bool has_target;
  double tx, ty, tn;
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

  this->pp_shift_points(prj);
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
  double x1,x2,y1,y2,d1,d2,tmpx,tmpy,testx,testy,dt;
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


