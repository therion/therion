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
 
#include "thimport.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdata.h"
#include "extern/img.h"
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <unistd.h>

thimport::thimport()
{
  // replace this by setting real properties initialization
  this->format = 0;
  this->fname = NULL;
  this->data = NULL;
}


thimport::~thimport()
{
}


int thimport::get_class_id() 
{
  return TT_IMPORT_CMD;
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


char * thimport::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


char * thimport::get_cmd_name()
{
  // insert command name here
  return "import";
}


thcmd_option_desc thimport::get_cmd_option_desc(char * opts)
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
        ththrow(("unknown import format -- %s", args[0]))
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thimport::self_delete()
{
  delete this;
}

void thimport::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thimport:\n");
  // insert intended print of object properties here
}

void thimport::set_file_name(char * fnm)
{
  
  thbuffer impf_path;
  impf_path.guarantee(1024);
  getcwd(impf_path.get_buffer(),1024);
  
  this->mysrc = this->db->csrc;
  
  long i;
  if (strlen(fnm) == 0)
    ththrow(("file not specified"))
  impf_path += "/";
  impf_path += thdb.csrc.name;
  char * pp = impf_path.get_buffer();
  for(i = strlen(pp); i >= 0; i--) {
    if ((pp[i] == '/') || (pp[i] == '\\')) {
      break;
    } else
      pp[i] = 0;
  }
  if (strlen(pp) == 0)
    impf_path = "/";
  impf_path += fnm;
  pp = impf_path.get_buffer();
  for(i = strlen(pp); i >= 0; i--) {
    if (pp[i] == '\\')
      pp[i] = '/';
  }
  this->fname = this->db->strstore(pp);

  FILE * tmp;
  tmp = fopen(this->fname,"r");
  if (tmp == NULL)
    ththrow(("unable to open file for import -- %s", this->fname))
  else
    fclose(tmp);
    
  // let's determine input type
#define check_ext(str) (strlen(this->fname) > strlen(str)) && \
  (strcasecmp(&(this->fname[strlen(this->fname) - strlen(str)]), str) == 0)
  
  if (check_ext(".3d"))
    this->format = TT_IMPORT_FMT_3D;
  else if (check_ext(".plt"))
    this->format = TT_IMPORT_FMT_PLT;
  else if (check_ext(".xyz"))
    this->format = TT_IMPORT_FMT_XYZ;

}


void thimport::import_file()
{
  thobjectsrc tmpsrc;
  thsurvey * tmpsv;
  tmpsv = this->db->csurveyptr;
  this->db->csurveyptr = this->fsptr;
  tmpsrc = this->db->csrc;
  this->db->csrc = this->mysrc;
  switch (this->format) {
    case TT_IMPORT_FMT_3D:
    case TT_IMPORT_FMT_PLT:
    case TT_IMPORT_FMT_XYZ:
      this->import_file_img();
      break;
    default:
      ththrow(("unknown file format -- %s", this->fname))
  }
  this->db->csurveyptr = tmpsv;
  this->db->csrc = tmpsrc;
}


const char * thimport::station_name(const char * sn, const char separator)
{

  if (separator == 0)
    return sn;
  
  static thbuffer bx, prevsurvey;
  static thmbuffer psurv, csurv;
  static long active_survey;
  long i, l;
  bx = sn;
  char * buff = bx.get_buffer(), * rv;
  
  switch (this->format) {
    case TT_IMPORT_FMT_3D:
      // TODO: parsnut meno surveya
      // separator na bodku
      {
        // splitne current na podstringy
        thsplit_strings(&csurv, sn, separator);
        char ** pbf = psurv.get_buffer(), ** cbf = csurv.get_buffer();
        long nps = psurv.get_size(), ncs = csurv.get_size();
        
        if (nps == 0)
          active_survey = 0;
        
        if (ncs == 1)
          return sn;
        
        // skusi porovnat s existujucim
        bool is_ident = true;
        if (nps != ncs) 
          is_ident = false;
        for(i = 0; is_ident && (i < (ncs - 1)); i++) {
          is_ident = is_ident && (strcmp(pbf[i], cbf[i]) == 0);
        }
        
        if (!is_ident) {
          thsplit_strings(&psurv, sn, separator);
          prevsurvey = "";
          thsurvey * csurvey = this->fsptr, * nsurvey;
          for (active_survey = 0; active_survey < (ncs - 1); active_survey++) {
            nsurvey = NULL;
            nsurvey = this->db->get_survey_noexc(cbf[active_survey], csurvey);
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
            } else
              break;
          }
          // nastavime prevsurvey, activesurvey a psurv
        }
        
        // poskladame meno bodu
        bx = "";
        for(i = active_survey; i < ncs; i++) {
          if (i > active_survey)
            bx += ".";
          bx += cbf[i];
        }
        if (strlen(prevsurvey.get_buffer()) > 0) {
          bx += "@";
          bx += prevsurvey;
        }
        return bx.get_buffer();
      }
      break;      
    default:
      l = strlen(bx);
      rv = buff;
      for(i = 0; i < l; i++) {
        if ((buff[i] == separator) && ((i + 1) < l)) {
          rv = &(buff[i+1]);
        }
      }
      return rv;
      break;
  }
  
}

struct thimg_shot {
  double fx, fy, fz, tx, ty, tz;
  long flags;
};


struct thimg_stpos {
  double x, y, z;
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
typedef std::map<thimg_stpos, std::string> pos2strmap;
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
  pos2strmap svxpos2ths;
  pos2strmap::iterator p2si;
  thimgshotlist shotlist;
  thimgshotlist::iterator sli;
  
  // postup - prebehneme subor, vytvorime zoznam shotov,
  // popridavame fixne body spolu s atributmi ak existuju a vytvarame
  // oba heshe
  // nakoniec pridame shots
  // thprintf("\n\n");

  img_point imgpt;
  int result;
  char * args [4];
  thbuffer n1, n2;
  thbuffer xb, yb, zb;
    xb.guarantee(128);
    yb.guarantee(128);
    zb.guarantee(128);
  std::string orig_name, new_name;  
  img* pimg = img_open(this->fname);
  if (pimg == NULL)
    ththrow(("unable to open file"))    
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
        orig_name = pimg->label;
        if (strlen(pimg->label) < 1)
          break;
        if (svxs2ths.find(orig_name) == svxs2ths.end()) {
          sprintf(xb.get_buffer(), "%.16g", imgpt.x);
          sprintf(yb.get_buffer(), "%.16g", imgpt.y);
          sprintf(zb.get_buffer(), "%.16g", imgpt.z);
          new_name = this->station_name(pimg->label, pimg->separator);
          // thprintf("%s -> %s\n", pimg->label, new_name.c_str());
          tmppos.x = imgpt.x;
          tmppos.y = imgpt.y;
          tmppos.z = imgpt.z;
          svxpos2ths[tmppos] = new_name;
          svxs2ths[orig_name] = new_name;
          n1 = new_name.c_str();
          args[0] = n1.get_buffer();
          args[1] = xb.get_buffer();
          args[2] = yb.get_buffer();
          args[3] = zb.get_buffer();
          this->data->set_data_fix(4, args);
          // ak bude entrance, vlozi aj station
          if ((pimg->flags & img_SFLAG_ENTRANCE) != 0) {
            args[1] = "";
            args[2] = "entrance";
            this->data->set_data_station(3, args, TT_UTF_8);
          }
        }
        break;
      case img_BAD:
        img_close(pimg);
        ththrow(("invalid file format"))
        break;
    }
  } while (result != img_STOP);
  img_close(pimg);
  
  args[0] = "nosurvey";
  args[1] = "from";
  args[2] = "to";
  this->data->set_data_data(3,args);
  
  // nakoniec povklada shoty
  for(sli = shotlist.begin(); sli != shotlist.end(); sli++) {
  
    tmppos.x = sli->fx;
    tmppos.y = sli->fy;
    tmppos.z = sli->fz;
    p2si = svxpos2ths.find(tmppos);
    if (p2si == svxpos2ths.end())
      continue;
    n1 = p2si->second.c_str();
    args[0] = n1.get_buffer();

    tmppos.x = sli->tx;
    tmppos.y = sli->ty;
    tmppos.z = sli->tz;
    p2si = svxpos2ths.find(tmppos);
    if (p2si == svxpos2ths.end())
      continue;
    n2 = p2si->second.c_str();
    args[1] = n2.get_buffer();
    
    this->data->d_flags = TT_LEGFLAG_NONE;
    if ((sli->flags & img_FLAG_SURFACE) != 0) {
      this->data->d_flags |= TT_LEGFLAG_SURFACE;
    }
    if ((sli->flags & img_FLAG_DUPLICATE) != 0) {
      this->data->d_flags |= TT_LEGFLAG_DUPLICATE;
    }
    this->data->insert_data_leg(2, args);
      
  }

}



