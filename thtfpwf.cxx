/**
 * @file thtfpwf.cxx
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
 
#include "thtfpwf.h"
#include "thexception.h"
#include "thinfnan.h"
#include "thparse.h"

void thtfpwf::clear()
{
  if (this->valn > 0) {
    delete [] this->valp;
    this->valn = 0;
    this->valp = NULL;
  }
}


thtfpwf::~thtfpwf()
{
  this->clear();
}


int thtfpwfxy_comp(const void * x1, const void * x2)
{
  thtfpwfxy * p1 = (thtfpwfxy *) x1;
  thtfpwfxy * p2 = (thtfpwfxy *) x2;
  if (p1->x < p2->x)
    return -1;
  if (p1->x == p2->x)
    return 0;
  else
    return 1;
}


void thtfpwf::set(size_t nv, double * pv)
{
  size_t idx;
  this->clear();
  if (nv > 0) {
    this->valp = new thtfpwfxy [nv];
    this->valn = nv;
    for(idx = 0; idx < nv; idx++) {
      this->valp[idx].x = *pv;
      pv++;
      this->valp[idx].y = *pv;
      pv++;
    }
    // now let's sort them
    qsort(this->valp,this->valn,sizeof(thtfpwfxy),thtfpwfxy_comp);
//    for(idx = 0; idx < nv; idx++) {
//      printf("PWF SETTING: %f -> %f\n", this->valp[idx].x, this->valp[idx].y);
//    }
  }
}



void thtfpwf::parse(int nfact, char ** sfact)
{
  int sv;
  switch (nfact) {
    case 1:
      thparse_double(sv, this->b, *sfact);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s", *sfact))
      break;
    case 2:
      thparse_double(sv, this->b, sfact[0]);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s", sfact[0]))
      thparse_double(sv, this->a, sfact[1]);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s", sfact[1]))
      break;
    default:
      if ((nfact > 2) && ((nfact % 2) == 0)) {
        double * coefs = new double [nfact];
        int cidx,spv;
        for(cidx = 0; cidx < nfact; cidx++) {
          thparse_double(spv,coefs[cidx],sfact[cidx]);
          if (spv == TT_SV_UNKNOWN)
            ththrow(("invalid number -- %s", sfact[cidx]))
        }
        this->set(nfact / 2, coefs);
        delete [] coefs;
      }
      else
        ththrow(("invalid number of transformation constants"))
  }
  if (this->a == 0.0)
    ththrow(("invalid scale factor -- %f", this->a))
}
  
double thtfpwf::evaluate(double value)
{
  switch (valn) {
    case 0:
      return (value - b) * a;
    case 1:
      return this->valp->y;
    default:
      // let's do a binary search
      int a = 0, b = (int) this->valn - 1, c, r, ret = -1;
      while (a <= b) {
        c = unsigned((a + b) / 2);
        if (this->valp[c].x < value)
          r = -1;
        else {
          if (c == 0)
            r = 0;
          else {
            if (this->valp[c - 1].x <= value)
              r = 0;
            else
              r = 1;
          }
        }
        if (r == 0) {
          ret = c;
          break;
        }
        if (r < 0)
          a = c + 1;
        else
          b = c - 1;
      }
      switch (ret) {
        case -1:
          return this->valp[this->valn - 1].y;
        case 0:
          return this->valp->y;
        default:
          double x1 = this->valp[ret - 1].x;
          double y1 = this->valp[ret - 1].y;
          double x2 = this->valp[ret].x;
          double y2 = this->valp[ret].y;
          if (thisnan(y1) || thisnan(y2))
            return thnan;
          if ((thisinf(y1) != 0) || (thisinf(y2) != 0)) {
            if ((thisinf(y1) + thisinf(y2)) == 0)
              return thnan;
            if ((thisinf(y1) + thisinf(y2)) > 0)
              return thinf;
            if ((thisinf(y1) + thisinf(y2)) < 0)
              return -thinf;
          }
          if (x1 == x2)
            return y1;
          return (y1 + (value - x1) * (y2 - y1) / (x2 - x1));
      }
  }
}

thtfpwfxy * thtfpwf::get_values()
{
  return this->valp;
}
  
  
size_t thtfpwf::get_size()
{
  return this->valn;
}
