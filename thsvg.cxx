/**
 * @file thconvert.cxx
 */
  
/* Copyright (C) 2005 Martin Budaj
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
 
// #include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>

#include <cstring>
#include <cstdio>
#include <cfloat>

#include "thepsparse.h"
//#include "thpdfdbg.h"
#include "thpdfdata.h"
//#include "thtexfonts.h"
#include "therion.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

void find_dimensions(double & MINX,double & MINY,double & MAXX,double & MAXY) {
  double llx = 0, lly = 0, urx = 0, ury = 0;
  MINX=DBL_MAX, MINY=DBL_MAX, MAXX=-DBL_MAX, MAXY=-DBL_MAX;
  for (list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    llx = DBL_MAX; lly = DBL_MAX; urx = -DBL_MAX; ury = -DBL_MAX;
    if (I->F != "" && I->E == "" && I->G == "" && 
        I->B == "" && I->X == "") { // clipped symbols shouldn't affect map size
      if (I->F1 < llx) llx = I->F1;
      if (I->F2 < lly) lly = I->F2;
      if (I->F3 > urx) urx = I->F3;
      if (I->F4 > ury) ury = I->F4;
    }
    if (I->E != "") {
      if (I->E1 < llx) llx = I->E1;
      if (I->E2 < lly) lly = I->E2;
      if (I->E3 > urx) urx = I->E3;
      if (I->E4 > ury) ury = I->E4;
    }
    if (I->G != "") {
      if (I->G1 < llx) llx = I->G1;
      if (I->G2 < lly) lly = I->G2;
      if (I->G3 > urx) urx = I->G3;
      if (I->G4 > ury) ury = I->G4;
    }
    if (I->B != "") {
      if (I->B1 < llx) llx = I->B1;
      if (I->B2 < lly) lly = I->B2;
      if (I->B3 > urx) urx = I->B3;
      if (I->B4 > ury) ury = I->B4;
    }
    if (I->I != "") {
      if (I->I1 < llx) llx = I->I1;
      if (I->I2 < lly) lly = I->I2;
      if (I->I3 > urx) urx = I->I3;
      if (I->I4 > ury) ury = I->I4;
    }
    if (I->X != "") {
      if (I->X1 < llx) llx = I->X1;
      if (I->X2 < lly) lly = I->X2;
      if (I->X3 > urx) urx = I->X3;
      if (I->X4 > ury) ury = I->X4;
    }

    if (llx == DBL_MAX || lly == DBL_MAX || urx == -DBL_MAX || ury == -DBL_MAX) 
      therror(("This can't happen -- no data for a scrap!"));
    
    map<int,layerrecord>::iterator J = LAYERHASH.find(I->layer);
    if (J == LAYERHASH.end()) therror (("This can't happen!"));

/*    map<int,set<string> >::iterator K = (((*J).second).scraps).find(I->level);
    if (K == (((*J).second).scraps).end()) {
      set<string> SCRP;
      (((*J).second).scraps).insert(make_pair(I->level,SCRP));
      K = (((*J).second).scraps).find(I->level);
    }
    ((*K).second).insert(I->name);
    
    (((*J).second).allscraps).insert(I->name); */
      
    if (((*J).second).Z == 0) {
      if (MINX > llx) MINX = llx;
      if (MINY > lly) MINY = lly;
      if (MAXX < urx) MAXX = urx;
      if (MAXY < ury) MAXY = ury;
    }
  }
}




void thsvg(char * fname) {
  thprintf("making svg map ... ");

  ofstream F(fname);
  //F.setf(ios::fixed, ios::floatfield);
  //F.precision(2);
  
  double llx=0, lly=0, urx=0, ury=0;
  find_dimensions(llx, lly, urx, ury);
  
  F << "<?xml version=\"1.0\"?>" << endl;
  F << "<svg width=\"" << 2.54/72*(urx-llx) << 
        "cm\" height=\"" << 2.54/72*(ury-lly) << 
        "cm\" viewBox=\"" << llx << " " << -ury << 
        " " << urx-llx << " " << ury-lly << 
        "\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << endl;

  F << "<defs>" << endl;
  for(list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    I->Fc.MP.print_svg(F, "F_" + I->name);
    I->Gc.MP.print_svg(F, "G_" + I->name);
    I->Bc.MP.print_svg(F, "B_" + I->name);
    I->Ic.MP.print_svg(F, "I_" + I->name);
    I->Ec.MP.print_svg(F, "E_" + I->name);
    I->Xc.MP.print_svg(F, "X_" + I->name);
  }
  F << "</defs>" << endl;

  for(list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    F << "<use x=\"" << I->F1 << "\" y=\"" << -I->F2 << "\" xlink:href=\"#F_" << I->name << "\" />" << endl;
//    F << "<use x=\"" << I->G1 << "\" y=\"" << -I->G2 << "\" xlink:href=\"#G_" << I->name << "\" />" << endl;
    F << "<use x=\"" << I->B1 << "\" y=\"" << -I->B2 << "\" xlink:href=\"#B_" << I->name << "\" />" << endl;
//    F << "<use x=\"" << I->I1 << "\" y=\"" << -I->I2 << "\" xlink:href=\"#I_" << I->name << "\" />" << endl;
    F << "<use x=\"" << I->E1 << "\" y=\"" << -I->E2 << "\" xlink:href=\"#E_" << I->name << "\" />" << endl;
    F << "<use x=\"" << I->X1 << "\" y=\"" << -I->X2 << "\" xlink:href=\"#X_" << I->name << "\" />" << endl;
  }



  F << "</svg>" << endl;
  
  F.close();
  thprintf("done\n");
}


