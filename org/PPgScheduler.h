#pragma once

#include "preferences.h"
#include "afxwin.h"

// CPPgScheduler dialog

class CPPgScheduler : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgScheduler)

public:
	CPPgScheduler();
	virtual ~CPPgScheduler();
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

// Dialog Data
	enum { IDD = IDD_PPG_SCHEDULER };

protected:
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	CPreferences *m_pPrefs;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	void LoadSettings(void);
public:
	afx_msg void OnSettingsChange()			{SetModified();}
	afx_msg void OnBnClickedSCHEnabled();
	afx_msg void OnCbnSelchangeShift1Time()		{SetModified();}
	afx_msg void OnCbnSelchangeShift2Time()		{SetModified();}
	void Localize(void);
protected:
	BOOL schedulerEnabled;
	BOOL exceptMonday;
	BOOL exceptTuesday;
	BOOL exceptWednesday;
	BOOL exceptThursday;
	BOOL exceptFriday;
	BOOL exceptSaturday;
	BOOL exceptSunday;
	CString shift1Down;
	CString shift1Up;
	CString shift1Conn;
	CString shift1Conn5Sec;
	CString shift2Up;
	CString shift2Down;
	CString shift2Conn;
	CString shift2Conn5Sec;
private:
	CComboBox shift1TimeCombo;
	CComboBox shift2TimeCombo;
	CButton exceptMondayCheck;
	CButton exceptTuesdayCheck;
	CButton exceptWednesdayCheck;
	CButton exceptThursdayCheck;
	CButton exceptFridayCheck;
	CButton exceptSaturdayCheck;
	CButton exceptSundayCheck;
	CButton schedulerEnabledCheck;
	CEdit shift1DownEdit;
	CEdit shift1UpEdit;
	CEdit shift1ConnEdit;
	CEdit shift1Conn5SecEdit;
	CEdit shift2UpEdit;
	CEdit shift2DownEdit;
	CEdit shift2ConnEdit;
	CEdit shift2Conn5SecEdit;
	CString schift1Desc;
	CString schift2Desc;
	CString kb1Desc;
	CString kb2Desc;
	CString timeDesc;
	CString downDesc;
	CString upDesc;
	CString connDesc;
	CString conn5Desc;
	CString frmDesc;
};	
