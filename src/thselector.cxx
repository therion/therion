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
#include "th2ddataobject.h"
#include "thscrap.h"
#include <vector>
#include <algorithm>


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
  TT_SELECTOR_UNKNOWN,
};

static const thstok thtt_selector_opts[] = {
  {"-chapter-level", TT_SELECTOR_CHAPTER_LEVEL},
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
    ththrow(("not enought command arguments"))

  // set object name
  if (strlen(*args) == 0)
    ththrow(("empty object name not allowed"))
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
            ththrow(("logical value expected -- %s", args[aid]))
        }
        break;

      case TT_SELECTOR_MAP_LEVEL:
        aid++;
        thparse_double(sv,dum,args[aid]);
        if (sv == TT_SV_NAN) {
          itm.map_level = -1;
          break;
        }
        if ((sv != TT_SV_NUMBER) || (dum <= 0))
          ththrow(("invalid map level -- %s", *args))
        if (double(int(dum)) != dum)
          ththrow(("invalid map level -- %s", *args))
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
          ththrow(("invalid chapter level -- %s", *args))
        if (double(int(dum)) != dum)
          ththrow(("invalid chapter level -- %s", *args))
        itm.chapter_level = long(dum);
        break;
        
      default:
        ththrow(("unknown option -- %s", args[aid]))
    }
  }
  
  this->data.push_back(itm);
  
}


class thselector_select_item {
  
  public:

  thdataobject * objp;
  char * n1, * n2, * n3;
  
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
      sp = (thsurvey *) op;
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

void thselector::dump_selection_db (FILE * cf, thdatabase * db)
{
  if (this->cfgptr->get_comments_skip())
    return;

  thselector_siv items;
  thselector_select_item itm;
  thdb_object_list_type::iterator ii;
  bool to_insert;
  for(ii = db->object_list.begin(); ii != db->object_list.end(); ii++) {
    to_insert = false;
    switch ((*ii)->get_class_id()) {
      case TT_SURVEY_CMD:
        to_insert = true;
        break;
      case TT_GRADE_CMD:
      case TT_LAYOUT_CMD:
        to_insert = false;
        break;  
      default:
        if (strlen((*ii)->get_name()) > 0)
          to_insert = true;
    }
    if (to_insert) {
      itm.parse(*ii);
      
      items.push_back(itm);
    }
  }
  
  // zoradi objekty
  // sort(items.begin(), items.end());
  
  if (items.empty())
    return;

  fprintf(cf,"##XTHERION## ##BEGIN##\n");
  thselector_siv::iterator itmi;
  for(itmi = items.begin(); itmi != items.end(); itmi++) {
    fprintf(cf,"##XTHERION## xth_cp_data_tree_insert %lu", itmi->objp->id);
    if (itmi->objp->fsptr != NULL) {
      // skusi ci neni v nejakom scrape
      if (itmi->objp->is(TT_2DDATAOBJECT_CMD)) {
        fprintf(cf," %lu", ((th2ddataobject*)(itmi->objp))->fscrapptr->id);
      } else {
        fprintf(cf," %lu", itmi->objp->fsptr->id);
      }
    } else {
      fprintf(cf," {}");
    }
    fprintf(cf," %s %s",itmi->objp->get_cmd_name(),itmi->objp->get_name());
    if (strlen(itmi->n1) > 0)
      fprintf(cf," %s@%s", itmi->n1, itmi->n3);
    else
      fprintf(cf," %s", itmi->n3);
    if (strlen(itmi->objp->get_title()) > 0) {
      thdecode_tex(&(thdb.buff_enc), itmi->objp->get_title());
      fprintf(cf," {%s}\n",thdb.buff_enc.get_buffer());
    }
    else
      fprintf(cf," {}\n");
    
  }
  fprintf(cf,"##XTHERION## ##END##\n");
  fprintf(cf,"\n");
  
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
        fprintf(cf," -map-level -");
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
  char ** nms, * nobj, * nsrv;
  int nn;
  thsurvey * objsrv;
  thdataobject * objptr;
  bool to_cont, select_all, has_selected_survey = false;
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
        nobj = "";
        nsrv = *nms;
        break;
      case 2:
        nobj = nms[0];
        nsrv = nms[1];
        break;
      default:
        thwarning(("%s [%d] -- invalid object specification", 
          ii->src_name, ii->src_ln))
        to_cont = false;
    }
    
    if (!to_cont) {
      ii++;
      continue;
    }
    
    if ((strlen(nsrv) == 0) || (strcmp(nsrv,"*") == 0)) {
      select_all = true;
    }
    else {
      // let's find the survey
      try {
        objsrv = db->get_survey(nsrv,NULL);
        objptr = objsrv;
      }
      catch(...) {
        objsrv = NULL;
      }
      if (objsrv == NULL) {
        thwarning(("%s [%d] -- survey not found -- \"%s\"", 
          ii->src_name, ii->src_ln, nsrv))
        to_cont = false;
      }
    } 
    
    if (to_cont && (nn == 2)) {
      try {
        objptr = db->get_object(thobjectname(nobj,nsrv),NULL);
      }
      catch(...) {
        objptr = NULL;
      }
      if (objptr == NULL) {
        thwarning(("%s [%d] -- object not found -- \"%s\"", 
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
          this->select_survey(&(*ii), (thsurvey *) objptr);
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
  } else {
    optr->selected = true;
    optr->selected_number = pitm->number;
  }
}

  
void thselector::select_all(thselector_item * pitm, class thdatabase * db)
{
  thdb_object_list_type::iterator ii = db->object_list.begin();
  while (ii != db->object_list.end()) {
    switch((*ii)->get_class_id()) {
      case TT_SURVEY_CMD:
        this->select_object(pitm,*ii);
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
      this->select_survey(pitm, (thsurvey *) cptr);
    //else {
    //  this->select_object(pitm, cptr);
    //}
    cptr = cptr->nsptr;
  }
  // oznaci samotne survey
  this->select_object(pitm, (thdataobject *) srv);
}


