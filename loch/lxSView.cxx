// Standard libraries
#ifndef LXDEPCHECK
#include <wx/choice.h>
#include <math.h>
#endif  
//LXDEPCHECK - standart libraries


#include "lxSView.h"
#include "lxGUI.h"
#include "lxGLC.h"
#include "lxSetup.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif




#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)


BEGIN_EVENT_TABLE(lxViewpointSetupDlg, wxMiniFrame)
  EVT_BUTTON(wxID_CLOSE, lxViewpointSetupDlg::OnCommand)
  EVT_CHECKBOX(LXVSTP_PERSP, lxViewpointSetupDlg::OnCommand)
  EVT_CHECKBOX(LXVSTP_ROTATION, lxViewpointSetupDlg::OnCommand)
  EVT_CHECKBOX(LXVSTP_ROTLOCK, lxViewpointSetupDlg::OnCommand)
  EVT_CHECKBOX(LXVSTP_STEREO, lxViewpointSetupDlg::OnCommand)
  EVT_CHECKBOX(LXVSTP_STEREOBW, lxViewpointSetupDlg::OnCommand)
  EVT_CHOICE(LXVSTP_3DGLASSES, lxViewpointSetupDlg::OnCommand)
  EVT_MOVE(lxViewpointSetupDlg::OnMove)
  EVT_CLOSE(lxViewpointSetupDlg::OnClose)
	EVT_LISTBOX(LXVSTP_CONTROLLB, lxViewpointSetupDlg::OnControlSelect)
  EVT_TEXT(LXVSTP_FACING, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_TILT, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_CENTERX, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_CENTERY, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_CENTERZ, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_DIST, lxViewpointSetupDlg::OnText)
  EVT_TEXT(LXVSTP_ZOOM, lxViewpointSetupDlg::OnText)
  EVT_COMMAND_SCROLL(LXVSTP_FACINGSLIDE, lxViewpointSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(LXVSTP_TILTSLIDE, lxViewpointSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(LXVSTP_ZOOMSLIDE, lxViewpointSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(LXVSTP_DISTSLIDE, lxViewpointSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(LXVSTP_EYESEP, lxViewpointSetupDlg::OnSlider)
  EVT_COMMAND_SCROLL(LXVSTP_ROTSPEED, lxViewpointSetupDlg::OnSlider)
END_EVENT_TABLE()


void lxViewpointSetupDlg::OnControlSelect(wxCommandEvent& event)
{
  this->m_controlSizer->Show(this->m_controlSizer_Orientation, event.GetSelection() == 0);
  this->m_controlSizer->Show(this->m_controlSizer_Center, event.GetSelection() == 1);
  this->m_controlSizer->Show(this->m_controlSizer_Camera, event.GetSelection() == 2);
  this->m_controlSizer->Show(this->m_controlSizer_Rotation, event.GetSelection() == 3);
  this->m_controlSizer->Show(this->m_controlSizer_Stereo, event.GetSelection() == 4);
	this->m_controlSizer->Layout();
}


void lxViewpointSetupDlg::OnText(wxCommandEvent& event)
{
  bool update = false;
  double tmpv;
  switch (event.GetId()) {
    case LXVSTP_FACING:
      if (this->m_uicFacing) {
        if ((lxFTextCtrl(LXVSTP_FACING)->GetValue().ToDouble(&tmpv)) 
            && (tmpv >= 0.0) && (tmpv < 360.0)) {
          this->m_mainFrame->setup->cam_dir = tmpv;
          update = true;
        }
      }
      this->m_uicFacing = true;
      break;
    case LXVSTP_TILT:
      if (this->m_uicTilt) {
        if ((lxFTextCtrl(LXVSTP_TILT)->GetValue().ToDouble(&tmpv))
            && (tmpv <= 90.0) && (tmpv >= -90.0)) {
          this->m_mainFrame->setup->cam_tilt = tmpv;
          update = true;
        }
      }
      this->m_uicTilt = true;
      break;
    case LXVSTP_DIST:
      if (this->m_uicDist) {
        if ((lxFTextCtrl(LXVSTP_DIST)->GetValue().ToDouble(&tmpv)) && (tmpv > 0.0)) {
          this->m_mainFrame->setup->cam_dist = tmpv;
          update = true;
        }
      }
      this->m_uicDist = true;
      break;
    case LXVSTP_ZOOM:
      if (this->m_uicZoom) {
        if ((lxFTextCtrl(LXVSTP_ZOOM)->GetValue().ToDouble(&tmpv)) && (tmpv >= 20.0) && (tmpv <= 2000.0)) {
          double oldvfovr = this->m_mainFrame->setup->cam_lens_vfovr;
          this->m_mainFrame->setup->SetLens(tmpv);
          this->m_mainFrame->setup->cam_dist *= oldvfovr / this->m_mainFrame->setup->cam_lens_vfovr;
          this->m_uicDist = false;
          lxFTextCtrl(LXVSTP_DIST)->SetValue(wxString::Format(_T("%.0f"), this->m_mainFrame->setup->cam_dist));
          update = true;
        }
      }
      this->m_uicZoom = true;
      break;
    case LXVSTP_CENTERX:
      if (this->m_uicCenterX) {
        if (lxFTextCtrl(LXVSTP_CENTERX)->GetValue().ToDouble(&tmpv)) {
          this->m_mainFrame->setup->cam_center.x = tmpv;
          update = true;
        }
      }
      this->m_uicCenterX = true;
      break;
    case LXVSTP_CENTERY:
      if (this->m_uicCenterY) {
        if (lxFTextCtrl(LXVSTP_CENTERY)->GetValue().ToDouble(&tmpv)) {
          this->m_mainFrame->setup->cam_center.y = tmpv;
          update = true;
        }
      }
      this->m_uicCenterY = true;
      break;
    case LXVSTP_CENTERZ:
      if (this->m_uicCenterZ) {
        if (lxFTextCtrl(LXVSTP_CENTERZ)->GetValue().ToDouble(&tmpv)) {
          this->m_mainFrame->setup->cam_center.z = tmpv;
          update = true;
        }
      }
      this->m_uicCenterZ = true;
      break;

  }
  if (update) {
    this->m_mainFrame->setup->UpdatePos();
    this->m_mainFrame->canvas->ForceRefresh(false);
    this->SetSliders();
  }
}


void lxViewpointSetupDlg::OnSlider(wxScrollEvent& event)
{
  wxCommandEvent tmpEvent(wxEVT_COMMAND_TEXT_UPDATED);
  switch (event.GetId()) {
    case LXVSTP_FACINGSLIDE:
#ifndef LXDEPCHECK
#if wxCHECK_VERSION(2,7,1)
      lxFTextCtrl(LXVSTP_FACING)->ChangeValue(wxString::Format(_T("%d"), event.GetInt()));
      tmpEvent.SetId(LXVSTP_FACING);
#else
      lxFTextCtrl(LXVSTP_FACING)->SetValue(wxString::Format(_T("%d"), event.GetInt()));
#endif      
      break;
    case LXVSTP_TILTSLIDE:
#if wxCHECK_VERSION(2,7,1)
      lxFTextCtrl(LXVSTP_TILT)->ChangeValue(wxString::Format(_T("%d"), event.GetInt()));
      tmpEvent.SetId(LXVSTP_TILT);
#else
      lxFTextCtrl(LXVSTP_TILT)->SetValue(wxString::Format(_T("%d"), event.GetInt()));
#endif      
      break;
    case LXVSTP_ZOOMSLIDE:
#if wxCHECK_VERSION(2,7,1)
      lxFTextCtrl(LXVSTP_ZOOM)->ChangeValue(wxString::Format(_T("%d"), int(20.0 * pow(100.0, (double(event.GetInt()) / 1000.0)))));
      tmpEvent.SetId(LXVSTP_ZOOM);
#else
      lxFTextCtrl(LXVSTP_ZOOM)->SetValue(wxString::Format(_T("%d"), int(20.0 * pow(100.0, (double(event.GetInt()) / 1000.0)))));
#endif      
      break;
    case LXVSTP_DISTSLIDE:
#if wxCHECK_VERSION(2,7,1)
      lxFTextCtrl(LXVSTP_DIST)->ChangeValue(wxString::Format(_T("%d"), int(pow(200.0 * this->m_mainFrame->setup->data_limits_diam, (double(event.GetInt()) / 1000.0)))));
      tmpEvent.SetId(LXVSTP_DIST);
#else
      lxFTextCtrl(LXVSTP_DIST)->SetValue(wxString::Format(_T("%d"), int(pow(200.0 * this->m_mainFrame->setup->data_limits_diam, (double(event.GetInt()) / 1000.0)))));
#endif      
#endif      
      break;
    case LXVSTP_ROTSPEED:
      this->m_mainFrame->canvas->m_sCameraAutoRotateAngle = (event.GetInt() >= 0 ? 1.0 : -1.0) * 
        0.05 * pow(200.0, fabs(double(event.GetInt())) / 1000.0);
      break;
    case LXVSTP_EYESEP:
      this->m_mainFrame->setup->cam_anaglyph_eyesep = (double(event.GetInt()) / 10000.0);
      if (this->m_mainFrame->setup->cam_anaglyph) 
        this->m_mainFrame->canvas->ForceRefresh(false);
      break;
  }
  this->OnText(tmpEvent);
}



void lxViewpointSetupDlg::OnCommand(wxCommandEvent& event)
{

  switch (event.GetId()) {
    case wxID_CLOSE:
      this->m_mainFrame->ToggleViewpointSetup();
      break;

    case LXVSTP_PERSP:
      this->m_mainFrame->TogglePerspective();
      break;

    case LXVSTP_STEREO:
      this->m_mainFrame->ToggleStereo();
      break;

    case LXVSTP_STEREOBW:
      this->m_mainFrame->ToggleStereoBW();
      break;

    case LXVSTP_ROTATION:
      this->m_mainFrame->ToggleRotation();
      break;

    case LXVSTP_ROTLOCK:
      this->m_mainFrame->ToggleRotLock();
      break;

    case LXVSTP_3DGLASSES:
      this->m_mainFrame->setup->cam_anaglyph_glasses = event.GetSelection();
      if (this->m_mainFrame->setup->cam_anaglyph)
        this->m_mainFrame->canvas->ForceRefresh();
      break;

  }
}


void lxViewpointSetupDlg::OnClose(wxCloseEvent& WXUNUSED(event))
{
  this->m_mainFrame->ToggleViewpointSetup();
}

void lxViewpointSetupDlg::OnMove(wxMoveEvent& WXUNUSED(event))
{
  if (!this->IsIconized()) {
    this->m_toolBoxPosition.Save();
  }
}


lxViewpointSetupDlg::lxViewpointSetupDlg(wxWindow *parent)
                : wxMiniFrame(parent, wxID_ANY, _(" Camera"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
{

  m_toolBoxPosition.Init(this, parent, 0, 0, 0);

#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

  this->m_mainFrame = (lxFrame *) parent;

  this->m_uicFacing = false;
  this->m_uicTilt = false;
  this->m_uicCenterX = false;
  this->m_uicCenterY = false;
  this->m_uicCenterZ = false;
  this->m_uicDist = false;
  this->m_uicZoom = false;

	/////////////////////////////////////////////////////////////


  wxBoxSizer * sizerTop = new wxBoxSizer(wxHORIZONTAL),
		* sizerFrame = new wxBoxSizer(wxVERTICAL);
	lxPanel = new wxPanel(this, wxID_ANY);

	wxListBox * ctrlLBox;

	ctrlLBox = new wxListBox(lxPanel, LXVSTP_CONTROLLB);
	ctrlLBox->AppendString(_("Orientation"));
	ctrlLBox->AppendString(_("Center"));
	ctrlLBox->AppendString(_("Focus"));
	ctrlLBox->AppendString(_("Rotation"));
	ctrlLBox->AppendString(_("Stereo mode"));

	this->m_controlSizer = new wxBoxSizer(wxVERTICAL);
  wxSize ctrlMinSize;

#define ADDCB(id, ss) \
	lxBoxSizer->Add( \
			new wxCheckBox(lxPanel, id, ss), \
	    0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT  | wxBOTTOM | wxEXPAND, lxBORDER);
#define ADDRB(id, ss) \
	lxBoxSizer->Add( \
			new wxRadioButton(lxPanel, id, ss), \
	    0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT  | wxBOTTOM | wxEXPAND, lxBORDER);
#define ADDST(id, ss) \
  lxBoxSizer->Add( \
			new wxStaticText(lxPanel, id, ss), \
	    0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, lxBORDER);


  /////////////////////////////////////////////////////////
  // Orientation

	lxBoxSizer = this->m_controlSizer_Orientation = new wxBoxSizer(wxVERTICAL);

	lxGBSizer = new wxGridBagSizer();
 	lxGBSizer->SetCols(2);

  lxGBSizer->AddGrowableCol(1, 1);

	lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Facing (deg)")),
		POS(0, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_FACING, _T("0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(0, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxGBSizer->Add(
		new wxSlider(lxPanel, LXVSTP_FACINGSLIDE, 0, 0, 359),
		POS(1, 0), SPAN(1, 2), wxBOTTOM | wxEXPAND, lxBORDER);
	lxSETWWIDTH(LXVSTP_FACING, 2);

	lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Tilt (deg)")),
		POS(2, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_TILT, _T("90"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(2, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxGBSizer->Add(
		new wxSlider(lxPanel, LXVSTP_TILTSLIDE, 0, -90, 90),
		POS(3, 0), SPAN(1, 2), wxBOTTOM | wxEXPAND, lxBORDER);
	lxSETWWIDTH(LXVSTP_TILT, 2);

  lxBoxSizer->Add(lxGBSizer, 0, wxEXPAND);
	this->m_controlSizer->Add(lxBoxSizer, 1, wxEXPAND);
  ctrlMinSize = lxBoxSizer->GetMinSize();

#define updateCtrlMinSize \
  lxSize = lxBoxSizer->GetMinSize(); \
  if (lxSize.x > ctrlMinSize.x) ctrlMinSize.x = lxSize.x; \
  if (lxSize.y > ctrlMinSize.y) ctrlMinSize.y = lxSize.y;


  /////////////////////////////////////////////////////////
  // Center

	lxBoxSizer = this->m_controlSizer_Center = new wxBoxSizer(wxVERTICAL);

  ADDST(wxID_ANY,_("Rotation center"));
  lxGBSizer = new wxGridBagSizer();

  lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Easting (m)")),
		POS(0, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_CENTERX, _T("100.0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(0, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxSETWWIDTH(LXVSTP_CENTERX, 3);

  lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Northing (m)")),
		POS(1, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_CENTERY, _T("100.0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(1, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxSETWWIDTH(LXVSTP_CENTERY, 3);

	lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Altitude (m)")),
		POS(2, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_CENTERZ, _T("100.0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(2, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxSETWWIDTH(LXVSTP_CENTERZ, 3);

  lxBoxSizer->Add(lxGBSizer, 1, wxEXPAND);
  this->m_controlSizer->Add(lxBoxSizer, 0, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Camera

	lxBoxSizer = this->m_controlSizer_Camera = new wxBoxSizer(wxVERTICAL);
  
	lxGBSizer = new wxGridBagSizer();
 	lxGBSizer->SetCols(2);

  lxGBSizer->AddGrowableCol(1, 1);

	lxGBSizer->Add(
	  new wxStaticText(lxPanel, wxID_ANY, _("Distance (m)")),
		POS(0, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
	  new wxTextCtrl(lxPanel, LXVSTP_DIST, _T("100"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(0, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxGBSizer->Add(
  	new wxSlider(lxPanel, LXVSTP_DISTSLIDE, 0, 0, 1000),
		POS(1, 0), SPAN(1, 2), wxBOTTOM | wxEXPAND, lxBORDER);
	lxSETWWIDTH(LXVSTP_DIST, 2.0);

  lxGBSizer->Add(
    new wxCheckBox(lxPanel, LXVSTP_PERSP, _("Orthogonal view")),
    POS(2,0), SPAN(1, 2), wxBOTTOM | wxALIGN_LEFT, lxBORDER);

	lxGBSizer->Add(
		new wxStaticText(lxPanel, wxID_ANY, _("Focus (mm)")),
		POS(3, 0), SPAN(1, 1), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT, lxBORDER);
	lxGBSizer->Add(
		new wxTextCtrl(lxPanel, LXVSTP_ZOOM, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT),
		POS(3, 1), SPAN(1, 1), wxALIGN_LEFT | wxBOTTOM, lxBORDER);
	lxGBSizer->Add(
		new wxSlider(lxPanel, LXVSTP_ZOOMSLIDE, 0, 0, 1000),
		POS(4, 0), SPAN(1, 2), wxBOTTOM | wxEXPAND, lxBORDER);
	lxSETWWIDTH(LXVSTP_ZOOM, 2.0);

  lxBoxSizer->Add(lxGBSizer, 0, wxEXPAND);

  this->m_controlSizer->Add(lxBoxSizer, 0, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Rotation

	lxBoxSizer = this->m_controlSizer_Rotation = new wxBoxSizer(wxVERTICAL);

  ADDCB(LXVSTP_ROTATION, _("Enable rotation"))
  ADDCB(LXVSTP_ROTLOCK, _("Lock rotation"))
  ADDST(wxID_ANY, _("Rotation speed"))

  wxSlider * rspeed = new wxSlider(lxPanel, LXVSTP_ROTSPEED, 0, -1000, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS);
#if wxCHECK_VERSION(3,0,0)
  rspeed->SetTickFreq(1000);
#else
  rspeed->SetTickFreq(1000,0);
#endif

  lxBoxSizer->Add(
		rspeed,
    0, wxBOTTOM | wxEXPAND, lxBORDER);

  lxBoxSizer->Add(
    new wxStaticText(lxPanel, LXVSTP_RENSPEED, _T("")),
    0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, lxBORDER);

  this->m_controlSizer->Add(lxBoxSizer, 0, wxEXPAND);
  updateCtrlMinSize;

  /////////////////////////////////////////////////////////
  // Stereo

	lxBoxSizer = this->m_controlSizer_Stereo = new wxBoxSizer(wxVERTICAL);

  ADDCB(LXVSTP_STEREO, _("Enable stereo mode"))
  ADDST(wxID_ANY, _("Eye separation"))

  lxBoxSizer->Add(
    new wxSlider(lxPanel, LXVSTP_EYESEP, 250, 1, 1000),
    0, wxEXPAND);

  wxChoice * ch = new wxChoice(lxPanel, LXVSTP_3DGLASSES);
  ch->Append(_("red & cyan"));
  ch->Append(_("red & green"));
  ch->Append(_("red & blue"));
  ch->Append(_("yellow & blue"));
  ch->Append(_("cyan & red"));
  ch->Append(_("green & red"));
  ch->Append(_("blue & red"));
  ch->Append(_("blue & yellow"));
  ch->SetSelection(0);

  ADDCB(LXVSTP_STEREOBW, _("Disable color textures"))

  ADDST(wxID_ANY, _("Color channels"))

  lxBoxSizer->Add(ch, 0, wxBOTTOM | wxEXPAND, lxBORDER);

  this->m_controlSizer->Add(lxBoxSizer, 0, wxEXPAND);
  updateCtrlMinSize;

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

  wxCommandEvent cmdEvt;
  cmdEvt.SetInt(3);
  this->OnControlSelect(cmdEvt);
	ctrlLBox->Select(3);

}


bool lxViewpointSetupDlg::IsChanged()
{
  return false;
}


void lxViewpointSetupDlg::LoadSetup()
{  

  lxSetup * stp = this->m_mainFrame->setup;

  this->m_uicFacing = false;
  lxFTextCtrl(LXVSTP_FACING)->SetValue(wxString::Format(_T("%.0f"), stp->cam_dir));

  this->m_uicTilt = false;
  lxFTextCtrl(LXVSTP_TILT)->SetValue(wxString::Format(_T("%.0f"), stp->cam_tilt));

  this->m_uicCenterX = false;
  lxFTextCtrl(LXVSTP_CENTERX)->SetValue(wxString::Format(_T("%.0f"), stp->cam_center.x));

  this->m_uicCenterY = false;
  lxFTextCtrl(LXVSTP_CENTERY)->SetValue(wxString::Format(_T("%.0f"), stp->cam_center.y));

  this->m_uicCenterZ = false;
  lxFTextCtrl(LXVSTP_CENTERZ)->SetValue(wxString::Format(_T("%.0f"), stp->cam_center.z));

  this->m_uicDist = false;
  lxFTextCtrl(LXVSTP_DIST)->SetValue(wxString::Format(_T("%.0f"), stp->cam_dist));

  this->m_uicZoom = false;
  lxFTextCtrl(LXVSTP_ZOOM)->SetValue(wxString::Format(_T("%.0f"), stp->cam_lens));

  lxFCheckBox(LXVSTP_PERSP)->SetValue(!stp->cam_persp);
  lxFCheckBox(LXVSTP_STEREO)->SetValue(stp->cam_anaglyph);
  lxFCheckBox(LXVSTP_STEREOBW)->SetValue(stp->cam_anaglyph_bw);
  lxFCheckBox(LXVSTP_ROTATION)->SetValue(this->m_mainFrame->canvas->m_sCameraAutoRotate);
  lxFCheckBox(LXVSTP_ROTLOCK)->SetValue(this->m_mainFrame->canvas->m_sCameraLockRotation);
  if (this->m_mainFrame->canvas->m_sCameraAutoRotate)
    lxFCheckBox(LXVSTP_ROTLOCK)->SetLabel(_("Lock rotation speed"));
  else
    lxFCheckBox(LXVSTP_ROTLOCK)->SetLabel(_("Lock rotation"));

  this->SetSliders();
  this->IsChanged();
}

void lxViewpointSetupDlg::SetSliders()
{
  lxSetup * stp = this->m_mainFrame->setup;
  lxFSlider(LXVSTP_FACINGSLIDE)->SetValue(int(stp->cam_dir));
  lxFSlider(LXVSTP_TILTSLIDE)->SetValue(int(stp->cam_tilt));
  lxFSlider(LXVSTP_ZOOMSLIDE)->SetValue(int(1000.0 * log(stp->cam_lens / 20.0)/log(100.0) + 0.5));
  lxFSlider(LXVSTP_DISTSLIDE)->SetValue(int(1000.0 * log(stp->cam_dist)/log(200.0 * stp->data_limits_diam) + 0.5));
  lxFSlider(LXVSTP_EYESEP)->SetValue(int(stp->cam_anaglyph_eyesep * 10000.0));
  lxFSlider(LXVSTP_ROTSPEED)->SetValue(int(fabs(this->m_mainFrame->canvas->m_sCameraAutoRotateAngle) < 0.05 ? 0.0 : (this->m_mainFrame->canvas->m_sCameraAutoRotateAngle > 0 ? 1 : -1) * 
    int(1000.0 * log(fabs(this->m_mainFrame->canvas->m_sCameraAutoRotateAngle) / 0.05)/log(200.0) + 0.5)));
}





