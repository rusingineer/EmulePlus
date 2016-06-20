//--- xrmb:partTrafficPrefs ---

#pragma once

#include "preferences.h"
#include "afxwin.h"

// CPPgPartTraffic dialog

class CPPgPartTraffic : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgPartTraffic)

public:
	CPPgPartTraffic();
	virtual ~CPPgPartTraffic();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs;}

// Dialog Data
	enum { IDD = IDD_PPG_PARTTRAFFIC };
protected:
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	CPreferences *m_pPrefs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void LoadSettings(void);
	void Localize(void);
	
	virtual BOOL OnApply();
	afx_msg void OnBnClickedPtUseit();
	afx_msg void OnSettingsChange()			{ SetModified(); }
	afx_msg void OnBnClickedUpbarst1()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarst2()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarst3()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarst4()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarcl1()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarcl2()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarcl3()		{ SetModified(); }
	afx_msg void OnBnClickedUpbarcl4()		{ SetModified(); }
protected:
	int uploadBarStyle;
	int uploadBarColor;
	BOOL displayPartTraffic;
private:
	CButton displayPartTrafficCheck;
	CButton upBarSt1Radio;
	CButton upBarSt2Radio;
	CButton upBarSt3Radio;	
	CButton upBarSt4Radio;
	CButton upBarCl1Radio;
	CButton upBarCl2Radio;
	CButton upBarCl3Radio;
	CButton upBarCl4Radio;
	CString upBarStDesc;
	CString upBarClDesc;
};
