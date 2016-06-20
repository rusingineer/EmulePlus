#pragma once

// CPPgStats dialog

#include "preferences.h"
#include "ColorButton.h"
#include "afxcmn.h"
#include "afxwin.h"

class CPPgStats : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgStats)

public:
	CPPgStats();
	virtual ~CPPgStats();
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

// Dialog Data
	enum { IDD = IDD_PPG_STATS };

protected:
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	CPreferences *m_pPrefs;	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG(CSpinToolBar)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	void ShowInterval();

	DECLARE_MESSAGE_MAP()
private:	
	DWORD	statcolors[12];

public:
	afx_msg void OnEnChangeACStats()		{SetModified();}
	afx_msg void OnBnClickedColorButton()	{SetModified();}
	afx_msg void OnCbnSelchangeRatio()		{SetModified();}
	afx_msg void OnCbnSelchangeColorselector();
	afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
	void Localize(void);
protected:
	CString acStat;
	int graphsUpdateInterval;
	int averageGraphTime;
	int statisticsUpdateInterval;
private:
	CColorButton colorButton;
	CComboBox colorsCombo;
	CComboBox ratioCombo;
	CSliderCtrl updateIntervalSlider;
	CSliderCtrl averageGraphTimeSlider;
	CSliderCtrl statisticsUpdateIntervalSlider;	
	CString graphUpdateIntervalDesc;
	CString statisticsUpdateIntervalDesc;
	CString avgGraphDesc;	
	CString streeDesc;
	CString acStatDesc;
	CString acRatioDesc;
	CString prefColorsDesc;
	CString graphsDesc;
};
