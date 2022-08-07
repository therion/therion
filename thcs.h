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
 

#ifndef thcs_h
#define thcs_h

#include "thcsdata.h"
#include <map>

const int TTCS_EPSG = 1000000;
const int TTCS_ESRI = 2000000;

int thcs_parse(const char * name);

const char * thcs_get_name(int cs);

const thcsdata * thcs_get_data(int cs);

std::string thcs_get_params(int cs);

class thcstrans {
  public:
	int from_id, to_id;
	thcstrans() : from_id(TTCS_UNKNOWN), to_id(TTCS_UNKNOWN) {}
	thcstrans(int from, int to) : from_id(from), to_id(to) {}
    friend bool operator < (const thcstrans & t1, const thcstrans &t2);
};

void thcs_add_cs(char * id, char * proj4id);

void thcs_add_cs_trans(const char * from_css, const char * to_css, const char * trans);

std::string thcs_get_trans(int from_cs, int to_cs);

#endif
