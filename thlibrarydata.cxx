/**
 * @file librarydata.cxx
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */


#include "thdatabase.h"
#include "thobjectsrc.h"
#include "thinfnan.h"
#include "thchencdata.h"
#include "thlayout.h"
#include "thlang.h"
#include "thgrade.h"


void thlibrary_init_grades()
{
	thgrade * pgrade;
	char * oname;

	pgrade = (thgrade *) thdb.create("grade", thobjectsrc("therion",0));
	oname = "BCRA3";
	pgrade->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),&oname,0,0);
	oname = "BCRA grade 3";
	pgrade->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),&oname,TT_UTF_8,0);
	pgrade->dls_length = 0.15;
	pgrade->dls_bearing = 3.33;
	pgrade->dls_gradient = 3.33;
	pgrade->dls_counter = thnan;
	pgrade->dls_depth = thnan;
	pgrade->dls_dx = thnan;
	pgrade->dls_dy = thnan;
	pgrade->dls_dz = thnan;
	pgrade->dls_x = 0.15;
	pgrade->dls_y = 0.15;
	pgrade->dls_z = 0.15;
	thdb.insert(pgrade);

	pgrade = (thgrade *) thdb.create("grade", thobjectsrc("therion",0));
	oname = "BCRA5";
	pgrade->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),&oname,0,0);
	oname = "BCRA grade 5";
	pgrade->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),&oname,TT_UTF_8,0);
	pgrade->dls_length = 0.033;
	pgrade->dls_bearing = 0.33;
	pgrade->dls_gradient = 0.33;
	pgrade->dls_counter = thnan;
	pgrade->dls_depth = thnan;
	pgrade->dls_dx = thnan;
	pgrade->dls_dy = thnan;
	pgrade->dls_dz = thnan;
	pgrade->dls_x = 0.033;
	pgrade->dls_y = 0.033;
	pgrade->dls_z = 0.033;
	thdb.insert(pgrade);
}

void thlibrary_init_layouts()
{
	thlayout * playout;
	char * oname;

	playout = (thlayout *) thdb.create("layout", thobjectsrc("therion",0));
	oname = "SCR200";
	playout->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),&oname,TT_UTF_8,0);
	oname = "Computer screen layout";
	playout->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),&oname,TT_UTF_8,0);
	playout->def_scale = true;
	playout->scale = 0.005;
	playout->def_base_scale = false;
	playout->redef_base_scale = false;
	playout->base_scale = 0.005;
	playout->def_rotate = false;
	playout->rotate = 0;
	playout->def_page_setup = true;
	playout->hsize = 0.24;
	playout->vsize = 0.1399;
	playout->paphs = 0.27;
	playout->papvs = 0.2025;
	playout->paghs = 0.26;
	playout->pagvs = 0.1925;
	playout->marts = 0.005;
	playout->marls = 0.005;
	playout->color_map_bg.defined = false;
	playout->color_map_bg.R = 1;
	playout->color_map_bg.G = 1;
	playout->color_map_bg.B = 1;
	playout->color_map_fg.defined = false;
	playout->color_crit = 0;
	playout->color_map_fg.R = 1;
	playout->color_map_fg.G = 1;
	playout->color_map_fg.B = 1;
	playout->color_preview_below.defined = false;
	playout->color_preview_below.R = 0.5;
	playout->color_preview_below.G = 0.5;
	playout->color_preview_below.B = 0.5;
	playout->color_preview_above.defined = false;
	playout->color_preview_above.R = 0;
	playout->color_preview_above.G = 0;
	playout->color_preview_above.B = 0;
	playout->def_overlap = true;
	playout->overlap = 0.01;
	playout->def_scale_bar = false;
	playout->scale_bar = -1;
	playout->def_transparency = false;
	playout->transparency = true;
	playout->def_legend = false;
	playout->legend = TT_LAYOUT_LEGEND_OFF;
	playout->def_legend_width = false;
	playout->legend_width = 0.14;
	playout->def_legend_columns = false;
	playout->legend_columns = 2;
	playout->def_map_header = false;
	playout->map_header = 3;
	playout->map_header_x = 100;
	playout->map_header_y = 0;
	playout->def_debug = false;
	playout->debug = 4;
	playout->def_max_explos = false;
	playout->max_explos = -1;
	playout->def_max_topos = false;
	playout->max_topos = -1;
	playout->def_max_cartos = false;
	playout->max_cartos = -1;
	playout->def_max_copys = false;
	playout->max_copys = -1;
	playout->def_explo_lens = false;
	playout->explo_lens = false;
	playout->def_topo_lens = false;
	playout->topo_lens = false;
	playout->def_lang = false;
	playout->lang = THLANG_UNKNOWN;
	playout->def_layers = false;
	playout->layers = true;
	playout->def_opacity = false;
	playout->opacity = 0.7;
	playout->def_surface_opacity = false;
	playout->surface_opacity = 0.7;
	playout->def_surface= false;
	playout->surface = 0;
	playout->def_grid = false;
	playout->grid = 1;
	playout->def_page_grid = false;
	playout->page_grid = false;
	playout->def_origin = false;
	playout->def_origin_label = true;
	playout->olx = "100";
	playout->oly = "100";
	playout->def_doc_title = false;
	playout->doc_title = "";
	playout->def_doc_comment = false;
	playout->doc_comment = "";
	playout->def_doc_author = false;
	playout->doc_author = "";
	playout->def_doc_subject = false;
	playout->doc_subject = "";
	playout->def_doc_keywords = false;
	playout->doc_keywords = "";
	playout->def_excl_pages = false;
	playout->excl_pages = false;
	playout->excl_list = NULL;
	playout->def_grid_size = true;
	playout->gxs = 10;
	playout->gys = 10;
	playout->gzs = 10;
	playout->def_grid_origin = false;
	playout->def_nav_factor = true;
	playout->navf = 25;
	playout->def_nav_size = true;
	playout->navsx = 2;
	playout->navsy = 2;
	playout->def_own_pages = false;
	playout->ownp = 0;
	playout->def_title_pages = false;
	playout->titlep = false;
	playout->def_page_numbers = false;
	playout->pgsnum = true;
	oname = "SKBB";
	playout->set(thcmd_option_desc(TT_LAYOUT_SYMBOL_DEFAULTS,1),&oname,TT_UTF_8,0);
	playout->def_tex_lines = true;
	thdb.insert(playout);
}

