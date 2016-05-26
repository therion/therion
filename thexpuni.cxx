/**
 * @file thexpuni.cxx
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
#include "thlayout.h"
#include "thmap.h"
#include "thsketch.h"
#include "thconfig.h"
#include <stdio.h>
#include "thtmpdir.h"
#include "thcsdata.h"
#include "thdb3d.h"

#ifdef THMSVC
#define hypot _hypot
#define snprintf _snprintf
#define strcasecmp _stricmp
#endif

#include "thchenc.h"
#include "thdb1d.h"
#include "thinit.h"
#include "thlogfile.h"
#include "thcmdline.h"
#include "thsurvey.h"
#include "thchenc.h"
#include <fstream>
#include "thmapstat.h"
#include "thsurface.h"
#include <stdlib.h>
#include "loch/lxMath.h"
#include "extern/shapefil.h"
#include "thexpmodel.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "thexpuni.h"
#include "thproj.h"
#include "thcs.h"
#include "thtexfonts.h"
#include "thlang.h"


static const char * DXFpre = 
"  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n  1\nAC1014\n  9\n$ACADMAINTVER\n"
" 70\n     0\n  9\n$DWGCODEPAGE\n  3\nANSI_1252\n  9\n$INSBASE\n"
" 10\n0.0\n 20\n0.0\n 30\n0.0\n  9\n$EXTMIN\n 10\n%.3f\n 20\n"
"%.3f\n 30\n0.0\n  9\n$EXTMAX\n 10\n%.3f\n 20\n%.3f\n"
" 30\n0.0\n  9\n$ORTHOMODE\n 70\n     1\n  9\n$REGENMODE\n"
" 70\n     1\n  9\n$FILLMODE\n 70\n     1\n  9\n$QTEXTMODE\n 70\n"
"     0\n  9\n$MIRRTEXT\n 70\n     1\n  9\n$DRAGMODE\n 70\n     2\n"
"  9\n$LTSCALE\n 40\n1.0\n  9\n$OSMODE\n 70\n     0\n  9\n$ATTMODE\n"
" 70\n     1\n  9\n$TEXTSIZE\n 40\n2.5\n  9\n$TRACEWID\n 40\n1.0\n"
"  9\n$TEXTSTYLE\n  7\nSTANDARD\n  9\n$CLAYER\n  8\n0\n  9\n$CELTYPE\n"
"  6\nBYLAYER\n  9\n$CECOLOR\n 62\n   256\n  9\n$CELTSCALE\n 40\n"
"1.0\n  9\n$DELOBJ\n 70\n     1\n  9\n$DISPSILH\n 70\n     0\n"
"  9\n$DIMSCALE\n 40\n1.0\n  9\n$DIMASZ\n 40\n2.5\n  9\n$DIMEXO\n"
" 40\n0.625\n  9\n$DIMDLI\n 40\n3.75\n  9\n$DIMRND\n 40\n0.0\n"
"  9\n$DIMDLE\n 40\n0.0\n  9\n$DIMEXE\n 40\n1.25\n  9\n$DIMTP\n"
" 40\n0.0\n  9\n$DIMTM\n 40\n0.0\n  9\n$DIMTXT\n 40\n2.5\n  9\n"
"$DIMCEN\n 40\n2.5\n  9\n$DIMTSZ\n 40\n0.0\n  9\n$DIMTOL\n 70\n"
"     0\n  9\n$DIMLIM\n 70\n     0\n  9\n$DIMTIH\n 70\n     0\n"
"  9\n$DIMTOH\n 70\n     0\n  9\n$DIMSE1\n 70\n     0\n  9\n$DIMSE2\n"
" 70\n     0\n  9\n$DIMTAD\n 70\n     1\n  9\n$DIMZIN\n 70\n     8\n"
"  9\n$DIMBLK\n  1\n\n  9\n$DIMASO\n 70\n     1\n  9\n$DIMSHO\n"
" 70\n     1\n  9\n$DIMPOST\n  1\n\n  9\n$DIMAPOST\n  1\n\n  9\n"
"$DIMALT\n 70\n     0\n  9\n$DIMALTD\n 70\n     4\n  9\n$DIMALTF\n"
" 40\n0.0394\n  9\n$DIMLFAC\n 40\n1.0\n  9\n$DIMTOFL\n 70\n     1\n"
"  9\n$DIMTVP\n 40\n0.0\n  9\n$DIMTIX\n 70\n     0\n  9\n$DIMSOXD\n"
" 70\n     0\n  9\n$DIMSAH\n 70\n     0\n  9\n$DIMBLK1\n  1\n\n"
"  9\n$DIMBLK2\n  1\n\n  9\n$DIMSTYLE\n  2\nSTANDARD\n  9\n$DIMCLRD\n"
" 70\n     0\n  9\n$DIMCLRE\n 70\n     0\n  9\n$DIMCLRT\n 70\n"
"     0\n  9\n$DIMTFAC\n 40\n1.0\n  9\n$DIMGAP\n 40\n0.625\n  9\n"
"$DIMJUST\n 70\n     0\n  9\n$DIMSD1\n 70\n     0\n  9\n$DIMSD2\n"
" 70\n     0\n  9\n$DIMTOLJ\n 70\n     1\n  9\n$DIMTZIN\n 70\n"
"     0\n  9\n$DIMALTZ\n 70\n     0\n  9\n$DIMALTTZ\n 70\n     0\n"
"  9\n$DIMFIT\n 70\n     3\n  9\n$DIMUPT\n 70\n     0\n  9\n$DIMUNIT\n"
" 70\n     8\n  9\n$DIMDEC\n 70\n     4\n  9\n$DIMTDEC\n 70\n     4\n"
"  9\n$DIMALTU\n 70\n     8\n  9\n$DIMALTTD\n 70\n     4\n  9\n"
"$DIMTXSTY\n  7\nSTANDARD\n  9\n$DIMAUNIT\n 70\n     0\n  9\n$LUNITS\n"
" 70\n     2\n  9\n$LUPREC\n 70\n     4\n  9\n$SKETCHINC\n 40\n"
"1.0\n  9\n$FILLETRAD\n 40\n10.0\n  9\n$AUNITS\n 70\n     0\n  9\n"
"$AUPREC\n 70\n     0\n  9\n$MENU\n  1\n.\n  9\n$ELEVATION\n 40\n"
"0.0\n  9\n$PELEVATION\n 40\n0.0\n  9\n$THICKNESS\n 40\n0.0\n  9\n"
"$LIMCHECK\n 70\n     0\n  9\n$BLIPMODE\n 70\n     0\n  9\n$CHAMFERA\n"
" 40\n10.0\n  9\n$CHAMFERB\n 40\n10.0\n  9\n$CHAMFERC\n 40\n20.0\n"
"  9\n$CHAMFERD\n 40\n0.0\n  9\n$SKPOLY\n 70\n     0\n  9\n$TDCREATE\n"
" 40\n2454180.933710197\n  9\n$TDUPDATE\n 40\n2454180.933710197\n"
"  9\n$TDINDWG\n 40\n0.0000000000\n  9\n$TDUSRTIMER\n 40\n0.0000000000\n"
"  9\n$USRTIMER\n 70\n     1\n  9\n$ANGBASE\n 50\n0.0\n  9\n$ANGDIR\n"
" 70\n     0\n  9\n$PDMODE\n 70\n     0\n  9\n$PDSIZE\n 40\n0.0\n"
"  9\n$PLINEWID\n 40\n0.0\n  9\n$COORDS\n 70\n     1\n  9\n$SPLFRAME\n"
" 70\n     0\n  9\n$SPLINETYPE\n 70\n     6\n  9\n$SPLINESEGS\n"
" 70\n     8\n  9\n$ATTDIA\n 70\n     0\n  9\n$ATTREQ\n 70\n     1\n"
"  9\n$HANDLING\n 70\n     1\n  9\n$HANDSEED\n  5\n3A\n  9\n$SURFTAB1\n"
" 70\n     6\n  9\n$SURFTAB2\n 70\n     6\n  9\n$SURFTYPE\n 70\n"
"     6\n  9\n$SURFU\n 70\n     6\n  9\n$SURFV\n 70\n     6\n  9\n"
"$UCSNAME\n  2\n\n  9\n$UCSORG\n 10\n0.0\n 20\n0.0\n 30\n0.0\n"
"  9\n$UCSXDIR\n 10\n1.0\n 20\n0.0\n 30\n0.0\n  9\n$UCSYDIR\n 10\n"
"0.0\n 20\n1.0\n 30\n0.0\n  9\n$PUCSNAME\n  2\n\n  9\n$PUCSORG\n"
" 10\n0.0\n 20\n0.0\n 30\n0.0\n  9\n$PUCSXDIR\n 10\n1.0\n 20\n"
"0.0\n 30\n0.0\n  9\n$PUCSYDIR\n 10\n0.0\n 20\n1.0\n 30\n0.0\n"
"  9\n$USERI1\n 70\n     0\n  9\n$USERI2\n 70\n     0\n  9\n$USERI3\n"
" 70\n     0\n  9\n$USERI4\n 70\n     0\n  9\n$USERI5\n 70\n     0\n"
"  9\n$USERR1\n 40\n0.0\n  9\n$USERR2\n 40\n0.0\n  9\n$USERR3\n"
" 40\n0.0\n  9\n$USERR4\n 40\n0.0\n  9\n$USERR5\n 40\n0.0\n  9\n"
"$WORLDVIEW\n 70\n     1\n  9\n$SHADEDGE\n 70\n     3\n  9\n$SHADEDIF\n"
" 70\n    70\n  9\n$TILEMODE\n 70\n     1\n  9\n$MAXACTVP\n 70\n"
"    48\n  9\n$PINSBASE\n 10\n0.0\n 20\n0.0\n 30\n0.0\n  9\n$PLIMCHECK\n"
" 70\n     0\n  9\n$PEXTMIN\n 10\n1.000000E+20\n 20\n1.000000E+20\n"
" 30\n1.000000E+20\n  9\n$PEXTMAX\n 10\n-1.000000E+20\n 20\n-1.000000E+20\n"
" 30\n-1.000000E+20\n  9\n$PLIMMIN\n 10\n0.0\n 20\n0.0\n  9\n$PLIMMAX\n"
" 10\n420.0\n 20\n297.0\n  9\n$UNITMODE\n 70\n     0\n  9\n$VISRETAIN\n"
" 70\n     1\n  9\n$PLINEGEN\n 70\n     0\n  9\n$PSLTSCALE\n 70\n"
"     1\n  9\n$TREEDEPTH\n 70\n  3020\n  9\n$PICKSTYLE\n 70\n     1\n"
"  9\n$CMLSTYLE\n  2\nSTANDARD\n  9\n$CMLJUST\n 70\n     0\n  9\n"
"$CMLSCALE\n 40\n20.0\n  9\n$PROXYGRAPHICS\n 70\n     1\n  9\n"
"$MEASUREMENT\n 70\n     1\n  0\nENDSEC\n  0\nSECTION\n  2\nCLASSES\n"
"  0\nCLASS\n  1\nLWPOLYLINE\n  2\nAcDbPolyline\n  3\n\"AutoCAD\"\n"
" 90\n        0\n280\n     0\n281\n     1\n  0\nCLASS\n  1\nDICTIONARYVAR\n"
"  2\nAcDbDictionaryVar\n  3\n\"AutoCAD\"\n 90\n        0\n280\n"
"     0\n281\n     0\n  0\nCLASS\n  1\nHATCH\n  2\nAcDbHatch\n"
"  3\n\"ACAD_SEDONA\"\n 90\n        0\n280\n     0\n281\n     1\n"
"  0\nENDSEC\n  0\nSECTION\n  2\nTABLES\n  0\nTABLE\n  2\nVPORT\n"
"  5\n8\n100\nAcDbSymbolTable\n 70\n     5\n  0\nVPORT\n  5\n39\n"
"100\nAcDbSymbolTableRecord\n100\nAcDbViewportTableRecord\n  2\n"
"*ACTIVE\n 70\n     0\n 10\n0.0\n 20\n0.0\n 11\n1.0\n 21\n1.0\n"
" 12\n298.941176\n 22\n148.5\n 13\n0.0\n 23\n0.0\n 14\n10.0\n 24\n"
"10.0\n 15\n10.0\n 25\n10.0\n 16\n0.0\n 26\n0.0\n 36\n1.0\n 17\n"
"0.0\n 27\n0.0\n 37\n0.0\n 40\n297.0\n 41\n2.013072\n 42\n50.0\n"
" 43\n0.0\n 44\n0.0\n 50\n0.0\n 51\n0.0\n 71\n     0\n 72\n   100\n"
" 73\n     1\n 74\n     1\n 75\n     0\n 76\n     0\n 77\n     0\n"
" 78\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nLTYPE\n  5\n5\n100\n"
"AcDbSymbolTable\n 70\n     1\n  0\nLTYPE\n  5\n12\n100\nAcDbSymbolTableRecord\n"
"100\nAcDbLinetypeTableRecord\n  2\nBYBLOCK\n 70\n     0\n  3\n"
"\n 72\n    65\n 73\n     0\n 40\n0.0\n  0\nLTYPE\n  5\n13\n100\n"
"AcDbSymbolTableRecord\n100\nAcDbLinetypeTableRecord\n  2\nBYLAYER\n"
" 70\n     0\n  3\n\n 72\n    65\n 73\n     0\n 40\n0.0\n  0\n"
"LTYPE\n  5\n14\n100\nAcDbSymbolTableRecord\n100\nAcDbLinetypeTableRecord\n"
"  2\nCONTINUOUS\n 70\n     0\n  3\nSolid line\n 72\n    65\n 73\n"
"     0\n 40\n0.0\n  0\nENDTAB\n  0\nTABLE\n  2\nLAYER\n  5\n2\n"
"100\nAcDbSymbolTable\n 70\n     1\n  0\nLAYER\n  5\nE\n100\nAcDbSymbolTableRecord\n"
"100\nAcDbLayerTableRecord\n  2\n0\n 70\n     0\n 62\n     7\n"
"  6\nCONTINUOUS\n  0\nENDTAB\n  0\nTABLE\n  2\nSTYLE\n  5\n3\n"
"100\nAcDbSymbolTable\n 70\n     1\n  0\nSTYLE\n  5\nF\n100\nAcDbSymbolTableRecord\n"
"100\nAcDbTextStyleTableRecord\n  2\nSTANDARD\n 70\n     0\n 40\n"
"0.0\n 41\n1.0\n 50\n0.0\n 71\n     0\n 42\n2.5\n  3\ntxt\n  4\n"
"\n  0\nENDTAB\n  0\nTABLE\n  2\nVIEW\n  5\n6\n100\nAcDbSymbolTable\n"
" 70\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nUCS\n  5\n7\n100\n"
"AcDbSymbolTable\n 70\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nAPPID\n"
"  5\n9\n100\nAcDbSymbolTable\n 70\n     1\n  0\nAPPID\n  5\n10\n"
"100\nAcDbSymbolTableRecord\n100\nAcDbRegAppTableRecord\n  2\n"
"ACAD\n 70\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nDIMSTYLE\n  5\n"
"A\n100\nAcDbSymbolTable\n 70\n     1\n  0\nDIMSTYLE\n105\n1D\n"
"100\nAcDbSymbolTableRecord\n100\nAcDbDimStyleTableRecord\n  2\n"
"STANDARD\n 70\n     0\n  3\n\n  4\n\n  5\n\n  6\n\n  7\n\n 40\n"
"1.0\n 41\n0.18\n 42\n0.0625\n 43\n0.38\n 44\n0.18\n 45\n0.0\n"
" 46\n0.0\n 47\n0.0\n 48\n0.0\n140\n0.18\n141\n0.09\n142\n0.0\n"
"143\n25.4\n144\n1.0\n145\n0.0\n146\n1.0\n147\n0.09\n 71\n     0\n"
" 72\n     0\n 73\n     1\n 74\n     1\n 75\n     0\n 76\n     0\n"
" 77\n     0\n 78\n     0\n170\n     0\n171\n     2\n172\n     0\n"
"173\n     0\n174\n     0\n175\n     0\n176\n     0\n177\n     0\n"
"178\n     0\n270\n     2\n271\n     4\n272\n     4\n273\n     2\n"
"274\n     2\n340\nF\n275\n     0\n280\n     0\n281\n     0\n282\n"
"     0\n283\n     1\n284\n     0\n285\n     0\n286\n     0\n287\n"
"     3\n288\n     0\n  0\nENDTAB\n  0\nTABLE\n  2\nBLOCK_RECORD\n"
"  5\n1\n100\nAcDbSymbolTable\n 70\n     0\n  0\nBLOCK_RECORD\n"
"  5\n18\n100\nAcDbSymbolTableRecord\n100\nAcDbBlockTableRecord\n"
"  2\n*MODEL_SPACE\n  0\nBLOCK_RECORD\n  5\n15\n100\nAcDbSymbolTableRecord\n"
"100\nAcDbBlockTableRecord\n  2\n*PAPER_SPACE\n  0\nENDTAB\n  0\n"
"ENDSEC\n  0\nSECTION\n  2\nBLOCKS\n  0\nBLOCK\n  5\n19\n100\n"
"AcDbEntity\n  8\n0\n100\nAcDbBlockBegin\n  2\n*MODEL_SPACE\n 70\n"
"     0\n 10\n0.0\n 20\n0.0\n 30\n0.0\n  3\n*MODEL_SPACE\n  1\n"
"\n  0\nENDBLK\n  5\n1A\n100\nAcDbEntity\n  8\n0\n100\nAcDbBlockEnd\n"
"  0\nBLOCK\n  5\n16\n100\nAcDbEntity\n 67\n     1\n  8\n0\n100\n"
"AcDbBlockBegin\n  2\n*PAPER_SPACE\n 70\n     0\n 10\n0.0\n 20\n"
"0.0\n 30\n0.0\n  3\n*PAPER_SPACE\n  1\n\n  0\nENDBLK\n  5\n17\n"
"100\nAcDbEntity\n 67\n     1\n  8\n0\n100\nAcDbBlockEnd\n  0\n"
"ENDSEC\n  0\nSECTION\n  2\nENTITIES\n";

static const char * DXFpost = 
"  0\nENDSEC\n  0\nSECTION\n"
"  2\nOBJECTS\n  0\nDICTIONARY\n  5\nC\n100\nAcDbDictionary\n  3\n"
"ACAD_GROUP\n350\nD\n  3\nACAD_MLINESTYLE\n350\n1B\n  3\nACDBVARIABLEDICTIONARY\n"
"350\n32\n  0\nDICTIONARY\n  5\nD\n102\n{ACAD_REACTORS\n330\nC\n"
"102\n}\n100\nAcDbDictionary\n  0\nDICTIONARY\n  5\n1B\n102\n{ACAD_REACTORS\n"
"330\nC\n102\n}\n100\nAcDbDictionary\n  3\nSTANDARD\n350\n1C\n"
"  0\nDICTIONARY\n  5\n32\n102\n{ACAD_REACTORS\n330\nC\n102\n}\n"
"100\nAcDbDictionary\n  3\nSORTENTS\n350\n33\n  0\nMLINESTYLE\n"
"  5\n1C\n102\n{ACAD_REACTORS\n330\n1B\n102\n}\n100\nAcDbMlineStyle\n"
"  2\nSTANDARD\n 70\n     0\n  3\n\n 62\n   256\n 51\n90.0\n 52\n"
"90.0\n 71\n     2\n 49\n0.5\n 62\n   256\n  6\nBYLAYER\n 49\n"
"-0.5\n 62\n   256\n  6\nBYLAYER\n  0\nDICTIONARYVAR\n  5\n33\n"
"102\n{ACAD_REACTORS\n330\n32\n102\n}\n100\nDictionaryVariables\n"
"280\n     0\n  1\n96\n  0\nENDSEC\n  0\nEOF\n\n";




thexpuni::thexpuni()
{
  this->resol = 0.304;
  this->clear();
}

void thexpuni::clear()
{
  this->m_cpart = NULL;
  this->m_part_list.clear();
}


void thexpuni::polygon_start_ring(bool outer)
{
  this->m_ring_list.clear();
  thexpuni_part p;
  p.m_outer = outer;
  this->m_cpart = &(*this->m_part_list.insert(this->m_part_list.end(), p));
}



// insert points from bezier curve
void insert_line_segment(double resolution, thline * ln, bool reverse, std::list<thexpuni_data> & lst, long startp = 0, long endp = -1)
{
  thdb2dlp * cpt, * prevpt;
  thdb2dpt * cp1, * cp2;
	double t, tt, ttt, t_, tt_, ttt_, nx, ny, nz, na, px, py;
  long pnum;

  prevpt = NULL;
  if (reverse)
    cpt = ln->last_point;
  else
    cpt = ln->first_point;

  pnum = 0;
  while (cpt != NULL) {

    // insert bezier curve
    if (pnum == startp)
      prevpt = NULL;

    if ((pnum >= startp) && ((endp < 0) || (pnum <= endp))) {
      if (prevpt != NULL) {
        px = prevpt->point->xt;
        py = prevpt->point->yt;
        if (reverse) {
          cp1 = prevpt->cp2;
          cp2 = prevpt->cp1;
        } else {
          cp1 = cpt->cp1;
          cp2 = cpt->cp2;
        }
        if ((cp1 != NULL) && (cp2 == NULL)) cp2 = cp1;
        if ((cp1 == NULL) && (cp2 != NULL)) cp1 = cp2;
        if ((cp1 != NULL) && (cp2 != NULL)) {
				  for(t = 0.05; t < 1.0; t += 0.05) {
				    tt = t * t;
					  ttt = tt * t;
					  t_ = 1.0 - t;
					  tt_ = t_ * t_;
					  ttt_ = tt_ * t_;				
					  nx = ttt_ * prevpt->point->xt + 
							  3.0 * t * tt_ * cp1->xt + 
							  3.0 * tt * t_ * cp2->xt + 
							  ttt * cpt->point->xt;
					  ny = ttt_ * prevpt->point->yt + 
							  3.0 * t * tt_ * cp1->yt + 
							  3.0 * tt * t_ * cp2->yt + 
							  ttt * cpt->point->yt;
					  nz = t_ * cpt->point->zt + t * prevpt->point->zt;
					  na = t_ * cpt->point->at + t * prevpt->point->at;
            // resolution 0.1 m
					  if (hypot(nx - px, ny - py) > resolution) {
			        lst.push_back(thexpuni_data(nx + ln->fscrapptr->proj->rshift_x, ny + ln->fscrapptr->proj->rshift_y, nz + ln->fscrapptr->proj->rshift_z, na));
						  px = nx;
						  py = ny;
					  }
          }
        }
      }
    }

    // insert point it self
    if ((pnum >= startp) && ((endp < 0) || (pnum <= endp)))
      lst.push_back(thexpuni_data(cpt->point->xt + ln->fscrapptr->proj->rshift_x, cpt->point->yt + ln->fscrapptr->proj->rshift_y, cpt->point->zt + ln->fscrapptr->proj->rshift_z, cpt->point->at));

    // next point
    prevpt = cpt;
    if (reverse)
      cpt = cpt->prevlp;
    else
      cpt = cpt->nextlp;
    pnum++;

  }

}


void thexpuni::polygon_insert_line(thline * ln, bool reverse)
{
  insert_line_segment(this->resol, ln, reverse, this->m_ring_list);
}


void thexpuni::polygon_close_ring()
{

  // check polygon orientation
  double area;
  std::list<thexpuni_data>::iterator i, iprev;

  if (this->m_ring_list.size() < 2)
    return;

  iprev = this->m_ring_list.end();
  iprev--;
  area = 0.0;
  for (i = this->m_ring_list.begin(); i != this->m_ring_list.end(); i++) {
    area += iprev->m_x * i->m_y - iprev->m_y * i->m_x;
    iprev = i;
  }
  area *= 0.5;

  bool reverse = ((area > 0) && (this->m_cpart->m_outer)) || ((area < 0) && (!this->m_cpart->m_outer));

  // according to orientation, insert points to point list
  if (reverse) {
    for(i = this->m_ring_list.end(); i != this->m_ring_list.begin(); ) {
      i--;
      this->m_cpart->m_point_list.push_back(*i);
    } 
  } else {
    for(i = this->m_ring_list.begin(); i != this->m_ring_list.end(); i++) {
      this->m_cpart->m_point_list.push_back(*i);
    } 
  }
}



void thexpuni::parse_scrap(thscrap * scrap)
{
	
	thbuffer stnbuff;

  // export scrap outline
  this->clear();
  thscraplo * lo = scrap->get_outline(), * lo2;
  while (lo != NULL) {
    if (lo->line->outline != TT_LINE_OUTLINE_NONE) {
      lo2 = lo;
      this->polygon_start_ring(lo->line->outline == TT_LINE_OUTLINE_OUT);
      this->m_cpart->m_lo = lo;
      while (lo2 != NULL) {
        this->polygon_insert_line(lo2->line, lo2->mode == TT_OUTLINE_REVERSED);
        lo2 = lo2->next_line;
      }
      this->polygon_close_ring();
    }    
    lo = lo->next_outline;
  }

}


void thexpuni::parse_line(thline * line)
{
  long sp = 0, cp = 1;
  int csubtype;
  this->clear();
  thdb2dlp * lp = line->first_point, * slp;
  if (lp == NULL)
    return;
  csubtype = lp->subtype;
  slp = lp;
  lp = lp->nextlp;
  while (lp != NULL) {
    if ((lp->subtype != csubtype) || (lp->nextlp == NULL)) {
      thexpuni_part p;
      this->m_cpart = &(*this->m_part_list.insert(this->m_part_list.end(), p));
      this->m_cpart->m_lp = slp;
      insert_line_segment(this->resol, line, false, this->m_cpart->m_point_list, sp, cp);
      csubtype = lp->subtype; 
      sp = cp;
      slp = lp;
    }
    lp = lp->nextlp;
    cp++;
  }
}






void thexpmap::export_kml(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (prj->type != TT_2DPROJ_PLAN) {
    thwarning(("unable to export KML from non plan projection"));
    return;
  }


  if (thcfg.outcs == TTCS_LOCAL) {
    thwarning(("data not georeferenced -- unable to export KML file"));
    return;
  }

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  FILE * out;
  const char * fnm = this->get_output("cave.kml");
  out = fopen(fnm, "w");
  if (out == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;


  thscrap * scrap;
  thexpuni xu;

  fprintf(out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.0\">\n");
  fprintf(out,"<Folder>\n");
  fprintf(out,"<Icon> <href>https://www.dropbox.com/s/qfou9ptatywklu1/Cave_symbol1.png?dl=1</href> </Icon>\n");
  fprintf(out,"<Style id=\"ThMapStyle\"> <PolyStyle> <fill>1</fill> <outline>0</outline> </PolyStyle> </Style>\n");
  // VG 250616: TODO change icon above, maybe upload to therion website after testing

  thsurvey * mainsrv = db->fsurveyptr;
  thdataobject * obj;
  for(obj = mainsrv->foptr; obj != NULL; obj = obj->nsptr) 
    if (obj->get_class_id() == TT_SURVEY_CMD) {
      mainsrv = (thsurvey *) obj;
      break;
    }

  // Export cave name and description in the selected language
  std::string cavename = ths2txt((strlen(mainsrv->title) > 0) ? mainsrv->title : mainsrv->name, layout->lang);
  cavename = replace_all(cavename, "<br>", "-");
  fprintf(out,"<name><![CDATA[%s]]></name>\n", cavename.c_str());
  double cavedepth = 0;
  if (mainsrv->stat.station_top != NULL) {
    cavedepth = mainsrv->stat.station_top->z - mainsrv->stat.station_bottom->z;
    if (cavedepth > mainsrv->stat.length)
      cavedepth = mainsrv->stat.length;
  }
  layout->units.lang = layout->lang;
  fprintf(out,"<description><![CDATA[%s %s %s<br>%s %s %s<br>%s]]></description>\n",
      thT("title cave length",layout->lang), layout->units.format_length(mainsrv->stat.length), layout->units.format_i18n_length_units(),
      thT("title cave depth",layout->lang), layout->units.format_length(cavedepth), layout->units.format_i18n_length_units(),
      ths2txt(this->layout->doc_comment, this->layout->lang).c_str());

  int cA, cR, cG, cB;
  #define checkc(c) if (c < 0) c = 0; if (c > 255) c = 255;

  while (cmap != NULL) {
    cbm = cmap->first_bm;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      if (cbm->mode == TT_MAPITEM_NORMAL) {
        // Export each map as a placemark, with the map name and colour, if defined
        std::string mapname = ths2txt((strlen(cbm->bm->title) > 0) ? cbm->bm->title : cbm->bm->name, layout->lang);
        fprintf(out,"<Placemark>\n");
        fprintf(out,"<styleUrl>#ThMapStyle</styleUrl>\n");
        fprintf(out,"<name><![CDATA[%s]]></name>\n", mapname.c_str());
        if ((layout->color_crit == TT_LAYOUT_CCRIT_MAP) && (cmap->map->colour.defined)) {
          cR = int (255.0 * cmap->map->colour.R + 0.5);
          cG = int (255.0 * cmap->map->colour.G + 0.5);
          cB = int (255.0 * cmap->map->colour.B + 0.5);
        } else {
          cR = int (255.0 * this->layout->color_map_fg.R + 0.5);
          cG = int (255.0 * this->layout->color_map_fg.G + 0.5);
          cB = int (255.0 * this->layout->color_map_fg.B + 0.5);
        }
        if (this->layout->transparency) {
          cA = int (255.0 * this->layout->opacity + 0.5);
        } else {
          cA = 255;
        }
        checkc(cA);
        checkc(cR);
        checkc(cG);
        checkc(cB);

        fprintf(out,"<Style> <PolyStyle> <color>%02x%02x%02x%02x</color> </PolyStyle> </Style>\n", cA,cB,cG,cR);
        fprintf(out,"<MultiGeometry>\n");

        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            scrap = (thscrap*) cmi->object;
            xu.parse_scrap(scrap);
            if (xu.m_part_list.size() > 0) {
              fprintf(out,"<Polygon>\n");
              std::list<thexpuni_part>::iterator it;
              std::list<thexpuni_data>::iterator ip;
              double x,y,z;
              for(it = xu.m_part_list.begin(); it != xu.m_part_list.end(); it++) {
                if (it->m_outer)
                  fprintf(out,"<outerBoundaryIs>\n");
                else
                  fprintf(out,"<innerBoundaryIs>\n");

                fprintf(out,"<LinearRing>\n<coordinates>\n");
                for(ip = it->m_point_list.begin(); ip != it->m_point_list.end(); ip++) {
                  thcs2cs(thcs_get_data(thcfg.outcs)->params, thcs_get_data(TTCS_LONG_LAT)->params, 
                    ip->m_x, ip->m_y, scrap->z, x, y, z);
                  fprintf(out, "\t%.14f,%.14f,%.14f ", x / THPI * 180.0, y / THPI * 180.0, 0.0);
                }
                fprintf(out,"</coordinates>\n</LinearRing>\n");

                if (it->m_outer)
                  fprintf(out,"</outerBoundaryIs>\n");
                else
                  fprintf(out,"</innerBoundaryIs>\n");
              }
              fprintf(out,"</Polygon>\n");
            }
          }
          cmi = cmi->prev_item;  
        }
        fprintf(out,"</MultiGeometry>\n</Placemark>\n");
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  fprintf(out,"</Folder>\n");
  fprintf(out,"</kml>\n");
  fclose(out);

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}



void thexpmap::export_bbox(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (prj->type != TT_2DPROJ_PLAN) {
    thwarning(("unable to export bounding box of non plan projection"));
    return;
  }

  if (thcfg.outcs == TTCS_LOCAL) {
    thwarning(("data not georeferenced -- unable to export bounding box file"));
    return;
  }

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  FILE * out;
  const char * fnm = this->get_output("cave.bbox");
  out = fopen(fnm, "w");
  if (out == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;

  double cx, cy, cz;
  bool has_scrap = false;

  thscrap * scrap;
  thexpuni xu;
  lxVecLimits lim;

  while (cmap != NULL) {
    cbm = cmap->first_bm;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      if (cbm->mode == TT_MAPITEM_NORMAL) {
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            scrap = (thscrap*) cmi->object;
            if (!thisnan(scrap->lxmin)) {
	    
              thcs2cs(thcs_get_data(thcfg.outcs)->params, thcs_get_data(TTCS_LONG_LAT)->params, 
                scrap->lxmin + prj->rshift_x, scrap->lymin + prj->rshift_y, scrap->z + prj->rshift_z, cx, cy, cz);
              lim.Add(cx / THPI * 180.0, cy / THPI * 180.0, cz);

              thcs2cs(thcs_get_data(thcfg.outcs)->params, thcs_get_data(TTCS_LONG_LAT)->params,  
                scrap->lxmin + prj->rshift_x, scrap->lymax + prj->rshift_y, scrap->z + prj->rshift_z, cx, cy, cz);
              lim.Add(cx / THPI * 180.0, cy / THPI * 180.0, cz);

              thcs2cs(thcs_get_data(thcfg.outcs)->params, thcs_get_data(TTCS_LONG_LAT)->params, 
                scrap->lxmax + prj->rshift_x, scrap->lymin + prj->rshift_y, scrap->z + prj->rshift_z, cx, cy, cz);
              lim.Add(cx / THPI * 180.0, cy / THPI * 180.0, cz);

              thcs2cs(thcs_get_data(thcfg.outcs)->params, thcs_get_data(TTCS_LONG_LAT)->params, 
                scrap->lxmax + prj->rshift_x, scrap->lymax + prj->rshift_y, scrap->z + prj->rshift_z, cx, cy, cz);
              lim.Add(cx / THPI * 180.0, cy / THPI * 180.0, cz);
	      
	      has_scrap = true;
	      
            }
          }
          cmi = cmi->prev_item;  
        }
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  if (has_scrap) {
    fprintf(out,"%.14f\n",lim.min.x);
    fprintf(out,"%.14f\n",lim.min.y);
    fprintf(out,"%.14f\n",lim.max.x);
    fprintf(out,"%.14f\n",lim.max.y);
  }
  fclose(out);
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}




void thexpmap::export_dxf(class thdb2dxm * maps, class thdb2dprj * prj)
{

  if (maps == NULL) {
    thwarning(("%s [%d] -- no selected projection data -- %s",
      this->src.name, this->src.line, this->projstr))
    return;
  }

  FILE * out;
  const char * fnm = this->get_output("cave.dxf");
  out = fopen(fnm, "w");
  if (out == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif     

  thdb2dxm * cmap = maps;
  thdb2dxs * cbm;
  thdb2dmi * cmi;
  thdb3dlim limits;

  thscrap * scrap;
  thexpuni xu;
  xu.resol = 0.1;

  fprintf(out,DXFpre,limits.minx, limits.miny, limits.maxx, limits.maxy);
  while (cmap != NULL) {
    cbm = cmap->first_bm;
    while (cbm != NULL) {
      cmi = cbm->bm->last_item;
      if (cbm->mode == TT_MAPITEM_NORMAL) {
        while (cmi != NULL) {
          if (cmi->type == TT_MAPITEM_NORMAL) {
            scrap = (thscrap*) cmi->object;
            xu.parse_scrap(scrap);
            if (xu.m_part_list.size() > 0) {
              double x(0.0), y(0.0), z(0.0), px(0.0), py(0.0), pz(0.0);
              bool inside;
              std::list<thexpuni_part>::iterator it;
              std::list<thexpuni_data>::iterator ip;

              fprintf(out, " 0\nHATCH\n 100\nAcDbEntity\n 8\n0\n 100\nAcDbHatch\n 10\n0.0\n 20\n0.0\n 30\n0.0\n 210\n0.0\n 220\n0.0\n 230\n1.0\n 2\nSOLID\n 70\n1\n 71\n0\n");
              fprintf(out, " 91\n%ld\n", (long) xu.m_part_list.size());

              for(it = xu.m_part_list.begin(); it != xu.m_part_list.end(); it++) {

                fprintf(out, " 92\n0\n 93\n%ld\n", (long)it->m_point_list.size());
                inside = false;
                for(ip = it->m_point_list.begin(); ip != it->m_point_list.end(); ip++) {
                  x = ip->m_x;
                  y = ip->m_y;
                  z = scrap->z + scrap->proj->rshift_z;
                  limits.update(x,y,z);
                  if (inside) {
                    fprintf(out, " 72\n1\n");
                    fprintf(out, " 10\n%.3f\n 20\n%.3f\n", px, py);
                    fprintf(out, " 11\n%.3f\n 21\n%.3f\n", x, y);
                  }
                  inside = true;
                  px = x;
                  py = y;
                  pz = z;
                }
                if (inside) {
                  ip = it->m_point_list.begin();
                  x = ip->m_x;
                  y = ip->m_y;
                  z = scrap->z + scrap->proj->rshift_z;
                  fprintf(out, " 72\n1\n");
                  fprintf(out, " 10\n%.3f\n 20\n%.3f\n", px, py);
                  fprintf(out, " 11\n%.3f\n 21\n%.3f\n", x, y);
                }
                fprintf(out, " 97\n0\n");
              }
              fprintf(out, " 75\n0\n 76\n1\n 98\n1\n 10\n0.0\n 20\n0.0\n");
            }
          }
          cmi = cmi->prev_item;  
        }
      }
      cbm = cbm->next_item;
    }
    cmap = cmap->next_item;
  }

  fprintf(out,"%s", DXFpost);
  fclose(out);
    
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

}






