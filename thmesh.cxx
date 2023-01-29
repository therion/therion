/**
 * @file thmesh.cxx
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
 
#include "thmesh.h"
#include "thexception.h"
#include "thchenc.h"
#include "thmbuffer.h"
#include "thtflength.h"
#include "thdatabase.h"
#include "thcsdata.h"
#include <cmath>
#include "thdatareader.h"
#include "thparse.h"
#include "thdb1d.h"
#include "thinfnan.h"
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

thmesh::thmesh()
{
  // replace this by setting real properties initialization
  this->file_name = NULL;
}


thmesh::~thmesh()
{
}


int thmesh::get_class_id() 
{
  return TT_MESH_CMD;
}


bool thmesh::is(int class_id)
{
  if (class_id == TT_MESH_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thmesh::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 0;
}


const char * thmesh::get_cmd_end()
{
  // insert endcommand if multiline command
  return "endmesh";
}


const char * thmesh::get_cmd_name()
{
  // insert command name here
  return "mesh";
}


thcmd_option_desc thmesh::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_mesh_opt);
  if (id == TT_MESH_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else switch(id) {
//    case TT_MESH_FILE
//      return thcmd_option_desc(id,2);
//      break;
    default:
      return thcmd_option_desc(id);
  }
}


void thmesh::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    // replace this by real properties setting
    case 0:
      this->parse_station(args);
      break;
      
    case TT_MESH_FILE:
      this->parse_file(args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thmesh::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thmesh:\n");
  if (this->file_name != NULL) {
    fprintf(outf,"\tfile_name:\t%s\n", this->file_name);
  }
}


void thmesh::parse_file(char ** args)
{
  if (strlen(args[0]) == 0)
    ththrow("fileure name not specified");

  std::error_code ec;
  auto file_path = fs::current_path(ec);
  thassert(!ec)

  if (fs::path(thdb.csrc.name).is_absolute())
	  file_path = thdb.csrc.name;
  else
	  file_path /= thdb.csrc.name;

  auto file_path_str = (file_path.parent_path() / args[0]).string();
  std::replace(file_path_str.begin(), file_path_str.end(), '\\', '/');

  this->file_name = thdb.strstore(file_path_str.c_str());
  // thprintf("\nfile name: %s\n", this->file_name);
  
}


void thmesh::calibrate() {
  // spocita origin, scale (pri 300 dpi) a rotaciu
  // TODO
}


void thmesh::check_stations() 
{
  for(auto it = this->cal_stations.begin(); it != this->cal_stations.end(); it++) {
	it->name.id = thdb.db1d.get_station_id(it->name, it->ssurvey);
	if (it->name.id == 0) {
	  if (it->name.survey == NULL)
		ththrow("{} -- station doesn't exist -- {}", it->src.to_string(), it->name.name);
	  else
		ththrow("{} -- station doesn't exist -- {}@{}", it->src.to_string(), it->name.name, it->name.survey);
	}
  }
  for(auto it = this->cal_stations.begin(); it != this->cal_stations.end(); it++) {
	  for(auto it2 = std::next(it,1); it2 != this->cal_stations.end(); it2++) {
		thdb1ds * ds1, * ds2;
		ds1 = &(thdb.db1d.station_vec[it->name.id - 1]);
		ds2 = &(thdb.db1d.station_vec[it2->name.id - 1]);
		if ((ds1->x == ds2->x) && (ds1->y == ds2->y) && (ds1->z == ds2->z)) {
			ththrow("{} -- {} -- duplicate points in picture calibration", it->src.throw_source(), it2->src.throw_source());
		}
	  }
  }
}


void thmesh::parse_station(char ** args)
{

  // rozdelime na argumenty
  thsplit_args(&thdb.mbuff_tmp, *args);
  
  // kazdy parsneme ako cislo a zapiseme do pola  
  long ni = thdb.mbuff_tmp.get_size();
  int sv;
  char ** values = thdb.mbuff_tmp.get_buffer();

  if (ni != 4) ththrow("invalid format -- <station> <x> <y> <z> expected");

  thmesh_calibst cst;

  thparse_objectname(cst.name, &(thdb.buff_stations), values[0]);
  cst.src = thdb.csrc;
  cst.ssurvey = thdb.get_current_survey();

  thparse_double(sv, cst.coords.x, values[1]);
  if (sv != TT_SV_NUMBER)
    ththrow("number expected -- {}", values[1]);
  thparse_double(sv, cst.coords.y, values[2]);
  if (sv != TT_SV_NUMBER)
    ththrow("number expected -- {}", values[2]);
  thparse_double(sv, cst.coords.z, values[3]);
  if (sv != TT_SV_NUMBER)
    ththrow("number expected -- {}", values[3]);

  this->cal_stations.push_back(cst);

}


void thmesh::start_insert() {
  if (this->cal_stations.empty())
    ththrow("missing calibration data");
  if (this->file_name == NULL)
	ththrow("missing mesh file reference");
}

