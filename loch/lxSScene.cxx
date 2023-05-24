// Standard libraries
#ifndef LXDEPCHECK
#include <wx/statline.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#endif  
//LXDEPCHECK - standard libraries

#include "lxSScene.h"
#include "lxSetup.h"
#include "lxGUI.h"
#include "lxGLC.h"
#include "lxData.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif


enum {
  lxSS_VIS_CENTERLINE = 4000,
  lxSS_VIS_WALLS,
  lxSS_VIS_SURFACE,
  lxSS_VIS_LABELS,
  lxSS_VIS_BBOX,
  lxSS_VIS_GRID,
  lxSS_VIS_INDICATORS,
  lxSS_SRF_TRANSPARENCY,
  lxSS_SRF_OPACITY,
  lxSS_SRF_TEXTURE,
  lxSS_SRF_LIGHTING,
  lxSS_WALLS_TRANSPARENCY,
  lxSS_WALLS_OPACITY,
  lxSS_COLORMD_ALTITUDE,
  lxSS_COLORMD_DEFAULT,
  lxSS_COLORAPP_CENTERLINE,
  lxSS_COLORAPP_WALLS,
  lxSS_CLNVIS_SURFACE,
  lxSS_CLNVIS_CAVE,
  lxSS_CLNVIS_SPLAY,
  lxSS_CLNVIS_DUPLICATE,
  lxSS_CLNVIS_STATION,
  lxSS_CLNVIS_ENTRANCE,
  lxSS_CLNVIS_FIX,
  lxSS_STLABEL_NAME,
  lxSS_STLABEL_SURVEY,
  lxSS_STLABEL_COMMENT,
  lxSS_STLABEL_ALTITUDE,
  lxSS_IND_PNAME,
};


BEGIN_EVENT_TABLE(lxModelSetupDlg, wxMiniFrame)
  EVT_RADIOBUTTON(lxSS_COLORMD_ALTITUDE, lxModelSetupDlg::OnCommand)
  EVT_RADIOBUTTON(lxSS_COLORMD_DEFAULT, lxModelSetupDlg::OnCommand)
  EVT_BUTTON(wxID_ANY, lxModelSetupDlg::OnCommand)
  EVT_BUTTON(wxID_CLOSE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_CENTERLINE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_WALLS, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_SURFACE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_LABELS, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_BBOX, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_GRID, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_VIS_INDICATORS, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_WALLS_TRANSPARENCY, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_SRF_TRANSPARENCY, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_SRF_TEXTURE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_SRF_LIGHTING, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_STATION, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_ENTRANCE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_FIX, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_SURFACE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_CAVE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_SPLAY, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_CLNVIS_DUPLICATE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_COLORAPP_CENTERLINE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_COLORAPP_WALLS, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_STLABEL_NAME, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_STLABEL_COMMENT, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_STLABEL_ALTITUDE, lxModelSetupDlg::OnCommand)
  EVT_CHECKBOX(lxSS_STLABEL_SURVEY, lxModelSetupDlg::OnCommand)
  EVT_MOVE(lxModelSetupDlg::OnMove)
  EVT_CLOSE(lxModelSetupDlg::OnClose)
  EVT_COMMAND_SCROLL(lxSS_WALLS_OPACITY, lxModelSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(lxSS_SRF_OPACITY, lxModelSetupDlg::OnSlider)
	EVT_LISTBOX(LXMSTP_CONTROLLB, lxModelSetupDlg::OnControlSelect)
  EVT_TEXT(lxSS_IND_PNAME, lxModelSetupDlg::OnCommand)
END_EVENT_TABLE()


void lxModelSetupDlg::OnCommand(wxCommandEvent& event)
{
  switch (event.GetId()) {
    case wxID_CLOSE:
      this->m_mainFrame->ToggleModelSetup();
      break;

    case lxSS_SRF_TEXTURE:
      this->m_mainFrame->ToggleSurfaceTexture();
      break;

    case lxSS_SRF_LIGHTING:
      this->m_mainFrame->ToggleSurfaceLighting();
      break;

    case lxSS_SRF_TRANSPARENCY:
      this->m_mainFrame->ToggleSurfaceTransparency();
      break;

    case lxSS_WALLS_TRANSPARENCY:
      this->m_mainFrame->ToggleWallsTransparency();
      break;

    case lxSS_VIS_CENTERLINE:
      this->m_mainFrame->ToggleVisibilityCenterline();
      break;

    case lxSS_VIS_WALLS:
      this->m_mainFrame->ToggleVisibilityWalls();
      break;

    case lxSS_VIS_SURFACE:
      this->m_mainFrame->ToggleVisibilitySurface();
      break;

    case lxSS_VIS_LABELS:
      this->m_mainFrame->ToggleVisibilityLabels();
      break;

    case lxSS_VIS_BBOX:
      this->m_mainFrame->ToggleVisibilityBBox();
      break;

    case lxSS_VIS_GRID:
      this->m_mainFrame->ToggleVisibilityGrid();
      break;

    case lxSS_VIS_INDICATORS:
      this->m_mainFrame->ToggleVisibilityIndicators();
      break;

    case lxSS_COLORMD_ALTITUDE:
      this->m_mainFrame->SetColorMode(lxSETUP_COLORMD_ALTITUDE);
      break;

    case lxSS_COLORMD_DEFAULT:
      this->m_mainFrame->SetColorMode(lxSETUP_COLORMD_DEFAULT);
      break;

    case lxSS_COLORAPP_CENTERLINE:
      this->m_mainFrame->ToggleColorsApplyCenterline();
      break;

    case lxSS_COLORAPP_WALLS:
      this->m_mainFrame->ToggleColorsApplyWalls();
      break;

    case lxSS_CLNVIS_SURFACE:
      this->m_mainFrame->ToggleVisibilityCenterlineSurface();
      break;

    case lxSS_CLNVIS_CAVE:
      this->m_mainFrame->ToggleVisibilityCenterlineCave();
      break;

    case lxSS_CLNVIS_SPLAY:
      this->m_mainFrame->ToggleVisibilityCenterlineSplay();
      break;

    case lxSS_CLNVIS_DUPLICATE:
      this->m_mainFrame->ToggleVisibilityCenterlineDuplicate();
      break;

    case lxSS_CLNVIS_FIX:
      this->m_mainFrame->ToggleVisibilityCenterlineFix();
      break;

    case lxSS_CLNVIS_ENTRANCE:
      this->m_mainFrame->ToggleVisibilityCenterlineEntrance();
      break;

    case lxSS_CLNVIS_STATION:
      this->m_mainFrame->ToggleVisibilityCenterlineStation();
      break;

    case lxSS_STLABEL_ALTITUDE:
      this->m_mainFrame->ToggleVisibilityStLabelAltitude();
      break;

    case lxSS_STLABEL_NAME:
      this->m_mainFrame->ToggleVisibilityStLabelName();
      break;

    case lxSS_STLABEL_SURVEY:
      this->m_mainFrame->ToggleVisibilityStLabelSurvey();
      break;

    case lxSS_STLABEL_COMMENT:
      this->m_mainFrame->ToggleVisibilityStLabelComment();
      break;

    case lxSS_IND_PNAME:
      this->m_mainFrame->data->title = lxFTextCtrl(lxSS_IND_PNAME)->GetValue();
      this->m_mainFrame->canvas->ForceRefresh();
      break;

  }
}


void lxModelSetupDlg::OnSlider(wxScrollEvent& event)
{
  bool update = false;

  switch (event.GetId()) {
    case lxSS_SRF_OPACITY:
      this->m_mainFrame->setup->m_srf_opacity = 1.0 - double(event.GetInt()) / 100.0;
      update = true;
      break;

    case lxSS_WALLS_OPACITY:
      this->m_mainFrame->setup->m_walls_opacity = 1.0 - double(event.GetInt()) / 100.0;
      update = true;
      break;
  }

  if (update)
    this->m_mainFrame->canvas->UpdateContents();
}



void lxModelSetupDlg::OnClose(wxCloseEvent& WXUNUSED(event))
{
  this->m_mainFrame->ToggleModelSetup();
}

void lxModelSetupDlg::OnMove(wxMoveEvent& WXUNUSED(event))
{
  this->m_toolBoxPosition.Save();
}

void lxModelSetupDlg::OnControlSelect(wxCommandEvent& event)
{
	this->m_controlSizer->Show(this->m_controlSizer_Visibility, event.GetSelection() == 0);
	this->m_controlSizer->Show(this->m_controlSizer_ColorMode, event.GetSelection() == 1);
	this->m_controlSizer->Show(this->m_controlSizer_Centerline, event.GetSelection() == 2);
	this->m_controlSizer->Show(this->m_controlSizer_Labels, event.GetSelection() == 3);
	this->m_controlSizer->Show(this->m_controlSizer_Walls, event.GetSelection() == 4);
	this->m_controlSizer->Show(this->m_controlSizer_Surface, event.GetSelection() == 5);
	//this->m_controlSizer->Show(this->m_controlSizer_BBox2Grid, event.GetSelection() == 6);
	this->m_controlSizer->Show(this->m_controlSizer_Indicators, event.GetSelection() == 6);
	//this->m_controlSizer->Show(this->m_controlSizer_ColorSetup, event.GetSelection() == 8);
	this->m_controlSizer->Layout();
}


lxModelSetupDlg::lxModelSetupDlg(wxWindow *parent)
                : wxMiniFrame(parent, wxID_ANY, _(" Scene"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
{

  this->m_toolBoxPosition.Init(this, parent, 2, 8, 8);

#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

  this->m_mainFrame = (lxFrame *) parent;
   
  wxBoxSizer * sizerTop = new wxBoxSizer(wxHORIZONTAL),
		* sizerFrame = new wxBoxSizer(wxVERTICAL);
	lxPanel = new wxPanel(this, wxID_ANY);

	wxListBox * ctrlLBox;

	ctrlLBox = new wxListBox(lxPanel, LXMSTP_CONTROLLB);
	ctrlLBox->AppendString(_("Visibility"));
	ctrlLBox->AppendString(_("Color mode"));
	ctrlLBox->AppendString(_("Centerline"));
	ctrlLBox->AppendString(_("Stations"));
	ctrlLBox->AppendString(_("Walls"));
	ctrlLBox->AppendString(_("Surface"));
	//ctrlLBox->AppendString(_("BBox & grid"));
	ctrlLBox->AppendString(_("Indicators"));
	//ctrlLBox->AppendString(_("Color setup"));
	ctrlLBox->Select(0);

	this->m_controlSizer = new wxBoxSizer(wxVERTICAL);
  wxSize ctrlMinSize;

  /////////////////////////////////////////////////////////
  // Visibility

	lxBoxSizer = this->m_controlSizer_Visibility = new wxBoxSizer(wxVERTICAL);

#define ADDCB(id, ss) \
	lxBoxSizer->Add( \
			new wxCheckBox(lxPanel, id, ss), \
	    0, wxALIGN_LEFT  | wxBOTTOM, lxBORDER);
#define ADDRB(id, ss) \
	lxBoxSizer->Add( \
			new wxRadioButton(lxPanel, id, ss), \
	    0, wxALIGN_LEFT  | wxBOTTOM, lxBORDER);
#define ADDST(id, ss) \
  lxBoxSizer->Add( \
			new wxStaticText(lxPanel, id, ss), \
	    0, wxALIGN_LEFT | wxBOTTOM, lxBORDER);

	ADDST(wxID_ANY, _("Scene items"))
	ADDCB(lxSS_VIS_CENTERLINE, _("Centerline"))
	ADDCB(lxSS_VIS_WALLS, _("Walls"))
	ADDCB(lxSS_VIS_SURFACE, _("Surface"))
//  ADDCB(lxSS_VIS_LABELS, _("Labels"))
	ADDCB(lxSS_VIS_BBOX, _("Bounding box"))
//	ADDCB(lxSS_VIS_GRID, _("Grid"))
	ADDCB(lxSS_VIS_INDICATORS, _("Indicators"))

	this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  ctrlMinSize = lxBoxSizer->GetMinSize();

#define updateCtrlMinSize \
  lxSize = lxBoxSizer->GetMinSize(); \
  if (lxSize.x > ctrlMinSize.x) ctrlMinSize.x = lxSize.x; \
  if (lxSize.y > ctrlMinSize.y) ctrlMinSize.y = lxSize.y;


  /////////////////////////////////////////////////////////
  // color mode

	lxBoxSizer = this->m_controlSizer_ColorMode = new wxBoxSizer(wxVERTICAL);
  
	ADDST(wxID_ANY, _("Color mode"))
	lxBoxSizer->Add(
			new wxRadioButton(lxPanel, lxSS_COLORMD_ALTITUDE, _("Altitude"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP),
	    0, wxALIGN_LEFT  | wxBOTTOM, lxBORDER);


	//ADDRB(lxSS_COLORMD_ALTITUDE, _("Altitude"))
	// ADDRB(wxID_ANY, _("Topo date"))
	ADDRB(lxSS_COLORMD_DEFAULT, _("Default"))

  lxBoxSizer->Add( \
			new wxStaticLine(lxPanel, wxID_ANY), \
	    0, wxBOTTOM | wxEXPAND, lxBORDER);

  ADDST(wxID_ANY, _("Applies to"));
  ADDCB(lxSS_COLORAPP_CENTERLINE, _("Centerline"))
	ADDCB(lxSS_COLORAPP_WALLS, _("Walls"))
	// ADDCB(wxID_ANY, _("Surface"))

  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Centerline

	lxBoxSizer = this->m_controlSizer_Centerline = new wxBoxSizer(wxVERTICAL);
  
	ADDST(wxID_ANY, _("Centerline visibility"))
	ADDCB(lxSS_CLNVIS_SURFACE, _("Surface"))
	ADDCB(lxSS_CLNVIS_CAVE, _("Underground"))
	ADDCB(lxSS_CLNVIS_SPLAY, _("Splay"))
	ADDCB(lxSS_CLNVIS_DUPLICATE, _("Duplicate"))

  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Walls

	lxBoxSizer = this->m_controlSizer_Walls = new wxBoxSizer(wxVERTICAL);

  ADDCB(lxSS_WALLS_TRANSPARENCY, _("Transparency"))
  lxBoxSizer->Add(
		new wxSlider(lxPanel, lxSS_WALLS_OPACITY, 50, 0, 100, wxDefaultPosition, wxDefaultSize),
    0, wxEXPAND | wxBOTTOM, lxBORDER);
  
  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Surface

	lxBoxSizer = this->m_controlSizer_Surface = new wxBoxSizer(wxVERTICAL);

  ADDCB(lxSS_SRF_TRANSPARENCY, _("Transparency"))
  lxBoxSizer->Add(
		new wxSlider(lxPanel, lxSS_SRF_OPACITY, 50, 0, 100, wxDefaultPosition, wxDefaultSize),
    0, wxEXPAND | wxBOTTOM, lxBORDER);
  ADDCB(lxSS_SRF_TEXTURE, _("Texture"))
  ADDCB(lxSS_SRF_LIGHTING, _("Lighting"))
  // ADDCB(wxID_ANY, _("Fit to underground"))
  
  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Labels

	lxBoxSizer = this->m_controlSizer_Labels = new wxBoxSizer(wxVERTICAL);
  

  ADDST(wxID_ANY, _("Marked stations"));
	ADDCB(lxSS_CLNVIS_ENTRANCE, _("Entrances"))
	ADDCB(lxSS_CLNVIS_FIX, _("Fixed stations"))
	ADDCB(lxSS_CLNVIS_STATION, _("All"))

  lxBoxSizer->Add(new wxStaticLine(lxPanel, wxID_ANY), 0, wxBOTTOM | wxEXPAND, lxBORDER);
  ADDST(wxID_ANY, _("Labels"))
  ADDCB(lxSS_STLABEL_COMMENT, _("Comment"))
  ADDCB(lxSS_STLABEL_NAME, _("Name"))
  ADDCB(lxSS_STLABEL_SURVEY, _("Survey"))
  ADDCB(lxSS_STLABEL_ALTITUDE, _("Altitude"))

//  lxBoxSizer->Add(new wxStaticLine(lxPanel, wxID_ANY), 0, wxBOTTOM | wxEXPAND, lxBORDER);
//
//  ADDST(wxID_ANY, _("Other labels"))
//  ADDCB(wxID_ANY, _("Survey names"))
//  ADDCB(wxID_ANY, _("Map labels"))
//  ADDCB(wxID_ANY, _("Map remarks"))
//  
  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // BBox & Grid
//
//	lxBoxSizer = this->m_controlSizer_BBox2Grid = new wxBoxSizer(wxVERTICAL);
//
//  ADDST(wxID_ANY, _("Bounding box size"))
//
//  wxBoxSizer * bs2;
//  bs2 = new wxBoxSizer(wxHORIZONTAL);
//
//  bs2->Add(
//    new wxStaticText(lxPanel, wxID_ANY, _T("Overlap (%)")),
//    0, wxALIGN_CENTER_VERTICAL | wxRIGHT, lxBORDER);
//  bs2->Add(
//    new wxSpinCtrl(lxPanel, 3332, _T("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 5));
//	lxSETWWIDTH(3332, 2);
//  lxBoxSizer->Add(bs2, 0, wxBOTTOM | wxALIGN_LEFT, lxBORDER);
//
//  ADDCB(wxID_ANY, _("Fit bbox to grid"))
//  lxBoxSizer->Add(new wxStaticLine(lxPanel, wxID_ANY), 0, wxBOTTOM | wxEXPAND, lxBORDER);
//  ADDST(wxID_ANY, _("Grid labels"))
//  ADDCB(wxID_ANY, _("Altitude"))
//  ADDCB(wxID_ANY, _("XY coordinates"))
//
//  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
//  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Indicators

	lxBoxSizer = this->m_controlSizer_Indicators = new wxBoxSizer(wxVERTICAL);

  ADDST(wxID_ANY, _("Project title"))
	lxBoxSizer->Add(
		new wxTextCtrl(lxPanel, lxSS_IND_PNAME),
    0, wxEXPAND | wxBOTTOM, lxBORDER);
  
  this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Color setup

//	lxBoxSizer = this->m_controlSizer_ColorSetup = new wxBoxSizer(wxVERTICAL);

//  lxFlexGridSizer = new wxFlexGridSizer(2);


  /////////////////////////////////////////////////////////
  // end of tools 

  this->m_controlSizer->Layout();
  wxCommandEvent cmdEvent;
  cmdEvent.SetInt(0);
  this->m_controlSizer->SetMinSize(ctrlMinSize);
  this->OnControlSelect(cmdEvent);

	sizerTop->Add(ctrlLBox, 
		0, wxALL | wxEXPAND, lxBORDER);
	lxBoxSizer = new wxBoxSizer(wxVERTICAL);
	lxBoxSizer->Add(
		this->m_controlSizer, 
		1, wxEXPAND, lxBORDER);
	lxBoxSizer->Add(
		new wxButton(lxPanel, wxID_CLOSE, _("Close")), 
		0, wxALIGN_RIGHT);
	sizerTop->Add(lxBoxSizer, 
		1, wxALL | wxEXPAND, lxBORDER);

  lxPanel->SetSizer(sizerTop);
  sizerTop->Fit(lxPanel);

	sizerFrame->Add(lxPanel, 1, wxEXPAND | wxALL);

	this->SetSizer(sizerFrame);
  sizerFrame->SetSizeHints(this);
	sizerFrame->Fit(this);

}



void lxModelSetupDlg::InitSetup()
{
  lxFTextCtrl(lxSS_IND_PNAME)->SetValue(this->m_mainFrame->data->title);
}  



void lxModelSetupDlg::LoadSetup()
{  

  lxSetup * stp = this->m_mainFrame->setup;

  lxFCheckBox(lxSS_VIS_CENTERLINE)->SetValue(stp->m_vis_centerline);
  lxFCheckBox(lxSS_VIS_WALLS)->SetValue(stp->m_vis_walls);
  lxFCheckBox(lxSS_VIS_SURFACE)->SetValue(stp->m_vis_surface);
//  lxFCheckBox(lxSS_VIS_LABELS)->SetValue(stp->m_vis_labels);
  lxFCheckBox(lxSS_VIS_BBOX)->SetValue(stp->m_vis_bbox);
//  lxFCheckBox(lxSS_VIS_GRID)->SetValue(stp->m_vis_grid);
  lxFCheckBox(lxSS_VIS_INDICATORS)->SetValue(stp->m_vis_indicators);

  lxFCheckBox(lxSS_CLNVIS_CAVE)->SetValue(stp->m_vis_centerline_cave);
  lxFCheckBox(lxSS_CLNVIS_SURFACE)->SetValue(stp->m_vis_centerline_surface);
  lxFCheckBox(lxSS_CLNVIS_SPLAY)->SetValue(stp->m_vis_centerline_splay);
  lxFCheckBox(lxSS_CLNVIS_DUPLICATE)->SetValue(stp->m_vis_centerline_duplicate);

  lxFCheckBox(lxSS_CLNVIS_FIX)->SetValue(stp->m_vis_centerline_fix);
  lxFCheckBox(lxSS_CLNVIS_ENTRANCE)->SetValue(stp->m_vis_centerline_entrance);
  lxFCheckBox(lxSS_CLNVIS_STATION)->SetValue(stp->m_vis_centerline_station);

  lxFRadioBtn(lxSS_COLORMD_ALTITUDE)->SetValue(stp->m_colormd == lxSETUP_COLORMD_ALTITUDE);
  lxFRadioBtn(lxSS_COLORMD_DEFAULT)->SetValue(stp->m_colormd == lxSETUP_COLORMD_DEFAULT);
  lxFCheckBox(lxSS_COLORAPP_CENTERLINE)->SetValue(stp->m_colormd_app_centerline);
  lxFCheckBox(lxSS_COLORAPP_WALLS)->SetValue(stp->m_colormd_app_walls);

  lxFCheckBox(lxSS_STLABEL_ALTITUDE)->SetValue(stp->m_stlabel_altitude);
  lxFCheckBox(lxSS_STLABEL_NAME)->SetValue(stp->m_stlabel_name);
  lxFCheckBox(lxSS_STLABEL_SURVEY)->SetValue(stp->m_stlabel_survey);
  lxFCheckBox(lxSS_STLABEL_COMMENT)->SetValue(stp->m_stlabel_comment);

  lxFCheckBox(lxSS_SRF_TRANSPARENCY)->SetValue(stp->m_srf_transparency);
  lxFCheckBox(lxSS_SRF_TEXTURE)->SetValue(stp->m_srf_texture);
  lxFCheckBox(lxSS_SRF_LIGHTING)->SetValue(stp->m_srf_lighting);
  lxFSlider(lxSS_SRF_OPACITY)->SetValue(100 - int(stp->m_srf_opacity * 100.0));

  lxFCheckBox(lxSS_WALLS_TRANSPARENCY)->SetValue(stp->m_walls_transparency);
  lxFSlider(lxSS_WALLS_OPACITY)->SetValue(100 - int(stp->m_walls_opacity * 100.0));

}



