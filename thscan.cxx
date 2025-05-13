/**
 * @file thscan.cxx
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
 
#include "thscan.h"

#include "thcsdata.h"
#include "thdatabase.h"
#include "thdatareader.h"
#include "thdb1d.h"
#include "therion.h"
#include "thexception.h"
#include "thinfnan.h"
#include "thmbuffer.h"
#include "thproj.h"
#include "thtflength.h"
#include "thtrans.h"

#include "stl_reader.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <list>

namespace fs = std::filesystem;

enum {
  TT_SCAN_COORDS_UNKNOWN,
  TT_SCAN_COORDS_X,
  TT_SCAN_COORDS_NX,
  TT_SCAN_COORDS_Y,
  TT_SCAN_COORDS_NY,
  TT_SCAN_COORDS_Z,
  TT_SCAN_COORDS_NZ,
};

static const thstok thtt_scan_coords[] = {
  {"-x", TT_SCAN_COORDS_NX},
  {"-y", TT_SCAN_COORDS_NY},
  {"-z", TT_SCAN_COORDS_NZ},
  {"x", TT_SCAN_COORDS_X},
  {"y", TT_SCAN_COORDS_Y},
  {"z", TT_SCAN_COORDS_Z},
  {nullptr, TT_SCAN_COORDS_UNKNOWN},
};





thscan::thscan()
  : datasrc_coords{TT_SCAN_COORDS_X, TT_SCAN_COORDS_Y, TT_SCAN_COORDS_Z}
{
  // replace this by setting real properties initialization
  this->d3dok = false;
  this->datasrc_cs = TTCS_LOCAL;
}

int thscan::get_class_id()
{
  return TT_SCAN_CMD;
}


bool thscan::is(int class_id)
{
  if (class_id == TT_SURFACE_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thscan::get_cmd_nargs()
{
  // replace by real number of arguments
  return 0;
}


const char * thscan::get_cmd_end()
{
  // insert endcommand if multiline command
  return "endscan";
}


const char * thscan::get_cmd_name()
{
  // insert command name here
  return "scan";
}


thcmd_option_desc thscan::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_scan_opt);
  if (id == TT_SCAN_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else switch(id) {
	case TT_SCAN_AXES:
	  return thcmd_option_desc(id, 3);
	default:
	  return thcmd_option_desc(id);
  }
}


void thscan::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    // replace this by real properties setting
    case 0:
	  thsplit_args(& this->db->mbuff_tmp, *args);
	  this->parse_calib(this->db->mbuff_tmp.get_size(), this->db->mbuff_tmp.get_buffer());
      break;
      
    case TT_SCAN_UNITS:
      this->units.parse_units(args[0]);
      break;

    case TT_SCAN_FILE:
      this->parse_data_source(args);
      this->datasrc_cs = this->cs;
      break;
      
    case TT_SCAN_AXES:
      this->parse_data_source_coords(args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}

void thscan::parse_data_source_coords(char ** args) {
  bool hasx, hasy, hasz;
  hasx = false;
  hasy = false;
  hasz = false;
  for(size_t i = 0; i < this->datasrc_coords.size(); i++) {
	this->datasrc_coords[i] = thmatch_token(args[i], thtt_scan_coords);
	if (this->datasrc_coords[i] == TT_SCAN_COORDS_UNKNOWN)
	  throw thexception(fmt::format("unknown scan coordinate -- {}", args[i]));
	switch(this->datasrc_coords[i]) {
	  case TT_SCAN_COORDS_NX:
	  case TT_SCAN_COORDS_X:
		hasx = true;
		break;
	  case TT_SCAN_COORDS_NY:
	  case TT_SCAN_COORDS_Y:
		hasy = true;
		break;
	  case TT_SCAN_COORDS_NZ:
	  case TT_SCAN_COORDS_Z:
		hasz = true;
		break;
	}
  }
  if (!hasx)
	throw thexception(fmt::format("x coordinate not defined -- {} {} {}", args[0], args[1], args[2]));
  if (!hasy)
	throw thexception(fmt::format("y coordinate not defined -- {} {} {}", args[0], args[1], args[2]));
  if (!hasz)
	throw thexception(fmt::format("z coordinate not defined -- {} {} {}", args[0], args[1], args[2]));
}


void thscan::transform_coords(double & x, double & y, double & z) {
  double tx, ty, tz;
  tx = x;
  ty = y;
  tz = z;
  std::array<double, 3> v{};
  for(size_t i = 0; i < this->datasrc_coords.size(); i++) switch(this->datasrc_coords[i]) {
	case TT_SCAN_COORDS_X:
	  v[i] = tx;
	  break;
	case TT_SCAN_COORDS_NX:
	  v[i] = -tx;
	  break;
	case TT_SCAN_COORDS_Y:
	  v[i] = ty;
	  break;
	case TT_SCAN_COORDS_NY:
	  v[i] = -ty;
	  break;
	case TT_SCAN_COORDS_Z:
	  v[i] = tz;
	  break;
	case TT_SCAN_COORDS_NZ:
	  v[i] = -tz;
	  break;
  }
  x = v[0];
  y = v[1];
  z = v[2];
}

struct morph3dpt {
  long m_id = 0;
  lxVec m_f;
  lxVec m_t;
  double m_shift_z = 0.0;
  thlintrans m_lt;
  morph3dpt() = default;
};

struct morph3d {
  std::list<morph3dpt> m_points;
  void insert_point(long id, lxVec f, lxVec t);
  void initialize();
  lxVec forward(lxVec v);
};

void morph3d::insert_point(long id, lxVec f, lxVec t) {
  morph3dpt p;
  p.m_id = id;
  p.m_f = f;
  p.m_t = t;
  m_points.push_back(p);
}

void morph3d::initialize() {
  std::list<morph3dpt>::iterator ci, pi, ni;
  double nd, f_cd2d, t_cd2d, t_cd3d;
  nd = 0.0;
  for(pi = this->m_points.begin(); pi != this->m_points.end(); pi++) {
	ni = pi;
	for(ci = this->m_points.begin(); ci != this->m_points.end(); ci++) {
	  if (ci != pi) {
        f_cd2d = std::hypot(ci->m_f.x - pi->m_f.x, ci->m_f.y - pi->m_f.y);
        t_cd2d = std::hypot(ci->m_t.x - pi->m_t.x, ci->m_t.y - pi->m_t.y);
        t_cd3d = std::hypot(t_cd2d, ci->m_t.z - pi->m_t.z);
        if ((f_cd2d > 0.0) && (t_cd2d > 0.0)) {
		  // find the nearest station, not vertically collinear
          if ((ni == pi) || (t_cd3d < nd)) {
			ni = ci;
			nd = t_cd3d;
		  }
        }
	  }
	}
	// calculate shift
	pi->m_shift_z = pi->m_t.z - pi->m_f.z;
	// calculate rotation & scale matrix
	if (ni != pi) {
	  pi->m_lt.init(thvec2(pi->m_f.x, pi->m_f.y), thvec2(ni->m_f.x, ni->m_f.y), thvec2(pi->m_t.x, pi->m_t.y), thvec2(ni->m_t.x, ni->m_t.y));
	} else {
	  pi->m_lt.init(thvec2(pi->m_f.x, pi->m_f.y), thvec2(pi->m_t.x, pi->m_t.y));
	}
  }
}


lxVec morph3d::forward(lxVec v) {
  lxVec tt = lxVec(0.0, 0.0, 0.0), c;
  thvec2 f, t;
  double tw, cw;
  tw = 0.0;
  for(const auto& point : this->m_points) {
	cw = std::hypot(v.x - point.m_f.x, v.y - point.m_f.y, v.z - point.m_f.z);
	cw = cw * cw;
	f.m_x = v.x;
	f.m_y = v.y;
	t = point.m_lt.forward(f);
	c.x = t.m_x;
	c.y = t.m_y;
	c.z = v.z + point.m_shift_z;
	if (cw > 0) {
	  cw = 1/cw;
	  tt += cw * c;
	  tw += cw;
	} else {
	  return c;
	}
  }
  return (1/tw*tt);
}



void thscan::parse_data_source(char ** args) {

  if (strlen(args[0]) == 0)
    throw thexception("source filename not specified");

  std::error_code ec;
  auto pict_path = fs::current_path(ec);
  thassert(!ec)

  if (fs::path(thdb.csrc.name).is_absolute())
	  pict_path = thdb.csrc.name;
  else
	  pict_path /= thdb.csrc.name;

  auto pict_path_str = (pict_path.parent_path() / args[0]).string();
  std::replace(pict_path_str.begin(), pict_path_str.end(), '\\', '/');

  this->datasrc = pict_path_str;
  //thprintf("\npict name: %s\n", pict_path_str.c_str());

}

void thscan::parse_calib(int nargs, char** args) {
  if (nargs != 4)
    throw thexception("wrong # of parameters -- expected <station> <x> <y> <z>");

  int sv;
  std::array<double, 3> coords{};
  thdatafix_list::iterator it;
  it = this->calib.insert(this->calib.end(),thdatafix());
  it->srcf = this->db->csrc;
  it->psurvey = this->db->get_current_survey();
  it->cs = this->cs;
  thparse_objectname(it->station, & this->db->buff_stations, args[0], this);
  for(size_t i = 0; i < coords.size(); i++) {
	thparse_double(sv, coords[i], args[i+1]);
	if (sv != TT_SV_NUMBER)
	  throw thexception(fmt::format("number expected -- {}", args[i+1]));
  }
  it->x = coords[0];
  it->y = coords[1];
  it->z = coords[2];
}


void thscan::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thscan:\n");
  if (not this->datasrc.empty()) {
    fprintf(outf,"\tsource:\t%s\n", this->datasrc.c_str());
  }
}

void thscan::check_stations()
{
  for(auto& fix : this->calib) {
	  if (!fix.station.is_empty()) {

	  // najde stations, error ak nie
	  fix.station.id = thdb.db1d.get_station_id(fix.station, this->fsptr);
	  if (fix.station.id == 0) {
		if (fix.station.survey == nullptr)
		  throw thexception(fmt::format("{} -- station doesn't exist -- {}", this->throw_source(), fix.station.name));
		else
		  throw thexception(fmt::format("{} -- station doesn't exist -- {}@{}", this->throw_source(), fix.station.name, fix.station.survey));
	  }
	}
  }
}


thdb3ddata * thscan::get_3d() {

  if (this->d3dok)
    return &(this->d3d);

  try {
	if (this->datasrc.empty()) throw thexception("scan file not specified");
    stl_reader::StlMesh <float, unsigned int> mesh (this->datasrc.c_str());
    lxVec v;
    thdb3dfc * fc;
    fc = this->d3d.insert_face(THDB3DFC_TRIANGLES);
    for(size_t itri = 0; itri < mesh.num_tris(); ++itri) {
        //std::cout << "coordinates of triangle " << itri << ": ";
        for(size_t icorner = 0; icorner < 3; ++icorner) {
            const float* c = mesh.tri_corner_coords (itri, icorner);
            // or alternatively:
            //const float* c = mesh.vrt_coords (mesh.tri_corner_ind (itri, icorner));
            //std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
            v.x = this->units.transform(c[0]);
            v.y = this->units.transform(c[1]);
            v.z = this->units.transform(c[2]);
            this->transform_coords(v.x, v.y, v.z);
            fc->insert_vertex(this->d3d.insert_vertex(v, nullptr, true));
        }
        //std::cout << std::endl;

        //const float* n = mesh.tri_normal (itri);
        //std::cout   << "normal of triangle " << itri << ": "
        //            << "(" << n[0] << ", " << n[1] << ", " << n[2] << ")\n";
    }
  }
  catch (const std::exception& e) {
	throw thexception(fmt::format("{} -- error reading {}", this->throw_source(), this->datasrc), e);
  }

  // TODO: inicializujeme transformaciu
  morph3d m3d;
  for(const auto& fix : this->calib) {
	lxVec f, t;
	f.x = this->units.transform(fix.x);
	f.y = this->units.transform(fix.y);
	f.z = this->units.transform(fix.z);
	this->transform_coords(f.x, f.y, f.z);
	thdb1ds * ts;
	ts = &(thdb.db1d.station_vec[fix.station.id - 1]);
	t.x = ts->x;
	t.y = ts->y;
	t.z = ts->z;
	m3d.insert_point(fix.station.id, f, t);
  }
  m3d.initialize();

  // TODO: transformujeme vsetky vrcholy
  thdb3dvx * vx;
  lxVec t;
  for(vx = this->d3d.firstvx; vx != nullptr; vx = vx->next) {
	t = m3d.forward(lxVec(vx->x, vx->y, vx->z));
	vx->x = t.x;
	vx->y = t.y;
	vx->z = t.z;
  }

  this->d3dok = true;
  this->d3d.postprocess();
  return &(this->d3d);    

}


void thscan::convert_all_cs() {
  // TODO: skonvertovat calibration
}

