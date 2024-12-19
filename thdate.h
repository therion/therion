/**
 * @file thdate.h
 * Date class.
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
 
#include <time.h>
#include <array>

#ifndef thdate_h
#define thdate_h

#define thdate_bufflen 255

/**
 * Convert date into decimal year.
 */
 
double thdate_d2y(int year, int month, int day, int hour, int min, double sec);


/**
 * Convert decimal year into date.
 */
 
void thdate_y2d(double dy, int  & year, int & month, int & day, int & hour, int & min, double & sec);

/**
 * Date formats.
 */
enum {
  TT_DATE_FMT_ISO,
  TT_DATE_FMT_LOCALE,
  TT_DATE_FMT_LOCALE_LONG,
  TT_DATE_FMT_Y,
  TT_DATE_FMT_UTF8_ISO,
  TT_DATE_FMT_UTF8_Y,
  TT_DATE_FMT_SQL_SINGLE,
  TT_DATE_FMT_THERION,
};

/**
 * Date class.
 *
 * Date is in fact a time interval.
 */
 
class thdate {

  public:

  int syear = -1,  ///< Start date year
    smonth = -1,  ///< Start date month
    sday = -1,  ///< Start date day
    shour = -1,  ///< Star date hour
    smin = -1,  ///< Star date minute
    eyear = -1,  ///< End date year
    emonth = -1,  ///< End date month
    eday = -1,  ///< End date month
    ehour = -1,  ///< End date hour
    emin = -1;  ///< End date minute

  double ssec = -1.0, ///< Start date seconds
    esec = -1.0;  ///< End date seconds
    
  std::array<char, thdate_bufflen> dstr = {}; ///< String for given date.

  friend bool operator < (const thdate &, const thdate &);  ///< Less operator.
  friend bool operator <= (const thdate &, const thdate &);  ///< Less operator.
  friend bool operator > (const thdate &, const thdate &);  ///< Greater operator.
  friend bool operator >= (const thdate &, const thdate &);  ///< Greater operator.
  friend bool operator == (const thdate &, const thdate &);  ///< Equal operator.
  friend bool operator != (const thdate &, const thdate &);  ///< Not equal operator.


  /**
   * Compare two dates if first is less than second, item by item.
   */
   
  static bool is_less(int y1, int m1, int d1, int h1, int n1, double s1,
    int y2, int m2, int d2, int h2, int n2, double s2);


  /**
   * Compare two dates if first is greater than second, item by item.
   */
   
  static bool is_greater(int y1, int m1, int d1, int h1, int n1, double s1,
    int y2, int m2, int d2, int h2, int n2, double s2);
    
  
  /**
   * Create string representation of date.
   */
   
  void print_str(int fmt);
  
  /**
   * Reset the date.
   */
   
  void reset();

  void reset_current();
    
  /**
   * Standard constructor.
   */
  
  thdate() = default;
  
  
  /**
   * Standard destructor.
   */
  
  ~thdate();
  
  
  /**
   * Parse string to date.
   */
   
  void parse(const char * src);
  
  
  /**
   * Return true, if date is a time interval.
   */
   
  bool is_interval();
  

  /**
   * Return true, if date is a defined.
   */
   
  bool is_defined();

  
  /**
   * Join two dates together.
   */
   
  void join(thdate & dt);
  
  
  /**
   * Return string representation of date.
   */
   
  char * get_str(int fmt = TT_DATE_FMT_ISO);
  
  
  /**
   * Return start year as double value.
   */
   
  double get_start_year();

  
  /**
   * Return end year as double value.
   */
   
  double get_end_year();


  /**
   * Return end average as double value.
   */

  double get_average_year();
  
  
  /**
   * Set date years.
   */
   
  void set_years(double sy, double ey);

  /**
   * Get the start or end date as time_t format
   */
  time_t get_start_t_time();
  time_t get_end_t_time();
  
  /**
   * Get the start or end date as number of days since 1 Jan 1900 for Survex export
   */
  int get_start_days1900();
  int get_end_days1900();
};


#endif


