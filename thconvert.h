/*
 * Copyright (C) 2003 Martin Budaj
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
 

#ifndef thconvert_h
#define thconvert_h

#ifndef NOTHERION
#include "thexception.h"
#endif

#include <string>

using namespace std;

typedef struct {
  string name,F,B,I,E,X,G,C,P;      // name + files
  double S1,S2;                      // shift
  int layer,level,sect;             // Y, V, Z
  float F1,F2,F3,F4,                // bounding boxes
        G1,G2,G3,G4,
        B1,B2,B3,B4,
        I1,I2,I3,I4,
        E1,E2,E3,E4,
        X1,X2,X3,X4;
} scraprecord;

extern list<scraprecord> SCRAPLIST;

int thconvert();

#ifdef NOTHERION
#define therror(P) {\
  cerr << P << endl;\
  exit(0);\
}
#endif

#endif


