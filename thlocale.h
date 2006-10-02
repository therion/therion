#ifndef thlocale_h
#define thlocale_h

enum {
	TTLC_UNKNOWN = 0,
	TTLC_UNITS_METRIC,
	TTLC_UNITS_IMPERIAL,
};

struct thlocale {

	int units, lang;
	
	thlocale();
	
	void parse_units(char * cc);
	
	double convert_length(double);

	char * format_length(double length, int prec = 0, int total = 1);

  char * format_human_length(double length);
  
	char * format_length_units();	

	char * format_i18n_length_units();
	
};

extern thlocale thdeflocale;

#endif
