// Standard libraries
#ifndef LXDEPCHECK
#include <wx/statline.h>
#include <wx/busyinfo.h>
#endif  
//LXDEPCHECK - standard libraries

#include "lxSTree.h"
#include "lxSetup.h"
#include "lxGUI.h"
#include "lxGLC.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif

#include <cstdio>

enum {
  lxSS_SURVEY_TREE = 4000,  
};


class SurveyTreeData : public wxTreeItemData
{
public:
   size_t m_id;
   SurveyTreeData(size_t id) 
     : wxTreeItemData()
   {
       this->m_id = id;
   };
};

BEGIN_EVENT_TABLE(lxModelTreeDlg, wxMiniFrame)
  EVT_BUTTON(wxID_ANY, lxModelTreeDlg::OnCommand)
  EVT_BUTTON(wxID_OK, lxModelTreeDlg::OnCommand)
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
    case wxID_OK: 
    	wxBusyInfo info(_("Building 3D model, please wait..."));
#if wxCHECK_VERSION(3,0,0)
    	wxWindowDisabler disableAll;
#endif
#if wxCHECK_VERSION(3,0,0)
    	wxTheApp->Yield();
#endif
		this->m_mainFrame->data->ClearSurveySelection();
		wxArrayTreeItemIds csel;
		size_t selcnt = this->m_treeControl->GetSelections(csel);
    	if (selcnt > 0) {
    		for(size_t i = 0; i < selcnt; i++) {
    			SurveyTreeData * data = (SurveyTreeData *) this->m_treeControl->GetItemData(csel.Item(i));
        		if (data != NULL) {
        			this->m_mainFrame->data->AddSelectedSurvey(data->m_id);
        		}
    		}
    	}
		this->m_mainFrame->data->Rebuild();
#if wxCHECK_VERSION(3,0,0)
    	wxTheApp->Yield();
#endif
		this->m_mainFrame->canvas->UpdateContents();
		this->m_mainFrame->setup->ResetCamera();
		this->m_mainFrame->canvas->ForceRefresh();
#if wxCHECK_VERSION(3,0,0)
    	wxTheApp->Yield();
#endif
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

  this->m_treeControl = new wxTreeCtrl(lxPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTR_MULTIPLE);

  wxBoxSizer * lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxButton(lxPanel, wxID_OK, _("Select")), 0, wxALIGN_CENTER | wxRIGHT, lxBORDER);
  lxBoxSizer->Add(new wxButton(lxPanel, wxID_CLOSE, _("Close")), 0, wxALIGN_CENTER , lxBORDER);

  
	sizerTop->Add( this->m_treeControl, 
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


void lxModelTreeDlg::LoadData()
{
  this->m_treeControl->DeleteAllItems();
  lxData * data = this->m_mainFrame->data;
  std::vector<wxTreeItemId> parents;
  std::list<wxTreeItemId> toexpand;
  lxDataSurveyVec::iterator i;
  if (data == NULL) return;
  i = data->surveys.begin();
  if (i == data->surveys.end()) return;
  wxTreeItemId tmpId = this->m_treeControl->AddRoot(_T("Surveys"));
  parents.push_back(tmpId);
  toexpand.push_back(tmpId);
  int add;
  add = 1;
  if (strlen(i->m_name) == 0) {
    i++;
    add = 0;
  }
  for(; i != data->surveys.end(); i++) {
    //printf("%s - parent %d\n",strlen(i->m_title) > 0 ? i->m_title : i->m_name,i->m_parent);
	tmpId = this->m_treeControl->AppendItem(parents[(i->m_parent + add) < parents.size() ? (i->m_parent + add) : 0], wxConvUTF8.cMB2WX(strlen(i->m_title) > 0 ? i->m_title : i->m_name), -1, -1, new SurveyTreeData(i->m_id));
    parents.push_back(tmpId);
    if (i->m_level < 3) toexpand.push_back(tmpId); 
  }
  for(auto ti = toexpand.begin(); ti != toexpand.end(); ti++) {
	this->m_treeControl->Expand(*ti);
  }
  
}



