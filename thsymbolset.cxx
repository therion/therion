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

thsymbolset::thsymbolset()
{
  for(int i = 0; i < thsymbolset_size; i++) {
    this->assigned[i] = !(thsymbolset__hidden[i]);
    this->used[i] = false;
  }
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
  {"centerline", SYMX_CENTERLINE},
  {"centreline", SYMX_CENTERLINE},
  {"sections", SYMX_SECTIONS},
  {NULL, SYMX_}
};


static const thstok thtt_symbol_point_spec[] = {
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
      if ((type == TT_POINT_TYPE_UNKNOWN) && (strlen(subtypes) == 0)) {
        type = thmatch_token(types,thtt_symbol_point_spec);
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
        cp3(TT_POINT_TYPE_CAMP,SYMP_CAMP);
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
        cp3(TT_POINT_TYPE_HELICTITE,SYMP_HELICTITE);
        cp3(TT_POINT_TYPE_ICE,SYMP_ICE);
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
      }
      break;
  }
  return rv;
}

void thsymbolset::export_symbol_defaults(FILE * mpf, char * symset) 
{
  fprintf(mpf,"\n\n\n%% %s symbol set.\n",symset);
  for(int id = 0; id < SYMX_; id++) {
    if (thsymbolset__assign[id]) {
      this->assigned[id] = true;
      fprintf(mpf,"mapsymbol(\"%s\",\"%s\",false);\n",thsymbolset__mp[id],symset);
    }
  }
}
  
void thsymbolset::export_symbol_assign(FILE * mpf, int sym_id, char * symset) 
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

void thsymbolset::export_symbol_assign_group(FILE * mpf, int sym_id, char * symset) 
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
    group(1,SYML_SURVEY)
    group(2,SYMP_STATIONNAME)
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
    
    bgroup(SYMX_POINT_STATION)
    group(0,SYMP_STATION_FIXED)
    group(1,SYMP_STATION_NATURAL)
    group(2,SYMP_STATION_PAINTED)
    group(3,SYMP_STATION_TEMPORARY)
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
  bool isin[thsymbolset_size];
  
  for(int i = 0; i < thsymbolset_size; i++)
    isin[i] = true;

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
  if (isused(mid)) {\
    isin[mid] = false;\
    LEGENDITEM = LEGENDLIST.insert(LEGENDLIST.end(),dummlr); \
    fprintf(mpf,"beginfig(%d);\n",sfig); \
    fprintf(mpf,"clean_legend_box;\n"); \
    sprintf(texb.get_buffer(),"data.%d",sfig); \
    LEGENDITEM->fname = texb.get_buffer(); \
    LEGENDITEM->name = thlegend_u2string(unsigned(symn++)); \
    LEGENDITEM->descr = txt; \
    sfig++;
  
#define endfig \
    fprintf(mpf,"draw_legend_box;\nendfig;\n"); \
  }

#define legend_point(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s((0.5,0.5) inscale,0.0,1.0,(0,0));\n",thsymbolset__mp[mid]);  \
  endfig;

#define legend_hpoint(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s((0.5,0.5) inscale,270.0,1.0,(0,0));\n",thsymbolset__mp[mid]);  \
  endfig;


  // meracie body + polygon + station-name
#define legend_station(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s((0.5,0.5) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
  
  legend_station(SYMP_STATION_TEMPORARY,thT("point station:temporary",layout->lang));
  legend_station(SYMP_STATION_PAINTED,thT("point station:painted",layout->lang));
  legend_station(SYMP_STATION_NATURAL,thT("point station:natural",layout->lang));
  legend_station(SYMP_STATION_FIXED,thT("point station:fixed",layout->lang));

#define insert_station(x,y) \
  helpsymbol \
  if isused(SYMP_STATION_TEMPORARY) \
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
  
  insfig(SYML_SURVEY,thT("line survey",layout->lang));
  fprintf(mpf,"%s(((-1,1) -- (0.8,0.6) -- (0,-1)) inscale);\n", thsymbolset__mp[SYML_SURVEY]);
  insert_station(0.8,0.6);
  endfig;

  insfig(SYMP_STATIONNAME,thT("point station-name",layout->lang));
  fprintf(mpf,"p_label.urt(btex \\thstationname %s etex,((0.3,0.3) inscale),0,7);\n",utf2tex("173"));
  insert_station(0.3,0.3);
  endfig;

  legend_hpoint(SYMP_ENTRANCE,thT("point entrance",layout->lang));

  insfig(SYML_ARROW,thT("line arrow",layout->lang));
  fprintf(mpf,"%s(((0.2,0.8) -- (0.8,0.2)) inscale,2)",thsymbolset__mp[SYML_ARROW]);
  endfig;


  // steny + wall-altitude + altitude
#define legend_wall(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s(((-.3,0.5) .. (.3,.3) .. (.7,.7) .. (1.3,.5)) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
  
  legend_wall(SYML_WALL_BEDROCK,thT("line wall:bedrock",layout->lang));
  legend_wall(SYML_WALL_UNDERLYING,thT("line wall:underlying",layout->lang));
  legend_wall(SYML_WALL_UNSURVEYED,thT("line wall:unsurveyed",layout->lang));
  legend_wall(SYML_WALL_PRESUMED,thT("line wall:presumed",layout->lang));
  legend_wall(SYML_WALL_BLOCKS,thT("line wall:blocks",layout->lang));
  legend_wall(SYML_WALL_DEBRIS,thT("line wall:debris",layout->lang));
  legend_wall(SYML_WALL_PEBBLES,thT("line wall:pebbles",layout->lang));
  legend_wall(SYML_WALL_SAND,thT("line wall:sand",layout->lang));
  legend_wall(SYML_WALL_CLAY,thT("line wall:clay",layout->lang));
  legend_wall(SYML_WALL_ICE,thT("line wall:ice",layout->lang));

  insfig(SYMP_WALLALTITUDE,thT("point wall-altitude",layout->lang));
  helpsymbol;
  if isused(SYML_WALL_BEDROCK) 
    fprintf(mpf,"%s(((-.3,0.5) .. controls (.2,.6) and (.2,.6) .. (.3,.7) .. controls (.4,.8) and (.4,.8) .. (.5,1.4)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
  endhelpsymbol;
  fprintf(mpf,"%s((0.2,0.6) inscale,(0.3,0.7) inscale,(0.4,0.8) inscale,btex \\thwallaltitude %s etex);\n",thsymbolset__mp[SYMP_WALLALTITUDE],utf2tex("1510"));
  endfig;
  
  insfig(SYMP_ALTITUDE,thT("point altitude",layout->lang));
  fprintf(mpf,"p_label.rt(btex \\thaltitude %s etex,((0.3,0.5) inscale),0,1);\n",utf2tex("1510"));
  endfig;


  insfig(SYML_SECTION,thT("line section",layout->lang));
  helpsymbol;
  if isused(SYML_WALL_BEDROCK) {
    fprintf(mpf,"%s(((.25,1.0) .. (.2,.5){dir 270} .. (.15,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
    fprintf(mpf,"%s(((.3,0.0) .. (.4,.5){dir 90} .. (.5,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);
    if isused(SYMP_SECTION) {
      fprintf(mpf,"%s(((.7,.5){dir 90} .. (.9,.75){dir 270} .. (.8,.15){dir 235} .. cycle) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]);    
    }
  }    
  endhelpsymbol;
  fprintf(mpf,"%s(((0,.5) .. controls (.195,.5) and (.405,.5) .. (.6,.5)) inscale,1);\n",thsymbolset__mp[SYML_SECTION]);
  endfig;


  // vysky chodby
#define insert_big_passage \
  helpsymbol; \
  if isused(SYML_WALL_BEDROCK) { \
    fprintf(mpf,"%s(((.35,1.0) .. (.3,.5){dir 270} .. (.25,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.6,0.0) .. (.7,.5){dir 90} .. (.8,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;

#define insert_big_water_passage \
  helpsymbol; \
  if isused(SYML_WALL_BEDROCK) { \
    fprintf(mpf,"%s(((.35,1.0) .. (.3,.5){dir 270} .. (.25,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.6,0.0) .. (.7,.5){dir 90} .. (.8,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;



  insfig(SYMP_PASSAGEHEIGHT_UNSIGNED,thT("point passage-height:unsigned",layout->lang));
  //insert_big_passage
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,5);\n",utf2tex("5"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_POSITIVE,thT("point passage-height:positive",layout->lang));
  //insert_big_water_passage
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,2);\n",utf2tex("3"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_NEGATIVE,thT("point passage-height:negative",layout->lang));
  //insert_big_water_passage
  fprintf(mpf,"p_label(btex \\thframed %s etex,((0.5,0.5) inscale),0,3);\n",utf2tex("2"));
  endfig;

  insfig(SYMP_PASSAGEHEIGHT_BOTH,thT("point passage-height:both",layout->lang));
  //insert_big_water_passage
  fprintf(mpf,"p_label(btex \\thframed \\updown{%s}",utf2tex("3"));
  fprintf(mpf,"{%s} etex,((0.5,0.5) inscale),0,4);\n",utf2tex("2"));
  endfig;

  legend_hpoint(SYMP_AIRDRAUGHT,thT("point air-draught",layout->lang));

  thdate d;
  d.parse("1999.12.31");
  insfig(SYMP_DATE,thT("point date",layout->lang));
  //insert_big_water_passage
  fprintf(mpf,"p_label(btex \\thdate %s etex,((0.5,0.5) inscale),0,0);\n",utf2tex(d.get_str(TT_DATE_FMT_UTF8_ISO)));
  endfig;

  
  // ukoncenia
#define legend_end(mid,txt) \
  insfig(mid,txt); \
  helpsymbol; \
  if isused(SYML_WALL_BEDROCK) {\
    fprintf(mpf,"%s(((0,.2){dir 30} .. {dir 0}(.5,.4)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.5,.6){dir 180} .. {dir 210}(0,.8)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol; \
  fprintf(mpf,"%s((.6,.5) inscale,270.0,1.0,(0,1));\n",thsymbolset__mp[mid]);  \
  endfig;
  
  legend_end(SYMP_CONTINUATION,thT("point continuation",layout->lang))
  legend_end(SYMP_NARROWEND,thT("point narrow-end",layout->lang))
  legend_end(SYMP_LOWEND,thT("point low-end",layout->lang))
  legend_end(SYMP_FLOWSTONECHOKE,thT("point flowstone-choke",layout->lang))
  legend_end(SYMP_BREAKDOWNCHOKE,thT("point breakdown-choke",layout->lang))
  
#define legend_iuline "(((.1,.6) .. (.5,.4) .. (.9,.6)) inscale)"
#define legend_cline "(((.85,.5){dir 90} .. (.5,.85){dir 180} .. (.4,.15){dir 0} .. cycle) inscale)"
#define legend_scline "(((.5,.5){dir 90} .. (.3,.75){dir 180} .. (.15,.5) .. (.2,.25){dir 0} .. cycle) inscale)"

#define legend_step(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s(%s);\n",thsymbolset__mp[mid],legend_iuline); \
  endfig;
#define legend_cycle(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s(%s);\n",thsymbolset__mp[mid],legend_cline); \
  endfig;
#define insert_small_passage \
  helpsymbol; \
  if isused(SYML_WALL_BEDROCK) { \
    fprintf(mpf,"%s(((.2,1.0) .. (.15,.5){dir 270} .. (.1,0.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
    fprintf(mpf,"%s(((.4,0.0) .. (.5,.5){dir 90} .. (.6,1.0)) inscale);\n",thsymbolset__mp[SYML_WALL_BEDROCK]); \
  } \
  endhelpsymbol;
  
  // zrazy + kominy + priepasti + gradient
  legend_step(SYML_FLOORSTEP,thT("line floor-step",layout->lang));
  legend_step(SYML_OVERHANG,thT("line overhang",layout->lang));
  legend_cycle(SYML_PIT,thT("line pit",layout->lang));
  legend_step(SYML_CEILINGSTEP,thT("line ceiling-step",layout->lang));
  legend_cycle(SYML_CHIMNEY,thT("line chimney",layout->lang));
  if isused(SYML_GRADIENT) {
    insfig(SYML_GRADIENT,thT("line gradient",layout->lang));
    fprintf(mpf,"%s(((0.2,0.5) -- (0.8,0.5)) inscale);\n",thsymbolset__mp[SYML_GRADIENT]);
    endfig;
  } else {
    legend_hpoint(SYMP_GRADIENT,thT("point gradient",layout->lang));
  }

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
  fprintf(mpf,"p_label.rt(btex \\thheight %s etex,((0.5,0.5) inscale),0,7);\n",utf2tex("4"));
  endfig;

  insfig(SYMP_HEIGHT_POSITIVE,thT("point height:positive",layout->lang));
  helpsymbol;
  if isused(SYML_CHIMNEY)
    fprintf(mpf,"%s(%s);\n",thsymbolset__mp[SYML_CHIMNEY],legend_scline);
  endhelpsymbol;
  fprintf(mpf,"p_label.rt(btex \\thheightpos %s etex,((0.5,0.5) inscale),0,7);\n",utf2tex("15"));
  endfig;

  insfig(SYMP_HEIGHT_NEGATIVE,thT("point height:negative",layout->lang));
  helpsymbol;
  if isused(SYML_PIT)
    fprintf(mpf,"%s(%s);\n",thsymbolset__mp[SYML_PIT],legend_scline);
  endhelpsymbol;
  fprintf(mpf,"p_label.rt(btex \\thheightneg %s etex,((0.5,0.5) inscale),0,7);\n",utf2tex("30"));
  endfig;

  insfig(SYML_CONTOUR,thT("line contour",layout->lang));
  fprintf(mpf,"%s(%s,-1);\n",thsymbolset__mp[SYML_CONTOUR],legend_iuline);
  endfig;

  insfig(SYML_SLOPE,thT("line slope",layout->lang));
  fprintf(mpf,"%s((((.1,.35) .. (.5,.25) .. (.9,.35)) inscale),1,(0,-1,1u),(2,-1,1u));\n",thsymbolset__mp[SYML_SLOPE]);
  endfig;
  
  // kamene
  insfig(SYML_ROCKBORDER,thT("line rock-border",layout->lang));
  fprintf(mpf,"%s(((.16,.36) -- (.61,.21) -- (.91,.46) -- (.84,.78) -- (.38,.86) -- (.20,.55) -- cycle) inscale)",thsymbolset__mp[SYML_ROCKBORDER]);
  endfig;

  insfig(SYML_ROCKEDGE,thT("line rock-edge",layout->lang));
  helpsymbol;
  if isused(SYML_ROCKBORDER)
    fprintf(mpf,"%s(((.16,.36) -- (.61,.21) -- (.91,.46) -- (.84,.78) -- (.38,.86) -- (.20,.55) -- cycle) inscale)",thsymbolset__mp[SYML_ROCKBORDER]);
  endhelpsymbol;
  fprintf(mpf,"%s(((.16,.36) -- (.42,.62) -- (.38,.86) -- (.42,.62) -- (.6,.55) -- (.61,.21) -- (.6,.55) -- (.84,.78)) inscale)",thsymbolset__mp[SYML_ROCKEDGE]);
  endfig;

  // vypln bodova
  legend_point(SYMP_BEDROCK,thT("point bedrock",layout->lang));
  legend_point(SYMP_BLOCKS,thT("point blocks",layout->lang));
  legend_point(SYMP_DEBRIS,thT("point debris",layout->lang));
  legend_point(SYMP_SAND,thT("point sand",layout->lang));
  legend_point(SYMP_CLAY,thT("point clay",layout->lang));
  legend_point(SYMP_WATER,thT("point water",layout->lang));
  legend_point(SYMP_ICE,thT("point ice",layout->lang));
  legend_point(SYMP_PEBBLES,thT("point pebbles",layout->lang));
  legend_point(SYMP_RAFT,thT("point raft",layout->lang));
  legend_point(SYMP_GUANO,thT("point guano",layout->lang));
  
  // okraje
  legend_cycle(SYML_BORDER_VISIBLE,thT("line border:visible",layout->lang));
  legend_cycle(SYML_BORDER_TEMPORARY,thT("line border:temporary",layout->lang));

  // vypln plosna
#define legend_area(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s(buildcycle((((-1,0) -- (1,0) -- (1,1) -- (0,1) -- (0,-1))  inscale)));\n",thsymbolset__mp[mid]); \
  endfig;
  legend_area(SYMA_WATER,thT("area water",layout->lang));  
  legend_area(SYMA_SUMP,thT("area sump",layout->lang));  
  legend_area(SYMA_DEBRIS,thT("area debris",layout->lang));  
  legend_area(SYMA_SAND,thT("area sand",layout->lang));  
  
  // vodne toky (ciary, body)
#define legend_waterflow(mid,txt) \
  insfig(mid,txt); \
  fprintf(mpf,"%s(((0.2,0.8) -- (0.8,0.2)) inscale);\n",thsymbolset__mp[mid]);  \
  endfig;
  legend_hpoint(SYMP_WATERFLOW_PERMANENT,thT("point waterflow:permanent",layout->lang));
  legend_hpoint(SYMP_WATERFLOW_INTERMITTENT,thT("point waterflow:intermittent",layout->lang));
  legend_hpoint(SYMP_WATERFLOW_PALEO,thT("point waterflow:paleo",layout->lang));
  legend_waterflow(SYML_WATERFLOW_PERMANENT,thT("line waterflow:permanent",layout->lang));
  legend_waterflow(SYML_WATERFLOW_INTERMITTENT,thT("line waterflow:intermittent",layout->lang));
  legend_waterflow(SYML_WATERFLOW_CONJECTURAL,thT("line waterflow:conjectural",layout->lang));

  insfig(SYMP_SPRING,thT("point spring",layout->lang));
  helpsymbol;
  if isused(SYML_WATERFLOW_PERMANENT)
    fprintf(mpf,"%s(((0.3,0.5) -- (0.9,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_PERMANENT]);
  else
  if isused(SYMP_WATERFLOW_PERMANENT)
    fprintf(mpf,"%s((0.3,0.5) inscale,270,1.0,(0,1));\n",thsymbolset__mp[SYMP_WATERFLOW_PERMANENT]);
  else
  if isused(SYML_WATERFLOW_INTERMITTENT)
    fprintf(mpf,"%s(((0.3,0.5) -- (0.9,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_INTERMITTENT]);
  else
  if isused(SYMP_WATERFLOW_INTERMITTENT)
    fprintf(mpf,"%s((0.3,0.5) inscale,270,1.0,(0,1));\n",thsymbolset__mp[SYMP_WATERFLOW_INTERMITTENT]);
  endhelpsymbol;
  fprintf(mpf,"%s((0.3,0.5) inscale,270,1.0,(0,-1));\n",thsymbolset__mp[SYMP_SPRING]);
  endfig;

  insfig(SYMP_SINK,thT("point sink",layout->lang));
  helpsymbol;
  if isused(SYML_WATERFLOW_PERMANENT)
    fprintf(mpf,"%s(((0.1,0.5) -- (0.7,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_PERMANENT]);
  else
  if isused(SYMP_WATERFLOW_PERMANENT)
    fprintf(mpf,"%s((0.7,0.5) inscale,270,1.0,(0,-1));\n",thsymbolset__mp[SYMP_WATERFLOW_PERMANENT]);
  else
  if isused(SYML_WATERFLOW_INTERMITTENT)
    fprintf(mpf,"%s(((0.1,0.5) -- (0.7,0.5)) inscale);\n",thsymbolset__mp[SYML_WATERFLOW_INTERMITTENT]);
  else
  if isused(SYMP_WATERFLOW_INTERMITTENT)
    fprintf(mpf,"%s((0.7,0.5) inscale,270,1.0,(0,-1));\n",thsymbolset__mp[SYMP_WATERFLOW_INTERMITTENT]);
  endhelpsymbol;
  fprintf(mpf,"%s((0.7,0.5) inscale,270,1.0,(0,1));\n",thsymbolset__mp[SYMP_SINK]);
  endfig;

    
  // vyzdoba
  legend_point(SYMP_FLOWSTONE,thT("point flowstone",layout->lang));
  legend_step(SYML_FLOWSTONE,thT("line flowstone",layout->lang));
  legend_point(SYMP_MOONMILK,thT("point moonmilk",layout->lang));
  legend_point(SYMP_STALACTITE,thT("point stalactite",layout->lang));
  legend_point(SYMP_STALAGMITE,thT("point stalagmite",layout->lang));
  legend_point(SYMP_PILLAR,thT("point pillar",layout->lang));
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

  // vystroj  
  legend_point(SYMP_NOEQUIPMENT,thT("point no-equipment",layout->lang));
  legend_point(SYMP_ANCHOR,thT("point anchor",layout->lang));
  legend_point(SYMP_ROPE,thT("point rope",layout->lang));
  legend_point(SYMP_ROPELADDER,thT("point rope-ladder",layout->lang));
  legend_point(SYMP_FIXEDLADDER,thT("point fixed-ladder",layout->lang));
  legend_point(SYMP_STEPS,thT("point steps",layout->lang));
  legend_point(SYMP_TRAVERSE,thT("point traverse",layout->lang));
  legend_point(SYMP_BRIDGE,thT("point bridge",layout->lang));
  legend_point(SYMP_CAMP,thT("point camp",layout->lang));


  // prejde vsetky ostatne objekty
  for (int m = 0; m < SYMX_; m++) {

    if (!this->assigned[m])
      continue;
    if (!(this->used[m] || (layout->legend == TT_LAYOUT_LEGEND_ALL)))
      continue;
    if (!isin[m])
      continue;

    switch (m) {

      // najprv tie, ktore nejdu do preview
      case SYMP_SECTION:
      case SYMP_UNDEFINED:

      case SYML_BORDER_INVISIBLE:
      case SYML_WALL_INVISIBLE:
      case SYML_UNDEFINED:
  
      // potom specialne
      case SYMP_LABEL:
      case SYMP_REMARK:
      case SYMP_STATIONNAME:
      case SYMP_HEIGHT_POSITIVE:
      case SYMP_HEIGHT_NEGATIVE:
      case SYMP_HEIGHT_UNSIGNED:
      case SYMP_DATE:
      case SYMP_ALTITUDE:
      case SYMP_WALLALTITUDE:
      case SYMP_PASSAGEHEIGHT_POSITIVE:
      case SYMP_PASSAGEHEIGHT_NEGATIVE:
      case SYMP_PASSAGEHEIGHT_UNSIGNED:
      case SYMP_PASSAGEHEIGHT_BOTH:
      
      case SYML_LABEL:
      case SYML_SECTION:
      case SYML_CONTOUR:
      case SYML_SLOPE:
      case SYML_ARROW:

      case SYML_WALL_PEBBLES:
      case SYML_WALL_CLAY:
      case SYML_WALL_ICE:

      case SYMS_NORTHARROW:
      case SYMS_SCALEBAR:

        break;
        
//      default:
//        // zapise defaultne preview pre bod/krivku/mapu
//        insfig(m,thsymbolset__mp[m]);
//        LEGENDITEM->descr = thsymbolset__mp[m];
//        if ((m > SYMP_) && (m < SYMP_ZZZ)) {
//          fprintf(mpf,"legend_point(\"%s\");\n",thsymbolset__mp[m]);
//        } else if ((m > SYML_) && (m < SYML_ZZZ)) {
//          fprintf(mpf,"legend_line(\"%s\");\n",thsymbolset__mp[m]);
//        } else if ((m > SYMA_) && (m < SYMA_ZZZ)) {
//          fprintf(mpf,"%s(buildcycle((((-1,0) -- (1,0) -- (1,1) -- (0,1) -- (0,-1))  inscale)));\n",thsymbolset__mp[m]);
//        }
//        endfig;
    }
  }
  
}
