#include <wx/statline.h>
#include <wx/xml/xml.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/ffile.h>
#include <wx/utils.h>
#include <cstdint>
#include <math.h>

#include "lxPres.h"
#include "lxSetup.h"
#include "lxGUI.h"
#include "lxGLC.h"
#include "lxRender.h"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif


enum {
  lxPR_LIST = 4000,
  lxPR_EXPORT,
  lxPR_OPTIONS,
  lxPR_PLAY,
};

#ifdef LXWIN32
static wxString lxQuoteBatArg(const wxString & value)
{
  wxString quoted = _T("\"");
  quoted += value;
  quoted.Replace(_T("%"), _T("%%"));
  quoted += _T("\"");
  return quoted;
}
#else
static wxString lxQuoteShellArg(const wxString & value)
{
  wxString quoted = _T("'");
  for (size_t i = 0; i < value.length(); i++) {
    if (value[i] == _T('\''))
      quoted += _T("'\\''");
    else
      quoted += value[i];
  }
  quoted += _T("'");
  return quoted;
}
#endif

static wxString lxFormatTimestamp(double seconds)
{
  long total = long(seconds + 0.5);
  long hours = total / 3600;
  long minutes = (total / 60) % 60;
  long secs = total % 60;

  return wxString::Format(_T("%02ld:%02ld:%02ld"), hours, minutes, secs);
}

static int lxGetMaxButtonWidth(wxSizer * sizer)
{
  int width = 0;
  wxSizerItemList::compatibility_iterator node;

  if (sizer == NULL)
    return 0;

  for (node = sizer->GetChildren().GetFirst(); node; node = node->GetNext()) {
    wxSizerItem * item = node->GetData();
    wxWindow * window = item->GetWindow();
    wxSizer * childSizer = item->GetSizer();
    wxButton * button = dynamic_cast<wxButton *>(window);

    if (button != NULL)
      width = wxMax(width, button->GetBestSize().GetWidth());
    if (childSizer != NULL)
      width = wxMax(width, lxGetMaxButtonWidth(childSizer));
  }

  return width;
}

static void lxSetButtonWidth(wxSizer * sizer, int width)
{
  wxSizerItemList::compatibility_iterator node;

  if ((sizer == NULL) || (width <= 0))
    return;

  for (node = sizer->GetChildren().GetFirst(); node; node = node->GetNext()) {
    wxSizerItem * item = node->GetData();
    wxWindow * window = item->GetWindow();
    wxSizer * childSizer = item->GetSizer();
    wxButton * button = dynamic_cast<wxButton *>(window);

    if (button != NULL)
      button->SetMinSize(wxSize(width, -1));
    if (childSizer != NULL)
      lxSetButtonWidth(childSizer, width);
  }
}

static void lxSetUniformButtonWidths(wxSizer * sizer)
{
  lxSetButtonWidth(sizer, lxGetMaxButtonWidth(sizer));
}


BEGIN_EVENT_TABLE(lxPresentDlg, wxMiniFrame)
  EVT_BUTTON(wxID_ANY, lxPresentDlg::OnCommand)
  EVT_BUTTON(wxID_CLOSE, lxPresentDlg::OnCommand)
	EVT_LIST_ITEM_SELECTED(lxPR_LIST, lxPresentDlg::OnListItemSelected)
  EVT_MOVE(lxPresentDlg::OnMove)
  EVT_CLOSE(lxPresentDlg::OnClose)
END_EVENT_TABLE()


bool lxPresentDlg::SavePresentation(bool saveas)
{
  bool rv = true;
  wxString defFName = _T("presentation.lxa");
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
                _("Loch animation file (*.lxa)|*.lxa"),
                wxFD_SAVE | wxFD_OVERWRITE_PROMPT
              );
    dialog.SetDirectory(this->m_fileDir);
    dialog.CentreOnParent();
    if (dialog.ShowModal() == wxID_OK) {
      wxFileName fileName(dialog.GetPath());
      if (fileName.GetExt().empty())
        fileName.SetExt(_T("lxa"));
      this->m_fileName = fileName.GetFullPath();
      this->m_fileDir = dialog.GetDirectory();
      saveas = false;
    } else {
      rv = false;
    }
  }
  if (!saveas) {
    this->SetLoopAnimation(this->GetLoopAnimation());
    this->SetSceneChanges(this->GetSceneChanges());
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
            _("Loch animation file (*.lxa)|*.lxa|All files (*.*)|*.*")
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
  r->AddAttribute(_T("loop-animation"), _T("true"));
  r->AddAttribute(_T("scene-changes"), _T("false"));
  this->m_mainFrame->m_pres->SetRoot(r);
  this->m_changed = false;
  this->m_posLBox->DeleteAllItems();
}

void lxPresentDlg::UpdateList() {
  this->m_posLBox->DeleteAllItems();
  wxXmlNode * n;
  if (this->m_mainFrame->m_pres->GetRoot() != NULL) {
    n = this->m_mainFrame->m_pres->GetRoot()->GetChildren();
    long time = 0;
    double timestamp = 0.0;
    wxXmlNode * prevScene = NULL;
    while (n != NULL) {
      if (n->GetName() == _T("Scene")) {
        if (prevScene != NULL) {
          double duration = 3.0;
          double rotationDuration = 15.0;
          long rotations = 0;
          this->GetSceneDuration(n).ToDouble(&duration);
          this->GetSceneRotationDuration(prevScene).ToDouble(&rotationDuration);
          this->GetSceneRotations(prevScene).ToLong(&rotations);
          timestamp += fabs(double(rotations)) * rotationDuration + duration;
        }
        long item = this->m_posLBox->InsertItem(time, this->GetSceneLabel(n, time));
        this->m_posLBox->SetItem(item, 1, lxFormatTimestamp(timestamp));
        time++;
        prevScene = n;
      }
      n = n->GetNext();
    }
  }
}

long lxPresentDlg::GetSelection() {
  return this->m_posLBox->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

void lxPresentDlg::SelectScene(long index) {
  if ((index < 0) || (index >= this->m_posLBox->GetItemCount()))
    return;

  this->m_posLBox->SetItemState(index, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
  this->m_posLBox->EnsureVisible(index);
}

wxXmlNode * lxPresentDlg::GetScene(long index) {
  wxXmlNode * r = this->m_mainFrame->m_pres->GetRoot();
  wxXmlNode * n;
  long c = 0;

  if (r == NULL)
    return NULL;

  n = r->GetChildren();
  while (n != NULL) {
    if (n->GetName() == _T("Scene")) {
      if (index == c)
        return n;
      c++;
    }
    n = n->GetNext();
  }

  return NULL;
}

wxString lxPresentDlg::GetSceneLabel(wxXmlNode * n, long index) {
  wxString name;

  if (n != NULL)
    name = n->GetAttribute(_T("name"), wxEmptyString);

  if (name.empty())
    name = wxString::Format(_T("%04ld"), index);

  return name;
}

wxString lxPresentDlg::GetSceneDuration(wxXmlNode * n) {
  wxString duration;
  double seconds;

  if (n != NULL)
    duration = n->GetAttribute(_T("duration"), wxEmptyString);

  if (duration.empty() || !duration.ToDouble(&seconds) || (seconds <= 0.0))
    duration = _T("3");

  return duration;
}

wxString lxPresentDlg::GetSceneRotations(wxXmlNode * n) {
  wxString rotations;
  long value;

  if (n != NULL)
    rotations = n->GetAttribute(_T("rotations"), wxEmptyString);

  if (rotations.empty() || !rotations.ToLong(&value))
    rotations = _T("0");

  return rotations;
}

wxString lxPresentDlg::GetSceneRotationDuration(wxXmlNode * n) {
  wxString duration;
  double seconds;

  if (n != NULL)
    duration = n->GetAttribute(_T("rotation-duration"), wxEmptyString);

  if (duration.empty() || !duration.ToDouble(&seconds) || (seconds <= 0.0))
    duration = _T("15");

  return duration;
}

bool lxPresentDlg::GetLoopAnimation() {
  wxXmlNode * r = this->m_mainFrame->m_pres->GetRoot();
  wxString value;

  if (r == NULL)
    return true;

  value = r->GetAttribute(_T("loop-animation"), _T("true"));
  return (value != _T("false")) && (value != _T("0"));
}

bool lxPresentDlg::GetSceneChanges() {
  wxXmlNode * r = this->m_mainFrame->m_pres->GetRoot();
  wxString value;

  if (r == NULL)
    return false;

  value = r->GetAttribute(_T("scene-changes"), _T("false"));
  return (value == _T("true")) || (value == _T("1"));
}

void lxPresentDlg::SetLoopAnimation(bool value) {
  wxXmlNode * r = this->m_mainFrame->m_pres->GetRoot();

  if (r == NULL)
    return;

  r->DeleteAttribute(_T("loop-animation"));
  r->AddAttribute(_T("loop-animation"), value ? _T("true") : _T("false"));
}

void lxPresentDlg::SetSceneChanges(bool value) {
  wxXmlNode * r = this->m_mainFrame->m_pres->GetRoot();

  if (r == NULL)
    return;

  r->DeleteAttribute(_T("scene-changes"));
  r->AddAttribute(_T("scene-changes"), value ? _T("true") : _T("false"));
}

void lxPresentDlg::ApplySceneChanges(wxXmlNode * n) {
  if (!this->GetSceneChanges())
    return;

  this->m_mainFrame->setup->LoadSceneFromXMLNode(n);
  this->m_mainFrame->canvas->UpdateContents();
}

void lxPresentDlg::EditOptions() {
  wxDialog dlg(this, wxID_ANY, _("Animation options"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
  wxBoxSizer * dialogSizer = new wxBoxSizer(wxVERTICAL);
  wxCheckBox * loopCheckBox = new wxCheckBox(&dlg, wxID_ANY, _("Loop animation"));
  wxCheckBox * sceneCheckBox = new wxCheckBox(&dlg, wxID_ANY, _("Scene changes"));

  loopCheckBox->SetValue(this->GetLoopAnimation());
  sceneCheckBox->SetValue(this->GetSceneChanges());

  dialogSizer->Add(loopCheckBox, 0, wxALL, lxBORDER);
  dialogSizer->Add(sceneCheckBox, 0, wxLEFT | wxRIGHT | wxBOTTOM, lxBORDER);
  dialogSizer->Add(dlg.CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxEXPAND, lxBORDER);
  dlg.SetSizer(dialogSizer);
  dialogSizer->SetSizeHints(&dlg);

  if (dlg.ShowModal() != wxID_OK)
    return;

  this->SetLoopAnimation(loopCheckBox->GetValue());
  this->SetSceneChanges(sceneCheckBox->GetValue());
  this->m_changed = true;
}

void lxPresentDlg::MarkCurrentView() {
  wxXmlNode * n, * r, * p;
  long c, sel;

  r = this->m_mainFrame->m_pres->GetRoot();
  if (r == NULL) {
    this->ResetPresentation();
    r = this->m_mainFrame->m_pres->GetRoot();
  }

  p = new wxXmlNode(wxXML_ELEMENT_NODE, _T("Scene"));
  this->m_mainFrame->setup->SaveToXMLNode(p);
  this->m_mainFrame->setup->SaveSceneToXMLNode(p);
  p->AddAttribute(_T("duration"), _T("3"));
  p->AddAttribute(_T("rotations"), _T("0"));
  p->AddAttribute(_T("rotation-duration"), _T("15"));
  sel = this->GetSelection();
  if (sel < 0) {
    r->AddChild(p);
    sel = this->m_posLBox->GetItemCount();
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
  this->SelectScene(sel);
  this->UpdateControls();
  this->m_changed = true;
  this->EditSelected();
}

void lxPresentDlg::EditSelected() {
  long sel = this->GetSelection();
  wxXmlNode * n;
  wxString name, duration, rotations, rotationDuration;
  double seconds, rotationSeconds;
  long rotationCount;

  if (sel < 0)
    return;

  n = this->GetScene(sel);
  if (n == NULL)
    return;

  wxDialog dlg(this, wxID_ANY, _("Edit view"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
  wxBoxSizer * dialogSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer * fieldSizer = new wxFlexGridSizer(2, 2, lxBORDER, lxBORDER);
  wxTextCtrl * nameTextCtrl = new wxTextCtrl(&dlg, wxID_ANY, this->GetSceneLabel(n, sel));
  wxTextCtrl * durationTextCtrl = new wxTextCtrl(&dlg, wxID_ANY, this->GetSceneDuration(n));
  wxTextCtrl * rotationsTextCtrl = new wxTextCtrl(&dlg, wxID_ANY, this->GetSceneRotations(n));
  wxTextCtrl * rotationDurationTextCtrl = new wxTextCtrl(&dlg, wxID_ANY, this->GetSceneRotationDuration(n));

  fieldSizer->Add(new wxStaticText(&dlg, wxID_ANY, _("Name")), 0, wxALIGN_CENTER_VERTICAL);
  fieldSizer->Add(nameTextCtrl, 1, wxEXPAND);
  fieldSizer->Add(new wxStaticText(&dlg, wxID_ANY, _("Transition duration (s)")), 0, wxALIGN_CENTER_VERTICAL);
  fieldSizer->Add(durationTextCtrl, 1, wxEXPAND);
  fieldSizer->Add(new wxStaticText(&dlg, wxID_ANY, _("Rotations")), 0, wxALIGN_CENTER_VERTICAL);
  fieldSizer->Add(rotationsTextCtrl, 1, wxEXPAND);
  fieldSizer->Add(new wxStaticText(&dlg, wxID_ANY, _("Rotation duration (s)")), 0, wxALIGN_CENTER_VERTICAL);
  fieldSizer->Add(rotationDurationTextCtrl, 1, wxEXPAND);
  fieldSizer->AddGrowableCol(1);

  dialogSizer->Add(fieldSizer, 1, wxALL | wxEXPAND, lxBORDER);
  dialogSizer->Add(dlg.CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxEXPAND, lxBORDER);
  dlg.SetSizer(dialogSizer);
  dialogSizer->SetSizeHints(&dlg);

  while (dlg.ShowModal() == wxID_OK) {
    name = nameTextCtrl->GetValue();
    duration = durationTextCtrl->GetValue();
    rotations = rotationsTextCtrl->GetValue();
    rotationDuration = rotationDurationTextCtrl->GetValue();
    if (duration.ToDouble(&seconds) && (seconds > 0.0) &&
      rotations.ToLong(&rotationCount) &&
      rotationDuration.ToDouble(&rotationSeconds) && (rotationSeconds > 0.0))
      break;

    wxMessageDialog msg(&dlg, _("Durations must be positive numbers and rotations must be an integer."), _("Warning"), wxOK | wxICON_EXCLAMATION | wxCENTRE);
    msg.ShowModal();
  }

  if (!duration.ToDouble(&seconds) || (seconds <= 0.0) ||
    !rotations.ToLong(&rotationCount) ||
    !rotationDuration.ToDouble(&rotationSeconds) || (rotationSeconds <= 0.0))
    return;

  n->DeleteAttribute(_T("name"));
  if (!name.empty())
    n->AddAttribute(_T("name"), name);

  n->DeleteAttribute(_T("duration"));
  n->AddAttribute(_T("duration"), duration);
  n->DeleteAttribute(_T("rotations"));
  n->AddAttribute(_T("rotations"), rotations);
  n->DeleteAttribute(_T("rotation-duration"));
  n->AddAttribute(_T("rotation-duration"), rotationDuration);

  this->UpdateList();
  this->SelectScene(sel);
  this->m_changed = true;
}


void lxPresentDlg::UpdateControls() {
  long sel = this->GetSelection();
  long count = this->m_posLBox->GetItemCount();
  wxXmlNode * n = this->GetScene(sel);

  if (this->m_playButton != NULL)
    this->m_playButton->SetLabel(this->m_mainFrame->canvas->m_sCameraPresentationAnimate ? _("Stop animation") : _("Play animation"));
  wxWindow::FindWindowById(LXMENU_PRESUPDATE, this)->Enable(n != NULL);
  wxWindow::FindWindowById(LXMENU_PRESDELETE, this)->Enable(n != NULL);
  wxWindow::FindWindowById(LXMENU_PRESMOVEDOWN, this)->Enable((n != NULL) && ((sel + 1) < count));
  wxWindow::FindWindowById(LXMENU_PRESMOVEUP, this)->Enable((n != NULL) && (sel > 0));
  wxWindow::FindWindowById(LXMENU_PRESEDIT, this)->Enable(n != NULL);
}

void lxPresentDlg::ExportPresentation() {
  long count = this->m_posLBox->GetItemCount();
  long totalFrames = 1;
  long frame = 0;
  bool rotationFallback = count < 2;
  bool exportMp4;
  bool loopAnimation = this->GetLoopAnimation();
  bool sceneChanges = this->GetSceneChanges();
  long sceneCountForExport;
  long lastAppliedScene = -1;
  wxString folderPath, targetPath, scriptPath;
  wxXmlNode savedSetup(wxXML_ELEMENT_NODE, _T("Scene"));

  if (this->m_fileDir.empty()) {
    this->m_fileDir = this->m_mainFrame->m_fileDir;
  }

  wxFileDialog dialog(
    this,
    _("Export presentation"),
    this->m_fileDir,
    _T("presentation.mp4"),
    _("MP4 files (*.mp4)|*.mp4|PNG image sequence (*.png)|*.png"),
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  dialog.SetFilterIndex(0);
  dialog.CentreOnParent();
  if (dialog.ShowModal() != wxID_OK)
    return;

  wxFileName outPath(dialog.GetPath());
  exportMp4 = dialog.GetFilterIndex() == 0;
  if (outPath.GetExt().empty())
    outPath.SetExt(exportMp4 ? _T("mp4") : _T("png"));
  targetPath = outPath.GetFullPath();
  folderPath = exportMp4 ? targetPath + _T(".images") : targetPath;

  if (wxFileName::FileExists(folderPath)) {
    wxMessageDialog dlg(this, _("Unable to create output folder; a file with this name already exists."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
    dlg.ShowModal();
    return;
  }

  if (!wxFileName::DirExists(folderPath) && !wxFileName::Mkdir(folderPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
    wxMessageDialog dlg(this, _("Unable to create output folder."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
    dlg.ShowModal();
    return;
  }

  if (rotationFallback) {
    totalFrames = 15 * 60;
  } else {
    sceneCountForExport = loopAnimation ? count : count - 1;
    for (long i = 0; i < sceneCountForExport; i++) {
      wxXmlNode * from = this->GetScene(i);
      wxXmlNode * to = this->GetScene((i + 1) % count);
      double duration = 3.0;
      double rotationDuration = 15.0;
      long rotations = 0;
      int transitionFrames;
      int rotationFrames;

      this->GetSceneRotations(from).ToLong(&rotations);
      this->GetSceneRotationDuration(from).ToDouble(&rotationDuration);
      rotationFrames = int(fabs(double(rotations)) * rotationDuration * 60.0 + 0.5);
      this->GetSceneDuration(to).ToDouble(&duration);
      transitionFrames = int(duration * 60.0 + 0.5);
      if (transitionFrames < 1)
        transitionFrames = 1;
      if (loopAnimation && (i == count - 1))
        transitionFrames--;
      totalFrames += rotationFrames + transitionFrames;
    }
  }

  wxProgressDialog progress(
    _("Export presentation"),
    _("Rendering presentation frames..."),
    totalFrames,
    this,
    wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

  lxRenderData * tmpRD = new lxRenderData();
  tmpRD->m_imgFileType = 1;
  tmpRD->m_askFName = false;
  tmpRD->m_scaleMode = LXRENDER_FIT_SCREEN;

  this->m_mainFrame->canvas->m_sCameraAutoRotate = false;
  this->m_mainFrame->canvas->StopCameraPresentationAnimation();
  this->m_mainFrame->setup->SaveToXMLNode(&savedSetup);
  this->m_mainFrame->setup->SaveSceneToXMLNode(&savedSetup);

  auto applyScene = [&](long index) -> void {
    if (!sceneChanges || (index == lastAppliedScene))
      return;

    wxXmlNode * scene = this->GetScene(index);
    if (scene == NULL)
      return;

    this->m_mainFrame->setup->LoadSceneFromXMLNode(scene);
    lastAppliedScene = index;
    this->m_mainFrame->canvas->UpdateRenderContents();
    this->m_mainFrame->canvas->UpdateRenderList();
  };

  auto renderFrame = [&]() -> bool {
    wxFileName framePath(folderPath, wxString::Format(_T("%06ld.png"), frame));
    tmpRD->m_imgFileName = framePath.GetFullPath();
    tmpRD->Render(this, this->m_mainFrame->canvas);
    this->m_mainFrame->canvas->SwapBuffers();
    frame++;
    return progress.Update(frame);
  };

  if (count == 1) {
    this->m_mainFrame->setup->LoadFromXMLNode(this->GetScene(0));
    applyScene(0);
  }
  bool keepGoing = true;

  if (rotationFallback) {
    double startDir = this->m_mainFrame->setup->cam_dir;
    this->m_mainFrame->setup->StartCameraMovement();
    for (int j = loopAnimation ? 1 : 0; keepGoing && (j < (loopAnimation ? (15 * 60 + 1) : (15 * 60))); j++) {
      double t = loopAnimation ? double(j) / double(15 * 60) : double(j) / double(15 * 60 - 1);
      this->m_mainFrame->setup->cam_dir = startDir + 360.0 * t;
      while (this->m_mainFrame->setup->cam_dir >= 360.0)
        this->m_mainFrame->setup->cam_dir -= 360.0;
      this->m_mainFrame->setup->UpdatePos();
      keepGoing = renderFrame();
    }
  }

  if (!rotationFallback) {
    this->m_mainFrame->setup->LoadFromXMLNode(this->GetScene(0));
    applyScene(0);
    keepGoing = renderFrame();
  }

  sceneCountForExport = loopAnimation ? count : count - 1;
  for (long i = 0; keepGoing && !rotationFallback && (i < sceneCountForExport); i++) {
    wxXmlNode * from = this->GetScene(i);
    wxXmlNode * to = this->GetScene((i + 1) % count);
    double duration = 3.0;
    double rotationDuration = 15.0;
    long rotations = 0;
    int transitionFrames;
    int rotationFrames;

    this->GetSceneRotations(from).ToLong(&rotations);
    this->GetSceneRotationDuration(from).ToDouble(&rotationDuration);
    rotationFrames = int(fabs(double(rotations)) * rotationDuration * 60.0 + 0.5);
    for (int j = 1; keepGoing && (j <= rotationFrames); j++) {
      double t = double(j) / double(rotationFrames);
      t = t * t * (3.0 - 2.0 * t);
      this->m_mainFrame->setup->LoadFromXMLNode(from);
      applyScene(i);
      this->m_mainFrame->setup->cam_dir += 360.0 * double(rotations) * t;
      while (this->m_mainFrame->setup->cam_dir >= 360.0)
        this->m_mainFrame->setup->cam_dir -= 360.0;
      this->m_mainFrame->setup->UpdatePos();
      keepGoing = renderFrame();
    }

    this->GetSceneDuration(to).ToDouble(&duration);
    transitionFrames = int(duration * 60.0 + 0.5);
    if (transitionFrames < 1)
      transitionFrames = 1;

    int lastTransitionFrame = (loopAnimation && (i == count - 1)) ? transitionFrames - 1 : transitionFrames;
    for (int j = 1; keepGoing && (j <= lastTransitionFrame); j++) {
      double t = double(j) / double(transitionFrames);
      t = t * t * (3.0 - 2.0 * t);
      this->m_mainFrame->setup->LoadFromXMLNode(from, to, t);
      if (j == transitionFrames)
        applyScene((i + 1) % count);
      else
        applyScene(i);
      keepGoing = renderFrame();
    }
  }

  this->m_mainFrame->setup->LoadFromXMLNode(&savedSetup);
  this->m_mainFrame->setup->LoadSceneFromXMLNode(&savedSetup);
  this->m_mainFrame->canvas->UpdateRenderContents();
  this->m_mainFrame->canvas->UpdateRenderList();
  this->m_mainFrame->canvas->ForceRefresh();
  this->m_mainFrame->UpdateM2TB();
  delete tmpRD;

  if (exportMp4 && keepGoing) {
#ifdef LXWIN32
    scriptPath = wxFileName(folderPath, _T(".animation.bat")).GetFullPath();
    wxString framePattern = wxFileName(folderPath, _T("%06d.png")).GetFullPath();
    wxString script =
      _T("@echo off\r\n")
      _T("ffmpeg -y -framerate 60 -i ") + lxQuoteBatArg(framePattern) +
      _T(" -c:v libx264 -pix_fmt yuv420p ") + lxQuoteBatArg(targetPath) + _T("\r\n");
    wxFFile file(scriptPath, _T("w"));
    if (!file.IsOpened() || !file.Write(script)) {
      wxMessageDialog dlg(this, _("Unable to create ffmpeg script."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
      dlg.ShowModal();
      return;
    }
    file.Close();
    if (wxExecute(_T("cmd.exe /C ") + lxQuoteBatArg(scriptPath), wxEXEC_SYNC) != 0) {
      wxMessageDialog dlg(this, _("ffmpeg failed. The exported images and script were left in place."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
      dlg.ShowModal();
    }
#else
    scriptPath = wxFileName(folderPath, _T(".animation.sh")).GetFullPath();
    wxString framePattern = wxFileName(folderPath, _T("%06d.png")).GetFullPath();
    wxString script =
      _T("#!/bin/sh\n")
      _T("ffmpeg -y -framerate 60 -i ") + lxQuoteShellArg(framePattern) +
      _T(" -c:v libx264 -pix_fmt yuv420p ") + lxQuoteShellArg(targetPath) + _T("\n");
    wxFFile file(scriptPath, _T("w"));
    if (!file.IsOpened() || !file.Write(script)) {
      wxMessageDialog dlg(this, _("Unable to create ffmpeg script."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
      dlg.ShowModal();
      return;
    }
    file.Close();
    if (wxExecute(_T("/bin/sh ") + lxQuoteShellArg(scriptPath), wxEXEC_SYNC) != 0) {
      wxMessageDialog dlg(this, _("ffmpeg failed. The exported images and script were left in place."), _("Error"), wxOK | wxICON_ERROR | wxCENTRE);
      dlg.ShowModal();
    }
#endif
  }
}

void lxPresentDlg::OnListItemSelected(wxListEvent& event)
{
  wxXmlNode * n = this->GetScene(event.GetIndex());

  if (n != NULL) {
    this->m_mainFrame->setup->LoadFromXMLNode(n);
    if (this->GetSceneChanges())
      this->ApplySceneChanges(n);
    else
      this->m_mainFrame->canvas->ForceRefresh();
    this->m_mainFrame->UpdateM2TB();
  }
  this->UpdateControls();
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

    case lxPR_PLAY:
      this->m_mainFrame->TogglePresentationAnimation();
      break;

    case LXMENU_PRESMARK:
      this->MarkCurrentView();
      break; 

    case LXMENU_PRESMOVEDOWN:
    case LXMENU_PRESMOVEUP:
    case LXMENU_PRESDELETE:
    case LXMENU_PRESUPDATE:
    case LXMENU_PRESEDIT:
      n = r->GetChildren();
      p = NULL;
      c = 0;
      sel = this->GetSelection();
      if (sel < 0) break;
      while (n != NULL) {
        if (n->GetName() == _T("Scene")) {
          if (sel == c) {
            switch (event.GetId()) {
              case LXMENU_PRESUPDATE:
                this->m_mainFrame->setup->SaveToXMLNode(n);
                this->m_mainFrame->setup->SaveSceneToXMLNode(n);
                this->m_changed = true;
                break;
              case LXMENU_PRESEDIT:
                this->EditSelected();
                break;
              case LXMENU_PRESDELETE:
                r->RemoveChild(n);
                delete n;
                this->UpdateList();
                if (this->m_posLBox->GetItemCount() > 0)
                  this->SelectScene(this->m_posLBox->GetItemCount() > c ? c : c-1);
                this->m_changed = true;
                break;
              case LXMENU_PRESMOVEUP:
                if (c > 0) {
                  r->RemoveChild(n);
                  r->InsertChild(n, p);
                  this->UpdateList();
                  this->SelectScene(c-1);
                  this->m_changed = true;
                }
                break;
              case LXMENU_PRESMOVEDOWN:
                if ((c+1) < this->m_posLBox->GetItemCount()) {
                  p = n->GetNext();
                  r->RemoveChild(n);
                  r->InsertChildAfter(n, p);
                  this->UpdateList();
                  this->SelectScene(c+1);
                  this->m_changed = true;
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
      this->SelectScene(0);
      break;

    case lxPR_EXPORT:
      this->ExportPresentation();
      break;

    case lxPR_OPTIONS:
      this->EditOptions();
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
                : wxMiniFrame(parent, wxID_ANY, _("Animation"),wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER) & (~(wxMINIMIZE_BOX | wxMAXIMIZE_BOX)))
{
  this->m_toolBoxPosition.Init(this, parent, 0, 8, 8);

#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

  this->m_mainFrame = dynamic_cast<lxFrame*>(parent);
  this->m_fileName = wxEmptyString;

   
  wxBoxSizer * sizerFrame = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * sizerTop = new wxBoxSizer(wxHORIZONTAL);

  lxPanel = new wxPanel(this, wxID_ANY);
  this->m_posLBox = new wxListCtrl(lxPanel, lxPR_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
  this->m_posLBox->InsertColumn(0, _("View"));
  this->m_posLBox->InsertColumn(1, _("Time"));
  this->m_posLBox->SetColumnWidth(0, 120);
  this->m_posLBox->SetColumnWidth(1, 80);

  wxBoxSizer * controlSizer = new wxBoxSizer(wxVERTICAL);
  this->m_playButton = new wxButton(lxPanel, lxPR_PLAY, _("Play animation"));
  controlSizer->Add(
    this->m_playButton,
    0, wxEXPAND | wxALL);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMARK, _("Mark")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
    new wxButton(lxPanel, LXMENU_PRESEDIT, _("Edit...")),
    0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESUPDATE, _("Update")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMOVEUP, _("Move up")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESMOVEDOWN, _("Move down")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESDELETE, _("Delete")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add( \
			new wxStaticLine(lxPanel, wxID_ANY), \
	    0, wxBOTTOM | wxTOP | wxEXPAND, lxBORDER);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESNEW, _("New")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESOPEN, _("Open...")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESSAVE, _("Save")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
		new wxButton(lxPanel, LXMENU_PRESSAVEAS, _("Save as...")), 
		0, wxEXPAND | lxNOTTOP);
  controlSizer->Add(
    new wxButton(lxPanel, lxPR_EXPORT, _("Export...")),
    0, wxEXPAND | lxNOTTOP);


  sizerTop->Add(m_posLBox, 1, wxTOP | wxBOTTOM | wxLEFT | wxEXPAND, lxBORDER);
  lxBoxSizer = new wxBoxSizer(wxVERTICAL);
  lxBoxSizer->Add(controlSizer, 1, wxEXPAND, lxBORDER);
  lxBoxSizer->Add(
    new wxButton(lxPanel, lxPR_OPTIONS, _("Options...")),
    0, wxEXPAND);
  lxBoxSizer->Add(
    new wxStaticLine(lxPanel, wxID_ANY),
    0, wxBOTTOM | wxTOP | wxEXPAND, lxBORDER);
  lxBoxSizer->Add(
		new wxButton(lxPanel, wxID_CLOSE, _("Close")), 
		0, wxEXPAND);
  sizerTop->Add(lxBoxSizer, 
		0, wxALL | wxEXPAND, lxBORDER);

  lxPanel->SetSizer(sizerTop);
  sizerTop->Fit(lxPanel);

  sizerFrame->Add(lxPanel, 1, wxEXPAND | wxALL);

  this->SetSizer(sizerFrame);
  lxSetUniformButtonWidths(sizerFrame);
  sizerFrame->SetSizeHints(this);
  sizerFrame->Fit(this);

  wxSize mfs = this->m_mainFrame->GetSize();
  this->SetSize(mfs.GetWidth() / 4, mfs.GetHeight() / 2);

  this->UpdateControls();

}
