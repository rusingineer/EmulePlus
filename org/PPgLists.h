#pragma once

#include "preferences.h"
#include "afxwin.h"
#include "colorbutton.h"

// CPPgLists dialog

class CPPgLists : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgLists)

public:
	CPPgLists();
	virtual ~CPPgLists();
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_LISTS };
protected:
	CPreferences *m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}
	virtual void DoDataExchange(CDataExchange* pDX);
	void LoadSettings(void);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void Localize(void);
	virtual BOOL OnApply();
	afx_msg void OnSettingsChange()						{ SetModified(); }
	afx_msg LONG OnColorButtonSelChange(UINT lParam, LONG wParam)	{ NOPRM(lParam); NOPRM(wParam); SetModified(); return TRUE; }
protected:
	BOOL showA4AF;
	BOOL showA4AFCount;
	BOOL showAvgDataRate;
	BOOL showFileTypeIcons;
	BOOL showTransferredOnCompleted;
	BOOL showDownloadPercentage;
	BOOL showPausedGray;
	BOOL showFileStatusIcons;
	BOOL showCountryFlag;
	BOOL roundSizes;
	BOOL displayUploadParts;
	BOOL bSmartFilterShowSourcesOQ;
	CString strSmartFilterMaxQR;
	CString strSmartFilterDesc;
	CString strFakeListColorLabel;
	BOOL showRatingIcons;
private:
	CButton showA4AFCheck;
	CButton showA4AFCountCheck;
	CButton showAvgDataRateCheck;
	CButton showFileTypeIconsCheck;
	CButton showTransferredOnCompletedCheck;
	CButton showDownloadPercentageCheck;
	CButton showPausedGrayCheck;
	CButton showFileStatusIconsCheck;
	CButton showCountryFlagCheck;
	CButton roundSizesCheck;
	CButton displayUploadPartsCheck;
	CButton SmartFilterShowSourcesOQCheck;
	CEdit   SmartFilterMaxQREdit;
	CStatic SmartFilterMaxQRLabel;
	CButton showRatingIconsCheck;
	CColorButton m_FakeListColorButton;
};
