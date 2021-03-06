#pragma once

#include "preferences.h"
#include "types.h"
#include "afxwin.h"

// CPPgAdvanced dialog

class CPPgAdvanced : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgAdvanced)

public:
	CPPgAdvanced();
	virtual ~CPPgAdvanced();
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

// Dialog Data
	enum { IDD = IDD_PPG_ADVANCED };

	afx_msg void OnSettingsChange()					{SetModified();}
	afx_msg void OnBnClickedComEnabled();
	afx_msg void OnBnClickedSLSEnabled();
	afx_msg void OnSetCleanupFilter();
	afx_msg void OnBnClickedBanEnabled();
	void Localize(void);

protected:
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	CPreferences *m_pPrefs;
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	void LoadSettings(void);

protected:
	CString banTimes;
	CString minRequestTime;
	CString banTimeInMins;
	CString comString;
	CString slsMaxSources;
	CString outdated;
	bool m_bModified;
	BOOL banMessage;
	BOOL banEnabled;
	BOOL noBanEnabled;
	BOOL comEnabled;
	BOOL slsEnabled;
	BOOL fnCleanup;
	BOOL fnCleanUpTag;
	BOOL counterMeasures;
	BOOL newAuto;

private:
	CEdit banTimesEdit;
	CEdit minRequestTimeEdit;
	CButton banMessageCheck;
	CEdit banTimeInMinsEdit;
	CEdit slsMaxSourcesEdit;
	CEdit outdatedEdit;
	CEdit comStringEdit;
	CButton noBanEnabledCheck;
	CString banDesc;
	CButton banEnabledCheck;
	CString clientRequestDesc;
	CString banForDesc;
	CString timesInDesc;
	CString minutesBan1Desc;
	CString minutesBan2Desc;
	CString comDesc;
	CButton comEnabledCheck;
	CString saveLoadSourcesDesc;
	CString slsMaxSourcesDesc;
	CString slsOutdatedDesc;
	CString slsSourcesDesc;
	CString slsDaysDesc;
	CButton slsEnabledCheck;
	CButton counterMeasuresCheck;
	CButton newAutoCheck;
	CString fnCleanUpDesc;
	CButton fncButton;
	CButton fnCleanUpCheck;
	CButton fnCleanUpTagCheck;
};
