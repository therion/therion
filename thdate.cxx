/**
 * @file thdate.cxx
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
 
#include "thdate.h"
#include "thexception.h"

#ifdef THWIN32
#define snprintf _snprintf
#endif

void thdate::reset()
{
  this->syear = -1;
  this->smonth = -1;
  this->sday = -1;
  this->shour = -1;
  this->smin = -1;
  this->eyear = -1;
  this->emonth = -1;
  this->eday = -1;
  this->ehour = -1;
  this->emin = -1;
  this->ssec = -1.0;
  this->esec = -1.0;
  
  this->dstr[0] = 0;
}

thdate::thdate()
{
  this->reset();
}


thdate::~thdate()
{
}


void thdate::parse(char * src)
{

  this->reset();
  
  unsigned char * osrc = (unsigned char *)src;
  unsigned char * ssrc = (unsigned char *)src;
  
  int ppos = 0;
  int dd[14]; 
    dd[0] = -1; dd[1] = -1; dd[2] = -1; 
    dd[3] = -1; dd[4] = -1; dd[5] = -1; dd[6] = -1; 
    dd[7] = -1; dd[8] = -1; dd[9] = -1; 
    dd[10] = -1; dd[11] = -1; dd[12] = -1; dd[13] = -1;
  double ss[2]; ss[0] = -1.0; ss[1] = -1.0;
  double sm[2]; sm[0] = 1; sm[1] = 1.0;
  while (ppos > -1) {
  
    if (*ssrc == 32) {
      ssrc++;
      continue;
    }
  
  
    if (*ssrc == 0) {
      ppos = -1;
      ssrc++;
      continue;
    }
    
    if (*ssrc == 46) {
      if ((ppos == 0) || (ppos == 1) || (ppos == 5) ||
          (ppos == 7) || (ppos == 8) || (ppos == 12))
        ppos++;
      else
        ththrow(("invalid date syntax -- \"%s\"", osrc))
      ssrc++;
      continue;
    }
        
    if (*ssrc == 58) {
      if ((ppos == 3) || (ppos == 4) ||
          (ppos == 10) || (ppos == 11))
        ppos++;
      else
        ththrow(("invalid date syntax -- \"%s\"", osrc))
      ssrc++;
      continue;
    }
        
    if (*ssrc == 64) {
      if ((ppos >= 0) && (ppos <= 2))
        ppos = 3;
      else {
        if ((ppos >= 7) && (ppos <= 9))
          ppos = 10;
        else
          ththrow(("invalid date syntax -- \"%s\"", osrc))   
      }
      ssrc++;
      continue;
    }

    if (*ssrc == 45) {
      if ((ppos >= 0) && (ppos <= 6))
        ppos = 7;
      else
        ththrow(("invalid date syntax -- \"%s\"", osrc))
      ssrc++;
      continue;
    }
    
    if ((*ssrc >= 48) && (*ssrc <= 57)) {
      int digit = *ssrc - 48;
      if ((ppos == 6) || (ppos == 13)) {
        int si = (ppos == 6 ? 0 : 1);
        if (ss[si] < 0.0) {
          ss[si] = (double) digit;
          sm[si] = 10;
        }
        else {
          ss[si] = 10 * ss[si] + (double) digit;
          sm[si] = 10 * sm[si];
        }
      }
      else {
        if (dd[ppos] < 0)
          dd[ppos] = digit;
        else
          dd[ppos] = 10 * dd[ppos] + digit;
      }
      ssrc++;
      continue;
    }
    else
      ththrow(("invalid date syntax -- \"%s\"", osrc))
  }  // while
  
  if (dd[0] < 0)
    ththrow(("invalid date year -- \"%s\"", osrc))
  else
    this->syear = dd[0];
    
  if (dd[1] > 0) {
    if (dd[1] < 13)
      this->smonth = dd[1];
    else
      ththrow(("invalid date month -- \"%s\"", osrc))
  }
  
  if (dd[2] > 0) {
    if ((dd[2] < 32) && (this->smonth > 0))
      this->sday = dd[2];
    else
      ththrow(("invalid date day -- \"%s\"", osrc))
  }

  if (dd[3] >= 0) {
    if ((dd[3] < 24) && (this->sday > 0))
      this->shour = dd[3];
    else
      ththrow(("invalid date hour -- \"%s\"", osrc))
  }

  if (dd[4] >= 0) {
    if ((dd[4] < 60) && (this->shour >= 0))
      this->smin = dd[4];
    else
      ththrow(("invalid date minute -- \"%s\"", osrc))
  }

  if (dd[5] >= 0) {
    if ((dd[5] < 60) && (this->smin >= 0))
      this->ssec = (double) dd[5];
    else
      ththrow(("invalid date second -- \"%s\"", osrc))
  }
  
  if (ss[0] > 0) {
    if (this->smin >= 0) {
      if (this->ssec < 0)
        this->ssec = 0.0;
      this->ssec += ss[0] / sm[0];
    }
    else
      ththrow(("invalid date second -- \"%s\"", osrc))
  }

  if (dd[7] >= 0) {
    if (this->syear >= 0)
      this->eyear = dd[7];
    else
      ththrow(("start date must be specified -- \"%s\"", osrc))
  }
    
  if (dd[8] > 0) {
    if ((dd[8] < 13) && (this->eyear >= 0))
      this->emonth = dd[8];
    else
      ththrow(("invalid date month -- \"%s\"", osrc))
  }
  
  if (dd[9] > 0) {
    if ((dd[9] < 32) && (this->emonth > 0))
      this->eday = dd[9];
    else
      ththrow(("invalid date day -- \"%s\"", osrc))
  }

  if (dd[10] >= 0) {
    if ((dd[10] < 24) && (this->eday > 0))
      this->ehour = dd[10];
    else
      ththrow(("invalid date hour -- \"%s\"", osrc))
  }

  if (dd[11] >= 0) {
    if ((dd[11] < 60) && (this->ehour >= 0))
      this->emin = dd[11];
    else
      ththrow(("invalid date minute -- \"%s\"", osrc))
  }

  if (dd[12] >= 0) {
    if ((dd[12] < 60) && (this->emin >= 0))
      this->esec = (double) dd[12];
    else
      ththrow(("invalid date second -- \"%s\"", osrc))
  }
  
  if (ss[1] > 0) {
    if (this->emin >= 0) {
      if (this->esec < 0)
        this->esec = 0.0;
      this->esec += ss[1] / sm[1];
    }
    else
      ththrow(("invalid date second -- \"%s\"", osrc))
  }
  
  this->prnstr();

}  // parse
  
  
bool thdate::is_interval()
{
  if (this->eyear != -1)
    return true;
  else
    return false;
}


bool thdate::is_defined()
{
  if (this->syear != -1)
    return true;
  else
    return false;
}


bool thdate::is_less(int y1, int m1, int d1, int h1, int n1, double s1,
  int y2, int m2, int d2, int h2, int n2, double s2)
{
  if (y1 < y2)
    return true;
  else if (y1 == y2) {
    if (m1 < m2)
      return true;
    else if (m1 == m2) {
      if (d1 < d2)
        return true;
      else if (d1 == d2) {
        if (h1 < h2)
          return true;
        else if (h1 == h2) {
          if (n1 < n2)
            return true;
          else if (n1 == n2) {
            if (s1 < s2)
              return true;
          }
        }
      } 
    }
  }
  return false;
}


bool thdate::is_greater(int y1, int m1, int d1, int h1, int n1, double s1,
  int y2, int m2, int d2, int h2, int n2, double s2)
{
  if (y1 > y2)
    return true;
  else if (y1 == y2) {
    if (m1 > m2)
      return true;
    else if (m1 == m2) {
      if (d1 > d2)
        return true;
      else if (d1 == d2) {
        if (h1 > h2)
          return true;
        else if (h1 == h2) {
          if (n1 > n2)
            return true;
          else if (n1 == n2) {
            if (s1 > s2)
              return true;
          }
        }
      } 
    }
  }
  return false;
}


bool operator < (const thdate & d1, const thdate & d2)
{
  return thdate::is_less(d1.syear, d1.smonth, d1.sday, d1.shour, d1.smin, d1.ssec,
    d2.syear, d2.smonth, d2.sday, d2.shour, d2.smin, d2.ssec);
}


bool operator > (const thdate & d1, const thdate & d2)
{
  return thdate::is_greater(d1.syear, d1.smonth, d1.sday, d1.shour, d1.smin, d1.ssec,
    d2.syear, d2.smonth, d2.sday, d2.shour, d2.smin, d2.ssec);
}


bool operator == (const thdate & d1, const thdate & d2)
{
  return !((d1 < d2) || (d1 > d2));
}


bool operator <= (const thdate & d1, const thdate & d2)
{
  return !(d1 > d2);
}


bool operator >= (const thdate & d1, const thdate & d2)
{
  return !(d1 < d2);
}


bool operator != (const thdate & d1, const thdate & d2)
{
  return ((d1 < d2) || (d1 > d2));
}


void thdate::join(const thdate & dt) 
{
  if ((thdate::is_less(dt.syear, dt.smonth, dt.sday, 
      dt.shour, dt.smin, dt.ssec,
      this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec))
      || (this->syear == -1)) {
    if (this->eyear == -1) {
      this->eyear = this->syear;
      this->emonth = this->smonth;
      this->eday = this->sday;
      this->ehour = this->shour;
      this->emin = this->smin;
      this->esec = this->ssec;
    }
    this->syear = dt.syear;
    this->smonth = dt.smonth;
    this->sday = dt.sday;
    this->shour = dt.shour;
    this->smin = dt.smin;
    this->ssec = dt.ssec;
  }
  if ((thdate::is_greater(dt.syear, dt.smonth, dt.sday, 
      dt.shour, dt.smin, dt.ssec,
      this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec)) && (*this != dt)) {
    this->eyear = dt.syear;
    this->emonth = dt.smonth;
    this->eday = dt.sday;
    this->ehour = dt.shour;
    this->emin = dt.smin;
    this->esec = dt.ssec;
  }
  if (thdate::is_greater(dt.eyear, dt.emonth, dt.eday, 
      dt.ehour, dt.emin, dt.esec,
      this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec)) {
    this->eyear = dt.eyear;
    this->emonth = dt.emonth;
    this->eday = dt.eday;
    this->ehour = dt.ehour;
    this->emin = dt.emin;
    this->esec = dt.esec;
  }

  this->prnstr();

}


void thdate::prnstr()
{
  int tl = 52;
  char * dst = &(this->dstr[0]);

  if (this->ssec >= 0.0)
    snprintf(dst,tl,"%04d.%02d.%02d %02d:%02d:%05.2f",
      this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec);
  else if (this->smin >= 0)
    snprintf(dst,tl,"%04d.%02d.%02d %02d:%02d",
      this->syear, this->smonth, this->sday,
      this->shour, this->smin);
  else if (this->shour >= 0)
    snprintf(dst,tl,"%04d.%02d.%02d %02d:%02d",
      this->syear, this->smonth, this->sday,
      this->shour, 0);
  else if (this->sday > 0)
    snprintf(dst,tl,"%04d.%02d.%02d",
      this->syear, this->smonth, this->sday);
  else if (this->smonth > 0)
    snprintf(dst,tl,"%04d.%02d",
      this->syear, this->smonth);
  else if (this->syear > 0)
    snprintf(dst,tl,"%04d",
      this->syear);
      
  tl -= strlen(dst);
  dst += strlen(dst);
      
  if (this->esec >= 0.0)
    snprintf(dst,tl," - %04d.%02d.%02d %02d:%02d:%05.2f",
      this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec);
  else if (this->emin >= 0)
    snprintf(dst,tl," - %04d.%02d.%02d %02d:%02d",
      this->eyear, this->emonth, this->eday,
      this->ehour, this->emin);
  else if (this->ehour >= 0)
    snprintf(dst,tl," - %04d.%02d.%02d %02d:%02d",
      this->eyear, this->emonth, this->eday,
      this->ehour, 0);
  else if (this->eday > 0)
    snprintf(dst,tl," - %04d.%02d.%02d",
      this->eyear, this->emonth, this->eday);
  else if (this->emonth > 0)
    snprintf(dst,tl," - %04d.%02d",
      this->eyear, this->emonth);
  else if (this->eyear > 0)
    snprintf(dst,tl," - %04d",
      this->eyear);
}


char * thdate::get_str() {
  return &(this->dstr[0]);
}


double thdate_d2y(int year, int month, int day, int hour, int min, double sec)
{

  double ret = (double) year;
  double nmd = 31.0;
  static const double mdays[12] = {31.0,29.0,31.0,30.0,31.0,30.0,
      31.0,31.0,30.0,31.0,30.0,31.0};

  if ((month > 0) && (month < 13)) {
    ret += ((double) month - 1.0) / 12.0;
    nmd = mdays[month - 1];
    if ((month == 2) && 
        (((year % 400) == 0) || (((year % 4) == 0) && ((year % 100) != 0))))
      nmd = 28.0;    
  }
  else
    return ret;
 
  if ((day > 0) && (day < 32)) {
    ret += (double) (day - 1) / (12.0 * nmd);
  }
  else
    return ret;
   
  if (hour >= 0) {
    ret += (double) hour / (12.0 * nmd * 24.0);
  }
  else
    return ret;
   
  if (min >= 0) {
    ret += (double) min / (12.0 * nmd * 24.0 * 60);
  }
  else
    return ret;
  
  if (sec >= 0) {
    ret += (double) sec / (12.0 * nmd * 24.0 * 3600);
  }

  return ret;
}


void thdate_y2d(double dy, int  & year, 
  int & month, int & day, int & hour, int & min, double & sec)
{
  double rest = 0;
  double nmd = 31.0;
  static const double mdays[12] = {31.0,29.0,31.0,30.0,31.0,30.0,
      31.0,31.0,30.0,31.0,30.0,31.0};
  year = int(dy);
  rest = dy - (double) (year);
  month = int(rest * 12.0);
  rest = (rest * 12.0) - (double) month;
  month = month + 1;
  nmd = mdays[month - 1];
  if ((month == 2) && 
      (((year % 400) == 0) || (((year % 4) == 0) && ((year % 100) != 0))))
    nmd = 28.0;    
  day = int(rest * nmd);
  rest = (rest * nmd) - (double) day;
  day = day + 1;
  hour = int(rest * 24);
  rest = (rest * 24) - (double) hour;
  min = int(rest * 60);
  rest = (rest * 60) - (double) hour;
  sec = rest * 60;
  if (sec < 0.01) {
    sec = -1.0;
    if (min == 0) {
      min = -1;
      if (hour == 0) {
        hour = -1;
        if (day == 1) {
          day = -1;
          if (month == 1) {
            month = -1;
          }
        }
      }
    }
  }
}


double thdate::get_start_year()
{
  return thdate_d2y(this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec);
}

  
double thdate::get_end_year()
{
  return thdate_d2y(this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec);
}

void thdate::set_years(double sy, double ey) 
{
  thdate_y2d(sy, this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec);
  thdate_y2d(ey, this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec);
  this->prnstr();
}

void thdate::print_export_str(int fmt) {
  int tl = 52;
  char * dst = &(this->dstr[0]);

  if (this->ssec >= 0.0)
    snprintf(dst,tl,"%d.%d.%d %02d:%02d:%05.2f",
      this->sday, this->smonth, this->syear,
      this->shour, this->smin, this->ssec);
  else if (this->smin >= 0)
    snprintf(dst,tl,"%d.%d.%d %02d:%02d",
      this->sday, this->smonth, this->syear,
      this->shour, this->smin);
  else if (this->shour >= 0)
    snprintf(dst,tl,"%d.%d.%d %02d:%02d",
      this->sday, this->smonth, this->syear,
      this->shour, 0);
  else if (this->sday > 0)
    snprintf(dst,tl,"%d.%d.%d",
      this->sday, this->smonth, this->syear);
  else if (this->smonth > 0)
    snprintf(dst,tl,"%d.%d",
      this->smonth, this->syear);
  else if (this->syear > 0)
    snprintf(dst,tl,"%d",
      this->syear);
      
  tl -= strlen(dst);
  dst += strlen(dst);
      
  if (this->esec >= 0.0)
    snprintf(dst,tl," - %d.%d.%d %02d:%02d:%05.2f",
      this->eday, this->emonth, this->eyear,
      this->ehour, this->emin, this->esec);
  else if (this->emin >= 0)
    snprintf(dst,tl," - %d.%d.%d %02d:%02d",
      this->eday, this->emonth, this->eyear,
      this->ehour, this->emin);
  else if (this->ehour >= 0)
    snprintf(dst,tl," - %d.%d.%d %02d:%02d",
      this->eday, this->emonth, this->eyear,
      this->ehour, 0);
  else if (this->eday > 0)
    snprintf(dst,tl," - %d.%d.%d",
      this->eday, this->emonth, this->eyear);
  else if (this->emonth > 0)
    snprintf(dst,tl," - %d.%d",
      this->emonth, this->eyear);
  else if (this->eyear > 0)
    snprintf(dst,tl," - %d",
      this->eyear);
}



