/**
 * @file thexpmap.cxx
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
 
#include "thexpmap.h"
#include "thexporter.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdb2d.h"
#include "thdb2dmi.h"
#include "thtmpdir.h"
#include "thscrap.h"
#include "thpoint.h"
#include "thline.h"
#include "thlayout.h"
#include "thmap.h"
#include "thconfig.h"
#include <stdio.h>
#include "thtmpdir.h"
#include <unistd.h>
#include "thchenc.h"
#include "thdb1d.h"
#include "thinit.h"
#include "thlogfile.h"
#include <fstream>

#ifdef THWIN32
#define snprintf _snprintf
#endif

thexpmap::thexpmap() {
  this->format = TT_EXPMAP_FMT_UNKNOWN;
  this->projstr = "plan";
  this->layoutstr = "";
  this->layout = new thlayout;
  this->layout->assigndb(&thdb);
  this->layout->id = ++thdb.objid;
  this->projptr = NULL;
}

thexpmap::~thexpmap() {
  delete this->layout;
}

void thexpmap_log_log_file(char * logfpath, char * on_title, char * off_title) {
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
  while (!(lf.eof())) {
    lf.getline(lnbuff,4096);
    thlog.printf("%s\n",lnbuff);
  }
  lf.close();
  delete [] lnbuff;
  thlog.printf("%s",off_title);
}


void thexpmap::parse_options(int & argx, int nargs, char ** args)
{

  int optid, optx; //,sv;
//  double dv;
  bool supform;
  char * fmt;
  //if (argx == 0) {
  //  optid = TT_EXPMAP_OPT_PROJECTION;
  //  optx = argx;
  //}
  //else {
  optid = thmatch_token(args[argx], thtt_expmap_opt);
  optx = argx;
  //}
  switch (optid) {
    case TT_EXPMAP_OPT_FORMAT:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing format -- \"%s\"",args[optx]))
      this->format = thmatch_token(args[argx], thtt_expmap_fmt);
      fmt = args[argx];
      if (this->format == TT_EXPMAP_FMT_UNKNOWN)
        ththrow(("unknown format -- \"%s\"", args[argx]))
      argx++;
      supform = false;
      switch (this->export_mode) {
        case TT_EXP_MAP:
          switch (this->format) {
            case TT_EXPMAP_FMT_PDF:
              supform = true;
          }
          break;
        case TT_EXP_ATLAS:
          switch (this->format) {
            case TT_EXPMAP_FMT_PDF:
              supform = true;
          }
          break;
      }
      if (!supform)
        ththrow(("format not supported -- %s",fmt))
      break;
    case TT_EXPMAP_OPT_PROJECTION:
      argx++;
      if (argx >= nargs)
        ththrow(("missing projection specification -- \"%s\"",args[optx]))
      this->projstr = thdb.strstore(args[argx],true);
      argx++;
      break;
    case TT_EXPMAP_OPT_LAYOUT:
      argx++;
      if (argx >= nargs)
        ththrow(("missing layout -- \"%s\"",args[optx]))
      this->layout->set(thcmd_option_desc(TT_LAYOUT_COPY), &(args[argx]), this->cfgptr->cfg_file.get_cif_encoding(), 0); // = thdb.strstore(args[argx],true);
      this->layoutopts += " -layout ";
      thencode(&(this->cfgptr->bf1), args[argx], this->cfgptr->cfg_file.get_cif_encoding());
      thdecode_arg(&(this->cfgptr->bf2), this->cfgptr->bf1.get_buffer());
      this->layoutopts += this->cfgptr->bf2.get_buffer();
      argx++;
      break;
    default:
      // skusi ci je to -layout-xxx
      if (strncmp(args[optx],"-layout-",8) == 0)
        this->parse_layout_option(argx, nargs, args);
      else
        thexport::parse_options(argx, nargs, args);
      break;
  }
}

void thexpmap::parse_layout_option(int & argx, int nargs, char ** args) {

  // ak ano, tak skontroluje ich pocet
  // pouzije set na nastavenie layoutu
  // ulozi ich - pozor na dekodovanie
  int i;
  char * opts = &((args[argx])[8]);
  thcmd_option_desc o = this->layout->get_cmd_option_desc(opts);
  
  if (o.id == TT_DATAOBJECT_UNKNOWN)
    ththrow(("unknown layout option -- -%s", opts));
    
  if (argx + o.nargs >= nargs)
    ththrow(("not enought layout option arguments -- -%s", opts));
  
  argx++;  
  
  this->layout->set(o, &(args[argx]), this->cfgptr->cfg_file.get_cif_encoding(), 0);
  this->layoutopts += " -layout-";
  this->layoutopts += opts;

  for (i = 0; i < o.nargs; i++) {
    thencode(&(this->cfgptr->bf1), args[argx], this->cfgptr->cfg_file.get_cif_encoding());
    thdecode_arg(&(this->cfgptr->bf2), this->cfgptr->bf1.get_buffer());
    this->layoutopts += " ";
    this->layoutopts += this->cfgptr->bf2.get_buffer();
    argx++;
  }  
 
}

void thexpmap::dump_header(FILE * xf)
{
  thexport::dump_header(xf);
}


void thexpmap::dump_body(FILE * xf)
{
  if (strcmp(this->projstr,"plan") != 0)
    fprintf(xf," -projection %s",this->projstr);
  thexport::dump_body(xf);
  if (this->format != this->get_default_format())
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expmap_fmt));
  thdecode(&(this->cfgptr->bf1), this->cfgptr->cfg_fenc, this->layoutopts.get_buffer());
  fprintf(xf,"%s",this->cfgptr->bf1.get_buffer());
}


void thexpmap::parse_projection(class thdatabase * dbp) {

  thdb2dprjpr prjid = dbp->db2d.parse_projection(this->projstr,false);
  if (prjid.newprj) {
    thwarning(("%s [%d] -- no projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }
  this->projptr = prjid.prj;
  
}



void thexpmap::process_db(class thdatabase * dbp) 
{
  
  if (this->projptr == NULL)
    return;

  if (this->format == TT_EXPMAP_FMT_UNKNOWN)
    this->format = this->get_default_format();

  thdb.db2d.process_projection(this->projptr);
  if (this->projptr->first_scrap == NULL) {
    thwarning(("%s [%d] -- no projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }
  
  // parse layout
  // thlayout * tmp = NULL;
  // this->layout = dbp->get_layout(this->layoutstr);
  // if (this->layout == NULL) {
  //  tmp = new thlayout;
  //  this->layout = tmp;
  // }
  this->layout->process_copy();
  
  // the export it self
  switch (this->format) {
    case TT_EXPMAP_FMT_PDF:
      this->export_pdf(thdb.db2d.select_projection(this->projptr),this->projptr);
      break;
  }
  
  //if (tmp != NULL) {
  //  tmp->self_delete();
  //}
}

int thexpmap::get_default_format() {
  if (this->export_mode == TT_EXP_MAP)
    return TT_EXPMAP_FMT_PDF;
  else 
    return TT_EXPMAP_FMT_PDF;
}

enum {
  TT_XMPS_NONE = 0,
  TT_XMPS_COUNT = 5,
  TT_XMPS_F = 1,
  TT_XMPS_COUNT_F = 0,
  TT_XMPS_B = 2,
  TT_XMPS_COUNT_B = 1,
  TT_XMPS_E = 4,
  TT_XMPS_COUNT_E = 2,
  TT_XMPS_X = 8,
  TT_XMPS_COUNT_X = 3,
};

char * thexpmap_u2string(unsigned u) {
  static char a [8];
  a[7] = 0;
  unsigned v = 1;
  for(int i = 6; i >= 0; i--) {
    a[i] = 'a' + ((u / v) % 10);
    v *= 10;
  }
  return (&(a[0]));
}

void thexpmap::export_pdf(thdb2dxm * maps, thdb2dprj * prj) {

  // poojde kapitolu za kapitolou a exportuje scrapy a 
  // vytvori hashe -> layer a scrap
  // spusti mp - thpdf - pdftex

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }
    
  FILE * mpf, * plf, *tf;
  unsigned sfig = 1;
  unsigned exps;
  char * fnm, * chtitle;
  bool quick_map_exp = false;
  double origin_shx, origin_shy;
  thexpmapmpxs out;
  th2ddataobject * op2;
  bool export_sections, export_outlines_only;
  double shx, shy;
  unsigned long sclevel, bmlevel;

  bool anyprev, anyprevabove = false, anyprevbelow = false;
  char * prevbf;
  thbuffer aboveprev, belowprev;
  
  if (this->outpt_def)
    fnm = this->outpt;
  else
    fnm = "cave.pdf";

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;
  thbuffer encb, texb;
  thscrap * cs;

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ...\n", fnm);
//  thtext_inline = true;
#endif 

  out.ms = this->layout->scale * 2834.64566929;
  // korekcia shiftu na layout origin
  if (!thisnan(this->layout->ox)) {
    switch (prj->type) {
      case TT_2DPROJ_EXTEND:
      case TT_2DPROJ_ELEV:
        origin_shx = (prj->shift_x - this->layout->ox) * out.ms;
        origin_shy = (prj->shift_z - this->layout->oz) * out.ms;
        break;
      default:
        origin_shx = (prj->shift_x - this->layout->ox) * out.ms;
        origin_shy = (prj->shift_y - this->layout->oy) * out.ms;
      break;
    }
  } else {
    origin_shx = 0.0;
    origin_shy = 0.0;
  }
  // koniec korekcie shiftu na layout origin
      
  tf = fopen(thtmp.get_file_name("Config"),"w");
  this->layout->export_config(tf,prj,out.ms,origin_shx,origin_shy);     
  fclose(tf);

  tf = fopen(thtmp.get_file_name("data.tex"),"w");
  fprintf(tf,"\\input therion.tex\n");
  this->layout->export_pdftex(tf,prj);     
  fprintf(tf,"\\end\n");
  fclose(tf);



  if (thexporter_quick_map_export && 
      (thexpmap_quick_map_export_scale == this->layout->scale)) {
    quick_map_exp = true;
    goto QUICK_MAP_EXPORT;
  }

  plf = fopen(thtmp.get_file_name("data.pl"),"w");     
  mpf = fopen(thtmp.get_file_name("data.mp"),"w");     

  out.file = mpf;
  
  thexpmap_quick_map_export_scale = this->layout->scale;
  fprintf(mpf,"Scale:=%f;\n",1 / this->layout->scale);
  fprintf(mpf,"input therion.mp;\n");
  fprintf(mpf,"verbatimtex \\font\\default=csss10\\default etex;\n");
  fprintf(mpf,"verbatimtex \\font\\it=csssi8 etex;\n");

  // prida nultu figure
  // fprintf(mpf,"beginfig(0);\nendfig;\n");
  
 
  fprintf(plf,"%%SCRAP = (\n");
  while (cmap != NULL) {
    cbm = cmap->first_bm;
    bmlevel = 0;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      sclevel = 0;
      // !!! Tu pridat aj ine druhy - teda ABOVE a BELOW
      export_outlines_only = ((cbm->mode == TT_MAPITEM_ABOVE) ||
        (cbm->mode == TT_MAPITEM_BELOW)) && (!cbm->bm->selection_xs->previewed)
        && (cbm->bm->selection_xs->fmap->output_number != cbm->bm->selection_xs->preview_output_number);
      if ((cbm->mode == TT_MAPITEM_NORMAL) || export_outlines_only) {
        if (export_outlines_only) {
          cbm->bm->selection_xs->previewed = true;
        }
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            cs = (thscrap *) cmi->object;
            export_sections = false;
            op2 = NULL;
            while ((!export_sections) || (op2 != NULL)) {
              if (export_sections) {
                if ((op2->get_class_id() == TT_POINT_CMD) &&
                  (((thpoint *)op2)->type == TT_POINT_TYPE_SECTION) &&
                  (((thpoint *)op2)->text != NULL)) {
                    cs = (thscrap *)((thpoint *)op2)->text;
                    thdb.db2d.process_projection(cs->proj);
                }
                else
                  cs = NULL;
              }
              if (cs != NULL) {
                if (export_sections) {
                  out.mx = (cs->lxmax + cs->lxmin) / 2.0;
                  out.my = (cs->lymax + cs->lymin) / 2.0;
                  shx = ((thpoint *)op2)->point->xt;
                  shy = ((thpoint *)op2)->point->yt;
                  switch (((thpoint *)op2)->align) {
                    case TT_POINT_ALIGN_B:
                      shy -= (cs->lymax - cs->lymin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_T:
                      shy += (cs->lymax - cs->lymin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_R:
                      shx += (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_L:
                      shx -= (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_BR:
                      shy -= (cs->lymax - cs->lymin) / 2.0;
                      shx += (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_BL:
                      shy -= (cs->lymax - cs->lymin) / 2.0;
                      shx -= (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_TR:
                      shy += (cs->lymax - cs->lymin) / 2.0;
                      shx += (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                    case TT_POINT_ALIGN_TL:
                      shy += (cs->lymax - cs->lymin) / 2.0;
                      shx -= (cs->lxmax - cs->lxmin) / 2.0;
                      break;
                  }
                  shx *= out.ms;
                  shy *= out.ms;
                } else {
                  out.mx = (cs->lxmax + cs->lxmin) / 2.0;
                  out.my = (cs->lymax + cs->lymin) / 2.0;
                  shx = out.mx * out.ms;
                  shy = out.my * out.ms;
                } 
                
                shx += origin_shx;
                shy += origin_shy;
                
                exps = this->export_mp(& out, cs, sfig, export_outlines_only);
                // naozaj ho exportujeme
                if (exps != TT_XMPS_NONE) {
                  fprintf(plf,"\t%s => {\n",thexpmap_u2string(sfig));
                  if (export_sections)
                    fprintf(plf,"\t\t Z => 1,\n");    
                  // pred orezanim
                  if ((exps & TT_XMPS_F) != TT_XMPS_NONE)
                    fprintf(plf,"\t\t F => \"data.%d\",\n",sfig+TT_XMPS_COUNT_F);
//                  else
//                    fprintf(plf,"\t\t F => \"data.0\",\n");
    
                  // orezavacia cesta a outlines
                  if ((exps & TT_XMPS_B) != TT_XMPS_NONE) {
                    fprintf(plf,"\t\t B => \"data.%d\",\n",sfig+TT_XMPS_COUNT_B);
                    fprintf(plf,"\t\t I => \"data.%dbg\",\n",sfig+TT_XMPS_COUNT_B);
                    fprintf(plf,"\t\t C => \"data.%dclip\",\n",sfig+TT_XMPS_COUNT_B);
                  }
//                  else {
//                    fprintf(plf,"\t\t B => \"data.0\",\n");
//                    fprintf(plf,"\t\t I => \"data.0\",\n");
//                    fprintf(plf,"\t\t C => \"data.0\",\n");
//                  }
    
                  // po orezani
                  if ((exps & TT_XMPS_E) != TT_XMPS_NONE)
                    fprintf(plf,"\t\t E => \"data.%d\",\n",sfig+TT_XMPS_COUNT_E);
//                  else
//                    fprintf(plf,"\t\t E => \"data.0\",\n");
    
                  if ((exps & TT_XMPS_X) != TT_XMPS_NONE) {
                    fprintf(plf,"\t\t X => \"data.%d\",\n",sfig+TT_XMPS_COUNT_X);
                    fprintf(plf,"\t\t P => \"data.%dbbox\",\n",sfig+TT_XMPS_COUNT_X);
                  }

                  if (export_outlines_only) {
                    fprintf(plf,"\t\t Y => %ld,\n",cbm->bm->selection_xs->preview_output_number);
                    fprintf(plf,"\t\t V => -1,\n");
                  } else {
                    fprintf(plf,"\t\t Y => %ld,\n",cmap->output_number);
                    fprintf(plf,"\t\t V => -%ld,\n",cmi->itm_level + bmlevel);
                  }
                  
                  fprintf(plf,"\t\t S => \"%.2f %.2f\",\n\t},\n",shx,shy);
                  
                  if ((!export_outlines_only) && (!export_sections) &&
                      (cbm->bm->selection_xs->fmap->output_number != cbm->bm->selection_xs->preview_output_number)) {
                    fprintf(plf,"\t%s => {\n",thexpmap_u2string(sfig + TT_XMPS_COUNT));
                    if ((exps & TT_XMPS_B) != TT_XMPS_NONE) {
                      fprintf(plf,"\t\t B => \"data.%d\",\n",sfig+TT_XMPS_COUNT_B);
                      fprintf(plf,"\t\t I => \"data.%dbg\",\n",sfig+TT_XMPS_COUNT_B);
                      fprintf(plf,"\t\t C => \"data.%dclip\",\n",sfig+TT_XMPS_COUNT_B);
                    }
                    fprintf(plf,"\t\t Y => %ld,\n",cbm->bm->selection_xs->preview_output_number);
                    fprintf(plf,"\t\t V => -1,\n");
                    fprintf(plf,"\t\t S => \"%.2f %.2f\",\n\t},\n",shx,shy);
                  }

                  sfig += TT_XMPS_COUNT;
                  
                }
              } // if cs != NULL
              if (!export_sections) {
                export_sections = true;
                if (export_outlines_only)
                  op2 = NULL;
                else
                  op2 = cs->ls2doptr;
              } else {
                op2 = op2->pscrapoptr;
              }
            } // koniec exportovania priecnych rezov 
          }
          cmi = cmi->prev_item;  
        }
        bmlevel += cbm->bm->last_level;
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  fprintf(mpf,"end;\n");
  fprintf(plf,");\n");
  fclose(mpf);

  cmap = maps;
  while (cmap != NULL) {
    if (cmap->expand) {      
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        cbm->bm->selection_xs->previewed = false;
        cbm = cbm->next_item;
      }
    }
    cmap = cmap->next_item;
  }
  
  cmap = maps;  
  prevbf = new char [128];
  prevbf[127] = 0;
  
  chtitle = NULL;
  fprintf(plf,"\n\n%%LAYER = (\n");
  while (cmap != NULL) {
    if (cmap->title)
      chtitle = (strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name);
    if (cmap->expand) {      
    
      fprintf(plf,"\t%ld => {\n",cmap->output_number);
      
      anyprev = false;
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if (cbm->mode == TT_MAPITEM_ABOVE) {
          if (!anyprev) {
            fprintf(plf,"\t\tU => \"%ld",cbm->bm->selection_xs->preview_output_number);
            anyprev = true;
          } else
            fprintf(plf," %ld",cbm->bm->selection_xs->preview_output_number);
        }
        cbm = cbm->next_item;
      }
      if (anyprev)
            fprintf(plf,"\",\n");      
      

      thdecode(& texb,TT_ISO8859_2,(strlen(cmap->map->title) > 0 ? cmap->map->title : cmap->map->name));      
      thdecode_tex(& encb, texb.get_buffer());
      fprintf(plf,"\t\tN => '%s',\n",encb.get_buffer());
      if ((chtitle != NULL) && ((cmap->next_item == NULL) || (cmap->next_item->title))) {
        thdecode(& texb,TT_ISO8859_2,chtitle);      
        thdecode_tex(& encb, texb.get_buffer());
        fprintf(plf,"\t\tT => '%s',\n",encb.get_buffer());
        chtitle = NULL;
      }
      
      
      anyprev = false;
      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if (cbm->mode == TT_MAPITEM_BELOW) {
          if (!anyprev) {
            fprintf(plf,"\t\tD => \"%ld",cbm->bm->selection_xs->preview_output_number);
            anyprev = true;
          } else
            fprintf(plf," %ld",cbm->bm->selection_xs->preview_output_number);
        }
        cbm = cbm->next_item;
      }
      if (anyprev)
            fprintf(plf,"\",\n");      
      
      
      fprintf(plf,"\t},\n");

      cbm = cmap->first_bm;
      while (cbm != NULL) {
        if ((! cbm->bm->selection_xs->previewed) && 
            (cbm->bm->selection_xs->fmap->output_number != cbm->bm->selection_xs->preview_output_number)) {
          cbm->bm->selection_xs->previewed = true;
          fprintf(plf,"\t%ld => {\n",cbm->bm->selection_xs->preview_output_number);
          fprintf(plf,"\t\tZ => 1,\n");
          fprintf(plf,"\t},\n");
          switch (cbm->bm->selection_mode) {
            case TT_MAPITEM_BELOW:
              if (!anyprevbelow) {
                snprintf(prevbf,127,"%ld",cbm->bm->selection_xs->preview_output_number);
                belowprev += prevbf;
                anyprevbelow = true;
              } else {
                snprintf(prevbf,127," %ld",cbm->bm->selection_xs->preview_output_number);
                belowprev += prevbf;
              }
              break;
            case TT_MAPITEM_ABOVE:
              if (!anyprevabove) {
                snprintf(prevbf,127,"%ld",cbm->bm->selection_xs->preview_output_number);
                aboveprev += prevbf;
                anyprevabove = true;
              } else {
                snprintf(prevbf,127," %ld",cbm->bm->selection_xs->preview_output_number);
                aboveprev += prevbf;
              }
              break;
          }
        }
        cbm = cbm->next_item;
      }
    }
    cmap = cmap->next_item; 
  }
  fprintf(plf,");\n");
  
  // map preview hash
  if (anyprevabove) {
    fprintf(plf,"\n\n$MAP_PREVIEW{U} = \"%s\";",aboveprev.get_buffer());
  }
  if (anyprevbelow) {
    fprintf(plf,"\n\n$MAP_PREVIEW{D} = \"%s\";",belowprev.get_buffer());
  }
  
  fprintf(plf,"\n\n\n$PATTERN_DEFS = \"./\";\n");
  fclose(plf);
  
  delete [] prevbf;

  QUICK_MAP_EXPORT:
  
  // teraz sa hodi do temp adresara - spusti metapost, thpdf, a pdftex a skopiruje vysledok
  thbuffer com, wdir;
  wdir.guarantee(1024);
  getcwd(wdir.get_buffer(),1024);
  chdir(thtmp.get_dir_name());
  int retcode;
  
  if (!quick_map_exp) {
    com = "\"";
    com += thini.get_path_mpost();
    com += "\"";
//    com += " --interaction nonstopmode data.mp";
    com += " data.mp";
#ifdef THDEBUG
    thprintf("running metapost\n");
#endif
    retcode = system(com.get_buffer());
    thexpmap_log_log_file("data.log",
    "####################### metapost log file ########################\n",
    "#################### end of metapost log file ####################\n");
    if (retcode != EXIT_SUCCESS) {
      chdir(wdir.get_buffer());
      ththrow(("metapost exit code -- %d", retcode))
    }
  }

  if (quick_map_exp)
    com = "thpdf -q";
  else
    com = "thpdf";

  if (this->export_mode == TT_EXP_MAP)
    com += " -m data.pl";
  else
    com += " data.pl";
    
#ifdef THDEBUG
  thprintf("running thpdf\n");
#endif
  retcode = system(com.get_buffer());
  if (retcode != EXIT_SUCCESS) {
    chdir(wdir.get_buffer());
    ththrow(("thpdf exit code -- %d", retcode))
  }

  com = "\"";
  com += thini.get_path_pdftex();
  com += "\"";
//  com += " --interaction nonstopmode data.tex";
  com += " data.tex";
#ifdef THDEBUG
  thprintf("running pdftex\n");
#endif
  retcode = system(com.get_buffer());
  thexpmap_log_log_file("data.log",
  "######################## pdftex log file #########################\n",
  "##################### end of pdftex log file #####################\n");
  if (retcode != EXIT_SUCCESS) {
    chdir(wdir.get_buffer());
    ththrow(("pdftex exit code -- %d", retcode))
  }

    
  // Let's copy results and log-file to working directory
  chdir(wdir.get_buffer());
#ifdef THWIN32
  com = "copy ";
#else
  com = "cp ";
#endif
  com += thtmp.get_file_name("data.pdf");
  com += " ";
  com += fnm;
#ifdef THDEBUG
  thprintf("copying results\n");
#endif
  retcode = system(com.get_buffer());
  if (retcode != EXIT_SUCCESS)
    ththrow(("cp exit code -- %d", retcode))
#ifdef THDEBUG
#else
  thprintf("done.\n");
  thtext_inline = false;
#endif
  
}


unsigned thexpmap::export_mp(thexpmapmpxs * out, class thscrap * scrap, 
    unsigned startnum, bool outline_mode)
{
  th2ddataobject * obj;
  unsigned result = TT_XMPS_NONE, from, to;
  int placeid;
  thscraplo * lo, * lo2;
  bool somex, first, vis;
  thscraplp * slp;
  thdb2dlp * lp;
  
  // preskuma vsetky objekty a ponastavuje im clip tagy
  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    if ((obj->tags & TT_2DOBJ_TAG_CLIP_AUTO) > 0) {
        obj->tags &= ~(TT_2DOBJ_TAG_CLIP_AUTO | TT_2DOBJ_TAG_CLIP_ON);
      switch (obj->get_class_id()) {
        case TT_POINT_CMD:
          switch (((thpoint*)obj)->type) {
            case TT_POINT_TYPE_BEDROCK:
            case TT_POINT_TYPE_SAND:
            case TT_POINT_TYPE_RAFT:
            case TT_POINT_TYPE_CLAY:
            case TT_POINT_TYPE_PEBBLES:
            case TT_POINT_TYPE_DEBRIS:
            case TT_POINT_TYPE_BLOCKS:
            case TT_POINT_TYPE_WATER:
            case TT_POINT_TYPE_ICE:
            case TT_POINT_TYPE_GUANO:
              obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
              break;
          }
          break;
        case TT_LINE_CMD:
          if ((((thline*)obj)->outline == TT_LINE_OUTLINE_NONE)) { 
            switch (((thline*)obj)->type) {
              case TT_LINE_TYPE_SECTION:
              case TT_LINE_TYPE_ARROW:
              case TT_LINE_TYPE_LABEL:
                break;
              default:
                obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
                break;
            }
          }  
          break;
        case TT_AREA_CMD:
          obj->tags |= TT_2DOBJ_TAG_CLIP_ON;
          break;
      }
    }  
    obj = obj->pscrapoptr;
  }
  
  
  // vypise objekty pred orezanim
  somex = false;
#define thexpmap_export_mp_bgif if (!somex) \
  fprintf(out->file,"beginfig(%d);\n",startnum + TT_XMPS_COUNT_F); \
  somex = true
  
  // najprv vyplne
  for (placeid = TT_2DOBJ_PLACE_BOTTOM; placeid <= TT_2DOBJ_PLACE_TOP; placeid++) {
    obj = scrap->ls2doptr;
    if (outline_mode) obj = NULL;
    while (obj != NULL) {
      if (((obj->tags & TT_2DOBJ_TAG_CLIP_ON) > 0) && (obj->place == placeid)) {
        thexpmap_export_mp_bgif;
        obj->export_mp(out);
      }
      obj = obj->pscrapoptr;
    }
  }

  // a polygon
  slp = scrap->get_polygon();
  if (outline_mode) slp = NULL;
  while (slp != NULL) {
    if (slp->lnio) {
      thexpmap_export_mp_bgif;
      fprintf(out->file,"Polygon(((%.2f,%.2f) -- (%.2f,%.2f)));\n",
          (slp->lnx1 - out->mx) * out->ms,
          (slp->lny1 - out->my) * out->ms,
          (slp->lnx2 - out->mx) * out->ms,
          (slp->lny2 - out->my) * out->ms);
    }
    slp = slp->next_item;
  }


  if (somex) {
    fprintf(out->file,"endfig;\n");
    result |= TT_XMPS_F;
  }
  
  
  // vypise outline
  somex = false;
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) \
  fprintf(out->file,"beginfig(%d);\n",startnum + TT_XMPS_COUNT_B); \
  somex = true
  
  lo = scrap->get_outline();
  //if ((lo != NULL) && (lo->line->outline == TT_LINE_OUTLINE_OUT)) {
  if (true) {  /// ;-) uz mame podporovanych viac vonkajsich a viac vnut. obrysov 
    while (lo != NULL) {
      thexpmap_export_mp_bgif;
      fprintf(out->file,"draw_upscrap(%d)(",((lo->line->outline == TT_LINE_OUTLINE_OUT) ? 1 : 0));
      // prejde celu krivku a rozlozi ju na viditelne a neviditelne casti
      first = true;
      lo2 = lo;
      while (lo2 != NULL) {              
        
        if (lo2->mode == TT_OUTLINE_REVERSED) {
          lp = lo2->line->first_point;
          from = 0;
          while (lp != NULL) {
            lp = lp->nextlp;
            from++;
          }
          from = from - 1;
          lp = lo2->line->last_point;
          to = from;
        } else {
          lp = lo2->line->first_point;
          from = 0;
          to = from;
        }
        
        bool cvis = false;
        while (lp != NULL) {

          // najde viditelnost prveho bodu
          if (lo2->mode == TT_OUTLINE_REVERSED) {
            if (lp->prevlp != NULL)
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
              (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
            else
              vis = cvis;
          } else {
            vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
              (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
          }
          cvis = vis;
          
          // pokracuje pokial nemame bod z inou viditelnostou
          while (lp != NULL) {
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              if (lp->prevlp != NULL)
                vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
              else
                vis = cvis;
            } else {
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
            }
            
            if (vis != cvis)
              break;
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              lp = lp->prevlp;
              if (lp != NULL)
                from--;
            } else {
              lp = lp->nextlp;
              if (lp != NULL)
                to++;
            }
          }

          if (!first) {
            fprintf(out->file,",\n");
          }
          first = false;
                    
          if (cvis) {
            fprintf(out->file,"1,");
          } else {
            fprintf(out->file,"0,");
          }
          
          if (lo2->mode == TT_OUTLINE_REVERSED) {
            fprintf(out->file,"reverse ");
            lo2->line->export_path_mp(out,from,to);
            to = from;
          } else {
            lo2->line->export_path_mp(out,from,to);
            from = to;
          }
          
        }
        
        lo2 = lo2->next_line;
      }
      fprintf(out->file,");\n");
      lo = lo->next_outline;
    }
  }
  if (somex) {
    fprintf(out->file,"draw_downscrap;\n");
    fprintf(out->file,"endfig;\n");
    result |= TT_XMPS_B;
  }
  
  // vypise objekty po orezani
  somex = false;
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) \
  fprintf(out->file,"beginfig(%d);\n",startnum + TT_XMPS_COUNT_E); \
  somex = true
  
  for (placeid = TT_2DOBJ_PLACE_BOTTOM; placeid <= TT_2DOBJ_PLACE_TOP; placeid++) {
    obj = scrap->ls2doptr;
    if (outline_mode) obj = NULL;
    while (obj != NULL) {
      if (((obj->tags & TT_2DOBJ_TAG_CLIP_ON) == 0) && (obj->place == placeid)) {
        switch (obj->get_class_id()) {
          case TT_LINE_CMD:
            switch (((thline*)obj)->type) {
              case TT_LINE_TYPE_LABEL:
                break;
              default:
                thexpmap_export_mp_bgif;
                obj->export_mp(out);
                break;
            }
            break;
          case TT_POINT_CMD:
            switch (((thpoint*)obj)->type) {
              case TT_POINT_TYPE_STATION:
              case TT_POINT_TYPE_STATION_NAME:
              case TT_POINT_TYPE_LABEL:
              case TT_POINT_TYPE_REMARK:
              case TT_POINT_TYPE_DATE:
              case TT_POINT_TYPE_ALTITUDE:
              case TT_POINT_TYPE_HEIGHT:
              case TT_POINT_TYPE_PASSAGE_HEIGHT:
                break;
              default:
                thexpmap_export_mp_bgif;
                obj->export_mp(out);
                break;
            }
            break;
          case TT_AREA_CMD:
            thexpmap_export_mp_bgif;
            obj->export_mp(out);
            break;
        }
      }
      obj = obj->pscrapoptr;
    }
  }
    
  // nakoniec meracske body
  // najprv z polygonu
  slp = scrap->get_polygon();
  if (outline_mode) slp = NULL;
  while (slp != NULL) {
    if (!slp->lnio) {
      thexpmap_export_mp_bgif;
      switch(slp->station->mark) {
        case TT_DATAMARK_FIXED:
          fprintf(out->file,"FixedStation(");
          break;
        case TT_DATAMARK_NATURAL:
          fprintf(out->file,"NaturalStation(");
          break;
        case TT_DATAMARK_PAINTED:
          fprintf(out->file,"PaintedStation(");
          break;
        default:
          fprintf(out->file,"TemporaryStation(");
          break;
      }
      fprintf(out->file,"(%.2f,%.2f));\n",
        (slp->stx - out->mx) * out->ms,
        (slp->sty - out->my) * out->ms);
    }
    slp = slp->next_item;
  }
  
  // potom zo scrapu
  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    if ((obj->tags & TT_2DOBJ_TAG_CLIP_ON) == 0) {
      switch (obj->get_class_id()) {
        case TT_POINT_CMD:
          if (((thpoint*)obj)->type == TT_POINT_TYPE_STATION) {
            thexpmap_export_mp_bgif;
            obj->export_mp(out);
          }
          break;
      }
    }
    obj = obj->pscrapoptr;
  }
  
  if (somex) {
    fprintf(out->file,"endfig;\n");  
    result |= TT_XMPS_E;
  }
  
  // teraz spracuje labels
#undef thexpmap_export_mp_bgif
#define thexpmap_export_mp_bgif if (!somex) \
  fprintf(out->file,"beginfig(%d);\n",startnum + TT_XMPS_COUNT_X); \
  somex = true
  somex = false;

  obj = scrap->ls2doptr;
  if (outline_mode) obj = NULL;
  while (obj != NULL) {
    switch (obj->get_class_id()) {
    
      // altitude labels zo steny
      case TT_LINE_CMD:
        switch (((thline*)obj)->type) {
          case TT_LINE_TYPE_WALL:
            // prescanuje stenu
            lp = ((thline*)obj)->first_point;
            while(lp != NULL) {
              if (((lp->tags | TT_LINEPT_TAG_ALTITUDE) > 0) &&
                  (!thisnan(lp->rsize))) {
                thexpmap_export_mp_bgif;
                fprintf(out->file,"Altitude(");
                lp->export_prevcp_mp(out);
                fprintf(out->file,",");
                lp->point->export_mp(out);
                fprintf(out->file,",");
                lp->export_nextcp_mp(out);
                fprintf(out->file,",\"%.0f\");\n",lp->rsize);
              }
              lp = lp->nextlp;
            }
            break;
          case TT_LINE_TYPE_LABEL:
            thexpmap_export_mp_bgif;
            obj->export_mp(out);
            break;
        }
        break;
      case TT_POINT_CMD:
        switch (((thpoint*)obj)->type) {
          case TT_POINT_TYPE_LABEL:
          case TT_POINT_TYPE_REMARK:
          case TT_POINT_TYPE_STATION_NAME:
          case TT_POINT_TYPE_DATE:
          case TT_POINT_TYPE_ALTITUDE:
          case TT_POINT_TYPE_HEIGHT:
          case TT_POINT_TYPE_PASSAGE_HEIGHT:
            thexpmap_export_mp_bgif;
            ((thpoint*)obj)->export_mp(out);
            break;
        }
        break;
    }
    obj = obj->pscrapoptr;
  }
  

  if (somex) {
    fprintf(out->file,"endfig;\n");  
    result |= TT_XMPS_X;
  }

  return result;
}

double thexpmap_quick_map_export_scale;


