/**
 * @file thparse.h
 * Parsing module.
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
 
#ifndef thparse_h
#define thparse_h

#include <stdio.h>
#include <string>
#include "thbuffer.h"
#include "thmbuffer.h"


enum {
  TT_IMG_TYPE_UNKNOWN,
  TT_IMG_TYPE_JPEG,
  TT_IMG_TYPE_PNG,
};

/**
 * Token definition structure.
 */

typedef struct {
  const char * s;  ///< String.
  int tok;  ///< Token.
} thstok;


/**
 * Match string token.
 *
 * Scan array of token definitions (stok) for given string. If not found,
 * last element of array is returned. This array should allways be 
 * alphabetically sorted and last element should be set to
 * like TT_UNKNOWN.
 *
 * @param buffer String buffer.
 * @param tab Array of token definitions.
 * @param tab_size Size of token array (ID of last element).
 *
 * @sa thmatch_token
 */
 
int thmatch_stok(const char *buffer, const thstok *tab, int tab_size);
 
int thcasematch_stok(const char *buffer, const thstok *tab, int tab_size);


/**
 * Match string token macro.
 */

#define thmatch_token(buffer, table) thmatch_stok(buffer, table,\
  ((sizeof(table)/sizeof(thstok)) - 1))

#define thcasematch_token(buffer, table) thcasematch_stok(buffer, table,\
  ((sizeof(table)/sizeof(thstok)) - 1))


/**
 * Match token string.
 *
 * Inverse function to thmatch_stok().
 */
 
const char * thmatch_tstring(int token, const thstok *tab, int tab_size);


/**
 * Match token string macro.
 */

#define thmatch_string(token, table) thmatch_tstring(token, table,\
  ((sizeof(table)/sizeof(thstok)) - 1))


/**
 * Boolean tokens.
 */

enum {
  TT_UNKNOWN_BOOL = 0,  ///< Not match.
  TT_TRUE,  ///< yes, on, 1, true.
  TT_FALSE,  ///< no, off, 0, false.
  TT_AUTO,  ///< automatical setting
};


/**
 * Boolean token table.
 *
 * Defines following:
 * - 1, on, yes, true = TT_TRUE
 * - 0, off, no, false = TT_FALSE
 * - TT_UNKNOWN_BOOL otherwise
 */
 
static const thstok thtt_bool[] = {
//  {"0", TT_FALSE},
//  {"1", TT_TRUE},
//  {"false", TT_FALSE},
//  {"no", TT_FALSE},
  {"off", TT_FALSE},
  {"on", TT_TRUE},
//  {"true", TT_TRUE},
//  {"yes", TT_TRUE},
  {NULL, TT_UNKNOWN_BOOL}
};


/**
 * ON/OFF/AUTO token table.
 */
 
static const thstok thtt_onoffauto[] = {
  {"auto", TT_AUTO},
  {"off", TT_FALSE},
  {"on", TT_TRUE},
  {NULL, TT_UNKNOWN_BOOL}
};


/**
 * Special values tokens.
 */

enum {
  TT_SV_UNKNOWN,  ///< Unknown special value
  TT_SV_NUMBER,  ///< A real number.
  TT_SV_NAN,  ///< Not a number.
  TT_SV_PINF,  ///< Positive infinity.
  TT_SV_NINF,  ///< Negative infinity.
  TT_SV_UP,  ///< Up direction.
  TT_SV_DOWN,  ///< Down direction.
  TT_SV_ALL,  ///< All
  TT_SV_OFF,  ///< Off
};


/**
 * Special values token table.
 *
 * Defines following:
 * - "-", ".", "nan", "NAN", "NaN" = TT_SV_NAN
 * - "Inf", "inf", "INF" = TT_SV_PINF
 * - "-Inf", "-inf", "-INF" = TT_SV_NINF
 * - "up", "Up", "UP" = TT_SV_UP
 * - "down", "Down", "DOWN" = TT_SV_DOWN
 * - TT_SV_UNKNOWN otherwise
 */
 
static const thstok thtt_special_val[] = {
  {"-", TT_SV_NAN},
  {"-INF", TT_SV_NINF},
  {"-Inf", TT_SV_NINF},
  {"-inf", TT_SV_NINF},
  {".", TT_SV_NAN},
  {"DOWN", TT_SV_DOWN},
  {"Down", TT_SV_DOWN},
  {"INF", TT_SV_PINF},
  {"Inf", TT_SV_PINF},
  {"NAN", TT_SV_NAN},
  {"NaN", TT_SV_NAN},
  {"UP", TT_SV_UP},
  {"Up", TT_SV_UP},
  {"all", TT_SV_ALL},
  {"down", TT_SV_DOWN},
  {"inf", TT_SV_PINF},
  {"nan", TT_SV_NAN},
  {"off", TT_SV_OFF},
  {"up", TT_SV_UP},
  {NULL, TT_SV_UNKNOWN}
};


/**
 * Parse double value (with check for special values).
 */
 
void thparse_double(int & sv, double & dv, const char * src); 

/**
 * Parse length value.
 */
 
void thparse_length(int & sv, double & dv, const char * src); 

/**
 * Parse double value given in DMS (with check for special values).
 */
 
void thparse_double_dms(int & sv, double & dv, const char * src); 

/**
 * Update double value (with check for special values).
 */
 
#define thupdate_double(ov, uv) if (!(thisnan(uv))) ov = uv


/**
 * Split given string into first word and the rest.
 */
 
void thsplit_word(thbuffer * dword, thbuffer * drest, const char * src);


/**
 * Split given string into words (separated by white spaces).
 */
 
void thsplit_words(thmbuffer * dest, const char * src);


/**
 * Split given string into substrings.
 *
 * In the source, they're separated by given character separator.
 */
 
void thsplit_strings(thmbuffer * dest, const char * src, const char separator);


/**
 * Split given string into paths.
 *
 * Take care of C:\....
 */
 
void thsplit_paths(thmbuffer * dest, const char * src, char separator);


/**
 * Separate given argument string.
 *
 * Arguments are separated by white spaces. Strings could be quoted
 * ("" = ") or given in [] brackets.
 */

void thsplit_args(thmbuffer * dest, const char * src);


/**
 * Separate path part from full file name.
 */
 
void thsplit_fpath(thbuffer * dest, const char * src);


/**
 * Check if given string is keyword.
 *
 * Valid keyword characters are A-Z, a-z, 0-9,'_' and '-'.
 */
 
bool th_is_keyword(const char * str);


/**
 * Check if given string is index (positive integer).
 */
 
bool th_is_index(const char * str);

/**
 * Check if given string is keyword list.
 *
 * Valid keyword characters are A-Z, a-z, 0-9,'_' and '-'.
 *
 * @param str Input string.
 * @param sep Keyword separator.
 */
 
bool th_is_keyword_list(const char * str, char sep);


/**
 * Check if given string is extended keyword.
 *
 * A-Z, a-z, 0-9 and [_.,-/+*']
 */
 
bool th_is_extkeyword(const char * str);


/**
 * Decode string into C format.
 */
 
void thdecode_c(thbuffer * dest, const char * src);

void thdecode_tcl(thbuffer * dest, const char * src);


/**
 * Decode string into TeX format.
 */
 
void thdecode_tex(thbuffer * dest, const char * src);
void thdecode_utf2tex(thbuffer * dest, const char * src);
void thdecode_sql(thbuffer * dest, const char * src);
void thdecode_mp(thbuffer * dest, const char * src);


/**
 * Decode into argument string.
 */
 
void thdecode_arg(thbuffer * dest, const char * src);


/**
 * Parse altitude value
 */
 
void thparse_altitude(const char * src, double & altv, double & fixv);


/**
 * Check image.
 */

void thparse_image(const char * fname, double & width, double & height, double & dpi, int & type);


/**
 * Set color according to color map (TODO).
 */
 
void thset_color(int color_map, double index, double total, double & R, double & G, double & B);


/**
 * Set grid origin and size.
 */
 
void thset_grid(double gorigin, double gsize, double min, double max, double & qstart, long & nquads);



/**
 * Convert therion string to xhtml.
 */
 
const char * thutf82xhtml(const char * src);

/**
 * Base 64 file encoder.
 */

void thbase64_encode(const char * fname, FILE * fout);

/**
 * Check if path is absolute.
 */

bool thpath_is_absolute(const char * fname);

/**
 * Check on attribute name.
 */
bool th_is_attr_name(const char * str);

std::string ths2tex(std::string original, int lang = -1, bool remove_kerning = false);

std::string ths2txt(std::string original, int lang = -1, int encoding = -1);

#endif





