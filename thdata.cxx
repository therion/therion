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
#include <string.h>
#include "thdatareader.h"

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
  this->stat_slength = 0.0;
  
  this->stat_st_state = 0;
  this->stat_st_top = NULL;
  this->stat_st_bottom = NULL;
  
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


char * thdata::get_cmd_end()
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


thcmd_option_desc thdata::get_cmd_option_desc(char * opts)
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
      case TT_DATA_DATA:
      case TT_DATA_STATION:
      case TT_DATA_EQUATE:
        return thcmd_option_desc(id,2);
      default:
        return thcmd_option_desc(id,1);
      case TT_DATA_BREAK:
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
      this->insert_data_leg(this->db->mbuff_tmp.get_size(),
        this->db->mbuff_tmp.get_buffer());
      break;
      
    case TT_DATA_FIX:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- fix"))
      this->set_data_fix(cod.nargs, args);
      break;
      
    case TT_DATA_EQUATE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- equate"))
      this->set_data_equate(cod.nargs, args);
      break;
      
    case TT_DATA_FLAGS:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- flags"))
      this->set_data_flags(cod.nargs, args);
      break;
      
    case TT_DATA_STATION:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- station"))
      this->set_data_station(cod.nargs, args, argenc);
      break;
      
    case TT_DATA_MARK:
      this->set_data_mark(cod.nargs, args);
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
      
    case TT_DATA_INSTRUMENT:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- instrument"))
      this->set_data_instrument(cod.nargs, args);
      break;
      
    case TT_DATA_SD:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- sd"))
      this->set_data_sd(cod.nargs, args);
      break;
      
    case TT_DATA_DATA:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- data"))
      this->set_data_data(cod.nargs, args);
      break;
      
    case TT_DATA_INFER:
      this->set_data_infer(cod.nargs, args);
      break;
      
    case TT_DATA_DECLINATION:
      this->set_data_declination(cod.nargs, args);
      break;
      
    case TT_DATA_UNITS:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- units"))
      this->set_data_units(cod.nargs, args);
      break;
      
    case TT_DATA_GRADE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- grade"))
      this->set_data_grade(cod.nargs, args);
      break;
        
    case TT_DATA_BREAK:
      if (cod.nargs > 0)
        ththrow(("no arguments allowed after break"))
      this->complete_interleaved_data();
      this->cd_leg_def = false;
      this->d_current = 0;
      break;
    
    case TT_DATA_CALIBRATE:
      if ((indataline & THOP_INLINE) == 0)
        ththrow(("not a command line option -- calibrate"))
      this->set_data_calibration(cod.nargs, args);
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
        ththrow(("not a command line option -- discovery-team"))
      thencode(&(this->db->buff_enc), *args, argenc);
      temp_person.parse(this->db, this->db->buff_enc.get_buffer());
      this->discovery_team_set.insert(temp_person);
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
    }
  }
}  // data calibration
  

void thdata::set_data_units(int nargs, char ** args)
{
  static int items[THDATA_MAX_ITEMS];
  int nitems, iit, itid, sdtype = 0; // 1-length, 2-angle

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
      case TT_DATALEG_POSITION:
      case TT_DATALEG_X:
      case TT_DATALEG_EASTING:
      case TT_DATALEG_Y:
      case TT_DATALEG_NORTHING:
      case TT_DATALEG_Z:
      case TT_DATALEG_ALTITUDE:
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
        break;
      case TT_DATALEG_GRADIENT:
        this->dlu_gradient = this->dlu_sdangle;
        break;
    }
  }
}  // data units
  

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
        this->di_direction = true;
        break;
        
      case TT_DATALEG_NEWLINE:
        if (this->di_newline) {
          err_duplicate = true;
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
        this->di_up = true;
        break;
        
      case TT_DATALEG_DOWN:
        if (this->di_down) {
          err_duplicate = true;
          break;
        }
        this->di_down = true;
        break;
        
      case TT_DATALEG_LEFT:
        if (this->di_left) {
          err_duplicate = true;
          break;
        }
        this->di_left = true;
        break;
        
      case TT_DATALEG_RIGHT:
        if (this->di_right) {
          err_duplicate = true;
          break;
        }
        this->di_right = true;
        break;

      case TT_DATALEG_IGNORE:
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
    // if no errors, set order  
    this->d_order[dix - 1] = idd;
    this->d_nitems++;
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
  }
  all_data = all_data &&
      ((this->di_from && this->di_to) || this->di_station);
  if (!all_data)
    ththrow(("not all data for given style"))
  
} // data data


void thdata::insert_data_leg(int nargs, char ** args)
{
  bool to_clear = true;
  thdataleg dumm;
  int carg;
  if (this->d_current == 0) {
    this->pd_leg = this->cd_leg;
    this->pd_leg_def = this->cd_leg_def;
    this->cd_leg = this->leg_list.insert(this->leg_list.end(),dumm);
    
    // set all the data
    this->cd_leg->srcf = this->db->csrc;
    this->cd_leg->is_valid = !(this->di_interleaved);
    this->cd_leg->data_type = this->d_type;
    this->cd_leg->s_mark = this->d_mark;
    this->cd_leg->flags = this->d_flags;
    this->cd_leg->psurvey = this->db->get_current_survey();
    
    this->cd_leg->length_sd = this->dls_length;
    this->cd_leg->bearing_sd = this->dls_bearing;
    this->cd_leg->gradient_sd = this->dls_gradient;
    this->cd_leg->counter_sd = this->dls_counter;
    this->cd_leg->depth_sd = this->dls_depth;
    this->cd_leg->dx_sd = this->dls_dx;
    this->cd_leg->dy_sd = this->dls_dy;
    this->cd_leg->dz_sd = this->dls_dz;
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
  
  // now set the data
  bool exit_new_line = false;
  int val_id;
  double val;
  for(carg = 0;this->d_current < this->d_nitems;this->d_current++, carg++) {
  
    if ((carg == nargs) && 
        (this->d_order[this->d_current] != TT_DATALEG_NEWLINE))
      ththrow(("not enought data readings"))
  
    switch(this->d_order[this->d_current]) {
    
      case TT_DATALEG_STATION:
        thparse_objectname(this->cd_leg->station, &(this->db->buff_stations),
          args[carg]);
        break;
        
      case TT_DATALEG_FROM:
        thparse_objectname(this->cd_leg->from, &(this->db->buff_stations),
          args[carg]);
        break;
        
      case TT_DATALEG_TO:
        thparse_objectname(this->cd_leg->to, &(this->db->buff_stations),
          args[carg]);
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
            val = this->dlc_counter.evaluate(val);
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
            val = this->dlc_depth.evaluate(val);
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
        
      case TT_DATALEG_NEWLINE:
        this->d_current++;
        exit_new_line = true;
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
    if ((this->di_gradient || this->di_backgradient) && (this->di_bearing || this->di_backbearing) &&
      (!(thisnan(this->cd_leg->bearing) 
      && thisnan(this->cd_leg->backbearing))) &&
      (((thisinf(this->cd_leg->gradient) != 0) || 
      (thisinf(this->cd_leg->backgradient) != 0))))
      ththrow(("compass reading given on plumbed shot"))
    if ((this->di_gradient && this->di_backgradient) && 
      (((thisinf(this->cd_leg->gradient) != 0) &&
      (thisinf(this->cd_leg->backgradient) == 0)) ||
      ((thisinf(this->cd_leg->gradient) == 0) &&
      (thisinf(this->cd_leg->backgradient) != 0))))
      ththrow(("plumbed shot with a gradient reading"))
    this->d_current = 0;
    this->complete_interleaved_data();
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
  
  this->pd_leg->is_valid = true;
}

void thdata::start_insert()
{
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
    case 10:
      break;
    default:
      ththrow(("invalid number of fix option arguments"))
  }
  it = this->fix_list.insert(this->fix_list.end(),dumm);
  it->srcf = this->db->csrc;
  it->psurvey = this->db->get_current_survey();
  it->sdx = this->dls_x;
  it->sdy = this->dls_y;
  it->sdz = this->dls_z;
  for(ai = 0; ai < nargs; ai++) {
    if (ai > 0) {
      thparse_double(vid, val, args[ai]);
      if (vid != TT_SV_NUMBER)
        ththrow(("invalid number -- %s", args[ai]))
    }
    switch (ai) {
      case 0:
        thparse_objectname(it->station, & this->db->buff_stations, args[0]);
        break;
      case 1:
        val = this->dlc_x.evaluate(val);
        val = this->dlu_x.transform(val);
        it->x = val; 
        break;
      case 2:
        val = this->dlc_y.evaluate(val);
        val = this->dlu_y.transform(val);
        it->y = val;
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
    thparse_objectname(it->station, & this->db->buff_stations, args[eid]);
    it->eqid = cid;
    it->srcf = this->db->csrc;
    it->psurvey = this->db->get_current_survey();
  } 
}
  

void thdata::set_data_station(int nargs, char ** args, int argenc)
{
  int ai, fid;
  thdatass_list::iterator it;
  thdatass dumm;
  it = this->ss_list.insert(this->ss_list.end(),dumm);
  it->srcf = this->db->csrc;
  it->psurvey = this->db->get_current_survey();
  for(ai = 0; ai < nargs; ai++) {
    switch (ai) {
      case 0:
        thparse_objectname(it->station, & this->db->buff_stations, args[0]);
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
            it->flags |= TT_STATIONFLAG_ENTRANCE;
            break;
          case TT_DATASFLAG_CONT:
            it->flags |= TT_STATIONFLAG_CONT;
            break;
          default:
            ththrow(("invalid station flag -- %s", args[ai]))
        }
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

  
void thdata::set_data_mark(int nargs, char ** args)
{
  if (nargs > 1)
    ththrow(("too many option arguments -- mark"))
  this->d_mark = thmatch_token(args[0], thtt_datamark);
  if (this->d_mark == TT_DATAMARK_UNKNOWN)
    ththrow(("unknown type of station mark -- %s", args[0]))
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


char * thdata::get_cmd_name()
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
    
  bool hasdec = csptr->get_decdef();
  while((csptr != NULL) && (!hasdec)) {
    csptr = csptr->get_father_survey();
    if (csptr != NULL)
      hasdec = csptr->get_decdef();
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

