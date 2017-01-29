/**
* @file lxRender.cxx
* Loch printing implementation.
*/


// Standard libraries
#ifndef LXDEPCHECK
#include <stdio.h>
#include <stdlib.h>
#include <wx/spinctrl.h>
#include <wx/radiobox.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/progdlg.h>
#include <wx/valgen.h>

#ifdef __cplusplus
 extern "C" {  // stupid JPEG library
#endif
#include <vtk_png.h>
#ifdef __cplusplus
 }
#endif


#endif  
//LXDEPCHECK - standard libraries


#include "lxRender.h"
#include "lxWX.h"
#include "lxGLC.h"
#include "lxGUI.h"
#include "lxSetup.h"
#include "lxTR.h"

#define lxRENDERBORDER this->m_glc->TRCGet(TR_TILE_BORDER)

//#ifdef LXLINUX
//#define lxPPMFORMAT 1
//#endif

lxRenderData::lxRenderData() 
{
  this->m_scale = 2000.0;
  this->m_scaleMode = LXRENDER_FIT_SCREEN;
  this->m_imgWhiteBg = true;
  this->m_imgWidth = 170.0;
  this->m_imgHeight = 257.0;
  this->m_imgResolution = 300.0;
  this->m_imgFileType = 0;
  this->m_askFName = true;

#ifdef lxPPMFORMAT
  this->m_imgFileName = _T("cave.ppm");
#else
  this->m_imgFileName = _T("cave.bmp");
#endif

}


#define pngerr(err) printf(err)

void lxRenderData::CalcImageDimensions(lxGLCanvas * glc)
{
  switch (this->m_scaleMode) {

    case LXRENDER_FIT_SCALE:
      this->m_imgPixH = 2.0 * glc->setup->cam_dist * glc->setup->cam_lens_vfovr / this->m_scale / 0.0254 * this->m_imgResolution;
      this->m_imgPixW = glc->setup->cam_width * this->m_imgPixH;
      break;

    case LXRENDER_FIT_WIDTH:
      this->m_imgPixW = (this->m_imgWidth / 25.4 * this->m_imgResolution);
      this->m_imgPixH = this->m_imgPixW / glc->setup->cam_width;
      break;

    case LXRENDER_FIT_HEIGHT:
      this->m_imgPixH = (this->m_imgHeight / 25.4 * this->m_imgResolution);
      this->m_imgPixW = this->m_imgPixH * glc->setup->cam_width;
      break;

      // screen shot
    default:
      this->m_imgPixH = glc->wh;
      this->m_imgPixW = glc->ww;
      break;
  }

  if (glc->setup->cam_width <= 1.0) {
    if (this->m_imgPixH < 32.0) {
      this->m_imgPixH = 32.0;
      this->m_imgPixW = this->m_imgPixH * glc->setup->cam_width;
    }
    if (this->m_imgPixH > 32768.0){
      this->m_imgPixH = 32768.0;
      this->m_imgPixW = this->m_imgPixH * glc->setup->cam_width;
    }
  } else {
    if (this->m_imgPixW < 32.0) {
      this->m_imgPixW = 32.0;
      this->m_imgPixH = this->m_imgPixW / glc->setup->cam_width;
    }
    if (this->m_imgPixW > 32768.0){
      this->m_imgPixW = 32768.0;
      this->m_imgPixH = this->m_imgPixW / glc->setup->cam_width;
    }
  }

  switch (this->m_scaleMode) {

    case LXRENDER_FIT_SCALE:
      this->m_imgHeight = this->m_imgPixH / this->m_imgResolution * 25.4;
      this->m_imgWidth = this->m_imgPixW / this->m_imgResolution * 25.4;
      break;

    case LXRENDER_FIT_WIDTH:
      this->m_imgHeight = this->m_imgPixH / this->m_imgResolution * 25.4;
      this->m_scale = 2.0 * glc->setup->cam_dist * glc->setup->cam_lens_vfovr / this->m_imgPixH / 0.0254 * this->m_imgResolution;
      break;

    case LXRENDER_FIT_HEIGHT:
      this->m_imgWidth = this->m_imgPixW / this->m_imgResolution * 25.4;
      this->m_scale = 2.0 * glc->setup->cam_dist * glc->setup->cam_lens_vfovr / this->m_imgPixH / 0.0254 * this->m_imgResolution;
      break;

  }

}






enum {
  LXRDC_START = 6000,
  LXRDC_FIT_SCREEN,
  LXRDC_FIT_WIDTH,
  LXRDC_FIT_HEIGHT,
  LXRDC_FIT_SCALE,
  LXRDC_WIDTH,
  LXRDC_HEIGHT,
  LXRDC_SCALE,
  LXRDC_RESOL,
  LXRDC_RESOL_LBL,
  LXRDC_IMGSIZE,
  LXRDC_IMGSIZE_LBL,
  LXRDC_WHITEBG,
  LXRDC_END,
};


class lxRenderDataConfig : public wxDialog {

public:

  lxRenderDataConfig(wxWindow * parent, lxRenderData * data, lxGLCanvas * glc);
  lxRenderData * m_data;
  lxCRC32 m_dataCRC;
  lxGLCanvas * m_glc;

  void OnUpdateUI(wxUpdateUIEvent & event);
  void OnKey(wxKeyEvent & event);

  DECLARE_EVENT_TABLE()

};


BEGIN_EVENT_TABLE(lxRenderDataConfig, wxDialog)
EVT_UPDATE_UI_RANGE(LXRDC_START, LXRDC_END, lxRenderDataConfig::OnUpdateUI)
END_EVENT_TABLE()


void lxRenderDataConfig::OnUpdateUI(wxUpdateUIEvent & WXUNUSED(event))
{
  lxFTextCtrl(LXRDC_WIDTH)->Enable(lxFRadioBtn(LXRDC_FIT_WIDTH)->GetValue());
  lxFTextCtrl(LXRDC_HEIGHT)->Enable(lxFRadioBtn(LXRDC_FIT_HEIGHT)->GetValue());
  lxFTextCtrl(LXRDC_SCALE)->Enable(lxFRadioBtn(LXRDC_FIT_SCALE)->GetValue());
  lxFTextCtrl(LXRDC_RESOL)->Enable(!lxFRadioBtn(LXRDC_FIT_SCREEN)->GetValue());
  lxFStaticText(LXRDC_RESOL_LBL)->Enable(!lxFRadioBtn(LXRDC_FIT_SCREEN)->GetValue());
  lxFCheckBox(LXRDC_WHITEBG)->Enable(!lxFRadioBtn(LXRDC_FIT_SCREEN)->GetValue());
  lxRenderData rd;
  lxFTextCtrl(LXRDC_WIDTH)->GetValue().ToDouble(&rd.m_imgWidth);
  lxFTextCtrl(LXRDC_HEIGHT)->GetValue().ToDouble(&rd.m_imgHeight);
  lxFTextCtrl(LXRDC_SCALE)->GetValue().ToDouble(&rd.m_scale);
  lxFTextCtrl(LXRDC_RESOL)->GetValue().ToDouble(&rd.m_imgResolution);
  if (lxFRadioBtn(LXRDC_FIT_SCALE)->GetValue()) rd.m_scaleMode = LXRENDER_FIT_SCALE;
  if (lxFRadioBtn(LXRDC_FIT_SCREEN)->GetValue()) rd.m_scaleMode = LXRENDER_FIT_SCREEN;
  if (lxFRadioBtn(LXRDC_FIT_WIDTH)->GetValue()) rd.m_scaleMode = LXRENDER_FIT_WIDTH;
  if (lxFRadioBtn(LXRDC_FIT_HEIGHT)->GetValue()) rd.m_scaleMode = LXRENDER_FIT_HEIGHT;

#define lxRenderDataConfig_check \
  this->m_dataCRC.Initialize(); \
  this->m_dataCRC.AddInteger(rd.m_scaleMode); \
  this->m_dataCRC.AddString(lxFTextCtrl(LXRDC_WIDTH)->GetValue()); \
  this->m_dataCRC.AddString(lxFTextCtrl(LXRDC_HEIGHT)->GetValue()); \
  this->m_dataCRC.AddString(lxFTextCtrl(LXRDC_SCALE)->GetValue()); \
  this->m_dataCRC.AddString(lxFTextCtrl(LXRDC_RESOL)->GetValue());
  lxRenderDataConfig_check
  rd.CalcImageDimensions(this->m_glc);

  if (this->m_dataCRC.Changed())	{

    double imgsize = 0.000003 * rd.m_imgPixH * rd.m_imgPixW;
    if (imgsize < 100.0)
      lxFStaticText(LXRDC_IMGSIZE)->SetLabel(wxString::Format(_T("%.0f x %.0f (%.1f MB)"), rd.m_imgPixW, rd.m_imgPixH, imgsize));
    else
      lxFStaticText(LXRDC_IMGSIZE)->SetLabel(wxString::Format(_T("%.0f x %.0f (%.1f GB)"), rd.m_imgPixW, rd.m_imgPixH, 0.001 * imgsize));

    if (rd.m_scaleMode != LXRENDER_FIT_SCREEN) {
      if (rd.m_scaleMode != LXRENDER_FIT_WIDTH) {
        lxFTextCtrl(LXRDC_WIDTH)->SetValue(wxString::Format(_T("%.0f"), rd.m_imgWidth));
      }
      if (rd.m_scaleMode != LXRENDER_FIT_HEIGHT) {
        lxFTextCtrl(LXRDC_HEIGHT)->SetValue(wxString::Format(_T("%.0f"), rd.m_imgHeight));
      }
      if (rd.m_scaleMode != LXRENDER_FIT_SCALE) {
        lxFTextCtrl(LXRDC_SCALE)->SetValue(wxString::Format(_T("%.0f"), rd.m_scale));
      }
    }

    lxRenderDataConfig_check
    this->m_dataCRC.Changed();

  }

}


lxRenderDataConfig::lxRenderDataConfig(wxWindow * parent, lxRenderData * data, class lxGLCanvas * glc) : wxDialog(parent, wxID_ANY, wxString(_("Rendering setup")))
{

  m_data = data;
  m_glc = glc;

  // TODO: dat to lxWX
  wxBoxSizer * sizerAll = new wxBoxSizer(wxVERTICAL);

  lxStaticBoxSizer = new wxStaticBoxSizer(
    new wxStaticBox(this, wxID_ANY, _("Size and scaling")),
    wxVERTICAL);

  lxStaticBoxSizer->Add(
    new wxRadioButton(this, LXRDC_FIT_SCREEN, _("Screen shot"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, lxRadioBtnValidator(&this->m_data->m_scaleMode, LXRENDER_FIT_SCREEN)),
    0, wxALIGN_LEFT | wxALL, lxBORDER);

  lxFlexGridSizer = new wxFlexGridSizer(3);

  lxFlexGridSizer->Add(
    new wxRadioButton(this, LXRDC_FIT_WIDTH, _("Image width"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_data->m_scaleMode, LXRENDER_FIT_WIDTH)),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | lxNOTTOP, lxBORDER);
  lxFlexGridSizer->Add(
    new wxStaticText(this, wxID_ANY, _("(mm)")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT | wxALIGN_RIGHT, lxBORDER);
  lxFlexGridSizer->Add(
    new wxTextCtrl(this, LXRDC_WIDTH, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, lxDoubleValidator(&this->m_data->m_imgWidth,0.005,5000.0)),
    0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT, lxBORDER);
  lxSETWWIDTH(LXRDC_WIDTH,  2);

  lxFlexGridSizer->Add(
    new wxRadioButton(this, LXRDC_FIT_HEIGHT, _("Image height"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_data->m_scaleMode, LXRENDER_FIT_HEIGHT)),
    0, wxALIGN_LEFT | lxNOTTOP | wxALIGN_CENTER_VERTICAL, lxBORDER);
  lxFlexGridSizer->Add(
    new wxStaticText(this, wxID_ANY, _("(mm)")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT | wxALIGN_RIGHT, lxBORDER);
  lxFlexGridSizer->Add(
    new wxTextCtrl(this, LXRDC_HEIGHT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, lxDoubleValidator(&this->m_data->m_imgHeight,0.005,5000.0)),
    0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT, lxBORDER);
  lxSETWWIDTH(LXRDC_HEIGHT,  2);

  lxFlexGridSizer->Add(
    new wxRadioButton(this, LXRDC_FIT_SCALE, _("Scale"), wxDefaultPosition, wxDefaultSize, 0, lxRadioBtnValidator(&this->m_data->m_scaleMode, LXRENDER_FIT_SCALE)),
    0, wxALIGN_LEFT | lxNOTTOP | wxALIGN_CENTER_VERTICAL, lxBORDER);
  lxFlexGridSizer->Add(
    new wxStaticText(this, wxID_ANY, _("1 :")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT | wxALIGN_RIGHT, lxBORDER);
  lxFlexGridSizer->Add(
    new wxTextCtrl(this, LXRDC_SCALE, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, lxDoubleValidator(&this->m_data->m_scale,1,1e10)),
    0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT, lxBORDER);
  lxSETWWIDTH(LXRDC_SCALE,  2);

  lxStaticBoxSizer->Add(lxFlexGridSizer, 0, wxEXPAND);
  sizerAll->Add(lxStaticBoxSizer, 0, wxEXPAND | wxALL, lxBORDER);

  lxStaticBoxSizer = new wxStaticBoxSizer(
    new wxStaticBox(this, wxID_ANY, _("Image options")),
    wxVERTICAL);
  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(
    new wxStaticText(this, LXRDC_RESOL_LBL, _("Rendering resolution (dpi)")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxALL | wxALIGN_RIGHT, lxBORDER);
  lxBoxSizer->Add(
    new wxTextCtrl(this, LXRDC_RESOL, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, lxDoubleValidator(&this->m_data->m_imgResolution,96,2400)),
    0, wxALIGN_CENTER | lxNOTLEFT, lxBORDER);
  lxSETWWIDTH(LXRDC_RESOL,  2);
  lxStaticBoxSizer->Add(lxBoxSizer, 0, wxEXPAND);

  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(
    new wxStaticText(this, LXRDC_IMGSIZE_LBL, _("Image size:")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | lxNOTTOP | wxALIGN_RIGHT, lxBORDER);
  lxBoxSizer->Add(
    new wxStaticText(this, LXRDC_IMGSIZE, _("00 x 00 (00.0 MB)")),
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | wxBOTTOM | wxRIGHT | wxALIGN_LEFT, lxBORDER);
  lxStaticBoxSizer->Add(lxBoxSizer, 0, wxEXPAND);

  lxStaticBoxSizer->Add(
    new wxCheckBox(this, LXRDC_WHITEBG, _("White background"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&this->m_data->m_imgWhiteBg)), 
    0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  | lxNOTTOP | wxALIGN_LEFT, lxBORDER);

  sizerAll->Add(lxStaticBoxSizer, 0, wxEXPAND | lxNOTTOP, lxBORDER);

  // Bottom line
  lxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  lxBoxSizer->Add(new wxButton(this, wxID_OK, _("OK")), 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, lxBORDER);
  lxBoxSizer->Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 0, wxALIGN_CENTER | wxRIGHT | wxBOTTOM, lxBORDER);
  sizerAll->Add(lxBoxSizer, 0, wxALIGN_RIGHT);

  SetSizer(sizerAll);
  sizerAll->SetSizeHints(this);
  sizerAll->Fit(this);
  this->CentreOnParent();
}

void lxRenderData::Configure(wxWindow * parent, class lxGLCanvas * glc) {

  lxRenderDataConfig cdlg(parent, this, glc);
  cdlg.ShowModal();

}



class lxRenderFile {

public:

  bool m_started, m_continue, m_progress;
  lxGLCanvas * m_glc;
  lxRenderData * m_pData;
  FILE * m_file, * m_fileTMP;
  GLint m_imgWidth, m_imgHeight, m_tWidth, m_tHeight;
  GLubyte * m_imgBuffRow;
  GLubyte * m_imgBuffLine;
  int m_imgBuffLineSize;
  int m_cTile;
  wxWindow * m_parent;
  png_structp png_ptr;
  png_infop png_info_ptr;


  void Render();
  void RenderBMPHeader();
  void RenderPNGHeader();
  void RenderPNGFooter();
  void RenderPDFHeader();
  void RenderPDFFooter();
  void ErrorExit(const wxString & err);
  lxRenderFile(wxWindow * p, lxRenderData * pdata, lxGLCanvas * glc);
  ~lxRenderFile();	

};

lxRenderFile::lxRenderFile(wxWindow * p, lxRenderData * pdata, lxGLCanvas * glc)
{
  m_parent = p;
  m_pData = pdata;
  m_glc = glc;
  m_started = false;
  m_file = NULL;
  m_continue = false;
  m_progress = false;

  m_imgBuffLine = NULL;
  m_imgBuffRow = NULL;
}

lxRenderFile::~lxRenderFile()
{
  if (m_file != NULL)
    fclose(m_file);
  delete [] this->m_imgBuffLine;
  delete [] this->m_imgBuffRow;
  this->m_glc->TRCDestroy();
  this->m_glc->m_renderData = NULL;
  this->m_glc->ForceRefresh();
}


void lxRenderFile::Render() {

  m_file = NULL;

  // najprv zrata rozmery oprazka
  double tmp_w, tmp_h;

  // zratame rozmery oprazka
  //if (m_pData->m_scaleMode == LXRENDER_FIT_SCALE) {
  //  tmp_w = (m_pData->m_imgWidth / 25.4 * m_pData->m_imgResolution);
  //  tmp_h = (m_pData->m_imgHeight / 25.4 * m_pData->m_imgResolution);
  //} else {
  //  tmp_h = 2.0 * m_glc->setup->cam_dist * lxVFOVR / m_pData->m_scale / 0.0254 * m_pData->m_imgResolution;
  //  tmp_w = m_glc->setup->cam_width * tmp_h;
  //}

  this->m_pData->CalcImageDimensions(m_glc);
  tmp_h = this->m_pData->m_imgPixH;
  tmp_w = this->m_pData->m_imgPixW;

  m_imgWidth = (GLint) tmp_w;
  m_imgHeight = (GLint) tmp_h;
  m_tWidth = m_glc->ww;
  m_tHeight = m_glc->wh;
  if (this->m_pData->m_scaleMode != LXRENDER_FIT_SCREEN) {
    if (m_tHeight * m_imgWidth > 5000000)
      m_tHeight = 5000000 / m_imgWidth;
    m_tHeight = m_tHeight - (m_tHeight % 8);
    m_tWidth = m_tWidth - (m_tHeight % 8);
  } else {
    m_tHeight = m_tHeight + 8 - (m_tHeight % 8);
    m_tWidth = m_tWidth + 8 - (m_tHeight % 8);
  }

  wxString cdir, cnm;
  wxFileName::SplitPath(m_pData->m_imgFileName, &cdir, &cnm, NULL);
  if (cdir.Length() == 0)
    cdir = this->m_glc->frame->m_fileDir;

  if (m_pData->m_askFName) {
    wxFileDialog fileDlg(this->m_parent, _("Save rendering as"), cdir, cnm, 
#ifdef lxPPMFORMAT
      _("PPM files (*.ppm)|*.ppm"), 
#else
      _("PDF files (*.pdf)|*.pdf|PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp"), 
#endif
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    fileDlg.CentreOnParent();
    fileDlg.SetFilterIndex(m_pData->m_imgFileType);
    if (fileDlg.ShowModal() == wxID_OK) {
      m_pData->m_imgFileName = fileDlg.GetPath();
      m_pData->m_imgFileType = fileDlg.GetFilterIndex();
    } else
      return;
  }

  // otvorime subor
  m_file = fopen(m_pData->m_imgFileName.mb_str(), "w+b");
  if (m_file == NULL) {
    ErrorExit(_("Unable to open file for output."));
    return;
  }

  // start rendering
  m_glc->m_renderData = m_pData;
  // allocate rendering buffers
  this->m_glc->TRCInit(LXGLCTR_TILE, this->m_imgWidth, this->m_imgHeight, this->m_tWidth, this->m_tHeight);
  this->m_imgBuffLineSize = 3 * this->m_imgWidth;

  switch (m_pData->m_imgFileType) {
    case 0:
      RenderPDFHeader();
      trRowOrder(this->m_glc->m_TRC->m_ctx, TR_TOP_TO_BOTTOM);
      break;
    case 1:
      RenderPNGHeader();
      trRowOrder(this->m_glc->m_TRC->m_ctx, TR_TOP_TO_BOTTOM);
      break;
    default:
      RenderBMPHeader();
      trRowOrder(this->m_glc->m_TRC->m_ctx, TR_BOTTOM_TO_TOP);
      break;
  }

  this->m_tWidth = this->m_glc->TRCGet(TR_TILE_WIDTH);
  this->m_tHeight = this->m_glc->TRCGet(TR_TILE_HEIGHT);
  this->m_imgBuffLine = new GLubyte [m_imgBuffLineSize];
  this->m_imgBuffRow = new GLubyte [3 * this->m_imgWidth * this->m_tHeight];

  double tmpcw = m_glc->setup->cam_width;
  m_glc->setup->cam_width = double(m_imgWidth) / double(m_imgHeight);

  m_cTile = 0;
  long max =
    ((this->m_imgWidth + this->m_tWidth - 2 * lxRENDERBORDER - 1) / (this->m_tWidth - 2 * lxRENDERBORDER)) *
    ((this->m_imgHeight + this->m_tHeight - 2 * lxRENDERBORDER - 1) / (this->m_tHeight - 2 * lxRENDERBORDER));
  m_continue = true;
  wxString renmsg = wxString::Format(_("Rendering (%d x %d, %.1f MB) ..."), m_imgWidth, m_imgHeight, double(m_imgWidth) * double(m_imgHeight) * 3 / 1048576.0);
  wxProgressDialog * dialog = NULL;
  if (this->m_pData->m_askFName) {
    dialog = new wxProgressDialog(_("Rendering to bitmap"),
      renmsg,
      max,    // range
      m_parent,   // parent
      wxPD_CAN_ABORT |
      wxPD_APP_MODAL |
      wxPD_AUTO_HIDE |
      wxPD_ELAPSED_TIME |
      wxPD_ESTIMATED_TIME |
      wxPD_REMAINING_TIME);
  }

  int curColumn;

  this->m_glc->SetFontColors();
  this->m_glc->m_fntTitleO->setPointSize(14.0 * this->m_pData->m_imgResolution / 96.0);
  this->m_glc->m_fntNumericO->setPointSize(8.0 * this->m_pData->m_imgResolution / 96.0);
    
  while (m_continue) {

    if (this->m_pData->m_scaleMode == LXRENDER_FIT_SCREEN) {

      this->m_glc->TRCBeginTile();
      this->m_glc->m_sInit = true;
      this->m_glc->m_isO = false;
      this->m_glc->RenderScreen();
      this->m_glc->m_isO = true;
      this->m_glc->m_sInit = true;

    } else {

      if (this->m_cTile == 0) {
        this->m_glc->UpdateRenderContents();
        if (this->m_glc->m_oList == 0) 
          this->m_glc->m_oList = glGenLists(1);
        glNewList(this->m_glc->m_oList, GL_COMPILE);
        this->m_glc->RenderAll();
        glEndList();
      }

      if (this->m_glc->setup->cam_anaglyph) {
        this->m_glc->setup->cam_anaglyph_left = false;
        this->m_glc->SetCamera();
        this->m_glc->TRCBeginTile();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        if (this->m_glc->m_renderData->m_imgWhiteBg)
          glClearColor(1.0, 1.0, 1.0, 1.0);
        else
          glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        this->m_glc->SetColorMask();
#ifdef LXLINUX
        this->m_glc->RenderAll();
#else    
        glCallList(this->m_glc->m_oList);
#endif

        this->m_glc->setup->cam_anaglyph_left = true;
        this->m_glc->SetCamera();
        this->m_glc->TRCBeginTile();
        this->m_glc->SetColorMask();
#ifdef LXLINUX
        this->m_glc->RenderAll();
#else    
        glCallList(this->m_glc->m_oList);
#endif

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

      } else {

        m_glc->SetCamera();
        this->m_glc->TRCBeginTile();
#ifdef LXLINUX
        this->m_glc->RenderAll();
#else    
        glCallList(this->m_glc->m_oList);
#endif

      }

      // render indicators
      if (this->m_glc->setup->m_vis_indicators)
        this->m_glc->RenderInds();

    }

    curColumn = this->m_glc->TRCGet(TR_CURRENT_COLUMN);
    m_continue = this->m_glc->TRCEndTile();

    this->m_cTile++;
    m_continue = m_continue && ((dialog == NULL) || dialog->Update(this->m_cTile, renmsg));

    // save tile do rowbuffera
    {
      int curTileWidth = this->m_glc->TRCGet(TR_CURRENT_TILE_WIDTH);
      int bytesPerImageRow = m_imgWidth * 3 * sizeof(GLubyte);
      int bytesPerTileRow = (m_tWidth - 2 * lxRENDERBORDER) * 3 * sizeof(GLubyte);
      int xOffset = curColumn * bytesPerTileRow;
      int bytesPerCurrentTileRow = (curTileWidth - 2 * lxRENDERBORDER) * 3 * sizeof(GLubyte);
      int i;
      int curTileHeight = this->m_glc->TRCGet(TR_CURRENT_TILE_HEIGHT);

      for (i = 0; i < curTileHeight; i++) {
        memcpy(m_imgBuffRow + i * bytesPerImageRow + xOffset,  // Dest
          this->m_glc->TRCGetBuffer() + i * bytesPerTileRow,   // Src
          bytesPerCurrentTileRow);                             // Byte count
      }
    }

    if (curColumn == (this->m_glc->TRCGet(TR_COLUMNS)-1)) {

      // write this buffered row of tiles to the file
      int curTileHeight = this->m_glc->TRCGet(TR_CURRENT_TILE_HEIGHT);
      int bytesPerImageRow = m_imgWidth * 3 * sizeof(GLubyte);
      int i;
      GLubyte * rowPtr;
      // The arithmetic is a bit tricky here because of borders and
      // the up/down flip.  Thanks to Marcel Lancelle for fixing it.
#ifdef lxPPMFORMAT
      for (i = 2 * lxRENDERBORDER; i < curTileHeight; i++) {
        rowPtr = m_imgBuffRow + (curTileHeight-1-i) * bytesPerImageRow; // (curTileHeight-1-i)
        fwrite(rowPtr, 1, 3 * m_imgWidth, m_file);
      }
#else
      GLubyte * sPtr, * dPtr;
      int j;

      switch (this->m_pData->m_imgFileType) {
//        case 0:
//          for (i = (curTileHeight - 2 * lxRENDERBORDER); i > 0 ; i--) {
//            rowPtr = m_imgBuffRow + (i-1) * bytesPerImageRow;
//            fwrite(rowPtr, 1, m_imgBuffLineSize, m_file);
//          }
//          break;
        case 0:
        case 1:
          for (i = (curTileHeight - 2 * lxRENDERBORDER); i > 0 ; i--) {
            rowPtr = m_imgBuffRow + (i-1) * bytesPerImageRow;
            png_write_row(this->png_ptr, rowPtr);
          }
          break;
        default:
          for (i = 0; i < (curTileHeight - 2 * lxRENDERBORDER); i++) {
            rowPtr = m_imgBuffRow + i * bytesPerImageRow;
            for (j = 0, dPtr = m_imgBuffLine, sPtr = rowPtr; 
              j < m_imgWidth; j++, dPtr += 3, sPtr += 3) {
                dPtr[2] = sPtr[0];
                dPtr[1] = sPtr[1];
                dPtr[0] = sPtr[2];
              }
              fwrite(m_imgBuffLine, 1, m_imgBuffLineSize, m_file);
          }
          break;
      }
#endif
    }    
  } 

  m_glc->setup->cam_width = tmpcw;
  //pngtst;

  delete dialog;

  switch (this->m_pData->m_imgFileType) {
    case 0:
      this->RenderPDFFooter();
      break;
    case 1:
      this->RenderPNGFooter();
      break;
  }

  delete [] this->m_imgBuffLine;
  delete [] this->m_imgBuffRow;
  this->m_imgBuffLine = NULL;
  this->m_imgBuffRow = NULL;

}

void lxRenderFile::ErrorExit(const wxString & err) {
  wxMessageBox(err, _("Error"), wxICON_ERROR | wxOK, m_parent);
}

void lxRenderFile::RenderBMPHeader() {

#ifdef lxPPMFORMAT

  fprintf(this->m_file,"P6\n");
  fprintf(this->m_file,"# ppm-file created by loch\n");
  fprintf(this->m_file,"%d %d\n", this->m_imgWidth, this->m_imgHeight);
  fprintf(this->m_file,"255\n");

#else

  wxUint16 bpp = 24;     // # of bits per pixel
  int palette_size = 0;  // # of color map entries, ie. 2^bpp colors

  unsigned width = this->m_imgWidth;
  unsigned row_padding = (4 - int(width*bpp/8.0) % 4) % 4; // # bytes to pad to dword
  unsigned row_width = int(width * bpp/8.0) + row_padding; // # of bytes per row
  this->m_imgBuffLineSize = row_width;


  struct {
    // BitmapHeader:
    wxUint16  magic;          // format magic, always 'BM'
    wxUint32  filesize;       // total file size, inc. headers
    wxUint32  reserved;       // for future use
    wxUint32  data_offset;    // image data offset in the file
    // BitmapInfoHeader:
    wxUint32  bih_size;       // 2nd part's size
    wxUint32  width, height;  // bitmap's dimensions
    wxUint16  planes;         // num of planes
    wxUint16  bpp;            // bits per pixel
    wxUint32  compression;    // compression method
    wxUint32  size_of_bmp;    // size of the bitmap
    wxUint32  h_res, v_res;   // image resolution in dpi
    wxUint32  num_clrs;       // number of colors used
    wxUint32  num_signif_clrs;// number of significant colors
  } hdr;


  double imgRes;
  if (this->m_pData->m_scaleMode == LXRENDER_FIT_SCREEN)
    imgRes = 96.0;
  else
    imgRes = this->m_pData->m_imgResolution;

  wxUint32 hdr_size = 14/*BitmapHeader*/ + 40/*BitmapInfoHeader*/;

  hdr.magic = wxUINT16_SWAP_ON_BE(0x4D42/*'BM'*/);
  hdr.filesize = wxUINT32_SWAP_ON_BE(hdr_size + palette_size * 4 +
    row_width * this->m_imgHeight);
  hdr.reserved = 0;
  hdr.data_offset = wxUINT32_SWAP_ON_BE(hdr_size + palette_size * 4);
  hdr.bih_size = wxUINT32_SWAP_ON_BE(hdr_size - 14);
  hdr.width = wxUINT32_SWAP_ON_BE(this->m_imgWidth);
  hdr.height = wxUINT32_SWAP_ON_BE(this->m_imgHeight);
  hdr.planes = wxUINT16_SWAP_ON_BE(1); // always 1 plane
  hdr.bpp = wxUINT16_SWAP_ON_BE(bpp);
  hdr.compression = 0; // RGB uncompressed
  hdr.size_of_bmp = wxUINT32_SWAP_ON_BE(row_width * this->m_imgHeight);
  hdr.h_res = hdr.v_res = wxUINT32_SWAP_ON_BE((wxUint32) (imgRes * 100.0 / 2.54 + 0.5));  // 72dpi is standard
  hdr.num_clrs = wxUINT32_SWAP_ON_BE(palette_size); // # colors in colormap
  hdr.num_signif_clrs = 0;     // all colors are significant

#ifdef bmpWrite
#undef bmpWrite
#endif
#define bmpWrite(srcp, srcs) fwrite(srcp, 1, srcs, this->m_file)

  bmpWrite(&hdr.magic, 2);
  bmpWrite(&hdr.filesize, 4);
  bmpWrite(&hdr.reserved, 4);
  bmpWrite(&hdr.data_offset, 4);

  bmpWrite(&hdr.bih_size, 4);
  bmpWrite(&hdr.width, 4);
  bmpWrite(&hdr.height, 4);
  bmpWrite(&hdr.planes, 2);
  bmpWrite(&hdr.bpp, 2);
  bmpWrite(&hdr.compression, 4);
  bmpWrite(&hdr.size_of_bmp, 4);
  bmpWrite(&hdr.h_res, 4);
  bmpWrite(&hdr.v_res, 4);
  bmpWrite(&hdr.num_clrs, 4);
  bmpWrite(&hdr.num_signif_clrs, 4);

#endif

}


void lxRenderData::Render(wxWindow * parent, lxGLCanvas * glc)
{

  lxRenderFile pf(parent, this, glc);
  
  char * prevlocale = setlocale(LC_NUMERIC,NULL);
  setlocale(LC_NUMERIC,"C");
  glc->m_isO = true;
  
  pf.Render();

  glc->m_isO = false;
  setlocale(LC_NUMERIC,prevlocale);
}


void lxRenderFile::RenderPNGHeader()
{

  double imgRes;
  if (this->m_pData->m_scaleMode == LXRENDER_FIT_SCREEN)
    imgRes = 96.0;
  else
    imgRes = this->m_pData->m_imgResolution;


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		pngerr("[write_png_file] png_create_write_struct failed");

	png_info_ptr = png_create_info_struct(png_ptr);

  if (!png_info_ptr)
		pngerr("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during init_io");

	png_init_io(png_ptr, this->m_file);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, png_info_ptr, 
    /*256, 256,*/ this->m_imgWidth, this->m_imgHeight,
		     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_set_pHYs(png_ptr, png_info_ptr,
    (png_uint_32) (imgRes * 39.3700787401574 + 0.5), 
    (png_uint_32) (imgRes * 39.3700787401574 + 0.5), 
    PNG_RESOLUTION_METER);
	png_write_info(png_ptr, png_info_ptr);

  /* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during writing bytes");

}


void lxRenderFile::RenderPNGFooter()
{
 	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during end of write");

 	png_write_end(png_ptr, NULL);

  png_destroy_write_struct(&this->png_ptr, &this->png_info_ptr);

}

size_t pdf_obj[10], pdf_png_start_length, pdf_png_start;

void lxRenderFile::RenderPDFHeader()
{
  double imgRes;

  if (this->m_pData->m_scaleMode == LXRENDER_FIT_SCREEN)
    imgRes = 96.0;
  else
    imgRes = this->m_pData->m_imgResolution;

  double imw, imh;
  imw = 72.0 * this->m_imgWidth / imgRes;
  imh = 72.0 * this->m_imgHeight / imgRes;
  char tmp_buff[256];


  fprintf(this->m_file,"%%PDF-1.4\n");

  pdf_obj[4] = ftell(this->m_file);
  sprintf(tmp_buff, "q\n%.4f 0 0 %.4f 0 0 cm\n/Im1 Do\nQ\n", imw, imh);
  fprintf(this->m_file,"4 0 obj <<\n/Length %u\n>>\nstream\n%sendstream\nendobj\n", (unsigned)strlen(tmp_buff), tmp_buff);

  pdf_obj[3] = ftell(this->m_file);
  fprintf(this->m_file,"3 0 obj <<\n/Type /Page\n/Contents 4 0 R\n/Resources 2 0 R\n/MediaBox [0 0 %.4f %.4f]\n/Parent 5 0 R\n>> endobj\n", imw, imh);

  pdf_obj[1] = ftell(this->m_file);
  fprintf(this->m_file,"1 0 obj <<\n/Type /XObject\n/Subtype /Image\n/Width %u\n"
    "/Height %u\n/BitsPerComponent 8\n/ColorSpace /DeviceRGB\n", this->m_imgWidth, this->m_imgHeight);

  pdf_png_start_length = ftell(this->m_file);
  fprintf(this->m_file,"/Length 00000000000000\n/Filter /FlateDecode\n/DecodeParms << /Colors 3 /Columns %u /BitsPerComponent 8 /Predictor 10 >>\n>>\nstream\n", this->m_imgWidth);

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		pngerr("[write_png_file] png_create_write_struct failed");

	png_info_ptr = png_create_info_struct(png_ptr);

  if (!png_info_ptr)
		pngerr("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during init_io");

  this->m_fileTMP = tmpfile();
	png_init_io(png_ptr, this->m_fileTMP);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		pngerr("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, png_info_ptr, 
    /*256, 256,*/ this->m_imgWidth, this->m_imgHeight,
		     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

 	png_write_info(png_ptr, png_info_ptr);

  pdf_png_start = ftell(this->m_fileTMP);

}



void lxRenderFile::RenderPDFFooter()
{


  size_t src_pos, dst_pos, png_len, png_pos, png_hdr, png_add, buff_read, buff_rest;
  unsigned char lenbuff[8]; 

  png_write_end(png_ptr, NULL);

  fseek(this->m_fileTMP, pdf_png_start, SEEK_SET);

  png_len = 0;
  bool contpng = true;
  do {
    fread(lenbuff, 1, 8, this->m_fileTMP);
    png_add = (size_t) lenbuff[3] + 256 * (size_t) lenbuff[2] + 256 * 256 * (size_t) lenbuff[1] + 256 * 256 * 256 * (size_t) lenbuff[0];
    png_hdr = (size_t) lenbuff[7] + 256 * (size_t) lenbuff[6] + 256 * 256 * (size_t) lenbuff[5] + 256 * 256 * 256 * (size_t) lenbuff[4];
    if (png_hdr == 0x49444154) {
      png_len += png_add;
      if (fseek(this->m_fileTMP, png_add + 4, SEEK_CUR) != 0)
        contpng = false;
    } else {
      contpng = false;
    }
  } while (contpng);

  fseek(this->m_file, pdf_png_start_length, SEEK_SET);
  fprintf(this->m_file,"/Length %u\n/Filter /FlateDecode\n/DecodeParms << /Colors 3 /Columns %u /BitsPerComponent 8 /Predictor 10 >>\n>>\nstream\n", (unsigned) png_len, this->m_imgWidth);

#define buffsize 256000
  unsigned char * buff = new unsigned char [buffsize]; 
  contpng = true;
  dst_pos = ftell(this->m_file);
  src_pos = pdf_png_start;
  do {
    fseek(this->m_fileTMP, src_pos, SEEK_SET);
    src_pos += 8;
    fread(lenbuff, 1, 8, this->m_fileTMP);
    png_add = (size_t) lenbuff[3] + 256 * (size_t) lenbuff[2] + 256 * 256 * (size_t) lenbuff[1] + 256 * 256 * 256 * (size_t) lenbuff[0];
    png_hdr = (size_t) lenbuff[7] + 256 * (size_t) lenbuff[6] + 256 * 256 * (size_t) lenbuff[5] + 256 * 256 * 256 * (size_t) lenbuff[4];
    if (png_hdr == 0x49444154) {
      for(png_pos = 0; png_pos < png_add; png_pos += buff_read) {
        if (fseek(this->m_fileTMP, src_pos, SEEK_SET) != 0)
          contpng = false;
        buff_rest = png_add - png_pos;
        if (buff_rest > buffsize)
          buff_rest = buffsize;
        buff_read = fread(buff, 1, buff_rest, this->m_fileTMP);
        //fseek(this->m_file, dst_pos, SEEK_SET);
        fwrite(buff, 1, buff_read, this->m_file);
        src_pos += buff_read;
        dst_pos += buff_read;
      }
      src_pos += 4;
    } else {
      contpng = false;
    }
  } while (contpng);

  delete [] buff;

  //fseek(this->m_file, dst_pos, SEEK_SET);

  fprintf(this->m_file,"\n");
  fprintf(this->m_file,"endstream\nendobj\n");

  pdf_obj[2] = ftell(this->m_file);
  fprintf(this->m_file,"2 0 obj <<\n/XObject << /Im1 1 0 R >>\n/ProcSet [ /PDF /ImageC ]\n>> endobj\n");

  pdf_obj[5] = ftell(this->m_file);
  fprintf(this->m_file,"5 0 obj <<\n/Type /Pages\n/Count 1\n/Kids [3 0 R]\n>> endobj\n");

  pdf_obj[6] = ftell(this->m_file);
  fprintf(this->m_file,"6 0 obj <<\n/Type /Catalog\n/Pages 5 0 R\n>> endobj\n");

  pdf_obj[7] = ftell(this->m_file);
  fprintf(this->m_file,"7 0 obj <<\n/Producer (Loch)\n/Creator (Therion)\n>> endobj\n");

  pdf_obj[9] = ftell(this->m_file);
  fprintf(this->m_file,"xref\n0 8\n%010u 65535 f\n%010u 00000 n\n%010u 00000 n\n%010u 00000 n\n%010u 00000 n\n%010u 00000 n\n%010u 00000 n\n%010u 00000 n\n",
    0,(unsigned)pdf_obj[1],(unsigned)pdf_obj[2],(unsigned)pdf_obj[3],(unsigned)pdf_obj[4],(unsigned)pdf_obj[5],(unsigned)pdf_obj[6],(unsigned)pdf_obj[7]);
  fprintf(this->m_file,"trailer\n<< /Size 8\n /Root 6 0 R\n/Info 7 0 R >>\nstartxref\n%u\n%%%%EOF\n",(unsigned)pdf_obj[9]);

  png_destroy_write_struct(&this->png_ptr, &this->png_info_ptr);

  fclose(this->m_fileTMP);
}


