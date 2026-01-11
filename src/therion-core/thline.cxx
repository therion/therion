/**
 * @file thline.cxx
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */

#include "thline.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdb2dlp.h"
#include "thexpmap.h"
#include "thtflength.h"
#include "thscrap.h"
#include "thdatabase.h"

#include <fmt/core.h>

thline::thline()
{
  this->type = TT_LINE_TYPE_UNKNOWN;
  this->outline = TT_LINE_OUTLINE_NONE;
  this->closed = TT_AUTO;
  this->reverse = false;
  this->is_closed = false;

  this->csubtype = TT_LINE_SUBTYPE_UNKNOWN;

  this->first_point = NULL;
  this->last_point = NULL;

  this->text = NULL;
  this->m_height = thnan;
}


thline::~thline()
{
}


int thline::get_class_id()
{
  return TT_LINE_CMD;
}


bool thline::is(int class_id)
{
  if (class_id == TT_LINE_CMD)
    return true;
  else
    return th2ddataobject::is(class_id);
}

int thline::get_cmd_nargs()
{
  return 1;
}


const char * thline::get_cmd_end()
{
  return "endline";
}


const char * thline::get_cmd_name()
{
  return "line";
}


thcmd_option_desc thline::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_line_opt);
  if (id == TT_LINE_UNKNOWN)
    return th2ddataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thline::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  int reversion, csmooth, sv;
  char * type, * subtype;
  if (cod.id == 1)
    cod.id = TT_LINE_TYPE;

  switch (cod.id) {

    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->insert_line_point(this->db->db2d.mbf.get_size(),
        this->db->db2d.mbf.get_buffer());
      break;

    case TT_LINE_TYPE:
      th2dsplitTT(*args, &type, &subtype);
      this->parse_type(type);
      if (strlen(subtype) > 0)
        this->parse_subtype(subtype);
      break;

    case TT_LINE_OUTLINE:
      this->outline = thmatch_token(*args,thtt_line_outlines);
      if (this->outline == TT_LINE_OUTLINE_UNKNOWN)
        throw thexception(fmt::format("unknown line outline -- {}",*args));
      break;

    case TT_LINE_CLOSED:
      this->closed = thmatch_token(*args,thtt_onoffauto);
      if (this->closed == TT_UNKNOWN_BOOL)
        throw thexception(fmt::format("invalid closure switch -- {}",*args));
      break;

    case TT_LINE_REVERSE:
      reversion = thmatch_token(*args,thtt_bool);
      if (reversion == TT_UNKNOWN_BOOL)
        throw thexception(fmt::format("invalid reversion switch -- {}",*args));
      this->reverse = (reversion == TT_TRUE);
      break;

    case TT_LINE_SUBTYPE:
      this->parse_subtype(*args);
      break;

	case TT_LINE_HEIGHT:
	  this->parse_height(*args);
	  break;

    case TT_LINE_BORDER:
      this->parse_border(*args);
      break;

    case TT_LINE_GRADIENT:
      this->parse_gradient(*args);
      break;

    case TT_LINE_DIRECTION:
      this->parse_direction(*args);
      break;

    case TT_LINE_HEAD:
      this->parse_head(*args);
      break;

    case TT_LINE_ADJUST:
      this->parse_adjust(*args);
      break;

    case TT_LINE_ALTITUDE:
      this->parse_altitude(*args);
      break;

    case TT_LINE_TEXT:
      thencode(&(this->db->buff_enc), *args, argenc);
      this->parse_text(this->db->buff_enc.get_buffer());
      break;

    case TT_LINE_SMOOTH:
      csmooth = thmatch_token(*args,thtt_onoffauto);
      if (csmooth == TT_UNKNOWN_BOOL)
        throw thexception(fmt::format("invalid smooth switch -- {}",*args));
      if (this->last_point != NULL) {
        this->last_point->smooth = csmooth;
        this->last_point->smooth_orig = csmooth;
      }
      else
        throw thexception("no line point specified");
      break;

    case TT_LINE_ORIENT:
      if (this->last_point != NULL) {
        if (this->type != TT_LINE_TYPE_SLOPE)
          throw thexception(fmt::format("orientation not valid with type {}", thmatch_string(this->type,thtt_line_types)));
        thparse_double(sv,this->last_point->orient,*args);
        if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
          throw thexception(fmt::format("invalid number -- {}",*args));
        if ((this->last_point->orient < 0.0) || (this->last_point->orient >= 360.0))
          throw thexception(fmt::format("orientation out of range -- {}",*args));
        this->last_point->tags |= TT_LINEPT_TAG_ORIENT;
      }
      else
        throw thexception("no line point specified");
      break;

    case TT_LINE_SIZE:
    case TT_LINE_RSIZE:
    case TT_LINE_LSIZE:
      this->parse_size(cod.id,*args);
      break;

    case TT_LINE_MARK:
      this->insert_point_mark(*args);
      break;

    case TT_LINE_ANCHORS:
    case TT_LINE_REBELAYS:
      if (this->type != TT_LINE_TYPE_ROPE)
        throw thexception(fmt::format("-anchors and -rebelays not valid with type {}", thmatch_string(this->type,thtt_line_types)));
      int flag, tagtype;
      flag = thmatch_token(*args,thtt_bool);
      if (flag == TT_UNKNOWN_BOOL)
        throw thexception(fmt::format("logical value expected -- {}",*args));
      tagtype = (cod.id == TT_LINE_ANCHORS) ? TT_LINE_TAG_ROPE_ANCHORS : TT_LINE_TAG_ROPE_REBELAYS;
      if (flag == TT_TRUE)
        this->tags |= tagtype;
      else
        this->tags &= ~tagtype;
      break;

    // if not found, try to set fathers properties
    default:
      th2ddataobject::set(cod, args, argenc, indataline);
  }
}


void thline::self_print_properties(FILE * outf)
{
  th2ddataobject::self_print_properties(outf);
  fprintf(outf,"thline:\n");
  fprintf(outf,"\ttype: %d\n", this->type);
  fprintf(outf,"\toutline: %d\n", this->outline);
  fprintf(outf,"\tclosed: %d\n", this->closed);
  fprintf(outf,"\treverse: %d\n", this->reverse);
  if (this->last_point != NULL) {
    fprintf(outf,"\tpoints:\n");
    thdb2dlp * cpt = this->first_point;
    while(cpt != NULL) {
      fprintf(outf,"\t");
      if (cpt->cp1 != NULL)
        fprintf(outf,"\t -- %f,%f (%f,%f,%f,%f)\n\t",cpt->cp1->x, cpt->cp1->y,
          cpt->cp1->xt, cpt->cp1->yt, cpt->cp1->zt, cpt->cp1->at);
      if (cpt->cp2 != NULL)
        fprintf(outf,"\t -- %f,%f (%f,%f,%f,%f)\n\t",cpt->cp2->x, cpt->cp2->y, cpt->cp2->xt,
          cpt->cp2->yt, cpt->cp2->zt, cpt->cp2->at);
      fprintf(outf,"\t%f,%f (%f,%f,%f,%f)",cpt->point->x, cpt->point->y,
        cpt->point->xt, cpt->point->yt, cpt->point->zt, cpt->point->at);
      fprintf(outf,"\t(subtype:%d smooth:%d orient:%f r-size:%f l-size:%f)\n",cpt->subtype,cpt->smooth,
        cpt->orient, cpt->rsize, cpt->lsize);
      cpt = cpt->nextlp;
    }
  }
  // insert intended print of object properties here
}


void thline::parse_type(char * ss)
{
  this->type = thmatch_token(ss,thtt_line_types);
  switch (this->type) {
    case TT_LINE_TYPE_UNKNOWN:
      throw thexception(fmt::format("unknown line type -- {}",ss));
      break;
    case TT_LINE_TYPE_WALL:
      this->csubtype = TT_LINE_SUBTYPE_BEDROCK;
      this->outline = TT_LINE_OUTLINE_OUT;
      break;
    case TT_LINE_TYPE_BORDER:
      this->csubtype = TT_LINE_SUBTYPE_VISIBLE;
      break;
    case TT_LINE_TYPE_WATER_FLOW:
      this->csubtype = TT_LINE_SUBTYPE_PERMANENT;
      break;
    case TT_LINE_TYPE_SURVEY:
      this->csubtype = TT_LINE_SUBTYPE_CAVE;
      break;
    case TT_LINE_TYPE_ARROW:
      this->tags |= TT_LINE_TAG_HEAD_END;
      break;
    case TT_LINE_TYPE_CHIMNEY:
      this->place = TT_2DOBJ_PLACE_DEFAULT_TOP;
      break;
    case TT_LINE_TYPE_CEILING_STEP:
      this->place = TT_2DOBJ_PLACE_DEFAULT_TOP;
      break;
    case TT_LINE_TYPE_CEILING_MEANDER:
      this->place = TT_2DOBJ_PLACE_DEFAULT_TOP;
      break;
    case TT_LINE_TYPE_ROPE:  // show anchors and rebelays on line rope by default
      this->tags |= TT_LINE_TAG_ROPE_ANCHORS;
      this->tags |= TT_LINE_TAG_ROPE_REBELAYS;
      break;
  }
}


void thline::parse_subtype(char * ss)
{
//  int prevcsubtype = this->csubtype;
  if (this->type == TT_LINE_TYPE_U) {
    if (this->last_point != NULL)
      throw thexception("subtype specification not allowed here");
    this->parse_u_subtype(ss);
    return;
  }
  this->csubtype = thmatch_token(ss,thtt_line_subtypes);
  if (this->csubtype == TT_LINE_SUBTYPE_UNKNOWN)
    throw thexception(fmt::format("invalid line subtype -- {}",ss));
//  if ((this->last_point != NULL) && (prevcsubtype != TT_LINE_SUBTYPE_UNKNOWN)
//       && (this->csubtype != prevcsubtype)) {
//    this->throw_source();
//    threwarning2(("multiple subtypes per line are deprecated"))
//  }
  bool tsok = false;
  switch (this->type) {
    case TT_LINE_TYPE_WALL:
      switch (this->csubtype) {
        case TT_LINE_SUBTYPE_INVISIBLE:
        case TT_LINE_SUBTYPE_BEDROCK:
        case TT_LINE_SUBTYPE_SAND:
        case TT_LINE_SUBTYPE_CLAY:
        case TT_LINE_SUBTYPE_PEBBLES:
        case TT_LINE_SUBTYPE_DEBRIS:
        case TT_LINE_SUBTYPE_BLOCKS:
        case TT_LINE_SUBTYPE_ICE:
        case TT_LINE_SUBTYPE_UNDERLYING:
        case TT_LINE_SUBTYPE_UNSURVEYED:
        case TT_LINE_SUBTYPE_PRESUMED:
        case TT_LINE_SUBTYPE_OVERLYING:
        case TT_LINE_SUBTYPE_PIT:
        case TT_LINE_SUBTYPE_MOONMILK:
        case TT_LINE_SUBTYPE_FLOWSTONE:
          tsok = true;
      }
      break;
    case TT_LINE_TYPE_BORDER:
      switch (this->csubtype) {
        case TT_LINE_SUBTYPE_INVISIBLE:
        case TT_LINE_SUBTYPE_TEMPORARY:
        case TT_LINE_SUBTYPE_VISIBLE:
        case TT_LINE_SUBTYPE_PRESUMED:
          tsok = true;
      }
      break;
    case TT_LINE_TYPE_SURVEY:
      switch (this->csubtype) {
        case TT_LINE_SUBTYPE_CAVE:
        case TT_LINE_SUBTYPE_SURFACE:
          tsok = true;
      }
      break;
    case TT_LINE_TYPE_WATER_FLOW:
      switch (this->csubtype) {
        case TT_LINE_SUBTYPE_PERMANENT:
        case TT_LINE_SUBTYPE_INTERMITTENT:
        case TT_LINE_SUBTYPE_CONJECTURAL:
          tsok = true;
      }
      break;
  }
  if (!tsok)
    throw thexception("invalid line type - subtype combination");
  if (this->last_point != NULL)
    this->last_point->subtype = this->csubtype;
}



void thline::insert_line_point(int nargs, char ** args, double * nums)
{
  // check number of parameters
  if ((nargs != 6) && (nargs != 2))
    throw thexception(fmt::format("invalid number of coordinates -- {}", nargs));

  double cp1x, cp1y, cp2x, cp2y, x, y;
  int pidx = 0, sv;
  char * invs = NULL;
  bool invnum = false;

  if (nargs == 6) {
    pidx = 4;
    if (args != NULL) {
      thparse_double(sv, cp1x, args[0]);
      if (sv != TT_SV_NUMBER) {
        invs = args[0];
        invnum = true;
      }
      thparse_double(sv, cp1y, args[1]);
      if (sv != TT_SV_NUMBER) {
        invs = args[1];
        invnum = true;
      }
      thparse_double(sv, cp2x, args[2]);
      if (sv != TT_SV_NUMBER) {
        invs = args[2];
        invnum = true;
      }
      thparse_double(sv, cp2y, args[3]);
      if (sv != TT_SV_NUMBER) {
        invs = args[3];
        invnum = true;
      }
    } else {
      cp1x = nums[0];
      cp1y = nums[1];
      cp2x = nums[2];
      cp2y = nums[3];
    }
  }
  if (args != NULL) {
    thparse_double(sv, x, args[pidx]);
    if (sv != TT_SV_NUMBER) {
      invs = args[pidx];
      invnum = true;
    }
    thparse_double(sv, y, args[pidx+1]);
    if (sv != TT_SV_NUMBER) {
      invs = args[pidx+1];
      invnum = true;
    }
  } else {
    x = nums[pidx];
    y = nums[pidx+1];
  }

  if (invnum)
    throw thexception(fmt::format("invalid number -- {}",invs));

  // let's insert point into database
  thdb2dlp * plp = this->db->db2d.insert_line_point();

  plp->subtype = this->csubtype;
  plp->smooth = TT_AUTO;
  plp->smooth_orig = TT_AUTO;

  if (this->last_point == NULL) {
    this->last_point = plp;
    this->first_point = plp;
    plp->nextlp = NULL;
    plp->prevlp = NULL;
  } else {
    this->last_point->nextlp = plp;
    plp->prevlp = this->last_point;
    plp->nextlp = NULL;
    this->last_point = plp;
  }

  thdb2dpt * cp1, * cp2, * pt;
  pt = this->db->db2d.insert_point();
  pt->x = x;
  pt->y = y;
  pt->pscrap = this->db->get_current_scrap();

  plp->point = pt;

  if (nargs == 6) {
    cp1 = this->db->db2d.insert_point();
    cp1->x = cp1x;
    cp1->y = cp1y;
    cp1->pscrap = this->db->get_current_scrap();
    cp2 = this->db->db2d.insert_point();
    cp2->x = cp2x;
    cp2->y = cp2y;
    cp2->pscrap = this->db->get_current_scrap();
    plp->cp1 = cp1;
    plp->cp2 = cp2;
  } else {
    plp->cp1 = NULL;
    plp->cp2 = NULL;
  }

}


void thline::insert_point_mark(char * ss)
{
  if (!th_is_keyword(ss))
    throw thexception(fmt::format("mark not a key word -- {}",ss));
  if (this->last_point == NULL)
    throw thexception("no line point specified");
  this->last_point->mark = this->db->strstore(ss,true);
}

thdb2dlp * thline::get_marked_station(const char * mark)
{
  if (strcmp(mark,"end") == 0) {
    if (this->reverse)
      return this->first_point;
    else
      return this->last_point;
  }
  thdb2dlp * cpt = (this->reverse ? this->last_point : this->first_point);
  if (th_is_index(mark)) {
    int index = -1, cidx;
    sscanf(mark,"%d",&index);
    cidx = 0;
    while(cpt != NULL) {
      if (cidx == index)
        return cpt;
      cidx++;
      cpt = (this->reverse ? cpt->prevlp : cpt->nextlp);
    }
  }
  cpt = (this->reverse ? this->last_point : this->first_point);
  while (cpt != NULL) {
    if (cpt->mark != NULL)
      if (strcmp(mark,cpt->mark) == 0)
        return cpt;
    cpt = (this->reverse ? cpt->prevlp : cpt->nextlp);
  }
  return NULL;
}


void thline::preprocess()
{

  if (this->first_point == NULL)
    return;

  // check closure
  this->is_closed = ((this->closed == TT_TRUE) ||
      ((this->closed == TT_AUTO) &&
      (this->first_point->point->x == this->last_point->point->x) &&
      (this->first_point->point->y == this->last_point->point->y)));
  if (this->is_closed) {
      this->last_point->point->x = this->first_point->point->x;
      this->last_point->point->y = this->first_point->point->y;
  }

  // check reversion
  thdb2dlp * c_item, * n_item, * t_item;
  thdb2dpt * t_point;
  int t_tags, t_subtype, t_smooth, t_smooth_orig;
  double t_rsize, t_lsize, t_orient;

  bool tmpreverse = this->reverse;
  if (this->fscrapptr->flip != TT_SCRAP_FLIP_NONE) {
	switch (this->type) {
	  case TT_LINE_TYPE_GRADIENT:
	  case TT_LINE_TYPE_WATER_FLOW:
	  case TT_LINE_TYPE_ARROW:
		break;
	  default:
		tmpreverse = !tmpreverse;
  	}
  }

  if (tmpreverse && (this->first_point->nextlp != NULL)) {

    // najprv prehodi podtypy
    c_item = this->first_point;
    while (c_item != NULL) {

      n_item = c_item->nextlp;

      // switch next - prev
      t_item = c_item->nextlp;
      c_item->nextlp = c_item->prevlp;
      c_item->prevlp = t_item;

      // switch control points
      t_point = c_item->cp1;
      c_item->cp1 = c_item->cp2;
      c_item->cp2 = t_point;

      c_item = n_item;

    }

    t_item = this->first_point;
    this->first_point = this->last_point;
    this->last_point = t_item;

    t_point = this->first_point->point;
    t_tags = this->first_point->tags;
    t_rsize = this->first_point->rsize;
    t_lsize = this->first_point->lsize;
    t_orient = this->first_point->orient;
    t_smooth = this->first_point->smooth;
    t_smooth_orig = this->first_point->smooth_orig;
    t_subtype = this->last_point->subtype;
    c_item = this->first_point;
    while (c_item != NULL) {
      if (c_item->nextlp != NULL) {
        c_item->point = c_item->nextlp->point;
        c_item->tags = c_item->nextlp->tags;
        c_item->lsize = c_item->nextlp->lsize;
        c_item->rsize = c_item->nextlp->rsize;
        c_item->orient = c_item->nextlp->orient;
        c_item->smooth = c_item->nextlp->smooth;
        c_item->smooth_orig = c_item->nextlp->smooth_orig;
        c_item->subtype = c_item->nextlp->subtype;
      } else {
        c_item->point = t_point;
        c_item->tags = t_tags;
        c_item->lsize = t_lsize;
        c_item->rsize = t_rsize;
        c_item->orient = t_orient;
        c_item->smooth = t_smooth;
        c_item->smooth_orig = t_smooth_orig;
        c_item->subtype = t_subtype;
      }
      c_item = c_item->nextlp;
    }

    t_item = this->last_point;
    this->last_point = t_item->prevlp;
    this->last_point->nextlp = NULL;
    t_item->prevlp = NULL;
    t_item->nextlp = this->first_point;
    this->first_point->prevlp = t_item;
    this->first_point = t_item;

    t_subtype = this->first_point->subtype;
    c_item = this->first_point;
    while (c_item != NULL) {
      if (c_item->nextlp != NULL) {
        c_item->subtype = c_item->nextlp->subtype;
      } else {
        c_item->subtype = t_subtype;
      }
      c_item = c_item->nextlp;
    }

  }

  // check smoothness
  c_item = this->first_point;
  double d1, d2, a1, a2;
  while (c_item != NULL)  {
    if (c_item->smooth == TT_AUTO) {
      c_item->smooth = TT_TRUE;
      if ((c_item->nextlp != NULL) &&
          (c_item->cp2 != NULL) &&
          (c_item->nextlp->cp1 != NULL)) {
        d1 = std::hypot(c_item->cp2->x - c_item->point->x, c_item->cp2->y - c_item->point->y);
        d2 = std::hypot(c_item->nextlp->cp1->x - c_item->point->x, c_item->nextlp->cp1->y - c_item->point->y);
        if ((d2 > 0) && (d1 > 0)) {
          a1 = atan2(c_item->cp2->y - c_item->point->y, c_item->cp2->x - c_item->point->x) / 3.14159265358979323338 * 180.0;
          a2 = atan2(c_item->point->y - c_item->nextlp->cp1->y, c_item->point->x - c_item->nextlp->cp1->x) / 3.14159265358979323338 * 180.0;
          if (a2 - a1 > 180.0)
            a2 -= 360;
          if (a2 - a1 < -180.0)
            a2 += 360;
          if ((a2 > a1 + 5.0) || (a2 < a1 - 5.0))
            c_item->smooth = TT_FALSE;
        }
      }
    }
    c_item = c_item->nextlp;
  }
}

#define thline_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;

bool thline::export_mp(class thexpmapmpxs * out)
{

  if (this->first_point == NULL)
    return(false);
  // actually not a line
  if (this->first_point == this->last_point)
	return(false);
  bool postprocess = true, todraw, fsize, frot, anypt;  //, first
  int from, to, cs, macroid = -1, omacroid = -1;
  double s1, r1;
  thdb2dlp * lp;

  th2ddataobject::export_mp(out);
  if (out->file != NULL) {
    if (thisnan(this->m_height)) {
      fprintf(out->file,"ATTR__height := -1;\n");
    } else {
      fprintf(out->file,"ATTR__height := %.2f;\n", this->m_height);
    }
  }

  if (this->scale_numeric < out->layout->min_symbol_scale) return(false);

  switch (this->type) {
    case TT_LINE_TYPE_WALL:
      from = 0;
      to = 0;
      lp = this->first_point;
      while (lp != NULL) {
        cs = lp->subtype;
        todraw = (lp->nextlp != NULL);
        while ((lp != NULL) && (lp->subtype == cs)) {
          to++;
          lp = lp->nextlp;
        }
        if (todraw) {
          macroid = SYML_UNDEFINED;
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_INVISIBLE, SYML_WALL_INVISIBLE)
            thline_type_export_mp(TT_LINE_SUBTYPE_BEDROCK, SYML_WALL_BEDROCK)
            thline_type_export_mp(TT_LINE_SUBTYPE_SAND, SYML_WALL_SAND)
            thline_type_export_mp(TT_LINE_SUBTYPE_CLAY, SYML_WALL_CLAY)
            thline_type_export_mp(TT_LINE_SUBTYPE_PEBBLES, SYML_WALL_PEBBLES)
            thline_type_export_mp(TT_LINE_SUBTYPE_DEBRIS, SYML_WALL_DEBRIS)
            thline_type_export_mp(TT_LINE_SUBTYPE_BLOCKS, SYML_WALL_BLOCKS)
            thline_type_export_mp(TT_LINE_SUBTYPE_ICE, SYML_WALL_ICE)
            thline_type_export_mp(TT_LINE_SUBTYPE_FLOWSTONE, SYML_WALL_FLOWSTONE)
            thline_type_export_mp(TT_LINE_SUBTYPE_MOONMILK, SYML_WALL_MOONMILK)
            thline_type_export_mp(TT_LINE_SUBTYPE_UNDERLYING, SYML_WALL_UNDERLYING)
            thline_type_export_mp(TT_LINE_SUBTYPE_OVERLYING, SYML_WALL_OVERLYING)
            thline_type_export_mp(TT_LINE_SUBTYPE_UNSURVEYED, SYML_WALL_UNSURVEYED)
            thline_type_export_mp(TT_LINE_SUBTYPE_PRESUMED, SYML_WALL_PRESUMED)
            thline_type_export_mp(TT_LINE_SUBTYPE_PIT, SYML_WALL_PIT)
          }
          omacroid = macroid;
          if (this->context >= 0)
            macroid = this->context;
          if (out->symset->is_assigned(macroid)) {
            if (out->file == NULL)
              return(true);
            out->symset->export_mp_symbol_options(out->file, omacroid);
            fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
            this->export_path_mp(out,from,to);
            fprintf(out->file,");\n");
            if (out->layout->is_debug_joins()) {
              fprintf(out->file,"l_debug(1,1,");
              this->export_path_mp(out,from,to,1);
              fprintf(out->file,");\n");
            }
            if (out->layout->is_debug_stations()) {
              fprintf(out->file,"l_debug(0,1,");
              this->export_path_mp(out,from,to,0);
              fprintf(out->file,");\n");
            }
          }
        }
        from = to;
      }
      postprocess = false;
      break;

    case TT_LINE_TYPE_LABEL:
      macroid = SYML_LABEL;
      omacroid = macroid;
      if (this->context >= 0)
        macroid = this->context;
      if ((this->text == NULL) || (!out->symset->is_assigned(macroid))) {
        postprocess = false;
        break;
      }
      if (out->file == NULL)
        return(true);
      out->symset->export_mp_symbol_options(out->file, omacroid);
      fprintf(out->file,"l_label(btex ");
      fprintf(out->file,"\\thlabel ");
      out->layout->export_mptex_font_size(out->file, this, true);
      //thdecode(&(this->db->buff_enc),TT_ISO8859_2,this->text);
      fprintf(out->file,"%s etex,",ths2tex(this->text, out->layout->lang, true).c_str());
      this->export_path_mp(out);
      fprintf(out->file,");\n");
      postprocess = false;
      break;
    case TT_LINE_TYPE_CONTOUR:
      macroid = SYML_CONTOUR;
      if (this->context >= 0)
        macroid = this->context;
      if (!out->symset->is_assigned(macroid)) {
        postprocess = false;
        break;
      }
      if (out->file == NULL)
        return(true);
      out->symset->export_mp_symbol_options(out->file, SYML_CONTOUR);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_CONTOUR));
      this->export_path_mp(out);
      from = 0;
      if ((this->tags & TT_LINE_TAG_GRADIENT_NONE) > 0) {
          fprintf(out->file,",-3");
      } else if ((this->tags & TT_LINE_TAG_GRADIENT_CENTER) > 0) {
          fprintf(out->file,",-2");
      } else if ((this->tags & TT_LINE_TAG_GRADIENT_POINT) > 0) {
        lp = this->first_point;
        while (lp != NULL) {
          if ((lp->tags & TT_LINEPT_TAG_GRADIENT) > 0)
          fprintf(out->file,",%d",from);
          lp = lp->nextlp;
          from++;
        }
      } else {
          fprintf(out->file,",-1");
      }
      fprintf(out->file,");\n");
      postprocess = false;
      break;
    case TT_LINE_TYPE_SLOPE:
      macroid = SYML_SLOPE;
      if (this->context >= 0)
        macroid = this->context;
      if (!out->symset->is_assigned(macroid)) {
        postprocess = false;
        break;
      }
      s1 = 0.0;
      r1 = -1.0;
      // najde prvu rotaciu a velkost
      lp = this->first_point;
      fsize = true;
      frot = true;
      while ((lp != NULL) && (fsize || frot)) {
        if ((lp->tags & TT_LINEPT_TAG_SIZE) > 0)
          if (fsize) {
            s1 = lp->lsize;
            fsize = false;
          }
        if ((lp->tags & TT_LINEPT_TAG_ORIENT) > 0)
          if (frot) {
            r1 = lp->orient + out->rr;
            frot = false;
          }
        lp = lp->nextlp;
      }
      if (out->file == NULL)
        return(true);
      out->symset->export_mp_symbol_options(out->file, SYML_SLOPE);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_SLOPE));
      this->export_path_mp(out);
      fprintf(out->file,",%d",
          ((this->tags & TT_LINE_TAG_BORDER) > 0 ? 1 : 0));

      // vypise prvy bod
      fprintf(out->file,",(0,%g,%g)",(r1 < 0.0 ? -1 : r1),s1 * out->ms);

      // vypise ostatne body
      lp = this->first_point->nextlp;
      from = 1;
      while (lp != NULL) {
        if ((lp->tags & (TT_LINEPT_TAG_SIZE | TT_LINEPT_TAG_ORIENT)) > 0) {

          // cislo bodu
          fprintf(out->file,",(%d,",from);

          // orientacia
          if ((lp->tags & TT_LINEPT_TAG_ORIENT) > 0) {
            r1 = lp->orient + out->rr;
            fprintf(out->file,"%g,",lp->orient + out->rr);
          } else {
            if (lp->nextlp != NULL)
              fprintf(out->file,"-1,");
            else
              fprintf(out->file,"%g,",(r1 < 0.0 ? -1.0 : r1));
          }

          // velkost
          if ((lp->tags & TT_LINEPT_TAG_SIZE) > 0) {
            s1 = lp->lsize;
            fprintf(out->file,"%g)",lp->lsize * out->ms);
          } else {
            if (lp->nextlp != NULL)
              fprintf(out->file,"-1)");
            else
              fprintf(out->file,"%g)",s1 * out->ms);
          }
        } else if (lp->nextlp == NULL) {
          // vypise posledny bod
          fprintf(out->file,",(%d,%g,%g)",from,(r1 < 0.0 ? -1.0 : r1),s1 * out->ms);
        }
        lp = lp->nextlp;
        from++;
      }
      // vypise prvy bod
      fprintf(out->file,");\n");
      postprocess = false;
      break;

    thline_type_export_mp(TT_LINE_TYPE_PIT, SYML_PIT)
    thline_type_export_mp(TT_LINE_TYPE_MAP_CONNECTION, SYML_MAPCONNECTION)
    thline_type_export_mp(TT_LINE_TYPE_CEILING_STEP, SYML_CEILINGSTEP)
    thline_type_export_mp(TT_LINE_TYPE_CEILING_MEANDER, SYML_CEILINGMEANDER)
    thline_type_export_mp(TT_LINE_TYPE_FLOOR_STEP, SYML_FLOORSTEP)
    thline_type_export_mp(TT_LINE_TYPE_FLOOR_MEANDER, SYML_FLOORMEANDER)
    thline_type_export_mp(TT_LINE_TYPE_OVERHANG, SYML_OVERHANG)
    thline_type_export_mp(TT_LINE_TYPE_CHIMNEY, SYML_CHIMNEY)
    thline_type_export_mp(TT_LINE_TYPE_FLOWSTONE, SYML_FLOWSTONE)
    thline_type_export_mp(TT_LINE_TYPE_MOONMILK, SYML_MOONMILK)
    thline_type_export_mp(TT_LINE_TYPE_ROCK_BORDER, SYML_ROCKBORDER)
    thline_type_export_mp(TT_LINE_TYPE_ROCK_EDGE, SYML_ROCKEDGE)
    thline_type_export_mp(TT_LINE_TYPE_GRADIENT, SYML_GRADIENT)
    thline_type_export_mp(TT_LINE_TYPE_HANDRAIL, SYML_HANDRAIL)
    thline_type_export_mp(TT_LINE_TYPE_ROPE_LADDER, SYML_ROPELADDER)
    thline_type_export_mp(TT_LINE_TYPE_FIXED_LADDER, SYML_FIXEDLADDER)
    thline_type_export_mp(TT_LINE_TYPE_STEPS, SYML_STEPS)
    thline_type_export_mp(TT_LINE_TYPE_VIA_FERRATA, SYML_VIAFERRATA)
    thline_type_export_mp(TT_LINE_TYPE_ABYSSENTRANCE, SYML_ABYSSENTRANCE)
    thline_type_export_mp(TT_LINE_TYPE_DRIPLINE, SYML_DRIPLINE)
    thline_type_export_mp(TT_LINE_TYPE_FAULT, SYML_FAULT)
    thline_type_export_mp(TT_LINE_TYPE_JOINT, SYML_JOINT)
    thline_type_export_mp(TT_LINE_TYPE_LOWCEILING, SYML_LOWCEILING)
    thline_type_export_mp(TT_LINE_TYPE_PITCHIMNEY, SYML_PITCHIMNEY)
    thline_type_export_mp(TT_LINE_TYPE_RIMSTONEDAM, SYML_RIMSTONEDAM)
    thline_type_export_mp(TT_LINE_TYPE_RIMSTONEPOOL, SYML_RIMSTONEPOOL)
    thline_type_export_mp(TT_LINE_TYPE_WALKWAY, SYML_WALKWAY)

    case TT_LINE_TYPE_U:
      macroid = this->db->db2d.register_u_symbol(TT_LINE_CMD, this->m_subtype_str);
      break;
	
    case TT_LINE_TYPE_ROPE:
      macroid = SYML_ROPE;
      if (this->context >= 0)
        macroid = this->context;
      if (!out->symset->is_assigned(macroid)) {
        postprocess = false;
        break;
      }
      if (out->file == NULL)
        return(true);

      out->symset->export_mp_symbol_options(out->file, SYML_ROPE);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_ROPE));
      this->export_path_mp(out);
      fprintf(out->file,",%s,%s);\n", ((this->tags & TT_LINE_TAG_ROPE_ANCHORS) > 0 ? "true" : "false"),
        ((this->tags & TT_LINE_TAG_ROPE_REBELAYS) > 0 ? "true" : "false"));

      postprocess = false;
      break;

    case TT_LINE_TYPE_ARROW:
      macroid = SYML_ARROW;
      if (this->context >= 0)
        macroid = this->context;
      if (!out->symset->is_assigned(macroid)) {
        postprocess = false;
        break;
      }
      if (out->file == NULL)
        return(true);
      out->symset->export_mp_symbol_options(out->file, SYML_ARROW);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_ARROW));
      this->export_path_mp(out);
      from = 0;
      if ((this->tags & TT_LINE_TAG_HEAD_BEGIN) > 0)
        from += 1;
      if ((this->tags & TT_LINE_TAG_HEAD_END) > 0)
        from += 2;
      fprintf(out->file,",%d);\n",from);
      postprocess = false;
      break;

    case TT_LINE_TYPE_SECTION:
      macroid = SYML_SECTION;
      if (this->context >= 0)
        macroid = this->context;
      if (!out->symset->is_assigned(macroid)) {
        postprocess = false;
        break;
      }
      if (out->file == NULL)
        return(true);
      out->symset->export_mp_symbol_options(out->file, SYML_SECTION);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_SECTION));
      this->export_path_mp(out);
      anypt = false;
      if ((this->tags & TT_LINE_TAG_DIRECTION_BEGIN) > 0) {
          fprintf(out->file,",0");
          anypt = true;
      };
      from = 0;
      lp = this->first_point;
      while (lp != NULL) {
        if (((lp->tags & TT_LINEPT_TAG_DIRECTION) > 0) &&
            ((this->tags & TT_LINE_TAG_DIRECTION_POINT) > 0)) {
          fprintf(out->file,",%d",from);
          anypt = true;
        }
        lp = lp->nextlp;
        from++;
      }
      if ((this->tags & TT_LINE_TAG_DIRECTION_END) > 0) {
          fprintf(out->file,",%d",(from - 1));
          anypt = true;
      };
      if (!anypt)
        fprintf(out->file,",");
      fprintf(out->file,");\n");
      postprocess = false;
      break;


    case TT_LINE_TYPE_BORDER:
      from = 0;
      to = 0;
      lp = this->first_point;
      while (lp != NULL) {
        cs = lp->subtype;
        todraw = (lp->nextlp != NULL);
        while ((lp != NULL) && (lp->subtype == cs)) {
          to++;
          lp = lp->nextlp;
        }
        if (todraw) {
          macroid = SYML_BORDER_VISIBLE;
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_TEMPORARY, SYML_BORDER_TEMPORARY)
            thline_type_export_mp(TT_LINE_SUBTYPE_PRESUMED, SYML_BORDER_PRESUMED)
            thline_type_export_mp(TT_LINE_SUBTYPE_INVISIBLE, SYML_BORDER_INVISIBLE)
          }
          omacroid = macroid;
          if (this->context >= 0)
            macroid = this->context;
          if (out->symset->is_assigned(macroid)) {
            if (out->file == NULL)
              return(true);
            out->symset->export_mp_symbol_options(out->file, omacroid);
            fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
            this->export_path_mp(out,from,to);
            fprintf(out->file,");\n");
            if (out->layout->is_debug_joins()) {
              fprintf(out->file,"l_debug(1,0,");
              this->export_path_mp(out,from,to,1);
              fprintf(out->file,");\n");
            }
            if (out->layout->is_debug_stations()) {
              fprintf(out->file,"l_debug(0,0,");
              this->export_path_mp(out,from,to,0);
              fprintf(out->file,");\n");
            }
          }
        }
        from = to;
      }
      postprocess = false;
      break;

    case TT_LINE_TYPE_WATER_FLOW:
      from = 0;
      to = 0;
      lp = this->first_point;
      while (lp != NULL) {
        cs = lp->subtype;
        todraw = (lp->nextlp != NULL);
        while ((lp != NULL) && (lp->subtype == cs)) {
          to++;
          lp = lp->nextlp;
        }
        if (todraw) {
          macroid = SYML_WATERFLOW_PERMANENT;
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_INTERMITTENT, SYML_WATERFLOW_INTERMITTENT)
            thline_type_export_mp(TT_LINE_SUBTYPE_CONJECTURAL, SYML_WATERFLOW_CONJECTURAL)
          }
          omacroid = macroid;
          if (this->context >= 0)
            macroid = this->context;
          if (out->symset->is_assigned(macroid)) {
            if (out->file == NULL)
              return(true);
            out->symset->export_mp_symbol_options(out->file, omacroid);
            fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
            this->export_path_mp(out,from,to);
            fprintf(out->file,");\n");
          }
        }
        from = to;
      }
      postprocess = false;
      break;

    case TT_LINE_TYPE_SURVEY:
      from = 0;
      to = 0;
      lp = this->first_point;
      while (lp != NULL) {
        cs = lp->subtype;
        todraw = (lp->nextlp != NULL);
        while ((lp != NULL) && (lp->subtype == cs)) {
          to++;
          lp = lp->nextlp;
        }
        if (todraw) {
          macroid = SYML_SURVEY_CAVE;
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_SURFACE, SYML_SURVEY_SURFACE)
          }
          omacroid = macroid;
          if (this->context >= 0)
            macroid = this->context;
          if (out->symset->is_assigned(macroid)) {
            if (out->file == NULL)
              return(true);
            out->symset->export_mp_symbol_options(out->file, omacroid);
            fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
            this->export_path_mp(out,from,to);
            fprintf(out->file,");\n");
          }
        }
        from = to;
      }
      postprocess = false;
      break;

    default:
      macroid = SYML_UNDEFINED;
      break;
  }

  if (postprocess) {
    if (macroid < 0) {
      this->export_path_mp(out);
      fprintf(out->file,");\n");
    } else {
      omacroid = macroid;
      if (this->context >= 0)
        macroid = this->context;
      if (out->symset->is_assigned(macroid)) {
        if (out->file == NULL)
          return(true);
        if (this->type == TT_LINE_TYPE_U) {
          out->symset->export_mp_symbol_options(out->file, omacroid);
          fprintf(out->file,"l_u_%s(", this->m_subtype_str);
          out->symset->usymbols[omacroid].m_used = true;
          this->db->db2d.use_u_symbol(this->get_class_id(), this->m_subtype_str);
        } else {
          out->symset->export_mp_symbol_options(out->file, omacroid);
          fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
        }
        this->export_path_mp(out);
        fprintf(out->file,");\n");
        if (out->layout->is_debug_joins()) {
          fprintf(out->file,"l_debug(1,0,");
          this->export_path_mp(out,0,-1,1);
          fprintf(out->file,");\n");
        }
        if (out->layout->is_debug_stations()) {
          fprintf(out->file,"l_debug(0,0,");
          this->export_path_mp(out,0,-1,0);
          fprintf(out->file,");\n");
        }
      }
    }
  }

  th2ddataobject::export_mp_end(out);
  return(false);
}


unsigned thline::export_path_mp(class thexpmapmpxs * out,
      int from, int to, int dbglevel)
{
  thdb2dlp * lp = this->first_point;
//  double xt, yt, d;
  unsigned last = 0;
  bool dnu = false;
  while ((lp != NULL) && ((long(last) <= long(to)) || (to < 0))) {
    if (long(last) == long(from)) {
      dnu = true;
      fprintf(out->file,"(");
      lp->point->export_mp(out,dbglevel);
      fprintf(out->file,"\n");
      if (long(last) == long(to)) {
        fprintf(out->file," -- ");
        lp->point->export_mp(out,dbglevel);
      }
    }
    else if (dnu) {
      if ((lp->cp1 != NULL) && (lp->cp2 != NULL)) {
        fprintf(out->file," .. controls ");
        lp->cp1->export_mp(out,dbglevel);
        fprintf(out->file," and ");
        lp->cp2->export_mp(out,dbglevel);
        fprintf(out->file," .. ");
      }
      else {
        fprintf(out->file," -- ");
      }
      if ((from == 0) && (lp->nextlp == NULL) && (this->is_closed))
        fprintf(out->file,"cycle");
      else
        lp->point->export_mp(out,dbglevel);
      fprintf(out->file,"\n");
    }
    lp = lp->nextlp;
    last++;
  }
  if (long(last) > long(from)) {
    fprintf(out->file,")");
    return (last - from);
  }
  else {
    return 0;
  }
}


void thline::parse_border(char * ss) {
  int bd;
  if (this->type != TT_LINE_TYPE_SLOPE)
    throw thexception(fmt::format("-border not valid with type {}", thmatch_string(this->type,thtt_line_types)));
  bd = thmatch_token(ss,thtt_bool);
  if (bd == TT_UNKNOWN_BOOL)
    throw thexception(fmt::format("logical value expected -- {}",ss));
  if (bd == TT_TRUE)
    this->tags |= TT_LINE_TAG_BORDER;
  else
    this->tags &= ~TT_LINE_TAG_BORDER;
}

enum {
  TT_LINE_GRADIENT_UNKNOWN,
  TT_LINE_GRADIENT_BEGIN,
  TT_LINE_GRADIENT_END,
  TT_LINE_GRADIENT_NONE,
  TT_LINE_GRADIENT_CENTER,
  TT_LINE_GRADIENT_POINT,
  TT_LINE_GRADIENT_BOTH,
};

static const thstok thtt_line_gradient[] = {
  {"begin", TT_LINE_GRADIENT_BEGIN},
  {"both", TT_LINE_GRADIENT_BOTH},
  {"center", TT_LINE_GRADIENT_CENTER},
  {"end", TT_LINE_GRADIENT_END},
  {"none", TT_LINE_GRADIENT_NONE},
  {"point", TT_LINE_GRADIENT_POINT},
  {NULL, TT_LINE_GRADIENT_UNKNOWN}
};


void thline::parse_gradient(char * ss) {
  int gd;
  if (this->type != TT_LINE_TYPE_CONTOUR)
    throw thexception(fmt::format("-gradient not valid with type {}", thmatch_string(this->type,thtt_line_types)));
  gd = thmatch_token(ss,thtt_line_gradient);
  switch (gd) {
    case TT_LINE_GRADIENT_NONE:
      this->tags &= ~(TT_LINE_TAG_GRADIENT_NONE | TT_LINE_TAG_GRADIENT_CENTER | TT_LINE_TAG_GRADIENT_POINT);
      this->tags |= TT_LINE_TAG_GRADIENT_NONE;
      break;
    case TT_LINE_GRADIENT_CENTER:
      this->tags &= ~(TT_LINE_TAG_GRADIENT_NONE | TT_LINE_TAG_GRADIENT_CENTER | TT_LINE_TAG_GRADIENT_POINT);
      this->tags |= TT_LINE_TAG_GRADIENT_CENTER;
      break;
    case TT_LINE_GRADIENT_POINT:
      if (this->last_point == NULL)
        throw thexception("no line point specified");
      this->tags &= ~(TT_LINE_TAG_GRADIENT_NONE | TT_LINE_TAG_GRADIENT_CENTER | TT_LINE_TAG_GRADIENT_POINT);
      this->tags |= TT_LINE_TAG_GRADIENT_POINT;
      this->last_point->tags |= TT_LINEPT_TAG_GRADIENT;
      break;
    default:
      throw thexception(fmt::format("invalid gradient specification -- {}",ss));
      break;
  }
}


void thline::parse_direction(char * ss) {
  int gd;
  if (this->type != TT_LINE_TYPE_SECTION)
    throw thexception(fmt::format("-direction not valid with type {}", thmatch_string(this->type,thtt_line_types)));
  gd = thmatch_token(ss,thtt_line_gradient);
  switch (gd) {
    case TT_LINE_GRADIENT_NONE:
      this->tags &= ~(TT_LINE_TAG_DIRECTION_BEGIN | TT_LINE_TAG_DIRECTION_END | TT_LINE_TAG_DIRECTION_POINT);
      break;
    case TT_LINE_GRADIENT_BEGIN:
      this->tags &= ~(TT_LINE_TAG_DIRECTION_END | TT_LINE_TAG_DIRECTION_POINT);
      this->tags |= TT_LINE_TAG_DIRECTION_BEGIN;
      break;
    case TT_LINE_GRADIENT_END:
      this->tags &= ~(TT_LINE_TAG_DIRECTION_BEGIN | TT_LINE_TAG_DIRECTION_POINT);
      this->tags |= TT_LINE_TAG_DIRECTION_END;
      break;
    case TT_LINE_GRADIENT_BOTH:
      this->tags &= ~TT_LINE_TAG_DIRECTION_POINT;
      this->tags |= TT_LINE_TAG_DIRECTION_END | TT_LINE_TAG_DIRECTION_BEGIN;
      break;
    case TT_LINE_GRADIENT_POINT:
      if (this->last_point == NULL)
        throw thexception("no line point specified");
      this->tags &= ~(TT_LINE_TAG_DIRECTION_BEGIN | TT_LINE_TAG_DIRECTION_END);
      this->tags |= TT_LINE_TAG_DIRECTION_POINT;
      this->last_point->tags |= TT_LINEPT_TAG_DIRECTION;
      break;
    default:
      throw thexception(fmt::format("invalid direction specification -- {}",ss));
      break;
  }
}




void thline::parse_head(char * ss) {
  int gd;
  if (this->type != TT_LINE_TYPE_ARROW)
    throw thexception(fmt::format("-direction not valid with type {}", thmatch_string(this->type,thtt_line_types)));
  gd = thmatch_token(ss,thtt_line_gradient);
  this->tags &= ~(TT_LINE_TAG_HEAD_BEGIN | TT_LINE_TAG_HEAD_END);
  switch (gd) {
    case TT_LINE_GRADIENT_NONE:
      break;
    case TT_LINE_GRADIENT_BEGIN:
      this->tags |= TT_LINE_TAG_HEAD_BEGIN;
      break;
    case TT_LINE_GRADIENT_END:
      this->tags |= TT_LINE_TAG_HEAD_END;
      break;
    case TT_LINE_GRADIENT_BOTH:
      this->tags |= TT_LINE_TAG_HEAD_END | TT_LINE_TAG_HEAD_BEGIN;
      break;
    default:
      throw thexception(fmt::format("invalid head specification -- {}",ss));
      break;
  }
}


void thline::parse_adjust(char * ss) {
  if (this->last_point != NULL)
    this->last_point->adjust = thmatch_token(ss,thtt_line_adjusts);
  else
    throw thexception("no line point specified");
  if (this->last_point->adjust == TT_LINE_ADJUST_UNKNOWN)
    throw thexception(fmt::format("invalid adjust specification -- {}",ss));
  if ((thdb.cscrapptr->proj->type == TT_2DPROJ_PLAN) &&
      (this->last_point->adjust != TT_LINE_ADJUST_NONE))
    throw thexception(fmt::format("adjustment and projection not compatible -- {}",ss));
}


void thline::parse_size(int w, char * ss) {
  int sv;
  double sz;
  if (this->last_point == NULL)
    throw thexception("no line point specified");
  const char * sizestr = NULL;
  switch (w) {
    case TT_LINE_SIZE:
      sizestr = "size";
      break;
    case TT_LINE_LSIZE:
      sizestr = "l-size";
      break;
    case TT_LINE_RSIZE:
      sizestr = "r-size";
      break;
  }
  bool ok = false;
  switch (this->type) {
    case TT_LINE_TYPE_SLOPE:
        if ((w == TT_LINE_SIZE) || (w == TT_LINE_LSIZE))
          ok = true;
      break;
  }
  if (!ok)
    throw thexception(fmt::format("{} not valid with type {}", sizestr,
        thmatch_string(this->type,thtt_line_types)));

  thparse_double(sv,sz,ss);
  if (sv != TT_SV_NUMBER)
    throw thexception(fmt::format("invalid number -- {}",ss));
  if (sz < 0.0)
    throw thexception(fmt::format("negative size -- {}",ss));

  switch (this->type) {
    case TT_LINE_TYPE_SLOPE:
      this->last_point->lsize = sz;
      this->last_point->tags |= TT_LINEPT_TAG_SIZE;
      break;
  }
}


void thline::parse_altitude(char * ss) {

  if (this->type != TT_LINE_TYPE_WALL)
    throw thexception(fmt::format("-altitude not valid with type {}", thmatch_string(this->type,thtt_line_types)));

  if (this->last_point == NULL)
    throw thexception("no line point specified");

  thparse_altitude(ss, this->last_point->rsize, this->last_point->lsize);

  this->last_point->tags |= TT_LINEPT_TAG_ALTITUDE;
}

void thline::parse_text(char * ss) {
  if (this->type != TT_LINE_TYPE_LABEL)
    throw thexception(fmt::format("-text not valid with type {}", thmatch_string(this->type,thtt_line_types)));
  if (strlen(ss) > 0)
    this->text = this->db->strstore(ss);
}


void thline::start_insert() {

  thdb2dlp * lp;
  bool fsize;

  switch (this->type) {
    case TT_LINE_TYPE_U:
      if (this->m_subtype_str == NULL)
        throw thexception("missing subtype specification for line of user defined type");
      this->db->db2d.register_u_symbol(this->get_class_id(), this->m_subtype_str);
      break;
    case TT_LINE_TYPE_SLOPE:
      lp = this->first_point;
      fsize = true;
      while ((lp != NULL) && fsize) {
        if ((lp->tags & TT_LINEPT_TAG_SIZE) > 0)
          if (fsize) {
            fsize = false;
          }
        lp = lp->nextlp;
      }
      if (fsize)
        throw thexception("no slope size specification at any line point");
      break;
  }
}





void thline::parse_height(char * ss) {
  switch (this->type) {
    case TT_LINE_TYPE_PIT:
      break;
	case TT_LINE_TYPE_WALL:
	  if (this->csubtype == TT_LINE_SUBTYPE_PIT)
		  break;
    [[fallthrough]];
    default:
      throw thexception(fmt::format("-height not valid with type {}", thmatch_string(this->type,thtt_line_types)));
      break;
  }

  thsplit_words(& this->db->db2d.mbf,ss);
  int npar = this->db->db2d.mbf.get_size();
  char ** pars = this->db->db2d.mbf.get_buffer();
  int sv, ux;
  double dv;
  thtflength lentf;

  ux = 0;
  switch (npar) {
    case 1:
      break;
    case 2:
      ux = 1;
      break;
    default:
      throw thexception(fmt::format("invalid distance -- {}",ss));
  }
  this->m_height = thnan;
  thparse_double(sv,dv,pars[0]);
  if (sv != TT_SV_NUMBER)
    throw thexception(fmt::format("not a number -- {}", pars[0]));
  if (dv <= 0.0)
	throw thexception(fmt::format("nor a positivie number -- {}", pars[0]));
  if (ux > 0) {
    lentf.parse_units(pars[ux]);
    dv = lentf.transform(dv);
  }
  this->m_height = dv;

}





