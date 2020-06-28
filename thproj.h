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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 

#ifndef thproj_h
#define thproj_h

#include <string>
#include <vector>

enum {GRID_INVALID, GRID_IGNORE, GRID_WARN, GRID_FAIL, GRID_CACHE, GRID_DOWNLOAD};

void thcs2cs(std::string, std::string, double, double, double, double &, double &, double &);
signed int thcs2zone(std::string, double, double, double);
double thcsconverg(std::string, double, double);
bool thcs_islatlong(std::string);
bool thcs_check(std::string);
void thcs_log_transf_used();
int thcs_parse_gridhandling(const char *);
std::string thcs_get_proj_version();

struct thcs_config {
  thcs_config();
  std::vector<double> bbox;
  bool proj_auto;
  int proj_auto_grid;
};

extern thcs_config thcs_cfg;

#endif
