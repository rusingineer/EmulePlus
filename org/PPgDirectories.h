#pragma once

#include "preferences.h"
#include "DirectoryTreeCtrl.h"
#include "afxwin.h"

// CPPgDirectories dialog

class CPPgDirectories : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgDirectories)

public:
	CPPgDirectories();
	virtual ~CPPgDirectories();

	void SetPrefs(CPreferences* in_prefs) {	m_pPrefs = in_prefs; }

// Dialog Data
	enum { IDD = IDD_PPG_DIRECTORIES };

protected:
	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
public:
	CDirectoryTreeCtrl m_ShareSelector;
	CDirectoryTreeCtrl m_TempSelector;	// InterCeptor (multiple tempdirs)
	CPreferences* m_pPrefs;
private:
	void LoadSettings(void);
	bool SelectDir(const TCHAR* indir, TCHAR* outdir, const CString& titletext);

public:
	virtual BOOL OnApply();

	afx_msg void OnBnClickedSelincdir();
	afx_msg void OnBnClickedSeltempdir();
	afx_msg void OnBnClickedSelvlc();
	afx_msg void OnEnChange()	{ SetModified(); }
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	void Localize(void);
private:
	void CheckSharedChanges(void);
	void CheckTempChanges(void);

protected:
	CString incFiles;
	CString tempFiles;
	CString vlc;
	BOOL videoBackup;
	BOOL smallBlocks;
private:
	CEdit incFilesEdit;
	CEdit tempFilesEdit;
	CEdit vlcEdit;
	CString incomingDesc;
	CString tempDesc;
	CString vlcDesc;
	CString sharedDesc;
	CString tempDirDesc;
	CButton videoBackupCheck;
	CButton smallBlocksCheck;
	CButton selIncDirButton;
	CButton selTempDirButton;
	CButton selVlcButton;

	bool	m_bSharedDirsModified;
};
