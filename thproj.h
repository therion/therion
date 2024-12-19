/* * Copyright (C) 2006 Martin Budaj
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
 

#ifndef thproj_h
#define thproj_h

#include <string>
#include <vector>
#include <proj.h>

enum {GRID_INVALID, GRID_IGNORE, GRID_WARN, GRID_FAIL, GRID_CACHE, GRID_DOWNLOAD};
enum class axis_orient {NORTH, SOUTH, WEST, EAST, OTHER};

void thcs2cs(int, int, double, double, double, double &, double &, double &);
signed int thcs2zone(int, double, double, double);
double thcsconverg(int, double, double);
bool thcs_islatlong(std::string);
std::vector<axis_orient> thcs_axesinfo(int, double &, bool &);
bool thcs_check(std::string);
void thcs_log_transf_used();
int thcs_parse_gridhandling(const char *);
std::string thcs_get_proj_version();
std::string thcs_get_proj_version_headers();
std::string thcs_get_label(int);
std::string thcs_get_wkt(int, bool=false, PJ_WKT_TYPE=PJ_WKT2_2019);

struct thcs_config {
  thcs_config();
  std::vector<double> bbox;
  int proj_auto_grid;
};

extern thcs_config thcs_cfg;

#endif
