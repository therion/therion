#ifndef lxWX_h
#define lxWX_h

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/gbsizer.h>
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#endif  
//LXDEPCHECK - standard libraries


#ifdef __WXGTK__
#define lxBORDER 3
#else
#define lxBORDER 5
#endif

#define lxFStaticText(ID) (dynamic_cast<wxStaticText*>(FindWindow(ID)))
#define lxFRadioBtn(ID) (dynamic_cast<wxRadioButton*>(FindWindow(ID)))
#define lxFCheckBox(ID) (dynamic_cast<wxCheckBox*>(FindWindow(ID)))
#define lxFButton(ID) (dynamic_cast<wxButton*>(FindWindow(ID)))
#define lxFTextCtrl(ID) (dynamic_cast<wxTextCtrl*>(FindWindow(ID)))
#define lxFSpinCtrl(ID) (dynamic_cast<wxSpinCtrl*>(FindWindow(ID)))
#define lxFSlider(ID) (dynamic_cast<wxSlider*>(FindWindow(ID)))
#define lxFChoice(ID) (dynamic_cast<wxChoice*>(FindWindow(ID)))

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

  wxObject *Clone() const override { return new lxDoubleValidator(*this); }

	bool Validate(wxWindow *parent) override;

  bool TransferToWindow() override;

  bool TransferFromWindow() override;

protected:

	double * m_variable;
	double m_vMin, m_vMax;
    wxString m_fmt;

	bool CheckValidator()	const
	{
		wxCHECK_MSG( m_validatorWindow,	FALSE,
			_T("No	window associated	with validator") );
		wxCHECK_MSG( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)), FALSE,
			_T("wxNumberValidator is	only for wxTextCtrl's")	);
		wxCHECK_MSG( dynamic_cast<wxTextCtrl*>(m_validatorWindow)->IsSingleLine(),	FALSE,
			_T("Multiline wxTextCtrl	not	allowed	yet")	);

		return TRUE;
	}

};



class lxRadioBtnValidator : public wxValidator {

public:

	lxRadioBtnValidator(long * val, const long optval);

  wxObject *Clone() const override { return new lxRadioBtnValidator(*this); }

	bool Validate(wxWindow *parent) override;

  bool TransferToWindow() override;

  bool TransferFromWindow() override;

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
