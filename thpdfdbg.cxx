/**
 * @file thpdfdbg.cxx
 */
  
/* Copyright (C) 2003 Martin Budaj
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
 
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <iomanip>
#include <fmt/core.h>

#include <iostream>

#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thexception.h"

using namespace std;


string tex_Xname(string s) {return("THX"+s);}
string tex_Wname(string s) {return("THW"+s);}   // special = northarrow &c.

string u2str(unsigned u) {
  unsigned i=u;
  char c;
  string s="";
  while (i>0) {
    c = 'a' + ((i-1) % 26);
    s = c + s;
    i = (i-1) / 26;
  };
  return (s);
}

string rgb2svg(double r, double g, double b) {
  return fmt::format("#{:02x}{:02x}{:02x}",int(255*r) % 256,
                                           int(255*g) % 256,
                                           int(255*b) % 256);
}

void print_hash(){
  ofstream F("scraps.dat");
  if(!F) therror(("Can't open file `scraps.dat'"));
  F << "[SCRAP]" << endl;
  for (list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    F << "N " << I->name << endl;
    if (I->F != "") {
      F << "F " << I->F1 << " " << I->F2 << " " << I->F3 << 
                           " " << I->F4 << endl;
    }
    if (I->G != "") {
      F << "G " << I->G1 << " " << I->G2 << " " << I->G3 << 
                           " " << I->G4 << endl;
    }
    if (I->B != "") {
      F << "B " << I->B1 << " " << I->B2 << " " << I->B3 << 
                           " " << I->B4 << endl;
    }
    if (I->I != "") {
      F << "I " << I->I1 << " " << I->I2 << " " << I->I3 << 
                           " " << I->I4 << endl;
    }
    if (I->E != "") {
      F << "E " << I->E1 << " " << I->E2 << " " << I->E3 << 
                           " " << I->E4 << endl;
    }
    if (I->X != "") {
      F << "X " << I->X1 << " " << I->X2 << " " << I->X3 << 
                            " " << I->X4 << endl;
    }
    if (I->P != "") {
      F << "P " << I->P << " " << I->S1 << " " << I->S2 << endl;
    }
    F << "Y " << I->layer << endl;
    F << "V " << I->level << endl;
    if (I->sect != 0) {
      F << "Z 1" << endl;
    }
  }
  F << "[LAYER]" << endl;
  for (map<int,layerrecord>::iterator I = LAYERHASH.begin(); 
                                  I != LAYERHASH.end(); I++) {
    F << "R " << (*I).first << endl;
    if (!(((*I).second).U).empty()) {
      F << "U";
      for (set<int>::iterator J = (((*I).second).U).begin(); J != (((*I).second).U).end(); J++) {
        F << " " << *J;
      }
      F << endl;
    }
    if (!(((*I).second).D).empty()) {
      F << "D";
      for (set<int>::iterator J = (((*I).second).D).begin(); J != (((*I).second).D).end(); J++) {
        F << " " << *J;
      }
      F << endl;
    }
    if (((*I).second).N != "") {
      F << "N " << ((*I).second).N << endl;
    }
    if (((*I).second).T != "") {
      F << "T " << ((*I).second).T << endl;
    }
    if (I->second.Z != 0) {
      F << "Z 1" << endl;
    }
  }
  F << "[MAP]" << endl;
  if (!MAP_PREVIEW_UP.empty()) {
    F << "U";
    for (set<int>::iterator J = MAP_PREVIEW_UP.begin(); 
                            J != MAP_PREVIEW_UP.end(); J++) {
      F << " " << *J;
    }
    F << endl;
  }
  if (!MAP_PREVIEW_DOWN.empty()) {
    F << "D";
    for (set<int>::iterator J = MAP_PREVIEW_DOWN.begin(); 
                            J != MAP_PREVIEW_DOWN.end(); J++) {
      F << " " << *J;
    }
    F << endl;
  }


  F.close();
}



void thpdfdbg() {
  print_hash();
}
