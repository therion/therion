// Standard libraries
#ifndef LXDEPCHECK
#include <wx/statline.h>
#include <wx/busyinfo.h>
#include <wx/button.h>
#include <vtkMassProperties.h>
#endif  
//LXDEPCHECK - standard libraries

#include "lxSStats.h"
#include "lxSetup.h"
#include "lxGUI.h"
#include "lxData.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif

enum {
  lxSS_SURVEY_STATS = 4000,
};


BEGIN_EVENT_TABLE(lxSurveyStatsDlg, wxMiniFrame)
  EVT_BUTTON(wxID_CLOSE, lxSurveyStatsDlg::OnCommand)
  EVT_MOVE(lxSurveyStatsDlg::OnMove)
  EVT_CLOSE(lxSurveyStatsDlg::OnClose)
END_EVENT_TABLE()


void lxSurveyStatsDlg::OnCommand(wxCommandEvent& event)
{
  switch (event.GetId()) {
    case wxID_CLOSE:
      this->m_mainFrame->ToggleSurveyStats();
      break;
  }
}



void lxSurveyStatsDlg::OnClose(wxCloseEvent& WXUNUSED(event))
{
    this->m_mainFrame->ToggleSurveyStats();
}

void lxSurveyStatsDlg::OnMove(wxMoveEvent& WXUNUSED(event))
{
  this->m_toolBoxPosition.Save();
}


lxSurveyStatsDlg::lxSurveyStatsDlg(wxWindow *parent)
                : wxMiniFrame(parent, wxID_ANY, _(" Survey statistics"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
{

  this->m_toolBoxPosition.Init(this, parent, 0, 8, 8);

#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

  this->m_mainFrame = (lxFrame *) parent;
   
  wxBoxSizer * sizerFrame = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * sizerTop = new wxBoxSizer(wxVERTICAL);

  lxPanel = new wxPanel(this, wxID_ANY);

  this->m_statsControl = new wxTextCtrl(lxPanel,wxID_ANY, wxEmptyString, wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

  wxBoxSizer * lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxButton(lxPanel, wxID_CLOSE, _("Close")), 0, wxALIGN_CENTER , lxBORDER);

  
	sizerTop->Add( this->m_statsControl,
		1, wxALL | wxEXPAND, lxBORDER);
	sizerTop->Add(
		lxBoxSizer,
		0, wxALIGN_RIGHT | lxNOTTOP, lxBORDER);
  
  lxPanel->SetSizer(sizerTop);
  sizerTop->Fit(lxPanel);

	sizerFrame->Add(lxPanel, 1, wxEXPAND | wxALL);

	this->SetSizer(sizerFrame);
  sizerFrame->SetSizeHints(this);
	sizerFrame->Fit(this);

  wxSize mfs = this->m_mainFrame->GetSize();
  this->SetSize(mfs.GetWidth() / 4, mfs.GetHeight() / 2);

}


void lxSurveyStatsDlg::LoadData()
{
  lxData * data = this->m_mainFrame->data;
  vtkMassProperties * smp = vtkMassProperties::New();
  smp->SetInputConnection(data->scrapWallsNormals->GetOutputPort());
  smp->Update();

  vtkMassProperties * emp = vtkMassProperties::New();
  emp->SetInputData(data->lrudWalls);
  emp->Update();

  double cf = 1.0;
  wxString cfu = _("cbm");
  if (this->m_mainFrame->m_iniUnits == LXUNITS_IMPERIAL) {
	  cf = 35.314667;
	  cfu = _("cft");
  }

  this->m_statsControl->Clear();
  this->m_statsControl->AppendText(wxString::Format(_("LRUD envelope volume: %.1f %s\n"), cf * emp->GetVolumeProjected(), cfu));
  this->m_statsControl->AppendText(wxString::Format(_("Maps 3d volume: %.1f %s\n"), cf * smp->GetVolumeProjected(), cfu));
}



