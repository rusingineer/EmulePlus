#pragma once

#include "preferences.h"
#include "afxwin.h"

// CPPgSMTP dialog

class CPPgSMTP : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgSMTP)

public:
	CPPgSMTP();
	virtual ~CPPgSMTP();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	enum { IDD = IDD_PPG_SMTP };
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	void Localize(void);
	afx_msg void OnDataChange()		{ SetModified(); }
	afx_msg void OnEnChangeSMTPAuthenticated();

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

protected:
	CString	m_strSMTPServer;
	CString m_strSMTPName;
	CString m_strSMTPFrom;
	CString m_strSMTPTo;
	CString m_strSMTPUserName;
	CString m_strSMTPPassword;
	BOOL m_bSMTPAuthenticated;
	BOOL m_bSMTPInfoEnabled;
	BOOL m_bSMTPWarningEnabled;
	BOOL m_bSMTPMsgInSubjEnabled;

private:
	CEdit SMTPServerEdit;
	CEdit SMTPNameEdit;
	CEdit SMTPFromEdit;
	CEdit SMTPToEdit;
	CEdit SMTPUserNameEdit;
	CEdit SMTPPasswordEdit;
	CButton SMTPAuthenticatedCheck;
	CButton SMTPInfoEnabledCheck;
	CButton SMTPWarningEnabledCheck;
	CButton	SMTPMsgInSubjectCheck;

	CString m_strSMTPServerDesc;
	CString m_strSMTPNameDesc;
	CString m_strSMTPFromDesc;
	CString m_strSMTPToDesc;
	CString m_strSMTPUserNameDesc;
	CString m_strSMTPPasswordDesc;
	CString m_strSMTPAuthenticationFrame;
};
