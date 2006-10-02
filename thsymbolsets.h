#ifndef thsymbolsets_h
#define thsymbolsets_h

#include "thsymbolsetlist.h"

#include <map>
#include <string>


#define thsymsets_size 6

extern int thsymsets_symbols [thsymbolset_size][thsymsets_size];

extern int thsymsets_figure [thsymbolset_size][thsymsets_size+1];

extern int thsymsets_order[thsymbolset_size];

extern int thsymsets_count[thsymsets_size];

extern std::map<unsigned, std::string> thsymsets_comment;


static const thsymbolset__char_ptr thsymsets [] = {
"ASF",
"BCRA",
"NSS",
"SKBB",
"SM",
"UIS",
};

void thsymsets_symbols_init();

#endif

