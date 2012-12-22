// Standard libraries
#ifndef LXDEPCHECK
#include <wx/statline.h>
#endif  
//LXDEPCHECK - standart libraries

#include "lxSTree.h"
#include "lxGUI.h"
#include "lxGLC.h"
#include "lxSetup.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif


enum {
  lxSS_SURVEY_TREE = 4000,
};


BEGIN_EVENT_TABLE(lxModelTreeDlg, wxMiniFrame)
  EVT_BUTTON(wxID_ANY, lxModelTreeDlg::OnCommand)
  EVT_BUTTON(wxID_CLOSE, lxModelTreeDlg::OnCommand)
  EVT_MOVE(lxModelTreeDlg::OnMove)
  EVT_CLOSE(lxModelTreeDlg::OnClose)
END_EVENT_TABLE()


void lxModelTreeDlg::OnCommand(wxCommandEvent& event)
{
  switch (event.GetId()) {
    case wxID_CLOSE:
      this->m_mainFrame->ToggleSelectionSetup();
      break;


  }
}



void lxModelTreeDlg::OnClose(wxCloseEvent& WXUNUSED(event))
{
    this->m_mainFrame->ToggleSelectionSetup();
}

void lxModelTreeDlg::OnMove(wxMoveEvent& WXUNUSED(event))
{
  this->m_toolBoxPosition.Save();
}


lxModelTreeDlg::lxModelTreeDlg(wxWindow *parent)
                : wxMiniFrame(parent, wxID_ANY, _(" Selection"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
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

  this->m_treeControl = new wxTreeCtrl(lxPanel);

	sizerTop->Add( this->m_treeControl, 
		1, wxALL | wxEXPAND, lxBORDER);
	sizerTop->Add(
		new wxButton(lxPanel, wxID_CLOSE, _("Close")), 
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


void lxModelTreeDlg::LoadData()
{
  this->m_treeControl->DeleteAllItems();
  lxData * data = this->m_mainFrame->data;
  std::vector<wxTreeItemId> parents;
  lxDataSurveyVec::iterator i;
  if (data == NULL) return;
  i = data->surveys.begin();
  if (i == data->surveys.end()) return;
  wxTreeItemId tmpId = this->m_treeControl->AddRoot(_T("Surveys"));
  parents.push_back(tmpId);
  i++;
  for(; i != data->surveys.end(); i++) {
    parents.push_back(this->m_treeControl->AppendItem(parents[i->m_parent], wxConvUTF8.cMB2WX(strlen(i->m_title) > 0 ? i->m_title : i->m_name)));
  }
}



