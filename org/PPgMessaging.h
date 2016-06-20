#pragma once
#include "afxwin.h"


// CPPgMessaging dialog

class CPPgMessaging : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgMessaging)

public:
	CPPgMessaging();
	virtual ~CPPgMessaging();
	void SetPrefs(CPreferences* in_prefs) { m_pPrefs = in_prefs; }

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	void Localize(void);

// Dialog Data
	enum { IDD = IDD_PPG_MESSAGING };

protected:
	CPreferences* m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
private:
	void LoadSettings(void);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRbImFromallusers()     	{ SetModified(); }
	afx_msg void OnBnClickedRbImOnlyfromfriends()  	{ SetModified(); }
	afx_msg void OnBnClickedRbImOnlyfromfriends2() 	{ SetModified(); }
	afx_msg void OnBnClickedRbImFromnone()  		{ SetModified(); }
	afx_msg void OnBnClickedCbImPutmeaway();
	afx_msg void OnEnChangeEditImAwaymessage() 		{ SetModified(); }
	afx_msg void OnEnChangeFilter() 				{ SetModified(); }
	afx_msg void OnEnChangeCommentsFilter() 		{ SetModified(); }
protected:
	int acceptMessages;
	BOOL putMeInAwayState;
	CString awayMessage;
	CString filter;
	CString commentsFilter;
private:
	CEdit awayMessageEdit;
	CString acceptMessagesDesc;
	CString otherDesc;
	CButton fromAllUsersRadio;
	CButton onlyFromFriendsRadio;
	CButton otherToLogRadio;
	CButton fromNoneRadio;
	CString commentsDesc;
	CString msgDesc;
	CString commentsFilterDesc;
	CString filterDesc;
	CButton putMeAwayCheck;
	CString awayMessageDesc;
};
