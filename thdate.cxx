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
#include "thparse.h"
#include "thexception.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <locale.h>
#ifndef THMSVC
#include <unistd.h>
#endif
#include <errno.h>

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
}


void thdate::reset_current()
{
  this->reset();
  time_t rawtime;
  struct tm * tim;
  time ( &rawtime );
  tim = localtime ( &rawtime );
  this->syear = 1900 + tim->tm_year;
  this->smonth = tim->tm_mon + 1;
  this->sday = tim->tm_mday;
  this->shour = tim->tm_hour;
  this->smin = tim->tm_min;
  this->ssec = double(tim->tm_sec > 59 ? 59 : tim->tm_sec);

}


thdate::thdate()
{
  this->reset();
}


thdate::~thdate()
{
}


void thdate::parse(const char * src)
{

  this->reset();
  
  unsigned const char * osrc = (unsigned const char *)src;
  unsigned const char * ssrc = (unsigned const char *)src;
  
  int ppos = 0;
  int dd[14]; 
    dd[0] = -1; dd[1] = -1; dd[2] = -1; 
    dd[3] = -1; dd[4] = -1; dd[5] = -1; dd[6] = -1; 
    dd[7] = -1; dd[8] = -1; dd[9] = -1; 
    dd[10] = -1; dd[11] = -1; dd[12] = -1; dd[13] = -1;
  double ss[2]; ss[0] = -1.0; ss[1] = -1.0;
  double sm[2]; sm[0] = 1.0; sm[1] = 1.0;
  bool firstchar = true;
  while (ppos > -1) {
  
    if (*ssrc == 32) {
      ssrc++;
      continue;
    }
    
    if (firstchar && (*ssrc == 45)) {
      if (ssrc[1] == 0)
        return;
    }
    firstchar = false;
    
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
  
}  // parse
  
  
bool thdate::is_interval()
{
  if (this->eyear > -1)
    return true;
  else
    return false;
}


bool thdate::is_defined()
{
  if (this->syear > -1)
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


void thdate::join(thdate & dt) 
{
  if (dt.syear <= -1)
    return;
  if ((thdate::is_less(dt.syear, dt.smonth, dt.sday, 
      dt.shour, dt.smin, dt.ssec,
      this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec))
      || (this->syear <= -1)) {
    if (this->eyear <= -1) {
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

}


char * thdate::get_str(int fmt) {
  this->print_str(fmt);
  return &(this->dstr[0]);
}


double thdate_d2y(int year, int month, int day, int hour, int min, double sec)
{

  double ret = (double) year;

  if (month < 1 || month > 12) {
    // Use midpoint of year if no month specified.
    return ret + 0.5;
  }

  static const double start_day[12] = {
    0.0, 31.0, 59.0, 90.0, 120.0, 151.0,
    181.0, 212.0, 243.0, 273.0, 304.0, 334.0
  };

  double msd = start_day[month - 1];
  double yl = 365.0;
  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
    // Leap year.
    yl = 366.0;
    if (month > 2) ++msd;
  }

  // Calculate the (possibly fractional) day of the year.
  double doy = msd;
  if (day >= 1 && day <= 32) {
    doy += (double) (day - 1);
    if (hour >= 0) {
      doy += (double) hour / 24.0;
      if (min >= 0) {
        doy += (double) min / (24.0 * 60.0);
        if (sec >= 0) {
          doy += (double) sec / (24.0 * 60.0 * 60.0);
        }
      }
    } else {
      // Use midday if no hour specified.
      doy += 0.5;
    }
  } else {
    // Use 15th of the month if no day specified.
    doy += (double) (15 - 1);
  }

  ret += doy / yl;
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

double thdate::get_average_year()
{
  if (this->is_interval()) {
    return ((this->get_start_year() + this->get_end_year()) / 2.0);
  } else {
    return this->get_start_year();
  }
}


void thdate::set_years(double sy, double ey) 
{
  thdate_y2d(sy, this->syear, this->smonth, this->sday,
      this->shour, this->smin, this->ssec);
  thdate_y2d(ey, this->eyear, this->emonth, this->eday,
      this->ehour, this->emin, this->esec);
}

void date2tm(int y, int m, int d, int hh, int mm, double ss, tm * info)
{
  if (y >= 0)
    info->tm_year = y - 1900;
  else
    info->tm_year = 100;
  if (m > 0)
    info->tm_mon = m - 1;
  else
    info->tm_mon = 0;
  if (d > 0)
    info->tm_mday = d;
  else
    info->tm_mday = 1;
  if (hh >= 0)
    info->tm_hour = hh;
  else
    info->tm_hour = 0;
  if (mm >= 0)
    info->tm_min = mm;
  else
    info->tm_min = 0;
  if (ss >= 0.0)
    info->tm_sec = int(ss);
  else
    info->tm_sec = 0;

}

void thdate::print_str(int fmt) {
  unsigned int tl = thdate_bufflen - 1;
  long yyyy, mm, dd;
  char * dst = &(this->dstr[0]);
  const char * sep = " - ";

  switch (fmt) {
    case TT_DATE_FMT_UTF8_Y:
      sep = " \xe2\x80\x93 ";
    case TT_DATE_FMT_Y:
      if (this->syear >= 0)
        std::snprintf(dst,tl,"%d",
        this->syear);
      tl -= strlen(dst);
      dst += strlen(dst);
      if ((this->eyear >= 0) && (this->syear != this->eyear))
        std::snprintf(dst,tl,"%s%d", sep,
          this->eyear);          
      break;
      
 
    case TT_DATE_FMT_SQL_SINGLE:
      if (this->eyear >= 0) {
        yyyy = this->eyear;
        mm = this->emonth;
        dd = this->eday;
      } else {
        yyyy = this->syear;
        mm = this->smonth;
        dd = this->sday;
      }
      if (dd > 0 && ((yyyy >= 1000) && (yyyy <= 9999))) {
        std::snprintf(dst,tl,"'%04ld-%02ld-%02ld'",yyyy,mm,dd);
      } else if ((mm > 0) && ((yyyy >= 1000) && (yyyy <= 9999))) {
        std::snprintf(dst,tl,"'%04ld-%02ld-01'",yyyy,mm);
      } else if ((yyyy >= 1000) && (yyyy <= 9999)) {
        std::snprintf(dst,tl,"'%04ld-01-01'",yyyy);
      } else {
        std::snprintf(dst,tl,"NULL");
      }
      break;

    case TT_DATE_FMT_LOCALE:
      {
        struct tm s, e;
        date2tm(this->syear, this->smonth, this->sday, this->shour, this->smin, this->ssec, &s);
        date2tm(this->eyear, this->emonth, this->eday, this->ehour, this->emin, this->esec, &e);
        setlocale (LC_TIME,"");
        if (this->shour >= 0)
          strftime(dst, tl, "%c", &s);
        else
          strftime(dst, tl, "%x", &s);
        tl -= strlen(dst);        
        dst += strlen(dst);
        if (this->eyear > 0) {
          if (this->ehour >= 0)
            strftime(dst, tl, " - %c", &e);
          else
            strftime(dst, tl, " - %x", &e);
        }
      }
      break;
      
      
    case TT_DATE_FMT_UTF8_ISO:
      sep = " \xe2\x80\x93 ";
    default:
      if (this->ssec >= 0.0)
        std::snprintf(dst,tl,"%04d-%02d-%02dT%02d:%02d:%05.2f",
          this->syear, this->smonth, this->sday, 
          this->shour, this->smin, this->ssec);
      else if (this->smin >= 0)
        std::snprintf(dst,tl,"%04d-%02d-%02dT%02d:%02d",
          this->syear, this->smonth, this->sday, 
          this->shour, this->smin);
      else if (this->shour >= 0)
        std::snprintf(dst,tl,"%04d-%02d-%02dT%02d:%02d",
          this->syear, this->smonth, this->sday, 
          this->shour, 0);
      else if (this->sday > 0)
        std::snprintf(dst,tl,"%04d-%02d-%02d",
          this->syear, this->smonth, this->sday);
      else if (this->smonth > 0)
        std::snprintf(dst,tl,"%04d-%02d",
          this->syear, this->smonth);
      else if (this->syear >= 0)
        std::snprintf(dst,tl,"%04d",
          this->syear);
          
      tl -= strlen(dst);
      dst += strlen(dst);
      
      if (this->esec >= 0.0)
        std::snprintf(dst,tl,"%s%04d-%02d-%02dT%02d:%02d:%05.2f", sep,
          this->eyear, this->emonth, this->eday, 
          this->ehour, this->emin, this->esec);
      else if (this->emin >= 0)
        std::snprintf(dst,tl,"%s%04d-%02d-%02dT%02d:%02d", sep,
          this->eyear, this->emonth, this->eday, 
          this->ehour, this->emin);
      else if (this->ehour >= 0)
        std::snprintf(dst,tl,"%s%04d-%02d-%02dT%02d:%02d", sep,
          this->eyear, this->emonth, this->eday, 
          this->ehour, 0);
      else if (this->eday > 0)
        std::snprintf(dst,tl,"%s%04d-%02d-%02d", sep,
          this->eyear, this->emonth, this->eday);
      else if (this->emonth > 0)
        std::snprintf(dst,tl,"%s%04d-%02d", sep,
          this->eyear, this->emonth);
      else if (this->eyear >= 0)
        std::snprintf(dst,tl,"%s%04d", sep,
          this->eyear);
      break;
  }    

}

#ifdef THMSVC
#define stat _stat
#endif


void thdate::set_file_date(char * fname) {
  struct stat buf;
  struct tm * tim;
  if ((fname == NULL) || (strlen(fname) == 0))
    return;
  if (stat(fname, &buf) != 0)
    return;
  tim = localtime(&(buf.st_mtime));
  this->syear = 1900 + tim->tm_year;
  this->smonth = tim->tm_mon + 1;
  this->sday = tim->tm_mday;
  this->shour = tim->tm_hour;
  this->smin = tim->tm_min;
  this->ssec = double(tim->tm_sec > 59 ? 59 : tim->tm_sec);
#ifdef THDEBUG
  thprintf("FILEDATE: %s => %s\n", fname, this->get_str());
#endif   
}

time_t thdate::get_start_t_time() {
  struct tm temp;
  date2tm(this->syear, this->smonth, this->sday, this->shour, this->smin, this->ssec, &temp);
  return mktime(&temp);
}

time_t thdate::get_end_t_time() {
  struct tm temp;
  date2tm(this->eyear, this->emonth, this->eday, this->ehour, this->emin, this->esec, &temp);
  return mktime(&temp);
}

int rdn(int y, int m, int d) { /* Rata Die day one is 0001-01-01. Used to calculate number of days since 1.1.1900 */
  if (m < 3)
    y--, m += 12;
  return 365*y + y/4 - y/100 + y/400 + (153*m - 457)/5 + d - 306;
}

int thdate::get_start_days1900() {
  int basedays = rdn(1900, 1, 1); 
  int thdays = rdn(this->syear, this->smonth, this->sday);
  return thdays - basedays;
}

int thdate::get_end_days1900() {
  int basedays = rdn(1900, 1, 1); 
  int thdays = rdn(this->eyear, this->emonth, this->eday);
  return thdays - basedays;
}

#ifdef THMSVC
#undef stat
#endif

