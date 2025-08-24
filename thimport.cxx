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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "loch/icase.h"
#include "thimport.h"
#include "thcsdata.h"
#include "thexception.h"
#include "thdata.h"
#include "thsurvey.h"
#include "thdatabase.h"
#include "therion.h"
#include "img.h"
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <filesystem>

#include <fmt/printf.h>

constexpr auto ANON_STATION_NAME = "-";

/**
 * Helper function for converting a string vector to a vector of char pointers.
 */
static std::vector<char *> to_charp_vec(std::vector<std::string> & svec) {
  const size_t N = svec.size();
  auto vec = std::vector<char *>(N);
  for (size_t i = 0; i != N; ++i) {
    vec[i] = svec[i].data();
  }
  return vec;
}

struct thsst {
  std::string name, fullname;
  thsurvey * survey = {};  

  /**
   * True if this is an anonymous station (splay station)
   */
  bool is_anon() const { return name.empty(); }

  bool has_survey_ancestor(thsurvey const * const survey) const {
    thassert(survey);
    for (auto sptr = this->survey; sptr; sptr = sptr->fsptr) {
      if (sptr->id == survey->id)
        return true;
    }
    return false;
  }

  std::string get_name_in_survey(thsurvey const * const survey) const {
    thassert(survey);
    std::string name_in_survey = this->name;
    const char * delim = "@";
    for (auto sptr = this->survey; sptr && sptr->id != survey->id;
         sptr = sptr->fsptr) {
      name_in_survey += delim;
      name_in_survey += sptr->name;
      delim = ".";
    }
    return name_in_survey;
  }
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
        throw thexception(fmt::format("unknown import format -- {}", args[0]));
      break;

    case TT_IMPORT_CALIB:
      this->parse_calib(args[0], argenc);
      break;
    
    case TT_IMPORT_SURVEYS:
      this->surveys = thmatch_token(args[0], thtt_import_surveys);
      if (this->surveys == TT_IMPORT_SURVEYS_UNKNOWN)
        throw thexception(fmt::format("unknown survey structure policy -- {}", args[0]));
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
    throw thexception("file not specified");

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
      throw thexception(fmt::format("unknown file format -- {}", this->fname));
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
  thsurvey * survey = nullptr;
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

/**
 * Remove filter prefix from name. If filter does not match, return NULL.
 */
static char * get_filtered_name(char * stnm, const char * filter,
                                size_t filterl, char separator) {
  if (filterl > 0) {
    if (strncmp(stnm, filter, filterl) != 0) {
      return nullptr;
    }
    stnm = &stnm[filterl];
    while ((*stnm != 0) && (*stnm == separator)) {
      stnm++;
    }
  }
  return stnm;
}

/**
 * Add an EQUATE command for the given stations.
 */
static void set_equate(thimport * imp,
                       thsst const & station1,
                       thsst const & station2) {
  // need copies because set_data_equate modifies the buffers
  std::string copies[] = {
    station1.fullname,
    station2.fullname,
  };
  char * args[] = {
    copies[0].data(),
    copies[1].data(),
  };
  auto tmpsurvey = imp->db->csurveyptr;
  imp->db->csurveyptr = imp->fsptr;
  imp->db->csurveyptr->data->set_data_equate(2, args);
  imp->db->csurveyptr->data->clear_last_equate_warning();
  imp->db->csurveyptr = tmpsurvey;
}

typedef std::map<std::string, std::string> str2strmap;
typedef std::map<thimg_stpos, std::vector<thsst>> pos2strmap;
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
  // thprint("\n\n");

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
    throw thexception(fmt::format("unable to open file {}, error code: {}", this->fname, static_cast<int>(imgerr)));
  }

  if (pimg->cs && this->cs == TTCS_LOCAL) {
    this->set(TT_DATAOBJECT_CS, &(pimg->cs), TT_UNKNOWN_ENCODING, 0);
  }

  const auto dummy_station_suffix = std::string{pimg->separator, 'x'};
  do {
    result = img_read_item(pimg, &imgpt);
    switch (result) {
    
      case img_MOVE:
        tmpshot.fx = imgpt.x;
        tmpshot.fy = imgpt.y;
        tmpshot.fz = imgpt.z;
        break;
        
      case img_LINE:
        orig_name = pimg->label + dummy_station_suffix;
        stnm = get_filtered_name(orig_name.data(), this->filter, filterl, pimg->separator);
        if (stnm == nullptr || strlen(stnm) == 0) {
          tmpshot.fx = imgpt.x;
          tmpshot.fy = imgpt.y;
          tmpshot.fz = imgpt.z;
          break;
        }
        new_name = this->station_name(stnm, pimg->separator, &tmpsst);
        tmpshot.survey = tmpsst.survey ? tmpsst.survey : this->fsptr;
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
        stnm = get_filtered_name(pimg->label, this->filter, filterl, pimg->separator);
        if (stnm == nullptr || strlen(stnm) == 0) {
          break;
        }
        orig_name = stnm;
        if (svxs2ths.find(orig_name) == svxs2ths.end()) {
          xb = fmt::sprintf("%.16g", imgpt.x + this->calib_x);
          yb = fmt::sprintf("%.16g", imgpt.y + this->calib_y);
          zb = fmt::sprintf("%.16g", imgpt.z + this->calib_z);
          tmpsurvey = this->db->csurveyptr;
          new_name = this->station_name(stnm, pimg->separator, &tmpsst);
          if (tmpsst.survey != NULL) {
            this->db->csurveyptr = tmpsst.survey;
          } else {
            thassert(tmpsst.name == new_name);
            if (this->fsptr != NULL)
              tmpsst.survey = this->fsptr;
            else {
              notimpst++;
              // do not import station
              break;
            }
            thassert(this->db->csurveyptr == tmpsst.survey);
          }
          n1 = tmpsst.name.c_str();
          n2 = tmpsst.name.c_str();
          tmpdata = tmpsst.survey->data;
          thassert(tmpdata);
          tmppos.x = imgpt.x;
          tmppos.y = imgpt.y;
          tmppos.z = imgpt.z;
          tmpsst.fullname = new_name;
          svxpos2ths[tmppos].push_back(tmpsst);
          svxs2ths[orig_name] = new_name;
          args[1] = xb.data();
          args[2] = yb.data();
          args[3] = zb.data();
          args[0] = n1.get_buffer();
          tmpdata->cs = this->cs;
          // only fix the first station, use equate for the others
          if (svxpos2ths[tmppos].size() == 1 || this->fsptr == nullptr) {
            tmpdata->set_data_fix(4, args);
          } else {
            set_equate(this, svxpos2ths[tmppos][0], tmpsst);
          }
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
        throw thexception("invalid file format");
        break;
    }
  } while (result != img_STOP);
  img_close(pimg);

  if (notimpst > 0) {
    thwarning(fmt::format("unable to import {} stations outside survey", notimpst));
  }

  std::vector<thsst> anon_stations(1);

  // nakoniec povklada shoty
  for(sli = shotlist.begin(); sli != shotlist.end(); sli++) {
  
    auto const find_tmppos_stations = [&]() -> std::vector<thsst> * {
      auto it = svxpos2ths.find(tmppos);
      if (it != svxpos2ths.end()) {
        return &(it->second);
      }
      if ((sli->flags & img_FLAG_SPLAY) != 0) {
        return &anon_stations;
      }
      return nullptr;
    };

    tmppos.x = sli->fx;
    tmppos.y = sli->fy;
    tmppos.z = sli->fz;
    auto * from_stations = find_tmppos_stations();
    if (from_stations == nullptr) {
      continue;
    }
    
    tmppos.x = sli->tx;
    tmppos.y = sli->ty;
    tmppos.z = sli->tz;
    auto * to_stations = find_tmppos_stations();
    if (to_stations == nullptr) {
      continue;
    }
    
    auto const import_shot_for_matching_station_pair = [&] {
      for (thsurvey * survey = sli->survey; survey; survey = survey->fsptr) {
        for (thsst const & s1s : *from_stations) {
          for (thsst const & s2s : *to_stations) {
            if (import_shot(s1s, s2s, &*sli, survey)) {
              return true;
            }
          }
        }
      }
      return false;
    };

    if (!import_shot_for_matching_station_pair()) {
      notimpsh++;
    }
  }

  if (notimpsh > 0) {
    thwarning(fmt::format("unable to import {} shots outside survey", notimpsh));
  }
}

/**
 * Import shot if stations s1s and s2s are in survey `survey`.
 *
 * One of the stations may be an anonymous station (which doesn't belong to
 * a survey).
 *
 * @return True if shot import was successful
 */
bool thimport::import_shot(thsst const & s1s, //
                           thsst const & s2s, //
                           thimg_shot const * sli,
                           thsurvey * survey) {
  {
    thassert(survey);

    if (!(s1s.has_survey_ancestor(survey) || s1s.is_anon()) ||
        !(s2s.has_survey_ancestor(survey) || s2s.is_anon())) {
      // do not import
      return false;
    }

    std::vector<std::string> shead;
    std::vector<std::string> sdata;

    if (s1s.is_anon()) {
      thassert(!s2s.is_anon());
      shead = {"cartesian", "from", "to", "easting", "northing", "altitude"};
      sdata = {
          s2s.get_name_in_survey(survey),
          ANON_STATION_NAME,
          std::to_string(sli->fx - sli->tx),
          std::to_string(sli->fy - sli->ty),
          std::to_string(sli->fz - sli->tz),
      };
    } else if (s2s.is_anon()) {
      shead = {"cartesian", "from", "to", "easting", "northing", "altitude"};
      sdata = {
          s1s.get_name_in_survey(survey),
          ANON_STATION_NAME,
          std::to_string(sli->tx - sli->fx),
          std::to_string(sli->ty - sli->fy),
          std::to_string(sli->tz - sli->fz),
      };
    } else {
      shead = {"nosurvey", "from", "to"};
      sdata = {
          s1s.get_name_in_survey(survey),
          s2s.get_name_in_survey(survey),
      };
    }

    auto * const tmpsurvey = this->db->csurveyptr;
    this->db->csurveyptr = survey;
    auto * const tmpdata = survey->data;

    tmpdata->set_data_data(shead.size(), to_charp_vec(shead).data());

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
    tmpdata->insert_data_leg(sdata.size(), to_charp_vec(sdata).data());
    this->db->csurveyptr = tmpsurvey;
  }

  return true;
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
    throw thexception(fmt::format("invalid import calibration -- \"{}\"", spec));
  for(i = 0; i < 6; i++) {
    thparse_double(sv, v[i], args[i]);
    if (sv != TT_SV_NUMBER)
      throw thexception(fmt::format("invalid number -- {}", args[i]));
  }
  this->calib_x = v[3] - v[0];
  this->calib_y = v[4] - v[1];
  this->calib_z = v[5] - v[2];
}




