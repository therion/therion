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

const int TTCS_EPSG = 1000000;
const int TTCS_ESRI = 2000000;

int thcs_parse(const char * name);

const char * thcs_get_name(int cs);

const thcsdata * thcs_get_data(int cs);

void thcs_add_cs(char * id, char * proj4id, size_t nargs, char ** args);

#endif
