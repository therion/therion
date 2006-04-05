#include "thsymbolsets.h"

int thsymsets_symbols [thsymbolset_size][thsymsets_size];

int thsymsets_figure [thsymbolset_size][thsymsets_size+1];

int thsymsets_order[thsymbolset_size];

int thsymsets_count[thsymsets_size];

std::map<unsigned, std::string> thsymsets_comment;




void thsymsets_symbols_init() {
	size_t i, j;
	for(i = 0; i <= thsymbolset_size; i++)
		for(j = 0; j < thsymsets_size; j++) 
			thsymsets_symbols[i][j] = 0;
		
	
	thsymsets_symbols[SYMP_AIRDRAUGHT][4] = 1;
	thsymsets_symbols[SYMP_ANASTOMOSIS][4] = 1;
	thsymsets_symbols[SYMP_ANCHOR][2] = 1;
	thsymsets_symbols[SYMP_ARAGONITE][1] = 1;
	thsymsets_symbols[SYMP_ARCHEOMATERIAL][4] = 1;
	thsymsets_symbols[SYMP_BEDROCK][0] = 1;
	thsymsets_symbols[SYMP_BLOCKS][4] = 1;
	thsymsets_symbols[SYMP_BREAKDOWNCHOKE][1] = 1;
	thsymsets_symbols[SYMP_BRIDGE][2] = 1;
	thsymsets_symbols[SYMP_CAMP][2] = 1;
	thsymsets_symbols[SYMP_CAVEPEARL][2] = 1;
	thsymsets_symbols[SYMP_CLAY][2] = 1;
	thsymsets_symbols[SYMP_CONTINUATION][4] = 1;
	thsymsets_symbols[SYMP_CRYSTAL][4] = 1;
	thsymsets_symbols[SYMP_CURTAIN][4] = 1;
	thsymsets_symbols[SYMP_DEBRIS][4] = 1;
	thsymsets_symbols[SYMP_DISK][4] = 1;
	thsymsets_symbols[SYMP_ENTRANCE][4] = 1;
	thsymsets_symbols[SYMP_FIXEDLADDER][2] = 1;
	thsymsets_symbols[SYMP_FLOWSTONE][4] = 1;
	thsymsets_symbols[SYMP_FLOWSTONECHOKE][1] = 1;
	thsymsets_symbols[SYMP_FLUTE][4] = 1;
	thsymsets_symbols[SYMP_GRADIENT][4] = 1;
	thsymsets_symbols[SYMP_GUANO][4] = 1;
	thsymsets_symbols[SYMP_GYPSUM][1] = 1;
	thsymsets_symbols[SYMP_GYPSUMFLOWER][1] = 1;
	thsymsets_symbols[SYMP_HELICTITE][4] = 1;
	thsymsets_symbols[SYMP_ICE][4] = 1;
	thsymsets_symbols[SYMP_KARREN][4] = 1;
	thsymsets_symbols[SYMP_LOWEND][1] = 1;
	thsymsets_symbols[SYMP_LOWEND][4] = 1;
	thsymsets_symbols[SYMP_MOONMILK][4] = 1;
	thsymsets_symbols[SYMP_NARROWEND][4] = 1;
	thsymsets_symbols[SYMP_NOEQUIPMENT][2] = 1;
	thsymsets_symbols[SYMP_PALEOMATERIAL][4] = 1;
	thsymsets_symbols[SYMP_PEBBLES][4] = 1;
	thsymsets_symbols[SYMP_PILLAR][4] = 1;
	thsymsets_symbols[SYMP_POPCORN][4] = 1;
	thsymsets_symbols[SYMP_RAFT][1] = 1;
	thsymsets_symbols[SYMP_RAFTCONE][1] = 1;
	thsymsets_symbols[SYMP_RIMSTONEDAM][0] = 1;
	thsymsets_symbols[SYMP_RIMSTONEPOOL][0] = 1;
	thsymsets_symbols[SYMP_ROOT][0] = 1;
	thsymsets_symbols[SYMP_ROPE][2] = 1;
	thsymsets_symbols[SYMP_ROPELADDER][2] = 1;
	thsymsets_symbols[SYMP_SAND][4] = 1;
	thsymsets_symbols[SYMP_SCALLOP][4] = 1;
	thsymsets_symbols[SYMP_SINK][2] = 1;
	thsymsets_symbols[SYMP_SNOW][2] = 1;
	thsymsets_symbols[SYMP_SODASTRAW][4] = 1;
	thsymsets_symbols[SYMP_SPRING][2] = 1;
	thsymsets_symbols[SYMP_STALACTITE][4] = 1;
	thsymsets_symbols[SYMP_STALAGMITE][4] = 1;
	thsymsets_symbols[SYMP_STATION_FIXED][0] = 1;
	thsymsets_symbols[SYMP_STATION_NATURAL][0] = 1;
	thsymsets_symbols[SYMP_STATION_PAINTED][2] = 1;
	thsymsets_symbols[SYMP_STATION_TEMPORARY][0] = 1;
	thsymsets_symbols[SYMP_STATION_TEMPORARY][2] = 1;
	thsymsets_symbols[SYMP_STEPS][2] = 1;
	thsymsets_symbols[SYMP_TRAVERSE][2] = 1;
	thsymsets_symbols[SYMP_VEGETABLEDEBRIS][0] = 1;
	thsymsets_symbols[SYMP_WALLCALCITE][4] = 1;
	thsymsets_symbols[SYMP_WATER][4] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_INTERMITTENT][4] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_PALEO][4] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_PERMANENT][4] = 1;
	thsymsets_symbols[SYML_ARROW][2] = 1;
	thsymsets_symbols[SYML_BORDER_PRESUMED][2] = 1;
	thsymsets_symbols[SYML_BORDER_TEMPORARY][2] = 1;
	thsymsets_symbols[SYML_BORDER_VISIBLE][2] = 1;
	thsymsets_symbols[SYML_CEILINGMEANDER][3] = 1;
	thsymsets_symbols[SYML_CEILINGSTEP][2] = 1;
	thsymsets_symbols[SYML_CHIMNEY][4] = 1;
	thsymsets_symbols[SYML_CONTOUR][2] = 1;
	thsymsets_symbols[SYML_CONTOUR][4] = 1;
	thsymsets_symbols[SYML_FLOORMEANDER][3] = 1;
	thsymsets_symbols[SYML_FLOORSTEP][4] = 1;
	thsymsets_symbols[SYML_FLOWSTONE][4] = 1;
	thsymsets_symbols[SYML_GRADIENT][4] = 1;
	thsymsets_symbols[SYML_OVERHANG][2] = 1;
	thsymsets_symbols[SYML_PIT][4] = 1;
	thsymsets_symbols[SYML_ROCKBORDER][4] = 1;
	thsymsets_symbols[SYML_ROCKEDGE][4] = 1;
	thsymsets_symbols[SYML_ROPE][2] = 1;
	thsymsets_symbols[SYML_SECTION][2] = 1;
	thsymsets_symbols[SYML_SLOPE][2] = 1;
	thsymsets_symbols[SYML_SURVEY_CAVE][2] = 1;
	thsymsets_symbols[SYML_SURVEY_CAVE][4] = 1;
	thsymsets_symbols[SYML_SURVEY_SURFACE][2] = 1;
	thsymsets_symbols[SYML_WALL_BEDROCK][4] = 1;
	thsymsets_symbols[SYML_WALL_BLOCKS][2] = 1;
	thsymsets_symbols[SYML_WALL_CLAY][2] = 1;
	thsymsets_symbols[SYML_WALL_DEBRIS][2] = 1;
	thsymsets_symbols[SYML_WALL_ICE][2] = 1;
	thsymsets_symbols[SYML_WALL_PEBBLES][2] = 1;
	thsymsets_symbols[SYML_WALL_PRESUMED][4] = 1;
	thsymsets_symbols[SYML_WALL_SAND][2] = 1;
	thsymsets_symbols[SYML_WALL_UNDERLYING][4] = 1;
	thsymsets_symbols[SYML_WALL_UNSURVEYED][2] = 1;
	thsymsets_symbols[SYML_WATERFLOW_CONJECTURAL][2] = 1;
	thsymsets_symbols[SYML_WATERFLOW_INTERMITTENT][2] = 1;
	thsymsets_symbols[SYML_WATERFLOW_PERMANENT][4] = 1;
	thsymsets_symbols[SYMA_BLOCKS][2] = 1;
	thsymsets_symbols[SYMA_CLAY][2] = 1;
	thsymsets_symbols[SYMA_DEBRIS][2] = 1;
	thsymsets_symbols[SYMA_DEBRIS][4] = 1;
	thsymsets_symbols[SYMA_ICE][2] = 1;
	thsymsets_symbols[SYMA_PEBBLES][2] = 1;
	thsymsets_symbols[SYMA_SAND][4] = 1;
	thsymsets_symbols[SYMA_SNOW][2] = 1;
	thsymsets_symbols[SYMA_SUMP][4] = 1;
	thsymsets_symbols[SYMA_WATER][4] = 1;
}
