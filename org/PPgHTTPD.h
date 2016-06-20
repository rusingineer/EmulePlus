
#pragma once

#include "Preferences.h"
#include "afxwin.h"

// CPPgHTTPD dialog

class CPPgHTTPD : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgHTTPD)

public:
	CPPgHTTPD();
	virtual ~CPPgHTTPD();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	enum { IDD = IDD_PPG_HTTPD };
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	
protected:
	CPreferences *m_pPrefs;
	
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

private:
	void LoadSettings(void);
	
public:
	void Localize(void);
	afx_msg void OnDataChange()		{SetModified();}
	afx_msg void OnEnChangeHTTPDEnabled();
	afx_msg void OnReloadTemplates();
	afx_msg void OnBnClickedTmplbrowse();
	afx_msg void OnEnChangeMMEnabled();
	afx_msg void OnEnChangeHTTPDGuestEnabled();
	afx_msg void OnEnChangeIntruderDetection();
protected:
	CString httpdPort;
	CString httpdPass;
	CString httpdPassLow;
	CString tmplPath;
	CString mmPort;
	CString mmPass;
	BOOL httpdEnabled;
	BOOL httpdEnabledLow;
	BOOL httpdIntruderDetectEnabled;
	BOOL mmEnabled;
	CString tempDisableLogin;
	CString loginAttemptsAllowed;
private:
	CButton tmplBrowseButton;
	CButton intruderDetectionCheck;
	CButton httpdReloadTmplButton;
	CButton httpdEnabledCheck;
	CButton httpdEnabledLowCheck;
	CEdit httpdPortEdit;
	CEdit tmplPathEdit;
	CEdit httpdPassEdit;
	CEdit httpdPassLowEdit;
	CEdit loginAttemptsAllowedEdit;
	CEdit tempDisableLoginEdit;
	CEdit mmPortEdit;
	CEdit mmPassEdit;
	CString adminPassDesc;
	CString templateDesc;
	CString generalDesc;
	CString lowUserDesc;
	CString lowUserPassDesc;
	CString portDesc;
	CString intruderDesc;
	CString intrLoginDesc;
	CString intrTimeDesc;
	CString intrDisableDesc;
	CString mmPortDesc;
	CString mmPassDesc;
	CButton mmEnabledCheck;
};
