#include "lxWX.h"
#include "icons/about.xpm"
#include "../thversion.h"

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/panel.h>
#endif  
//LXDEPCHECK - standart libraries


class lxAboutDlg : public wxDialog {

public:

  lxAboutDlg(wxWindow * parent);
  ~lxAboutDlg();

  void OnKeyPress(wxKeyEvent&);
  void OnMouseDown(wxMouseEvent&);

  DECLARE_EVENT_TABLE();

};


enum {
  lxABDG_TEXT1 = 6000,
  lxABDG_TEXT2,
  lxABDG_TEXT3,
  lxABDG_BMP,
};


BEGIN_EVENT_TABLE(lxAboutDlg, wxDialog)
EVT_KEY_DOWN(lxAboutDlg::OnKeyPress)
EVT_LEFT_DOWN(lxAboutDlg::OnMouseDown)
END_EVENT_TABLE()




void lxAboutDlg::OnKeyPress(wxKeyEvent& WXUNUSED(event)) {
  this->Destroy();
}


void lxAboutDlg::OnMouseDown(wxMouseEvent&WXUNUSED(event)) {
  this->Destroy();
}


lxAboutDlg::lxAboutDlg(wxWindow * parent)
: wxDialog(parent, wxID_ANY, wxString(_("About Loch")))

//, wxDefaultPosition, wxDefaultSize,
//#if defined(__WXX11__)
//                     wxTHICK_FRAME
//#else
//                     wxSIMPLE_BORDER
//#endif
//                     | wxFRAME_TOOL_WINDOW)
{
  wxBitmap bmp(about_xpm);
  wxStaticBitmap * pbmp;
  wxFont tmpFont;
  this->SetBackgroundColour(wxColour(_T("black")));
  this->SetForegroundColour(_T("white"));
  pbmp = new wxStaticBitmap(this, lxABDG_BMP, bmp);

  wxBoxSizer * sizerBmp = new wxBoxSizer(wxHORIZONTAL);
  sizerBmp->AddSpacer(100);
  sizerBmp->Add(pbmp, 0, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL, 0);

  wxBoxSizer * sizerAll = new wxBoxSizer(wxVERTICAL);
  sizerAll->Add(new wxStaticText(this, lxABDG_TEXT1,_T("Loch")), 0, wxALL, lxBORDER);
  sizerAll->Add(new wxStaticText(this, lxABDG_TEXT2,_("3D visualization tool for caves")), 0, lxNOTTOP, lxBORDER);
  wxString lxVer;
  lxVer = _T("Version: ");
  lxVer += _T("" THVERSION "");
  sizerAll->Add(new wxStaticText(this, lxABDG_TEXT3, lxVer), 0, lxNOTTOP, lxBORDER);
  
  wxBoxSizer * sizerAllAbove = new wxBoxSizer(wxHORIZONTAL);
  sizerAllAbove->Add(sizerAll, 0, wxALL, 2 * lxBORDER);
  SetSizer(sizerAllAbove);

  sizerBmp->SetSizeHints(this);
  sizerBmp->Fit(this);

  tmpFont = lxFStaticText(lxABDG_TEXT1)->GetFont();
  tmpFont.SetWeight(wxFONTWEIGHT_BOLD);
  lxFStaticText(lxABDG_TEXT1)->SetFont(tmpFont);
  wxSize tmpsize = this->GetClientSize();
  pbmp->SetSize(tmpsize.x - bmp.GetWidth(), tmpsize.y - bmp.GetHeight(), -1, -1);
  this->CentreOnParent();

  FindWindow(lxABDG_TEXT1)->SetEventHandler(this->GetEventHandler());
  FindWindow(lxABDG_TEXT2)->SetEventHandler(this->GetEventHandler());
  FindWindow(lxABDG_TEXT3)->SetEventHandler(this->GetEventHandler());
  FindWindow(lxABDG_BMP)->SetEventHandler(this->GetEventHandler());

  delete sizerBmp;

}

lxAboutDlg::~lxAboutDlg()
{
  //delete this->m_bmp;
}

void lxShowAboutDialog(wxWindow * parent)
{
  lxAboutDlg ad(parent);
  ad.ShowModal();
}
