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
#include <list>
#include <map>
#include <set>
#include <string>


#include "thconvert.h"
#include "thpdf.h"

#ifndef NOTHERION
#include "thexception.h"
#endif

using namespace std;


list<scraprecord> SCRAPLIST;
map<int,layerrecord> LAYERHASH;
set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;


string tex_Xname(string s) {return("THX"+s);}

void print_hash(){
  ofstream F("scraps.dat");
  if(!F) therror(("???"));
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

void print_hash1(){                       // only temporary solution
  ofstream F("th_hashdef");
  if(!F) therror(("???"));
  F << "%COORD = (" << endl;
  for (list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    F << " " << I->name << " => {" << endl;
    if ((I->F1) || (I->F2) || (I->F3) || (I->F4)) {
      F << "  " << "C => \"" << I->F1 << " " << I->F2 << "\"," << endl;
      F << "  " << "CU => \"" << I->F3 << " " << I->F4 << "\"," << endl;
    }
    if ((I->G1) || (I->G2) || (I->G3) || (I->G4)) {
      F << "  " << "CG => \"" << I->G1 << " " << I->G2 << "\"," << endl;
      F << "  " << "CGU => \"" << I->G3 << " " << I->G4 << "\"," << endl;
    }
    if ((I->B1) || (I->B2) || (I->B3) || (I->B4)) {
      F << "  " << "CB => \"" << I->B1 << " " << I->B2 << "\"," << endl;
      F << "  " << "CBU => \"" << I->B3 << " " << I->B4 << "\"," << endl;
    }
    if ((I->I1) || (I->I2) || (I->I3) || (I->I4)) {
      F << "  " << "CI => \"" << I->I1 << " " << I->I2 << "\"," << endl;
      F << "  " << "CIU => \"" << I->I3 << " " << I->I4 << "\"," << endl;
    }
    if ((I->E1) || (I->E2) || (I->E3) || (I->E4)) {
      F << "  " << "CE => \"" << I->E1 << " " << I->E2 << "\"," << endl;
      F << "  " << "CEU => \"" << I->E3 << " " << I->E4 << "\"," << endl;
    }
    if ((I->X1) || (I->X2) || (I->X3) || (I->X4)) {
      F << "  " << "CX => \"" << I->X1 << " " << I->X2 << "\"," << endl;
      F << "  " << "CXU => \"" << I->X3 << " " << I->X4 << "\"," << endl;
    }
    
    F << " }," << endl;
  }
  F << ");" << endl;
  F.close();
}


int thpdfdbg() {
  print_hash();
  print_hash1(); // only temporary solution
  return(0);
}
