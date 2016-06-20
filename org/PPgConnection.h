#pragma once

#include "Preferences.h"
#include "afxwin.h"

// CPPgConnection dialog

class CPPgConnection : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgConnection)

public:
	CPPgConnection();
	virtual ~CPPgConnection();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	// Dialog Data
	enum { IDD = IDD_PPG_CONNECTION };
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
	void EnableEditBoxes(BOOL bEnable = TRUE);
public:
	afx_msg void OnCbnSelchangeConType();
	afx_msg void OnSettingsChange()				{ SetModified(); }
	afx_msg void OnEnChangeUDPDisable();
	afx_msg void OnQueueChange();
	afx_msg void OnSpeedChange();
	afx_msg void OnBnClickedLimitless();
	void Localize(void);
protected:
	CComboBox m_conprof;
	BOOL showOverhead;
	BOOL lancastEnable;
	BOOL openportsEnable;
	BOOL limitlessDownload;
	CString queueSize;
	CString maxCon;
	CString maxCon5;
	CString uploadCap;
	CString maxUp;
	CString downloadCap;
	CString maxDown;
	CString port;
	BOOL udpDisable;
	CString udpPort;
private:
	CEdit tcpPortEdit;
	CEdit udpPortEdit;
	CEdit downloadCapEdit;
	CEdit uploadCapEdit;
	CEdit maxDownEdit;
	CEdit maxUpEdit;
	CEdit maxConEdit;
	CString conProfDesc;
	CString trFrmDesc;
	CString capacitiesDesc;
	CString kbS1Desc;
	CString kbS2Desc;
	CString limitsDesc;
	CButton lancastEnableCheck;
	CButton openportsEnableCheck;
	CButton showOverheadCheck;
	CButton udpDisableCheck;
	CString maxCon5Desc;
	CString dCapDesc;
	CString uCapDesc;
	CButton limitlessDownloadCheck;
	CString queueFrmDesc;
	CString queueSizeDesc;
	CString clientPortFrmDesc;
	CString maxConnFrmDesc;
};
