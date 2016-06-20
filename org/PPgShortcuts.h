#pragma once
#include "afxcmn.h"
#include "KeyboardShortcut.h"
#include "afxwin.h"
#include "resource.h"

typedef struct
{
	HTREEITEM hHnd;
	CKeyboardShortcut Shortcut;
} ShortcutTreeItem;

enum
{
	SCUT_NODE_GEN,
	SCUT_NODE_GEN_WIN,
	SCUT_NODE_GEN_LINK,
	SCUT_NODE_GEN_FILES,
	SCUT_NODE_DL,
	SCUT_NODE_DL_ADV,
	SCUT_NODE_SRC,
	SCUT_NODE_SHARED,

	SCUT_NODES
};

// CPPgShortcuts dialog

class CPPgShortcuts : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgShortcuts)

public:
	CPPgShortcuts();
	virtual ~CPPgShortcuts();

	enum { IDD = IDD_PPG_SHORTCUTS };

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	void CreateShortcutsTree(void);
	void RefreshShortcutsTree(void);
	void RefreshKeyCombo(void);
	void SetPrefs(CPreferences* pPrefs)	{ m_pPrefs = pPrefs; }
	void Localize(void);
	int  GetKeyComboIndexFromShortcutKey(char cKey) const;
	char GetShortcutKeyFromKeyComboIndex(int iIndex) const;
	int  GetShortcutTreeItemIndexFromItem(HTREEITEM hItem);
	afx_msg void OnSettingsChange(void);
	afx_msg void OnEnableShortcutClick(void);
	afx_msg void OnDestroy();
	afx_msg void OnTvnSelchangingShortcutsTree(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateCheckBoxesState(void);

protected:
	CPreferences *m_pPrefs;

	bool m_bModified;
	void SetModified(BOOL bChanged = TRUE)
	{
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

protected:
	BOOL shortcutsEnable;
	BOOL shortcutsAlt;
	BOOL shortcutsCtrl;
	BOOL shortcutsShift;
	BOOL shortcutsWin;

private:
	void LoadSettings(void);
	void LoadShortcut(HTREEITEM item);
	void SaveShortcut(HTREEITEM item);

private:
	CImageList imagelistShortcutsTree;
	ShortcutTreeItem m_ShortcutTreeItem[SCUT_COUNT];
	HTREEITEM m_ahTree[SCUT_NODES];

	CButton shortcutsEnableCheck;
	CButton shortcutsAltCheck;
	CButton shortcutsCtrlCheck;
	CButton shortcutsShiftCheck;
	CButton shortcutsWinCheck;
	CStatic shortcutsStaticLbl;
	CComboBox shortcutsKeyCombo;
	CStatic shortcutsGroupBox;
	CTreeCtrl shortcutsTree;
};
