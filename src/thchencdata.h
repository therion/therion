/**
 * @file thchencdata.h
 * Therion character encodings module data.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */
  
#ifndef thchencdata_h
#define thchencdata_h

#include "thparse.h"


/**
 * Encoding tokens.
 */

enum {
	TT_UNKNOWN_ENCODING = -1,
	TT_ASCII = 0,
	TT_CP1250 = 1,
	TT_ISO8859_1 = 2,
	TT_ISO8859_2 = 3,
	TT_ISO8859_5 = 4,
	TT_ISO8859_7 = 5,
	TT_UTF_8 = 6
};


/**
 * Encoding parsing table.
 */
 
static const thstok thtt_encoding[] = {
	{"ASCII",	TT_ASCII},
	{"CP1250",	TT_CP1250},
	{"ISO8859-1",	TT_ISO8859_1},
	{"ISO8859-2",	TT_ISO8859_2},
	{"ISO8859-5",	TT_ISO8859_5},
	{"ISO8859-7",	TT_ISO8859_7},
	{"UTF-8",	TT_UTF_8},
	{"ascii",	TT_ASCII},
	{"cp1250",	TT_CP1250},
	{"iso8859-1",	TT_ISO8859_1},
	{"iso8859-2",	TT_ISO8859_2},
	{"iso8859-5",	TT_ISO8859_5},
	{"iso8859-7",	TT_ISO8859_7},
	{"utf-8",	TT_UTF_8},
	{NULL, TT_UNKNOWN_ENCODING}
};


#endif
