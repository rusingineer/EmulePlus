#pragma once

#include "preferences.h"
#include "Loggable.h"
#include "afxwin.h"

// CPPgLogs dialog

class CPPgLogs : public CPropertyPage, public CLoggable
{
	DECLARE_DYNAMIC(CPPgLogs)

public:
	CPPgLogs();
	virtual ~CPPgLogs();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	void Localize(void);

// Dialog Data
	enum { IDD = IDD_PPG_LOGS };
protected:
	CPreferences *m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	static CBarShader s_preview;
	DECLARE_MESSAGE_MAP()
private:
	void LoadSettings(void);
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnSettingsChange()						{SetModified();}
	afx_msg void OnChangeDebugLogging();
protected:
	BOOL writeLogToFile;
	BOOL debugLog;
	BOOL cmNotLog;
	BOOL uploadLog;
	BOOL downloadLog;
	BOOL autoSourcesLogEnabled;
	BOOL clientTransferLogEnabled;
private:
	CButton writeLogToFileCheck;
	CButton debugLogCheck;
	CButton cmNotLogCheck;
	CButton uploadLogCheck;
	CButton downloadLogCheck;
	CButton autoSourcesLogEnabledCheck;
	CButton clientTransferLogEnabledCheck;
};
