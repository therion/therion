/**
 * @file thpoint.cxx
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
 
#include "thpoint.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdatabase.h"
#include "thinfnan.h"
#include "thexpmap.h"
#include "thtflength.h"
#include "thtexfonts.h"
#include "thdate.h"

thpoint::thpoint()
{
  // replace this by setting real properties initialization
  this->type = TT_POINT_TYPE_UNKNOWN;
  this->subtype = TT_POINT_SUBTYPE_UNKNOWN;
  this->point = thdb.db2d.insert_point();
  this->cpoint = NULL;
  this->point->pscrap = thdb.get_current_scrap();
  
  this->station_name.clear();
  this->extend_name.clear();
  this->extend_point = NULL;
  this->extend_opts = TT_POINT_EXTEND_NONE;
  
  this->orient = thnan;
  this->xsize = thnan;
  this->ysize = thnan;
  this->align = TT_POINT_ALIGN_C;
  
  this->text = NULL;
}


thpoint::~thpoint()
{
  if (this->type == TT_POINT_TYPE_DATE) {
    thdate * dp = (thdate *) this->text;
    if (dp != NULL)
      delete dp;
    this->text = NULL;
  }
}


int thpoint::get_class_id() 
{
  return TT_POINT_CMD;
}


bool thpoint::is(int class_id)
{
  if (class_id == TT_POINT_CMD)
    return true;
  else
    return th2ddataobject::is(class_id);
}

int thpoint::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 3;
}


char * thpoint::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


char * thpoint::get_cmd_name()
{
  // insert command name here
  return "point";
}


thcmd_option_desc thpoint::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_point_opt);
  if (id == TT_POINT_UNKNOWN)
    return th2ddataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thpoint::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  double dv;
  int sv;
  
  if (cod.id == 3)
    cod.id = TT_POINT_TYPE;
    
  switch (cod.id) {
  
    case 1:
    case 2:
      thparse_double(sv,dv,*args);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s",*args))
      if (cod.id == 1)
        this->point->x = dv;
      else
        this->point->y = dv;
      break;

    case TT_POINT_TYPE:
      this->parse_type(*args);
      break;

    case TT_POINT_VALUE:
      this->parse_value(*args);
      break;

    case TT_POINT_TEXT:
      thencode(&(this->db->buff_enc), *args, argenc);
      this->parse_text(this->db->buff_enc.get_buffer());
      break;

    case TT_POINT_ALIGN:
      this->parse_align(*args);
      break;

    case TT_POINT_SCRAP:
      if (this->type != TT_POINT_TYPE_SECTION)
        ththrow(("point not section -- -scrap"))
      thparse_objectname(this->station_name, & this->db->buff_stations, *args);
      break;

    case TT_POINT_XSIZE:
      ththrow(("-x-size not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      thparse_double(sv,this->xsize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s",*args))
      if (this->xsize <= 0.0)
        ththrow(("size not positive -- %s",*args))
      break;

    case TT_POINT_SIZE:
      ththrow(("-size not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      thparse_double(sv,this->xsize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s",*args))
      if (this->xsize <= 0.0)
        ththrow(("size not positive -- %s",*args))
      this->ysize = this->xsize;  
      break;
      
    case TT_POINT_YSIZE:
      ththrow(("-y-size not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      thparse_double(sv,this->ysize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid number -- %s",*args))
      if (this->ysize <= 0.0)
        ththrow(("size not positive -- %s",*args))
      break;

    case TT_POINT_ORIENT:
      switch (this->type) {
        case TT_POINT_TYPE_STATION:
          ththrow(("-orientation not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      }
      thparse_double(sv,this->orient,*args);
      if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
        ththrow(("invalid number -- %s",*args))
      if ((this->orient < 0.0) || (this->orient >= 360.0))
        ththrow(("orientation out of range -- %s",*args))
      break;
    
    case TT_POINT_SUBTYPE:
      this->parse_subtype(*args);
      break;
      
    case TT_POINT_STATION:
      if (this->type != TT_POINT_TYPE_STATION)
        ththrow(("point not station -- -name"))
      thparse_objectname(this->station_name, & this->db->buff_stations, *args);
      break;
      
    case TT_POINT_EXTEND:
      this->parse_extend(*args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      if (cod.id == TT_2DOBJ_CLIP) {
        switch (this->type) {
          case TT_POINT_TYPE_STATION_NAME:
          case TT_POINT_TYPE_LABEL:
          case TT_POINT_TYPE_REMARK:
          case TT_POINT_TYPE_DATE:
          case TT_POINT_TYPE_ALTITUDE:
          case TT_POINT_TYPE_HEIGHT:
          case TT_POINT_TYPE_PASSAGE_HEIGHT:
            ththrow(("-clip not valid with type %s", thmatch_string(this->type,thtt_point_types)))
            break;
        }
      }
      th2ddataobject::set(cod, args, argenc, indataline);
  }
}


void thpoint::self_delete()
{
  delete this;
}

void thpoint::self_print_properties(FILE * outf)
{
  th2ddataobject::self_print_properties(outf);
  fprintf(outf,"thpoint:\n");
  fprintf(outf,"\ttype: %d\n",this->type);
  fprintf(outf,"\tsubtype: %d\n",this->subtype);
  fprintf(outf,"\talign: %d\n",this->align);
  fprintf(outf,"\tpoint: %f,%f (%f,%f,%f,%f)\n",this->point->x, this->point->y,
    this->point->xt,this->point->yt,this->point->zt,this->point->at);
  fprintf(outf,"\torientation: %f\n", this->orient);
  fprintf(outf,"\tx-size: %f\n", this->xsize);
  fprintf(outf,"\ty-size: %f\n", this->ysize);
  if (this->type == TT_POINT_TYPE_STATION) {
    fprintf(outf,"\tstation: ");
    fprintf(outf,this->station_name);
    fprintf(outf,"\n\textend station: ");
    fprintf(outf,this->extend_name);
    fprintf(outf,"\n\textend opts: %d\n",this->extend_opts);
  }
  // insert intended print of object properties here
}


void thpoint::parse_type(char * tstr)
{
  this->type = thmatch_token(tstr, thtt_point_types);
  if (this->type == TT_POINT_TYPE_UNKNOWN)
    ththrow(("unknown point type -- %s", tstr))
  this->xsize = thnan;
  this->ysize = thnan;
  switch (this->type) {
    case TT_POINT_TYPE_DATE:
      if (this->type == TT_POINT_TYPE_DATE) 
        this->text = (char *) new thdate;
      break;
    case TT_POINT_TYPE_ALTITUDE:
      this->xsize = 0.0;
      this->ysize = 1.0;
      break;
  }
}
 
 
void thpoint::parse_subtype(char * ststr)
{
  if (this->type == TT_POINT_TYPE_UNKNOWN)
    ththrow(("point type must be specified before subtype"))
  this->subtype = thmatch_token(ststr, thtt_point_subtypes);
  if (this->subtype == TT_POINT_SUBTYPE_UNKNOWN)
    ththrow(("unknown point subtype -- %s", ststr))
  // let's check type - subtype
  bool combok = false;
  switch (this->type) {
    case TT_POINT_TYPE_STATION:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_TEMP:
        case TT_POINT_SUBTYPE_FIXED:
        case TT_POINT_SUBTYPE_PAINTED:
        case TT_POINT_SUBTYPE_NATURAL:
          combok = true;
          break;
      }
      break;
    case TT_POINT_TYPE_WATER_FLOW:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_PERMANENT:
        case TT_POINT_SUBTYPE_INTERMITTENT:
        case TT_POINT_SUBTYPE_PALEO:
          combok = true;
          break;
      }
      break;
  }
  if (!combok)
    ththrow(("invalid point type - subtype combination"))
}

void thpoint::parse_extend(char * estr)
{
  thsplit_words(& this->db->db2d.mbf, estr);
  int npar = this->db->db2d.mbf.get_size(),cpar,stic;
  char ** pars = this->db->db2d.mbf.get_buffer();
  if (npar < 1)
    ththrow(("no -extend arguments"))
  cpar = 0;
  while (cpar < npar) {
    switch (thmatch_token(pars[cpar],thtt_point_extopt)) {
      case TT_POINT_EXTEND_LEFT:
        this->extend_opts = this->extend_opts & 
          (~(TT_POINT_EXTEND_LEFT | TT_POINT_EXTEND_RIGHT));
        this->extend_opts = this->extend_opts | TT_POINT_EXTEND_LEFT;
        break;  
      case TT_POINT_EXTEND_RIGHT:
        this->extend_opts = this->extend_opts & 
          (~(TT_POINT_EXTEND_LEFT | TT_POINT_EXTEND_RIGHT));
        this->extend_opts = this->extend_opts | TT_POINT_EXTEND_RIGHT;
        break;
      case TT_POINT_EXTEND_ROOT:
        this->extend_opts = this->extend_opts | TT_POINT_EXTEND_ROOT;
        break;
      case TT_POINT_EXTEND_STICKY:
        cpar++;
        if (cpar == npar)
          ththrow(("logical value expected"))
        stic = thmatch_token(pars[cpar],thtt_bool);
        if (stic == TT_UNKNOWN_BOOL)
          ththrow(("not a logical value -- %s", pars[cpar]))
        this->extend_opts = this->extend_opts & 
          (~(TT_POINT_EXTEND_STICKYON | TT_POINT_EXTEND_STICKYOFF));
        if (stic == TT_TRUE)
          this->extend_opts = this->extend_opts | TT_POINT_EXTEND_STICKYON;
        else
          this->extend_opts = this->extend_opts | TT_POINT_EXTEND_STICKYOFF;
        break;
      case TT_POINT_EXTEND_PREV:
        cpar++;
        if (cpar == npar)
          ththrow(("point or station name expected"))
        thparse_objectname(this->extend_name,& this->db->buff_stations,pars[cpar]);
        break;
      default:
        ththrow(("invalid keyword -- %s",pars[cpar]))
    }
    cpar++;
  }
}


char * thpoint_export_mp_align2mp(int a) {
  switch (a) {
    case TT_POINT_ALIGN_R:
      return ".rt";
    case TT_POINT_ALIGN_L:
      return ".lft";
    case TT_POINT_ALIGN_T:
      return ".top";
    case TT_POINT_ALIGN_B:
      return ".bot";
    case TT_POINT_ALIGN_BR:
      return ".lrt";
    case TT_POINT_ALIGN_BL:
      return ".llft";
    case TT_POINT_ALIGN_TR:
      return ".urt";
    case TT_POINT_ALIGN_TL:
      return ".ulft";
    default:
      return "";
  }
}


bool thpoint::export_mp(class thexpmapmpxs * out)
{
  bool postprocess = true;
  int macroid = -1;
  int postprocess_label = -1;
  this->db->buff_enc.guarantee(8128);
  char * buff = this->db->buff_enc.get_buffer();

  switch(this->type) {

    case TT_POINT_TYPE_LABEL:  
    case TT_POINT_TYPE_REMARK:  
    case TT_POINT_TYPE_STATION_NAME:
      if (this->text != NULL) {
      
        switch (this->type) {
          case TT_POINT_TYPE_LABEL:
            macroid = SYMP_LABEL;
            break;
          case TT_POINT_TYPE_REMARK:
            macroid = SYMP_LABEL;
            break;
          default:
            macroid = SYMP_STATIONNAME;
            break;
        }
        if (!out->symset->assigned[macroid])
          return(false);
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(macroid);
        fprintf(out->file,"p_label%s(btex ",thpoint_export_mp_align2mp(this->align));
        switch (this->type) {
          case TT_POINT_TYPE_STATION_NAME:
            fprintf(out->file,"\\thstationname ");
            break;
          case TT_POINT_TYPE_LABEL:
            fprintf(out->file,"\\thlabel ");
            break;
          case TT_POINT_TYPE_REMARK:
            fprintf(out->file,"\\thremark ");
            break;
        }
        fprintf(out->file,"%s etex,",utf2tex(this->text));        
        if (this->type == TT_POINT_TYPE_STATION_NAME)
          postprocess_label = 7;
        else
          postprocess_label = 0;
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_STATION:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_FIXED:
          macroid = SYMP_STATION_FIXED;
          break;
        case TT_POINT_SUBTYPE_NATURAL:
          macroid = SYMP_STATION_NATURAL;
          break;
        case TT_POINT_SUBTYPE_PAINTED:
          macroid = SYMP_STATION_PAINTED;
          break;
        default:
          macroid = SYMP_STATION_TEMPORARY;
      }
      if (out->symset->assigned[macroid]) {
        if (out->file == NULL)
          return(true);
        fprintf(out->file,"%s(",out->symset->get_mp_macro(macroid));
        this->point->export_mp(out);
        fprintf(out->file,");\n");
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_ALTITUDE:
      if ((!thisnan(this->xsize)) && (out->symset->assigned[SYMP_ALTITUDE])) {          
        sprintf(buff,"%.0f",this->xsize);
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(SYMP_ALTITUDE);    
        fprintf(out->file,"p_label%s(btex \\thaltitude %s etex,",thpoint_export_mp_align2mp(this->align),utf2tex(buff));
        postprocess_label = 1;
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
          TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {        

        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case TT_POINT_TAG_HEIGHT_P:
            macroid = SYMP_HEIGHT_POSITIVE;
            break;
          case TT_POINT_TAG_HEIGHT_N:
            macroid = SYMP_HEIGHT_NEGATIVE;
            break;
          case TT_POINT_TAG_HEIGHT_U:
            macroid = SYMP_HEIGHT_UNSIGNED;
            break;
          default:
            return(false);
        }
        
        if (!out->symset->assigned[macroid])
          return(false);

        if (out->file == NULL)
          return(true);

        out->symset->get_mp_macro(macroid);
        fprintf(out->file,"p_label%s(btex ",thpoint_export_mp_align2mp(this->align));

        if ((this->tags & TT_POINT_TAG_HEIGHT_P) != 0)
          fprintf(out->file,"\\thheightpos ");
        else if ((this->tags & TT_POINT_TAG_HEIGHT_N) != 0)
          fprintf(out->file,"\\thheightneg ");
        else
          fprintf(out->file,"\\thheight ");  

        if (!thisnan(this->xsize)) {
          if (double(int(this->xsize)) != this->xsize)
            sprintf(buff,"%.1f",this->xsize);
          else
            sprintf(buff,"%.0f",this->xsize);
          fprintf(out->file,utf2tex(buff));
        }
        this->db->buff_enc.strcpy((this->tags & (TT_POINT_TAG_HEIGHT_PQ |
            TT_POINT_TAG_HEIGHT_NQ | TT_POINT_TAG_HEIGHT_UQ)) != 0 ? "?" : "" );
        fprintf(out->file,"%s etex,",utf2tex(this->db->buff_enc.get_buffer()));
        postprocess_label = 7;
      }
      postprocess = false;
      break;


    case TT_POINT_TYPE_DATE:
      if  ((out->symset->assigned[SYMP_DATE]) &&
          ((this->tags & TT_POINT_TAG_DATE) > 0)) {
//        ((thdate *)this->text)->print_export_str();
//        fprintf(out->file,"Datelabel%s(\"%s\",",
//            thpoint_export_mp_align2mp(this->align),
//            ((thdate *)this->text)->get_str());
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(SYMP_DATE);    
        fprintf(out->file,"p_label%s(btex \\thdate %s etex,",
            thpoint_export_mp_align2mp(this->align),
            utf2tex(((thdate *)this->text)->get_str(TT_DATE_FMT_UTF8_ISO)));
        postprocess_label = 0;
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_PASSAGE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
          TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {

        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case (TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N):
            macroid = SYMP_PASSAGEHEIGHT_BOTH;
            break;
          case TT_POINT_TAG_HEIGHT_P:
            macroid = SYMP_PASSAGEHEIGHT_POSITIVE;
            break;
          case TT_POINT_TAG_HEIGHT_N:
            macroid = SYMP_PASSAGEHEIGHT_NEGATIVE;
            break;
          case TT_POINT_TAG_HEIGHT_U:
            macroid = SYMP_PASSAGEHEIGHT_UNSIGNED;
            break;
          default:
            return(false);
        }
        
        if (!out->symset->assigned[macroid])
          return(false);
        if (out->file == NULL)
          return(true);
        
        out->symset->get_mp_macro(macroid);
            
        fprintf(out->file,"p_label%s(btex ",thpoint_export_mp_align2mp(this->align));
        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case (TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N):
            fprintf(out->file,"\\thframed \\updown");
            postprocess_label = 4;
            break;
          case TT_POINT_TAG_HEIGHT_P:
            fprintf(out->file,"\\thframed ");
            postprocess_label = 2;
            break;
          case TT_POINT_TAG_HEIGHT_N:
            fprintf(out->file,"\\thframed ");
            postprocess_label = 3;
            break;
          default:
            fprintf(out->file,"\\thframed ");
            postprocess_label = 5;
            break;
        }

        if (!thisnan(this->xsize)) {
          if (double(int(this->xsize)) != this->xsize)
            sprintf(buff,"%.1f",this->xsize);
          else
            sprintf(buff,"%.0f",this->xsize);
          fprintf(out->file,"{%s}", utf2tex(buff));
        }
        
        if (!thisnan(this->ysize)) {
          if (double(int(this->ysize)) != this->ysize)
            sprintf(buff,"%.1f",this->ysize);
          else
            sprintf(buff,"%.0f",this->ysize);
          fprintf(out->file,"{%s}", utf2tex(buff));
        }        
        
        fprintf(out->file," etex,");
      }
      postprocess = false;
      break;
      
    case TT_POINT_TYPE_SECTION:
      postprocess = false;
      break;

#define thpoint_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;
      
// ostatne typy      
    case TT_POINT_TYPE_WATER_FLOW:
      switch (this->subtype) {
        thpoint_type_export_mp(TT_POINT_SUBTYPE_PALEO,SYMP_WATERFLOW_PALEO);
        thpoint_type_export_mp(TT_POINT_SUBTYPE_INTERMITTENT,SYMP_WATERFLOW_INTERMITTENT);
        default:
          macroid = SYMP_WATERFLOW_PERMANENT;
      }
      break;

// specialne symboly
    thpoint_type_export_mp(TT_POINT_TYPE_AIR_DRAUGHT,SYMP_AIRDRAUGHT)
    thpoint_type_export_mp(TT_POINT_TYPE_SPRING,SYMP_SPRING)
    thpoint_type_export_mp(TT_POINT_TYPE_SINK,SYMP_SINK)
    thpoint_type_export_mp(TT_POINT_TYPE_ENTRANCE,SYMP_ENTRANCE)
    thpoint_type_export_mp(TT_POINT_TYPE_GRADIENT,SYMP_GRADIENT)
  
// vystroj
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE,SYMP_ROPE)
    thpoint_type_export_mp(TT_POINT_TYPE_FIXED_LADDER,SYMP_FIXEDLADDER)
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE_LADDER,SYMP_ROPELADDER)
    thpoint_type_export_mp(TT_POINT_TYPE_STEPS,SYMP_STEPS)
    thpoint_type_export_mp(TT_POINT_TYPE_BRIDGE,SYMP_BRIDGE)
    thpoint_type_export_mp(TT_POINT_TYPE_TRAVERSE,SYMP_TRAVERSE)
    thpoint_type_export_mp(TT_POINT_TYPE_NO_EQUIPMENT,SYMP_NOEQUIPMENT)
    thpoint_type_export_mp(TT_POINT_TYPE_ANCHOR,SYMP_ANCHOR)
    thpoint_type_export_mp(TT_POINT_TYPE_CAMP,SYMP_CAMP)

// ukoncenia chodby
    thpoint_type_export_mp(TT_POINT_TYPE_CONTINUATION,SYMP_CONTINUATION)
    thpoint_type_export_mp(TT_POINT_TYPE_NARROW_END,SYMP_NARROWEND)
    thpoint_type_export_mp(TT_POINT_TYPE_LOW_END,SYMP_LOWEND)
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE_CHOKE,SYMP_FLOWSTONECHOKE)
    thpoint_type_export_mp(TT_POINT_TYPE_BREAKDOWN_CHOKE,SYMP_BREAKDOWNCHOKE)

// vypln
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE,SYMP_FLOWSTONE)
    thpoint_type_export_mp(TT_POINT_TYPE_MOONMILK,SYMP_MOONMILK)
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITE,SYMP_STALACTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_STALAGMITE,SYMP_STALAGMITE)
    thpoint_type_export_mp(TT_POINT_TYPE_PILLAR,SYMP_PILLAR)
    thpoint_type_export_mp(TT_POINT_TYPE_CURTAIN,SYMP_CURTAIN)
    thpoint_type_export_mp(TT_POINT_TYPE_HELICTITE,SYMP_HELICTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_SODA_STRAW,SYMP_SODASTRAW)
    thpoint_type_export_mp(TT_POINT_TYPE_CRYSTAL,SYMP_CRYSTAL)
    thpoint_type_export_mp(TT_POINT_TYPE_WALL_CALCITE,SYMP_WALLCALCITE)
    thpoint_type_export_mp(TT_POINT_TYPE_POPCORN,SYMP_POPCORN)
    thpoint_type_export_mp(TT_POINT_TYPE_DISK,SYMP_DISK)
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM,SYMP_GYPSUM)
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM_FLOWER,SYMP_GYPSUMFLOWER)
    thpoint_type_export_mp(TT_POINT_TYPE_ARAGONITE,SYMP_ARAGONITE)
    thpoint_type_export_mp(TT_POINT_TYPE_CAVE_PEARL,SYMP_CAVEPEARL)
    thpoint_type_export_mp(TT_POINT_TYPE_RIMSTONE_POOL,SYMP_RIMSTONEPOOL)
    thpoint_type_export_mp(TT_POINT_TYPE_RIMSTONE_DAM,SYMP_RIMSTONEDAM)
    thpoint_type_export_mp(TT_POINT_TYPE_ANASTOMOSIS,SYMP_ANASTOMOSIS)
    thpoint_type_export_mp(TT_POINT_TYPE_KARREN,SYMP_KARREN)
    thpoint_type_export_mp(TT_POINT_TYPE_SCALLOP,SYMP_SCALLOP)
    thpoint_type_export_mp(TT_POINT_TYPE_FLUTE,SYMP_FLUTE)
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT_CONE,SYMP_RAFT)
  
// plosne vyplne
    thpoint_type_export_mp(TT_POINT_TYPE_BEDROCK,SYMP_BEDROCK)
    thpoint_type_export_mp(TT_POINT_TYPE_SAND,SYMP_SAND)
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT,SYMP_RAFT)
    thpoint_type_export_mp(TT_POINT_TYPE_CLAY,SYMP_CLAY)
    thpoint_type_export_mp(TT_POINT_TYPE_PEBBLES,SYMP_PEBBLES)
    thpoint_type_export_mp(TT_POINT_TYPE_DEBRIS,SYMP_DEBRIS)
    thpoint_type_export_mp(TT_POINT_TYPE_BLOCKS,SYMP_BLOCKS)
    thpoint_type_export_mp(TT_POINT_TYPE_WATER,SYMP_WATER)
    thpoint_type_export_mp(TT_POINT_TYPE_ICE,SYMP_ICE)
    thpoint_type_export_mp(TT_POINT_TYPE_GUANO,SYMP_GUANO)

// ina vypln
    thpoint_type_export_mp(TT_POINT_TYPE_ARCHEO_MATERIAL,SYMP_ARCHEOMATERIAL)
    thpoint_type_export_mp(TT_POINT_TYPE_PALEO_MATERIAL,SYMP_PALEOMATERIAL)
    thpoint_type_export_mp(TT_POINT_TYPE_VEGETABLE_DEBRIS,SYMP_VEGETABLEDEBRIS)
    thpoint_type_export_mp(TT_POINT_TYPE_ROOT,SYMP_ROOT)
      
    default:
      macroid = SYMP_UNDEFINED;
      break;
  }
  
  if (postprocess_label >= 0) {
    this->point->export_mp(out);
    fprintf(out->file,",%.1f,%d);\n",(thisnan(this->orient) ? 0 : 360 - this->orient), postprocess_label);
  }
  
  if ((macroid > 0) && postprocess) {
    if (out->symset->assigned[macroid]) {
      if (out->file == NULL)
        return(true);
      fprintf(out->file,"%s(",out->symset->get_mp_macro(macroid));
    }
    else
      postprocess = false;
  }
  
  if (postprocess) {
    this->point->export_mp(out);
    double scl = 1.0;
    switch (this->scale) {
      case TT_2DOBJ_SCALE_L:
        scl = 1.414;
        break;
      case TT_2DOBJ_SCALE_XL:
        scl = 2.0;
        break;
      case TT_2DOBJ_SCALE_S:
        scl = 0.707;
        break;
      case TT_2DOBJ_SCALE_XS:
        scl = 0.5;
        break;
    }
    char * al = "(0,0)";
    switch (this->align) {
      case TT_POINT_ALIGN_B:
        al = "(0,-1)";
        break;
      case TT_POINT_ALIGN_T:
        al = "(0,1)";
        break;
      case TT_POINT_ALIGN_L:
        al = "(-1,0)";
        break;
      case TT_POINT_ALIGN_R:
        al = "(1,0)";
        break;
      case TT_POINT_ALIGN_BL:
        al = "(-1,-1)";
        break;
      case TT_POINT_ALIGN_TL:
        al = "(-1,1)";
        break;
      case TT_POINT_ALIGN_BR:
        al = "(1,-1)";
        break;
      case TT_POINT_ALIGN_TR:
        al = "(1,1)";
        break;
    }
    fprintf(out->file,",%.1f,%.2f,%s);\n",
        (thisnan(this->orient) ? 0 : 360 - this->orient),scl,al);
  }
  
  return(false);
}

void thpoint::parse_align(char * tstr) {
  switch (this->type) {
    case TT_POINT_TYPE_STATION:
      ththrow(("-align not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      break;
  }
  this->align = thmatch_token(tstr, thtt_point_aligns);
  if (this->align == TT_POINT_ALIGN_UNKNOWN)
    ththrow(("unknown alignment -- %s", tstr))
}


void thpoint::parse_text(char * ss) {
  switch (this->type) {
    case TT_POINT_TYPE_LABEL:
    case TT_POINT_TYPE_REMARK:
    case TT_POINT_TYPE_STATION_NAME:
      break;
    default:
      ththrow(("-text not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      break;
  }
  if (strlen(ss) > 0)
    this->text = this->db->strstore(ss);
}


void thpoint_parse_value(int & sv, double & dv, bool & qw, int & sign, char * str) {

  // odstrani znamienka a nastavi sign
  sign = 0;
  qw = false;
  bool error = false;
  sv = TT_SV_NAN;
  dv = thnan;
  if (strlen(str) > 0) {
    switch (str[0]) {
      case '+':
        sign = 1;
        str++;
        break;
      case '-':
        sign = -1;
        str++;
        break;
    }
  } else
    error = true;

  // skusi odstranit otaznik
  size_t strl = strlen(str);
  if ((strl > 0) && (!error)) {
    if (str[strl-1] == '?') {
      str[strl-1] = 0;
      qw = true;
    }
  }
  
  // parsne cislo
  if (strlen(str)) {
    thparse_double(sv,dv,str);
    if ((sv != TT_SV_NAN) && (sv != TT_SV_NUMBER))
      error = true;
    else if ((sv != TT_SV_NAN) && (dv < 0.0))
      error = true;
  }
  
  if ((sv == TT_SV_NAN) && (sign == 0))
    error = true;
  
  if (error)
    ththrow(("invalid value -- %s",str))
    
}


void thpoint::parse_value(char * ss) {

  switch (this->type) {
    case TT_POINT_TYPE_ALTITUDE:
    case TT_POINT_TYPE_HEIGHT:
    case TT_POINT_TYPE_PASSAGE_HEIGHT:
    case TT_POINT_TYPE_DATE:
      break;
    default:
      ththrow(("-value not valid with type %s", thmatch_string(this->type,thtt_point_types)))
      break;
  }

  thsplit_words(& this->db->db2d.mbf,ss);
  int npar = this->db->db2d.mbf.get_size();
  char ** pars = this->db->db2d.mbf.get_buffer();
  int sv, ux, vx, sv2;
  bool parsev, quest, quest2;
  double dv, dv2;
  int sign, sign2;
  thtflength lentf;
  thdate * dp;
  
  switch (this->type) {
  
    // let's parse altitude
    case TT_POINT_TYPE_ALTITUDE:
      thparse_altitude(ss, this->xsize, this->ysize);
      break;
      
    case TT_POINT_TYPE_HEIGHT:
      ux = 0;
      parsev = false;
      switch (npar) {
        case 1:
          break;
        case 2:
          ux = 1;
          break;
        default:
          ththrow(("invalid value -- %s",ss))
      }
      this->xsize = thnan;
      this->tags &= ~(TT_POINT_TAG_HEIGHT_ALL);
      thpoint_parse_value(sv,dv,quest,sign,pars[0]);
      if ((ux > 0) && (!thisnan(dv))) {
        lentf.parse_units(pars[ux]);
        dv = lentf.transform(dv);
      }
      if (sign < 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_N;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_NQ;
      }
      else if (sign > 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_P;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_PQ;
      }
      else {
        this->tags |= TT_POINT_TAG_HEIGHT_U;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_UQ;
      }
      this->xsize = dv;
      break;

    case TT_POINT_TYPE_PASSAGE_HEIGHT:
      ux = 0;
      vx = 0;
      parsev = false;
      switch (npar) {
        case 1:
          break;
        case 2:
          ux = 1;
          try {
            lentf.parse_units(pars[ux]);
          } catch (...) {
            ux = 0;
            vx = 1;
          }
          break;
        case 3:
          ux = 2;
          vx = 1;
          break;
        default:
          ththrow(("invalid value -- %s",ss))
      }
      this->xsize = thnan;
      this->ysize = thnan;
      this->tags &= ~(TT_POINT_TAG_HEIGHT_ALL);
      thpoint_parse_value(sv,dv,quest,sign,pars[0]);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid value -- %s",pars[0]))
      if (vx > 0) {
        thpoint_parse_value(sv2,dv2,quest2,sign2,pars[vx]);
        if (sv2 != TT_SV_NUMBER)
          ththrow(("invalid value -- %s",pars[vx]))
        if ((sign == 0) || (sign2 == 0) || (sign == sign2))
          ththrow(("invalid combination of values -- %s",ss))
      }
      if ((ux > 0) && (!thisnan(dv))) {
        lentf.parse_units(pars[ux]);
        dv = lentf.transform(dv);
        if (vx > 0)
          dv2 = lentf.transform(dv2);
      }
      if (sign < 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_N;
        this->ysize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_NQ;
      }
      else if (sign > 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_P;
        this->xsize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_PQ;
      }
      else {
        this->tags |= TT_POINT_TAG_HEIGHT_U;
        this->xsize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_UQ;
      }
      if (vx > 0) {
        if (sign2 < 0) {
          this->tags |= TT_POINT_TAG_HEIGHT_N;
          this->ysize = dv2;
          if (quest)
            this->tags |= TT_POINT_TAG_HEIGHT_NQ;
        }
        else if (sign2 > 0) {
          this->tags |= TT_POINT_TAG_HEIGHT_P;
          this->xsize = dv2;
          if (quest)
            this->tags |= TT_POINT_TAG_HEIGHT_PQ;
        }
      }
      break;
      
    case TT_POINT_TYPE_DATE:
      if (npar != 1)
        ththrow(("invalid date -- %s",ss))
      dp = (thdate *) this->text;
      dp->parse(pars[0]);
      this->tags |= TT_POINT_TAG_DATE;
      break;
  }
}



