// Standard libraries
#ifndef LXDEPCHECK
#include <wx/statline.h>
#include <wx/xml/xml.h>
#include <wx/filedlg.h>
#endif
//LXDEPCHECK - standard libraries

#include "lxPres.h"
#include "lxSetup.h"
#include "lxGUI.h"
#include "lxGLC.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif


enum {
  lxPR_LIST = 4000,
};


BEGIN_EVENT_TABLE(lxPresentDlg, wxMiniFrame)
  EVT_BUTTON(wxID_ANY, lxPresentDlg::OnCommand)
  EVT_BUTTON(wxID_CLOSE, lxPresentDlg::OnCommand)
	EVT_LISTBOX(lxPR_LIST, lxPresentDlg::OnCommand)
  EVT_MOVE(lxPresentDlg::OnMove)
  EVT_CLOSE(lxPresentDlg::OnClose)
END_EVENT_TABLE()


bool lxPresentDlg::SavePresentation(bool saveas)
{
  bool rv = true;
  wxString defFName = _T("presentation.lxp");
  if (this->m_fileName.empty()) {
    saveas = true;
  } else {
    defFName = this->m_fileName;
  }
  if (this->m_fileDir.empty()) {
    this->m_fileDir = this->m_mainFrame->m_fileDir;
  }
  if (saveas) {
    wxFileDialog dialog(
                this,
                _("Save presentation"),
                wxEmptyString,
                defFName,
                _("Loch presentation file (*.lxp)|*.lxp"),
                wxFD_SAVE | wxFD_OVERWRITE_PROMPT
              );
    dialog.SetDirectory(this->m_fileDir);
    dialog.CentreOnParent();
    if (dialog.ShowModal() == wxID_OK) {
      this->m_fileName = dialog.GetPath();
      this->m_fileDir = dialog.GetDirectory();
      saveas = false;
    } else {
      rv = false;
    }
  }
  if (!saveas) {
    this->m_mainFrame->m_pres->Save(this->m_fileName);
    this->m_changed = false;
  }
  return rv;
}


void lxPresentDlg::LoadPresentation()
{
  if (this->m_fileDir.empty()) {
    this->m_fileDir = this->m_mainFrame->m_fileDir;
  }
  if (this->m_changed) {
    wxMessageDialog dlg(this, _("Presentation was changed. Save it?"), _("Warning"), wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_EXCLAMATION | wxCENTRE);
    switch (dlg.ShowModal()) {
      case wxID_CANCEL:
        return;
      case wxID_YES:
        if (!this->SavePresentation()) return;
        break;
    }
  }
  wxFileDialog dialog
         (
            this,
            _("Open"),
            wxEmptyString,
            wxEmptyString,
            _("Loch presentation file (*.lxp)|*.lxp")
          );
  dialog.SetDirectory(this->m_fileDir);
  dialog.CentreOnParent();

  if (dialog.ShowModal() == wxID_OK) {
    this->ResetPresentation();
    this->m_fileName = dialog.GetPath();
    this->m_fileDir  = dialog.GetDirectory();
    this->m_mainFrame->m_pres->Load(dialog.GetPath());
    this->UpdateList();
  }
}


void lxPresentDlg::ResetPresentation(bool save) {
  wxXmlNode * r;
  if (save && this->m_changed) {
    wxMessageDialog dlg(this, _("Presentation was changed. Save it?"), _("Warning"), wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_EXCLAMATION | wxCENTRE);
    switch (dlg.ShowModal()) {
      case wxID_CANCEL:
        return;
      case wxID_YES:
        if (!this->SavePresentation()) return;
        break;
    }
  }
  r = new wxXmlNode(wxXML_ELEMENT_NODE, _T("LochPresentation"));
  this->m_mainFrame->m_pres->SetRoot(r);
  this->m_changed = false;
  this->m_posLBox->Clear();
}

void lxPresentDlg::UpdateList() {
  this->m_posLBox->Clear();
  wxXmlNode * n;
  if (this->m_mainFrame->m_pres->GetRoot() != NULL) {
    n = this->m_mainFrame->m_pres->GetRoot()->GetChildren();
    long time = 0;
    while (n != NULL) {
      if (n->GetName() == _T("Scene")) {
        this->m_posLBox->AppendString(wxString::Format(_T("%04ld"), time));
        time++;
      }
      n = n->GetNext();
    }
  }
}


void lxPresentDlg::UpdateControls() {
  int sel = this->m_posLBox->GetSelection();
  size_t count = this->m_posLBox->GetCount();
  wxWindow::FindWindowById(LXMENU_PRESUPDATE, this)->Enable(count > 0);
  wxWindow::FindWindowById(LXMENU_PRESDELETE, this)->Enable(count > 0);
  wxWindow::FindWindowById(LXMENU_PRESMOVEDOWN, this)->Enable((count > 0) && (sel != wxNOT_FOUND) && ((size_t(sel) + 1) < count));
  wxWindow::FindWindowById(LXMENU_PRESMOVEUP, this)->Enable((count > 0) && (sel != wxNOT_FOUND) && (sel > 0));
}




void lxPresentDlg::OnCommand(wxCommandEvent& event)
{
  wxXmlNode * n, * r, * p;
  long c, sel;
  r = this->m_mainFrame->m_pres->GetRoot();
  if (r == NULL) {
    this->ResetPresentation();
    r = this->m_mainFrame->m_pres->GetRoot();
  }

  switch (event.GetId()) {

    case wxID_CLOSE:
      this->m_mainFrame->TogglePresentationDlg();
      break;

    case LXMENU_PRESMARK:
      p = new wxXmlNode(wxXML_ELEMENT_NODE, _T("Scene"));
      this->m_mainFrame->setup->SaveToXMLNode(p);
      sel = this->m_posLBox->GetSelection();
      if (sel == wxNOT_FOUND) {
        r->AddChild(p);
        sel = this->m_posLBox->GetCount();
      } else {
        n = r->GetChildren();
        c = 0;
        while (n != NULL) {
          if (n->GetName() == _T("Scene")) {
            if (sel == c) {
              r->InsertChildAfter(p, n);
              sel++;
              break;
            }
            c++;
          }
          n = n->GetNext();
        }
      }
      this->UpdateList();
      this->m_posLBox->Select(sel);
      this->UpdateControls();
      //this->m_posLBox->EnsureVisible(this->m_posLBox->GetCount()-1);
      this->m_changed = true;
      break; 

    case LXMENU_PRESMOVEDOWN:
    case LXMENU_PRESMOVEUP:
    case LXMENU_PRESDELETE:
    case LXMENU_PRESUPDATE:
    case lxPR_LIST:
      n = r->GetChildren();
      p = NULL;
      c = 0;
      if (event.GetId() == lxPR_LIST)
        sel = event.GetSelection();
      else {
        sel = this->m_posLBox->GetSelection();
        if (sel == wxNOT_FOUND) break;
      }
      while (n != NULL) {
        if (n->GetName() == _T("Scene")) {
          if (sel == c) {
            switch (event.GetId()) {
              case lxPR_LIST:
                this->m_mainFrame->setup->LoadFromXMLNode(n);
                this->m_mainFrame->canvas->ForceRefresh();
                this->m_mainFrame->UpdateM2TB();
                break;
              case LXMENU_PRESUPDATE:
                this->m_mainFrame->setup->SaveToXMLNode(n);
                break;
              case LXMENU_PRESDELETE:
                r->RemoveChild(n);
                delete n;
                this->UpdateList();
                if (this->m_posLBox->GetCount() > 0)
                  this->m_posLBox->Select(static_cast<intmax_t>(this->m_posLBox->GetCount()) > c ? c : c-1);
                break;
              case LXMENU_PRESMOVEUP:
                if (c > 0) {
                  r->RemoveChild(n);
                  r->InsertChild(n, p);
                  this->UpdateList();
                  this->m_posLBox->Select(c-1);
                }
                break;
              case LXMENU_PRESMOVEDOWN:
                if ((c+1) < static_cast<intmax_t>(this->m_posLBox->GetCount())) {
                  p = n->GetNext();
                  r->RemoveChild(n);
                  r->InsertChildAfter(n, p);
                  this->UpdateList();
                  this->m_posLBox->Select(c+1);
                }
                break;
            }
            break;
          }
          c++;
          p = n;
        }
        n = n->GetNext();
      }
      break;

    case LXMENU_PRESSAVE:
      this->SavePresentation();
      break;

    case LXMENU_PRESSAVEAS:
      this->SavePresentation(true);
      break;

    case LXMENU_PRESNEW:
      this->ResetPresentation(true);
      break;

    case LXMENU_PRESOPEN:
      this->LoadPresentation();
      this->m_posLBox->Select(0);
      break;

  }
  this->UpdateControls();
}



void lxPresentDlg::OnClose(wxCloseEvent& WXUNUSED(event))
{
    this->m_mainFrame->TogglePresentationDlg();
}

void lxPresentDlg::OnMove(wxMoveEvent& WXUNUSED(event))
{
  this->m_toolBoxPosition.Save();
}


lxPresentDlg::lxPresentDlg(wxWindow *parent)
                : wxMiniFrame(parent, wxID_ANY, _("Presentation"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
{
  this->m_toolBoxPosition.Init(this, parent, 0, 8, 8);

#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

  this->m_mainFrame = (lxFrame *) parent;
  this->m_fileName = wxEmptyString;

   
  wxBoxSizer * sizerFrame = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * sizerTop = new wxBoxSizer(wxHORIZONTAL);

  lxPanel = new wxPanel(this, wxID_ANY);
  this->m_posLBox = new wxListBox(lxPanel, lxPR_LIST);

  wxBoxSizer * controlSizer = new wxBoxSizer(wxVERTICAL);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMARK, _("Mark")), 
		0, wxALIGN_RIGHT | wxALL);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESUPDATE, _("Update")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMOVEUP, _("Move up")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMOVEDOWN, _("Move down")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESDELETE, _("Delete")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add( \
			new wxStaticLine(lxPanel, wxID_ANY), \
	    0, wxBOTTOM | wxTOP | wxEXPAND, lxBORDER);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESNEW, _("New")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESOPEN, _("Open...")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESSAVE, _("Save")), 
		0, wxALIGN_RIGHT | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESSAVEAS, _("Save as...")), 
		0, wxALIGN_RIGHT | lxNOTTOP);


  sizerTop->Add(m_posLBox, 1, wxTOP | wxBOTTOM | wxLEFT | wxEXPAND, lxBORDER);
  lxBoxSizer = new wxBoxSizer(wxVERTICAL);
  lxBoxSizer->Add(controlSizer, 1, wxEXPAND, lxBORDER);
  lxBoxSizer->Add(
		new wxButton(lxPanel, wxID_CLOSE, _("Close")), 
		0, wxALIGN_RIGHT);
  sizerTop->Add(lxBoxSizer, 
		0, wxALL | wxEXPAND, lxBORDER);

  lxPanel->SetSizer(sizerTop);
  sizerTop->Fit(lxPanel);

  sizerFrame->Add(lxPanel, 1, wxEXPAND | wxALL);

  this->SetSizer(sizerFrame);
  sizerFrame->SetSizeHints(this);
  sizerFrame->Fit(this);

  wxSize mfs = this->m_mainFrame->GetSize();
  this->SetSize(mfs.GetWidth() / 4, mfs.GetHeight() / 2);

  this->UpdateControls();

}




