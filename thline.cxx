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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thline.h"
#include "thexception.h"
#include "thparse.h"
#include "thchenc.h"
#include "thdb2dlp.h"
#include "thexpmap.h"
#include "thtflength.h"
#include "thtexfonts.h"
#include "thscrap.h"

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


char * thline::get_cmd_end()
{
  return "endline";
}


char * thline::get_cmd_name()
{
  return "line";
}


thcmd_option_desc thline::get_cmd_option_desc(char * opts)
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
  if (cod.id == 1)
    cod.id = TT_LINE_TYPE;
    
  switch (cod.id) {
  
    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->insert_line_point(this->db->db2d.mbf.get_size(), 
        this->db->db2d.mbf.get_buffer());
      break;

    case TT_LINE_TYPE:
      this->parse_type(*args);
      break;
    
    case TT_LINE_OUTLINE:
      this->outline = thmatch_token(*args,thtt_line_outlines);
      if (this->outline == TT_LINE_OUTLINE_UNKNOWN)
        ththrow(("unknown line outline -- %s",*args))
      break;
    
    case TT_LINE_CLOSED:
      this->closed = thmatch_token(*args,thtt_onoffauto);
      if (this->closed == TT_UNKNOWN_BOOL)
        ththrow(("invalid closure switch -- %s",*args))
      break;
    
    case TT_LINE_REVERSE:
      reversion = thmatch_token(*args,thtt_bool);
      if (reversion == TT_UNKNOWN_BOOL)
        ththrow(("invalid reversion switch -- %s",*args))
      this->reverse = (reversion == TT_TRUE);
      break;
    
    case TT_LINE_SUBTYPE:
      this->parse_subtype(*args);
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
        ththrow(("invalid smooth switch -- %s",*args))
      if (this->last_point != NULL)
        this->last_point->smooth = csmooth;
      else 
        ththrow(("no line point specified"))
      break;
      
    case TT_LINE_ORIENT:
      if (this->last_point != NULL) {
        if (this->type != TT_LINE_TYPE_SLOPE) 
          ththrow(("orientation not valid with type %s", thmatch_string(this->type,thtt_line_types)))
        thparse_double(sv,this->last_point->orient,*args);
        if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
          ththrow(("invalid number -- %s",*args))
        if ((this->last_point->orient < 0.0) || (this->last_point->orient >= 360.0))
          ththrow(("orientation out of range -- %s",*args))
        this->last_point->tags |= TT_LINEPT_TAG_ORIENT;
      }
      else
        ththrow(("no line point specified"))
      break;

    case TT_LINE_SIZE:
    case TT_LINE_RSIZE:
    case TT_LINE_LSIZE:
      this->parse_size(cod.id,*args);
      break;
      
    case TT_LINE_MARK:
      this->insert_point_mark(*args);
      break;
      
    
    // if not found, try to set fathers properties  
    default:
      th2ddataobject::set(cod, args, argenc, indataline);
  }
}


void thline::self_delete()
{
  delete this;
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
      ththrow(("unknown line type -- %s",ss))
      break;
    case TT_LINE_TYPE_WALL:
      this->csubtype = TT_LINE_SUBTYPE_BEDROCK;  
      this->outline = TT_LINE_OUTLINE_OUT;
      break;
    case TT_LINE_TYPE_BORDER:
      this->csubtype = TT_LINE_SUBTYPE_VISIBLE;  
      break;
    case TT_LINE_TYPE_CONTOUR:
      this->tags |= TT_LINE_TAG_GRADIENT_CENTER;
      break;
    case TT_LINE_TYPE_ARROW:
      this->tags |= TT_LINE_TAG_HEAD_END;
      break;
  }
}


void thline::parse_subtype(char * ss) 
{
  this->csubtype = thmatch_token(ss,thtt_line_subtypes);
  if (this->csubtype == TT_LINE_SUBTYPE_UNKNOWN)
    ththrow(("invalid line subtype -- %s",ss))
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
          tsok = true;
      }
      break;
    case TT_LINE_TYPE_BORDER:
      switch (this->csubtype) {
        case TT_LINE_SUBTYPE_INVISIBLE:
        case TT_LINE_SUBTYPE_TEMPORARY:
        case TT_LINE_SUBTYPE_VISIBLE:
          tsok = true;
      }
      break;
  }
  if (!tsok)
    ththrow(("invalid line type - subtype combination"))
  if (this->last_point != NULL)
    this->last_point->subtype = this->csubtype;
}



void thline::insert_line_point(int nargs, char ** args)
{
  // check number of parameters
  if ((nargs != 6) && (nargs != 2))
    ththrow(("invalid number of coordinates -- %d", nargs))
  
  double cp1x, cp1y, cp2x, cp2y, x, y;
  int pidx = 0, sv;
  char * invs = NULL;
  bool invnum = false;
  
  if (nargs == 6) {
    pidx = 4;
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
  }
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
  if (invnum)
    ththrow(("invalid number -- %s",invs))

  // let's insert point into database
  thdb2dlp * plp = this->db->db2d.insert_line_point();
  
  plp->subtype = this->csubtype;
  plp->smooth = TT_AUTO;
  
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
    ththrow(("mark not a key word -- %s",ss))
  if (this->last_point == NULL)
    ththrow(("no line point specified"))
  this->last_point->mark = this->db->strstore(ss,true);
}

thdb2dlp * thline::get_marked_station(char * mark)
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
  int t_tags, t_subtype, t_smooth;
  double t_rsize, t_lsize, t_orient;
  if (this->reverse) {
  
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
        c_item->subtype = c_item->nextlp->subtype;
      } else {
        c_item->point = t_point;
        c_item->tags = t_tags;
        c_item->lsize = t_lsize;
        c_item->rsize = t_rsize;
        c_item->orient = t_orient;
        c_item->smooth = t_smooth;
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
        d1 = hypot(c_item->cp2->x - c_item->point->x, c_item->cp2->y - c_item->point->y);
        d2 = hypot(c_item->nextlp->cp1->x - c_item->point->x, c_item->nextlp->cp1->y - c_item->point->y);
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
  fprintf(out->file,"%s(",out->symset->get_mp_macro(mid)); \
  break;

void thline::export_mp(class thexpmapmpxs * out)
{

  if (this->first_point == NULL)
    return;
  bool postprocess = true, todraw, fsize, frot;  //, first
  int from, to, cs;
  double s1, s2, r1, r2;
  thdb2dlp * lp, * plp;
  
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
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_INVISIBLE, SYML_WALL_INVISIBLE)
            thline_type_export_mp(TT_LINE_SUBTYPE_BEDROCK, SYML_WALL_BEDROCK)
            thline_type_export_mp(TT_LINE_SUBTYPE_SAND, SYML_WALL_SAND)
            thline_type_export_mp(TT_LINE_SUBTYPE_CLAY, SYML_WALL_CLAY)
            thline_type_export_mp(TT_LINE_SUBTYPE_PEBBLES, SYML_WALL_PEBBLES)
            thline_type_export_mp(TT_LINE_SUBTYPE_DEBRIS, SYML_WALL_DEBRIS)
            thline_type_export_mp(TT_LINE_SUBTYPE_BLOCKS, SYML_WALL_BLOCKS)
            thline_type_export_mp(TT_LINE_SUBTYPE_ICE, SYML_WALL_ICE)
            thline_type_export_mp(TT_LINE_SUBTYPE_UNDERLYING, SYML_WALL_UNDERLYING)
            thline_type_export_mp(TT_LINE_SUBTYPE_UNSURVEYED, SYML_WALL_UNSURVEYED)
            thline_type_export_mp(TT_LINE_SUBTYPE_PRESUMED, SYML_WALL_PRESUMED)
            default:
              fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_UNDEFINED));
              break;
          }
        }
        if (todraw) {
          this->export_path_mp(out,from,to);
          fprintf(out->file,");\n");
        }
        from = to;
      }
      postprocess = false;  
      break;
    case TT_LINE_TYPE_LABEL:
      if (this->text == NULL) {
        postprocess = false;
        break;
      } 
      fprintf(out->file,"l_label(btex ");
      switch (this->scale) {
        case TT_2DOBJ_SCALE_XL:
          fprintf(out->file,"\\thhugesize ");
          break;
        case TT_2DOBJ_SCALE_L:
          fprintf(out->file,"\\thlargesize ");
          break;
        case TT_2DOBJ_SCALE_S:
          fprintf(out->file,"\\thsmallsize ");
          break;
        case TT_2DOBJ_SCALE_XS:
          fprintf(out->file,"\\thtinysize ");
          break;
        default:
          fprintf(out->file,"\\thnormalsize ");
      }
      //thdecode(&(this->db->buff_enc),TT_ISO8859_2,this->text);
      fprintf(out->file,"%s etex,",utf2tex(this->text));
      this->export_path_mp(out);
      fprintf(out->file,");\n");
      postprocess = false;
      break;
    case TT_LINE_TYPE_CONTOUR:
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_CONTOUR));
      this->export_path_mp(out);
      from = 0;
      if ((this->tags & TT_LINE_TAG_GRADIENT_CENTER) > 0) {
          fprintf(out->file,",-1");
      } else if ((this->tags & TT_LINE_TAG_GRADIENT_POINT) > 0) {
        lp = this->first_point;
        while (lp != NULL) {
          if ((lp->tags & TT_LINEPT_TAG_GRADIENT) > 0)
          fprintf(out->file,",%d",from);
          lp = lp->nextlp;
          from++;
        }
      } else {
          fprintf(out->file,",");
      }
      fprintf(out->file,");\n");
      postprocess = false;  
      break;
    case TT_LINE_TYPE_SLOPE:
      s1 = 28.34;
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
            r1 = lp->orient;
            frot = false;
          }
        lp = lp->nextlp;
      }
      
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
            r1 = lp->orient;
            fprintf(out->file,"%g,",lp->orient);
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
    thline_type_export_mp(TT_LINE_TYPE_CEILING_STEP, SYML_CEILINGSTEP)
    thline_type_export_mp(TT_LINE_TYPE_FLOOR_STEP, SYML_FLOORSTEP)
    thline_type_export_mp(TT_LINE_TYPE_OVERHANG, SYML_OVERHANG)
    thline_type_export_mp(TT_LINE_TYPE_CHIMNEY, SYML_CHIMNEY)
    thline_type_export_mp(TT_LINE_TYPE_FLOWSTONE, SYML_FLOWSTONE)
    thline_type_export_mp(TT_LINE_TYPE_ROCK_BORDER, SYML_ROCKBORDER)
    thline_type_export_mp(TT_LINE_TYPE_ROCK_EDGE, SYML_ROCKEDGE)
    thline_type_export_mp(TT_LINE_TYPE_SURVEY, SYML_SURVEY)
    case TT_LINE_TYPE_ARROW:
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
      plp = this->first_point;
      lp = plp->nextlp;
      while (lp != NULL) {
        // zisti ci bude kreslit pociatocnu a koncovu sipku
        if ((plp->prevlp == NULL) && 
             (((this->tags & TT_LINE_TAG_DIRECTION_BEGIN) != 0) ||
               (((this->tags & TT_LINE_TAG_DIRECTION_POINT) != 0) &&
               ((plp->tags & TT_LINEPT_TAG_DIRECTION) > 0)))) {
          // vykresli pociatocnu sipku
          fprintf(out->file,"%s(",out->symset->get_mp_macro(SYMP_SECTIONARROW));
          plp->point->export_mp(out);
          fprintf(out->file,",%.2f);\n",atan2(lp->point->yt - plp->point->yt,
            lp->point->xt- plp->point->xt) / 3.14159265358 * 180);        
        }
        
        if (((lp->nextlp == NULL) 
              && ((this->tags & TT_LINE_TAG_DIRECTION_END) != 0)) ||
           (((lp->tags & TT_LINEPT_TAG_DIRECTION) > 0) 
              && ((this->tags & TT_LINE_TAG_DIRECTION_POINT) != 0))) {
          // vykresli koncovu sipku
          fprintf(out->file,"%s(",out->symset->get_mp_macro(SYMP_SECTIONARROW));
          lp->point->export_mp(out);
          fprintf(out->file,",%.2f);\n",atan2(lp->point->yt - plp->point->yt,
            lp->point->xt - plp->point->xt) / 3.14159265358 * 180);        
        }
        
        if ((lp->cp1 != NULL) && (lp->cp2 != NULL)) {
          r1 = lp->point->xt - plp->point->xt;
          r2 = lp->point->yt - plp->point->yt;
          s1 = (r1 * (lp->cp1->xt - plp->point->xt) + 
                r2 * (lp->cp1->yt - plp->point->yt)) /
               (pow(r1,2.0) + pow(r2,2.0)); 
          s2 = (- r1 * (lp->cp2->xt - lp->point->xt) - 
                  r2 * (lp->cp2->yt - lp->point->yt)) /
               (pow(r1,2.0) + pow(r2,2.0));           
          fprintf(out->file,"%s((",out->symset->get_mp_macro(SYML_SECTION));
          plp->point->export_mp(out);          
          fprintf(out->file," -- (%.2f,%.2f)));\n%s(((%.2f,%.2f) -- ",
            (plp->point->xt + s1 * r1 - out->mx) * out->ms,
            (plp->point->yt + s1 * r2 - out->my) * out->ms,
            out->symset->get_mp_macro(SYML_SECTION),
            (lp->point->xt - s2 * r1 - out->mx) * out->ms,
            (lp->point->yt - s2 * r2 - out->my) * out->ms
            );
          lp->point->export_mp(out);
          fprintf(out->file,"));\n");
        } else {
          fprintf(out->file,"%s((",out->symset->get_mp_macro(SYML_SECTION));
          plp->point->export_mp(out);          
          fprintf(out->file," -- ");
          lp->point->export_mp(out);
          fprintf(out->file,"));\n");
        }
        // zisti si ci ma kreslit medzibody, ak ano tak ich nakresli
        plp = lp;
        lp = lp->nextlp;
      }
      postprocess = false;
      break;
    /*
    case TT_LINE_TYPE_SECTION:
      // vypise section arrows
      if ((this->tags & TT_LINE_TAG_DIRECTION_BEGIN) != 0) {
        fprintf(out->file,"Sectionarrow(");
        this->first_point->point->export_mp(out);
        fprintf(out->file,",%.2f);\n",this->first_point->get_rotation());
      }
      if ((this->tags & TT_LINE_TAG_DIRECTION_END) != 0) {
        fprintf(out->file,"Sectionarrow(");
        this->last_point->point->export_mp(out);
        fprintf(out->file,",%.2f);\n",this->last_point->get_rotation());
      }
      if ((this->tags & TT_LINE_TAG_DIRECTION_POINT) != 0) {
        lp = this->first_point;
        while (lp != NULL) {
          if ((lp->tags & TT_LINEPT_TAG_DIRECTION) > 0) {
            fprintf(out->file,"Sectionarrow(");
            lp->point->export_mp(out);
            fprintf(out->file,",%.2f);\n",lp->get_rotation());
          }
          lp = lp->nextlp;
        }
      }
      fprintf(out->file,"Sectionline(");
      break;
      */
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
          switch (cs) {
            thline_type_export_mp(TT_LINE_SUBTYPE_TEMPORARY, SYML_BORDER_TEMPORARY)
            thline_type_export_mp(TT_LINE_SUBTYPE_INVISIBLE, SYML_BORDER_INVISIBLE)
            default:
              fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_BORDER_VISIBLE));
              break;
          }
        }
        if (todraw) {
          this->export_path_mp(out,from,to);
          fprintf(out->file,");\n");
        }
        from = to;
      }
      postprocess = false;  
      break;    
    default:
      fprintf(out->file,"%s(",out->symset->get_mp_macro(SYML_UNDEFINED));
      break;
  }
  
  if (postprocess) {
    this->export_path_mp(out);
    fprintf(out->file,");\n");
  }
  
}


unsigned thline::export_path_mp(class thexpmapmpxs * out,
      int from, int to)
{
  thdb2dlp * lp = this->first_point;
  thdb2dpt * prev_pt;
//  double xt, yt, d;
  unsigned last = 0;
  bool dnu = false;
  while ((lp != NULL) && ((long(last) <= long(to)) || (to < 0))) {
    if (long(last) == long(from)) {
      dnu = true;
      fprintf(out->file,"(");
      lp->point->export_mp(out);
      fprintf(out->file,"\n");
      if (long(last) == long(to)) {
        fprintf(out->file," -- ");
        lp->point->export_mp(out);
      }
      prev_pt = lp->point;
    } 
    else if (dnu) {
      if ((lp->cp1 != NULL) && (lp->cp2 != NULL)) {
        fprintf(out->file," .. controls ");
        lp->cp1->export_mp(out);
        fprintf(out->file," and ");
        lp->cp2->export_mp(out);
        fprintf(out->file," .. ");
      } 
      else {
        fprintf(out->file," -- ");
      }
      lp->point->export_mp(out);
      fprintf(out->file,"\n");
    }
    prev_pt = lp->point;
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
    ththrow(("-border not valid with type %s", thmatch_string(this->type,thtt_line_types)))
  bd = thmatch_token(ss,thtt_bool);
  if (bd == TT_UNKNOWN_BOOL)
    ththrow(("logical value expected -- %s",ss))
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
    ththrow(("-gradient not valid with type %s", thmatch_string(this->type,thtt_line_types)))
  gd = thmatch_token(ss,thtt_line_gradient);
  switch (gd) {
    case TT_LINE_GRADIENT_NONE:
      this->tags &= ~(TT_LINE_TAG_GRADIENT_CENTER | TT_LINE_TAG_GRADIENT_POINT);
      break;
    case TT_LINE_GRADIENT_CENTER:
      this->tags &= ~(TT_LINE_TAG_GRADIENT_CENTER | TT_LINE_TAG_GRADIENT_POINT);
      this->tags |= TT_LINE_TAG_GRADIENT_CENTER;
      break;
    case TT_LINE_GRADIENT_POINT:
      if (this->last_point == NULL)
        ththrow(("no line point specified"))
      this->tags &= ~TT_LINE_TAG_GRADIENT_CENTER;
      this->tags |= TT_LINE_TAG_GRADIENT_POINT;
      this->last_point->tags |= TT_LINEPT_TAG_GRADIENT;
      break;
    default:
      ththrow(("invalid gradient specification -- %s",ss))
      break;
  }
}


void thline::parse_direction(char * ss) {
  int gd;
  if (this->type != TT_LINE_TYPE_SECTION)
    ththrow(("-direction not valid with type %s", thmatch_string(this->type,thtt_line_types)))
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
        ththrow(("no line point specified"))
      this->tags &= ~(TT_LINE_TAG_DIRECTION_BEGIN | TT_LINE_TAG_DIRECTION_END);
      this->tags |= TT_LINE_TAG_DIRECTION_POINT;
      this->last_point->tags |= TT_LINEPT_TAG_DIRECTION;
      break;
    default:
      ththrow(("invalid direction specification -- %s",ss))
      break;
  }
}




void thline::parse_head(char * ss) {
  int gd;
  if (this->type != TT_LINE_TYPE_ARROW)
    ththrow(("-direction not valid with type %s", thmatch_string(this->type,thtt_line_types)))
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
      ththrow(("invalid head specification -- %s",ss))
      break;
  }
}


void thline::parse_size(int w, char * ss) {
  int sv;
  double sz;
  if (this->last_point == NULL)
    ththrow(("no line point specified"))
  char * sizestr = NULL;
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
    ththrow(("%s not valid with type %s", sizestr,
        thmatch_string(this->type,thtt_line_types)))

  thparse_double(sv,sz,ss);
  if (sv != TT_SV_NUMBER)
    ththrow(("invalid number -- %s",ss))
  if (sz < 0.0)
    ththrow(("negative size -- %s",ss))
  
  switch (this->type) {
    case TT_LINE_TYPE_SLOPE:
      this->last_point->lsize = sz;
      this->last_point->tags |= TT_LINEPT_TAG_SIZE;
      break;
  }
}


void thline::parse_altitude(char * ss) {

  if (this->type != TT_LINE_TYPE_WALL)
    ththrow(("-altitude not valid with type %s", thmatch_string(this->type,thtt_line_types)))

  if (this->last_point == NULL)
    ththrow(("no line point specified"))

  thparse_altitude(ss, this->last_point->rsize, this->last_point->lsize);
    
  this->last_point->tags |= TT_LINEPT_TAG_ALTITUDE;
}

void thline::parse_text(char * ss) {
  if (this->type != TT_LINE_TYPE_LABEL)
    ththrow(("-text not valid with type %s", thmatch_string(this->type,thtt_line_types)))
  if (strlen(ss) > 0)
    this->text = this->db->strstore(ss);
}






