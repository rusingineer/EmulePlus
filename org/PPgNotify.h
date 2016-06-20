//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once
#include "afxwin.h"


// finestra di dialogo CPPgNotify

class CPPgNotify : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgNotify)

public:	
	CPPgNotify();
	virtual ~CPPgNotify();	
	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

// Dialog Data
	enum { IDD = IDD_PPG_NOTIFY };

protected:
	CPreferences* m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV
	
	DECLARE_MESSAGE_MAP()
private:
	void LoadSettings(void);
public:
	void Localize(void);
	afx_msg void OnBnClickedCbTbnUsesound();
	afx_msg void OnBnClickedCbTbnOnlog()			{ SetModified(); }	
	afx_msg void OnBnClickedCbTbnPopAlways()		{ SetModified(); }
	afx_msg void OnBnClickedCbTbnOndownloadAdd()	{ SetModified(); }
	afx_msg void OnBnClickedCbTbnOndownload()		{ SetModified(); }
	afx_msg void OnBnClickedCbTbnImportant()		{ SetModified(); }
	afx_msg void OnBnClickedCbTbnWebServer()		{ SetModified(); }
	afx_msg void OnBnClickedCbTbnServer()			{ SetModified(); }
	afx_msg void OnBnClickedCbTbnUseScheduler()		{ SetModified(); }
	afx_msg void OnBnClickedCbTbnOnchat();
	afx_msg void OnBnClickedBtnBrowseWav();
	afx_msg void OnCbnSelchangeDtime()				{ SetModified(); }
	afx_msg void OnCbnSelchangeFsize()				{ SetModified(); }
	afx_msg void OnEnChangeEditTbnWavfile()			{ SetModified(); }
protected:
	BOOL useSound;
	BOOL onLog;
	BOOL onChat;
	BOOL onChatMessage;
	BOOL onDownloadAdded;
	BOOL onDownloadFinished;
	BOOL useScheduler;
	BOOL onWebServer;
	BOOL onImportant;
	BOOL onServerError;
	CString wavFileName;
private:	
	CComboBox dtimeCombo;
	CComboBox fsizeCombo;
	CButton useSoundCheck;
	CButton onLogCheck;
	CButton onChatCheck;
	CButton onChatMessageCheck;
	CButton onDownloadAddedCheck;
	CButton onDownloadFinishedCheck;
	CButton useSchedulerCheck;
	CButton onWebServerCheck;
	CButton onImportantCheck;
	CButton onServerErrorCheck;
	CString warningDesc;
	CString optionsDesc;
	CString propertiesDesc;
	CString displayTimeDesc;
	CString fontSizeDesc;
	CString secDesc;
	CEdit wavFileNameEdit;
	CButton wavFileNameButton;
};