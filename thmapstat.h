/**
 * @file thmapstat.h
 * Multiple string buffer class.
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
 
#ifndef thmapstat_h
#define thmapstat_h


#include <set>
#include <map>
#include <stdio.h>
#include "thdate.h"
#include "thperson.h"
#include "thlegenddata.h"


class thmapstat_data {
  
  public:

  friend bool operator < (const thmapstat_data & c1, 
      const thmapstat_data & c2);
  
  thdate date;
  double crit;
  
  thmapstat_data();
  
};


class thmapstat_copyright {
  
  public:

  friend bool operator < (const thmapstat_copyright & c1, 
      const thmapstat_copyright & c2);
  
  const char * str;  
  
};


class thmapstat_dataptr {
  
  public:

  friend bool operator < (const thmapstat_dataptr & c1, 
      const thmapstat_dataptr & c2);
  
  class thdata * ptr;  

  thmapstat_dataptr();
  
};



// person map type /person, datum, kriterium/
// copyright map type /copyright, datum, kriterium/
// data map type /dataid/
typedef std::map <thperson, thmapstat_data> thmapstat_personmap;
typedef std::map <thmapstat_copyright, thmapstat_data> thmapstat_copyrightmap;
typedef std::map <thmapstat_dataptr, unsigned long> thmapstat_datamap;


class thmapstat {

  public:
  
  bool scanned;
  
  thmapstat_personmap discovered_by, surveyed_by, drawn_by;
  
  thdate drawn_date, surveyed_date, discovered_date;
  
  thmapstat_copyrightmap copyright;
  
  thmapstat_datamap data;
  
  /**
   * Standard constructor.
   */
  
  thmapstat();
  
  
  /**
   * Scan authors and copyrights and data.
   */
   
  void scanmap(class thmap * map);

  /**
   * Add data from another objects.
   */
   
  void addobj(class thdataobject * obj);
  void addstat(thmapstat * source);
  void adddata(thmapstat_datamap * dm);
  

  void export_pdftex(FILE * f, class thlayout * layout, legenddata * ldata);
  
};

#endif


