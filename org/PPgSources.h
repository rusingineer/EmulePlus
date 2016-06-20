#pragma once

// CPPgSources dialog

#include "preferences.h"
#include "afxcmn.h"
#include "afxwin.h"

class CPPgSources : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgSources)

public:
	CPPgSources();
	virtual ~CPPgSources();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_SOURCES };

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnSettingsChange()				{ SetModified(); }
	afx_msg void OnSourcesChange();
	afx_msg void OnBnClickedAutoSources();
	afx_msg void OnBnClickedDisableXS();
	afx_msg void OnBnClickedDisableXSUpTo();
	void Localize(void);

protected:
	CPreferences *m_pPrefs;	

	//{{AFX_MSG(CSpinToolBar)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void LoadSettings(void);

protected:
	BOOL disableXS;
	BOOL enableXSUpTo;
	BOOL autoSourcesEnabled;
	CString xsUpTo;
	CString maxSourcePerFile;
	uint32 minAutoSourcesPerFile;
	uint32 maxAutoSourcesPerFile;
	uint32 maxAutoSourcesTotal;
	uint32 maxAutoExchangeSources;

private:
	CButton autoSourcesEnabledCheck;
	CButton enableXsUpToCheck;
	CButton disableXsCheck;
	CEdit maxSourcePerFileEdit;
	CEdit minAutoSourcesPerFileEdit;
	CEdit maxAutoSourcesPerFileEdit;
	CEdit maxAutoSourcesTotalEdit;
	CEdit maxAutoExchangeSourcesEdit;
	CEdit xsUpToEdit;
	CString maxSrcDesc;
	CString autosrcLblGrp;
	CString autosrcLblFrom;
	CString autosrcLblTo;
	CString autosrcLblMaxtotal;
	CString autosrcLblSeUpto;
	CString xsSourcesDesc;
};
