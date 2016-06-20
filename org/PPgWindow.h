//	this file is part of eMule Plus
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

#include "FontPreviewCombo.h"

class CPreferences;

class CPPgWindow : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgWindow)

public:
	CPPgWindow();
	virtual ~CPPgWindow();
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_WINDOW };
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
protected:
	BOOL startMin;
	BOOL minTray;
	BOOL closeToTray;
	BOOL bPromptOnExit;
	BOOL bPromptOnDisconnect;
	BOOL bPromptOnFriendDel;
	BOOL bringToForeground;
	BOOL showRateOnTitle;
	BOOL showSpeedMeterOnToolbar;
	BOOL m_bKeepSearchHistory;
private:
	CFontPreviewCombo	fontPreviewCombo;
	CComboBox			fontSizeCombo;
};
