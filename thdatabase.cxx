/**
 * @file thdatabase.cxx
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
 

#include "thdatabase.h"
#include "thsurvey.h"
#include "thendsurvey.h"
#include "thobjectid.h"
#include "thexception.h"
#include "thdata.h"
#include "thcmdline.h"
#include "thparse.h"
#include "thdatastation.h"
#include "thgrade.h"
#include "thlayout.h"
#include "thscrap.h"
#include "th2ddataobject.h"
#include "thscrap.h"
#include "thpoint.h"
#include "thline.h"
#include "tharea.h"
#include "thjoin.h"
#include "thmap.h"
#include "thsurface.h"
#include "thendscrap.h"


const char * thlibrarydata_init_text =
  "/**\n"
  " * @file librarydata.cxx\n"
  " *\n"
  " * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!\n"
  " */\n\n\n"
  "#include \"thdatabase.h\"\n"
  "#include \"thobjectsrc.h\"\n"
  "#include \"thinfnan.h\"\n"
  "#include \"thchencdata.h\"\n"
  "#include \"thlayout.h\"\n"
  "#include \"thlang.h\"\n"
  "#include \"thgrade.h\"\n\n\n";

const char * thlibrarydata_grades_text =
  "void thlibrary_init_grades()\n"
  "{\n"
  "\tthgrade * pgrade;\n"
  "\tchar * oname;\n";


const char * thlibrarydata_layouts_text =
  "void thlibrary_init_layouts()\n"
  "{\n"
  "\tthlayout * playout;\n"
  "\tchar * oname;\n";


bool operator < (const class thsurveyname & n1, const class thsurveyname & n2)
{
  if (strcmp(n1.n,n2.n) < 0)
    return true;
  else 
    return false;  
}


bool operator < (const class threvision & r1, const class threvision & r2)
{
  if (r1.id < r2.id)
    return true;
  else if (r1.id == r2.id) {
    if (r1.r < r2.r)
      return true;
  }
  return false;
}


void thdatabase::reset_context()
{
  this->ccontext = THCTX_NONE;
  this->objid = 0;
  this->nscraps = 0;
  
  this->fsurveyptr = NULL;
  this->csurveyptr = NULL;
  this->lcsobjectptr = NULL;
  
  this->cscrapptr = NULL;
  this->lcscrapoptr = NULL;
}


thdatabase::thdatabase()
{
  this->reset_context();
  this->db1d.assigndb(this);
  this->db2d.assigndb(this);
}

thdatabase::~thdatabase()
{
  this->clear(); 
}

void thdatabase::clear()
{

  // let's delete all objects and clear object list
  thdb_object_list_type::iterator oli = this->object_list.begin();
  while (oli != this->object_list.end()) {
    (*oli)->self_delete();
    oli++;
  }
  this->object_list.clear();
  
  // clear search hashes
  this->object_map.clear();
  this->survey_map.clear();  
  
  // reset other variables
  this->reset_context();
}

void thdatabase::check_context(class thdataobject * optr) {
  if (optr == NULL)
    return;
  // Let's check the object context
  char * cmdname = optr->get_cmd_name();
  char * misscmd = "some";
  if ((optr->get_context() & this->ccontext) == 0) {
    switch (optr->get_context()) {
      case (THCTX_SURVEY | THCTX_SCRAP):
        misscmd = "survey or scrap";
        break;
      case THCTX_SURVEY:
        misscmd = "survey";
        break;
      case THCTX_SCRAP:
        misscmd = "scrap";
        break;
    }
    optr->self_delete();
    ththrow(("missing %s command before %s command", misscmd, cmdname));
  }
}

void thdatabase::insert(thdataobject * optr)
{

  thsurvey * survey_optr;
  thscrap * scrap_optr;
  th2ddataobject * th2ddo_optr;
  char * optr_name;
  int optr_class_id;
  unsigned long csurvey_id = (this->csurveyptr == NULL) ? 0 : csurveyptr->id;
  
  // Call start_insert object method.
  optr->start_insert();
  this->check_context(optr);
  
  // Let's take case of insertion of special objects
  bool is_special_o = false;
  switch (optr->get_class_id()) {
    case TT_GRADE_CMD:
      this->insert_grade((thgrade *) optr);
      is_special_o = true;
      break;
    case TT_LAYOUT_CMD:
      this->insert_layout((thlayout *) optr);
      is_special_o = true;
      break;
  }
  if (is_special_o)
    return;
  
  optr_class_id = optr->get_class_id();
  // Let's check, if name already not defined.

  if ((strlen(optr->name) > 0) && 
      (optr_class_id != TT_ENDSURVEY_CMD) &&
      (optr_class_id != TT_ENDSCRAP_CMD)) {
    if (this->object_map.find(thobjectid(optr->name,csurvey_id)) !=
        this->object_map.end()) {
      optr_name = optr->get_name();  
      optr->self_delete();
      ththrow(("duplicate object name -- %s", optr_name));
    }
  }
  
  // First let's select objects that have to be deleted (end...)
  switch (optr_class_id) {
  
    // end of survey
    case TT_ENDSURVEY_CMD:
      // let's check, if name of endsurvey is correct
      if (strlen(optr->name) > 0) {
        if (strcmp(optr->name, csurveyptr->name) != 0)
          ththrow(("survey - endsurvey names don't match -- %s - %s",
            csurveyptr->name, optr->name))
      }
      // whether csurvey is not the first one
      if (csurveyptr->fsptr == NULL) {
        this->lcsobjectptr = csurveyptr;
        this->ccontext = THCTX_NONE;
        this->csurveyptr = NULL;
      }
      else {
        this->csurveyptr = csurveyptr->fsptr;
        this->lcsobjectptr = csurveyptr->loptr;
      }
      optr->self_delete();
      break;
      
    // end of scrap
    case TT_ENDSCRAP_CMD:
      // let's check, if name of endsurvey is correct
      if (strlen(optr->name) > 0) {
        if (strcmp(optr->name, cscrapptr->name) != 0)
          ththrow(("scrap - endscrap names don't match -- %s - %s",
            cscrapptr->name, optr->name))
      }
      this->ccontext = THCTX_SURVEY;
      this->cscrapptr = NULL;
      this->lcscrapoptr = NULL;
      optr->self_delete();
      break;
  
    // other objects that has to be inserted into database
    default:

      // let's insert object into database
      this->object_list.push_back(optr);
      this->object_map[thobjectid(optr->name, csurvey_id)] = optr;
      
      if (this->lcsobjectptr != NULL) {
        this->lcsobjectptr->nsptr = optr;
        optr->psptr = this->lcsobjectptr;
      }
      this->lcsobjectptr = optr;
      
      if(this->csurveyptr != NULL) {
        optr->fsptr = this->csurveyptr;
        if (this->csurveyptr->foptr == NULL)
          this->csurveyptr->foptr = optr;
        this->csurveyptr->loptr = optr;
      }
      
      // let's take care of 2d object
      if(optr->is(TT_2DDATAOBJECT_CMD)) {
        th2ddo_optr = (th2ddataobject *) optr;
        
        if (this->lcscrapoptr != NULL)
          this->lcscrapoptr->nscrapoptr = th2ddo_optr;
        else
          this->cscrapptr->fs2doptr = th2ddo_optr;
          
        th2ddo_optr->pscrapoptr = this->lcscrapoptr;
        this->cscrapptr->ls2doptr = th2ddo_optr;
        th2ddo_optr->fscrapptr = this->cscrapptr;
        
        this->lcscrapoptr = th2ddo_optr;
        
      }

      // let's take care of special objects
      switch (optr_class_id) {
      
        case TT_SURVEY_CMD:
          survey_optr = (thsurvey *) optr;
          
          if (this->fsurveyptr == NULL) {
            this->fsurveyptr = survey_optr;
          }
          
          if (this->csurveyptr == NULL) {
            this->ccontext = THCTX_SURVEY;
            survey_optr->full_name = survey_optr->name;
            survey_optr->reverse_full_name = survey_optr->name;
          }
          else {
            this->buff_tmp.strcpy(survey_optr->name);
            this->buff_tmp.strcat(".");
            this->buff_tmp.strcat(this->csurveyptr->full_name);
            survey_optr->full_name = this->strstore(this->buff_tmp.get_buffer());
            survey_optr->reverse_full_name = this->strstore(survey_optr->full_name);
            survey_optr->full_name_reverse();
          }

          this->survey_map[thsurveyname(survey_optr->full_name)] = survey_optr;
          this->csurveyptr = survey_optr;
          this->lcsobjectptr = survey_optr->loptr;
          break;
          
        case TT_SCRAP_CMD:
          scrap_optr = (thscrap *) optr;
          this->ccontext = THCTX_SCRAP;
          this->cscrapptr = scrap_optr;
          this->nscraps++;
          break;  
        
      }  // end of special objects insertion
      
  }  // end of other objects
  
}  // end of object inserion


thsurvey * thdatabase::get_survey(char * sn, thsurvey * ps)
{
  thdb_survey_map_type::iterator si;
  
  if (sn != NULL) {
    this->buff_tmp.strcpy(sn);
    if (ps != NULL) {
      this->buff_tmp.strcat(".");
      this->buff_tmp.strcat(ps->full_name);
    }
    si = this->survey_map.find(thsurveyname(this->buff_tmp.get_buffer()));
    if (si != this->survey_map.end())
      return si->second;
    else
      ththrow(("survey does not exist -- %s", sn))
  }
  else
    return ps;
}


unsigned long thdatabase::get_survey_id(char * sn, thsurvey * ps)
{
  thsurvey * sptr = this->get_survey(sn, ps);
  if (sptr == NULL)
    return 0;
  else
    return sptr->id;
}


thdataobject * thdatabase::get_object(thobjectname on, thsurvey * ps)
{
  unsigned long csurvey_id = this->get_survey_id(on.survey, ps);
  thdb_object_map_type::iterator oi;
  
  oi = this->object_map.find(thobjectid(on.name,csurvey_id));
  if (oi == this->object_map.end())
    return (thdataobject *) NULL;
  else
    return oi->second;    
}


bool thdatabase::insert_datastation(thobjectname on, thsurvey * ps)
{
  static thdatastation ds;
  unsigned long csurvey_id = this->get_survey_id(on.survey, ps);
  
  if (this->object_map.find(thobjectid(on.name,csurvey_id)) !=
      this->object_map.end())
    return false;
  
  this->object_map[thobjectid(on.name, csurvey_id)] = & ds;
  return true;
      
}  // end of datastation inserion


class thdataobject * thdatabase::create(char * oclass, 
  thobjectsrc osrc)
{
  int tclass = thmatch_token(oclass, thtt_commands);
  thdataobject * ret;
  switch (tclass) {
  
    case TT_SURVEY_CMD:
      ret = new thsurvey;
      break;
      
    case TT_ENDSURVEY_CMD:
      ret = new thendsurvey;
      break;
      
    case TT_SCRAP_CMD:
      ret = new thscrap;
      break;
      
    case TT_ENDSCRAP_CMD:
      ret = new thendscrap;
      break;
      
    case TT_DATA_CMD:
      ret = new thdata;
      break;
      
    case TT_GRADE_CMD:
      ret = new thgrade;
      break;
      
    case TT_LAYOUT_CMD:
      ret = new thlayout;
      break;
      
    case TT_POINT_CMD:
      ret = new thpoint;
      break;
      
    case TT_LINE_CMD:
      ret = new thline;
      break;
      
    case TT_AREA_CMD:
      ret = new tharea;
      break;
      
    case TT_JOIN_CMD:
      ret = new thjoin;
      break;
      
    case TT_MAP_CMD:
      ret = new thmap;
      break;
      
    case TT_SURFACE_CMD:
      ret = new thsurface;
      break;
      
    default:
      ret = NULL;
  }
  
  if (ret != NULL) {
    ret->assigndb(this);
    
    // set object id and mark revision
    ret->id = ++this->objid;
    this->revision_set.insert(threvision(ret->id, 0, osrc));

  }
  return ret;
}


char * thdatabase::strstore(char * src, bool use_dic)
{
  if (use_dic) {
    thdb_dictionary_type::iterator ii = 
        this->dictionary.find(thsurveyname(src));
    if (ii != this->dictionary.end())
      return ii->n;
    else {
      ii = this->dictionary.insert(thsurveyname(this->buff_strings.append(src))).first;
      return ii->n;
    }
  }
  else
    return this->buff_strings.append(src);
}


void thdatabase::end_insert()
{
  if (this->ccontext != THCTX_NONE) {
    thdb_revision_set_type::iterator ii = 
        this->revision_set.find(threvision(this->csurveyptr->id, 0));
    therror(("%s [%d] -- incomplete survey - endsurvey pair -- %s",
      ii->srcf.name, ii->srcf.line, this->csurveyptr->full_name))
    }
}


class thdataobject * thdatabase::revise(char * nn, class thsurvey * fathersptr,
    thobjectsrc osrc)
{
  char ** divname;
  char * objname, * sfullname;
  unsigned long fsid;
  thdb_survey_map_type::iterator iii;
  thdb_object_map_type::iterator jjj;
  thsplit_strings(& this->mbuff_tmp, nn, '@');
  if ((this->mbuff_tmp.get_size() == 2)) {
    divname = this->mbuff_tmp.get_buffer();
    objname = divname[0];
    this->buff_tmp.strcpy(divname[1]);
    if (fathersptr != NULL) {
      this->buff_tmp.strcat(".");
      this->buff_tmp.strcat(fathersptr->full_name);
    }
    sfullname = this->buff_tmp.get_buffer();
    iii = this->survey_map.find(thsurveyname(sfullname));
    if (iii == this->survey_map.end())
      ththrow(("unknown survey -- %s",nn))
    else {
      fsid = iii->second->id;
    }
  }
  else {
    objname = nn;
    if (fathersptr != NULL)
      fsid = fathersptr->id;
    else
      fsid = 0;
  }

  jjj = this->object_map.find(thobjectid(objname,fsid));
  if (jjj == this->object_map.end()) {
/*    jjj = this->object_map.begin();
    while (jjj != this->object_map.end()) {
      thprintf("%d:%s\n",jjj->first.sid,jjj->first.name);
      jjj++;
    }*/
    return NULL;
  }  
  else {
    jjj->second->revision++;
    this->revision_set.insert(threvision(jjj->second->id,
        jjj->second->revision, osrc));
    return jjj->second;
  }  
 
}

void thdatabase::self_print(FILE * outf)
{
  thdb_object_list_type::iterator oli = this->object_list.begin();
  while (oli != this->object_list.end()) {
    fprintf(outf, "\n");
    (*oli)->self_print(outf);
    oli++;
  }
  fprintf(outf, "\n");
  this->db1d.self_print(outf);
  this->db2d.self_print(outf);
}

class thsurvey * thdatabase::get_current_survey()
{
  return this->csurveyptr;
}


class thscrap * thdatabase::get_current_scrap()
{
  return this->cscrapptr;
}


void thdatabase::insert_grade(class thgrade * optr)
{
//  thdb_grade_map_type::iterator gi = 
//    this->grade_map.find(thsurveyname(optr->get_name()));
    
//  if ((gi == this->grade_map.end()) || 
//      (thcmdln.get_print_state() == THPS_LIB_SRC)) {
    // insert grade
    this->object_list.push_back(optr);
    this->grade_map[thsurveyname(optr->get_name())] = optr;
//  }
//  else
//    ththrow(("survey grade already exists -- %s",optr->get_name()))
  
}
 
 
void thdatabase::insert_layout(class thlayout * optr)
{
//  thdb_layout_map_type::iterator gi = 
//    this->layout_map.find(thsurveyname(optr->get_name()));
    
//  if ((gi == this->layout_map.end()) || 
//      (thcmdln.get_print_state() == THPS_LIB_SRC)) {
    // insert grade
    this->object_list.push_back(optr);
    this->layout_map[thsurveyname(optr->get_name())] = optr;
//  }
//  else
//    ththrow(("map layout already exists -- %s",optr->get_name()))
  
}
 
 
class thgrade * thdatabase::get_grade(char * gname)
{
  thdb_grade_map_type::iterator gi = 
    this->grade_map.find(gname);
  if (gi == this->grade_map.end())
    return NULL;
  else
    return (thgrade *)(gi->second);
}


class thlayout * thdatabase::get_layout(char * gname)
{
  thdb_layout_map_type::iterator gi = 
    this->layout_map.find(gname);
  if (gi == this->layout_map.end())
    return NULL;
  else
    return (thlayout *)(gi->second);
}


void thdatabase::self_print_library()
{
  thprintf(thlibrarydata_init_text);

  // print survey grades
  thprintf(thlibrarydata_grades_text);
  thdb_grade_map_type::iterator gi = this->grade_map.begin();
  while (gi != this->grade_map.end()) {
    thprintf("\n\tpgrade = (thgrade *) thdb.create(\"grade\", thobjectsrc(\"therion\",0));\n");
    ((thgrade *)(gi->second))->self_print_library();
    gi++;
    thprintf("\tthdb.insert(pgrade);\n");
  }
  thprintf("}\n\n");

  // print map layouts
  thprintf(thlibrarydata_layouts_text);
  thdb_layout_map_type::iterator li = this->layout_map.begin();
  while (li != this->layout_map.end()) {
    thprintf("\n\tplayout = (thlayout *) thdb.create(\"layout\", thobjectsrc(\"therion\",0));\n");
    ((thlayout *)(li->second))->self_print_library();
    li++;
    thprintf("\tthdb.insert(playout);\n");
  }
  thprintf("}\n\n");
  
}


void thdb_object_rename_persons(thdataobject * op, thsurveyp2pmap * rmap) {

  if ((rmap == NULL) || (rmap->size() == 0))
    return;

  thsurveyp2pmap::iterator mi;
  thdataobject_author tmpa;
  thdo_author_map_type::iterator aii;
  thdate tmpdt;
  thdata * dp;
  for (mi = rmap->begin(); mi != rmap->end(); mi++) {
    tmpa.name = mi->first;
    tmpa.rev = 0;
    aii = op->author_map.find(tmpa);
    if (aii != op->author_map.end()) {
      tmpdt = aii->second;
      op->author_map.erase(aii->first);
      tmpa.name = mi->second;
      op->author_map[tmpa].join(tmpdt);
    }
    if (op->get_class_id() == TT_DATA_CMD) {
      dp = (thdata*) op;
      if (dp->team_set.erase(mi->first) > 0) {
        dp->team_set.insert(mi->second);
      }
      if (dp->discovery_team_set.erase(mi->first) > 0) {
        dp->discovery_team_set.insert(mi->second);
      }
    }
  }
}


void thdb_survey_rename_persons(thsurvey * cs, thsurveyp2pmap * rmap) {
  
  thsurveyp2pmap cmap;
  thsurveyp2pmap::iterator mi;
  cmap.clear();

  if ((rmap != NULL) && (rmap->size() > 0)) {
    mi = rmap->begin();
    while (mi != rmap->end()) {
      cmap[mi->first] = mi->second;
      mi++;
    }
  }

  if (cs->person_renames.size() > 0) {
    mi = cs->person_renames.begin();
    while (mi != cs->person_renames.end()) {
      if (cs->person_renames.find(mi->first) != cs->person_renames.end())
        cmap[mi->first] = mi->second;
      mi++;
    }
  }
  
  thdb_object_rename_persons(cs, &cmap);
  thdataobject * op;
  op = cs->foptr;
  while (op != NULL) {
    if (op->get_class_id() == TT_SURVEY_CMD) {
      thdb_survey_rename_persons((thsurvey*)op, &cmap);
    } else {
      thdb_object_rename_persons(op, &cmap);
    }
    op = op->nsptr;
  }
  
}

void thdatabase::preprocess() {
#ifdef THDEBUG
  thprintf("\n\npreprocessing database\n");
#else
  thprintf("preprocessing database ... ");
  thtext_inline = true;
#endif
  
  thsurvey * cs = this->fsurveyptr;
  while (cs != NULL) {
    thdb_survey_rename_persons(cs, NULL);
    cs = (thsurvey*)(cs->nsptr);
  }
  

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}


thdatabase thdb;


