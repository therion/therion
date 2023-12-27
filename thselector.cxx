/**
 * @file thselector.cxx
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
 
#include "thselector.h"
#include "thexception.h"
#include "thconfig.h"
#include <stdio.h>
#include <string.h>
#include "thparse.h"
#include "thdatabase.h"
#include "thdataobject.h"
#include "thsurvey.h"
#include "thscrap.h"
#include <vector>
#include "thchenc.h"
#include "thmap.h"

#include <fmt/printf.h>

thselector::thselector() {
  this->number = 0;
  this->cfgptr = NULL;
}


void thselector::assign_config(class thconfig * cptr) {
  this->cfgptr = cptr;
}


enum {
  TT_SELECTOR_RECURSIVE,
  TT_SELECTOR_MAP_LEVEL,
  TT_SELECTOR_CHAPTER_LEVEL,
  TT_SELECTOR_COLOR,
  TT_SELECTOR_UNKNOWN,
};

static const thstok thtt_selector_opts[] = {
  {"-chapter-level", TT_SELECTOR_CHAPTER_LEVEL},
  {"-color", TT_SELECTOR_COLOR},
  {"-colour", TT_SELECTOR_COLOR},
  {"-map-level", TT_SELECTOR_MAP_LEVEL},
  {"-recursive", TT_SELECTOR_RECURSIVE},
  {NULL, TT_SELECTOR_UNKNOWN}
};


void thselector::parse_selection (bool usid, int nargs, char ** args)
{

  double dum;
  int sv;
  thselector_item itm;
  itm.unselect = usid;
  if (nargs < 1)
    ththrow("not enough command arguments");

  // set object name
  if (strlen(*args) == 0)
    ththrow("empty object name not allowed");
  itm.name = this->cfgptr->get_str_buff()->append(*args);
  itm.src_name = this->cfgptr->get_db()->strstore(this->cfgptr->get_cfg_file()->get_cif_name(), true);
  itm.src_ln = this->cfgptr->get_cfg_file()->get_cif_line_number();
  itm.number = ++this->number;
  
  // set recursive option
  int aid;
  for(aid = 1; aid < nargs; aid++) {
    switch (thmatch_token(args[aid],thtt_selector_opts)) {

      case TT_SELECTOR_RECURSIVE:
        aid++;
        switch (thmatch_token(args[aid],thtt_bool)) {
          case TT_FALSE:
            itm.recursive = false;
            break;
          case TT_TRUE:
            itm.recursive = true;
            break;
          case TT_UNKNOWN_BOOL:
            ththrow("logical value expected -- {}", args[aid]);
        }
        break;

      case TT_SELECTOR_COLOR:
        aid++;
        itm.m_color.parse(args[aid]);
        break;

      case TT_SELECTOR_MAP_LEVEL:
        aid++;
        thparse_double(sv,dum,args[aid]);
        if ((sv != TT_SV_NUMBER) && (strcmp(args[aid],"basic") == 0)) {
          itm.map_level = -1;
          break;
        }
        if ((sv != TT_SV_NUMBER) || (dum < 0))
          ththrow("invalid map level -- {}", *args);
        if (double(int(dum)) != dum)
          ththrow("invalid map level -- {}", *args);
        itm.map_level = long(dum);
        break;

      case TT_SELECTOR_CHAPTER_LEVEL:
        aid++;
        thparse_double(sv,dum,args[aid]);
        if (sv == TT_SV_NAN) {
          itm.chapter_level = -1;
          break;
        }
        if ((sv != TT_SV_NUMBER) || (dum <= 0))
          ththrow("invalid chapter level -- {}", *args);
        if (double(int(dum)) != dum)
          ththrow("invalid chapter level -- {}", *args);
        itm.chapter_level = long(dum);
        break;
        
      default:
        ththrow("unknown option -- {}", args[aid]);
    }
  }
  
  this->data.push_back(itm);
  
}


class thselector_select_item {
  
  public:

  thdataobject * objp;
  const char * n1, * n2, * n3;
  
  void clear();
  
  thselector_select_item() {this->clear();}
  
  void parse (thdataobject * op);
  
  bool operator < (const thselector_select_item & ii) const;
  
};

void thselector_select_item::clear()
{
  this->n1 = "";
  this->n2 = this->n1;
  this->n3 = this->n1;
  this->objp = NULL;
}


bool thselector_select_item::operator < (const thselector_select_item & ii) const
{
  int cmp1 = strcmp(this->n2, ii.n2);
  if (cmp1 < 0)
    return true;
  else if (cmp1 == 0) {
    size_t l1 = strlen(this->n1);
    size_t l2 = strlen(ii.n1);
    if ((l1 == 0) && (l2 > 0))
      return true;
    if ((l1 > 0) && (l2 == 0))
      return false;
    int cmp2 = strcmp(this->n1, ii.n1);
    if (cmp2 < 0)
      return true;
    else
      return false;
  }
  else
    return false;
}


void thselector_select_item::parse (thdataobject * op)
{
  this->clear();
  thsurvey * sp;
  switch (op->get_class_id()) {
    case TT_GRADE_CMD:
    case TT_LAYOUT_CMD:
      return;
    case TT_SURVEY_CMD:
      sp = dynamic_cast<thsurvey*>(op);
      this->n1 = "";
      this->n2 = sp->get_reverse_full_name();
      this->n3 = sp->get_full_name();
      break;
    default:
      this->n1 = op->get_name();
      this->n2 = op->get_father_survey()->get_reverse_full_name();
      this->n3 = op->get_father_survey()->get_full_name();
  }
  this->objp = op;
}

typedef std::vector <thselector_select_item> thselector_siv;

void thselector_export_survey_tree_node (FILE * cf, unsigned long level, thdataobject * optr) {
  thsurvey * ss;
  while (optr != NULL) {
    if (optr->get_class_id() == TT_SURVEY_CMD) {
      ss = dynamic_cast<thsurvey*>(optr);
      fprintf(cf,"xth_cp_data_tree_insert %lu", ss->id);
      if (ss->fsptr != NULL)
        fprintf(cf," %lu %lu", ss->fsptr->id, level);
      else
        fprintf(cf," {} %lu",level);
      fprintf(cf," %s %s",ss->get_name(),ss->full_name);
      if (strlen(ss->get_title()) > 0) {
        thdecode_tex(&(thdb.buff_enc), ss->get_title());
        fprintf(cf," \"%s\"",thdb.buff_enc.get_buffer());
        thdecode(&(thdb.buff_enc),TT_ASCII,ss->get_title());
        thdecode_tex(&(thdb.buff_tmp), thdb.buff_enc.get_buffer());
        fprintf(cf," \"%s\"",thdb.buff_tmp.get_buffer());
      }
      else
        fprintf(cf," \"\" \"\"");
      // nakoniec statistika
      fprintf(cf,
          " \"length: %.2fm (surface %.2fm, duplicated %.2fm)"
          ,ss->stat.length,ss->stat.length_surface,ss->stat.length_duplicate);
      if (ss->stat.station_top != NULL) {
        fprintf(cf,"\\n"
        "vertical range: %.2fm (from %s@%s at %.2fm to %s@%s at %.2fm)\\n"
        "north-south range: %.2fm (from %s@%s at %.2fm to %s@%s at %.2fm)\\n"
        "east-west range: %.2fm (from %s@%s at %.2fm to %s@%s at %.2fm)\\n"
        "number of shots: %lu\\n"
        "number of stations: %lu"
        "\"\n",
        ss->stat.station_top->z - ss->stat.station_bottom->z,
        ss->stat.station_top->name, ss->stat.station_top->survey->full_name,
        ss->stat.station_top->z,
        ss->stat.station_bottom->name, ss->stat.station_bottom->survey->full_name,
        ss->stat.station_bottom->z,
        ss->stat.station_north->y - ss->stat.station_south->y,
        ss->stat.station_north->name, ss->stat.station_north->survey->full_name,
        ss->stat.station_north->y,
        ss->stat.station_south->name, ss->stat.station_south->survey->full_name,
        ss->stat.station_south->y,
        ss->stat.station_east->x - ss->stat.station_west->x,
        ss->stat.station_east->name, ss->stat.station_east->survey->full_name,
        ss->stat.station_east->x,
        ss->stat.station_west->name, ss->stat.station_west->survey->full_name,
        ss->stat.station_west->x,
        ss->stat.num_shots, ss->stat.num_stations
        );
      } else {
        fprintf(cf,"\"\n");
      }
      if (ss->foptr != NULL)
        thselector_export_survey_tree_node(cf,level+1,ss->foptr);
    }
    optr = optr->nsptr;
  }
}

void thselector_prepare_map_tree_export (thdatabase * db) {

  // prejde vsetky mapy a surveye a nastavi tmp_bool na true a tmp_ulong na 0
  thdb_object_list_type::iterator obi = db->object_list.begin();
  while (obi != db->object_list.end()) {
    (*obi)->tmp_bool = true;
    (*obi)->tmp_ulong = 0;
    obi++;
  }

  // prejde vsetky mapy a objektom v nich nastavi (tmp_bool) na false
  thdb2dmi * mi;
  obi = db->object_list.begin();
  while (obi != db->object_list.end()) {
    if (((*obi)->fsptr != NULL) && ((*obi)->get_class_id() == TT_MAP_CMD)) {
      mi = dynamic_cast<thmap*>(obi->get())->first_item;
      while (mi != NULL) {
        if (mi->type == TT_MAPITEM_NORMAL) {
          mi->object->tmp_bool = false;
        }
        mi = mi->next_item;
      }
    }
    obi++;
  }
  
}

void thselector_export_map_tree_node (FILE * cf, unsigned long level, unsigned long pass, thdataobject * optr, thmap * fmap) {
  if (optr->tmp_ulong == pass)
    return;
  if (optr->fsptr == NULL)
    return;
  optr->tmp_ulong = pass;
  thmap * mptr = NULL;
  int subtype = 0;
  const char * types = NULL;
  switch (optr->get_class_id()) {
    case TT_MAP_CMD:
      mptr = dynamic_cast<thmap*>(optr); //id fid level
      types = "map";
      if (mptr->is_basic)
        subtype = 1;
      break;
    default:
      //case TT_SCRAP_CMD:
      //types = "scrap";
      //do not export scraps
      return;
      break;
    //default:
    //  break;
  }
  dynamic_cast<thmap*>(optr)->nz++;
  fprintf(cf,"xth_cp_map_tree_insert %s %d %luX%lu",types,subtype,optr->id,dynamic_cast<thmap*>(optr)->nz);
  if (fmap == NULL)
    fprintf(cf," p%d %lu",dynamic_cast<thmap*>(optr)->projection_id,level);
  else
    fprintf(cf," %luX%lu %lu",fmap->id,fmap->nz,level);

  fprintf(cf," %s %s%s%s",optr->get_name(),optr->get_name(), (strlen(optr->fsptr->full_name) > 0 ? "@" : ""), optr->fsptr->full_name);
  if (strlen(optr->get_title()) > 0) {
    thdecode_tex(&(thdb.buff_enc), optr->get_title());
    fprintf(cf," \"%s\"",thdb.buff_enc.get_buffer());
    thdecode(&(thdb.buff_enc),TT_ASCII,optr->get_title());
    thdecode_tex(&(thdb.buff_tmp), thdb.buff_enc.get_buffer());
    fprintf(cf," \"%s\"\n",thdb.buff_tmp.get_buffer());
  }
  else
    fprintf(cf," \"\" \"\"\n");
  
  thdb2dmi * mi;
  if (mptr != NULL) {
    mi = mptr->first_item;
    while (mi != NULL) {
      if (mi->type == TT_MAPITEM_NORMAL) {
        thselector_export_map_tree_node(cf, level+1, pass, mi->object, mptr);
      }
      mi = mi->next_item;
    }
  }
}

void thselector::dump_selection_db (FILE * cf, thdatabase * db)
{
  
  // exportuje strom surveyov
  fprintf(cf,"set xth(ctrl,cp,datlist) {}\n");
  if (db->fsurveyptr != NULL) 
    thselector_export_survey_tree_node(cf,0,db->fsurveyptr);
  fprintf(cf,"xth_cp_data_tree_create\n");
  
  // exportuje strom map po vsetkych projekciach
  fprintf(cf,"set xth(ctrl,cp,maplist) {}\n");
  // exportuje vsetky projekcie
  thdb2dprj_list::iterator prjli = db->db2d.prj_list.begin();
  while (prjli != db->db2d.prj_list.end()) {
	  std::string projdir;
		if ((prjli->type == TT_2DPROJ_ELEV) && (prjli->pp1 != 0.0)) {
			projdir = fmt::sprintf("\\[%.1f\\]", prjli->pp1);
		}
    fprintf(cf,"xth_cp_map_tree_insert projection 0 p%d {} 0",prjli->id); 
    if (strlen(prjli->index) > 0)
      fprintf(cf," %s%s:%s",thmatch_string(prjli->type,thtt_2dproj), projdir.c_str(), prjli->index);
    else
      fprintf(cf," %s%s",thmatch_string(prjli->type,thtt_2dproj), projdir.c_str());
    fprintf(cf," {} {} {}\n");
    prjli++;
  }
  // exportuje vsetky mapy a scrapy
  thselector_prepare_map_tree_export(db);
  thdb_object_list_type::iterator obi = db->object_list.begin();
  while (obi != db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_MAP_CMD)
      dynamic_cast<thmap*>(obi->get())->nz = 0;
    obi++;
  }

  obi = db->object_list.begin();
  while (obi != db->object_list.end()) {
    if (((*obi)->fsptr != NULL) && ((*obi)->get_class_id() == TT_MAP_CMD) && dynamic_cast<thmap*>(obi->get())->tmp_bool)
      thselector_export_map_tree_node(cf,1,(*obi)->id,obi->get(),NULL);
    obi++;
  }
  
  fprintf(cf,"xth_cp_map_tree_create\n");
  
}


void thselector::dump_selection (FILE * cf)
{
  thselector_list::iterator ii = this->data.begin();
  if ((ii != this->data.end()) && (!this->cfgptr->get_comments_skip())) 
    fprintf(cf,"# Objects selected for output.\n");
  while (ii != this->data.end()) {
    if (ii->unselect)
      fprintf(cf,"unselect");
    else
      fprintf(cf,"select");
    fprintf(cf," %s", ii->name);
    if (ii->recursive == false)
      fprintf(cf," -recursive off");
    if (ii->map_level != 0) {
      if (ii->map_level < 0)
        fprintf(cf," -map-level basic");
      else
        fprintf(cf," -map-level %ld", ii->map_level);
    }
    if (ii->chapter_level != 0) {
      if (ii->chapter_level < 0)
        fprintf(cf," -chapter-level -");
      else
        fprintf(cf," -chapter-level %ld", ii->chapter_level);
    }
    fprintf(cf,"\n");
    ii++;
  }
  ii = this->data.begin();
  if (ii != this->data.end())
    fprintf(cf,"\n");
}

void thselector::select_db(class thdatabase * db)
{
  thmbuffer mbf;
  char ** nms, * nobj = NULL, * nsrv = NULL;
  int nn = 0;
  //thsurvey * objsrv;
  thdataobject * objptr = NULL;
  bool to_cont = false, select_all = false, has_selected_survey = false;
  thselector_list::iterator ii = this->data.begin();
  if (this->data.empty()) {
    thselector_item xitm;
    xitm.unselect = false;
    this->select_all(&xitm, db);
    return;
  }
  while (ii != this->data.end()) {
  
    if (ii->unselect && (ii == this->data.begin())) {
      thselector_item xitm;
      xitm.unselect = false;
      this->select_all(&xitm, db);
      has_selected_survey = true;
    }
  
    // let's separate name & survey
    to_cont = true;
    select_all = false;
    thsplit_strings(& mbf, ii->name, '@');
    nms = mbf.get_buffer();
    nn = mbf.get_size();
    switch (nn) {
      case 1:
        nobj = *nms;
        nsrv = NULL;
        break;
      case 2:
        nobj = nms[0];
        nsrv = nms[1];
        break;
      default:
        thwarning(("%s [%lu] -- invalid object specification", 
          ii->src_name, ii->src_ln))
        to_cont = false;
    }
    
    if (!to_cont) {
      ii++;
      continue;
    }
    
    if ((nsrv == NULL) && ((strlen(nobj) == 0) || (strcmp(nobj,"*") == 0))) {
      select_all = true;
    }
    
    if ((!select_all)) {
      try {
        objptr = db->get_object(thobjectname(nobj,nsrv),NULL);
      }
      catch(...) {
        objptr = NULL;
      }
      if (objptr == NULL) {
        thwarning(("%s [%lu] -- object not found -- \"%s\"", 
          ii->src_name, ii->src_ln, nobj))
        to_cont = false;
      }
    }

    if (!to_cont) {
      ii++;
      continue;
    }
    
    if (select_all) {
      this->select_all(&(*ii), db);
      has_selected_survey = true;
    }
    else {
      ii->optr = objptr;
      switch (objptr->get_class_id()) {
        case TT_SURVEY_CMD:
          this->select_survey(&(*ii), dynamic_cast<thsurvey*>(objptr));
          has_selected_survey = true;
          break;
        default:
          this->select_object(&(*ii), objptr);
          break;
      }
    }
    
    ii++;
  } // cycle of select and unselect options
  
  if (!has_selected_survey) {
    thselector_item xitm;
    xitm.unselect = false;
    this->select_all(&xitm, db);
  }
  
}

void thselector::select_object(thselector_item * pitm, class thdataobject * optr)
{
  if (pitm->unselect) {
    optr->selected = false;
    optr->selected_number = pitm->number;
    optr->selected_color.defined = 0;
  } else {
    optr->selected = true;
    optr->selected_number = pitm->number;
    optr->selected_color = pitm->m_color;
  }
}

  
void thselector::select_all(thselector_item * pitm, class thdatabase * db)
{
  thdb_object_list_type::iterator ii = db->object_list.begin();
  while (ii != db->object_list.end()) {
    switch((*ii)->get_class_id()) {
      case TT_SURVEY_CMD:
        this->select_object(pitm,ii->get());
      default:
        break;
    }
    ii++;
  }
}
  

void thselector::select_survey(thselector_item * pitm, class thsurvey * srv)
{
  // oznaci vsety pod nim
  thdataobject * cptr = srv->get_first_survey_object();
  while (cptr != NULL) {
    if ((cptr->get_class_id() == TT_SURVEY_CMD) && pitm->recursive)
      this->select_survey(pitm, dynamic_cast<thsurvey*>(cptr));
    //else {
    //  this->select_object(pitm, cptr);
    //}
    cptr = cptr->nsptr;
  }
  // oznaci samotne survey
  this->select_object(pitm, (thdataobject *) srv);
}


