/**
 * @file thimport.cxx
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
 
#include "loch/icase.h"
#include "thimport.h"
#include "thexception.h"
#include "thdata.h"
#include "thsurvey.h"
#include "thdatabase.h"
#include "img.h"
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <filesystem>

#include <fmt/printf.h>

struct thsst {
  std::string name, fullname;
  thsurvey * survey = {};  
};


thimport::thimport()
{
  this->format = 0;
  this->fname = NULL;
  this->surveys = TT_IMPORT_SURVEYS_CREATE;
  this->filter = NULL;

  this->calib_x = 0.0;
  this->calib_y = 0.0;
  this->calib_z = 0.0;
}


thimport::~thimport()
{
}


int thimport::get_class_id() 
{
  return TT_IMPORT_CMD;
}


int thimport::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE);
}



bool thimport::is(int class_id)
{
  if (class_id == TT_IMPORT_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thimport::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 1;
}


const char * thimport::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


const char * thimport::get_cmd_name()
{
  // insert command name here
  return "import";
}


thcmd_option_desc thimport::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_import_opt);
  if (id == TT_IMPORT_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thimport::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    // replace this by real properties setting
    case 1:
      this->set_file_name(*args);
      break;
    
    case TT_IMPORT_FORMAT:
      this->format = thmatch_token(args[0], thtt_import_fmts);
      if (this->format == TT_IMPORT_FMT_UNKNOWN)
        ththrow("unknown import format -- {}", args[0]);
      break;

    case TT_IMPORT_CALIB:
      this->parse_calib(args[0], argenc);
      break;
    
    case TT_IMPORT_SURVEYS:
      this->surveys = thmatch_token(args[0], thtt_import_surveys);
      if (this->surveys == TT_IMPORT_SURVEYS_UNKNOWN)
        ththrow("unknown survey structure policy -- {}", args[0]);
      break;
    
    case TT_IMPORT_FILTER:
      if (strlen(args[0]) > 0)
        this->filter = this->db->strstore(args[0]);
      else
        this->filter = NULL;
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thimport::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thimport:\n");
  // insert intended print of object properties here
}

void thimport::set_file_name(char * fnm)
{
  if (strlen(fnm) == 0)
    ththrow("file not specified");

  std::error_code ec;
  const auto impf_path = (std::filesystem::current_path(ec) / thdb.csrc.name).parent_path() / fnm;
  thassert(!ec);

  auto impf_path_str = impf_path.string();
  std::replace(impf_path_str.begin(), impf_path_str.end(), '\\', '/');

  this->mysrc = this->db->csrc;
  this->fname = this->db->strstore(impf_path_str.c_str());

  const auto ext = impf_path.extension().string();
  for (const auto& [str, type] : {std::tuple{".3d", TT_IMPORT_FMT_3D}, 
                                  std::tuple{".plt", TT_IMPORT_FMT_PLT}, 
                                  std::tuple{".xyz", TT_IMPORT_FMT_XYZ}}) {
    if (icase_equals(ext, str)) {
      this->format = type;
      return;
    }
  }
}


void thimport::import_file()
{
  thobjectsrc tmpsrc;
  thsurvey * tmpsv;
  thdataobject * tmpobj;
  tmpsv = this->db->csurveyptr;
  tmpobj = this->db->lcsobjectptr;
  this->db->csurveyptr = this->fsptr;
  if (this->fsptr == NULL) {
    this->db->lcsobjectptr = this;
    while (this->db->lcsobjectptr->nsptr != NULL)
      this->db->lcsobjectptr = this->db->lcsobjectptr->nsptr;
  } else
    this->db->lcsobjectptr = this->db->csurveyptr->loptr;
  tmpsrc = this->db->csrc;
  this->db->csrc = this->mysrc;
  this->db->csrc.context = this;
  switch (this->format) {
    case TT_IMPORT_FMT_3D:
    case TT_IMPORT_FMT_PLT:
    case TT_IMPORT_FMT_XYZ:
      this->import_file_img();
      break;
    default:
      ththrow("unknown file format -- {}", this->fname);
  }
  this->db->lcsobjectptr = tmpobj;
  this->db->csurveyptr = tmpsv;
  this->db->csrc = tmpsrc;
}


const char * thimport::station_name(const char * sn, const char separator, struct thsst * sst)
{

  sst->survey = NULL;
  sst->name = sn;
  
  if (separator == 0)
    return sn;
    
  
  static thbuffer bx, prevsurvey;
  static thmbuffer psurv, csurv;
  static long active_survey;
  static thsurvey * prevpsurvey;
  long i, l;
  bx = sn;
  char * buff = bx.get_buffer(), * rv;
    
  switch (this->format) {
    case TT_IMPORT_FMT_3D:
      {
        // splitne current na podstringy
        thsplit_strings(&csurv, sn, separator);
        char ** pbf = psurv.get_buffer(), ** cbf = csurv.get_buffer();
        long nps = psurv.get_size(), ncs = csurv.get_size();
        
        if (nps == 0) {
          prevpsurvey = NULL;
        }
        
        if (nps == 0)
          active_survey = 0;
        
        if ((ncs == 1) || (this->surveys == TT_IMPORT_SURVEYS_IGNORE))
          return sn;
        
        // skusi porovnat s existujucim
        bool is_ident = true;
        if (nps != ncs) 
          is_ident = false;
        for(i = 0; is_ident && (i < (ncs - 1)); i++) {
          is_ident = is_ident && (strcmp(pbf[i], cbf[i]) == 0);
        }
        
        if (!is_ident) {
          prevpsurvey = NULL;
          thsplit_strings(&psurv, sn, separator);
          prevsurvey = "";
          int prevctx;
          thsurvey * csurvey = this->fsptr, * nsurvey,
            * prevcsptr = this->db->csurveyptr;
//          thendsurvey * nendsurvey;
          for (active_survey = 0; active_survey < (ncs - 1); active_survey++) {
            nsurvey = NULL;
            nsurvey = this->db->get_survey_noexc(cbf[active_survey], csurvey);
            // if nsurvey not found, try if there is no other object 
            // with this name
            if ((nsurvey == NULL) && (this->surveys == TT_IMPORT_SURVEYS_CREATE) &&
                (this->db->object_map.find(thobjectid(cbf[active_survey],(csurvey == NULL) ? 0 : csurvey->id)) 
                == this->db->object_map.end())) {
              // if not, create this survey
              prevcsptr = this->db->csurveyptr;
              prevctx = this->db->ccontext;
              this->db->csurveyptr = csurvey;
              if (csurvey == NULL) {
                this->db->lcsobjectptr = this;
                while (this->db->lcsobjectptr->nsptr != NULL)
                  this->db->lcsobjectptr = this->db->lcsobjectptr->nsptr;
              }
              else
                this->db->lcsobjectptr = this->db->csurveyptr->loptr;
              this->db->ccontext = THCTX_SURVEY;

              {
                auto unique_nsurvey = this->db->create<thsurvey>(this->mysrc);
                nsurvey = unique_nsurvey.get();
                // TODO - nastavit mu meno cez set
                nsurvey->name = this->db->strstore(cbf[active_survey]);
                this->db->insert(std::move(unique_nsurvey));
              }
              this->db->csrc.context = this;
              this->db->csurveylevel--;
//              nendsurvey = (thendsurvey*) this->db->create("endsurvey", this->mysrc);
//              this->db->insert(nendsurvey);
              this->db->csurveyptr = prevcsptr;
              if (prevcsptr == NULL) {
                this->db->lcsobjectptr = this;
                while (this->db->lcsobjectptr->nsptr != NULL)
                  this->db->lcsobjectptr = this->db->lcsobjectptr->nsptr;
              }
              else
                this->db->lcsobjectptr = this->db->csurveyptr->loptr;
              this->db->ccontext = prevctx;
              nsurvey = this->db->get_survey_noexc(cbf[active_survey], csurvey);
            }
            if (nsurvey != NULL) {
              if (strlen(prevsurvey.get_buffer()) == 0) {
                prevsurvey = cbf[active_survey];
              } else {
                bx = prevsurvey.get_buffer();
                prevsurvey = cbf[active_survey];
                prevsurvey += ".";
                prevsurvey += bx.get_buffer();
              }
              csurvey = nsurvey;
              prevpsurvey = csurvey;
            } else
              break;
          }
          //
        }
        
        // poskladame meno bodu
        bx = "";
        for(i = active_survey; i < ncs; i++) {
          if (i > active_survey)
            bx += ".";
          bx += cbf[i];
        }
        sst->survey = prevpsurvey;
        sst->name = bx.get_buffer();
        if (strlen(prevsurvey.get_buffer()) > 0) {
          bx += "@";
          bx += prevsurvey;
        }
        return bx.get_buffer();
      }
      break;      
    default:
      l = (long)strlen(bx);
      rv = buff;
      for(i = 0; i < l; i++) {
        if ((buff[i] == separator) && ((i + 1) < l)) {
          rv = &(buff[i+1]);
        }
      }
      sst->name = rv;
      return rv;
      break;
  }
  
}

struct thimg_shot {
  double fx = {}, fy = {}, fz = {}, tx = {}, ty = {}, tz = {};
  long flags = {};
};

struct thimg_stpos {
  double x = {}, y = {}, z = {};
};

bool operator < (const thimg_stpos & p1, 
    const thimg_stpos & p2)
{
  if (p1.x < p2.x)
    return true;
  if (p1.x > p2.x)
    return false;
  if (p1.y < p2.y)
    return true;
  if (p1.y > p2.y)
    return false;
  if (p1.z < p2.z)
    return true;
  return false;
}


typedef std::map<std::string, std::string> str2strmap;
typedef std::map<thimg_stpos, thsst> pos2strmap;
typedef std::list<thimg_shot> thimgshotlist;

void thimport::import_file_img()
{

  // potrebujeme:
  // funkciu ktora premeni ass.sss.1 na 1@sds.sds, ale 
  //   iba ak survey existuje
  // hash: ass.ass.1 -> 1@sds.sds
  // hash: x,y,z -> 1@sds.sds
  // list: xyz->xyz + flags
  
  thimg_stpos tmppos;
  thimg_shot tmpshot;
  
  str2strmap svxs2ths;
  thsst tmpsst;
  thdata * tmpdata;
  thsurvey * tmpsurvey;
  pos2strmap svxpos2ths;
  pos2strmap::iterator p2si;
  thimgshotlist shotlist;
  thimgshotlist::iterator sli;

  unsigned long notimpst = 0, notimpsh = 0;  
  
  // postup - prebehneme subor, vytvorime zoznam shotov,
  // popridavame fixne body spolu s atributmi ak existuju a vytvarame
  // oba heshe
  // nakoniec pridame shots
  // thprintf("\n\n");

  img_point imgpt;
  img_errcode imgerr;
  int result;
  char * args [4], * stnm, a0[32], a1[32], a2[32], a3[32];
  args[0] = a0;
  args[1] = a1;
  args[2] = a2;
  args[3] = a3;
  size_t filterl = 0;
  if (this->filter != NULL)
    filterl = strlen(this->filter);
  thbuffer n1, n2;
  std::string xb, yb, zb;
  std::string orig_name, new_name;  
  img* pimg = img_open(this->fname);
  if (pimg == NULL) {	
    imgerr = img_error();
    ththrow("unable to open file {}, error code: {}", this->fname, static_cast<int>(imgerr));
  }
  do {
    result = img_read_item(pimg, &imgpt);
    switch (result) {
    
      case img_MOVE:
        tmpshot.fx = imgpt.x;
        tmpshot.fy = imgpt.y;
        tmpshot.fz = imgpt.z;
        break;
        
      case img_LINE:
        tmpshot.tx = imgpt.x;
        tmpshot.ty = imgpt.y;
        tmpshot.tz = imgpt.z;
        tmpshot.flags = pimg->flags;
        shotlist.insert(shotlist.end(), tmpshot);
        tmpshot.fx = tmpshot.tx;
        tmpshot.fy = tmpshot.ty;
        tmpshot.fz = tmpshot.tz;
        break;
        
      case img_LABEL:      
        // vlozime fix station
        stnm = pimg->label;
        if ((filterl > 0) && (strncmp(stnm, this->filter, filterl) == 0)) {
          stnm = &stnm[filterl];
          while ((*stnm != 0) && (*stnm == pimg->separator)) {
            stnm++;
          }
        }
        orig_name = stnm;
        if (strlen(stnm) < 1)
          break;
        if (svxs2ths.find(orig_name) == svxs2ths.end()) {
          xb = fmt::sprintf("%.16g", imgpt.x + this->calib_x);
          yb = fmt::sprintf("%.16g", imgpt.y + this->calib_y);
          zb = fmt::sprintf("%.16g", imgpt.z + this->calib_z);
          tmpsurvey = this->db->csurveyptr;
          new_name = this->station_name(stnm, pimg->separator, &tmpsst);
          // thprintf("%s -> %s\n", pimg->label, new_name.c_str());
          tmpdata = NULL;
          if (tmpsst.survey != NULL) {
            n1 = tmpsst.name.c_str();
            n2 = tmpsst.name.c_str();
            tmpdata = tmpsst.survey->data;
            this->db->csurveyptr = tmpsst.survey;
          } else {
            n1 = new_name.c_str();
            n2 = new_name.c_str();
            if (this->fsptr != NULL)
              tmpdata = this->fsptr->data;
            else {
              notimpst++;
              // do not import station
              break;
            }
          }
          tmppos.x = imgpt.x;
          tmppos.y = imgpt.y;
          tmppos.z = imgpt.z;
          tmpsst.fullname = new_name;
          svxpos2ths[tmppos] = tmpsst;
          svxs2ths[orig_name] = new_name;
          args[1] = xb.data();
          args[2] = yb.data();
          args[3] = zb.data();
          args[0] = n1.get_buffer();
          tmpdata->cs = this->cs;
          tmpdata->set_data_fix(4, args);
          // ak bude entrance, vlozi aj station
          if ((pimg->flags & img_SFLAG_ENTRANCE) != 0) {
            args[0] = n2.get_buffer();
	    args[1] = strcpy(a1, "");
            args[2] = strcpy(a2, "entrance");
            tmpdata->set_data_station(3, args, TT_UTF_8);
          }
          if ((pimg->flags & img_SFLAG_FIXED) == 0) {
            args[0] = n2.get_buffer();
            args[1] = strcpy(a1, "");
            args[2] = strcpy(a2, "not");
            args[3] = strcpy(a3, "fixed");
            tmpdata->set_data_station(4, args, TT_UTF_8);
          }
          this->db->csurveyptr = tmpsurvey;
        }
        break;
      case img_BAD:
        img_close(pimg);
        ththrow("invalid file format");
        break;
    }
  } while (result != img_STOP);
  img_close(pimg);

  if (notimpst > 0) {
    thwarning(("unable to import %lu stations outside survey", notimpst));
  }

  thsurvey * s1survey, * s2survey;
  long s1slevel, s2slevel, maxlevel, i, j;
  thsst s1s, s2s;  
  
  // nakoniec povklada shoty
  for(sli = shotlist.begin(); sli != shotlist.end(); sli++) {
  
    tmppos.x = sli->fx;
    tmppos.y = sli->fy;
    tmppos.z = sli->fz;
    p2si = svxpos2ths.find(tmppos);
    if (p2si == svxpos2ths.end())
      continue;
    s1s = p2si->second;
    
    tmppos.x = sli->tx;
    tmppos.y = sli->ty;
    tmppos.z = sli->tz;
    p2si = svxpos2ths.find(tmppos);
    if (p2si == svxpos2ths.end())
      continue;
    s2s = p2si->second;
    
    tmpsurvey = this->db->csurveyptr;
    tmpdata = NULL;
    
    // find survey levels
    s1slevel = 0;
    s1survey = s1s.survey;
    while (s1survey != NULL) {
      s1survey = s1survey->fsptr;
      s1slevel++;
    }
    s2slevel = 0;
    s2survey = s2s.survey;
    while (s2survey != NULL) {
      s2survey = s2survey->fsptr;
      s2slevel++;
    }
    maxlevel = s1slevel;
    if (s2slevel < maxlevel) maxlevel = s2slevel;
    // try from max to min level
    for (i = maxlevel; i > 0; i--) {
      // find survey at i
      s1survey = s1s.survey;
      n1 = s1s.name.c_str();
      for (j = s1slevel; j > i; j--) {
        if (j == s1slevel)
          n1 += "@";
        else
          n1 += ".";
        n1 += s1survey->name;
        s1survey = s1survey->fsptr;
      }

      s2survey = s2s.survey;
      n2 = s2s.name.c_str();
      for (j = s2slevel; j > i; j--) {
        if (j == s2slevel)
          n2 += "@";
        else
          n2 += ".";
        n2 += s2survey->name;
        s2survey = s2survey->fsptr;
      }
      if (s1survey->id == s2survey->id) {
        this->db->csurveyptr = s1survey;
        tmpdata = s1survey->data;
        break;
      }
    }
    
    if (tmpdata == NULL) {
      if (this->fsptr != NULL)
        tmpdata = this->fsptr->data;
      else {
        notimpsh++;
        // do not import
        continue;
      }

      // tmpdata = this->data;
      n1 = s1s.fullname.c_str();
      n2 = s2s.fullname.c_str();
    }
    
//    thprintf("%s - %s = %s - %s in %s\n", s1s.fullname.c_str(), s2s.fullname.c_str(), n1.get_buffer(), n2.get_buffer(), tmpdata->fsptr->full_name);

    args[0] = strcpy(a0, "nosurvey");
    args[1] = strcpy(a1, "from");
    args[2] = strcpy(a2, "to");
    tmpdata->set_data_data(3,args);

    args[0] = n1.get_buffer();
    args[1] = n2.get_buffer();
    tmpdata->d_flags = TT_LEGFLAG_NONE;
    if ((sli->flags & img_FLAG_SURFACE) != 0) {
      tmpdata->d_flags |= TT_LEGFLAG_SURFACE;
    }
    if ((sli->flags & img_FLAG_DUPLICATE) != 0) {
      tmpdata->d_flags |= TT_LEGFLAG_DUPLICATE;
    }
    if ((sli->flags & img_FLAG_SPLAY) != 0) {
      tmpdata->d_flags |= TT_LEGFLAG_SPLAY;
    }
    tmpdata->insert_data_leg(2, args);      
    this->db->csurveyptr = tmpsurvey;
  }

  if (notimpsh > 0) {
    thwarning(("unable to import %lu shots outside survey", notimpsh));
  }

}


void thimport::parse_calib(char * spec, int /*enc*/) // TODO unused parameter enc
{
  thmbuffer * mb;
  char ** args;
  mb = &this->db->mbuff_tmp;
  thsplit_args(mb, spec);
  args = mb->get_buffer();
  double v[6];
  int sv, i;
  if (mb->get_size() != 6)
    ththrow("invalid import calibration -- \"{}\"", spec);
  for(i = 0; i < 6; i++) {
    thparse_double(sv, v[i], args[i]);
    if (sv != TT_SV_NUMBER)
      ththrow("invalid number -- {}", args[i]);
  }
  this->calib_x = v[3] - v[0];
  this->calib_y = v[4] - v[1];
  this->calib_z = v[5] - v[2];
}




