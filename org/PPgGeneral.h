#pragma once

#include "3dpreviewcontrol.h"

class CPreferences;

class CPPgGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgGeneral)

public:
	CPPgGeneral();
	virtual ~CPPgGeneral();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	void Localize(void);

// Dialog Data
	enum { IDD = IDD_PPG_GENERAL };
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
	void LoadLanguagesCombo(void);
	void DrawPreview();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	afx_msg void OnSettingsChange()						{SetModified();}
	afx_msg void On3DDepth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEditWebservices();
	afx_msg void OnBnClickedEd2kfix();
	afx_msg void OnBnClickedAutoTakeED2KLinks()			{ SetModified(); }
	afx_msg void OnBnClickedFakeUpdate();
protected:		
	CString userNick;
	CString tooltipDelay;
	BOOL beepOnErrors;
	BOOL showSplashscreen;
	BOOL allowMultipleInstances;
	BOOL onlineSignature;
	BOOL doubleClickClientDetails;
	BOOL autoTakeEd2kLinks;
	int mainProcess;
	BOOL updateFakeList;
	CString updateFakeListURL;
	BOOL watchClipboard;
private:
	CSliderCtrl threeDSlider;
	CComboBox languageCombo;
	C3DPreviewControl m_3DPreview;
	CEdit userNickEdit;
	CButton beepOnErrorsCheck;
	CButton showSplashscreenCheck;
	CButton allowMultipleInstancesCheck;
	CButton onlineSignatureCheck;
	CButton doubleClickClientDetailsCheck;
	CButton standardProcessRadio;
	CButton highProcessRadio;
	CButton webSvEditButton;
	CButton autoTakeEd2kLinksCheck;
	CButton ed2kFixButton;
	CStatic nickDesc;
	CStatic languageDesc;
	CStatic miscDesc;
	CStatic tooltipDelayDesc;
	CStatic threeDDepDesc;
	CStatic mainProcessDesc;
	CButton updateFakeListCheck;
	CButton updateFakeListButton;
	CStatic updateFakeListURLLabel;
	CButton watchClipboardCheck;
};
