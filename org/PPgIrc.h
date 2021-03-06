#pragma once

// CPPgIRC dialog

#include "preferences.h"
#include "afxwin.h"

class CPPgIRC : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgIRC)

public:
	CPPgIRC();
	virtual ~CPPgIRC();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_IRC };
protected:
	CPreferences *m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	void LoadSettings(void);
	bool m_bnickModified;
public:
	virtual BOOL OnApply();
	afx_msg void OnEnChangeServer()				{ SetModified(); }
	afx_msg void OnEnChangeNick()				{ SetModified(); m_bnickModified = true;}
	afx_msg void OnBnClickedAddTimeStamp()			{ SetModified(); }
	afx_msg void OnBnClickedUseFilter();
	afx_msg void OnBnClickedUsePerform();
	afx_msg void OnBnClickedListOnConnect()			{ SetModified(); }
	afx_msg void OnEnChangeName()				{ SetModified(); }
	afx_msg void OnEnChangeUser()				{ SetModified(); }
	afx_msg void OnEnChangePerformString()			{ SetModified(); }	
	afx_msg void OnBnClickedIgnoreInfoMessages()		{ SetModified(); }
	afx_msg void OnBnClickedIgnoreEmuleProtoInfoMessages()	{ SetModified(); }
	void Localize(void);
protected:
	CString server;
	CString nick;
	CString name;
	CString minUser;
	BOOL useChannelFilter;
	CString perform;
	BOOL usePerform;
	BOOL timeStamp;
	BOOL listOnConnect;
	BOOL ignoreInfoMessages;
	BOOL ignoreEmuleMessages;
private:
	CEdit serverEdit;
	CEdit nickEdit;
	CEdit nameEdit;
	CEdit minUserEdit;
	CEdit performEdit;
	CString serverDesc;
	CString nickDesc;
	CString filterDesc;
	CString performDesc;
	CString miscDesc;
	CButton timeStampCheck;
	CString nameDesc;
	CString minUserDesc;
	CButton useChannelFilterCheck;
	CButton usePerformCheck;
	CButton listOnConnectCheck;
	CButton ignoreInfoMessageCheck;
	CButton ignoreEmuleMessagesCheck;
};
