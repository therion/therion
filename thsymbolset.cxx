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
#include "thpdfdata.h"
#include "thlayout.h"
#include "thtexfonts.h"
#include "thdate.h"
#include "thlang.h"
#include "thtmpdir.h"
#include "thcmdline.h"
#include "thmpost.h"
#include "thinit.h"
#include "thsymbolsets.h"
#include "thlogfile.h"
#include "thepsparse.h"
#ifndef THMSVC
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#define putenv _putenv
#define hypot _hypot
#define mkdir _mkdir
#endif


thsymbolset::thsymbolset()
{
  for(int i = 0; i < thsymbolset_size; i++) {
    this->assigned[i] = !(thsymbolset__hidden[i]);
    this->used[i] = false;
  }
  this->group_symbols = true;
}


void thsymbolset_log_log_file(const char * logfpath, const char * on_title, const char * off_title, bool mpbug = false) {
  char * lnbuff = new char [4097];
//  unsigned long lnum = 0;
  thlog.printf("%s",on_title);
  std::ifstream lf(logfpath);
  if (!(lf.is_open())) {{
    thwarning(("can't open %s file for input", logfpath));
    }
    thlog.printf("can't open %s file for input",logfpath);
    thlog.printf("%s",off_title);
    delete [] lnbuff;
    return;
  }
  // let's read line by line and print to log file
  bool skip_next = false, skip_this = false, peoln = false;
  while (!(lf.eof())) {
    lf.getline(lnbuff,4096);
    if (mpbug && (!skip_this)) {
      if (strncmp(lnbuff,"write",5) == 0) {
        skip_next = true;
        skip_this = true;
        peoln = false;
      }
    }
    if (!skip_this) {
      if (!skip_next) {
        thlog.printf("%s%s", (peoln ? "\n" : ""), lnbuff);
        peoln = true;
      } else {
        skip_next = false;
      }
    } else {
      skip_this = false;
    } 
  }
  if (peoln) 
    thlog.printf("\n");
  lf.close();
  delete [] lnbuff;
  thlog.printf("%s",off_title);
}



char * thlegend_u2string(unsigned u) {
  static char a [5];
  a[4] = 0;
  unsigned v = 1;
  for(int i = 3; i >= 0; i--) {
    a[i] = 'a' + ((u / v) % 10);
    v *= 10;
  }
  return (&(a[0]));
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
  TT_SYMBOL_GROUP,
  TT_SYMBOL_UNKNOWN,
  TT_SYMBOL_SPECIAL,
};

static const thstok thtt_symbol_class[] = {
  {"area", TT_SYMBOL_AREA},
  {"group", TT_SYMBOL_GROUP},
  {"line", TT_SYMBOL_LINE},
  {"point", TT_SYMBOL_POINT},
  {"special", TT_SYMBOL_SPECIAL},
  {NULL, TT_SYMBOL_UNKNOWN}
};


static const thstok thtt_symbol_group[] = {
  {"all", SYMX_ALL},
  {"cave-centerline", SYMX_CAVECENTERLINE},
  {"cave-centreline", SYMX_CAVECENTERLINE},
  {"centerline", SYMX_CENTERLINE},
  {"centreline", SYMX_CENTERLINE},
  {"equipment", SYMX_EQUIPMENT},
  {"ice", SYMX_ICE},
  {"passage-fills", SYMX_PASSAGEFILLS},
  {"sections", SYMX_SECTIONS},
  {"sediments", SYMX_SEDIMENTS},
  {"speleothems", SYMX_SPELEOTHEMS},
  {"surface-centerline", SYMX_SURFACECENTERLINE},
  {"surface-centreline", SYMX_SURFACECENTERLINE},
  {"text", SYMX_TEXT},
  {"water", SYMX_WATER},
  {NULL, SYMX_}
};


static const thstok thtt_symbol_point_spec[] = {
  {"cave-station", SYMP_CAVESTATION},
  {"flag", SYMX_POINT_FLAG},
  {"flag:air-draught", SYMP_FLAG_AIRDRAUGHT},
  {"flag:arch", SYMP_FLAG_ARCH},
  {"flag:continuation", SYMP_FLAG_CONTINUATION},
  {"flag:dig", SYMP_FLAG_DIG},
  {"flag:doline", SYMP_FLAG_DOLINE},
  {"flag:entrance", SYMP_FLAG_ENTRANCE},
  {"flag:overhang", SYMP_FLAG_OVERHANG},
  {"flag:sink", SYMP_FLAG_SINK},
  {"flag:spring", SYMP_FLAG_SPRING},
  {"surface-station", SYMP_SURFACESTATION},
  {"wall-altitude", SYMP_WALLALTITUDE},
  {NULL, SYMX_}
};


static const thstok thtt_symbol_special[] = {
  {"north-arrow", SYMS_NORTHARROW},
  {"scale-bar", SYMS_SCALEBAR},
  {NULL, SYMX_}
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
#define sgroup(MP) if (strlen(subtypes) == 0) { \
            rv = MP; \
            break; \
          }

int thsymbolset__get_id(const char * symclass, const char * symbol)
{
  int type, subtype, rv;
  char types[128], 
    subtypes[128];
  types[127] = 0;
  subtypes[127] = 0;
  int ci = 0, slen = (int)strlen(symbol);
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
    case TT_SYMBOL_GROUP:
      type = thmatch_token(types,thtt_symbol_group);
      if (strlen(subtypes) > 0) 
        break;
      if (type != SYMX_)
        rv = type;
      break;
    case TT_SYMBOL_SPECIAL:
      type = thmatch_token(types,thtt_symbol_special);
      if (strlen(subtypes) > 0) 
        break;
      if (type != SYMX_)
        rv = type;
      break;
    case TT_SYMBOL_AREA:
      type = thmatch_token(types,thtt_area_types);
      if (strlen(subtypes) > 0) 
        break;
      switch (type) {
        c2(TT_AREA_TYPE_DEBRIS, SYMA_DEBRIS);
        c2(TT_AREA_TYPE_SAND, SYMA_SAND);
        c2(TT_AREA_TYPE_SUMP, SYMA_SUMP);
        c2(TT_AREA_TYPE_WATER, SYMA_WATER);
        c2(TT_AREA_TYPE_ICE, SYMA_ICE);
        c2(TT_AREA_TYPE_SNOW, SYMA_SNOW);
        c2(TT_AREA_TYPE_BLOCKS, SYMA_BLOCKS);
        c2(TT_AREA_TYPE_CLAY, SYMA_CLAY);
        c2(TT_AREA_TYPE_PEBBLES, SYMA_PEBBLES);
        c2(TT_AREA_TYPE_DIMENSIONS, SYMA_DIMENSIONS);
        c2(TT_AREA_TYPE_FLOWSTONE, SYMA_FLOWSTONE);
        c2(TT_AREA_TYPE_MOONMILK, SYMA_MOONMILK);
      }
      break;
    case TT_SYMBOL_LINE:
      type = thmatch_token(types,thtt_line_types);
      subtype = thmatch_token(subtypes,thtt_line_subtypes);
      switch (type) {
        case TT_LINE_TYPE_WALL:
          sgroup(SYMX_LINE_WALL)
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
            c2(TT_LINE_SUBTYPE_FLOWSTONE,SYML_WALL_FLOWSTONE)
            c2(TT_LINE_SUBTYPE_MOONMILK,SYML_WALL_MOONMILK)
			c2(TT_LINE_SUBTYPE_PIT,SYML_WALL_PIT)
            c2(TT_LINE_SUBTYPE_OVERLYING,SYML_WALL_OVERLYING)

          }
          break;
        case TT_LINE_TYPE_BORDER:
          sgroup(SYMX_LINE_BORDER)
          switch (subtype) {
            c2(TT_LINE_SUBTYPE_TEMPORARY,SYML_BORDER_TEMPORARY)
            c2(TT_LINE_SUBTYPE_INVISIBLE,SYML_BORDER_INVISIBLE)
            c2(TT_LINE_SUBTYPE_VISIBLE,SYML_BORDER_VISIBLE)
          }
          break;
        case TT_LINE_TYPE_SURVEY:
          sgroup(SYMX_LINE_SURVEY)
          switch (subtype) {
            c2(TT_LINE_SUBTYPE_CAVE,SYML_SURVEY_CAVE)
            c2(TT_LINE_SUBTYPE_SURFACE,SYML_SURVEY_SURFACE)
          }
          break;
        case TT_LINE_TYPE_WATER_FLOW:
          sgroup(SYMX_LINE_WATERFLOW)
          switch (subtype) {
            c2(TT_LINE_SUBTYPE_PERMANENT,SYML_WATERFLOW_PERMANENT)
            c2(TT_LINE_SUBTYPE_INTERMITTENT,SYML_WATERFLOW_INTERMITTENT)
            c2(TT_LINE_SUBTYPE_CONJECTURAL,SYML_WATERFLOW_CONJECTURAL)
          }
          break;
        cl3(TT_LINE_TYPE_ARROW,SYML_ARROW);
        cl3(TT_LINE_TYPE_GRADIENT,SYML_GRADIENT);
        cl3(TT_LINE_TYPE_CEILING_STEP,SYML_CEILINGSTEP);
        cl3(TT_LINE_TYPE_CEILING_MEANDER,SYML_CEILINGMEANDER);
        cl3(TT_LINE_TYPE_CHIMNEY,SYML_CHIMNEY);
        cl3(TT_LINE_TYPE_CONTOUR,SYML_CONTOUR);
        cl3(TT_LINE_TYPE_FLOOR_STEP,SYML_FLOORSTEP);
        cl3(TT_LINE_TYPE_FLOOR_MEANDER,SYML_FLOORMEANDER);
        cl3(TT_LINE_TYPE_FLOWSTONE,SYML_FLOWSTONE);
        cl3(TT_LINE_TYPE_MOONMILK,SYML_MOONMILK);
        cl3(TT_LINE_TYPE_LABEL,SYML_LABEL);
        cl3(TT_LINE_TYPE_MAP_CONNECTION,SYML_MAPCONNECTION);
        cl3(TT_LINE_TYPE_OVERHANG,SYML_OVERHANG);
        cl3(TT_LINE_TYPE_PIT,SYML_PIT);
        cl3(TT_LINE_TYPE_ROCK_BORDER,SYML_ROCKBORDER);
        cl3(TT_LINE_TYPE_ROCK_EDGE,SYML_ROCKEDGE);
        cl3(TT_LINE_TYPE_SECTION,SYML_SECTION);
        cl3(TT_LINE_TYPE_SLOPE,SYML_SLOPE);
        cl3(TT_LINE_TYPE_HANDRAIL,SYML_HANDRAIL);
        cl3(TT_LINE_TYPE_STEPS,SYML_STEPS);
        cl3(TT_LINE_TYPE_ROPE,SYML_ROPE);
        cl3(TT_LINE_TYPE_ROPE_LADDER,SYML_ROPELADDER);
        cl3(TT_LINE_TYPE_FIXED_LADDER,SYML_FIXEDLADDER);
        cl3(TT_LINE_TYPE_VIA_FERRATA,SYML_VIAFERRATA);
      }
      break;
    case TT_SYMBOL_POINT:
      type = thmatch_token(types,thtt_point_types);
      subtype = thmatch_token(subtypes,thtt_point_subtypes);
      if ((type == TT_POINT_TYPE_UNKNOWN) && (strlen(subtypes) == 0)) {
        type = thmatch_token(types,thtt_symbol_point_spec);
        if (type != SYMX_)
          rv = type;
        type = TT_POINT_TYPE_UNKNOWN;
      }
      if ((type == TT_POINT_TYPE_UNKNOWN) && (strlen(subtypes) > 0)) {
        type = thmatch_token(symbol,thtt_symbol_point_spec);
        if (type != SYMX_)
            rv = type;
        type = TT_POINT_TYPE_UNKNOWN;
      }
      switch (type) {
        case TT_POINT_TYPE_PASSAGE_HEIGHT:
          sgroup(SYMX_POINT_PASSAGEHEIGHT)
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_POSITIVE,SYMP_PASSAGEHEIGHT_POSITIVE);
            c2(TT_POINT_SUBTYPE_NEGATIVE,SYMP_PASSAGEHEIGHT_NEGATIVE);
            c2(TT_POINT_SUBTYPE_BOTH,SYMP_PASSAGEHEIGHT_BOTH);
            c2(TT_POINT_SUBTYPE_UNSIGNED,SYMP_PASSAGEHEIGHT_UNSIGNED);
          }
          break;
        case TT_POINT_TYPE_HEIGHT:
          sgroup(SYMX_POINT_HEIGHT)
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_POSITIVE,SYMP_HEIGHT_POSITIVE);
            c2(TT_POINT_SUBTYPE_NEGATIVE,SYMP_HEIGHT_NEGATIVE);
            c2(TT_POINT_SUBTYPE_UNSIGNED,SYMP_HEIGHT_UNSIGNED);
          }
          break;
        case TT_POINT_TYPE_WATER_FLOW:
          sgroup(SYMX_POINT_WATERFLOW)
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_INTERMITTENT,SYMP_WATERFLOW_INTERMITTENT)
            c2(TT_POINT_SUBTYPE_PERMANENT,SYMP_WATERFLOW_PERMANENT)
            c2(TT_POINT_SUBTYPE_PALEO,SYMP_WATERFLOW_PALEO)
          }
          break;
        case TT_POINT_TYPE_STATION:
          sgroup(SYMX_POINT_STATION)
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_FIXED,SYMP_STATION_FIXED)
            c2(TT_POINT_SUBTYPE_PAINTED,SYMP_STATION_PAINTED)
            c2(TT_POINT_SUBTYPE_NATURAL,SYMP_STATION_NATURAL)
            c2(TT_POINT_SUBTYPE_TEMP,SYMP_STATION_TEMPORARY)
          }
          break;
        case TT_POINT_TYPE_AIR_DRAUGHT:
          sgroup(SYMX_POINT_AIRDRAUGHT)
          switch (subtype) {
            c2(TT_POINT_SUBTYPE_UNDEF,SYMP_AIRDRAUGHT)
            c2(TT_POINT_SUBTYPE_SUMMER,SYMP_AIRDRAUGHT_SUMMER)
            c2(TT_POINT_SUBTYPE_WINTER,SYMP_AIRDRAUGHT_WINTER)
          }
          break;
        cp3(TT_POINT_TYPE_ALTITUDE,SYMP_ALTITUDE);
        cp3(TT_POINT_TYPE_ANASTOMOSIS,SYMP_ANASTOMOSIS);
        cp3(TT_POINT_TYPE_ANCHOR,SYMP_ANCHOR);
        cp3(TT_POINT_TYPE_ARAGONITE,SYMP_ARAGONITE);
        cp3(TT_POINT_TYPE_ARCHEO_MATERIAL,SYMP_ARCHEOMATERIAL);
        cp3(TT_POINT_TYPE_BEDROCK,SYMP_BEDROCK);
        cp3(TT_POINT_TYPE_BLOCKS,SYMP_BLOCKS);
        cp3(TT_POINT_TYPE_BREAKDOWN_CHOKE,SYMP_BREAKDOWNCHOKE);
        cp3(TT_POINT_TYPE_BRIDGE,SYMP_BRIDGE);
        cp3(TT_POINT_TYPE_CAMP,SYMP_CAMP);
        cp3(TT_POINT_TYPE_CAVE_PEARL,SYMP_CAVEPEARL);
        cp3(TT_POINT_TYPE_CLAY,SYMP_CLAY);
        cp3(TT_POINT_TYPE_CONTINUATION,SYMP_CONTINUATION);
        cp3(TT_POINT_TYPE_CRYSTAL,SYMP_CRYSTAL);
        cp3(TT_POINT_TYPE_CURTAIN,SYMP_CURTAIN);
        cp3(TT_POINT_TYPE_DATE,SYMP_DATE);
        cp3(TT_POINT_TYPE_DEBRIS,SYMP_DEBRIS);
        cp3(TT_POINT_TYPE_DIG,SYMP_DIG);
        cp3(TT_POINT_TYPE_DISK,SYMP_DISK);
        cp3(TT_POINT_TYPE_ENTRANCE,SYMP_ENTRANCE);
        cp3(TT_POINT_TYPE_FIXED_LADDER,SYMP_FIXEDLADDER);
        cp3(TT_POINT_TYPE_FLOWSTONE,SYMP_FLOWSTONE);
        cp3(TT_POINT_TYPE_FLOWSTONE_CHOKE,SYMP_FLOWSTONECHOKE);
        cp3(TT_POINT_TYPE_CLAY_CHOKE,SYMP_CLAYCHOKE);
        cp3(TT_POINT_TYPE_FLUTE,SYMP_FLUTE);
        cp3(TT_POINT_TYPE_GRADIENT,SYMP_GRADIENT);
        cp3(TT_POINT_TYPE_GUANO,SYMP_GUANO);
        cp3(TT_POINT_TYPE_ALTAR,SYMP_ALTAR);
        cp3(TT_POINT_TYPE_DANGER,SYMP_DANGER);
        cp3(TT_POINT_TYPE_ELECTRICLIGHT,SYMP_ELECTRICLIGHT);
        cp3(TT_POINT_TYPE_EXVOTO,SYMP_EXVOTO);
        cp3(TT_POINT_TYPE_GATE,SYMP_GATE);
        cp3(TT_POINT_TYPE_HUMANBONE,SYMP_HUMANBONE);
        cp3(TT_POINT_TYPE_MASONRY,SYMP_MASONRY);
        cp3(TT_POINT_TYPE_MINUS,SYMP_MINUS);
        cp3(TT_POINT_TYPE_MUD,SYMP_MUD);
        cp3(TT_POINT_TYPE_MUDCRACK,SYMP_MUDCRACK);
        cp3(TT_POINT_TYPE_NAMEPLATE,SYMP_NAMEPLATE);
        cp3(TT_POINT_TYPE_NOTWHEELCHAIR,SYMP_NOTWHEELCHAIR);
        cp3(TT_POINT_TYPE_PLUS,SYMP_PLUS);
        cp3(TT_POINT_TYPE_PLUSMINUS,SYMP_PLUSMINUS);
        cp3(TT_POINT_TYPE_WALKWAY,SYMP_WALKWAY);
        cp3(TT_POINT_TYPE_WATERDRIP,SYMP_WATERDRIP);
        cp3(TT_POINT_TYPE_WHEELCHAIR,SYMP_WHEELCHAIR);
        cp3(TT_POINT_TYPE_GYPSUM,SYMP_GYPSUM);
        cp3(TT_POINT_TYPE_GYPSUM_FLOWER,SYMP_GYPSUMFLOWER);
        cp3(TT_POINT_TYPE_HELICTITE,SYMP_HELICTITE);
        cp3(TT_POINT_TYPE_ICE,SYMP_ICE);
        cp3(TT_POINT_TYPE_SNOW,SYMP_SNOW);
        cp3(TT_POINT_TYPE_KARREN,SYMP_KARREN);
        cp3(TT_POINT_TYPE_LABEL,SYMP_LABEL);
        cp3(TT_POINT_TYPE_LOW_END,SYMP_LOWEND);
        cp3(TT_POINT_TYPE_MOONMILK,SYMP_MOONMILK);
        cp3(TT_POINT_TYPE_NARROW_END,SYMP_NARROWEND);
        cp3(TT_POINT_TYPE_NO_EQUIPMENT,SYMP_NOEQUIPMENT);
        cp3(TT_POINT_TYPE_PALEO_MATERIAL,SYMP_PALEOMATERIAL);
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
        cp3(TT_POINT_TYPE_SECTION,SYMP_SECTION);
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
        cp3(TT_POINT_TYPE_ICE_STALACTITE,SYMP_ICESTALACTITE);
        cp3(TT_POINT_TYPE_ICE_STALAGMITE,SYMP_ICESTALAGMITE);
        cp3(TT_POINT_TYPE_ICE_PILLAR,SYMP_ICEPILLAR);
        cp3(TT_POINT_TYPE_HANDRAIL,SYMP_HANDRAIL);
        cp3(TT_POINT_TYPE_VIA_FERRATA,SYMP_VIAFERRATA);
        cp3(TT_POINT_TYPE_STALACTITES,SYMP_STALACTITES);
        cp3(TT_POINT_TYPE_STALAGMITES,SYMP_STALAGMITES);
        cp3(TT_POINT_TYPE_PILLARS,SYMP_PILLARS);
      }
      break;
  }
  return rv;
}

void thsymbolset::export_symbol_defaults(FILE * mpf, const char * symset) 
{
  fprintf(mpf,"\n\n\n%% %s symbol set.\n",symset);
  for(int id = 0; id < SYMX_; id++) {
    if (thsymbolset__assign[id]) {
      this->assigned[id] = true;
      fprintf(mpf,"mapsymbol(\"%s\",\"%s\",false);\n",thsymbolset__mp[id],symset);
    }
  }
}
  
void thsymbolset::export_symbol_assign(FILE * mpf, int sym_id, const char * symset) 
{
  if (sym_id > SYMX_)
    export_symbol_assign_group(mpf, sym_id, symset);
  else if (thsymbolset__assign[sym_id]) {
    fprintf(mpf,"mapsymbol(\"%s\",\"%s\",true);\n",thsymbolset__mp[sym_id],symset);
  }
}

void thsymbolset::export_symbol_hide(FILE * mpf, int sym_id) 
{
  if (sym_id > SYMX_)
    export_symbol_hide_group(mpf, sym_id);
  else   
    this->assigned[sym_id] = false;
}


void thsymbolset::export_symbol_show(FILE * mpf, int sym_id) 
{
  if (sym_id > SYMX_)
    export_symbol_show_group(mpf, sym_id);
  else
    this->assigned[sym_id] = true;
}


void thsymbolset::export_symbol_color(FILE * mpf, int sym_id, thlayout_color * clr) 
{
  if (sym_id > SYMX_)
    export_symbol_color_group(mpf, sym_id, clr);
  else {
    this->color[sym_id] = *clr;
    this->color[sym_id].defined = true;
  }
}

void thsymbolset::export_symbol_color_group(FILE * mpf, int sym_id, thlayout_color * clr) 
{
  int id = 0;
  int cid = thsymbolset__get_group(sym_id,id++);
  while (cid >= 0) {
    this->export_symbol_color(mpf, cid, clr);
    cid = thsymbolset__get_group(sym_id,id++);
  }
}


void thsymbolset::export_symbol_assign_group(FILE * mpf, int sym_id, const char * symset) 
{
  int id = 0;
  int cid = thsymbolset__get_group(sym_id,id++);
  while (cid >= 0) {
    this->export_symbol_assign(mpf, cid, symset);
    cid = thsymbolset__get_group(sym_id,id++);
  }
}

void thsymbolset::export_symbol_hide_group(FILE * mpf, int sym_id) 
{
  int id = 0;
  int cid = thsymbolset__get_group(sym_id,id++);
  while (cid >= 0) {
    this->export_symbol_hide(mpf, cid);
    cid = thsymbolset__get_group(sym_id,id++);
  }
}


void thsymbolset::export_symbol_show_group(FILE * mpf, int sym_id) 
{
  int id = 0;
  int cid = thsymbolset__get_group(sym_id,id++);
  while (cid >= 0) {
    this->export_symbol_show(mpf, cid);
    cid = thsymbolset__get_group(sym_id,id++);
  }
}


#define bgroup(gid) case gid: \
  switch (cid) {
#define group(id,mid) case id: rv = mid; break;
#define egroup } break;

int thsymbolset__get_group(int group_id, int cid) {
  int rv = -1;
  switch (group_id) {

    case SYMX_ALL:
      if (cid < SYMS_) rv = cid;
      break;
    
    bgroup(SYMX_SECTIONS)
    group(0,SYMP_SECTION)
    group(1,SYML_SECTION)
    egroup  

    bgroup(SYMX_CENTERLINE)
    group(0,SYMX_POINT_STATION)
    group(1,SYML_SURVEY_SURFACE)
    group(2,SYML_SURVEY_CAVE)
    group(3,SYMP_STATIONNAME)
    egroup  


    bgroup(SYMX_TEXT)
    group(0,SYMP_LABEL)
    group(1,SYMP_REMARK)
    group(2,SYMP_DATE)
    group(3,SYML_LABEL)
    group(4,SYMP_STATIONNAME)
    group(5,SYMP_CONTINUATION)
    group(6,SYMP_FLAG_CONTINUATION)
    group(7,SYMP_WALLALTITUDE)
    egroup


    bgroup(SYMX_WATER)
    group(0,SYMX_LINE_WATERFLOW)
    group(1,SYMX_POINT_WATERFLOW)
    group(2,SYMP_WATER)
    group(3,SYMA_WATER)
    group(4,SYMA_SUMP)
    group(5,SYMP_SPRING)
    group(6,SYMP_SINK)
    egroup  

    bgroup(SYMX_EQUIPMENT)
    group( 0,SYMP_ANCHOR);
    group( 1,SYMP_ROPE);
    group( 2,SYMP_FIXEDLADDER);
    group( 3,SYMP_ROPELADDER);
    group( 4,SYMP_STEPS);
    group( 5,SYMP_BRIDGE);
    group( 6,SYMP_TRAVERSE);
    group( 7,SYMP_CAMP);
    group( 8,SYMP_NOEQUIPMENT);
    group( 9,SYML_ROPE);
    group(10,SYML_HANDRAIL);
    group(11,SYMP_HANDRAIL);
    group(12,SYML_FIXEDLADDER);
    group(13,SYML_ROPELADDER);
    group(14,SYMP_VIAFERRATA);
    group(15,SYML_VIAFERRATA);
    group(16,SYML_STEPS);
    group(17,SYMP_NOTWHEELCHAIR);
    group(18,SYMP_WHEELCHAIR);
    group(19,SYMP_WALKWAY);
    group(20,SYMP_NAMEPLATE);
    group(21,SYMP_GATE);
    group(22,SYMP_ELECTRICLIGHT);
    egroup

    bgroup(SYMX_SPELEOTHEMS)
    group( 0,SYMP_FLOWSTONE);
    group( 1,SYMP_MOONMILK);
    group( 2,SYMP_STALACTITE);
    group( 3,SYMP_STALAGMITE);
    group( 4,SYMP_PILLAR);
    group( 5,SYMP_CURTAIN);
    group( 6,SYMP_HELICTITE);
    group( 7,SYMP_SODASTRAW);
    group( 8,SYMP_CRYSTAL);
    group( 9,SYMP_WALLCALCITE);
    group(10,SYMP_POPCORN);
    group(11,SYMP_DISK);
    group(12,SYMP_GYPSUM);
    group(13,SYMP_GYPSUMFLOWER);
    group(14,SYMP_ARAGONITE);
    group(15,SYMP_CAVEPEARL);
    group(16,SYMP_RIMSTONEPOOL);
    group(17,SYMP_RIMSTONEDAM);
    group(19,SYML_FLOWSTONE);
    group(20,SYML_MOONMILK);
    group(21,SYMA_FLOWSTONE)
    group(22,SYMA_MOONMILK)
    egroup


    bgroup(SYMX_ICE)
    group(0,SYML_WALL_ICE);
    group(1,SYMP_ICE);
    group(2,SYMP_ICESTALACTITE);
    group(3,SYMP_ICESTALAGMITE);
    group(4,SYMP_ICEPILLAR);
    group(5,SYMP_SNOW);
    group(6,SYMA_ICE);
    group(7,SYMA_SNOW);
    egroup


    bgroup(SYMX_SEDIMENTS)
    group(0,SYML_WALL_SAND);
    group(1,SYML_WALL_CLAY);
    group(2,SYMP_SAND);
    group(3,SYMP_CLAY);
    group(4,SYMA_SAND);
    group(5,SYMA_CLAY);
    group(6,SYMP_CLAYTREE);
    group(7,SYMP_RAFT);
    group(8,SYMP_RAFTCONE);
    group(9,SYMP_GUANO);
    group(10,SYMP_MUDCRACK);
    group(11,SYMP_MUD);
    egroup


    bgroup(SYMX_PASSAGEFILLS)
    group(0,SYMX_POINT_WATERFLOW)
    group(1,SYMP_BEDROCK);
    group(2,SYMP_SAND);
    group(3,SYMP_RAFT);
    group(4,SYMP_CLAY);
    group(5,SYMP_PEBBLES);
    group(6,SYMP_DEBRIS);
    group(7,SYMP_BLOCKS);
    group(8,SYMP_WATER);
    group(9,SYMP_ICE);
    group(10,SYMP_GUANO);
    group(11,SYMP_SNOW);
    group(12,SYML_ROCKBORDER);
    group(13,SYML_ROCKEDGE);
    group(14,SYMX_LINE_WATERFLOW)
    group(15,SYMA_WATER)
    group(16,SYMA_SUMP)
    group(17,SYMA_BEDROCK)
    group(18,SYMA_BLOCKS)
    group(19,SYMA_CLAY)
    group(20,SYMA_DEBRIS)
    group(21,SYMA_ICE)
    group(22,SYMA_PEBBLES)
    group(23,SYMA_SAND)
    group(24,SYMA_SNOW)
    group(25,SYMP_MUDCRACK)
    group(26,SYMP_MUD)
    egroup

    bgroup(SYMX_SURFACECENTERLINE)
    group(0,SYMP_SURFACESTATION)
    group(1,SYML_SURVEY_SURFACE)
    egroup  

    bgroup(SYMX_POINT_FLAG)
    group(0,SYMP_FLAG_ENTRANCE)
    group(1,SYMP_FLAG_CONTINUATION)
    group(2,SYMP_FLAG_SINK)
    group(3,SYMP_FLAG_SPRING)
    group(4,SYMP_FLAG_DOLINE)
    group(5,SYMP_FLAG_DIG)
    group(6,SYMP_FLAG_AIRDRAUGHT)
    group(7,SYMP_FLAG_ARCH)
    group(8,SYMP_FLAG_OVERHANG)
    egroup  

    bgroup(SYMX_CAVECENTERLINE)
    group(0,SYMP_CAVESTATION)
    group(1,SYML_SURVEY_CAVE)
    egroup  

    bgroup(SYMX_LINE_WALL)
    group(0,SYML_WALL_BEDROCK)
    group(1,SYML_WALL_BLOCKS)
    group(2,SYML_WALL_CLAY)
    group(3,SYML_WALL_DEBRIS)
    group(4,SYML_WALL_ICE)
    group(5,SYML_WALL_PEBBLES)
    group(6,SYML_WALL_PRESUMED)
    group(7,SYML_WALL_SAND)
    group(8,SYML_WALL_UNDERLYING)
    group(9,SYML_WALL_UNSURVEYED)
    group(10,SYML_WALL_PIT)
    group(11,SYML_WALL_OVERLYING)
    group(12,SYML_WALL_MOONMILK)
    group(13,SYML_WALL_FLOWSTONE)
    egroup  

    bgroup(SYMX_LINE_WATERFLOW)
    group(0,SYML_WATERFLOW_CONJECTURAL)
    group(1,SYML_WATERFLOW_INTERMITTENT)
    group(2,SYML_WATERFLOW_PERMANENT)
    egroup  
    
    bgroup(SYMX_LINE_BORDER)
    group(0,SYML_BORDER_TEMPORARY)
    group(1,SYML_BORDER_VISIBLE)
    egroup    
    
    bgroup(SYMX_LINE_SURVEY)
    group(0,SYML_SURVEY_CAVE)
    group(1,SYML_SURVEY_SURFACE)
    egroup    
    
    bgroup(SYMX_POINT_STATION)
    group(0,SYMP_STATION_FIXED)
    group(1,SYMP_STATION_NATURAL)
    group(2,SYMP_STATION_PAINTED)
    group(3,SYMP_STATION_TEMPORARY)
    group(4,SYMP_SURFACESTATION)
    group(5,SYMP_CAVESTATION)
    egroup

    bgroup(SYMX_POINT_AIRDRAUGHT)
    group(0,SYMP_AIRDRAUGHT)
    group(1,SYMP_AIRDRAUGHT_SUMMER)
    group(2,SYMP_AIRDRAUGHT_WINTER)
    egroup

    bgroup(SYMX_POINT_WATERFLOW)
    group(0,SYMP_WATERFLOW_INTERMITTENT)
    group(1,SYMP_WATERFLOW_PALEO)
    group(2,SYMP_WATERFLOW_PERMANENT)
    egroup

    bgroup(SYMX_POINT_PASSAGEHEIGHT)
    group(0,SYMP_PASSAGEHEIGHT_POSITIVE)
    group(1,SYMP_PASSAGEHEIGHT_NEGATIVE)
    group(2,SYMP_PASSAGEHEIGHT_BOTH)
    group(3,SYMP_PASSAGEHEIGHT_UNSIGNED)
    egroup
    
    bgroup(SYMX_POINT_HEIGHT)
    group(0,SYMP_HEIGHT_POSITIVE)
    group(1,SYMP_HEIGHT_NEGATIVE)
    group(2,SYMP_HEIGHT_UNSIGNED)
    egroup
    
    
  }
  return rv;
}



void thsymbolset::export_pdf(class thlayout * layout, FILE * mpf, unsigned & sfig) {

  list<legendrecord>::iterator LEGENDITEM;
  legendrecord dummlr;
  LEGENDLIST.clear();
  thbuffer texb;
  texb.guarantee(128);
  unsigned symn = 0;
  
  for(int i = 0; i < thsymbolset_size; i++)
	if (layout->legend == TT_LAYOUT_LEGEND_ALL) {
		this->export_symbol_show_group(mpf, SYMX_POINT_FLAG);
	}


// TODO:
//   + section point (rez), line (ciara), arrow (sipocka)
//   + wall:pebbles, wall:clay, wall:ice
//   + point: passase-height (p,n,pn,u), height (p,n,u), date, label, remark
//   + line: label

#define helpsymbol fprintf(mpf,"drawoptions(withcolor HelpSymbolColor);\n");
#define endhelpsymbol fprintf(mpf,"drawoptions();\n");

#define isused(mid) \
    (this->assigned[mid] && \
    (this->used[mid] || (layout->legend == TT_LAYOUT_LEGEND_ALL)))

// vlozi figure do metapostu
#define insfig(mid,txt) \
  if (((mid == 0) || (isused(mid))) && (strlen(txt) > 0)) {\
    LEGENDITEM = LEGENDLIST.insert(LEGENDLIST.end(),dummlr); \
    fprintf(mpf,"beginfig(%d);\n",sfig); \
    fprintf(mpf,"clean_legend_box;\n"); \
    sprintf(texb.get_buffer(),"data.%d",sfig); \
    LEGENDITEM->idfig = (unsigned) sfig; \
    LEGENDITEM->idsym = (unsigned) mid; \
    LEGENDITEM->idnum = (unsigned) symn; \
    LEGENDITEM->fname = texb.get_buffer(); \
    LEGENDITEM->name = thlegend_u2string(unsigned(symn++)); \
    LEGENDITEM->descr = txt; \
    sfig++;
  
#define endfig \
    fprintf(mpf,"draw_legend_box;\nendfig;\n"); \
  }

#define legend_point(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s((0.5,0.5) inscale,0.0,1.0,(0,0));\n",thsymbolset__mp[mid]);  \
  endfig;

#define legend_hpoint(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s((0.5,0.5) inscale,270.0,1.0,(0,0));\n",thsymbolset__mp[mid]);  \
  endfig;


  // meracie body + polygon + station-name
#define legend_station(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s((0.5,0.5) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
  // thT("point station")  
  legend_station(SYMP_STATION_TEMPORARY,thT("point station:temporary",layout->lang));
  legend_station(SYMP_STATION_PAINTED,thT("point station:painted",layout->lang));
  legend_station(SYMP_STATION_NATURAL,thT("point station:natural",layout->lang));
  legend_station(SYMP_STATION_FIXED,thT("point station:fixed",layout->lang));

#define legend_station_flag(mid,txt,flag) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"p_station((0.5,0.5) inscale, 0, \"\", \"%s\");\n",flag);  \
  endfig;
  legend_station_flag(SYMP_FLAG_ENTRANCE,thT("point flag:entrance",layout->lang),"entrance");
  legend_station_flag(SYMP_FLAG_SINK,thT("point flag:sink",layout->lang),"sink");
  legend_station_flag(SYMP_FLAG_SPRING,thT("point flag:spring",layout->lang),"spring");
  legend_station_flag(SYMP_FLAG_DOLINE,thT("point flag:doline",layout->lang),"doline");
  legend_station_flag(SYMP_FLAG_DIG,thT("point flag:dig",layout->lang),"dig");
  legend_station_flag(SYMP_FLAG_CONTINUATION,thT("point flag:continuation",layout->lang),"continuation");
  legend_station_flag(SYMP_FLAG_AIRDRAUGHT,thT("point flag:air-draught",layout->lang),"airdraught");
  legend_station_flag(SYMP_FLAG_ARCH,thT("point flag:arch",layout->lang),"arch");
  legend_station_flag(SYMP_FLAG_OVERHANG,thT("point flag:overhang",layout->lang),"overhang");



#define insert_station(x,y) \
  helpsymbol \
  if (true || isused(SYMP_STATION_TEMPORARY)) \
    fprintf(mpf,"%s((%g,%g) inscale);\n",thsymbolset__mp[SYMP_STATION_TEMPORARY],x,y); \
  else \
  if isused(SYMP_STATION_PAINTED) \
    fprintf(mpf,"%s((%g,%g) inscale);\n",thsymbolset__mp[SYMP_STATION_PAINTED],x,y); \
  else \
  if isused(SYMP_STATION_NATURAL) \
    fprintf(mpf,"%s((%g,%g) inscale);\n",thsymbolset__mp[SYMP_STATION_NATURAL],x,y); \
  else \
  if isused(SYMP_STATION_FIXED) \
    fprintf(mpf,"%s((%g,%g) inscale);\n",thsymbolset__mp[SYMP_STATION_FIXED],x,y); \
  endhelpsymbol;  
  
  insfig(SYML_SURVEY_CAVE,thT("line survey",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_SURVEY_CAVE);
  fprintf(mpf,"%s(((-1,1) -- (0.8,0.6) -- (0,-1)) inscale);\n", thsymbolset__mp[SYML_SURVEY_CAVE]);
  insert_station(0.8,0.6);
  endfig;

  insfig(SYMP_STATIONNAME,thT("point station-name",layout->lang));
  this->export_mp_symbol_options(mpf, SYMP_STATIONNAME);
  fprintf(mpf,"p_label.urt(btex \\thstationname %s etex,((0.3,0.3) inscale),0,p_label_mode_station);\n",utf2tex("173"));
  insert_station(0.3,0.3);
  endfig;

  legend_hpoint(SYMP_ENTRANCE,thT("point entrance",layout->lang));

  insfig(SYML_MAPCONNECTION,thT("line map-connection",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_MAPCONNECTION);
  fprintf(mpf,"%s(((0.2,0.8) -- (0.8,0.2)) inscale)",thsymbolset__mp[SYML_MAPCONNECTION]);
  endfig;

  // steny + wall-altitude + altitude
#define legend_wall(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(((-.3,0.5) .. (.3,.3) .. (.7,.7) .. (1.3,.5)) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
  
  // thT("line wall")  
  legend_wall(SYML_WALL_BEDROCK,thT("line wall:bedrock",layout->lang));
  legend_wall(SYML_WALL_UNDERLYING,thT("line wall:underlying",layout->lang));
  legend_wall(SYML_WALL_OVERLYING,thT("line wall:overlying",layout->lang));
  legend_wall(SYML_WALL_UNSURVEYED,thT("line wall:unsurveyed",layout->lang));
  legend_wall(SYML_WALL_PRESUMED,thT("line wall:presumed",layout->lang));
  legend_wall(SYML_WALL_PIT,thT("line wall:pit",layout->lang));
  legend_wall(SYML_WALL_BLOCKS,thT("line wall:blocks",layout->lang));
  legend_wall(SYML_WALL_DEBRIS,thT("line wall:debris",layout->lang));
  legend_wall(SYML_WALL_PEBBLES,thT("line wall:pebbles",layout->lang));
  legend_wall(SYML_WALL_SAND,thT("line wall:sand",layout->lang));
  legend_wall(SYML_WALL_CLAY,thT("line wall:clay",layout->lang));
  legend_wall(SYML_WALL_ICE,thT("line wall:ice",layout->lang));
  legend_wall(SYML_WALL_FLOWSTONE,thT("line wall:flowstone",layout->lang));
  legend_wall(SYML_WALL_MOONMILK,thT("line wall:moonmilk",layout->lang));

  insfig(SYMP_WALLALTITUDE,thT("point wall-altitude",layout->lang));
  helpsymbol;
  if (true || isused(SYML_WALL_BEDROCK)) 
    fprintf(mpf,"%s(((-.3,0.5) .. controls (.2,.6) and (.2,.6) .. (.3,.7) .. controls (.4,.8) and (.4,.8) .. (.5,1.4)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_WALLALTITUDE);
  fprintf(mpf,"%s((0.2,0.6) inscale,(0.3,0.7) inscale,(0.4,0.8) inscale,btex \\thwallaltitude %s etex);\n",thsymbolset__mp[SYMP_WALLALTITUDE],utf2tex("1510"));
  endfig;
  
  insfig(SYMP_ALTITUDE,thT("point altitude",layout->lang));
  this->export_mp_symbol_options(mpf, SYMP_ALTITUDE);
  fprintf(mpf,"p_label.rt(btex \\thaltitude %s etex,((0.3,0.5) inscale),0,p_label_mode_altitude);\n",utf2tex("1510"));
  endfig;

  // thT("point section")  
  insfig(SYML_SECTION,thT("line section",layout->lang));
  helpsymbol;
  if (true || (true || isused(SYML_WALL_BEDROCK))) {
    fprintf(mpf,"%s(((.25,1.0) .. (.2,.5){dir 270} .. (.15,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
    fprintf(mpf,"%s(((.3,0.0) .. (.4,.5){dir 90} .. (.5,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
    if (true || isused(SYMP_SECTION)) {
      fprintf(mpf,"%s(((.7,.5){dir 90} .. (.9,.75){dir 270} .. (.8,.15){dir 235} .. cycle) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);    
    }
  }    
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYML_SECTION);
  fprintf(mpf,"%s(((0,.5) .. controls (.195,.5) and (.405,.5) .. (.6,.5)) inscale,1);\n",thsymbolset__mp[SYML_SECTION]);
  endfig;


  // vysky chodby
#define insert_big_passage \
  helpsymbol; \
  if (true || isused(SYML_WALL_BEDROCK)) { \
    fprintf(mpf,"%s(((.35,1.0) .. (.3,.5){dir 270} .. (.25,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.6,0.0) .. (.7,.5){dir 90} .. (.8,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;

#define insert_big_water_passage \
  helpsymbol; \
  if (true || isused(SYML_WALL_BEDROCK)) { \
    fprintf(mpf,"%s(((.35,1.0) .. (.3,.5){dir 270} .. (.25,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.6,0.0) .. (.7,.5){dir 90} .. (.8,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;


  // thT("point passage-height")  

  insfig(SYMP_PASSAGEHEIGHT_UNSIGNED,thT("point passage-height:unsigned",layout->lang));
  //insert_big_passage
  this->export_mp_symbol_options(mpf, SYMP_PASSAGEHEIGHT_UNSIGNED);
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,p_label_mode_passageheight);\n",utf2tex("5"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_POSITIVE,thT("point passage-height:positive",layout->lang));
  //insert_big_water_passage
  this->export_mp_symbol_options(mpf, SYMP_PASSAGEHEIGHT_POSITIVE);
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,p_label_mode_passageheightpos);\n",utf2tex("3"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_NEGATIVE,thT("point passage-height:negative",layout->lang));
  //insert_big_water_passage
  this->export_mp_symbol_options(mpf, SYMP_PASSAGEHEIGHT_NEGATIVE);
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,p_label_mode_passageheightneg);\n",utf2tex("2"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_BOTH,thT("point passage-height:both",layout->lang));
  //insert_big_water_passage
  this->export_mp_symbol_options(mpf, SYMP_PASSAGEHEIGHT_BOTH);
  fprintf(mpf,"p_label(btex \\thframed \\updown{%s}",utf2tex("3"));
  fprintf(mpf,"{%s} etex,((0.5,0.5) inscale),0,p_label_mode_passageheightposneg);\n",utf2tex("2"));
  endfig;

  legend_hpoint(SYMP_AIRDRAUGHT,thT("point air-draught",layout->lang));
  legend_hpoint(SYMP_AIRDRAUGHT_WINTER,thT("point air-draught:winter",layout->lang));
  legend_hpoint(SYMP_AIRDRAUGHT_SUMMER,thT("point air-draught:summer",layout->lang));

  thdate d;
  d.parse("1999.12.31");
  insfig(SYMP_DATE,thT("point date",layout->lang));
  //insert_big_water_passage
  this->export_mp_symbol_options(mpf, SYMP_DATE);
  fprintf(mpf,"p_label(btex \\thdate %s etex,((0.5,0.5) inscale),0,p_label_mode_date);\n",utf2tex(d.get_str(TT_DATE_FMT_UTF8_ISO)));
  endfig;

  
  // ukoncenia
#define legend_end(mid,txt) \
  insfig(mid,txt); \
  helpsymbol; \
  if (true || isused(SYML_WALL_BEDROCK)) {\
    fprintf(mpf,"%s(((0,.2){dir 30} .. {dir 0}(.5,.4)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.5,.6){dir 180} .. {dir 210}(0,.8)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol; \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s((.6,.5) inscale,270.0,1.0,(0,1));\n",thsymbolset__mp[mid]);  \
  endfig;
  
  legend_point(SYMP_DIG,thT("point dig",layout->lang))
  legend_end(SYMP_CONTINUATION,thT("point continuation",layout->lang))
  legend_end(SYMP_NARROWEND,thT("point narrow-end",layout->lang))
  legend_end(SYMP_LOWEND,thT("point low-end",layout->lang))
  legend_end(SYMP_FLOWSTONECHOKE,thT("point flowstone-choke",layout->lang))
  legend_end(SYMP_BREAKDOWNCHOKE,thT("point breakdown-choke",layout->lang))
  legend_end(SYMP_CLAYCHOKE,thT("point clay-choke",layout->lang))
  
#define legend_iuline "(((.1,.6) .. (.5,.4) .. (.9,.6)) inscale)"
#define legend_cline "(((.85,.5){dir 90} .. (.5,.85){dir 180} .. (.4,.15){dir 0} .. cycle) inscale)"
#define legend_scline "(((.5,.5){dir 90} .. (.3,.75){dir 180} .. (.15,.5) .. (.2,.25){dir 0} .. cycle) inscale)"

#define legend_step(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(%s);\n",thsymbolset__mp[mid],legend_iuline); \
  endfig;
#define legend_cycle(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(%s);\n",thsymbolset__mp[mid],legend_cline); \
  endfig;
#define insert_small_passage \
  helpsymbol; \
  if (true || isused(SYML_WALL_BEDROCK)) { \
    fprintf(mpf,"%s(((.2,1.0) .. (.15,.5){dir 270} .. (.1,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.4,0.0) .. (.5,.5){dir 90} .. (.6,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;
  
  // zrazy + kominy + priepasti + gradient
  legend_step(SYML_FLOORSTEP,thT("line floor-step",layout->lang));
  legend_step(SYML_OVERHANG,thT("line overhang",layout->lang));
  legend_step(SYML_FLOORMEANDER,thT("line floor-meander",layout->lang));
  legend_cycle(SYML_PIT,thT("line pit",layout->lang));
  legend_step(SYML_CEILINGSTEP,thT("line ceiling-step",layout->lang));
  legend_step(SYML_CEILINGMEANDER,thT("line ceiling-meander",layout->lang));
  legend_cycle(SYML_CHIMNEY,thT("line chimney",layout->lang));
  if isused(SYML_GRADIENT) {
    insfig(SYML_GRADIENT,thT("line gradient",layout->lang));
    this->export_mp_symbol_options(mpf, SYML_GRADIENT);
    fprintf(mpf,"%s(((0.2,0.5) -- (0.8,0.5)) inscale);\n",thsymbolset__mp[SYML_GRADIENT]);
    endfig;
  } 
  if ((!(isused(SYML_GRADIENT)) || (!(this->group_symbols)))) {
    this->export_mp_symbol_options(mpf, SYMP_GRADIENT);
    legend_hpoint(SYMP_GRADIENT,thT("point gradient",layout->lang));
  }

// thT("point height")  
// thT("point dimensions")  
  insfig(SYMP_HEIGHT_UNSIGNED,thT("point height:unsigned",layout->lang));
  helpsymbol;
  if isused(SYML_FLOORSTEP) {
    fprintf(mpf,"%s(((.15,.5){dir 340} .. (.5,.5)) inscale);\n",thsymbolset__mp[SYML_FLOORSTEP]);
    insert_small_passage
  } else 
  if isused(SYML_PIT) {
    fprintf(mpf,"%s(((.15,.5){dir 340} .. (.5,.5)) inscale);\n",thsymbolset__mp[SYML_PIT]);
    insert_small_passage
  }
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_HEIGHT_UNSIGNED);
  fprintf(mpf,"p_label.rt(btex \\thheight %s etex,((0.5,0.5) inscale),0,p_label_mode_height);\n",utf2tex("4"));
  endfig;

  insfig(SYMP_HEIGHT_POSITIVE,thT("point height:positive",layout->lang));
  helpsymbol;
  if isused(SYML_CHIMNEY)
    fprintf(mpf,"%s(%s);\n",thsymbolset__mp[SYML_CHIMNEY],legend_scline);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_HEIGHT_POSITIVE);
  fprintf(mpf,"p_label.rt(btex \\thheightpos %s etex,((0.5,0.5) inscale),0,p_label_mode_height);\n",utf2tex("15"));
  endfig;

  insfig(SYMP_HEIGHT_NEGATIVE,thT("point height:negative",layout->lang));
  helpsymbol;
  if isused(SYML_PIT)
    fprintf(mpf,"%s(%s);\n",thsymbolset__mp[SYML_PIT],legend_scline);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_HEIGHT_NEGATIVE);
  fprintf(mpf,"p_label.rt(btex \\thheightneg %s etex,((0.5,0.5) inscale),0,p_label_mode_height);\n",utf2tex("30"));
  endfig;

  insfig(SYML_CONTOUR,thT("line contour",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_CONTOUR);
  fprintf(mpf,"%s(%s,-1);\n",thsymbolset__mp[SYML_CONTOUR],legend_iuline);
  endfig;

  insfig(SYML_SLOPE,thT("line slope",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_SLOPE);
  fprintf(mpf,"%s((((.1,.35) .. (.5,.25) .. (.9,.35)) inscale),1,(0,-1,1u),(2,-1,1u));\n",thsymbolset__mp[SYML_SLOPE]);
  endfig;
  
  // kamene
  insfig(SYML_ROCKBORDER,thT("line rock-border",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_ROCKBORDER);
  fprintf(mpf,"%s(((.16,.36) -- (.61,.21) -- (.91,.46) -- (.84,.78) -- (.38,.86) -- (.20,.55) -- cycle) inscale)",thsymbolset__mp[SYML_ROCKBORDER]);
  endfig;

  insfig(SYML_ROCKEDGE,thT("line rock-edge",layout->lang));
  helpsymbol;
  if isused(SYML_ROCKBORDER)
    fprintf(mpf,"%s(((.16,.36) -- (.61,.21) -- (.91,.46) -- (.84,.78) -- (.38,.86) -- (.20,.55) -- cycle) inscale)",thsymbolset__mp[SYML_ROCKBORDER]);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYML_ROCKEDGE);
  fprintf(mpf,"%s(((.16,.36) -- (.42,.62) -- (.38,.86) -- (.42,.62) -- (.6,.55) -- (.61,.21) -- (.6,.55) -- (.84,.78)) inscale)",thsymbolset__mp[SYML_ROCKEDGE]);
  endfig;

  // vypln bodova
  legend_point(SYMP_BEDROCK,thT("point bedrock",layout->lang));
  legend_point(SYMP_BLOCKS,thT("point blocks",layout->lang));
  legend_point(SYMP_DEBRIS,thT("point debris",layout->lang));
  legend_point(SYMP_SAND,thT("point sand",layout->lang));
  legend_point(SYMP_CLAY,thT("point clay",layout->lang));
  legend_point(SYMP_MUDCRACK,thT("point mudcrack",layout->lang));
  legend_point(SYMP_WATER,thT("point water",layout->lang));
  legend_point(SYMP_ICE,thT("point ice",layout->lang));
  legend_point(SYMP_SNOW,thT("point snow",layout->lang));
  legend_point(SYMP_PEBBLES,thT("point pebbles",layout->lang));
  legend_point(SYMP_RAFT,thT("point raft",layout->lang));
  legend_point(SYMP_GUANO,thT("point guano",layout->lang));
  legend_point(SYMP_CLAYTREE,thT("point clay-tree",layout->lang));
  
  // okraje
  // thT("line border")  
  legend_cycle(SYML_BORDER_VISIBLE,thT("line border:visible",layout->lang));
  legend_cycle(SYML_BORDER_TEMPORARY,thT("line border:temporary",layout->lang));
  legend_cycle(SYML_BORDER_PRESUMED,thT("line border:presumed",layout->lang));

  // vypln plosna
#define legend_area(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(buildcycle((((-1,0) -- (1,0) -- (1,1) -- (0,1) -- (0,-1))  inscale)));\n",thsymbolset__mp[mid]); \
  endfig;
#define legend_nocliparea(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(buildcycle((((-4,-4) -- (4,-4) -- (4,4) -- (-4,4) -- (-4,-4))  inscale)));\n",thsymbolset__mp[mid]); \
  endfig;
  legend_area(SYMA_WATER,thT("area water",layout->lang));  
  legend_area(SYMA_SUMP,thT("area sump",layout->lang));  
  legend_area(SYMA_SNOW,thT("area snow",layout->lang));  
  legend_area(SYMA_ICE,thT("area ice",layout->lang));  
  legend_area(SYMA_SAND,thT("area sand",layout->lang));  
  legend_area(SYMA_CLAY,thT("area clay",layout->lang));  
  legend_area(SYMA_PEBBLES,thT("area pebbles",layout->lang));  
  legend_area(SYMA_DEBRIS,thT("area debris",layout->lang));  
  legend_area(SYMA_FLOWSTONE,thT("area flowstone",layout->lang));  
  legend_area(SYMA_MOONMILK,thT("area moonmilk",layout->lang));  
  legend_nocliparea(SYMA_BLOCKS,thT("area blocks",layout->lang));  
  legend_nocliparea(SYMA_BEDROCK,thT("area bedrock",layout->lang));  
  
  // vodne toky (ciary, body)
#define legend_waterflow(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(((0.2,0.8) -- (0.8,0.2)) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
	
	
  // thT("line water-flow")  
  // thT("point water-flow")  
  if (isused(SYML_WATERFLOW_PERMANENT) && isused(SYMP_WATERFLOW_PERMANENT) && this->group_symbols) {
	  insfig(SYML_WATERFLOW_PERMANENT,thT("line water-flow:permanent",layout->lang));
    this->export_mp_symbol_options(mpf, SYML_WATERFLOW_PERMANENT);
	  fprintf(mpf,"%s(((0.1,0.3) -- (0.9,0.3)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_PERMANENT]);
    this->export_mp_symbol_options(mpf, SYMP_WATERFLOW_PERMANENT);
    fprintf(mpf,"%s((0.5,0.7) inscale,270.0,1.0,(0,0));\n",thsymbolset__mp[SYMP_WATERFLOW_PERMANENT]);
	  endfig;
  } else {
	  legend_waterflow(SYML_WATERFLOW_PERMANENT,thT("line water-flow:permanent",layout->lang));
	  legend_hpoint(SYMP_WATERFLOW_PERMANENT,thT("point water-flow:permanent",layout->lang));
	}
	
  if (isused(SYML_WATERFLOW_INTERMITTENT) && isused(SYMP_WATERFLOW_INTERMITTENT) && this->group_symbols) {
	  insfig(SYML_WATERFLOW_INTERMITTENT,thT("line water-flow:intermittent",layout->lang));
    this->export_mp_symbol_options(mpf, SYML_WATERFLOW_INTERMITTENT);
	  fprintf(mpf,"%s(((0.1,0.3) -- (0.9,0.3)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_INTERMITTENT]);
    this->export_mp_symbol_options(mpf, SYML_WATERFLOW_INTERMITTENT);
    fprintf(mpf,"%s((0.5,0.7) inscale,270.0,1.0,(0,0));\n",thsymbolset__mp[SYMP_WATERFLOW_INTERMITTENT]);
	  endfig;
  } else {
	  legend_waterflow(SYML_WATERFLOW_INTERMITTENT,thT("line water-flow:intermittent",layout->lang));
	  legend_hpoint(SYMP_WATERFLOW_INTERMITTENT,thT("point water-flow:intermittent",layout->lang));
  }

  legend_waterflow(SYML_WATERFLOW_CONJECTURAL,thT("line water-flow:conjectural",layout->lang));
  legend_hpoint(SYMP_WATERFLOW_PALEO,thT("point water-flow:paleo",layout->lang));

  insfig(SYMP_SPRING,thT("point spring",layout->lang));
  helpsymbol;
  fprintf(mpf,"%s(((0.3,0.5) -- (0.9,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_PERMANENT]);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_SPRING);
  fprintf(mpf,"%s((0.3,0.5) inscale,270,1.0,(0,-1));\n",thsymbolset__mp[SYMP_SPRING]);
  endfig;

  insfig(SYMP_SINK,thT("point sink",layout->lang));
  helpsymbol;
  fprintf(mpf,"%s(((0.1,0.5) -- (0.7,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_PERMANENT]);
  endhelpsymbol;
  this->export_mp_symbol_options(mpf, SYMP_SINK);
  fprintf(mpf,"%s((0.7,0.5) inscale,270,1.0,(0,1));\n",thsymbolset__mp[SYMP_SINK]);
  endfig;

    
  // vyzdoba
  if (isused(SYMP_FLOWSTONE) && isused(SYML_FLOWSTONE) && this->group_symbols) {
	  insfig(SYMP_FLOWSTONE,thT("point flowstone",layout->lang));
    this->export_mp_symbol_options(mpf, SYML_FLOWSTONE);
	  fprintf(mpf,"%s(((.1,.4) .. (.5,.2) .. (.9,.4)) inscale);\n",thsymbolset__mp[SYML_FLOWSTONE]);
    this->export_mp_symbol_options(mpf, SYMP_FLOWSTONE);
    fprintf(mpf,"%s((0.5,0.7) inscale,0.0,1.0,(0,0));\n",thsymbolset__mp[SYMP_FLOWSTONE]);
	  endfig;
  } else {
	  legend_point(SYMP_FLOWSTONE,thT("point flowstone",layout->lang));
	  legend_step(SYML_FLOWSTONE,thT("line flowstone",layout->lang));
  }

  if (isused(SYMP_MOONMILK) && isused(SYML_MOONMILK) && this->group_symbols) {
	  insfig(SYMP_MOONMILK,thT("point moonmilk",layout->lang));
    this->export_mp_symbol_options(mpf, SYML_MOONMILK);
	  fprintf(mpf,"%s(((.1,.4) .. (.5,.2) .. (.9,.4)) inscale);\n",thsymbolset__mp[SYML_MOONMILK]);
    this->export_mp_symbol_options(mpf, SYMP_MOONMILK);
    fprintf(mpf,"%s((0.5,0.7) inscale,0.0,1.0,(0,0));\n",thsymbolset__mp[SYMP_MOONMILK]);
	  endfig;
  } else {
	  legend_point(SYMP_MOONMILK,thT("point moonmilk",layout->lang));
	  legend_step(SYML_MOONMILK,thT("line moonmilk",layout->lang));
  }

  legend_point(SYMP_STALACTITE,thT("point stalactite",layout->lang));
  legend_point(SYMP_STALACTITES,thT("point stalactites",layout->lang));
  legend_point(SYMP_STALAGMITE,thT("point stalagmite",layout->lang));
  legend_point(SYMP_STALAGMITES,thT("point stalagmites",layout->lang));
  legend_point(SYMP_PILLAR,thT("point pillar",layout->lang));
  legend_point(SYMP_PILLARS,thT("point pillars",layout->lang));
  legend_point(SYMP_ICESTALACTITE,thT("point ice-stalactite",layout->lang));
  legend_point(SYMP_ICESTALAGMITE,thT("point ice-stalagmite",layout->lang));
  legend_point(SYMP_ICEPILLAR,thT("point ice-pillar",layout->lang));
  legend_point(SYMP_CURTAIN,thT("point curtain",layout->lang));
  legend_point(SYMP_SODASTRAW,thT("point soda-straw",layout->lang));
  legend_point(SYMP_POPCORN,thT("point popcorn",layout->lang));
  legend_point(SYMP_CAVEPEARL,thT("point cave-pearl",layout->lang));
  legend_point(SYMP_DISK,thT("point disk",layout->lang));
  legend_point(SYMP_HELICTITE,thT("point helictite",layout->lang));
  legend_point(SYMP_ARAGONITE,thT("point aragonite",layout->lang));
  legend_point(SYMP_CRYSTAL,thT("point crystal",layout->lang));
  legend_point(SYMP_WALLCALCITE,thT("point wall-calcite",layout->lang));
  legend_point(SYMP_GYPSUM,thT("point gypsum",layout->lang));
  legend_point(SYMP_GYPSUMFLOWER,thT("point gypsum-flower",layout->lang));
  legend_point(SYMP_RIMSTONEDAM,thT("point rimstone-dam",layout->lang));
  legend_point(SYMP_RIMSTONEPOOL,thT("point rimstone-pool",layout->lang));
  legend_point(SYMP_ANASTOMOSIS,thT("point anastomosis",layout->lang));
  legend_point(SYMP_KARREN,thT("point karren",layout->lang));
  legend_point(SYMP_SCALLOP,thT("point scallop",layout->lang));
  legend_point(SYMP_FLUTE,thT("point flute",layout->lang));
  legend_point(SYMP_RAFTCONE,thT("point raft-cone",layout->lang));

  // ostatne materialy
  legend_point(SYMP_ARCHEOMATERIAL,thT("point archeo-material",layout->lang));
  legend_point(SYMP_PALEOMATERIAL,thT("point paleo-material",layout->lang));
  legend_point(SYMP_VEGETABLEDEBRIS,thT("point vegetable-debris",layout->lang));
  legend_point(SYMP_ROOT,thT("point root",layout->lang));

#define legend_eqline(mid,txt) \
  insfig(mid,txt); \
  this->export_mp_symbol_options(mpf, mid); \
  fprintf(mpf,"%s(((0.1,0.5) .. (0.9,.5)) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;

  // vystroj  
  legend_point(SYMP_NOEQUIPMENT,thT("point no-equipment",layout->lang));
  legend_point(SYMP_ANCHOR,thT("point anchor",layout->lang));
  legend_point(SYMP_ROPE,thT("point rope",layout->lang));
  legend_point(SYMP_ROPELADDER,thT("point rope-ladder",layout->lang));
  legend_eqline(SYML_ROPELADDER,thT("line rope-ladder",layout->lang));
  legend_point(SYMP_FIXEDLADDER,thT("point fixed-ladder",layout->lang));
  legend_eqline(SYML_FIXEDLADDER,thT("line fixed-ladder",layout->lang));
  legend_point(SYMP_STEPS,thT("point steps",layout->lang));
  legend_point(SYMP_ALTAR,thT("point altar",layout->lang));
  legend_point(SYMP_DANGER,thT("point danger",layout->lang));
  legend_point(SYMP_ELECTRICLIGHT,thT("point electric-light",layout->lang));
  legend_point(SYMP_EXVOTO,thT("point ex-voto",layout->lang));
  legend_point(SYMP_GATE,thT("point gate",layout->lang));
  legend_point(SYMP_HUMANBONE,thT("point human-bone",layout->lang));
  legend_point(SYMP_MASONRY,thT("point masonry",layout->lang));
  legend_point(SYMP_MINUS,thT("point minus",layout->lang));
  legend_point(SYMP_MUD,thT("point mud",layout->lang));
  legend_point(SYMP_NAMEPLATE,thT("point nameplate",layout->lang));
  legend_point(SYMP_NOTWHEELCHAIR,thT("point notwheelchair",layout->lang));
  legend_point(SYMP_PLUS,thT("point plus",layout->lang));
  legend_point(SYMP_PLUSMINUS,thT("point plus-minus",layout->lang));
  legend_point(SYMP_WALKWAY,thT("point walkway",layout->lang));
  legend_point(SYMP_WATERDRIP,thT("point water-drip",layout->lang));
  legend_point(SYMP_WHEELCHAIR,thT("point wheelchair",layout->lang));

  insfig(SYML_ROPE,thT("line rope",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_ROPE);
  fprintf(mpf,"%s(((0.1,0.85) -- (0.25,0.6) -- (0.5,0.1)) inscale, false, true);\n",thsymbolset__mp[SYML_ROPE]);
  fprintf(mpf,"%s(((0.6,0.8) -- (0.8,0.2)) inscale, true, false);\n",thsymbolset__mp[SYML_ROPE]);
  endfig;

	insfig(SYML_STEPS,thT("line steps",layout->lang));
  this->export_mp_symbol_options(mpf, SYML_STEPS);
  fprintf(mpf,"%s(((0.1,0.3) -- (0.9,.3) -- (0.9,0.7) -- (0.1,0.7) -- cycle) inscale);\n",thsymbolset__mp[SYML_STEPS]);
  endfig;

  legend_point(SYMP_VIAFERRATA,thT("point via-ferrata",layout->lang));
  legend_eqline(SYML_VIAFERRATA,thT("line via-ferrata",layout->lang));
  legend_point(SYMP_TRAVERSE,thT("point traverse",layout->lang));
  legend_point(SYMP_BRIDGE,thT("point bridge",layout->lang));
  legend_point(SYMP_HANDRAIL,thT("point handrail",layout->lang));
  legend_eqline(SYML_HANDRAIL,thT("line handrail",layout->lang));
  legend_point(SYMP_CAMP,thT("point camp",layout->lang));

  // thT("point remark")  
  // thT("point label")  
  // thT("line label")  

  // nakoniec prejde uzivatelsky definovane symboly
  thdb2d_udef_map::iterator it;
  std::string udef_desc;
  int mid = SYMX_;
  if (layout->db != NULL) {
    for(it = layout->db->db2d.m_udef_map.begin(); it != layout->db->db2d.m_udef_map.end(); it++) {
      if (it->second->m_assigned && (it->second->m_used || (layout->legend == TT_LAYOUT_LEGEND_ALL))) {
        switch (it->first.m_command) {
          case TT_POINT_CMD:
            udef_desc = "point";
            break;
          case TT_LINE_CMD:
            udef_desc = "line";
            break;
          case TT_AREA_CMD:
            udef_desc = "area";
            break;
        }
        udef_desc += " u:";
        udef_desc += it->first.m_type;
        if (strlen(thT(udef_desc.c_str(), layout->lang)) > 0) {
          insfig(0,thsymbolset__mp[0]);
          switch (it->first.m_command) {
            case TT_POINT_CMD:
              fprintf(mpf,"p_u_%s_legend;\n",it->first.m_type);
              break;
            case TT_LINE_CMD:
              fprintf(mpf,"l_u_%s_legend;\n",it->first.m_type);
              break;
            case TT_AREA_CMD:
              fprintf(mpf,"a_u_%s_legend;\n",it->first.m_type);
              break;
          }
          LEGENDITEM->descr = thT(udef_desc.c_str(), layout->lang);
          LEGENDITEM->idsym = (unsigned) mid++;
          endfig;
        }
      }
    }
  }
  
}



void export_all_symbols()
{

  // vytvori si temporary MP subor
  FILE * mpf;
  thlayout tmplayout;
  tmplayout.lang = THLANG_SYSTEM;
  mpf = fopen(thtmp.get_file_name("data.mp"),"w");

  // spusti metapost a nasledne pdftex
//  fprintf(mpf,"Scale:=%.2f;\n",0.01 / tmplayout.scale);
//  if (tmplayout.def_base_scale || tmplayout.redef_base_scale)
//    fprintf(mpf,"BaseScale:=%.2f;\n",0.01 / tmplayout.base_scale);
  fprintf(mpf,"Scale:=0.8;\n");
  fprintf(mpf,"BaseScale:=2;\n");
  fprintf(mpf,"color HelpSymbolColor;\nHelpSymbolColor := (0.8, 0.8, 0.8);\n");
  fprintf(mpf,"background:=(1.0, 1.0, 1.0);\n");
  fprintf(mpf,"verbatimtex \\input th_enc.tex etex;\n");
  if (thcmdln.extern_libs)
    fprintf(mpf,"input therion;\n");
  else
    fprintf(mpf,"%s\n",thmpost_library);
  fprintf(mpf,"lang:=\"%s\";\n",thlang_getid(tmplayout.lang));
  if (ENC_NEW.NFSS==0)
    fprintf(mpf,"defaultfont:=\"%s\";\n",FONTS.begin()->ss.c_str());
  else
    fprintf(mpf,"defaultfont:=\"thss00\";\n");
  
  tmplayout.export_mpost(mpf);
  fprintf(mpf,"background:=white;\n");
  fprintf(mpf,"transparency:=false;\n");

  // vyexportuje secky znacky
  size_t iset, isym;
  unsigned figi = 1;
  thsymbolset symset;
  thsymsets_symbols_init();
  list<legendrecord>::iterator li;
  // najprv exportujeme secky defaultne
  tmplayout.legend = TT_LAYOUT_LEGEND_ALL;
  LEGENDLIST.clear();
  symset.group_symbols = false;
  symset.export_pdf(&tmplayout, mpf, figi);
  for(isym = 0; isym < thsymbolset_size; isym++) {
    thsymsets_figure[isym][thsymsets_size] = 0;
    thsymsets_order[isym] = 0;
  }
  for(li = LEGENDLIST.begin(); li != LEGENDLIST.end(); li++) {
    thsymsets_figure[li->idsym][thsymsets_size] = li->idfig;
    thsymsets_comment[li->idsym] = li->descr;
    thsymsets_order[li->idnum] = li->idsym;
  }
  tmplayout.legend = TT_LAYOUT_LEGEND_ON;
  for (iset = 0; iset < thsymsets_size; iset++) {
    LEGENDLIST.clear();
    symset.export_symbol_defaults(mpf, thsymsets[iset]);
    for(isym = 0; isym < thsymbolset_size; isym++) {
      if (thsymsets_symbols[isym][iset] > 0)
        symset.used[isym] = true;
      else
        symset.used[isym] = false;
    }
    symset.export_pdf(&tmplayout, mpf, figi);
    for(isym = 0; isym < thsymbolset_size; isym++) {
      thsymsets_figure[isym][iset] = 0;
    }
    thsymsets_count[iset] = 0;
    for(li = LEGENDLIST.begin(); li != LEGENDLIST.end(); li++) {
      thsymsets_figure[li->idsym][iset] = li->idfig;
      thsymsets_count[iset]++;
    }
  }

  fprintf(mpf,"end;\n");
  fclose(mpf);

  // run MP
  thbuffer com, wdir;
  wdir.guarantee(1024);
  getcwd(wdir.get_buffer(),1024);
  chdir(thtmp.get_dir_name());

  // vypise kodovania
  print_fonts_setup();
  
  int retcode;
  
#ifdef THWIN32
  if (!thini.tex_env) {
    putenv("TEXMFCNF=");
    putenv("DVIPSHEADERS=");
    putenv("GFFONTS=");
    putenv("GLYPHFONTS=");
    putenv("MFBASES=");
    putenv("MFINPUTS=");
    putenv("MFPOOL=");
#ifdef THMSVC
    putenv("MPINPUTS=../mpost;.");
#else
    putenv("MPINPUTS=");
#endif
    putenv("MPMEMS=");
    putenv("MPPOOL=");
    putenv("MPSUPPORT=");
    putenv("PKFONTS=");
    putenv("PSHEADERS=");
    putenv("T1FONTS=");
    putenv("T1INPUTS=");
    putenv("T42FONTS=");
    putenv("TEXCONFIG=");
    putenv("TEXDOCS=");
    putenv("TEXFONTMAPS=");
    putenv("TEXFONTS=");
    putenv("TEXFORMATS=");
    putenv("TEXINPUTS=");
    putenv("TEXMFDBS=");
    putenv("TEXMFINI=");
    putenv("TEXPICTS=");
    putenv("TEXPKS=");
    putenv("TEXPOOL=");
    putenv("TEXPSHEADERS=");
    putenv("TEXSOURCES=");
    putenv("TFMFONTS=");
    putenv("TTFONTS=");
    putenv("VFFONTS=");
    putenv("WEB2C=");
  }
#endif  

  // exportuje 
  com = "\"";
  com += thini.get_path_mpost();
  com += "\" ";
  com += thini.get_opt_mpost();
//    com += " --interaction nonstopmode data.mp";
  com += " data.mp";
#ifdef THDEBUG
  thprintf("running metapost\n");
#endif
  retcode = system(com.get_buffer());
  thsymbolset_log_log_file("data.log",
  "####################### metapost log file ########################\n",
  "#################### end of metapost log file ####################\n",true);
  if (retcode != EXIT_SUCCESS) {
    chdir(wdir.get_buffer());
    ththrow(("metapost exit code -- %d", retcode))
  }

  thconvert_new();

  chdir(wdir.get_buffer());
  ofstream hf ("symbols.xhtml");
  hf << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  hf << "<html xmlns=\"http://www.w3.org/1999/xhtml\"  xmlns:xlink=\"http://www.w3.org/1999/xlink\"><title>Therion symbols</title>\n<body>\n";
  hf << "<table border=\"2\" bordercolor=\"#505050\" cellspacing=\"0\">\n<tr>\n<td>Symbol set</td>\n";
  for(iset = 0; iset < thsymsets_size; iset++) {
    if (thsymsets_count[iset] > 0) {
      hf << "<td><b>" << thsymsets[iset] << "</b></td>\n";
    }
  }
  hf << "</tr>\n";
  unsigned sx, fx;
  char fname[100];
  converted_data svgpict;
  double a,b,c,d;
  for(isym = 0; isym < thsymbolset_size; isym++) {
    if(thsymsets_order[isym] > 0) {
      sx = thsymsets_order[isym];
      hf << "<tr>\n<td>" << thsymsets_comment[sx].c_str() << "</td>\n";
      fx = 0;
      for(iset = 0; iset < thsymsets_size; iset++) {
        fx += thsymsets_figure[sx][iset];
      }
      if (fx > 0) {
        for(iset = 0; iset < thsymsets_size; iset++) {
          if (thsymsets_count[iset] > 0) {
            fx = thsymsets_figure[sx][iset];
            if (fx > 0) {
              sprintf(fname, "%s/data.%d", thtmp.get_dir_name(),fx);
         	    parse_eps(fname,"",0,0,a,b,c,d,svgpict);
              hf << "<td>\n";
         	    svgpict.print_svg(hf);
              hf << "</td>\n";
            } else {
              hf << "<td></td>\n";
            }
          }
        }
      } else {
        sprintf(fname, "%s/data.%d", thtmp.get_dir_name(),thsymsets_figure[sx][thsymsets_size]);
        parse_eps(fname,"",0,0,a,b,c,d,svgpict);
        hf << "<td bgcolor=\"#cccccc\" colspan=\"" << thsymsets_size << "\">";
        svgpict.print_svg(hf);
        hf << "</td>\n";
      }
      hf << "</tr>\n";
    }
  }
  hf << "</table>\n";
  hf << "</body></html>" << endl;
  hf.close();
}



void thsymbolset::export_mp_symbol_options(FILE * mpf, int sym_id)
{
  if ((sym_id >= 0) && (this->color[sym_id].defined)) {
    fprintf(mpf,"drawoptions(withcolor (%.6f,%.6f,%.6f));\n", this->color[sym_id].R, this->color[sym_id].G, this->color[sym_id].B);
  } else {
    fprintf(mpf,"drawoptions();\n");
  }
}

void thsymbolset::export_mp_symbol_options(thexception * x, int sym_id)
{
  if ((sym_id >= 0) && (this->color[sym_id].defined)) {
    x->appspf("drawoptions(withcolor (%.6f,%.6f,%.6f));\n", this->color[sym_id].R, this->color[sym_id].G, this->color[sym_id].B);
  } else {
    x->appspf("drawoptions();\n");
  }
}


bool thsymbolset::is_assigned(int symbol)
{
  if (symbol < SYMX_)
    return this->assigned[symbol];
  if (symbol > SYMX_) {
    int id = 0;
    int cid = thsymbolset__get_group(symbol,id++);
    while (cid >= 0) {
      if (this->assigned[cid]) return true;
      cid = thsymbolset__get_group(symbol,id++);
    }
  }
  return false;
}
