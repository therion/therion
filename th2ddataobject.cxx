/**
 * @file th2ddataobject.cxx
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
 
#include "th2ddataobject.h"
#include "thexception.h"
#include "thsymbolset.h"
#include "thchenc.h"

th2ddataobject::th2ddataobject()
{
  this->pscrapoptr = NULL;
  this->nscrapoptr = NULL;
  this->fscrapptr = NULL;
  this->scale = TT_2DOBJ_SCALE_M;
  this->scale_numeric = 1.0;
  this->tags = TT_2DOBJ_TAG_CLIP_AUTO | TT_2DOBJ_TAG_VISIBILITY_ON;
  this->place = TT_2DOBJ_PLACE_DEFAULT;
  this->context = -1;
  this->m_subtype_str = NULL;
}


th2ddataobject::~th2ddataobject()
{
}


int th2ddataobject::get_class_id() 
{
  return TT_2DDATAOBJECT_CMD;
}


bool th2ddataobject::is(int class_id)
{
  if (class_id == TT_2DDATAOBJECT_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}



thcmd_option_desc th2ddataobject::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_2ddataobject_opt);
  if (id == TT_2DOBJ_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else switch(id) {
    case TT_2DOBJ_CONTEXT:
      return thcmd_option_desc(id,2);
    default:
      return thcmd_option_desc(id);
  }
}


void th2ddataobject::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  int i;
  switch (cod.id) {
  
    case TT_2DOBJ_SCALE:
      th2dparse_scale(*args, this->scale, this->scale_numeric);
      if (this->get_class_id() == TT_AREA_CMD)
        ththrow(("scale specification for area not allowed"))
      break;    

    case TT_2DOBJ_CLIP:
      i = thmatch_token(*args, thtt_onoffauto);
      this->tags &= ~(TT_2DOBJ_TAG_CLIP_ON | TT_2DOBJ_TAG_CLIP_AUTO);
      switch (i) {
        case TT_TRUE:
          this->tags |= TT_2DOBJ_TAG_CLIP_ON;
          break;
        case TT_FALSE:
          break;
        case TT_AUTO:
          this->tags |= TT_2DOBJ_TAG_CLIP_AUTO;
          break;
        default:
          ththrow(("invalid clip -- %s",*args))
      }
      break;    

    case TT_2DOBJ_VISIBILITY:
      i = thmatch_token(*args, thtt_bool);
      this->tags &= ~TT_2DOBJ_TAG_VISIBILITY_ON;
      switch (i) {
        case TT_TRUE:
          this->tags |= TT_2DOBJ_TAG_VISIBILITY_ON;
          break;
        case TT_FALSE:
          break;
        default:
          ththrow(("invalid visibility switch -- %s",*args))
      }
      break;    

    case TT_2DOBJ_PLACE:
      this->place = thmatch_token(*args, thtt_2ddataobject_place);
      if (this->place == TT_2DOBJ_PLACE_UNKNOWN)
        ththrow(("invalid place value -- %s",*args))
      break;    
      
    case TT_2DOBJ_CONTEXT:
      this->context = thsymbolset__get_id(args[0], args[1]);
      if (this->context < 0)
        ththrow(("invalid object context -- %s %s", args[0], args[1]))
      if ((this->context > SYMP_ZZZ) 
         && (this->context != SYMX_POINT_AIRDRAUGHT)
         && (this->context != SYMX_POINT_HEIGHT)
         && (this->context != SYMX_POINT_PASSAGEHEIGHT)
         && (this->context != SYMX_POINT_STATION)
         && (this->context != SYMX_POINT_WATERFLOW)
         && (this->context != SYMX_LINE_BORDER)
         && (this->context != SYMX_LINE_SURVEY)
         && (this->context != SYMX_LINE_WALL)
         && (this->context != SYMX_LINE_WATERFLOW))
        ththrow(("object context not allowed -- %s %s", args[0], args[1]))
      break;

    default:
      thdataobject::set(cod, args, argenc, indataline);
      
  }
}


int th2ddataobject::get_context()
{
  return THCTX_SCRAP;
}


void th2ddataobject::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"th2ddataobject:\n");
  fprintf(outf,"\tscale: %d\n",this->scale);
  fprintf(outf,"\ttags: %d\n",this->tags);
  // insert intended print of object properties here
}

thbuffer sTTtype, sTTsubtype;
void th2dsplitTT(char * src, char ** type, char ** subtype)
{
  size_t sl, x, tl, stl;
  char * t, * st;
  sl = strlen(src);
  sTTtype.guarantee(sl+1);
  sTTsubtype.guarantee(sl+1);
  t = sTTtype.get_buffer();
  st = sTTsubtype.get_buffer();
  t[0] = 0;
  st[0] = 0;
  tl = 0;
  stl = 0;
  bool inst = false;
  for(x = 0; x < sl; x++) {
    if (src[x] == ':') {
      inst = true;
    } else if (inst) {
      st[stl] = src[x];
      stl++;
      st[stl] = 0;
    } else {
      t[tl] = src[x];
      tl++;
      t[tl] = 0;
    }
  }
  *type = t;
  *subtype = st;
}



void th2ddataobject::parse_u_subtype(const char * subtype)
{
  if (th_is_keyword(subtype)) {
    this->m_subtype_str = this->db->strstore(subtype, true);
  } else
    ththrow(("invalid subtype name -- %s", subtype))
}

void th2dparse_scale(const char * spec, int & type, double & value)
{
  type = thmatch_token(spec, thtt_2dobj_scales);
  if (type == TT_2DOBJ_SCALE_UNKNOWN) {
    // try to parse numeric value
    thparse_double(type, value, spec);
    if ((type != TT_SV_NUMBER) || (value <= 0.0)) {
      ththrow(("invalid scale -- %s", spec));
    } else {
      type = TT_2DOBJ_SCALE_NUMBERIC;
    }
  } else {
    switch(type) {
      case TT_2DOBJ_SCALE_XS:
        value = 0.5;
        break;
      case TT_2DOBJ_SCALE_S:
        value = 0.707;
        break;
      case TT_2DOBJ_SCALE_M:
        value = 1.0;
        break;
      case TT_2DOBJ_SCALE_L:
        value = 1.414;
        break;
      case TT_2DOBJ_SCALE_XL:
        value = 2.0;
        break;
    }
  }
}
