/**
 * @file lxSetup.cxx
 * Loch model setup.
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
#include <math.h>
#include <stdio.h>
#include <locale.h>
#endif  
//LXDEPCHECK - standard libraries

#include "lxSetup.h"
#include "lxData.h"

lxSetup::lxSetup(lxData * dat) 
{
  this->data = dat;
  this->cam_dir = 0;
  this->cam_tilt = 90;
  this->cam_persp = true;
  this->cam_anaglyph = false;
  this->cam_anaglyph_bw = true;
  this->cam_anaglyph_left = false;
  this->cam_anaglyph_eyesep = 0.02;
  this->cam_anaglyph_glasses = 0;

  this->m_vis_centerline = true;
  this->m_vis_walls = true;
  this->m_vis_surface = true;
  this->m_vis_labels = false;
  this->m_vis_bbox = true;
  this->m_vis_indicators = true;
  this->m_vis_grid = false;
  this->m_vis_centerline_cave = true;
  this->m_vis_centerline_surface = true;
  this->m_vis_centerline_splay = false;
  this->m_vis_centerline_duplicate = true;
  this->m_vis_centerline_entrance = false;
  this->m_vis_centerline_fix = false;
  this->m_vis_centerline_station = false;

  this->m_stlabel_altitude = false;
  this->m_stlabel_comment = false;
  this->m_stlabel_name = false;
  this->m_stlabel_survey = false;

  this->m_colormd = lxSETUP_COLORMD_ALTITUDE;
  this->m_colormd_app_centerline = true;
  this->m_colormd_app_walls = true;

  this->m_srf_transparency = true;
  this->m_srf_texture = true;
  this->m_srf_opacity = 0.5;
  this->m_srf_lighting = false;

  this->m_walls_transparency = false;
  this->m_walls_opacity = 0.5;

  this->UpdateData();
  this->SetLens(50.0);
}


void lxSetup::SetLens(double lens)
{
  this->cam_lens = lens;
  if (this->cam_lens < 20.0)
    this->cam_lens = 20.0;
  if (this->cam_lens > 2000.0)
    this->cam_lens = 2000.0;

  this->cam_lens_vfov = atan(12.0 / this->cam_lens) / lxPI * 360.0;
  this->cam_lens_vfovr = 12.0 / this->cam_lens;

}

void lxSetup::UpdateData()
{

  // udate data limits
  lxVecLimits tmp, tmpZ;
  tmp.valid = false;
  this->data_limits.valid = false;
  unsigned long id, nid;
  double * bnds;
  nid = this->data->shots.size();
  lxDataShot * shp;
  for(id = 0; id < nid; id++) {
    shp = &(this->data->shots[id]);
    if (!shp->m_selected) continue;    
    if (this->m_vis_centerline && 
      (((!shp->surface) && this->m_vis_centerline_cave) ||
      (shp->surface && this->m_vis_centerline_surface)))  {
      this->data_limits.Add(this->data->stations[shp->from].pos);
      this->data_limits.Add(this->data->stations[shp->to].pos);
      if (!shp->surface) {
        tmpZ.Add(this->data->stations[shp->from].pos);
        tmpZ.Add(this->data->stations[shp->to].pos);
      }
    }
  }

  nid = this->data->stations.size();
  for(id = 0; id < nid; id++) {
    if (!this->data->stations[id].m_selected) continue;    
    tmp.Add(this->data->stations[id].pos);
  }


  vtkPolyData * w;
  w = this->data->allWalls->GetOutput();
  if ((w != NULL) && (w->GetNumberOfPoints() > 0)) {
    bnds = w->GetBounds();
    if (this->m_vis_walls) {
      this->data_limits.Add(bnds[0], bnds[2], bnds[4]);
      this->data_limits.Add(bnds[1], bnds[3], bnds[5]);
      tmpZ.Add(bnds[0], bnds[2], bnds[4]);
      tmpZ.Add(bnds[1], bnds[3], bnds[5]);
    }
    tmp.Add(bnds[0], bnds[2], bnds[4]);
    tmp.Add(bnds[1], bnds[3], bnds[5]);
  }


  if (tmpZ.valid) {
	  this->data->luTable->SetTableRange(tmpZ.min.z, tmpZ.max.z);
  } else if (this->data_limits.valid) {
	  this->data->luTable->SetTableRange(this->data_limits.min.z, this->data_limits.max.z);
  }

  if ((this->data->surface != NULL) && (this->data->surface->GetNumberOfPoints() > 0)) {
    bnds = this->data->surface->GetBounds();
    if (this->m_vis_surface) {
      this->data_limits.Add(bnds[0], bnds[2], bnds[4]);
      this->data_limits.Add(bnds[1], bnds[3], bnds[5]);
    }
    tmp.Add(bnds[0], bnds[2], bnds[4]);
    tmp.Add(bnds[1], bnds[3], bnds[5]);
  }

  if (!this->data_limits.valid) {
    if (tmp.valid)
      this->data_limits = tmp;
    else {
      this->data_limits.Add(0.0, 0.0, 0.0);
      this->data_limits.Add(300.0, 200.0, 100.0);
    }
	  this->data->luTable->SetTableRange(this->data_limits.min.z, this->data_limits.max.z);
  }

  this->data_limits_diam = (this->data_limits.max - this->data_limits.min).Length();

}

void lxSetup::UpdatePos()
{
  this->cam_pos = this->cam_center + \
    lxPol2Vec(this->cam_dist, this->cam_dir + 180.0, this->cam_tilt);
}


void lxSetup::ResetCamera()
{
  this->cam_center = (this->data_limits.min + this->data_limits.max) / 2.0;
  lxVecLimits target = this->data_limits.Rotate(this->cam_dir, this->cam_tilt, this->cam_center);
  double model_width;
  model_width = target.max.x / lxVecAbs(target.max).z;
  if (this->cam_width > model_width) {
    this->cam_dist = target.max.z / (this->cam_lens_vfovr * 0.99);
  } else {
    this->cam_dist = target.max.x / (this->cam_lens_vfovr * this->cam_width * 0.99);
  }
  if (this->cam_persp) {
    this->cam_dist += target.max.y;
  }
  this->UpdatePos();
	
  //if (this->cam_persp) {
  //  
  //}
  //printf("MAX: %8.2f %8.2f %8.2f\n", lxVecXYZ(this->data_limits.max));
  //printf("MIN: %8.2f %8.2f %8.2f\n", lxVecXYZ(this->data_limits.min));
  //printf("CENTER: %8.2f %8.2f %8.2f\n", lxVecXYZ(this->cam_center));
  //printf("MLMAX: %8.2f %8.2f %8.2f\n", lxVecXYZ(target.max));
  //printf("MLMIN: %8.2f %8.2f %8.2f\n", lxVecXYZ(target.min));
  //printf("DIST: %8.2f\n", this->cam_dist);
  //printf("WIDTH: %8.2f\n", this->cam_width);
  
}


#define calc_cam_pos 

void lxSetup::StartCameraMovement()
{
  this->cam_orig_center = this->cam_center;
  this->cam_orig_pos = this->cam_pos;
  this->cam_orig_dist = this->cam_dist;
  this->cam_orig_dir = this->cam_dir;
  this->cam_orig_tilt = this->cam_tilt;
}


void lxSetup::ZoomCamera(double zoom)
{
  if (zoom > 0.0)
    this->cam_dist = this->cam_orig_dist / zoom;
  this->UpdatePos();
}


void lxSetup::PanCamera(double panx, double pany)
{
  this->cam_center = this->cam_orig_center + 
    lxPol2Vec(panx * this->cam_dist * 2.0 * this->cam_lens_vfovr * this->cam_width, this->cam_dir + 270.0, 0);
  this->cam_center = this->cam_center + 
    lxPol2Vec(pany * this->cam_dist * 2.0 * this->cam_lens_vfovr, 
    this->cam_dir + (this->cam_tilt > 0.0 ? 0 : 180.0), 
    (this->cam_tilt > 0 ? 90.0 - this->cam_tilt : 90 + this->cam_tilt));
  this->UpdatePos();
}


void lxSetup::TiltCamera(double tilt)
{
  this->cam_tilt = this->cam_orig_tilt + tilt;
  if (this->cam_tilt > 90.0)
    this->cam_tilt = 90.0;
  if (this->cam_tilt < -90.0)
    this->cam_tilt = -90.0;
  this->UpdatePos();
}


void lxSetup::RotateCamera(double rot)
{
  this->cam_dir = this->cam_orig_dir + rot;
  if (this->cam_dir < 0.0) {
    this->cam_dir += 360.0 * ceil(fabs(this->cam_dir) / 360.0);
  }
  if (this->cam_dir >= 360.0) {
    this->cam_dir -= 360.0 * floor(this->cam_dir / 360.0);
  }
  this->UpdatePos();
}



void lxSetup::RotateCameraF(double rot)
{
	this->cam_orig_dir += rot;
	this->RotateCamera(0.0);
}



void lxSetup::SelectSurvey(const char * survey)
{
  this->m_survey_selection.insert(std::string(survey));
}


bool lxSetup::IsSurveySelected(const char * survey)
{
  if (this->m_survey_selection.size() == 0) return true;
  std::set<std::string>::iterator it = this->m_survey_selection.find(std::string(survey));
  if (it != this->m_survey_selection.end()) return true;
  return false;
}


void lxSetup::ClearSurveySelection()
{
  this->m_survey_selection.clear();
}


void lxSetup::SaveToXMLNode(wxXmlNode * n)
{
  wxXmlNode * tmp, * tmpd;
  // remove all children
  tmp = n->GetChildren();
  while (tmp != NULL) {
    tmpd = tmp;
    tmp = tmp->GetNext();
    if (tmpd->GetName().StartsWith(_T("Camera"))) n->RemoveChild(tmpd);
    delete tmpd;
  }
  // save settings
  char * prevlocale = setlocale(LC_NUMERIC,NULL);
  setlocale(LC_NUMERIC,"C");
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraFacing"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_dir)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraTilt"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_tilt)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraCenterX"));
  tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_center.x)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraCenterY"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_center.y)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraCenterZ"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_center.z)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraDistance"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_dist)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraFocus"));
	tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(_T("%.4f"), this->cam_lens)));
	n->AddChild(tmp);
	tmp = new wxXmlNode(wxXML_ELEMENT_NODE, _T("CameraPerspective"));
  tmp->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, this->cam_persp ? _T("true") : _T("false")));
	n->AddChild(tmp);
  setlocale(LC_NUMERIC,prevlocale);
}

wxString getXmlValue(wxXmlNode * n, const wxString & name)
{
  wxString rv;
  if (n != NULL) {
    wxXmlNode * t = n->GetChildren(), * tt;
    while(t != NULL) {
      if (t->GetName() == name) {
        tt = t->GetChildren();
        if (tt->GetType() == wxXML_TEXT_NODE)
          rv = tt->GetContent();
      }
      t = t->GetNext();
    }
  }
  return rv;
}

void interpolateFloat(double * value, wxString v1, wxString v2, double t)
{
  if (v1.empty()) return;
  if (v2.empty()) *value = atof(v1.mbc_str());
  else *value = (1.0 - t) * atof(v1.mbc_str()) + t * atof(v2.mbc_str());
}

void interpolateBoolean(bool * value, wxString v1, wxString v2, double t)
{
  if (v1.empty()) return;
  if (v2.empty()) *value = (v1 == _T("true"));
  else {
    if (t <= 0.5) *value = (v1 == _T("true"));
    else *value = (v2 == _T("true"));
  }
}

void lxSetup::LoadFromXMLNode(wxXmlNode * n, wxXmlNode * nn, double t)
{
	if (n->GetName() != _T("Scene"))
		return;
  char * prevlocale = setlocale(LC_NUMERIC,NULL);
  double d;
  setlocale(LC_NUMERIC,"C");
  interpolateFloat(&(this->cam_dir), getXmlValue(n, _T("CameraFacing")), getXmlValue(nn, _T("CameraFacing")), t);
  interpolateFloat(&(this->cam_tilt), getXmlValue(n, _T("CameraTilt")), getXmlValue(nn, _T("CameraTilt")), t);
  interpolateFloat(&(this->cam_center.x), getXmlValue(n, _T("CameraCenterX")), getXmlValue(nn, _T("CameraCenterX")), t);
  interpolateFloat(&(this->cam_center.y), getXmlValue(n, _T("CameraCenterY")), getXmlValue(nn, _T("CameraCenterY")), t);
  interpolateFloat(&(this->cam_center.z), getXmlValue(n, _T("CameraCenterZ")), getXmlValue(nn, _T("CameraCenterZ")), t);
  interpolateFloat(&(this->cam_dist), getXmlValue(n, _T("CameraDistance")), getXmlValue(nn, _T("CameraDistance")), t);
  d = 20.0;
  interpolateFloat(&d, getXmlValue(n, _T("CameraFocus")), getXmlValue(nn, _T("CameraFocus")), t);
  this->SetLens(d);
  interpolateBoolean(&(this->cam_persp), getXmlValue(n, _T("CameraPerspective")), getXmlValue(nn, _T("CameraPerspective")), t);
  this->UpdatePos();
  setlocale(LC_NUMERIC,prevlocale);
}
