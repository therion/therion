#include "lxWX.h"
#include "lxGUI.h"

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/dirdlg.h>
#include <wx/valgen.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/valtext.h>
#endif  
//LXDEPCHECK - standard libraries


enum {
  LXOD_START = 6000,
  LXOD_INIT_DIR_PATH,
  LXOD_INIT_DIR_BROWSE,
  LXOD_GLASSES,
  LXOD_END,
};


class lxOptionsDlg : public wxDialog {

public:

  long tmp = 0;
  lxFrame * m_lxframe;

  lxOptionsDlg(wxWindow * parent);
  void OnCommand(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()

};


BEGIN_EVENT_TABLE(lxOptionsDlg, wxDialog)
//EVT_UPDATE_UI_RANGE(LXOD_START, LXOD_END, lxOptionsDlg::OnUpdateUI)
EVT_BUTTON(LXOD_INIT_DIR_BROWSE, lxOptionsDlg::OnCommand)
END_EVENT_TABLE()


lxOptionsDlg::lxOptionsDlg(wxWindow * parent)
: wxDialog(parent, wxID_ANY, wxString(_("Options")))
{

  this->m_lxframe = (lxFrame *) parent;

  wxBoxSizer * sizerAll = new wxBoxSizer(wxVERTICAL);
  lxStaticBoxSizer = new wxStaticBoxSizer(
    new wxStaticBox(this, wxID_ANY, _("General options")),
    wxVERTICAL);

  lxStaticBoxSizer->Add(
    new wxStaticText(this, wxID_ANY, _("Default data files directory")),
    0, wxALIGN_LEFT | wxALL, lxBORDER);

  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxTextCtrl(this, LXOD_INIT_DIR_PATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(0, &this->m_lxframe->m_iniDirectory)), 
    1, wxALIGN_CENTER | lxNOTTOP, lxBORDER);
  lxBoxSizer->Add(new wxButton(this, LXOD_INIT_DIR_BROWSE, _("Browse...")), 0, wxALIGN_LEFT | wxRIGHT | wxBOTTOM, lxBORDER);
  lxStaticBoxSizer->Add(lxBoxSizer, 0, wxEXPAND);

  lxStaticBoxSizer->Add(
    new wxStaticText(this, wxID_ANY, _("Default stereo colors")),
    0, wxALIGN_LEFT | lxNOTTOP, lxBORDER);

  wxChoice * ch = new wxChoice(this, LXOD_GLASSES, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxGenericValidator(&this->m_lxframe->m_iniStereoGlasses));
  ch->Append(_("red & cyan"));
  ch->Append(_("red & green"));
  ch->Append(_("red & blue"));
  ch->Append(_("yellow & blue"));
  ch->Append(_("cyan & red"));
  ch->Append(_("green & red"));
  ch->Append(_("blue & red"));
  ch->Append(_("blue & yellow"));
  ch->Append(_("remember last used channels"));

  lxStaticBoxSizer->Add(ch, 0, wxEXPAND | lxNOTTOP, lxBORDER);

  lxStaticBoxSizer->Add(
    new wxStaticText(this, wxID_ANY, _("Measurement system")),
    0, wxALIGN_LEFT | lxNOTTOP, lxBORDER);
  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxRadioButton(this, wxID_ANY, _("Metric"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, lxRadioBtnValidator(&this->m_lxframe->m_iniUnits, LXUNITS_METRIC)),
    0, wxALIGN_LEFT | lxNOTTOP, lxBORDER);
  lxBoxSizer->Add(new wxRadioButton(this, wxID_ANY, _("Imperial"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_lxframe->m_iniUnits, LXUNITS_IMPERIAL)),
    0, wxALIGN_LEFT | wxRIGHT | wxBOTTOM, lxBORDER);
  lxStaticBoxSizer->Add(lxBoxSizer, 0, wxEXPAND);

  sizerAll->Add(lxStaticBoxSizer, 0, wxEXPAND | wxALL, lxBORDER);


  lxStaticBoxSizer = new wxStaticBoxSizer(
    new wxStaticBox(this, wxID_ANY, _("Walls extrapolation")),
    wxVERTICAL);

  lxStaticBoxSizer->Add(
    new wxRadioButton(this, wxID_ANY, _("Do not extrapolate walls"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, lxRadioBtnValidator(&this->m_lxframe->m_iniWallsInterpolate, LXWALLS_INTERP_NONE)),
    0, wxALIGN_LEFT | wxALL, lxBORDER);
  lxStaticBoxSizer->Add(
    new wxRadioButton(this, wxID_ANY, _("Extrapolate only files without walls information"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_lxframe->m_iniWallsInterpolate, LXWALLS_INTERP_ALL_ONLY)),
    0, wxALIGN_LEFT | wxALL, lxBORDER);
  lxStaticBoxSizer->Add(
    new wxRadioButton(this, wxID_ANY, _("Extrapolate all missing walls information"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_lxframe->m_iniWallsInterpolate, LXWALLS_INTERP_MISSING)),
    0, wxALIGN_LEFT | wxALL, lxBORDER);


  sizerAll->Add(lxStaticBoxSizer, 0, wxEXPAND | lxNOTTOP, lxBORDER);

  // Bottom line
  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxButton(this, wxID_OK, _("OK")), 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, lxBORDER);
  lxBoxSizer->Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 0, wxALIGN_CENTER | wxRIGHT | wxBOTTOM, lxBORDER);
  sizerAll->Add(lxBoxSizer, 0, wxALIGN_RIGHT);

  lxFButton(wxID_CANCEL)->SetFocus();
  SetSizer(sizerAll);
  sizerAll->SetSizeHints(this);
  sizerAll->Fit(this);
  this->CentreOnParent();

}


void lxOptionsDlg::OnCommand(wxCommandEvent& event)
{
  switch (event.GetId()) {
    case LXOD_INIT_DIR_BROWSE:
      {
        wxDirDialog dd(this);
        dd.SetPath(lxFTextCtrl(LXOD_INIT_DIR_PATH)->GetValue());
        if (dd.ShowModal() == wxID_OK) {
          lxFTextCtrl(LXOD_INIT_DIR_PATH)->SetValue(dd.GetPath());
        }
      }
      break;
//    case wxID_OK:
//      this->m_lxframe->m_iniDirectory = lxFTextCtrl(LXOD_INIT_DIR_PATH)->GetValue();
//      this->m_lxframe->m_iniStereoGlasses = lxFChoice(LXOD_GLASSES)->GetSelection();
//      break;
  }
}


void lxShowOptionsDialog(wxWindow * parent)
{
  lxOptionsDlg od(parent);
  od.ShowModal();
}
