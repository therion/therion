/**
 * @file thdata.cxx
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
 
#include "thdata.h"
#include "thexception.h"
#include "thchenc.h"
#include "thinfnan.h"
#include "thsurvey.h"
#include "thgrade.h"
#include "thcsdata.h"
#include <string.h>
#include "thdatareader.h"
#ifdef THMSVC
#define strcasecmp _stricmp
#endif

thdata::thdata()
{
  this->team_set.clear();
  
  // set standard deviations and declination
  this->dl_declination = thnan;
  this->dl_survey_declination = thnan;
  this->dl_survey_declination_on = false;
  this->dl_direction = true;
  this->dli_plumbs = false;
  this->dli_equates = false;
  this->d_mark = TT_DATAMARK_TEMP;
  this->d_flags = TT_LEGFLAG_NONE;
  this->d_extend = TT_EXTENDFLAG_NORMAL;
  this->d_vtresh = 67.5;
  this->d_shape = TT_DATALEG_SHAPE_OCTAGON;
  this->d_walls = TT_AUTO;
  this->d_last_equate = 0;
  
  this->reset_data_sd();

  this->reset_data();  
  this->di_from = true;
  this->di_to = true;
  this->di_length = true;
  this->di_bearing = true;
  this->di_gradient = true;
  this->d_type = TT_DATATYPE_NORMAL;
  this->d_order[0] = TT_DATALEG_FROM;
  this->d_order[1] = TT_DATALEG_TO;
  this->d_order[2] = TT_DATALEG_LENGTH;
  this->d_order[3] = TT_DATALEG_BEARING;
  this->d_order[4] = TT_DATALEG_GRADIENT;
  this->d_nitems = 5;
  this->d_current = 0;
  
  this->stat_length = 0.0;
  this->stat_dlength = 0.0;
  this->stat_splaylength = 0.0;
  this->stat_slength = 0.0;
  this->stat_alength = 0.0;
  
  this->stat_st_state = 0;
  this->stat_st_top = NULL;
  this->stat_st_bottom = NULL;
  
  this->ugroup = NULL;
  this->cgroup = this;
	this->dlu_gradient.allow_percentage = true;
  
}

void thdata::reset_data_sd()
{
  this->dls_length = thnan;
  this->dls_gradient = thnan;
  this->dls_bearing = thnan;
  this->dls_counter = thnan;
  this->dls_depth = thnan;
  this->dls_dx = thnan;
  this->dls_dy = thnan;
  this->dls_dz = thnan;
  this->dls_x = thnan;
  this->dls_y = thnan;
  this->dls_z = thnan;
}


void thdata::reset_data()
{
  this->di_station = false;
  this->di_from = false;
  this->di_to = false;
  this->di_length = false;
  this->di_bearing = false;
  this->di_gradient = false;
  this->di_backbearing = false;
  this->di_backgradient = false;
  this->di_depth = false;
  this->di_fromdepth = false;
  this->di_todepth = false;
  this->di_depthchange = false;
  this->di_count = false;
  this->di_fromcount = false;
  this->di_tocount = false;
  this->di_dx = false;
  this->di_dy = false;
  this->di_dz = false;
  this->di_direction = false;
  this->di_newline = false;
  this->di_interleaved = false;
  
  this->di_up = false;
  this->di_down = false;
  this->di_left = false;
  this->di_right = false;
  
  this->d_type = TT_DATATYPE_UNKNOWN;
  int i;
  for(i = 0; i < THDATA_MAX_ITEMS; i++)
    this->d_order[i] = TT_DATALEG_UNKNOWN;
  this->d_nitems = 0;
  this->d_current = 0;
  
  this->cd_leg_def = false;
  this->pd_leg_def = false;
  
}


thdata::~thdata()
{
}


int thdata::get_class_id() 
{
  return TT_DATA_CMD;
}


bool thdata::is(int class_id)
{
  if (class_id == TT_DATA_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thdata::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 0;
}


const char * thdata::get_cmd_end()
{
  // insert endcommand if multiline command
  return "endcentreline";
}


bool thdata::get_cmd_ends_state() {
  return true;
}


static const thstok thdata__end_cmds[] = {
  {"endcenterline", TT_DATA_CMD},
  {"endcentreline", TT_DATA_CMD},
	{NULL, TT_UNKNOWN_CMD},
};


bool thdata::get_cmd_ends_match(char * cmd) {
  return (thmatch_token(cmd,thdata__end_cmds) == TT_DATA_CMD);
}


thcmd_option_desc thdata::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_data_opt);
  if (id == TT_DATA_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else
    switch (id) {
      case TT_DATA_FIX:
        return thcmd_option_desc(id,4);
      case TT_DATA_SD:
      case TT_DATA_UNITS:
      case TT_DATA_INSTRUMENT:
      case TT_DATA_CALIBRATE:
      case TT_DATA_INFER:
      case TT_DATA_STATION:
      case TT_DATA_EQUATE:
      case TT_DATA_DATA:
      case TT_DATA_DECLINATION:
      case TT_DATA_VTRESH:
        return thcmd_option_desc(id,2);
      default:
        return thcmd_option_desc(id,1);
      case TT_DATA_BREAK:
      case TT_DATA_GROUP:
      case TT_DATA_ENDGROUP:
        return thcmd_option_desc(id,0);
    }
}


void thdata::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  thdate temp_date;
  thperson temp_person;
  int prole_i;
  switch (cod.id) {
  
    case 0:
      // let's split the line
      thsplit_args(& this->db->mbuff_tmp, *args);
      this->cgroup->insert_data_leg(this->db->mbuff_tmp.get_size(),
        this->db->mbuff_tmp.get_buffer());
      break;
      
    case TT_DATA_FIX:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- fix"))
      this->cgroup->set_data_fix(cod.nargs, args);
      break;
      
    case TT_DATA_EQUATE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- equate"))
      this->cgroup->set_data_equate(cod.nargs, args);
      break;
      
    case TT_DATA_FLAGS:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- flags"))
      this->cgroup->set_data_flags(cod.nargs, args);
      break;

    case TT_DATA_EXTEND:
      this->cgroup->set_data_extend(cod.nargs, args);
      break;
      
    case TT_DATA_STATION:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- station"))
      this->cgroup->set_data_station(cod.nargs, args, argenc);
      break;
      
    case TT_DATA_MARK:
      this->cgroup->set_data_mark(cod.nargs, args);
      break;

    case TT_DATA_DATE:
      temp_date.parse(*args);
      this->date.join(temp_date);
      this->set_survey_declination();
      break;
      
    case TT_DATA_DISCOVERY_DATE:
      temp_date.parse(*args);
      this->discovery_date.join(temp_date);
      break;
      
    case TT_DATA_WALLS:
      if (cod.nargs != 1)
    	  ththrow(("invalid number of option arguments -- walls"));
      this->cgroup->d_walls = thmatch_token(*args, thtt_onoffauto);
      if (this->cgroup->d_walls == TT_UNKNOWN_BOOL)
    	  ththrow(("invalid walls switch -- %s", *args));
      break;
      
    case TT_DATA_SHAPE:
      if (cod.nargs != 1)
    	  ththrow(("invalid number of option arguments -- shape"));
      this->cgroup->d_shape = thmatch_token(*args, thtt_dataleg_shape);
      if (this->cgroup->d_shape == TT_DATALEG_SHAPE_UNKNOWN)
    	  ththrow(("unknown shape type -- %s", *args));
      break;
      
    case TT_DATA_VTRESH:
      this->cgroup->set_data_vtresh(cod.nargs, args);
      break;
      
    case TT_DATA_INSTRUMENT:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- instrument"))
      this->set_data_instrument(cod.nargs, args);
      break;
      
    case TT_DATA_SD:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- sd"))
      this->cgroup->set_data_sd(cod.nargs, args);
      break;
      
    case TT_DATA_DATA:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- data"))
      this->cgroup->set_data_data(cod.nargs, args);
      break;
      
    case TT_DATA_INFER:
      this->cgroup->set_data_infer(cod.nargs, args);
      break;
      
    case TT_DATA_DECLINATION:
      this->cgroup->set_data_declination(cod.nargs, args);
      break;
      
    case TT_DATA_UNITS:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- units"))
      this->cgroup->set_data_units(cod.nargs, args);
      break;
      
    case TT_DATA_GRADE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- grade"))
      this->cgroup->set_data_grade(cod.nargs, args);
      break;
        
    case TT_DATA_BREAK:
      if (cod.nargs > 0)
        ththrow(("no arguments allowed after break"))
      this->cgroup->complete_interleaved_data();
      this->cgroup->cd_leg_def = false;
      this->cgroup->d_current = 0;
      break;
    
    case TT_DATA_CALIBRATE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- calibrate"))
      this->cgroup->set_data_calibration(cod.nargs, args);
      break;
    
    case TT_DATA_TEAM:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- team"))
      thencode(&(this->db->buff_enc), *args, argenc);
      temp_person.parse(this->db, this->db->buff_enc.get_buffer());
      this->team_set.insert(temp_person);
      // check person roles
      if (cod.nargs > 1) {
        for(prole_i = 1; prole_i < cod.nargs; prole_i++) {
          switch (thmatch_token(args[prole_i], thtt_dataleg_comp)) {
            case TT_DATALEG_LENGTH:
            case TT_DATALEG_BEARING:
            case TT_DATALEG_GRADIENT:
            case TT_DATALEG_NOTES:
            case TT_DATALEG_ASSISTANT:
            case TT_DATALEG_INSTRUMENTS:
            case TT_DATALEG_PICTURES:
            case TT_DATALEG_COUNT:
            case TT_DATALEG_DEPTH:
            case TT_DATALEG_STATION:
            case TT_DATALEG_UP:
            case TT_DATALEG_DOWN:
            case TT_DATALEG_LEFT:
            case TT_DATALEG_RIGHT:
              break;
            default:
              ththrow(("unknown team role -- %s", args[prole_i]))
              break;
          }
        }
      }
      break;

    case TT_DATA_DISCOVERY_TEAM:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- explo-team"))
      thencode(&(this->db->buff_enc), *args, argenc);
      temp_person.parse(this->db, this->db->buff_enc.get_buffer());
      this->discovery_team_set.insert(temp_person);
      break;

    case TT_DATA_GROUP:
      this->start_group();
      break;
      
    case TT_DATA_ENDGROUP:
      this->end_group();
      break;
      
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}

void thdata::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thdata:\n");
  fprintf(outf,"\tdate: %s\n",this->date.get_str());
  fprintf(outf,"\tlength: %g\n",this->stat_length);
  
  // print the data team
  thdata_team_set_type::iterator ii;
  for(ii = this->team_set.begin();
      ii != this->team_set.end(); ii++)
    fprintf(outf,"\tteam: \"%s/%s\"\n", ii->get_n1(), ii->get_n2());
  fprintf(outf,"\tdiscovery-date: %s\n",this->discovery_date.get_str());

  // print the data team
  for(ii = this->discovery_team_set.begin();
      ii != this->discovery_team_set.end(); ii++)
    fprintf(outf,"\tdiscovery-team: \"%s/%s\"\n", ii->get_n1(), ii->get_n2());
    
  // print survey data
  for(thdataleg_list::iterator li = this->leg_list.begin();
    li != this->leg_list.end(); li++)
    if (li->is_valid) {
      fprintf(outf,"\tshot: ");
      fprintf(outf,li->from);
      fprintf(outf," - ");
      fprintf(outf,li->to);
      switch(li->data_type) {
        case TT_DATATYPE_NORMAL:
          fprintf(outf," (normal)\n");
          fprintf(outf,"\t\tlength: %f +- %f m\n", thinn(li->length), thinn(li->length_sd));
          fprintf(outf,"\t\tbearing: %f +- %f deg\n", thinn(li->bearing), thinn(li->bearing_sd));
          fprintf(outf,"\t\tgradient: %f +- %f deg\n", thinn(li->gradient), thinn(li->gradient_sd));
          fprintf(outf,"\t\ttotal length: %f\n",thinn(li->total_length));
          break;
        case TT_DATATYPE_CYLPOLAR:
          fprintf(outf," (cylpolar)\n");
        case TT_DATATYPE_DIVING:
          if (li->data_type == TT_DATATYPE_DIVING) 
            fprintf(outf," (diving)\n");
          fprintf(outf,"\t\tlength: %f +- %f m\n", thinn(li->length), thinn(li->length_sd));
          fprintf(outf,"\t\tbearing: %f +- %f deg\n", thinn(li->bearing), thinn(li->bearing_sd));
          if (thisnan(li->depthchange)) {
            fprintf(outf,"\t\tfromdepth: %f +- %f m\n", thinn(li->fromdepth), thinn(li->depth_sd));
            fprintf(outf,"\t\ttodepth: %f +- %f m\n", thinn(li->todepth), thinn(li->depth_sd));
          }
          else
            fprintf(outf,"\t\tdepthchange: %f +- %f m\n", thinn(li->depthchange), thinn(li->depth_sd));
          fprintf(outf,"\t\ttotal length: %f\n",thinn(li->total_length));
          break;
        case TT_DATATYPE_CARTESIAN:
          fprintf(outf," (cartesian)\n");
          fprintf(outf,"\t\tdx: %f +- %f m\n", thinn(li->dx), thinn(li->dx_sd));
          fprintf(outf,"\t\tdy: %f +- %f m\n", thinn(li->dy), thinn(li->dy_sd));
          fprintf(outf,"\t\tdz: %f +- %f m\n", thinn(li->dz), thinn(li->dz_sd));
          fprintf(outf,"\t\ttotal length: %f\n",thinn(li->total_length));
          break;
        case TT_DATATYPE_NOSURVEY:
          fprintf(outf," (nosurvey)\n");
          break;
      }
      fprintf(outf,"\t\tUDLR: (%g,%g,%g,%g) - (%g,%g,%g,%g)\n",
        thinn(li->from_up),thinn(li->from_down),thinn(li->from_left),thinn(li->from_right),
        thinn(li->to_up),thinn(li->to_down),thinn(li->to_left),thinn(li->to_right));
      fprintf(outf,"\t\tdecination: %f deg\n", thinn(li->declination));
      fprintf(outf,"\t\tmark: %d\n", li->s_mark);
      fprintf(outf,"\t\tflags: %d\n", li->flags);
      fprintf(outf,"\t\tinfer:");
      if (li->infer_plumbs)
        fprintf(outf," plumbs");
      if (li->infer_equates)
        fprintf(outf," equates");
      if (!(li->infer_plumbs || li->infer_equates))
        fprintf(outf," none");
      fprintf(outf,"\n");
      fprintf(outf,"\t\tsurvey: %s\n", li->psurvey->get_full_name());
      fprintf(outf,"\t\tsource: %s [%ld]\n", li->srcf.name, li->srcf.line);
    }
 
  // print fixed stations
  for(thdatafix_list::iterator fxi = this->fix_list.begin();
    fxi != this->fix_list.end(); fxi++) {
    fprintf(outf,"\tfix: ");
    fprintf(outf,fxi->station);
    fprintf(outf,"\n");
    fprintf(outf,"\t\tpos:\t%f\t%f\t%f\n", thinn(fxi->x), thinn(fxi->y), thinn(fxi->z));
    fprintf(outf,"\t\tstd:\t%f\t%f\t%f\n", thinn(fxi->sdx), thinn(fxi->sdy), thinn(fxi->sdz));
    fprintf(outf,"\t\tcov:\t%f\t%f\t%f\n", thinn(fxi->cxy), thinn(fxi->cyz), thinn(fxi->cxz));
    fprintf(outf,"\t\tsource: %s [%ld]\n", fxi->srcf.name, fxi->srcf.line);
  }

  // print equates
  int last_e = 0;
  thdataequate_list::iterator eii;
  for(thdataequate_list::iterator ei = this->equate_list.begin();
    ei != this->equate_list.end(); ei++) {
    if (last_e != ei->eqid) {
      if (last_e > 0)
        fprintf(outf,"\n");
      fprintf(outf,"\tequate:");
      eii = ei;
      last_e = ei->eqid;
    }
    fprintf(outf," ");
    fprintf(outf,ei->station);
  }
  if (last_e > 0) {
    fprintf(outf," (%s [%ld])\n", eii->srcf.name, eii->srcf.line);
  }
  
    
  // print stations
  for(thdatass_list::iterator si = this->ss_list.begin();
    si != this->ss_list.end(); si++) {
    fprintf(outf,"\tstation: ");
    fprintf(outf,si->station);
    fprintf(outf," \"%s\" %d (%s [%ld])\n", (si->comment == NULL ? "" : si->comment),
      si->flags, si->srcf.name, si->srcf.line);
  }

  // print dimension lists
  for(thstdims_list::iterator di = this->dims_list.begin();
    di != this->dims_list.end(); di++) {
    fprintf(outf,"\tUDLR: ");
    fprintf(outf,di->station);
    fprintf(outf," %g,%g,%g,%g\n", thinn(di->up), thinn(di->down), thinn(di->left), thinn(di->right));
  }


}


void thdata::self_delete()
{
  delete this;
}


void thdata::set_data_calibration(int nargs, char ** args)
{
  static int items[THDATA_MAX_ITEMS];
  int nitems, iit, itid; // 1-length, 2-angle

  nitems = nargs - 1;
  if (nitems > THDATA_MAX_ITEMS)
    ththrow(("too many quantities"))

  // scan quantities
  bool to_set;
  for (iit = 0; iit < nitems; iit++) {
    itid = thmatch_token(args[iit], thtt_dataleg_comp);
    to_set = true;
    switch (itid) {
      case TT_DATALEG_LENGTH:
      case TT_DATALEG_COUNT:
      case TT_DATALEG_DEPTH:
      case TT_DATALEG_X:
      case TT_DATALEG_POSITION:
      case TT_DATALEG_EASTING:
      case TT_DATALEG_Y:
      case TT_DATALEG_NORTHING:
      case TT_DATALEG_Z:
      case TT_DATALEG_DIMS:
      case TT_DATALEG_ALTITUDE:
      case TT_DATALEG_BEARING:
      case TT_DATALEG_GRADIENT:
      case TT_DATALEG_UP:
      case TT_DATALEG_DOWN:
      case TT_DATALEG_LEFT:
      case TT_DATALEG_RIGHT:
        break;
      default:
        to_set = false;
        nitems = iit;
        break;
    }
    if (to_set)
      items[iit] = itid;
    else
      break;
  }
  
  // parse calibration
  this->dlc_default.parse(nargs - nitems, args + nitems);
  
  // set units
  for (iit = 0; iit < nitems; iit++) {
    switch (items[iit]) {
      case TT_DATALEG_LENGTH:
        this->dlc_length = this->dlc_default;
        break;
      case TT_DATALEG_COUNT:
        this->dlc_counter = this->dlc_default;
        break;
      case TT_DATALEG_DEPTH:
        this->dlc_depth = this->dlc_default;
        break;
      case TT_DATALEG_X:
        this->dlc_x = this->dlc_default;
        break;
      case TT_DATALEG_EASTING:
        this->dlc_dx = this->dlc_default;
        break;
      case TT_DATALEG_Y:
        this->dlc_y = this->dlc_default;
        break;
      case TT_DATALEG_NORTHING:
        this->dlc_dy = this->dlc_default;
        break;
      case TT_DATALEG_Z:
        this->dlc_z = this->dlc_default;
        break;
      case TT_DATALEG_POSITION:
        this->dlc_x = this->dlc_default;
        this->dlc_y = this->dlc_default;
        this->dlc_z = this->dlc_default;
        break;
      case TT_DATALEG_ALTITUDE:
        this->dlc_dz = this->dlc_default;
        break;
      case TT_DATALEG_BEARING:
        this->dlc_bearing = this->dlc_default;
        break;
      case TT_DATALEG_GRADIENT:
        this->dlc_gradient = this->dlc_default;
        break;
      case TT_DATALEG_DIMS:
        this->dlc_up = this->dlc_default;
        this->dlc_down = this->dlc_default;
        this->dlc_left = this->dlc_default;
        this->dlc_right = this->dlc_default;
        break;
      case TT_DATALEG_UP:
        this->dlc_up = this->dlc_default;
        break;
      case TT_DATALEG_DOWN:
        this->dlc_down = this->dlc_default;
        break;
      case TT_DATALEG_LEFT:
        this->dlc_left = this->dlc_default;
        break;
      case TT_DATALEG_RIGHT:
        this->dlc_right = this->dlc_default;
        break;
    }
  }
}  // data calibration
  

void thdata::set_data_units(int nargs, char ** args)
{
  static int items[THDATA_MAX_ITEMS];
  int nitems, iit, itid, sdtype = 0; // 1-length, 2-angle
	
	this->dlu_sdangle.allow_percentage = true;

  nitems = nargs - 1;
  if (nitems > THDATA_MAX_ITEMS)
    ththrow(("too many quantities"))

  // scan quantities
  bool to_set;
  for (iit = 0; iit < nitems; iit++) {
    itid = thmatch_token(args[iit], thtt_dataleg_comp);
    to_set = true;
    switch (itid) {
      case TT_DATALEG_LENGTH:
      case TT_DATALEG_COUNT:
      case TT_DATALEG_DEPTH:
      case TT_DATALEG_DEPTHCHANGE:
      case TT_DATALEG_POSITION:
      case TT_DATALEG_X:
      case TT_DATALEG_EASTING:
      case TT_DATALEG_Y:
      case TT_DATALEG_NORTHING:
      case TT_DATALEG_Z:
      case TT_DATALEG_ALTITUDE:
      case TT_DATALEG_DIMS:
      case TT_DATALEG_UP:
      case TT_DATALEG_DOWN:
      case TT_DATALEG_LEFT:
      case TT_DATALEG_RIGHT:
        if (sdtype == 0)
          sdtype = 1;
        else {
          if (sdtype != 1)
            ththrow(("incompatible quantity -- %s", args[iit]))
        }
        break;
      case TT_DATALEG_BEARING:
      case TT_DATALEG_GRADIENT:
        if (sdtype == 0)
          sdtype = 2;
        else {
          if (sdtype != 2)
            ththrow(("incompatible quantity -- %s", args[iit]))
        }
        break;
      default:
        to_set = false;
        if (iit == nitems - 1) {
          nitems--;
          break;
        }
        else 
          ththrow(("invalid quantity -- %s",args[iit]))
    }
    if (to_set)
      items[iit] = itid;
  }
  
  // parse units
  switch (sdtype) {
    case 1:
      this->dlu_sdlength.parse_units(args[nargs-1]);
      if (nitems == nargs - 2)
        this->dlu_sdlength.parse_scale(args[nitems]);
      break;
    case 2:
      this->dlu_sdangle.parse_units(args[nargs-1]);
      if (nitems == nargs - 2)
        this->dlu_sdangle.parse_scale(args[nitems]);
      break;
  }
  
  // set units
  for (iit = 0; iit < nitems; iit++) {
    switch (items[iit]) {
      case TT_DATALEG_LENGTH:
        this->dlu_length = this->dlu_sdlength;
        break;
      case TT_DATALEG_COUNT:
        this->dlu_counter = this->dlu_sdlength;
        break;
      case TT_DATALEG_DEPTH:
      case TT_DATALEG_DEPTHCHANGE:
        this->dlu_depth = this->dlu_sdlength;
        break;
      case TT_DATALEG_X:
        this->dlu_x = this->dlu_sdlength;
        break;
      case TT_DATALEG_EASTING:
        this->dlu_dx = this->dlu_sdlength;
        break;
      case TT_DATALEG_Y:
        this->dlu_y = this->dlu_sdlength;
        break;
      case TT_DATALEG_NORTHING:
        this->dlu_dy = this->dlu_sdlength;
        break;
      case TT_DATALEG_Z:
        this->dlu_z = this->dlu_sdlength;
        break;
      case TT_DATALEG_POSITION:
        this->dlu_x = this->dlu_sdlength;
        this->dlu_y = this->dlu_sdlength;
        this->dlu_z = this->dlu_sdlength;
        break;
      case TT_DATALEG_ALTITUDE:
        this->dlu_dz = this->dlu_sdlength;
        break;
      case TT_DATALEG_BEARING:
        this->dlu_bearing = this->dlu_sdangle;
				if (this->dlu_bearing.get_units() == TT_TFU_PERC)
					ththrow(("percentage bearing specification not allowed"))
        break;
      case TT_DATALEG_GRADIENT:
        this->dlu_gradient = this->dlu_sdangle;
        break;
      case TT_DATALEG_DIMS:
        this->dlu_up = this->dlu_sdlength;
        this->dlu_down = this->dlu_sdlength;
        this->dlu_left = this->dlu_sdlength;
        this->dlu_right = this->dlu_sdlength;
        break;
      case TT_DATALEG_UP:
        this->dlu_up = this->dlu_sdlength;
        break;
      case TT_DATALEG_DOWN:
        this->dlu_down = this->dlu_sdlength;
        break;
      case TT_DATALEG_LEFT:
        this->dlu_left = this->dlu_sdlength;
        break;
      case TT_DATALEG_RIGHT:
        this->dlu_right = this->dlu_sdlength;
        break;
    }
  }

	this->dlu_sdangle.allow_percentage = false;
}  // data units
  

void thdata::set_data_vtresh(int nargs, char ** args)
{

  if ((nargs < 1) || (nargs > 2))
    ththrow(("invalid number of option arguments -- vthreshold"))
  int nid;
  thtfangle dlu;
  thparse_double(nid, this->d_vtresh, args[0]);
  if (nid != TT_SV_NUMBER)
    ththrow(("invalid vthreshold -- %s", args[0]))
  if (nargs > 1) {
    dlu.parse_units(args[1]);
    this->d_vtresh = dlu.transform(this->d_vtresh);
  }
  if ((this->d_vtresh < 0.0) || (this->d_vtresh > 90.0))
    ththrow(("vthreshold out of range -- %s", args[0]))
  
}


void thdata::set_data_declination(int nargs, char ** args)
{
  if (nargs > 2)
    ththrow(("too many option arguments"))
  int nid;
  thparse_double(nid, this->dl_declination, args[0]);
  if (nargs > 1) {
    this->dlu_declination.parse_units(args[1]);
    this->dl_declination = this->dlu_declination.transform(this->dl_declination);
  }
  else 
    if (nid != TT_SV_NAN)
      ththrow(("missing declination units"))
  if ((nid != TT_SV_NUMBER) && (nid != TT_SV_NAN))
    ththrow(("invalid declination -- %s", args[0]))
}
  
void thdata::set_data_infer(int nargs, char ** args)
{
  if (nargs > 2)
    ththrow(("too many option arguments"))
  int iwhat, ihow;
  iwhat = thmatch_token(args[0],thtt_dataleg_infer);
  ihow = thmatch_token(args[1],thtt_bool);
  if (ihow == TT_UNKNOWN_BOOL)
    ththrow(("invalid boolean argument -- %s", args[1]))
  switch (iwhat) {
    case TT_DATALEG_INFER_EQUATES:
      this->dli_equates = (ihow == TT_TRUE);
      break;
    case TT_DATALEG_INFER_PLUMBS:
      this->dli_plumbs = (ihow == TT_TRUE);
      break;
    default:
      ththrow(("invalid argument -- %s", args[0]))
  }
}

void thdata::set_data_instrument(int nargs, char ** args)
{
  int iid, i;
  for (i = 0; i < nargs - 1; i++) {
    iid = thmatch_token(args[i], thtt_dataleg_comp);
    switch (iid) {
      case TT_DATALEG_STATION:
      case TT_DATALEG_LENGTH:
      case TT_DATALEG_BEARING:
      case TT_DATALEG_GRADIENT:
      case TT_DATALEG_DEPTH:
      case TT_DATALEG_COUNT:
      case TT_DATALEG_NOTES:
      case TT_DATALEG_PICTURES:
      case TT_DATALEG_POSITION:
      case TT_DATALEG_INSTRUMENTS:
      case TT_DATALEG_ASSISTANT:
      case TT_DATALEG_UP:
      case TT_DATALEG_DOWN:
      case TT_DATALEG_LEFT:
      case TT_DATALEG_RIGHT:
        break;
      default:
        ththrow(("invalid instrument -- %s", args[i]))
    }
  }
}

void thdata::set_data_sd(int nargs, char ** args)
{
  static int items[THDATA_MAX_ITEMS];
  int nitems, nid, iit, itid, sdtype = 0; // 1-length, 2-angle
  double nval;

  nitems = nargs - 1;
  if (nitems > THDATA_MAX_ITEMS)
    ththrow(("too many quantities"))

  // scan quantities
  bool to_set;
  for (iit = 0; iit < nitems; iit++) {
    itid = thmatch_token(args[iit], thtt_dataleg_comp);
    to_set = true;
    switch (itid) {
      case TT_DATALEG_LENGTH:
      case TT_DATALEG_COUNT:
      case TT_DATALEG_DEPTH:
      case TT_DATALEG_X:
      case TT_DATALEG_EASTING:
      case TT_DATALEG_Y:
      case TT_DATALEG_NORTHING:
      case TT_DATALEG_Z:
      case TT_DATALEG_ALTITUDE:
      case TT_DATALEG_POSITION:
        if (sdtype == 0)
          sdtype = 1;
        else {
          if (sdtype != 1)
            ththrow(("incompatible quantity -- %s", args[iit]))
        }
        break;
      case TT_DATALEG_BEARING:
      case TT_DATALEG_GRADIENT:
        if (sdtype == 0)
          sdtype = 2;
        else {
          if (sdtype != 2)
            ththrow(("incompatible quantity -- %s", args[iit]))
        }
        break;
      default:
        to_set = false;
        if (iit == nitems - 1) {
          nitems--;
          break;
        }
        else 
          ththrow(("invalid quantity -- %s",args[iit]))
    }
    if (to_set)
      items[iit] = itid;
  }
  
  // parse value  
  thparse_double(nid, nval, args[nitems]);
  switch (nid) {
    case TT_SV_NUMBER:
      if (nitems == nargs - 1)
        ththrow(("missing standard deviation units"));
      switch (sdtype) {
        case 1:
          this->dlu_sdlength.parse_units(args[nitems+1]);
          nval = this->dlu_sdlength.transform(nval);
          break;
        case 2:
          this->dlu_sdangle.parse_units(args[nitems+1]);
          nval = this->dlu_sdangle.transform(nval);
          break;
      }
      break;
    case TT_SV_NAN:
      break;
    default:
      if (nid != TT_SV_NUMBER)
        ththrow(("invalid value -- %s", args[nitems]))
  }
  
  // set standard deviations
  for (iit = 0; iit < nitems; iit++) {
    switch (items[iit]) {
      case TT_DATALEG_LENGTH:
        this->dls_length = nval;
        break;
      case TT_DATALEG_COUNT:
        this->dls_counter = nval;
        break;
      case TT_DATALEG_DEPTH:
        this->dls_depth = nval;
        break;
      case TT_DATALEG_X:
        this->dls_x = nval;
        break;
      case TT_DATALEG_EASTING:
        this->dls_dx = nval;
        break;
      case TT_DATALEG_Y:
        this->dls_y = nval;
        break;
      case TT_DATALEG_NORTHING:
        this->dls_dy = nval;
        break;
      case TT_DATALEG_Z:
        this->dls_z = nval;
        break;
      case TT_DATALEG_POSITION:
        this->dls_x = nval;
        this->dls_y = nval;
        this->dls_z = nval;
        break;
      case TT_DATALEG_ALTITUDE:
        this->dls_dz = nval;
        break;
      case TT_DATALEG_BEARING:
        this->dls_bearing = nval;
        break;
      case TT_DATALEG_GRADIENT:
        this->dls_gradient = nval;
        break;
    }
  }
  
} // standard deviation


void thdata::set_data_data(int nargs, char ** args)
{
  // OK, finish the data previosly specified
  this->complete_interleaved_data();
  // first let's detect data type
  this->reset_data();
  this->d_type = thmatch_token(args[0], thtt_datatype);
  if (this->d_type == TT_DATATYPE_UNKNOWN)
    ththrow(("unknown data type -- %s", args[0]))
    
  // second let's detect data order
  if (nargs > THDATA_MAX_ITEMS)
    ththrow(("too many quantities"))
  int dix, idd;
  bool err_duplicate = false, err_inimm = false, err_itype = false,
    err_idanl = false;
  for(dix = 1; dix < nargs; dix++) {
    idd = thmatch_token(args[dix], thtt_dataleg_comp);
    switch (idd) {

      case TT_DATALEG_STATION:
        if (this->di_station) {
          err_duplicate = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        if (this->di_from || this->di_to) {
          err_inimm = true;
          break;
        }
        this->di_station = true;
        this->di_interleaved = true;
        break;        
        
      case TT_DATALEG_FROM:
        if (this->di_from) {
          err_duplicate = true;
          break;
        }
        if (this->di_station) {
          err_inimm = true;
          break;
        }
        if (this->d_type == TT_DATATYPE_DIMS) {
          err_itype = true;
          break;
        }
        this->di_from = true;
        break;        
     
      case TT_DATALEG_TO:
        if (this->di_to) {
          err_duplicate = true;
          break;
        }
        if (this->di_station) {
          err_inimm = true;
          break;
        }
        if (this->d_type == TT_DATATYPE_DIMS) {
          err_itype = true;
          break;
        }
        this->di_to = true;
        break;
        
      case TT_DATALEG_LENGTH:
        if (this->di_length || this->di_fromcount || this->di_tocount) {
          err_duplicate = true;
          break;
        }
        if (this->di_count) {
          err_inimm = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_NORMAL:
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_length = true;
        break;
        
      case TT_DATALEG_BEARING:
        if (this->di_bearing) {
          err_duplicate = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_NORMAL:
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_bearing = true;
        break;
        
      case TT_DATALEG_BACKBEARING:
        if (this->di_backbearing) {
          err_duplicate = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_NORMAL:
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_backbearing = true;
        break;
        
      case TT_DATALEG_GRADIENT:
        if (this->di_gradient) {
          err_duplicate = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_NORMAL:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_gradient = true;
        break;
        
      case TT_DATALEG_BACKGRADIENT:
        if (this->di_backgradient) {
          err_duplicate = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_NORMAL:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_backgradient = true;
        break;
        
      case TT_DATALEG_DEPTH:
        if (this->di_depth) {
          err_duplicate = true;
          break;
        }
        if (this->di_fromdepth || this->di_todepth || this->di_depthchange) {
          err_inimm = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_depth = true;
        this->di_interleaved = true;
        break;
        
      case TT_DATALEG_FROMDEPTH:
        if (this->di_fromdepth || this->di_depthchange) {
          err_duplicate = true;
          break;
        }
        if (this->di_depth) {
          err_inimm = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_fromdepth = true;
        break;

      case TT_DATALEG_TODEPTH:
        if (this->di_todepth || this->di_depthchange) {
          err_duplicate = true;
          break;
        }
        if (this->di_depth) {
          err_inimm = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_todepth = true;
        break;
        
      case TT_DATALEG_DEPTHCHANGE:
        if (this->di_depthchange || this->di_fromdepth || this->di_todepth) {
          err_duplicate = true;
          break;
        }
        if (this->di_depth) {
          err_inimm = true;
          break;
        }
        switch (this->d_type) {
          case TT_DATATYPE_DIVING:
          case TT_DATATYPE_CYLPOLAR:
            break;
          default:
            err_itype = true;
        }
        if (err_itype)
          break;
        this->di_depthchange = true;
        break;

      case TT_DATALEG_COUNT:
        if (this->di_count) {
          err_duplicate = true;
          break;
        }
        if (this->di_fromcount || this->di_tocount || this->di_length) {
          err_inimm = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_NORMAL) {
          err_itype = true;
          break;
        }
        this->di_count = true;
        this->di_interleaved = true;
        break;
        
      case TT_DATALEG_FROMCOUNT:
        if (this->di_fromcount || this->di_length) {
          err_duplicate = true;
          break;
        }
        if (this->di_count) {
          err_inimm = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_NORMAL) {
          err_itype = true;
          break;
        }
        this->di_fromcount = true;
        break;

      case TT_DATALEG_TOCOUNT:
        if (this->di_tocount || this->di_length) {
          err_duplicate = true;
          break;
        }
        if (this->di_count) {
          err_inimm = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_NORMAL) {
          err_itype = true;
          break;
        }
        this->di_tocount = true;
        break;

      case TT_DATALEG_EASTING:
        if (this->di_dx) {
          err_duplicate = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_CARTESIAN) {
          err_itype = true;
          break;
        }
        this->di_dx = true;
        break;
        
      case TT_DATALEG_NORTHING:
        if (this->di_dy) {
          err_duplicate = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_CARTESIAN) {
          err_itype = true;
          break;
        }
        this->di_dy = true;
        break;
        
      case TT_DATALEG_ALTITUDE:
        if (this->di_dz) {
          err_duplicate = true;
          break;
        }
        if (this->d_type != TT_DATATYPE_CARTESIAN) {
          err_itype = true;
          break;
        }
        this->di_dz = true;
        break;

      case TT_DATALEG_DIRECTION:
        if (this->di_direction) {
          err_duplicate = true;
          break;
        }
        if (this->d_type == TT_DATATYPE_DIMS) {
          err_itype = true;
          break;
        }
        this->di_direction = true;
        break;
        
      case TT_DATALEG_NEWLINE:
        if (this->di_newline) {
          err_duplicate = true;
          break;
        }
        if (this->d_type == TT_DATATYPE_DIMS) {
          err_itype = true;
          break;
        }
        if ((dix == 1) || (dix == nargs - 1))
          ththrow(("invalid newline position")) 
        this->di_newline = true;
        break;

      case TT_DATALEG_UP:
        if (this->di_up) {
          err_duplicate = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        this->di_up = true;
        break;
        
      case TT_DATALEG_DOWN:
        if (this->di_down) {
          err_duplicate = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        this->di_down = true;
        break;
        
      case TT_DATALEG_LEFT:
        if (this->di_left) {
          err_duplicate = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        this->di_left = true;
        break;
        
      case TT_DATALEG_RIGHT:
        if (this->di_right) {
          err_duplicate = true;
          break;
        }
        if (this->di_newline) {
          err_idanl = true;
          break;
        }
        this->di_right = true;
        break;

      case TT_DATALEG_IGNORE:
        break;

      case TT_DATALEG_IGNOREALL:
        break;        
        
      default:
        ththrow(("invalid identifier -- %s", args[dix]))
    }
    
    // catch errors
    if (err_duplicate)
      ththrow(("duplicate identifier -- %s", args[dix]))
    if (err_inimm)
      ththrow(("interleaved and non-interleaved reading mix -- %s", args[dix]))
    if (err_itype)
      ththrow(("invalid reading for this style -- %s", args[dix]))
    if (err_idanl)
      ththrow(("interleaved reading after newline -- %s", args[dix]))
   
    // if no errors, no ignoreall - set order  
    this->d_order[dix - 1] = idd;
    this->d_nitems++;
    if (idd == TT_DATALEG_IGNOREALL)
      break;
      
  }
  
  // check if all the data specified
  bool all_data = false;
  switch(this->d_type) {
    case TT_DATATYPE_NORMAL:
      all_data = (this->di_count ||
        (this->di_fromcount && this->di_tocount) ||
        this->di_length) && 
        (this->di_bearing || this->di_backbearing) && 
        (this->di_gradient || this->di_backgradient);
      break;
    case TT_DATATYPE_CYLPOLAR:
    case TT_DATATYPE_DIVING:
      all_data = this->di_length && (this->di_bearing || this->di_backbearing) 
        && (this->di_depth ||
        (this->di_fromdepth && this->di_todepth) || this->di_depthchange);
      break;
    case TT_DATATYPE_CARTESIAN:
      all_data = this->di_dx || this->di_dy || this->di_dz;
      break;
    case TT_DATATYPE_NOSURVEY:
      all_data = true;
      break;
    case TT_DATATYPE_DIMS:
      all_data = this->di_up || this->di_down || this->di_left || this->di_right;
      break;
  }
  all_data = all_data &&
      ((this->di_from && this->di_to) || this->di_station);
  if (!all_data)
    ththrow(("not all data for given style"))
   
  if (this->di_interleaved) {
    for(dix = 0; (!err_inimm) && (dix < this->d_nitems) && (this->d_order[dix] != TT_DATALEG_NEWLINE); dix++) {
      switch (this->d_order[dix]) {
        case TT_DATALEG_FROM:
        case TT_DATALEG_TO:
        case TT_DATALEG_DIRECTION:
        case TT_DATALEG_LENGTH:
        case TT_DATALEG_BEARING:
        case TT_DATALEG_BACKBEARING:
        case TT_DATALEG_GRADIENT:
        case TT_DATALEG_BACKGRADIENT:
        case TT_DATALEG_FROMCOUNT:
        case TT_DATALEG_TOCOUNT:
        case TT_DATALEG_FROMDEPTH:
        case TT_DATALEG_TODEPTH:
        case TT_DATALEG_DEPTHCHANGE:
        case TT_DATALEG_NORTHING:
        case TT_DATALEG_EASTING:
        case TT_DATALEG_ALTITUDE:
        case TT_DATALEG_X:
        case TT_DATALEG_Y:
        case TT_DATALEG_Z:
          err_inimm = true;
          break;
      }
      
    }
    if (err_inimm)
      ththrow(("non-interleaved data before newline -- %s", args[dix]))
  }
    
  
} // data data


void thdata_parse_dim(const char * src, double & d1, double & d2, 
  bool & d2ok, const char * item, thtfpwf * ctran, thtflength * utran) {

  int sv;  
  d1 = thnan;
  d2 = thnan;
  
  if (!d2ok) {
    thparse_double(sv, d1, src);
    if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
      ththrow(("invalid %s reading -- %s", item, src))
  } else {
    char ** args;
    long nargs;
    thsplit_args(& thdb.mbuff_tmp2, src);
    args = thdb.mbuff_tmp2.get_buffer();
    nargs = thdb.mbuff_tmp2.get_size();    
    d2ok = false;
    if ((nargs < 1) || (nargs > 2))
      ththrow(("invalid %s reading -- %s", item, src))
    if (nargs > 1) {
      d2ok = true;
      thparse_double(sv, d2, args[1]);
      if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
        ththrow(("invalid %s reading -- %s", item, args[1]))
    }
    thparse_double(sv, d1, args[0]);
    if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
      ththrow(("invalid %s reading -- %s", item, args[0]))
  }
  
  if (!thisnan(d1)) {
    d1 = ctran->evaluate(d1);
    d1 = utran->transform(d1);
    if (d1 < 0.0)
      ththrow(("negative %s reading -- %s", item, src))
  }
  if (!thisnan(d2)) {
    d2 = ctran->evaluate(d2);
    d2 = utran->transform(d2);
    if (d2 < 0.0)
      ththrow(("negative %s reading -- %s", item, src))
  }
  
}


void thdata::insert_data_leg(int nargs, char ** args)
{
  bool to_clear = true, ftgiven = false, ftorder = false;
  thdataleg dumm;
  thstdims * cdims = NULL;
  
#ifdef THDEBUG
  thdataleg * cdleg;
#endif

  int carg;
  
  if ((this->d_current == 0) && (this->d_type != TT_DATATYPE_DIMS)) {
  
    this->pd_leg = this->cd_leg;
    this->pd_leg_def = this->cd_leg_def;
    this->cd_leg = this->leg_list.insert(this->leg_list.end(),dumm);

#ifdef THDEBUG
    cdleg = &(*this->cd_leg);
#endif
    
    // set all the data
    this->cd_leg->srcf = this->db->csrc;
    this->cd_leg->is_valid = !(this->di_interleaved);
    this->cd_leg->data_type = this->d_type;
    this->cd_leg->s_mark = this->d_mark;
    this->cd_leg->flags = this->d_flags;
    this->cd_leg->extend = (unsigned char) this->d_extend;
    this->d_extend &= (TT_EXTENDFLAG_DIRECTION | TT_EXTENDFLAG_IGNORE | TT_EXTENDFLAG_HIDE);

    this->cd_leg->psurvey = this->db->get_current_survey();
    
    this->cd_leg->walls = this->d_walls;
    this->cd_leg->vtresh = this->d_vtresh;
    this->cd_leg->shape = this->d_shape;
    
    this->cd_leg->length_sd = this->dls_length;
    this->cd_leg->bearing_sd = this->dls_bearing;
    this->cd_leg->gradient_sd = this->dls_gradient;
    this->cd_leg->counter_sd = this->dls_counter;
    this->cd_leg->depth_sd = this->dls_depth;
    this->cd_leg->dx_sd = this->dls_dx;
    this->cd_leg->dy_sd = this->dls_dy;
    this->cd_leg->dz_sd = this->dls_dz;
    this->cd_leg->x_sd = this->dls_x;
    this->cd_leg->y_sd = this->dls_y;
    this->cd_leg->z_sd = this->dls_z;
    if (thisnan(this->dl_declination)) {
      if (!(this->dl_survey_declination_on)) 
        this->set_survey_declination();
      this->cd_leg->declination = this->dl_survey_declination;
    }
    else
      this->cd_leg->declination = this->dl_declination;
    this->cd_leg->infer_plumbs = this->dli_plumbs;
    this->cd_leg->infer_equates = this->dli_equates;
    
    this->cd_leg_def = true;
    if (this->di_newline)
      to_clear = false;
  }
  
  if (this->d_type == TT_DATATYPE_DIMS) {
    // vlozi novu dimension a nastavi pointer na nu
    cdims = &(*(this->dims_list.insert(this->dims_list.end(), thstdims())));
    cdims->psurvey = this->db->get_current_survey();
    cdims->srcf = this->db->csrc;
    to_clear = false;
  }
  
  // now set the data
  bool exit_new_line = false;
  int val_id;
  double val;
  
  ftgiven = this->di_from && this->di_to;
  if (ftgiven) {
    for(carg = 0; carg < this->d_nitems; carg++) {
      if (this->d_order[carg] == TT_DATALEG_FROM) {
        ftorder = true;
        break;
      } else if (this->d_order[carg] == TT_DATALEG_TO) {
        ftorder = false;
        break;
      }
    }
  }
  
  for(carg = 0;this->d_current < this->d_nitems;this->d_current++, carg++) {
  
    if ((carg == nargs) && 
        ((this->d_order[this->d_current] != TT_DATALEG_NEWLINE)
         && (this->d_order[this->d_current] != TT_DATALEG_IGNOREALL)))
      ththrow(("not enough data readings"))
  
    switch(this->d_order[this->d_current]) {
    
      case TT_DATALEG_STATION:
        if (this->d_type == TT_DATATYPE_DIMS) {
          thparse_objectname(cdims->station, &(this->db->buff_stations),
            args[carg], this);
        } else {
          thparse_objectname(this->cd_leg->station, &(this->db->buff_stations),
            args[carg], this);
        }
        break;
        
      case TT_DATALEG_FROM:
        thparse_objectname(this->cd_leg->from, &(this->db->buff_stations),
          args[carg], this);
        break;
        
      case TT_DATALEG_TO:
        thparse_objectname(this->cd_leg->to, &(this->db->buff_stations),
          args[carg], this);
        break;
        
      case TT_DATALEG_DIRECTION:
        if (strcasecmp(args[carg],"b") == 0)
          this->cd_leg->direction = false;
        else if (strcasecmp(args[carg], "f") != 0)
          ththrow(("invalid survey direction -- %s", args[carg]))
        break;
        
      case TT_DATALEG_LENGTH:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_length.evaluate(val);
            val = this->dlu_length.transform(val);
            if (val < 0.0)
              ththrow(("negative length reading -- %s", args[carg]))
            else
              this->cd_leg->length = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->length = val;
//            break;
          default:
            ththrow(("invalid length reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_BEARING:
        if (this->dlu_bearing.get_units() == TT_TFU_DEG)
          thparse_double_dms(val_id, val, args[carg]);
        else
          thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_bearing.evaluate(val);
            val = this->dlu_bearing.transform(val);
            if (val < 0.0)
              val += 360.0;
            if (val >= 360.0)
              val -= 360.0;
            if ((val < 0.0) || (val >= 360.0))
              ththrow(("bearing reading out of range -- %s", args[carg]))
            else
              this->cd_leg->bearing = val;
            break;
          case TT_SV_NAN:
            this->cd_leg->bearing = val;
            break;
          default:
            ththrow(("invalid bearing reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_BACKBEARING:
        if (this->dlu_bearing.get_units() == TT_TFU_DEG)
          thparse_double_dms(val_id, val, args[carg]);
        else
          thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_bearing.evaluate(val);
            val = this->dlu_bearing.transform(val);
            if (val < 0.0)
              val += 360.0;
            if (val >= 360.0)
              val -= 360.0;
            if ((val < 0.0) || (val >= 360.0))
              ththrow(("backwards bearing reading out of range -- %s", args[carg]))
            else
              this->cd_leg->backbearing = val;
            break;
          case TT_SV_NAN:
            this->cd_leg->backbearing = val;
            break;
          default:
            ththrow(("invalid backwards bearing reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_GRADIENT:
        if (this->dlu_gradient.get_units() == TT_TFU_DEG)
          thparse_double_dms(val_id, val, args[carg]);
        else
          thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_gradient.evaluate(val);
            val = this->dlu_gradient.transform(val);
            if ((val <= -90.00001) || (val >= 90.00001))
              ththrow(("gradient reading out of range -- %s", args[carg]))
            else
              this->cd_leg->gradient = val;
            break;
//          case TT_SV_NAN:
          case TT_SV_UP:
          case TT_SV_DOWN:
            this->cd_leg->gradient = val;
            break;
          default:
            ththrow(("invalid gradient reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_BACKGRADIENT:
        if (this->dlu_gradient.get_units() == TT_TFU_DEG)
          thparse_double_dms(val_id, val, args[carg]);
        else
          thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_gradient.evaluate(val);
            val = this->dlu_gradient.transform(val);
            if ((val <= -90.00001) || (val >= 90.00001))
              ththrow(("backwards gradient reading out of range -- %s", args[carg]))
            else
              this->cd_leg->backgradient = val;
            break;
//          case TT_SV_NAN:
          case TT_SV_UP:
          case TT_SV_DOWN:
            this->cd_leg->backgradient = val;
            break;
          default:
            ththrow(("invalid backwards gradient reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_COUNT:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_counter.evaluate(val);
            val = this->dlu_counter.transform(val);
            if (val < 0.0)
              ththrow(("negative counter reading -- %s", args[carg]))
            else
              this->cd_leg->counter = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->counter = val;
//            break;
          default:
            ththrow(("invalid counter reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_FROMCOUNT:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_counter.evaluate(val) - this->dlc_counter.evaluate(0.0);
            val = this->dlu_counter.transform(val);
            this->cd_leg->fromcounter = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->fromcounter = val;
//            break;
          default:
            ththrow(("invalid counter reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_TOCOUNT:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_counter.evaluate(val);
            val = this->dlu_counter.transform(val);
            this->cd_leg->tocounter = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->tocounter = val;
//            break;
          default:
            ththrow(("invalid counter reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_DEPTH:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_depth.evaluate(val);
            val = this->dlu_depth.transform(val);
            this->cd_leg->depth = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->depth = val;
//            break;
          default:
            ththrow(("invalid depth reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_FROMDEPTH:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_depth.evaluate(val) - this->dlc_depth.evaluate(0.0);
            val = this->dlu_depth.transform(val);
            this->cd_leg->fromdepth = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->fromdepth = val;
//            break;
          default:
            ththrow(("invalid depth reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_TODEPTH:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_depth.evaluate(val);
            val = this->dlu_depth.transform(val);
            this->cd_leg->todepth = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->todepth = val;
//            break;
          default:
            ththrow(("invalid depth reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_DEPTHCHANGE:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_depth.evaluate(val);
            val = this->dlu_depth.transform(val);
            this->cd_leg->depthchange = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->depthchange = val;
//            break;
          default:
            ththrow(("invalid depth change reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_NORTHING:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_dy.evaluate(val);
            val = this->dlu_dy.transform(val);
            this->cd_leg->dy = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->dy = val;
//            break;
          default:
            ththrow(("invalid northing reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_EASTING:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_dx.evaluate(val);
            val = this->dlu_dx.transform(val);
            this->cd_leg->dx = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->dx = val;
//            break;
          default:
            ththrow(("invalid easting reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_ALTITUDE:
        thparse_double(val_id, val, args[carg]);
        switch (val_id) {
          case TT_SV_NUMBER:
            val = this->dlc_dz.evaluate(val);
            val = this->dlu_dz.transform(val);
            this->cd_leg->dz = val;
            break;
//          case TT_SV_NAN:
//            this->cd_leg->dz = val;
//            break;
          default:
            ththrow(("invalid altitude reading -- %s", args[carg])) 
        }
        break;
        
      case TT_DATALEG_IGNOREALL:
        carg = nargs;
        to_clear = true;
      case TT_DATALEG_NEWLINE:
        this->d_current++;
        exit_new_line = true;
        break;

      // iba ak specifikovane from a to, tak moozu byt dve
      // hodnoty - inak nacitavame do to
        
      case TT_DATALEG_UP:
        if (this->d_type == TT_DATATYPE_DIMS) {
          ftgiven = false;
          thdata_parse_dim(args[carg], cdims->up, val, ftgiven, "up", 
            &this->dlc_up, &this->dlu_up);
        } else {
          ftgiven = this->di_from && this->di_to;
          thdata_parse_dim(args[carg], 
            this->cd_leg->to_up, this->cd_leg->from_up, ftgiven, "up", 
            &this->dlc_up, &this->dlu_up);
          if (ftgiven && ftorder) {
            val = this->cd_leg->to_up;
            this->cd_leg->to_up = this->cd_leg->from_up;
            this->cd_leg->from_up = val;
          }
        }
        break;
        
      case TT_DATALEG_DOWN:
        if (this->d_type == TT_DATATYPE_DIMS) {
          ftgiven = false;
          thdata_parse_dim(args[carg], cdims->down, val, ftgiven, "down", 
            &this->dlc_down, &this->dlu_down);
        } else {
          ftgiven = this->di_from && this->di_to;
          thdata_parse_dim(args[carg], 
            this->cd_leg->to_down, this->cd_leg->from_down, ftgiven, "down", 
            &this->dlc_down, &this->dlu_down);
          if (ftgiven && ftorder) {
            val = this->cd_leg->to_down;
            this->cd_leg->to_down = this->cd_leg->from_down;
            this->cd_leg->from_down = val;
          }
        }
        break;
        
      case TT_DATALEG_LEFT:
        if (this->d_type == TT_DATATYPE_DIMS) {
          ftgiven = false;
          thdata_parse_dim(args[carg], cdims->left, val, ftgiven, "left", 
            &this->dlc_left, &this->dlu_left);
        } else {
          ftgiven = this->di_from && this->di_to;
          thdata_parse_dim(args[carg], 
            this->cd_leg->to_left, this->cd_leg->from_left, ftgiven, "left", 
            &this->dlc_left, &this->dlu_left);
          if (ftgiven && ftorder) {
            val = this->cd_leg->to_left;
            this->cd_leg->to_left = this->cd_leg->from_left;
            this->cd_leg->from_left = val;
          }
        }
        break;
        
      case TT_DATALEG_RIGHT:
        if (this->d_type == TT_DATATYPE_DIMS) {
          ftgiven = false;
          thdata_parse_dim(args[carg], cdims->right, val, ftgiven, "right", 
            &this->dlc_right, &this->dlu_right);
        } else {
          ftgiven = this->di_from && this->di_to;
          thdata_parse_dim(args[carg], 
            this->cd_leg->to_right, this->cd_leg->from_right, ftgiven, "right", 
            &this->dlc_right, &this->dlu_right);
          if (ftgiven && ftorder) {
            val = this->cd_leg->to_right;
            this->cd_leg->to_right = this->cd_leg->from_right;
            this->cd_leg->from_right = val;
          }
        }
        break;
        
    }
    
    if (exit_new_line)
      break;
      
  }  // end of data setting
  
  if (carg < nargs)
    ththrow(("too many data readings"))
  
  if (to_clear) {
    if (this->di_fromcount && this->di_tocount && 
      (this->cd_leg->fromcounter > this->cd_leg->tocounter))
      ththrow(("negative counter difference"))
//    if ((this->di_gradient || this->di_backgradient) && (this->di_bearing || this->di_backbearing) &&
//      (!(thisnan(this->cd_leg->bearing) 
//      && thisnan(this->cd_leg->backbearing))) &&
//      (((thisinf(this->cd_leg->gradient) != 0) || 
//      (thisinf(this->cd_leg->backgradient) != 0))))
//      ththrow(("compass reading given on plumbed shot"))
    if ((this->di_gradient && this->di_backgradient) && 
      (((thisinf(this->cd_leg->gradient) != 0) &&
      (thisinf(this->cd_leg->backgradient) == 0)) ||
      ((thisinf(this->cd_leg->gradient) == 0) &&
      (thisinf(this->cd_leg->backgradient) != 0))))
      ththrow(("plumbed shot with a gradient reading"))
    this->d_current = 0;
    this->complete_interleaved_data();
  }

  if (this->d_type == TT_DATATYPE_DIMS) {
    this->d_current = 0;
  }
  
}


void thdata::complete_interleaved_data()
{
  if (!this->di_interleaved)
    return;
  if (!this->pd_leg_def)
    return;
  if (this->cd_leg->is_valid)
    return;
  
  // OK, set the station name
  if (this->di_station) {
    this->pd_leg->from = this->pd_leg->station;
    this->pd_leg->to = this->cd_leg->station;
  }
  
  if (this->di_up) {
    this->pd_leg->from_up = this->pd_leg->to_up;
    this->pd_leg->to_up = this->cd_leg->to_up;
  }
  if (this->di_down) {
    this->pd_leg->from_down = this->pd_leg->to_down;
    this->pd_leg->to_down = this->cd_leg->to_down;
  }
  if (this->di_left) {
    this->pd_leg->from_left = this->pd_leg->to_left;
    this->pd_leg->to_left = this->cd_leg->to_left;
  }
  if (this->di_right) {
    this->pd_leg->from_right = this->pd_leg->to_right;
    this->pd_leg->to_right = this->cd_leg->to_right;
  }

  // set the counter
  if (this->di_count) {
    this->pd_leg->fromcounter = this->pd_leg->counter;
    this->pd_leg->tocounter = this->cd_leg->counter;
    if ((!thisnan(this->pd_leg->fromcounter)) &&
      (!thisnan(this->pd_leg->tocounter)) &&
      (this->pd_leg->fromcounter > this->pd_leg->tocounter))
      ththrow(("negative counter difference"))
  }
  
  // set the depth
  if (this->di_depth) {
    this->pd_leg->fromdepth = this->pd_leg->depth;
    this->pd_leg->todepth = this->cd_leg->depth;
  }
  
  this->pd_leg->topofil = (this->di_count || this->di_fromcount || this->di_tocount);
  this->pd_leg->is_valid = true;
  
}

void thdata::start_insert()
{
  if (this->cgroup->ugroup != NULL)
    ththrow(("missing endgroup"))
  this->complete_interleaved_data();
}


void thdata::set_data_fix(int nargs, char ** args)
{
  int ai, vid;
  double val;
  thdatafix dumm;
  thdatafix_list::iterator it;
  switch(nargs) {
    case 4:
    case 5:
    case 6:
    case 7:
//    case 10:
      break;
    default:
      ththrow(("invalid number of fix option arguments"))
  }
  it = this->fix_list.insert(this->fix_list.end(),dumm);
  it->srcf = this->db->csrc;
  it->psurvey = this->db->get_current_survey();
  it->sdx = thnan;
  it->sdy = thnan;
  it->sdz = thnan;
  this->convert_cs(args[1], args[2], it->x, it->y);
  for(ai = 0; ai < nargs; ai++) {
    if (ai > 2) {
      thparse_double(vid, val, args[ai]);
      if (vid != TT_SV_NUMBER)
        ththrow(("invalid number -- %s", args[ai]))
    }
    switch (ai) {
      case 0:
        thparse_objectname(it->station, & this->db->buff_stations, args[0], this);
        break;
      case 1:
        if (this->cs == TTCS_LOCAL) {
          val = this->dlc_x.evaluate(it->x);
          val = this->dlu_x.transform(val);
          it->x = val; 
        }
        break;
      case 2:
        if (this->cs == TTCS_LOCAL) {
          val = this->dlc_y.evaluate(it->y);
          val = this->dlu_y.transform(val);
          it->y = val;
        }
        break;
      case 3:
        val = this->dlc_z.evaluate(val);
        val = this->dlu_z.transform(val);
        it->z = val;
        break;
      case 4:
        val = this->dlu_x.transform(val);
        it->sdx = val;
        it->sdy = val;
        it->sdz = val;
        break;
      case 5:
        if (nargs == 6) {
          val = this->dlu_z.transform(val);
          it->sdz = val;
        }
        else {
          val = this->dlu_y.transform(val);
          it->sdy = val;
        }
        break;
      case 6:
        val = this->dlu_z.transform(val);
        it->sdz = val;
        break;
      case 7:
        val = this->dlu_x.transform(val);
        val = this->dlu_y.transform(val);
        it->cxy = val;
        break;
      case 8:
        val = this->dlu_y.transform(val);
        val = this->dlu_z.transform(val);
        it->cyz = val;
        break;
      case 9:
        val = this->dlu_x.transform(val);
        val = this->dlu_z.transform(val);
        it->cxz = val;
        break;
    }
  }
}

  
void thdata::set_data_equate(int nargs, char ** args)
{
  int eid, cid = ++this->d_last_equate;
  thdataequate dumm;
  thdataequate_list::iterator it;
  for(eid = 0; eid < nargs; eid++) {
    it = this->equate_list.insert(this->equate_list.end(),dumm);
    thparse_objectname(it->station, & this->db->buff_stations, args[eid], this);
    it->eqid = cid;
    it->srcf = this->db->csrc;
    it->psurvey = this->db->get_current_survey();
  } 
}
  

#define setstflag(casev, flagv) case casev: \
            if (notflag) \
                it->flags &= ~flagv; \
            else \
                it->flags |= flagv; \
            break


void thdata::set_data_station(int nargs, char ** args, int argenc)
{
  int ai, fid, sv;
  thdatass_list::iterator it;
  thdatass dumm;
  std::string attrname;
  bool notflag = false;
  it = this->ss_list.insert(this->ss_list.end(),dumm);
  it->srcf = this->db->csrc;
  it->psurvey = this->db->get_current_survey();
  for(ai = 0; ai < nargs; ai++) {
    switch (ai) {
      case 0:
        thparse_objectname(it->station, & this->db->buff_stations, args[0], this);
        break;
      case 1:
        if (strlen(args[1]) > 0) {
          thencode(&(this->db->buff_enc), args[1], argenc);
          it->comment = this->db->strstore(this->db->buff_enc.get_buffer());
        }
        break;
      default:
        fid = thmatch_token(args[ai], thtt_datasflag);
        switch (fid) {
          case TT_DATASFLAG_ENTRANCE:
            if (notflag)
                it->flags &= ~TT_STATIONFLAG_ENTRANCE;
            else
                it->flags |= TT_STATIONFLAG_ENTRANCE;
            break;

          setstflag(TT_DATASFLAG_AIRDRAUGHT, (TT_STATIONFLAG_AIRDRAUGHT_SUMMER + TT_STATIONFLAG_AIRDRAUGHT_WINTER));
          setstflag(TT_DATASFLAG_AIRDRAUGHT_SUMMER, TT_STATIONFLAG_AIRDRAUGHT_SUMMER);
          setstflag(TT_DATASFLAG_AIRDRAUGHT_WINTER, TT_STATIONFLAG_AIRDRAUGHT_WINTER);
          setstflag(TT_DATASFLAG_SINK, TT_STATIONFLAG_SINK);
          setstflag(TT_DATASFLAG_SPRING, TT_STATIONFLAG_SPRING);
          setstflag(TT_DATASFLAG_DOLINE, TT_STATIONFLAG_DOLINE);
          setstflag(TT_DATASFLAG_DIG, TT_STATIONFLAG_DIG);

          case TT_DATASFLAG_FIXED:              
            if (notflag)
                it->flags |= TT_STATIONFLAG_NOTFIXED;
            else
              ththrow(("you can not set fixed station flag directly - fix command needs to be used for this"));
            break;
          case TT_DATASFLAG_CONT:
            if (notflag) {
		          it->explored = thnan;
              it->flags &= ~TT_STATIONFLAG_CONT;
            } else
              it->flags |= TT_STATIONFLAG_CONT;
            break;
          //case TT_DATASFLAG_CODE:
          //  if ((it->flags & TT_STATIONFLAG_CONT) == 0)
          //    ththrow(("missing continuation flag before code"));
          //  if (notflag) {
          //      it->code = NULL;
          //      break;
          //  }
          //  if ((ai + 1) == nargs)
          //    ththrow(("too few flags - missing continuation code"));
          //  ai++;
          //  if (strlen(args[ai]) == 0) {
          //    it->code = NULL;
          //  } else {
          //    thencode(&(this->db->buff_enc), args[ai], argenc);
          //    it->code = this->db->strstore(this->db->buff_enc.get_buffer());
          //  }
          //  break;
          case TT_DATASFLAG_EXPLORED:
            if ((it->flags & TT_STATIONFLAG_CONT) == 0)
              ththrow(("missing continuation flag before explored length"));
            if (notflag) {
                it->explored = thnan;
                break;
            }
            if ((ai + 1) == nargs)
              ththrow(("too few flags - missing explored length"));
            ai++;
            if (strlen(args[ai]) == 0) {
              it->explored = thnan;
            } else {
	            thparse_length(sv, it->explored, args[ai]);
	            if (sv == TT_SV_UNKNOWN)
                      ththrow(("invalid explored length specification -- %s", args[ai]));
            }
            break;

          case TT_DATASFLAG_ATTR:              
            if ((ai + 1) == nargs)
              ththrow(("too few flags - missing attribute name"));
            ai++;
            if (strlen(args[ai]) == 0)
              ththrow(("epmty attribute name not allowed"))
            if (args[ai][0] == '_')
              ththrow(("attribute name starting with '_' not allowed -- %s", args[ai]))
            if (!th_is_attr_name(args[ai]))
              ththrow(("invalid characters in attribute name -- %s", args[ai]))
            attrname = args[ai];
            if (notflag) {
                it->attr.erase(attrname);
                break;
            }
            if ((ai + 2) == nargs)
              ththrow(("too few flags - missing attribute value"));
            ai++;            
            if (strlen(args[ai]) > 0) {
              thencode(&(this->db->buff_enc), args[ai], argenc);
              it->attr[attrname] = this->db->strstore(this->db->buff_enc.get_buffer());
            } else {
              it->attr.erase(attrname);
            }
            break;

          case TT_DATASFLAG_NOT:
            break;
          default:
            ththrow(("invalid station flag -- %s", args[ai]))
        }
        if (fid == TT_DATASFLAG_NOT)
            notflag = true;
        else
            notflag = false;
        break;
    }
  }
}
  

void thdata::set_data_flags(int nargs, char ** args)
{
  int fid, ffl;
  bool notb  = false;
  for (fid = 0; fid < nargs; fid++) {
    ffl = thmatch_token(args[fid], thtt_datalflag);
    switch (ffl) {
      case TT_DATALFLAG_NOT:
        if ((fid < nargs - 1) && (!notb))
          notb = true;
        else
          ththrow(("wrong not context"))
        break;
      case TT_DATALFLAG_SURFACE:
        if (notb)
          this->d_flags &= ~TT_LEGFLAG_SURFACE;
        else
          this->d_flags |= TT_LEGFLAG_SURFACE;
        notb = false;
        break;
      case TT_DATALFLAG_SPLAY:
        if (notb)
          this->d_flags &= ~TT_LEGFLAG_SPLAY;
        else
          this->d_flags |= TT_LEGFLAG_SPLAY;
        notb = false;
        break;
      case TT_DATALFLAG_APPROXIMATE:
        if (notb)
          this->d_flags &= ~TT_LEGFLAG_APPROXIMATE;
        else
          this->d_flags |= TT_LEGFLAG_APPROXIMATE;
        notb = false;
        break;
      case TT_DATALFLAG_DUPLICATE:
        if (notb)
          this->d_flags &= ~TT_LEGFLAG_DUPLICATE;
        else
          this->d_flags |= TT_LEGFLAG_DUPLICATE;
        notb = false;
        break;
      default:
        ththrow(("unknown leg flag -- %s", args[fid]))
    }
  }
}

  
void thdata::set_data_extend(int nargs, char ** args)
{
  int cextend;
  thdataextend dumm;
  cextend = thmatch_token(args[0], thtt_extendflag);
  if (cextend == TT_EXTENDFLAG_UNKNOWN)
    ththrow(("unknown extend flag -- %s", args[0]))
  switch (nargs) {
    case 1:
      if ((cextend & TT_EXTENDFLAG_DIRECTION) != 0) {
        this->d_extend &= ~TT_EXTENDFLAG_DIRECTION;
      }
      this->d_extend |= cextend;
      break;
    case 3:
      // parsnut mena to bodu
	    thparse_objectname(dumm.to, & this->db->buff_stations, args[2], this);
    case 2:
      // parsnut meno from bodu a prida
	    thparse_objectname(dumm.from, & this->db->buff_stations, args[1], this);
      dumm.extend = cextend;
      dumm.srcf = this->db->csrc;
      dumm.psurvey = this->db->get_current_survey();
      this->extend_list.insert(this->extend_list.end(), dumm);
      break;
    default:
      ththrow(("invalid extend specification"))
  }


}

  
void thdata::set_data_mark(int nargs, char ** args)
{
  if (nargs < 1)
	  ththrow(("missing option arguments -- mark"));

  int tmp_mark = thmatch_token(args[nargs - 1], thtt_datamark);
	if (tmp_mark == TT_DATAMARK_UNKNOWN) {
	  ththrow(("unknown type of station mark -- %s", args[nargs - 1]))
	}
		
  if (nargs > 1) {
		thdatamark dumm;
		thdatamark_list::iterator it;
		int mi;
	  for(mi = 0; mi < (nargs - 1); mi++) {
    	it = this->mark_list.insert(this->mark_list.end(),dumm);
      it->mark = tmp_mark;
	    thparse_objectname(it->station, & this->db->buff_stations, args[mi], this);
      it->srcf = this->db->csrc;
      it->psurvey = this->db->get_current_survey();
    } 
	} else
	  this->d_mark = tmp_mark;
}


void thdata::set_data_grade(int nargs, char ** args) 
{
  thgrade * cgrd;
  int gid;
  this->reset_data_sd();
  for (gid = 0; gid < nargs; gid++) {
    cgrd = this->db->get_grade(args[gid]);
    if (cgrd != NULL)
      cgrd->update_data_sd(this);
    else
      ththrow(("unknown survey grade -- %s", args[gid]))
  }
}


const char * thdata::get_cmd_name()
{
  return "centreline";
}


void thdata::set_survey_declination()
{
  double ad = 0, admin, admax;
  thsurvey * csptr;
  bool addef = false;
  thtfpwfxy * decdata;
  size_t ndecdata, idd;
  
  // calculate average survey date
  if (this->date.is_defined()) {
    addef = true;
    if (this->date.is_interval())
      ad = (this->date.get_start_year() + this->date.get_end_year()) / 2.0;
    else
      ad = this->date.get_start_year();
  } 
  else {
    addef = false;
  }
  
  // find nearest survey, where declination is defined
  if (this->fsptr != NULL)
    csptr = this->fsptr;
  else
    csptr = this->db->get_current_survey();
  
  bool hasdec = false;
  
  if (csptr != NULL) {  
    hasdec = csptr->get_decdef();
    while((csptr != NULL) && (!hasdec)) {
      csptr = csptr->get_father_survey();
      if (csptr != NULL)
        hasdec = csptr->get_decdef();
    }
  }
    
  // set survey declination
  if (hasdec) {
    if ((!addef) && (!thisnan(csptr->get_decuds()))) {
      this->dl_survey_declination = csptr->get_decuds();
    } 
    else {
      if (!addef) {
        // let's calculate ad in the middle of defined interval
        ndecdata = csptr->get_declin()->get_size();
        decdata = csptr->get_declin()->get_values();
        admin = decdata->x;
        admax = decdata->y;
        for (idd = 1; idd < ndecdata; idd++) {
          if (decdata[idd].x < admin)
            admin = decdata[idd].x;
          if (decdata[idd].x > admax)
            admax = decdata[idd].x;
        }
        ad = (admin + admax) / 2.0;
      }
      this->dl_survey_declination = csptr->get_declin()->evaluate(ad);
      if (!addef)
      thwarning(("%s [%d] -- no declination specified for undated survey data -- using average (%.2f degrees)",
        thdbreader.get_cinf()->get_cif_name(),
        thdbreader.get_cinf()->get_cif_line_number(),
        this->dl_survey_declination));
    }
  }
  else
    this->dl_survey_declination = thnan;
  dl_survey_declination_on = true;
}

void thdata::start_group() {
  thdata * tmp = new thdata;

  tmp->ugroup = this->cgroup;
  this->cgroup->complete_interleaved_data();
  this->cgroup->cd_leg_def = false;
  this->cgroup->d_current = 0;
  
  tmp->db = this->db;

  // nastavi secko tak, ako to mame nastavene teraz
  tmp->dlu_length = this->cgroup->dlu_length;
  tmp->dlu_counter = this->cgroup->dlu_counter; 
  tmp->dlu_depth = this->cgroup->dlu_depth; 
  tmp->dlu_dx = this->cgroup->dlu_dx; 
  tmp->dlu_dy = this->cgroup->dlu_dy; 
  tmp->dlu_dz = this->cgroup->dlu_dz;
  tmp->dlu_x = this->cgroup->dlu_x; 
  tmp->dlu_y = this->cgroup->dlu_y; 
  tmp->dlu_z = this->cgroup->dlu_z; 
  tmp->dlu_sdlength = this->cgroup->dlu_sdlength;
  
  tmp->dlu_bearing = this->cgroup->dlu_bearing; 
  tmp->dlu_gradient = this->cgroup->dlu_gradient; 
  tmp->dlu_declination = this->cgroup->dlu_declination; 
  tmp->dlu_sdangle = this->cgroup->dlu_sdangle;
  
  tmp->dlc_length = this->cgroup->dlc_length; 
  tmp->dlc_gradient = this->cgroup->dlc_gradient; 
  tmp->dlc_bearing = this->cgroup->dlc_bearing; 
  tmp->dlc_counter = this->cgroup->dlc_counter; 
  tmp->dlc_depth = this->cgroup->dlc_depth;
  tmp->dlc_dx = this->cgroup->dlc_dx; 
  tmp->dlc_dy = this->cgroup->dlc_dy; 
  tmp->dlc_dz = this->cgroup->dlc_dz; 
  tmp->dlc_x = this->cgroup->dlc_x; 
  tmp->dlc_y = this->cgroup->dlc_y; 
  tmp->dlc_z = this->cgroup->dlc_z; 
  tmp->dlc_default = this->cgroup->dlc_default;
    
  // dls - data standard deviation and declination
  tmp->dls_length = this->cgroup->dls_length; 
  tmp->dls_gradient = this->cgroup->dls_gradient; 
  tmp->dls_bearing = this->cgroup->dls_bearing; 
  tmp->dls_counter = this->cgroup->dls_counter; 
  tmp->dls_depth = this->cgroup->dls_depth;
  tmp->dls_dx = this->cgroup->dls_dx; 
  tmp->dls_dy = this->cgroup->dls_dy; 
  tmp->dls_dz = this->cgroup->dls_dz; 
  tmp->dls_x = this->cgroup->dls_x; 
  tmp->dls_y = this->cgroup->dls_y; 
  tmp->dls_z = this->cgroup->dls_z; 
  tmp->dl_declination = this->cgroup->dl_declination;
  tmp->dl_survey_declination = this->cgroup->dl_survey_declination;
    
  tmp->dli_plumbs = this->cgroup->dli_plumbs; 
  tmp->dli_equates = this->cgroup->dli_equates; 
  tmp->dl_direction = this->cgroup->dl_direction;

  // what is inserted
  tmp->di_station = this->cgroup->di_station; 
  tmp->di_from = this->cgroup->di_from; 
  tmp->di_to = this->cgroup->di_to; 
  tmp->di_length = this->cgroup->di_length; 
  tmp->di_bearing = this->cgroup->di_bearing; 
  tmp->di_gradient = this->cgroup->di_gradient;
  tmp->di_backbearing = this->cgroup->di_backbearing; 
  tmp->di_backgradient = this->cgroup->di_backgradient;
  tmp->di_depth = this->cgroup->di_depth; 
  tmp->di_fromdepth = this->cgroup->di_fromdepth; 
  tmp->di_todepth = this->cgroup->di_todepth; 
  tmp->di_depthchange = this->cgroup->di_depthchange; 
  tmp->di_count = this->cgroup->di_count; 
  tmp->di_fromcount = this->cgroup->di_fromcount;
  tmp->di_tocount = this->cgroup->di_tocount; 
  tmp->di_dx = this->cgroup->di_dx; 
  tmp->di_dy = this->cgroup->di_dy; 
  tmp->di_dz = this->cgroup->di_dz; 
  tmp->di_direction = this->cgroup->di_direction; 
  tmp->di_newline = this->cgroup->di_newline; 
  tmp->di_interleaved = this->cgroup->di_interleaved;
  tmp->di_up = this->cgroup->di_up; 
  tmp->di_down = this->cgroup->di_down; 
  tmp->di_left = this->cgroup->di_left; 
  tmp->di_right = this->cgroup->di_right;
  
  tmp->dl_survey_declination_on = this->cgroup->dl_survey_declination_on;
  
  tmp->d_type = this->cgroup->d_type;
  for (int i = 0; i < THDATA_MAX_ITEMS; i++)
    tmp->d_order[i] = this->cgroup->d_order[i];
  tmp->d_nitems = this->cgroup->d_nitems;
  tmp->d_mark = this->cgroup->d_mark;
  tmp->d_flags = this->cgroup->d_flags;
  tmp->d_extend = this->cgroup->d_extend & 
    (TT_EXTENDFLAG_DIRECTION | TT_EXTENDFLAG_IGNORE | TT_EXTENDFLAG_HIDE);
  tmp->d_last_equate = this->cgroup->d_last_equate;
  
  tmp->d_vtresh = this->cgroup->d_vtresh;
  tmp->d_walls = this->cgroup->d_walls;
  tmp->d_shape = this->cgroup->d_shape;

  this->cgroup = tmp;
}


void thdata::end_group() {
  if (this->cgroup->ugroup == NULL) {
    ththrow(("endgroup without startgroup"))
  }
  thdata * tmp = this->cgroup;
  this->cgroup = this->cgroup->ugroup;
  tmp->complete_interleaved_data();
  
  // do cgroupu prida vsetky zoznamy ktore existuju
  // teda stations, fixes, equates a legs
  // survey data
  for(thdataleg_list::iterator li = tmp->leg_list.begin();
    li != tmp->leg_list.end(); li++)
    if (li->is_valid) {
      this->cgroup->leg_list.insert(this->cgroup->leg_list.end(), (*li));
    }
  // fixed stations
  for(thdatafix_list::iterator fxi = tmp->fix_list.begin();
    fxi != tmp->fix_list.end(); fxi++) {
    this->cgroup->fix_list.insert(this->cgroup->fix_list.end(), (*fxi));
  }
  // extend specs
  for(thdataextend_list::iterator xxi = tmp->extend_list.begin();
    xxi != tmp->extend_list.end(); xxi++) {
    this->cgroup->extend_list.insert(this->cgroup->extend_list.end(), (*xxi));
  }
	// marks
	for(thdatamark_list::iterator mi = tmp->mark_list.begin();
		mi != tmp->mark_list.end(); mi++) {
    this->cgroup->mark_list.insert(this->cgroup->mark_list.end(), (*mi));
  }
  // equates
  for(thdataequate_list::iterator ei = tmp->equate_list.begin();
    ei != tmp->equate_list.end(); ei++) {
    this->cgroup->equate_list.insert(this->cgroup->equate_list.end(), (*ei));
  }
  // stations
  for(thdatass_list::iterator si = tmp->ss_list.begin();
    si != tmp->ss_list.end(); si++) {
    this->cgroup->ss_list.insert(this->cgroup->ss_list.end(), (*si));
  }
  // dimensions
  for(thstdims_list::iterator di = tmp->dims_list.begin();
    di != tmp->dims_list.end(); di++) {
    this->cgroup->dims_list.insert(this->cgroup->dims_list.end(), (*di));
  }
  
  tmp->self_delete();
}


struct thdatadimrec {
  double u,d,l,r;
  thdatadimrec() : u(thnan), d(thnan), l(thnan), r(thnan) {}
};


typedef std::map<long, thdatadimrec> thdatadimmap;


#define setdims(sU,sD,sL,sR,tU,tD,tL,tR) {\
  if (!thisnan(sU)) tU = sU; \
  if (!thisnan(sD)) tD = sD; \
  if (!thisnan(sL)) tL = sL; \
  if (!thisnan(sR)) tR = sR; \
  }

#define adddims(sU,sD,sL,sR,tU,tD,tL,tR) {\
  if (thisnan(tU)) tU = sU; \
  if (thisnan(tD)) tD = sD; \
  if (thisnan(tL)) tL = sL; \
  if (thisnan(tR)) tR = sR; \
  }


void thdata::complete_dimensions()
{

  thdatadimmap dm, idm;
  thdatadimmap::iterator dmi;
  thdatadimrec dr;
  thdataleg_list::iterator li, pli;

  // vytvori explicitny dim map
  for(thstdims_list::iterator di = this->dims_list.begin();
    di != this->dims_list.end(); di++) {
    dr = dm[di->station.id];
    setdims(di->up,di->down,di->left,di->right,dr.u,dr.d,dr.l,dr.r);
    dm[di->station.id] = dr;
  }

#define start_leg_cycle \
    for(li = this->leg_list.begin(); li != this->leg_list.end(); li++) \
      if (li->is_valid) {

#define end_leg_cycle }
  
  // vytvori implicitny dim map
  start_leg_cycle
    dr = idm[li->from.id];
    setdims(li->from_up, li->from_down, li->from_left, li->from_right, dr.u, dr.d, dr.l, dr.r)
    idm[li->from.id] = dr;
    dr = idm[li->to.id];
    setdims(li->to_up, li->to_down, li->to_left, li->to_right, dr.u, dr.d, dr.l, dr.r)
    idm[li->to.id] = dr;
  end_leg_cycle
  

  // priradi explicitny dim map
  if (this->dims_list.size() > 0) {
    start_leg_cycle  
      dmi = dm.find(li->from.id);
      if (dmi != dm.end()) adddims(dmi->second.u, dmi->second.d, dmi->second.l, dmi->second.r, li->from_up, li->from_down, li->from_left, li->from_right);
      dmi = dm.find(li->to.id);
      if (dmi != dm.end()) adddims(dmi->second.u, dmi->second.d, dmi->second.l, dmi->second.r, li->to_up, li->to_down, li->to_left, li->to_right);
    end_leg_cycle
  }

  pli = this->leg_list.end();
  start_leg_cycle
    // ak je zadany aspon jeden udaj
    if ((!thisnan(li->from_up)) || (!thisnan(li->from_down)) ||
        (!thisnan(li->from_left)) || (!thisnan(li->from_right)) ||
        (!thisnan(li->to_up)) || (!thisnan(li->to_down)) ||
        (!thisnan(li->to_left)) || (!thisnan(li->to_right))) {

      // predchadzajuca zamera
      if ((pli != this->leg_list.end()) && (pli->to.id == li->from.id))
        adddims(pli->to_up, pli->to_down, pli->to_left, pli->to_right, li->from_up, li->from_down, li->from_left, li->from_right);
      if ((pli != this->leg_list.end()) && (pli->from.id == li->from.id))
        adddims(pli->from_up, pli->from_down, pli->from_left, pli->from_right, li->from_up, li->from_down, li->from_left, li->from_right);

      // impl. dim map        
      dmi = idm.find(li->from.id);
      if (dmi != idm.end()) adddims(dmi->second.u, dmi->second.d, dmi->second.l, dmi->second.r, li->from_up, li->from_down, li->from_left, li->from_right);
      dmi = idm.find(li->to.id);
      if (dmi != idm.end()) adddims(dmi->second.u, dmi->second.d, dmi->second.l, dmi->second.r, li->to_up, li->to_down, li->to_left, li->to_right);
      
      // prev station/next station
      adddims(li->to_up, li->to_down, li->to_left, li->to_right, li->from_up, li->from_down, li->from_left, li->from_right);
      adddims(li->from_up, li->from_down, li->from_left, li->from_right, li->to_up, li->to_down, li->to_left, li->to_right);
      
      pli = li;
    }
  end_leg_cycle
  
  // TODO: pre kazdy rez, kde aspon niekde nieco - podoplna chybajuce
  // a nastavi walls na off kde nic nenajde
  start_leg_cycle
    adddims(li->to_down, li->to_up, li->to_right, li->to_left, li->to_up, li->to_down, li->to_left, li->to_right);
    adddims(li->to_left, li->to_left, li->to_up, li->to_up, li->to_up, li->to_down, li->to_left, li->to_right);
    adddims(li->from_down, li->from_up, li->from_right, li->from_left, li->from_up, li->from_down, li->from_left, li->from_right);
    adddims(li->from_left, li->from_left, li->from_up, li->from_up, li->from_up, li->from_down, li->from_left, li->from_right);
    if ((thisnan(li->to_up)) || (thisnan(li->from_up)))
      li->walls = TT_FALSE;
  end_leg_cycle
  

}







