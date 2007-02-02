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
		
	
	thsymsets_symbols[SYMP_AIRDRAUGHT][5] = 1;
	thsymsets_symbols[SYMP_ANASTOMOSIS][5] = 1;
	thsymsets_symbols[SYMP_ANCHOR][3] = 1;
	thsymsets_symbols[SYMP_ARAGONITE][2] = 1;
	thsymsets_symbols[SYMP_ARCHEOMATERIAL][5] = 1;
	thsymsets_symbols[SYMP_BEDROCK][0] = 1;
	thsymsets_symbols[SYMP_BLOCKS][5] = 1;
	thsymsets_symbols[SYMP_BREAKDOWNCHOKE][2] = 1;
	thsymsets_symbols[SYMP_BRIDGE][3] = 1;
	thsymsets_symbols[SYMP_CAMP][3] = 1;
	thsymsets_symbols[SYMP_CAVEPEARL][3] = 1;
	thsymsets_symbols[SYMP_CLAY][3] = 1;
	thsymsets_symbols[SYMP_CONTINUATION][5] = 1;
	thsymsets_symbols[SYMP_CRYSTAL][5] = 1;
	thsymsets_symbols[SYMP_CURTAIN][5] = 1;
	thsymsets_symbols[SYMP_DEBRIS][5] = 1;
	thsymsets_symbols[SYMP_DISK][5] = 1;
	thsymsets_symbols[SYMP_ENTRANCE][5] = 1;
	thsymsets_symbols[SYMP_FIXEDLADDER][3] = 1;
	thsymsets_symbols[SYMP_FLOWSTONE][5] = 1;
	thsymsets_symbols[SYMP_FLOWSTONECHOKE][2] = 1;
	thsymsets_symbols[SYMP_FLUTE][5] = 1;
	thsymsets_symbols[SYMP_GRADIENT][1] = 1;
	thsymsets_symbols[SYMP_GRADIENT][2] = 1;
	thsymsets_symbols[SYMP_GRADIENT][5] = 1;
	thsymsets_symbols[SYMP_GUANO][5] = 1;
	thsymsets_symbols[SYMP_GYPSUM][2] = 1;
	thsymsets_symbols[SYMP_GYPSUMFLOWER][2] = 1;
	thsymsets_symbols[SYMP_HELICTITE][5] = 1;
	thsymsets_symbols[SYMP_ICE][5] = 1;
	thsymsets_symbols[SYMP_KARREN][5] = 1;
	thsymsets_symbols[SYMP_LOWEND][2] = 1;
	thsymsets_symbols[SYMP_LOWEND][5] = 1;
	thsymsets_symbols[SYMP_MOONMILK][5] = 1;
	thsymsets_symbols[SYMP_NARROWEND][5] = 1;
	thsymsets_symbols[SYMP_NOEQUIPMENT][3] = 1;
	thsymsets_symbols[SYMP_PALEOMATERIAL][5] = 1;
	thsymsets_symbols[SYMP_PEBBLES][5] = 1;
	thsymsets_symbols[SYMP_PILLAR][5] = 1;
	thsymsets_symbols[SYMP_POPCORN][5] = 1;
	thsymsets_symbols[SYMP_RAFT][2] = 1;
	thsymsets_symbols[SYMP_RAFTCONE][2] = 1;
	thsymsets_symbols[SYMP_RIMSTONEDAM][0] = 1;
	thsymsets_symbols[SYMP_RIMSTONEPOOL][0] = 1;
	thsymsets_symbols[SYMP_ROOT][0] = 1;
	thsymsets_symbols[SYMP_ROPE][3] = 1;
	thsymsets_symbols[SYMP_ROPELADDER][3] = 1;
	thsymsets_symbols[SYMP_SAND][5] = 1;
	thsymsets_symbols[SYMP_SCALLOP][5] = 1;
	thsymsets_symbols[SYMP_SINK][3] = 1;
	thsymsets_symbols[SYMP_SNOW][3] = 1;
	thsymsets_symbols[SYMP_SODASTRAW][5] = 1;
	thsymsets_symbols[SYMP_SPRING][3] = 1;
	thsymsets_symbols[SYMP_STALACTITE][5] = 1;
	thsymsets_symbols[SYMP_STALAGMITE][5] = 1;
	thsymsets_symbols[SYMP_STATION_FIXED][0] = 1;
	thsymsets_symbols[SYMP_STATION_NATURAL][0] = 1;
	thsymsets_symbols[SYMP_STATION_PAINTED][3] = 1;
	thsymsets_symbols[SYMP_STATION_TEMPORARY][0] = 1;
	thsymsets_symbols[SYMP_STATION_TEMPORARY][3] = 1;
	thsymsets_symbols[SYMP_STEPS][3] = 1;
	thsymsets_symbols[SYMP_TRAVERSE][3] = 1;
	thsymsets_symbols[SYMP_VEGETABLEDEBRIS][0] = 1;
	thsymsets_symbols[SYMP_WALLCALCITE][5] = 1;
	thsymsets_symbols[SYMP_WATER][5] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_INTERMITTENT][5] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_PALEO][5] = 1;
	thsymsets_symbols[SYMP_WATERFLOW_PERMANENT][5] = 1;
	thsymsets_symbols[SYML_ARROW][3] = 1;
	thsymsets_symbols[SYML_BORDER_PRESUMED][3] = 1;
	thsymsets_symbols[SYML_BORDER_TEMPORARY][3] = 1;
	thsymsets_symbols[SYML_BORDER_VISIBLE][3] = 1;
	thsymsets_symbols[SYML_CEILINGMEANDER][3] = 1;
	thsymsets_symbols[SYML_CEILINGSTEP][3] = 1;
	thsymsets_symbols[SYML_CHIMNEY][5] = 1;
	thsymsets_symbols[SYML_CONTOUR][3] = 1;
	thsymsets_symbols[SYML_CONTOUR][5] = 1;
	thsymsets_symbols[SYML_FLOORMEANDER][3] = 1;
	thsymsets_symbols[SYML_FLOORSTEP][5] = 1;
	thsymsets_symbols[SYML_FLOWSTONE][5] = 1;
	thsymsets_symbols[SYML_GRADIENT][1] = 1;
	thsymsets_symbols[SYML_GRADIENT][5] = 1;
	thsymsets_symbols[SYML_OVERHANG][3] = 1;
	thsymsets_symbols[SYML_PIT][5] = 1;
	thsymsets_symbols[SYML_ROCKBORDER][5] = 1;
	thsymsets_symbols[SYML_ROCKEDGE][5] = 1;
	thsymsets_symbols[SYML_ROPE][3] = 1;
	thsymsets_symbols[SYML_SECTION][3] = 1;
	thsymsets_symbols[SYML_SLOPE][1] = 1;
	thsymsets_symbols[SYML_SLOPE][3] = 1;
	thsymsets_symbols[SYML_SURVEY_CAVE][3] = 1;
	thsymsets_symbols[SYML_SURVEY_CAVE][5] = 1;
	thsymsets_symbols[SYML_SURVEY_SURFACE][3] = 1;
	thsymsets_symbols[SYML_WALL_BEDROCK][5] = 1;
	thsymsets_symbols[SYML_WALL_BLOCKS][3] = 1;
	thsymsets_symbols[SYML_WALL_CLAY][3] = 1;
	thsymsets_symbols[SYML_WALL_DEBRIS][3] = 1;
	thsymsets_symbols[SYML_WALL_ICE][3] = 1;
	thsymsets_symbols[SYML_WALL_PEBBLES][3] = 1;
	thsymsets_symbols[SYML_WALL_PRESUMED][5] = 1;
	thsymsets_symbols[SYML_WALL_SAND][3] = 1;
	thsymsets_symbols[SYML_WALL_UNDERLYING][5] = 1;
	thsymsets_symbols[SYML_WALL_UNSURVEYED][3] = 1;
	thsymsets_symbols[SYML_WATERFLOW_CONJECTURAL][3] = 1;
	thsymsets_symbols[SYML_WATERFLOW_INTERMITTENT][3] = 1;
	thsymsets_symbols[SYML_WATERFLOW_PERMANENT][5] = 1;
	thsymsets_symbols[SYMA_BEDROCK][3] = 1;
	thsymsets_symbols[SYMA_BLOCKS][3] = 1;
	thsymsets_symbols[SYMA_CLAY][3] = 1;
	thsymsets_symbols[SYMA_DEBRIS][3] = 1;
	thsymsets_symbols[SYMA_DEBRIS][5] = 1;
	thsymsets_symbols[SYMA_ICE][3] = 1;
	thsymsets_symbols[SYMA_PEBBLES][3] = 1;
	thsymsets_symbols[SYMA_SAND][5] = 1;
	thsymsets_symbols[SYMA_SNOW][3] = 1;
	thsymsets_symbols[SYMA_SUMP][5] = 1;
	thsymsets_symbols[SYMA_WATER][5] = 1;
}
