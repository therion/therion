/**
 * @file thdb2dxs.cxx
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
 
#include "thsymbolset.h"
#include "thparse.h"
#include "thpoint.h"
#include "thline.h"
#include "tharea.h"
#include "thexception.h"

thsymbolset::thsymbolset()
{
  for(int i = 0; i < thsymbolset_size; i++) {
    this->assigned[i] = !(thsymbolset__hidden[i]);
    this->used[i] = false;
  }
}
  
const char * thsymbolset::get_mp_macro(int id)
{
  this->used[id] = true;
  return thsymbolset__mp[id];
}
  

enum {
  TT_SYMBOL_POINT,
  TT_SYMBOL_LINE,
  TT_SYMBOL_AREA,
  TT_SYMBOL_UNKNOWN,
};

static const thstok thtt_symbol_class[] = {
  {"area", TT_SYMBOL_AREA},
  {"line", TT_SYMBOL_LINE},
  {"point", TT_SYMBOL_POINT},
  {NULL, TT_SYMBOL_UNKNOWN}
};

#define c2(TH,MP) case TH: rv = MP; break;
#define cl3(TH,MP) case TH: \
  if (subtype != TT_LINE_SUBTYPE_UNKNOWN) break; \
  rv = MP; \
  break;
#define cp3(TH,MP) case TH: \
  if (subtype != TT_POINT_SUBTYPE_UNKNOWN) break; \
  rv = MP; \
  break;

int thsymbolset__get_id(char * symclass, char * symbol)
{
  int type, subtype, rv;
  char types[128], 
    subtypes[128];
  types[127] = 0;
  subtypes[127] = 0;
  int ci = 0, slen = strlen(symbol);
  while((ci < slen) && (symbol[ci] != ':')) ci++;
  if (symbol[ci] == ':') {
    strncpy(&(types[0]),symbol,ci);
    types[ci] = 0;
    strncpy(&(subtypes[0]),&(symbol[ci+1]),127);
  } else {
    strncpy(&(types[0]),symbol,127);
    subtypes[0] = 0;
  }
  rv = -1;
  switch (thmatch_token(symclass,thtt_symbol_class)) {
    case TT_SYMBOL_AREA:
      type = thmatch_token(types,thtt_area_types);
      if (strlen(subtypes) > 0) 
        break;
      switch (type) {
        c2(TT_AREA_TYPE_DEBRIS, SYMA_DEBRIS);
        c2(TT_AREA_TYPE_SAND, SYMA_SAND);
        c2(TT_AREA_TYPE_SUMP, SYMA_SUMP);
        c2(TT_AREA_TYPE_WATER, SYMA_WATER);
      }
      break;
    case TT_SYMBOL_LINE:
      type = thmatch_token(types,thtt_line_types);
      subtype = thmatch_token(subtypes,thtt_line_subtypes);
      switch (type) {
        case TT_LINE_TYPE_WALL:
          switch (subtype) {
            c2(TT_LINE_SUBTYPE_BEDROCK,SYML_WALL_BEDROCK)
            c2(TT_LINE_SUBTYPE_BLOCKS,SYML_WALL_BLOCKS)
            c2(TT_LINE_SUBTYPE_CLAY,SYML_WALL_CLAY)
            c2(TT_LINE_SUBTYPE_DEBRIS,SYML_WALL_DEBRIS)
            c2(TT_LINE_SUBTYPE_ICE,SYML_WALL_ICE)
            c2(TT_LINE_SUBTYPE_INVISIBLE,SYML_WALL_INVISIBLE)
            c2(TT_LINE_SUBTYPE_PEBBLES,SYML_WALL_PEBBLES)
            c2(TT_LINE_SUBTYPE_PRESUMED,SYML_WALL_PRESUMED)
            c2(TT_LINE_SUBTYPE_SAND,SYML_WALL_SAND)
            c2(TT_LINE_SUBTYPE_UNDERLYING,SYML_WALL_UNDERLYING)
            c2(TT_LINE_SUBTYPE_UNSURVEYED,SYML_WALL_UNSURVEYED)
          }
          break;
        case TT_LINE_TYPE_BORDER:
          switch (subtype) {
            c2(TT_LINE_SUBTYPE_TEMPORARY,SYML_BORDER_TEMPORARY)
            c2(TT_LINE_SUBTYPE_INVISIBLE,SYML_BORDER_INVISIBLE)
            c2(TT_LINE_SUBTYPE_VISIBLE,SYML_BORDER_VISIBLE)
          }
          break;
        cl3(TT_LINE_TYPE_ARROW,SYML_ARROW);
        cl3(TT_LINE_TYPE_CEILING_STEP,SYML_CEILINGSTEP);
        cl3(TT_LINE_TYPE_CHIMNEY,SYML_CHIMNEY);
        cl3(TT_LINE_TYPE_CONTOUR,SYML_CONTOUR);
        cl3(TT_LINE_TYPE_FLOOR_STEP,SYML_FLOORSTEP);
        cl3(TT_LINE_TYPE_FLOWSTONE,SYML_FLOWSTONE);
        cl3(TT_LINE_TYPE_LABEL,SYML_LABEL);
        cl3(TT_LINE_TYPE_OVERHANG,SYML_OVERHANG);
        cl3(TT_LINE_TYPE_PIT,SYML_PIT);
        cl3(TT_LINE_TYPE_ROCK_BORDER,SYML_ROCKBORDER);
        cl3(TT_LINE_TYPE_ROCK_EDGE,SYML_ROCKEDGE);
        cl3(TT_LINE_TYPE_SECTION,SYML_SECTION);
        cl3(TT_LINE_TYPE_SLOPE,SYML_SLOPE);
        cl3(TT_LINE_TYPE_SURVEY,SYML_SURVEY);
      }
      break;
    case TT_SYMBOL_POINT:
      type = thmatch_token(types,thtt_point_types);
      subtype = thmatch_token(subtypes,thtt_point_subtypes);
      switch (type) {
        case TT_POINT_TYPE_WATER_FLOW:
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_INTERMITTENT,SYMP_WATERFLOW_INTERMITTENT)
            c2(TT_POINT_SUBTYPE_PERMANENT,SYMP_WATERFLOW_PERMANENT)
            c2(TT_POINT_SUBTYPE_PALEO,SYMP_WATERFLOW_PALEO)
          }
          break;
        case TT_POINT_TYPE_STATION:
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_FIXED,SYMP_STATION_FIXED)
            c2(TT_POINT_SUBTYPE_PAINTED,SYMP_STATION_PAINTED)
            c2(TT_POINT_SUBTYPE_NATURAL,SYMP_STATION_NATURAL)
            c2(TT_POINT_SUBTYPE_TEMP,SYMP_STATION_TEMPORARY)
          }
          break;
        cp3(TT_POINT_TYPE_AIR_DRAUGHT,SYMP_AIRDRAUGHT);
        cp3(TT_POINT_TYPE_ALTITUDE,SYMP_ALTITUDE);
        cp3(TT_POINT_TYPE_ANASTOMOSIS,SYMP_ANASTOMOSIS);
        cp3(TT_POINT_TYPE_ANCHOR,SYMP_ANCHOR);
        cp3(TT_POINT_TYPE_ARAGONITE,SYMP_ARAGONITE);
        cp3(TT_POINT_TYPE_ARCHEO_MATERIAL,SYMP_ARCHEOMATERIAL);
        cp3(TT_POINT_TYPE_BEDROCK,SYMP_BEDROCK);
        cp3(TT_POINT_TYPE_BLOCKS,SYMP_BLOCKS);
        cp3(TT_POINT_TYPE_BREAKDOWN_CHOKE,SYMP_BREAKDOWNCHOKE);
        cp3(TT_POINT_TYPE_BRIDGE,SYMP_BRIDGE);
//        cp3(TT_POINT_TYPE_CAMP,SYMP_CAVE);
        cp3(TT_POINT_TYPE_CAVE_PEARL,SYMP_CAVEPEARL);
        cp3(TT_POINT_TYPE_CLAY,SYMP_CLAY);
        cp3(TT_POINT_TYPE_CONTINUATION,SYMP_CONTINUATION);
        cp3(TT_POINT_TYPE_CRYSTAL,SYMP_CRYSTAL);
        cp3(TT_POINT_TYPE_CURTAIN,SYMP_CURTAIN);
        cp3(TT_POINT_TYPE_DATE,SYMP_DATE);
        cp3(TT_POINT_TYPE_DEBRIS,SYMP_DEBRIS);
        cp3(TT_POINT_TYPE_DISK,SYMP_DISK);
        cp3(TT_POINT_TYPE_ENTRANCE,SYMP_ENTRANCE);
        cp3(TT_POINT_TYPE_FIXED_LADDER,SYMP_FIXEDLADDER);
        cp3(TT_POINT_TYPE_FLOWSTONE,SYMP_FLOWSTONE);
        cp3(TT_POINT_TYPE_FLOWSTONE_CHOKE,SYMP_FLOWSTONECHOKE);
        cp3(TT_POINT_TYPE_FLUTE,SYMP_FLUTE);
        cp3(TT_POINT_TYPE_GRADIENT,SYMP_GRADIENT);
        cp3(TT_POINT_TYPE_GUANO,SYMP_GUANO);
        cp3(TT_POINT_TYPE_GYPSUM,SYMP_GYPSUM);
        cp3(TT_POINT_TYPE_GYPSUM_FLOWER,SYMP_GYPSUMFLOWER);
        cp3(TT_POINT_TYPE_HEIGHT,SYMP_HEIGHT);
        cp3(TT_POINT_TYPE_HELICTITE,SYMP_HELICTITE);
        cp3(TT_POINT_TYPE_ICE,SYMP_ICE);
        cp3(TT_POINT_TYPE_KARREN,SYMP_KARREN);
        cp3(TT_POINT_TYPE_LABEL,SYMP_LABEL);
        cp3(TT_POINT_TYPE_LOW_END,SYMP_LOWEND);
        cp3(TT_POINT_TYPE_MOONMILK,SYMP_MOONMILK);
        cp3(TT_POINT_TYPE_NARROW_END,SYMP_NARROWEND);
        cp3(TT_POINT_TYPE_NO_EQUIPMENT,SYMP_NOEQUIPMENT);
        cp3(TT_POINT_TYPE_PALEO_MATERIAL,SYMP_PALEOMATERIAL);
        cp3(TT_POINT_TYPE_PASSAGE_HEIGHT,SYMP_PASSAGEHEIGHT);
        cp3(TT_POINT_TYPE_PEBBLES,SYMP_PEBBLES);
        cp3(TT_POINT_TYPE_PILLAR,SYMP_PILLAR);
        cp3(TT_POINT_TYPE_POPCORN,SYMP_POPCORN);
        cp3(TT_POINT_TYPE_RAFT,SYMP_RAFT);
        cp3(TT_POINT_TYPE_RAFT_CONE,SYMP_RAFTCONE);
        cp3(TT_POINT_TYPE_REMARK,SYMP_REMARK);
        cp3(TT_POINT_TYPE_RIMSTONE_DAM,SYMP_RIMSTONEDAM);
        cp3(TT_POINT_TYPE_RIMSTONE_POOL,SYMP_RIMSTONEPOOL);
        cp3(TT_POINT_TYPE_ROOT,SYMP_ROOT);
        cp3(TT_POINT_TYPE_ROPE,SYMP_ROPE);
        cp3(TT_POINT_TYPE_ROPE_LADDER,SYMP_ROPELADDER);
        cp3(TT_POINT_TYPE_SAND,SYMP_SAND);
        cp3(TT_POINT_TYPE_SCALLOP,SYMP_SCALLOP);
        cp3(TT_POINT_TYPE_SINK,SYMP_SINK);
        cp3(TT_POINT_TYPE_SODA_STRAW,SYMP_SODASTRAW);
        cp3(TT_POINT_TYPE_SPRING,SYMP_SPRING);
        cp3(TT_POINT_TYPE_STALACTITE,SYMP_STALACTITE);
        cp3(TT_POINT_TYPE_STALAGMITE,SYMP_STALAGMITE);
        cp3(TT_POINT_TYPE_STATION_NAME,SYMP_STATIONNAME);
        cp3(TT_POINT_TYPE_STEPS,SYMP_STEPS);
        cp3(TT_POINT_TYPE_TRAVERSE,SYMP_TRAVERSE);
        cp3(TT_POINT_TYPE_VEGETABLE_DEBRIS,SYMP_VEGETABLEDEBRIS);
        cp3(TT_POINT_TYPE_WALL_CALCITE,SYMP_WALLCALCITE);
        cp3(TT_POINT_TYPE_WATER,SYMP_WATER);
      }
      break;
  }
  return rv;
}

void thsymbolset::export_symbol_assign(FILE * mpf, int sym_id, char * symset) 
{
  if (thsymbolset__assign[sym_id]) {
    this->assigned[sym_id] = true;
    fprintf(mpf,"mapsymbol(\"%s\",\"%s\",true);\n",thsymbolset__mp[sym_id],symset);
  }
}

void thsymbolset::export_symbol_defaults(FILE * mpf, char * symset) 
{
  fprintf(mpf,"\n\n\n%% %s symbol set.\n",symset);
  for(int id = 0; id < thsymbolset_size; id++) {
    if (thsymbolset__assign[id]) {
      this->assigned[id] = true;
      fprintf(mpf,"mapsymbol(\"%s\",\"%s\",false);\n",thsymbolset__mp[id],symset);
    }
  }
}
  
void thsymbolset::export_symbol_hide(FILE * mpf, int sym_id) 
{
  if (thsymbolset__assign[sym_id]) {
    this->assigned[sym_id] = false;
    fprintf(mpf,"hidesymbol(\"%s\");\n",thsymbolset__mp[sym_id]);
  }
}


