#ifndef lxWX_h
#define lxWX_h

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/gbsizer.h>
#endif  
//LXDEPCHECK - standart libraries

#if !wxCHECK_VERSION(2,7,0)
#define wxFD_OVERWRITE_PROMPT wxOVERWRITE_PROMPT
#define wxFD_SAVE wxSAVE
#endif

#ifdef __WXGTK__
#define lxBORDER 3
#else
#define lxBORDER 5
#endif

#define lxFStaticText(ID) ((wxStaticText *)FindWindow(ID))
#define lxFRadioBtn(ID) ((wxRadioButton *)FindWindow(ID))
#define lxFCheckBox(ID) ((wxCheckBox *)FindWindow(ID))
#define lxFButton(ID) ((wxButton *)FindWindow(ID))
#define lxFTextCtrl(ID) ((wxTextCtrl*)FindWindow(ID))
#define lxFSpinCtrl(ID) ((wxSpinCtrl*)FindWindow(ID))
#define lxFSlider(ID) ((wxSlider*)FindWindow(ID))
#define lxFChoice(ID) ((wxChoice*)FindWindow(ID))

#define lxNOTTOP wxBOTTOM | wxLEFT | wxRIGHT
#define lxNOTLEFT wxBOTTOM | wxTOP | wxRIGHT

void lxAdjustBitmapRGB(size_t s, unsigned char * d);

#ifdef LXWIN32
#define lxADJUSTRGB(s, n) lxAdjustBitmapRGB(s, n)
#else
#define lxADJUSTRGB(s, n)
#endif

extern wxStaticBox * lxStaticBox;
extern wxStaticBoxSizer * lxStaticBoxSizer;
extern wxBoxSizer * lxBoxSizer;
extern wxGridSizer * lxGridSizer;
extern wxGridBagSizer * lxGBSizer;
extern wxFlexGridSizer * lxFlexGridSizer;
extern wxRadioButton * lxRadioBtn;
extern wxCheckBox * lxCheckBox;
extern wxPanel * lxPanel;
extern wxSize lxSize;
extern wxPoint lxPoint;

#define lxSETWWIDTH(ID, fact) \
  lxSize = FindWindow(ID)->GetSize(); \
  lxSize.SetWidth((int) (lxSize.GetHeight() * fact)); \
  FindWindow(ID)->SetSizeHints(lxSize, lxSize);

#define lxSETWWH(ID, fact, fact2) \
  lxSize = FindWindow(ID)->GetSize(); \
  lxSize.SetWidth((int) (lxSize.GetHeight() * fact)); \
  lxSize.SetHeight((int) (lxSize.GetHeight() * fact * fact2)); \
  FindWindow(ID)->SetSizeHints(lxSize, lxSize);


class lxTBoxPos {

  int m_xOffset, m_yOffset, m_corner;
  wxWindow * m_winTool, * m_winFrame;

public:

  static int m_fsOffset;
  lxTBoxPos();
  lxTBoxPos(wxWindow * tool, wxWindow * frame, int corner, int xOffset, int yOffset);
  void Init(wxWindow * tool, wxWindow * frame, int corner = 0, int xOffset = 0, int yOffset = 0);
  void Set(int corner, int xOffset, int yOffset);
  void Save();
  void Restore();

};


class lxCRC32 {

public:

	unsigned long m_prevCrc;
	wxString m_crcStr;

	lxCRC32();
	void Initialize();
	void AddInteger(long var);
	void AddReal(double dbl);
	void AddString(wxString sss);
	unsigned long Calculate();
	bool Changed(); 

};




class lxDoubleValidator : public wxValidator {

public:

	lxDoubleValidator(double * val, const double vmin, const double vmax, const wxChar * fmt = _T("%.0f"));

	lxDoubleValidator(const lxDoubleValidator & val);

	~lxDoubleValidator();

  virtual wxObject *Clone() const { return new lxDoubleValidator(*this); }

  bool Copy(const lxDoubleValidator& val);
    
	virtual bool Validate(wxWindow *parent);

  virtual bool TransferToWindow();

  virtual bool TransferFromWindow();

protected:

	double * m_variable;
	double m_vMin, m_vMax;
  const wxChar * m_fmt;

	bool CheckValidator()	const
	{
		wxCHECK_MSG( m_validatorWindow,	FALSE,
			_T("No	window associated	with validator") );
		wxCHECK_MSG( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)), FALSE,
			_T("wxNumberValidator is	only for wxTextCtrl's")	);
		wxCHECK_MSG( ((wxTextCtrl	*)m_validatorWindow)->IsSingleLine(),	FALSE,
			_T("Multiline wxTextCtrl	not	allowed	yet")	);

		return TRUE;
	}

};



class lxRadioBtnValidator : public wxValidator {

public:

	lxRadioBtnValidator(long * val, const long optval);

	lxRadioBtnValidator(const lxRadioBtnValidator & val);

	~lxRadioBtnValidator();

  virtual wxObject *Clone() const { return new lxRadioBtnValidator(*this); }

  bool Copy(const lxRadioBtnValidator& val);
    
	virtual bool Validate(wxWindow *parent);

  virtual bool TransferToWindow();

  virtual bool TransferFromWindow();

protected:

	long * m_variable;
	long m_variableValue;

	bool CheckValidator()	const
	{
		wxCHECK_MSG( m_validatorWindow,	FALSE,
			_T("No	window associated	with validator") );
		wxCHECK_MSG( m_validatorWindow->IsKindOf(CLASSINFO(wxRadioButton)), FALSE,
			_T("wxRadioBtnValidator is only for wxRadioButtons")	);

		return TRUE;
	}

};

#endif
