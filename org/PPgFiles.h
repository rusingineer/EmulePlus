//	This file is part of eMule Plus
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either
//	version 2 of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once

// CPPgFiles dialog

#include "preferences.h"
#include "afxcmn.h"
#include "afxwin.h"

class CPPgFiles : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgFiles)

public:
	CPPgFiles();
	virtual ~CPPgFiles();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_FILES };
protected:
	CPreferences *m_pPrefs;	

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	void LoadSettings(void);
public:
	virtual BOOL OnApply();
	afx_msg void OnSettingsChange()				{ SetModified(); }
	afx_msg void OnBnClickedDAP();
	afx_msg void OnBnClickedStartPaused();
	void Localize(void);
protected:
	BOOL dapEnable;
	BOOL uap;
	BOOL startPaused;
	BOOL startPausedOnComplete;
	BOOL resumeOtherCat;
	BOOL autoClearCompleted;
	CString apHigh;
	CString apLow;
	int seeShare;
	int iFilePermission;
	int hashPriority;
	int fileBufferSize;
	int m_iCopyBufferSz;
private:
	CSliderCtrl fileBufferSizeSlider;
	CSliderCtrl m_fileCopySizeSlider;
	CString fileBufferSizeDesc;
	CEdit apHighEdit;
	CEdit apLowEdit;
	CString seeMyShareDesc;
	CButton seeEverybodyRadio;
	CButton seeFriendRadio;
	CButton seeNooneRadio;
	CButton permPublicRadio;
	CButton permFriendsRadio;
	CButton permHiddenRadio;
	CButton hashStandardRadio;
	CButton hashLowerRadio;
	CButton hashIdleRadio;
	CString apGrpDesc;
	CString apHighDesc;
	CString apLowDesc;
	CButton dapEnableCheck;
	CButton uapCheck;
	CButton startPausedCheck;
	CButton startPausedOnCompleteCheck;
	CButton resumeOtherCatCheck;
	CButton autoClearCompletedCheck;
	CString apSources1Desc;
	CString apSources2Desc;
	CString strPermFrmLbl;
	CString hashingFrmDesc;
	CString m_strCopyBufferDesc;
};
