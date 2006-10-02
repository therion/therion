/**
 * @file lxData.cxx
 * Loch model data.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
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

// Standard libraries
#ifndef LXDEPCHECK
#include <stdlib.h>
#include <wx/wx.h>
#include <wx/txtstrm.h>
#include <wx/strconv.h>
#include <wx/wfstream.h>
#include <wx/msgdlg.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <vtkVoidArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <locale.h>
#include <GL/glu.h>
#include <map>
#endif  
//LXDEPCHECK - standart libraries

#include "lxData.h"
#include "lxLRUD.h"

lxData::lxData()
{
  this->m_centerline = NULL;

  this->scrapWalls = NULL;
  this->scrapWallsNormals = vtkPolyDataNormals::New();

  this->lrudWalls = NULL;

  this->allWalls = vtkAppendPolyData::New();
  this->allWallsTriangle = vtkTriangleFilter::New();
  this->allWallsSorted = vtkDepthSortPolyData::New();
  this->allWallsSorted->SetDirectionToSpecifiedVector();
  this->allWallsSorted->SetVector(0.0, 0.0, -1.0);
  this->allWallsSorted->SetDepthSortModeToBoundsCenter();
  this->allWallsStripped = vtkStripper::New();

  this->surface = NULL;
  this->surfaceNormals = vtkPolyDataNormals::New();
  this->surfaceTriangle = vtkTriangleFilter::New();
  this->surfaceSorted = vtkDepthSortPolyData::New();
  this->surfaceSorted->SetDirectionToSpecifiedVector();
  this->surfaceSorted->SetDepthSortModeToBoundsCenter();
  this->surfaceSorted->SetVector(0.0, 0.0, -1.0);
    
  this->luTable = vtkLookupTable::New();
  this->luTable->SetHueRange(0.93,0.0);
  this->luTable->SetValueRange(1.0,1.0);
  this->luTable->SetSaturationRange(1.0,1.0);
  this->luTable->Build();
  
  this->title = _T("No file open");

}


void lxData::Clear()
{
  this->shots.clear();
  this->stations.clear();
  if (this->scrapWalls != NULL) {
    this->scrapWalls->Delete();
    this->scrapWalls = NULL;
  }
  if (this->lrudWalls != NULL) {
    this->lrudWalls->Delete();
    this->lrudWalls = NULL;
  }
  if (this->surface != NULL) {
    this->surface->Delete();
    this->surface = NULL;
  }
  if (this->m_centerline != NULL) {
    this->m_centerline->Delete();
    this->m_centerline = NULL;
  }
  this->m_textureSurface.Clear();
}


void lxDataTexture::Clear()
{
  lxImageRGBFree(this->image);
  this->ClearTexImages();
}

lxDataTexture::~lxDataTexture()
{
  this->Clear();
}


lxData::~lxData()
{
  this->Clear();

  this->scrapWallsNormals->Delete();
  
  this->allWallsStripped->Delete();
  this->allWallsSorted->Delete();
  this->allWallsTriangle->Delete();
  this->allWalls->Delete();

  this->surfaceTriangle->Delete();
  this->surfaceSorted->Delete();
  this->surfaceNormals->Delete();

  this->luTable->Delete();
}



bool lxDataTexture::InitCalibInverse(double idx, double idy, double ixx, double ixy, double iyx, double iyy)
{
#define invmen (iyy*ixx - ixy*iyx)
  if (invmen == 0.0)
    return false;
  this->xx = iyy / invmen;
  this->xy = -ixy / invmen;
  this->yx = -iyx / invmen;
  this->yy = ixx / invmen;
  this->dx = -1.0 * (this->xx * idx + this->xy * idy);
  this->dy = -1.0 * (this->yx * idx + this->yy * idy);
  return true;
}

bool lxDataTexture::InitCalibNormal(double idx, double idy, double ixx, double ixy, double iyx, double iyy)
{
  this->dx = idx;
  this->dy = idy;
  this->xx = ixx;
  this->xy = ixy;
  this->yx = iyx;
  this->yy = iyy;
  return true;
}


void lxDataTexture::ClearTexImages()
{
#define lxDataTextureClear(i) \
  if (this->i != NULL) { \
    delete [] this->i; \
    this->i = NULL; \
  }
  lxDataTextureClear(texS);
  lxDataTextureClear(texSbw);
  lxDataTextureClear(texO);
  lxDataTextureClear(texObw);
}

bool lxDataTexture::SetImage(lxImageRGB img)
{
  this->Clear();
  this->image = img;
  if (this->image.data == NULL)
    return false;
  else
    return true;
}

void lxDataTexture::CreateTexImages(int sizeS, int sizeO)
{
  this->ClearTexImages();
  if (this->image.data == NULL)
    return;

  unsigned char * psrc, * pdst;
  long pidx, pidn;

  this->iw = double(this->image.width);
  this->ih = double(this->image.height);
  
  // resize the image
  int newSize = this->image.width;
  if (this->image.height > this->image.width)
    newSize = this->image.height;

  while (sizeS >= (2 * newSize)) sizeS /= 2;
  this->texS = new unsigned char [3 * sizeS * sizeS];
  this->texSbw = new unsigned char [3 * sizeS * sizeS];
  gluScaleImage(GL_RGB, 
  this->image.width, this->image.height, GL_UNSIGNED_BYTE, this->image.data,  
  sizeS, sizeS, GL_UNSIGNED_BYTE, this->texS);

  pidn = 3 * sizeS * sizeS;
  for(pidx = 0, psrc = this->texS, pdst = this->texSbw; pidx < pidn; pidx += 3, psrc += 3, pdst += 3) {
    pdst[2] = pdst[1] = pdst[0] = (unsigned char) (0.2989*double(psrc[0]) + 0.5870*double(psrc[1]) + 0.1140*double(psrc[2]));
  }

  this->texSizeS = sizeS;

  while (sizeO >= (2 * newSize)) sizeO /= 2;
  this->texO = new unsigned char [3 * sizeO * sizeO];
  this->texObw = new unsigned char [3 * sizeO * sizeO];
  gluScaleImage(GL_RGB, 
  this->image.width, this->image.height, GL_UNSIGNED_BYTE, this->image.data,  
  sizeO, sizeO, GL_UNSIGNED_BYTE, this->texO);

  pidn = 3 * sizeO * sizeO;
  for(pidx = 0, psrc = this->texO, pdst = this->texObw; pidx < pidn; pidx += 3, psrc += 3, pdst += 3) {
    pdst[2] = pdst[1] = pdst[0] = (unsigned char) (0.2989*double(psrc[0]) + 0.5870*double(psrc[1]) + 0.1140*double(psrc[2]));
  }

  this->texSizeO = sizeO;

}


struct lxDataRebuildStationStruct {
  lxFileStation * m_pst;
  size_t m_pos;
};


void lxData::Rebuild()
{

  wxString inpln, flags, filename;
  wxUint32 id1, id2, nid, lnNum, scrapWallsNpt, currentXpt = 0,
		surfaceNpt;
  wxInt32 xMode;
  double fpos[3], norm[3];

  lxLRUD lrud;
  
  lxDataStation st;
  lxDataShot sh;
  bool err;

  this->Clear();
  this->title = _T("");
  this->scrapWalls = vtkPolyData::New();  
  this->lrudWalls = vtkPolyData::New();
  this->surface = vtkPolyData::New();
  this->m_centerline = vtkPolyData::New();

  //vtkVoidArray * cldata_array = vtkVoidArray::New();
  vtkIntArray * cldata_array = vtkIntArray::New();
  cldata_array->SetName("Temp");
  err = false;
  lnNum = 0;
  xMode = -1;
	surfaceNpt = 0;
  lxVec tmpVec;
  
  vtkPoints * sWpoints = vtkPoints::New(),
    * lWpoints = vtkPoints::New(),
		* spoints = vtkPoints::New(),
    * cl_points = vtkPoints::New();
  vtkCellArray * sWpolys = vtkCellArray::New(),
    * lWpolys = vtkCellArray::New(),
    * sWstrips = vtkCellArray::New(),
		* spolys = vtkCellArray::New(),
    * cl_cells = vtkCellArray::New();
  vtkFloatArray * lWnorms = vtkFloatArray::New();
  lWnorms->SetName("Normals");
//    * tmpCA;

  std::map<size_t, lxDataRebuildStationStruct> stationMap;
  size_t st_num;
  lxDataRebuildStationStruct stR;
  lxFileStation_list::iterator st_it;
  lxFileShot_list::iterator sh_it;
  lxFileSurvey_list::iterator su_it = this->m_input.m_surveys.begin();
  if (su_it != this->m_input.m_surveys.end()) {
    this->title = wxConvUTF8.cMB2WX(this->m_input.m_surveysData.GetString(su_it->m_titlePtr));
    if (this->title.Length() == 0)
      this->title = wxConvUTF8.cMB2WX(this->m_input.m_surveysData.GetString(su_it->m_namePtr));
  }

  for(st_it = this->m_input.m_stations.begin(), st_num = 0; st_it != this->m_input.m_stations.end(); st_it++, st_num++) {
    st.pos.x = st_it->m_c[0];
    st.pos.y = st_it->m_c[1];
    st.pos.z = st_it->m_c[2];
    stR.m_pos = st_num;
    stR.m_pst = &(*st_it);
    stationMap[st_it->m_id] = stR;
    this->stations.push_back(st);
    cl_points->InsertNextPoint(st_it->m_c[0], st_it->m_c[1], st_it->m_c[2]);
  }

  lxFileDbl * tmpDbl;
  double fc[3], tc[3], fd[4], td[4];
  for(sh_it = this->m_input.m_shots.begin(); sh_it != this->m_input.m_shots.end(); sh_it++) {
    sh.from = stationMap[sh_it->m_from].m_pos;
    sh.to = stationMap[sh_it->m_to].m_pos;
    sh.surface = sh_it->GetFlag(LXFILE_SHOT_FLAG_SURFACE);
    sh.invisible = sh_it->GetFlag(LXFILE_SHOT_FLAG_NOT_VISIBLE);
    this->shots.push_back(sh);
    cldata_array->InsertNextValue(sh.from);
    //cldata_array->InsertNextVoidPointer(&(*sh_it));
    cl_cells->InsertNextCell(2);
    cl_cells->InsertCellPoint(sh.from);
    cl_cells->InsertCellPoint(sh.to);
    if (sh_it->m_sectionType != LXFILE_SHOT_SECTION_NONE) {    
      tmpDbl = stationMap[sh_it->m_from].m_pst->m_c;
      fc[0] = tmpDbl[0]; fc[1] = tmpDbl[1]; fc[2] = tmpDbl[2];
      tmpDbl = stationMap[sh_it->m_to].m_pst->m_c;
      tc[0] = tmpDbl[0]; tc[1] = tmpDbl[1]; tc[2] = tmpDbl[2];
      tmpDbl = sh_it->m_fLRUD;
      fd[0] = tmpDbl[0]; fd[1] = tmpDbl[1]; fd[2] = tmpDbl[2]; fd[3] = tmpDbl[3];
      tmpDbl = sh_it->m_tLRUD;
      td[0] = tmpDbl[0]; td[1] = tmpDbl[1]; td[2] = tmpDbl[2]; td[3] = tmpDbl[3];
      lrud.InsertShot(fc, tc, fd, td, NULL);
    }
  }
  this->m_centerline->SetPoints(cl_points);
  this->m_centerline->SetLines(cl_cells);
  vtkCellData * cdt;
  cdt = this->m_centerline->GetCellData();
  cdt->SetScalars(cldata_array);
  cldata_array->Delete();
  cl_points->Delete();
  cl_cells->Delete();


  // walls
  scrapWallsNpt = 0;
  lxFileScrap_list::iterator sw_it;
  lxFile3Point * sw_pts;
  lxFile3Angle * sw_tri;
  for(sw_it = this->m_input.m_scraps.begin(); sw_it != this->m_input.m_scraps.end(); sw_it++) {
    sw_pts = (lxFile3Point *) this->m_input.m_scrapsData.GetData(sw_it->m_pointsPtr);
    currentXpt = scrapWallsNpt;
    for(id1 = 0; id1 < sw_it->m_numPoints; id1++) {
      sWpoints->InsertPoint(scrapWallsNpt, sw_pts[id1].m_c[0],  sw_pts[id1].m_c[1],  sw_pts[id1].m_c[2]);
      scrapWallsNpt++;
    }
    sw_tri = (lxFile3Angle *) this->m_input.m_scrapsData.GetData(sw_it->m_3AnglesPtr);
    for(id1 = 0; id1 < sw_it->m_num3Angles; id1++) {
      sWpolys->InsertNextCell(3);
      sWpolys->InsertCellPoint(currentXpt + sw_tri[id1].m_v[0]);
      sWpolys->InsertCellPoint(currentXpt + sw_tri[id1].m_v[2]);
      sWpolys->InsertCellPoint(currentXpt + sw_tri[id1].m_v[1]);
    }
  }
  this->scrapWalls->SetPoints(sWpoints);
  this->scrapWalls->SetPolys(sWpolys);


  // single surface
  surfaceNpt = 0;
  lxFileSurface_list::iterator sf_it = this->m_input.m_surfaces.begin();
  lxFileDbl * sf_a;
  if (sf_it != this->m_input.m_surfaces.end()) {
    sf_a = (lxFileDbl *) this->m_input.m_surfacesData.GetData(sf_it->m_dataPtr);
    currentXpt = surfaceNpt;
    nid = 0;
    for(id1 = 0; id1 < sf_it->m_height; id1++) {
      for(id2 = 0; id2 < sf_it->m_width; id2++) {
        spoints->InsertPoint(surfaceNpt, 
          sf_it->m_calib[0] + sf_it->m_calib[2] * double(id2) + sf_it->m_calib[3] * double(id1),  
          sf_it->m_calib[1] + sf_it->m_calib[4] * double(id2) + sf_it->m_calib[5] * double(id1),  
          sf_a[nid]);
        surfaceNpt++;
        nid++;
      }
    }
    wxUint32 r1, r2;
    for(id1 = 1; id1 < sf_it->m_height; id1++) {
      for(id2 = 1; id2 < sf_it->m_width; id2++) {
        r2 = (id1 * sf_it->m_width) + id2;
        r1 = r2 - sf_it->m_width;
        spolys->InsertNextCell(4);
        spolys->InsertCellPoint(currentXpt + r1 - 1);
        spolys->InsertCellPoint(currentXpt + r1);
        spolys->InsertCellPoint(currentXpt + r2);
        spolys->InsertCellPoint(currentXpt + r2 - 1);
      }
    }
  }
	this->surface->SetPoints(spoints);
	this->surface->SetPolys(spolys);

  // texture for first surface object
  lxFileSurfaceBitmap_list::iterator sfb_it = this->m_input.m_surfaceBitmaps.begin();
  if ((sfb_it != this->m_input.m_surfaceBitmaps.end()) && (sfb_it->m_surfaceId == 0) && (sfb_it->m_type == LXFILE_BITMAP_JPEG)) {
     this->m_textureSurface.SetImage(lxRead_JPEG_file("", this->m_input.m_surfaceBitmapsData.GetTmpFile(sfb_it->m_dataPtr)));
     this->m_textureSurface.InitCalibInverse(sfb_it->m_calib[0], sfb_it->m_calib[1], sfb_it->m_calib[2], sfb_it->m_calib[3], sfb_it->m_calib[4], sfb_it->m_calib[5]);
  }
  

  // insert LRUD into walls
  lrud.Calculate();
  long i, ni;
  ni = lrud.GetPointsN();
  lWnorms->SetNumberOfComponents(3);
  for (i = 0; i < ni; i++) {
    lrud.GetPoint(i, fpos, norm);
    lWpoints->InsertPoint(i, fpos[0], fpos[1], fpos[2]);
    lWnorms->InsertTuple(i, norm);
  }
  
  ni = lrud.GetTrianglesN();
  long triv[3];
  for (i = 0; i < ni; i++) {
    lrud.GetTriangle(i, triv);
    lWpolys->InsertNextCell(3);
    lWpolys->InsertCellPoint(triv[0]);
    lWpolys->InsertCellPoint(triv[1]);
    lWpolys->InsertCellPoint(triv[2]);
  }
  this->lrudWalls->SetPoints(lWpoints);
  this->lrudWalls->SetPolys(lWpolys);
  this->lrudWalls->GetPointData()->SetNormals(lWnorms);

  // rescale image if too large
  double * bnds, x1, y1, x2, y2, x3, x4, y3, y4, tmp1, tmp2;
  int ix1, iy1, ix2, iy2, nsx, nsy;
  if ((this->surface != NULL) && (this->surface->GetNumberOfPoints() > 0) && (this->m_textureSurface.image.data != NULL)) {
    bnds = this->surface->GetBounds();
    x1 = this->m_textureSurface.xx * bnds[0] + this->m_textureSurface.xy * bnds[2] + this->m_textureSurface.dx;
    x2 = this->m_textureSurface.xx * bnds[1] + this->m_textureSurface.xy * bnds[2] + this->m_textureSurface.dx;
    x3 = this->m_textureSurface.xx * bnds[0] + this->m_textureSurface.xy * bnds[3] + this->m_textureSurface.dx;
    x4 = this->m_textureSurface.xx * bnds[1] + this->m_textureSurface.xy * bnds[3] + this->m_textureSurface.dx;
    y1 = this->m_textureSurface.yx * bnds[0] + this->m_textureSurface.yy * bnds[2] + this->m_textureSurface.dy;
    y2 = this->m_textureSurface.yx * bnds[1] + this->m_textureSurface.yy * bnds[2] + this->m_textureSurface.dy;
    y3 = this->m_textureSurface.yx * bnds[0] + this->m_textureSurface.yy * bnds[3] + this->m_textureSurface.dy;
    y4 = this->m_textureSurface.yx * bnds[1] + this->m_textureSurface.yy * bnds[3] + this->m_textureSurface.dy;
    tmp1 = x1; tmp2 = x1;
    if (x2 < tmp1) tmp1 = x2; if (x2 > tmp2) tmp2 = x2;
    if (x3 < tmp1) tmp1 = x3; if (x3 > tmp2) tmp2 = x3;
    if (x4 < tmp1) tmp1 = x4; if (x4 > tmp2) tmp2 = x4;
    x1 = tmp1; x2 = tmp2;
    tmp1 = y1; tmp2 = y1;
    if (y2 < tmp1) tmp1 = y2; if (y2 > tmp2) tmp2 = y2;
    if (y3 < tmp1) tmp1 = y3; if (y3 > tmp2) tmp2 = y3;
    if (y4 < tmp1) tmp1 = y4; if (y4 > tmp2) tmp2 = y4;
    y1 = tmp1; y2 = tmp2;
    ix1 = int(floor(x1)) - 1; if (ix1 < 0) ix1 = 0;
    iy1 = int(floor(y1)) - 1; if (iy1 < 0) iy1 = 0;
    ix2 = int(ceil(x2)) + 1; if (ix2 > this->m_textureSurface.image.width) ix2 = this->m_textureSurface.image.width;
    iy2 = int(ceil(y2)) + 1; if (iy2 > this->m_textureSurface.image.height) iy2 = this->m_textureSurface.image.height;
    nsx = ix2 - ix1;
    nsy = iy2 - iy1;
    if ((((ix2 - ix1) < this->m_textureSurface.image.width) || ((iy2 - iy1) < this->m_textureSurface.image.height)) && (nsx > 0) && (nsy > 0)) {
      // skopirujeme data
      unsigned char * nd = (unsigned char *)malloc(3 * nsx * nsy);
      int rr, rs, ors, dcs;
      rs = 3 * nsx;
      dcs = 3 * ix1;
      ors = 3 * this->m_textureSurface.image.width;
      for(rr = 0; rr < nsy; rr++) {
        memcpy(
          nd + ((nsy-rr-1) * rs),
          this->m_textureSurface.image.data + ((this->m_textureSurface.image.height - 1 - iy1 - rr) * ors + dcs),
          rs);
      }
      lxImageRGBFree(this->m_textureSurface.image);
      this->m_textureSurface.image.data = nd;
      this->m_textureSurface.image.width = nsx;
      this->m_textureSurface.image.height = nsy;
      this->m_textureSurface.iw = (double) nsx;
      this->m_textureSurface.ih = (double) nsy;
      this->m_textureSurface.dx -= (double) ix1;
      this->m_textureSurface.dy -= (double) iy1;
    }

  } else {
    this->m_textureSurface.Clear();
  }

  this->scrapWallsNormals->SetInput(this->scrapWalls);
  this->scrapWallsNormals->SetFeatureAngle(360.0);
  this->scrapWallsNormals->SetAutoOrientNormals(false);
  this->scrapWallsNormals->Update();

  // COUNTER needed
  this->allWalls->RemoveAllInputs();
  this->allWalls->AddInput(this->scrapWallsNormals->GetOutput());
  this->allWalls->AddInput(this->lrudWalls);
  this->allWalls->Update();
  this->allWallsTriangle->SetInput(this->allWalls->GetOutput());
  this->allWallsSorted->SetInput(this->allWallsTriangle->GetOutput());
  this->allWallsSorted->Update();
  this->allWallsStripped->SetInput(this->allWallsTriangle->GetOutput());
  this->allWallsStripped->Update();

	this->surfaceNormals->SetInput(this->surface);
	this->surfaceNormals->SetFeatureAngle(360);
	this->surfaceNormals->Update();
  this->surfaceTriangle->SetInput(this->surfaceNormals->GetOutput());
  this->surfaceSorted->SetInput(this->surfaceTriangle->GetOutput());
  this->surfaceSorted->Update();

  sWpoints->Delete();
  sWpolys->Delete();
  sWstrips->Delete();

  lWpoints->Delete();
  lWpolys->Delete();
  lWnorms->Delete();

	spoints->Delete();
	spolys->Delete();
  
}


