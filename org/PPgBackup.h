#pragma once

#include "Preferences.h"
#include "afxwin.h"

// CPPgBackup dialog

class CPPgBackup : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgBackup)

public:
	CPPgBackup();
	virtual ~CPPgBackup();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs;}

	// Dialog Data
	enum { IDD = IDD_PPG_BACKUP };
protected:
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}
	CPreferences *m_pPrefs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	void Localize(void);

private:
	void LoadSettings();
	void CheckBackupNowButton();
	bool guardian;
	bool SelectDir(TCHAR* outdir, CString titletext);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBackupnow();
	afx_msg void OnBnClickedCommon();
	afx_msg void OnBnClickedPart();
	afx_msg void OnBnClickedSelectall();
	afx_msg void OnBnClickedAutobackup();
	afx_msg void OnBnClickedOverwrite();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnEnChangeBackupDir();
	afx_msg void OnBnClickedBackupAuto();
	afx_msg void OnBnClickedScheduledBackupCheck();
	afx_msg void OnEnChangeScheduledBackupInterval()			{ SetModified(); };
protected:
	BOOL datFiles;
	BOOL metFiles;
	BOOL iniFiles;
	BOOL partFiles;
	BOOL partMetFiles;
	BOOL partTxtsrcFiles;
	BOOL autoBackup;
	BOOL overwriteFiles;
	CString backupDir;
	BOOL scheduledBackup;
	CString scheduledBackupInterval;
private:
	CString backupTypeDesc;
	CButton selectAllButton;
	CButton backupNowButton;
	CString backupOptionDesc;
	CString backupDirDesc;
	CButton autoBackupCheck;
	CButton overwriteFilesCheck;
	CEdit	backupDirEdit;
	CButton backupBrowseButton;
	CButton scheduledBackupCheck;
	CEdit	scheduledBackupEdit;
	CString hours;
};
