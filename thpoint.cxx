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
          case TT_POINT_TYPE_STATION:
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


void thpoint::export_mp(class thexpmapmpxs * out)
{
  bool postprocess = true;
  switch(this->type) {

    case TT_POINT_TYPE_STATION_NAME:
      if (this->text != NULL) {          
        fprintf(out->file,"Stationlabel%s(\"",thpoint_export_mp_align2mp(this->align));
        thdecode(&(this->db->buff_enc),TT_ISO8859_2,this->text);
        fprintf(out->file,"%s\",",this->db->buff_enc.get_buffer());
        this->point->export_mp(out);
        fprintf(out->file,");\n");
      }
      postprocess = false;
      break;
    case TT_POINT_TYPE_LABEL:  
    case TT_POINT_TYPE_REMARK:  
      if (this->text != NULL) {          
        fprintf(out->file,"Label%s(btex ",thpoint_export_mp_align2mp(this->align));
        if (this->type == TT_POINT_TYPE_REMARK) {
          fprintf(out->file,"\\it{}");
        }
        thdecode(&(this->db->buff_enc),TT_ISO8859_2,this->text);
        fprintf(out->file,"%s etex,",this->db->buff_enc.get_buffer());
        this->point->export_mp(out);
        fprintf(out->file,",%.1f);\n",
        (thisnan(this->orient) ? 0 : 360.0 - this->orient));
      }
      postprocess = false;
      break;
      
    case TT_POINT_TYPE_STATION:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_FIXED:
          fprintf(out->file,"FixedStation(");
          break;
        case TT_POINT_SUBTYPE_NATURAL:
          fprintf(out->file,"NaturalStation(");
          break;
        case TT_POINT_SUBTYPE_PAINTED:
          fprintf(out->file,"PaintedStation(");
          break;
        default:
          fprintf(out->file,"TemporaryStation(");
      }
      this->point->export_mp(out);
      fprintf(out->file,");\n");
      postprocess = false;
      break;

    case TT_POINT_TYPE_ALTITUDE:
      if (!thisnan(this->xsize)) {          
        fprintf(out->file,"Altitudelabel%s(\"%.0f\",",thpoint_export_mp_align2mp(this->align),this->xsize);
        this->point->export_mp(out);
        fprintf(out->file,");\n");
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {
        if ((this->tags & TT_POINT_TAG_HEIGHT_P) != 0)
          fprintf(out->file,"ElevDiffPos");
        else if ((this->tags & TT_POINT_TAG_HEIGHT_N) != 0)
          fprintf(out->file,"ElevDiffNeg");
        else
          fprintf(out->file,"ElevDiff");  
        fprintf(out->file,"%s(\"",thpoint_export_mp_align2mp(this->align));
        if (!thisnan(this->xsize)) {
          if (double(int(this->xsize)) != this->xsize)
            fprintf(out->file,"%.1f",this->xsize);
          else
            fprintf(out->file,"%.0f",this->xsize);
        }
        fprintf(out->file,"%s\",",((this->tags & (TT_POINT_TAG_HEIGHT_PQ |
            TT_POINT_TAG_HEIGHT_NQ | TT_POINT_TAG_HEIGHT_UQ)) != 0 ? "?" : "" ));
        this->point->export_mp(out);
        fprintf(out->file,");\n");
        postprocess = false;
      }
      break;

    case TT_POINT_TYPE_DATE:
      if ((this->tags & TT_POINT_TAG_DATE) > 0) {
        ((thdate *)this->text)->print_export_str();
        fprintf(out->file,"Datelabel%s(\"%s\",",
            thpoint_export_mp_align2mp(this->align),
            ((thdate *)this->text)->get_str());
        this->point->export_mp(out);
        fprintf(out->file,");\n");
        postprocess = false;
      }
      break;

    case TT_POINT_TYPE_PASSAGE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {
        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case (TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N):
            fprintf(out->file,"PassageWater%s(",thpoint_export_mp_align2mp(this->align));
            break;
          case TT_POINT_TAG_HEIGHT_P:
            fprintf(out->file,"Passageheight%s(",thpoint_export_mp_align2mp(this->align));
            break;
          case TT_POINT_TAG_HEIGHT_N:
            fprintf(out->file,"Waterdepth%s(",thpoint_export_mp_align2mp(this->align));
            break;
          default:
            fprintf(out->file,"Passage%s(",thpoint_export_mp_align2mp(this->align));
            break;
        }

        if (!thisnan(this->xsize)) {
          if (double(int(this->xsize)) != this->xsize)
            fprintf(out->file,"\"%.1f\",",this->xsize);
          else
            fprintf(out->file,"\"%.0f\",",this->xsize);
        }
        
        if (!thisnan(this->ysize)) {
          if (double(int(this->ysize)) != this->ysize)
            fprintf(out->file,"\"%.1f\",",this->ysize);
          else
            fprintf(out->file,"\"%.0f\",",this->ysize);
        }        
        this->point->export_mp(out);
        fprintf(out->file,");\n");
        postprocess = false;
      }
      break;
      
    case TT_POINT_TYPE_SECTION:
      postprocess = false;
      break;
      
// ostatne typy      
    case TT_POINT_TYPE_WATER_FLOW:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_PALEO:
          fprintf(out->file,"PaleoFlow(");
          break;
        case TT_POINT_SUBTYPE_INTERMITTENT:
          fprintf(out->file,"IntermittentStream(");
          break;
        default:
          fprintf(out->file,"PermanentStream(");
      }
      break;

#define thpoint_type_export_mp(type,str) case type: \
  fprintf(out->file,str); \
  break;
  
// specialne symboly
    thpoint_type_export_mp(TT_POINT_TYPE_AIR_DRAUGHT,"AirflowArrow(")
    thpoint_type_export_mp(TT_POINT_TYPE_SPRING,"Spring(")
    thpoint_type_export_mp(TT_POINT_TYPE_SINK,"Sink(")
    thpoint_type_export_mp(TT_POINT_TYPE_ENTRANCE,"EntranceArrow(")
    thpoint_type_export_mp(TT_POINT_TYPE_GRADIENT,"GradientArrow(")
  
// vystroj
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE,"Rope(")
    thpoint_type_export_mp(TT_POINT_TYPE_FIXED_LADDER,"FixedLadder(")
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE_LADDER,"RopeLadder(")
    thpoint_type_export_mp(TT_POINT_TYPE_STEPS,"Steps(")
    thpoint_type_export_mp(TT_POINT_TYPE_BRIDGE,"Bridge(")
    thpoint_type_export_mp(TT_POINT_TYPE_TRAVERSE,"Traverse(")
    thpoint_type_export_mp(TT_POINT_TYPE_NO_EQUIPEMENT,"Notequipped(")
    thpoint_type_export_mp(TT_POINT_TYPE_ANCHOR,"Anchor(")
//    thpoint_type_export_mp(TT_POINT_TYPE_CAMP,"Camp(")

// ukoncenia chodby
    thpoint_type_export_mp(TT_POINT_TYPE_CONTINUATION,"Continuation(")
    thpoint_type_export_mp(TT_POINT_TYPE_NARROW_END,"NarrowEnd(")
    thpoint_type_export_mp(TT_POINT_TYPE_LOW_END,"LowEnd(")
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE_CHOKE,"FlowstoneChoke(")
    thpoint_type_export_mp(TT_POINT_TYPE_BREAKDOWN_CHOKE,"BreakdownChoke(")

// vypln
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE,"Flowstone(")
    thpoint_type_export_mp(TT_POINT_TYPE_MOONMILK,"Moonmilk(")
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITE,"Stalactite(")
    thpoint_type_export_mp(TT_POINT_TYPE_STALAGMITE,"Stalagmite(")
    thpoint_type_export_mp(TT_POINT_TYPE_PILLAR,"Pillar(")
    thpoint_type_export_mp(TT_POINT_TYPE_CURTAIN,"Curtain(")
    thpoint_type_export_mp(TT_POINT_TYPE_HELECTITE,"Helectite(")
    thpoint_type_export_mp(TT_POINT_TYPE_SODA_STRAW,"Sodastraw(")
    thpoint_type_export_mp(TT_POINT_TYPE_CRYSTAL,"Crystal(")
    thpoint_type_export_mp(TT_POINT_TYPE_WALL_CALCITE,"Wallcalcite(")
    thpoint_type_export_mp(TT_POINT_TYPE_POPCORN,"Popcorn(")
    thpoint_type_export_mp(TT_POINT_TYPE_DISK,"Disk(")
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM,"Gypsum(")
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM_FLOWER,"Gypsumflower(")
    thpoint_type_export_mp(TT_POINT_TYPE_ARAGONITE,"Aragonite(")
    thpoint_type_export_mp(TT_POINT_TYPE_CAVE_PEARL,"Cavepearl(")
    thpoint_type_export_mp(TT_POINT_TYPE_RIMSTONE_POOL,"Rimstonepool(")
    thpoint_type_export_mp(TT_POINT_TYPE_ROMSTONE_DAM,"Rimstonedam(")
    thpoint_type_export_mp(TT_POINT_TYPE_ANASTOMOSIS,"Anastomosis(")
    thpoint_type_export_mp(TT_POINT_TYPE_KARREN,"Karren(")
    thpoint_type_export_mp(TT_POINT_TYPE_SCALLOP,"Scallop(")
    thpoint_type_export_mp(TT_POINT_TYPE_FLUTE,"Flute(")
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT_CONE,"Raftcone(")
  
// plosne vyplne
    thpoint_type_export_mp(TT_POINT_TYPE_BEDROCK,"Bedrock(")
    thpoint_type_export_mp(TT_POINT_TYPE_SAND,"Sand(")
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT,"Raft(")
    thpoint_type_export_mp(TT_POINT_TYPE_CLAY,"Clay(")
    thpoint_type_export_mp(TT_POINT_TYPE_PEBBLES,"Pebbles(")
    thpoint_type_export_mp(TT_POINT_TYPE_DEBRIS,"Debris(")
    thpoint_type_export_mp(TT_POINT_TYPE_BLOCKS,"Blocks(")
    thpoint_type_export_mp(TT_POINT_TYPE_WATER,"Water(")
    thpoint_type_export_mp(TT_POINT_TYPE_ICE,"Ice(")
    thpoint_type_export_mp(TT_POINT_TYPE_GUANO,"Guano(")

// ina vypln
    thpoint_type_export_mp(TT_POINT_TYPE_ARCHEO_MATERIAL,"ArcheoMaterial(")
    thpoint_type_export_mp(TT_POINT_TYPE_PALEO_MATERIAL,"PaleoMaterial(")
    thpoint_type_export_mp(TT_POINT_TYPE_VEGETABLE_DEBRIS,"VegetableDebris(")
    thpoint_type_export_mp(TT_POINT_TYPE_ROOT,"Root(")
      
    default:
      fprintf(out->file,"thUndefinedSymbol(");
      break;
  }
  if (postprocess) {
    this->point->export_mp(out);
    double scl = 1.0;
    switch (this->scale) {
      case TT_2DOBJ_SCALE_L:
        scl = 2.0;
        break;
      case TT_2DOBJ_SCALE_XL:
        scl = 4.0;
        break;
      case TT_2DOBJ_SCALE_S:
        scl = 0.5;
        break;
      case TT_2DOBJ_SCALE_XS:
        scl = 0.25;
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


void thpoint_parse_value(int & sv, double & dv, bool & qw, char & sign, char * str) {

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
  char sign, sign2;
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



