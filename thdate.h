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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#ifndef thdate_h
#define thdate_h

#define thdate__bufflen 255

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
};

/**
 * Date class.
 *
 * Date is in fact a time interval.
 */
 
class thdate {

  public:

  int syear,  ///< Start date year
    smonth,  ///< Start date month
    sday,  ///< Stard date day
    shour,  ///< Star date hour
    smin,  ///< Star date minute
    eyear,  ///< End date year
    emonth,  ///< End date month
    eday,  ///< End date month
    ehour,  ///< End date hour
    emin;  ///< End date minute

  double ssec, ///< Start date seconds
    esec;  ///< End date seconds
    
  char dstr[thdate__bufflen]; ///< String for given date.

  friend bool operator < (const thdate &, const thdate &);  ///< Less operator.
  friend bool operator <= (const thdate &, const thdate &);  ///< Less operator.
  friend bool operator > (const thdate &, const thdate &);  ///< Greater operator.
  friend bool operator >= (const thdate &, const thdate &);  ///< Greater operator.
  friend bool operator == (const thdate &, const thdate &);  ///< Equal operator.
  friend bool operator != (const thdate &, const thdate &);  ///< Not equal operator.


  /**
   * Compare two dates if first is less than sencond, item by item.
   */
   
  static bool is_less(int y1, int m1, int d1, int h1, int n1, double s1,
    int y2, int m2, int d2, int h2, int n2, double s2);


  /**
   * Compare two dates if first is greater than sencond, item by item.
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

    
  /**
   * Standard constructor.
   */
  
  thdate();
  
  
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

  void set_file_date(char * fname);
  
};


#endif


