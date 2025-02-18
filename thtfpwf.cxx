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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thtfpwf.h"
#include "thexception.h"
#include "thinfnan.h"
#include "thparse.h"

#include <fmt/core.h>

#include <algorithm>


void thtfpwf::set(size_t nv, double * pv)
{
  size_t idx;
  this->values.clear();
  if (nv > 0) {
    this->values.resize(nv);
    for(idx = 0; idx < nv; idx++) {
      this->values[idx].x = *pv;
      pv++;
      this->values[idx].y = *pv;
      pv++;
    }
    // now let's sort them
    std::sort(this->values.begin(), this->values.end(), [](const auto& a, const auto& b){ return a.x < b.x; });
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
        throw thexception(fmt::format("invalid number -- {}", *sfact));
      this->a = 1.0;
      break;
    case 2:
      thparse_double(sv, this->b, sfact[0]);
      if (sv != TT_SV_NUMBER)
        throw thexception(fmt::format("invalid number -- {}", sfact[0]));
      thparse_double(sv, this->a, sfact[1]);
      if (sv != TT_SV_NUMBER)
        throw thexception(fmt::format("invalid number -- {}", sfact[1]));
      break;
    default:
      if ((nfact > 2) && ((nfact % 2) == 0)) {
        std::vector<double> coefs(nfact);
        int cidx,spv;
        for(cidx = 0; cidx < nfact; cidx++) {
          thparse_double(spv,coefs[cidx],sfact[cidx]);
          if (spv == TT_SV_UNKNOWN)
            throw thexception(fmt::format("invalid number -- {}", sfact[cidx]));
        }
        this->set(nfact / 2, coefs.data());
      }
      else
        throw thexception("invalid number of transformation constants");
  }
  if (this->a == 0.0)
    throw thexception(fmt::format("invalid scale factor -- {}", this->a));
}
  
double thtfpwf::evaluate(double value)
{
  switch (this->values.size()) {
    case 0:
      return (value - b) * a;
    case 1:
      return this->values.front().y;
    default:
      // let's do a binary search
      int a = 0, b = (int) this->values.size() - 1, c, r, ret = -1;
      while (a <= b) {
        c = unsigned((a + b) / 2);
        if (this->values[c].x < value)
          r = -1;
        else {
          if (c == 0)
            r = 0;
          else {
            if (this->values[c - 1].x <= value)
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
          return this->values.back().y;
        case 0:
          return this->values.front().y;
        default:
          double x1 = this->values[ret - 1].x;
          double y1 = this->values[ret - 1].y;
          double x2 = this->values[ret].x;
          double y2 = this->values[ret].y;
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
  return this->values.data();
}
  
  
size_t thtfpwf::get_size()
{
  return this->values.size();
}
