// PPgShortcuts.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgShortcuts.h"


// CPPgShortcuts dialog

IMPLEMENT_DYNAMIC(CPPgShortcuts, CPropertyPage)
CPPgShortcuts::CPPgShortcuts()
	: CPropertyPage(CPPgShortcuts::IDD)
{
}

CPPgShortcuts::~CPPgShortcuts()
{
}

void CPPgShortcuts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHORTCUTS_TREE, shortcutsTree);
	DDX_Check(pDX, IDC_SHORTCUTS_ENABLE, shortcutsEnable);
	DDX_Check(pDX, IDC_SHORTCUTS_ALT, shortcutsAlt);
	DDX_Check(pDX, IDC_SHORTCUTS_CTRL, shortcutsCtrl);
	DDX_Check(pDX, IDC_SHORTCUTS_SHIFT, shortcutsShift);
	DDX_Check(pDX, IDC_SHORTCUTS_WIN, shortcutsWin);
	DDX_Control(pDX, IDC_SHORTCUTS_ALT, shortcutsAltCheck);
	DDX_Control(pDX, IDC_SHORTCUTS_CTRL, shortcutsCtrlCheck);
	DDX_Control(pDX, IDC_SHORTCUTS_SHIFT, shortcutsShiftCheck);
	DDX_Control(pDX, IDC_SHORTCUTS_WIN, shortcutsWinCheck);
	DDX_Control(pDX, IDC_SHORTCUTS_STATIC, shortcutsStaticLbl);
	DDX_Control(pDX, IDC_SHORTCUTS_COMBO, shortcutsKeyCombo);
	DDX_Control(pDX, IDC_SHORTCUTS_GROUPBOX, shortcutsGroupBox);
}

BEGIN_MESSAGE_MAP(CPPgShortcuts, CPropertyPage)
	ON_BN_CLICKED(IDC_SHORTCUTS_ENABLE, OnEnableShortcutClick)
	ON_BN_CLICKED(IDC_SHORTCUTS_ALT, OnSettingsChange)
	ON_BN_CLICKED(IDC_SHORTCUTS_CTRL, OnSettingsChange)
	ON_BN_CLICKED(IDC_SHORTCUTS_SHIFT, OnSettingsChange)
	ON_BN_CLICKED(IDC_SHORTCUTS_WIN, OnSettingsChange)
	ON_CBN_SELCHANGE(IDC_SHORTCUTS_COMBO, OnSettingsChange)
	ON_NOTIFY(TVN_SELCHANGING, IDC_SHORTCUTS_TREE, OnTvnSelchangingShortcutsTree)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CPPgShortcuts::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CreateShortcutsTree();	// WARNING : shortcuts tree
	RefreshKeyCombo();		//           and key combo must have been created
	LoadSettings();			//           before calling LoadSettings function!
	Localize();

	return TRUE;
}

void CPPgShortcuts::OnDestroy()
{
//	Free resources on close not to keep them until next creation of this page
	imagelistShortcutsTree.DeleteImageList();
	shortcutsTree.DeleteAllItems();

	CPropertyPage::OnDestroy();
}

BOOL CPPgShortcuts::OnApply()
{
	for (int i = 0; i < SCUT_COUNT; i++)
		m_pPrefs->SetShortcutCode(m_ShortcutTreeItem[i].Shortcut.GetCode(), i);

	RefreshShortcutsTree();

	return CPropertyPage::OnApply();
}

void CPPgShortcuts::OnSettingsChange(void)
{
	SetModified();
	SaveShortcut(shortcutsTree.GetSelectedItem());
}

void CPPgShortcuts::OnEnableShortcutClick(void)
{
	OnSettingsChange();
	UpdateCheckBoxesState();
}

void CPPgShortcuts::CreateShortcutsTree(void)
{
	static const uint16 s_auIconResID[] =
	{
	//	Generic icon
		IDI_TVI_GENERIC,	// 0 - items
	//	Main category icons
		IDI_TRAYICON,		// 1 - General
		IDI_DIRECTDOWNLOAD,	// 2 - Download list
		IDI_USERS,			// 3 - Sources
		IDI_PREF_FILES,		// 4 - Share list
	//	Sub-category icons
		IDI_PREF_WINDOW,	// 5 - General > Show window
		IDI_PREF_ADVANCED,	// 6 - General > Adanved
		IDI_GLOBAL,			// 7 - UL & Share > ED2K Links
		IDI_PREF_FOLDERS	// 8 - General Files/Folders
	};

	imagelistShortcutsTree.Create(16, 16, g_App.m_iDfltImageListColorFlags | ILC_MASK, ELEMENT_COUNT(s_auIconResID), 0);
	FillImgLstWith16x16Icons(&imagelistShortcutsTree, s_auIconResID, ELEMENT_COUNT(s_auIconResID));
	shortcutsTree.SetImageList(&imagelistShortcutsTree, TVSIL_NORMAL);

	m_ahTree[SCUT_NODE_GEN] = shortcutsTree.InsertItem(_T(""), 1, 1);
	m_ahTree[SCUT_NODE_DL] = shortcutsTree.InsertItem(_T(""), 2, 2);
	m_ahTree[SCUT_NODE_SRC] = shortcutsTree.InsertItem(_T(""), 3, 3);
	m_ahTree[SCUT_NODE_SHARED] = shortcutsTree.InsertItem(_T(""), 4, 4);
	m_ShortcutTreeItem[SCUT_WIN_MINIMIZE].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN]);
	m_ShortcutTreeItem[SCUT_WIN_SWITCH].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN]);
	m_ahTree[SCUT_NODE_GEN_WIN] = shortcutsTree.InsertItem(_T(""), 5, 5, m_ahTree[SCUT_NODE_GEN]);
	m_ShortcutTreeItem[SCUT_WIN_SRV].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_TRANSFER].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_SEARCH].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_SHAREDFILES].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_CHAT].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_IRC].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_STATS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ShortcutTreeItem[SCUT_WIN_PREFS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_WIN]);
	m_ahTree[SCUT_NODE_GEN_LINK] = shortcutsTree.InsertItem(_T(""), 7, 7, m_ahTree[SCUT_NODE_GEN]);
	m_ShortcutTreeItem[SCUT_LINK].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_LINK]);
	m_ShortcutTreeItem[SCUT_LINK_HTML].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_LINK]);
	m_ShortcutTreeItem[SCUT_LINK_SOURCE].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_LINK]);
	m_ahTree[SCUT_NODE_GEN_FILES] = shortcutsTree.InsertItem(_T(""), 8, 8, m_ahTree[SCUT_NODE_GEN]);
	m_ShortcutTreeItem[SCUT_FILE_OPENDIR].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_FILES]);
	m_ShortcutTreeItem[SCUT_DL_CANCEL].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_STOP].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_PAUSE].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_RESUME].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_FILE_OPEN].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_PREVIEW].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_FILE_RENAME].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_FILE_COMMENTS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_FILE_DETAILS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_FD_SOURCES].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_CLEAR].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_CLEARALL].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_SHOWALL].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_DL_DEFSORT].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL]);
	m_ahTree[SCUT_NODE_DL_ADV] = shortcutsTree.InsertItem(_T(""), 6, 6, m_ahTree[SCUT_NODE_DL]);
	m_ShortcutTreeItem[SCUT_FILE_NAMECLEANUP].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_DL_PREALLOC].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_DL_A4AF].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_DL_A4AFSAMECAT].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_DL_A4AFAUTO].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_DL_A4AFOTHER].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_DL_ADV]);
	m_ShortcutTreeItem[SCUT_SRC_DETAILS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_SRC]);
	m_ShortcutTreeItem[SCUT_SRC_FRIEND].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_SRC]);
	m_ShortcutTreeItem[SCUT_SRC_MSG].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_SRC]);
	m_ShortcutTreeItem[SCUT_SRC_SHAREDFILES].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_SRC]);
	m_ShortcutTreeItem[SCUT_FILE_EDITCOMMENTS].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_SHARED]);
	m_ShortcutTreeItem[SCUT_LINK_HASH].hHnd = shortcutsTree.InsertItem(_T(""), m_ahTree[SCUT_NODE_GEN_LINK]);

	for (unsigned ui = 0; ui < SCUT_NODES; ui++)
		shortcutsTree.SetItemState(m_ahTree[ui], TVIS_BOLD, TVIS_BOLD);

	shortcutsTree.Expand(m_ahTree[SCUT_NODE_GEN], TVE_EXPAND);
	shortcutsTree.Expand(m_ahTree[SCUT_NODE_GEN_WIN], TVE_EXPAND);
	shortcutsTree.Expand(m_ahTree[SCUT_NODE_DL_ADV], TVE_EXPAND);
	shortcutsTree.Expand(m_ahTree[SCUT_NODE_GEN_LINK], TVE_EXPAND);
	shortcutsTree.Expand(m_ahTree[SCUT_NODE_GEN_FILES], TVE_EXPAND);

	shortcutsTree.Select(m_ShortcutTreeItem[0].hHnd, TVGN_CARET);
}

void CPPgShortcuts::RefreshShortcutsTree(void)
{
	static const uint16 s_auResTbl[] =
	{
		IDS_PW_GENERAL,				//SCUT_NODE_GEN
		IDS_TREE_GENERAL_SHOW_WND,	//SCUT_NODE_GEN_WIN
		IDS_ED2KLINKFIX,			//SCUT_NODE_GEN_LINK
		IDS_FILES,					//SCUT_NODE_GEN_FILES
		IDS_TREE_DL,				//SCUT_NODE_DL
		IDS_PW_ADVANCED,			//SCUT_NODE_DL_ADV
		IDS_DL_SOURCES,				//SCUT_NODE_SRC
		IDS_SHAREDFILES				//SCUT_NODE_SHARED
	};
	static const uint16 s_auResTbl2[] =
	{
		IDS_MINIMIZE_TO_SYSTRAY,			//SCUT_WIN_MINIMIZE
		IDS_TREE_GENERAL_SWITCH_WND,		//SCUT_WIN_SWITCH
		IDS_SERVERS,						//SCUT_WIN_SRV
		IDS_EM_TRANS,						//SCUT_WIN_TRANSFER
		IDS_SEARCH_NOUN,					//SCUT_WIN_SEARCH
		IDS_EM_FILES,						//SCUT_WIN_SHAREDFILES
		IDS_MESSAGES,						//SCUT_WIN_CHAT
		IDS_IRC,							//SCUT_WIN_IRC
		IDS_STATISTICS,						//SCUT_WIN_STATS
		IDS_PREFERENCES,					//SCUT_WIN_PREFS
		IDS_CANCEL,							//SCUT_DL_CANCEL
		IDS_STOP_VERB,						//SCUT_DL_STOP
		IDS_PAUSE_VERB,						//SCUT_DL_PAUSE
		IDS_RESUME,							//SCUT_DL_RESUME
		IDS_OPENFILE,						//SCUT_FILE_OPEN
		IDS_OPENFOLDER,						//SCUT_FILE_OPENDIR
		IDS_PREVIEW_VERB,					//SCUT_DL_PREVIEW
		IDS_RENAME,							//SCUT_FILE_RENAME
		IDS_CMT_SHOWALL,					//SCUT_FILE_COMMENTS
		IDS_DL_INFO,						//SCUT_FILE_DETAILS
		IDS_SHOW_FILEDETAILS_SOURCES,		//SCUT_DL_FD_SOURCES
		IDS_TREE_DL_CLEAR_COMPLETED,		//SCUT_DL_CLEAR
		IDS_TREE_DL_CLEAR_ALL_COMPLETED,	//SCUT_DL_CLEARALL
		IDS_TREE_DL_SHOW_ALLUP,				//SCUT_DL_SHOWALL
		IDS_TREE_DL_USE_SORT,				//SCUT_DL_DEFSORT
		IDS_DL_PREALLOCATE,					//SCUT_DL_PREALLOC
		IDS_ALL_A4AF_TO_HERE,				//SCUT_DL_A4AF
		IDS_TREE_DL_A4AF_AUTO,				//SCUT_DL_A4AFAUTO
		IDS_ALL_A4AF_TO_OTHER,				//SCUT_DL_A4AFOTHER
		IDS_ALL_A4AF_SAMECAT,				//SCUT_DL_A4AFSAMECAT
		IDS_DL_LINK1,						//SCUT_LINK
		IDS_DL_LINK2,						//SCUT_LINK_HTML
		IDS_CREATESOURCELINK,				//SCUT_LINK_SOURCE
		IDS_SHOWDETAILS,					//SCUT_SRC_DETAILS
		IDS_ADDREMOVE_FRIEND,				//SCUT_SRC_FRIEND
		IDS_SEND_MSG,						//SCUT_SRC_MSG
		IDS_VIEWFILES,						//SCUT_SRC_SHAREDFILES
		IDS_EDIT_FILE_COMMENT,				//SCUT_FILE_EDITCOMMENTS
		IDS_COPYHASH,						//SCUT_LINK_HASH
		IDS_DOCLEANUP						//SCUT_FILE_NAMECLEANUP
	};
	CString	strTmp;

	for (unsigned ui = 0; ui < SCUT_NODES; ui++)
	{
		::GetResString(&strTmp, s_auResTbl[ui]);
		shortcutsTree.SetItemText(m_ahTree[ui], strTmp);
	}
	for (unsigned ui = 0; ui < SCUT_COUNT; ui++)
	{
		::GetResString(&strTmp, s_auResTbl2[ui]);
		strTmp += GetStringFromShortcutCode(ui, SSP_SPACE_PREFIX, TRUE);
		shortcutsTree.SetItemText(m_ShortcutTreeItem[ui].hHnd, strTmp);
	}
}

void CPPgShortcuts::RefreshKeyCombo(void)
{
	static const uint16 s_auResTbl[] =
	{
		IDS_SHORTCUTS_ENTER_KEY,	//  0
		IDS_SHORTCUTS_SPACE_KEY,	//  1
		IDS_SHORTCUTS_TAB_KEY,		//  2
		IDS_SHORTCUTS_BACK_KEY,		//  3
		IDS_SHORTCUTS_INSERT_KEY,	//  4
		IDS_SHORTCUTS_DELETE_KEY,	//  5
		IDS_SHORTCUTS_HOME_KEY,		//  6
		IDS_SHORTCUTS_END_KEY,		//  7
		IDS_SHORTCUTS_PAGEUP_KEY,	//  8
		IDS_SHORTCUTS_PAGEDOWN_KEY	//  9
	};
	static const TCHAR s_auResTbl2[][4] =
	{
		_T("F1"),	// 10
		_T("F2"),	// 11
		_T("F3"),	// 12
		_T("F4"),	// 13
		_T("F5"),	// 14
		_T("F6"),	// 15
		_T("F7"),	// 16
		_T("F8"),	// 17
		_T("F9"),	// 18
		_T("F10"),	// 19
		_T("F11"),	// 20
		_T("F12"),	// 21
		_T("A"),	// 22 - letters start
		_T("B"),	// 23
		_T("C"),	// 24
		_T("D"),	// 25
		_T("E"),	// 26
		_T("F"),	// 27
		_T("G"),	// 28
		_T("H"),	// 29
		_T("I"),	// 30
		_T("J"),	// 31
		_T("K"),	// 32
		_T("L"),	// 33
		_T("M"),	// 34
		_T("N"),	// 35
		_T("O"),	// 36
		_T("P"),	// 37
		_T("Q"),	// 38
		_T("R"),	// 39
		_T("S"),	// 40
		_T("T"),	// 41
		_T("U"),	// 42
		_T("V"),	// 43
		_T("W"),	// 44
		_T("X"),	// 45
		_T("Y"),	// 46
		_T("Z"),	// 47 - letters end
		_T("0"),	// 48 - numbers start
		_T("1"),	// 49
		_T("2"),	// 50
		_T("3"),	// 51
		_T("4"),	// 52
		_T("5"),	// 53
		_T("6"),	// 54
		_T("7"),	// 55
		_T("8"),	// 56
		_T("9")		// 57 - numbers end
	};
	shortcutsKeyCombo.ResetContent();

	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auResTbl); ui++)
		shortcutsKeyCombo.AddString(GetResString(s_auResTbl[ui]));
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auResTbl2); ui++)
		shortcutsKeyCombo.AddString(reinterpret_cast<const TCHAR*>(&s_auResTbl2[ui]));

	int iIndex = GetShortcutTreeItemIndexFromItem(shortcutsTree.GetSelectedItem());
	if (iIndex >= 0)
		shortcutsKeyCombo.SetCurSel(GetKeyComboIndexFromShortcutKey(m_ShortcutTreeItem[iIndex].Shortcut.GetKey()));
}

void CPPgShortcuts::LoadSettings(void)
{
	for (int i = 0; i < SCUT_COUNT; i++)
		m_ShortcutTreeItem[i].Shortcut.SetCode(m_pPrefs->GetShortcutCode(i));

	LoadShortcut(shortcutsTree.GetSelectedItem());

	SetModified(FALSE);
}

void CPPgShortcuts::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_PW_SHORTCUTS));
		shortcutsGroupBox.SetWindowText(_T("      ") + GetResString(IDS_SHORTCUTS_ENABLE)); // the spaces let a blank space to display the IDC_SHORTCUTS_ENABLE checkbox
		shortcutsStaticLbl.SetWindowText(GetResString(IDS_SHORTCUTS_STATIC));
		RefreshShortcutsTree();
		RefreshKeyCombo();
	}
}

int CPPgShortcuts::GetShortcutTreeItemIndexFromItem(HTREEITEM hItem)
{
	for (int i = 0; i < SCUT_COUNT; i++)
		if (m_ShortcutTreeItem[i].hHnd == hItem)
			return i;

	return -1;
}

void CPPgShortcuts::LoadShortcut(HTREEITEM hItem)
{
	int i = GetShortcutTreeItemIndexFromItem(hItem);

	if (i >= 0)
	{
		shortcutsEnable	= m_ShortcutTreeItem[i].Shortcut.IsEnabled();
		shortcutsAlt	= m_ShortcutTreeItem[i].Shortcut.IsAlt();
		shortcutsCtrl	= m_ShortcutTreeItem[i].Shortcut.IsCtrl();
		shortcutsShift	= m_ShortcutTreeItem[i].Shortcut.IsShift();
		shortcutsWin	= m_ShortcutTreeItem[i].Shortcut.IsWin();
		shortcutsKeyCombo.SetCurSel(GetKeyComboIndexFromShortcutKey(m_ShortcutTreeItem[i].Shortcut.GetKey()));
		UpdateData(FALSE);
		UpdateCheckBoxesState();
	}
}

void CPPgShortcuts::SaveShortcut(HTREEITEM hItem)
{
	int i = GetShortcutTreeItemIndexFromItem(hItem);

	if (i >= 0)
	{
		UpdateData();
		m_ShortcutTreeItem[i].Shortcut.SetEnabled(shortcutsEnable);
		m_ShortcutTreeItem[i].Shortcut.SetAlt(shortcutsAlt);
		m_ShortcutTreeItem[i].Shortcut.SetCtrl(shortcutsCtrl);
		m_ShortcutTreeItem[i].Shortcut.SetShift(shortcutsShift);
		m_ShortcutTreeItem[i].Shortcut.SetWin(shortcutsWin);
		m_ShortcutTreeItem[i].Shortcut.SetKey(GetShortcutKeyFromKeyComboIndex(shortcutsKeyCombo.GetCurSel()));
	}
}

int CPPgShortcuts::GetKeyComboIndexFromShortcutKey(char cKey) const
{
	if (cKey == VK_RETURN) // Enter key
		return 0;
	else if (cKey == VK_SPACE) // Spacebar
		return 1;
	else if (cKey == VK_TAB) // Tab key
		return 2;
	else if (cKey == VK_BACK) // Backspace key
		return 3;
	else if (cKey == VK_INSERT) // Insert key
		return 4;
	else if (cKey == VK_DELETE) // Delete key
		return 5;
	else if (cKey == VK_HOME) // Home key
		return 6;
	else if (cKey == VK_END) // End key
		return 7;
	else if (cKey == VK_PRIOR) // Page Up key
		return 8;
	else if (cKey == VK_NEXT) // Page Down key
		return 9;
	else if ((cKey >= VK_F1) && (cKey <= VK_F12)) // function keys
		return (cKey - VK_F1) + 10;
	else if ((cKey >= 'A') && (cKey <= 'Z')) // letter keys
		return (cKey - 'A') + 22;
	else if ((cKey >= '0') && (cKey <= '9')) // number keys
		return (cKey - '0') + 48;
	else /*** should never happen ***/
	{
		AfxMessageBox(_T("Error in key code. 'Enter' key was set to default."), MB_OK | MB_ICONERROR);
		return 0; // return Enter key index by default
	}
}

char CPPgShortcuts::GetShortcutKeyFromKeyComboIndex(int iIndex) const
{
	if (iIndex == 0) // Enter key
		return VK_RETURN;
	else if (iIndex == 1) // Spacebar
		return VK_SPACE;
	else if (iIndex == 2) // Tab key
		return VK_TAB;
	else if (iIndex == 3) // Backspace key
		return VK_BACK;
	else if (iIndex == 4) // Insert key
		return VK_INSERT;
	else if (iIndex == 5) // Delete key
		return VK_DELETE;
	else if (iIndex == 6) // Home key
		return VK_HOME;
	else if (iIndex == 7) // End key
		return VK_END;
	else if (iIndex == 8) // Page Up key
		return VK_PRIOR;
	else if (iIndex == 9) // Page Down key
		return VK_NEXT;
	else if ((iIndex >= 10) && (iIndex <= 21)) // function keys
		return VK_F1 + (iIndex - 10);
	else if ((iIndex >= 22) && (iIndex <= 47)) // letter keys
		return 'A' + (iIndex - 22);
	else if ((iIndex >= 48) && (iIndex <= 57)) // number keys
		return '0' + (iIndex - 48);
	else /*** should never happen ***/
	{
		AfxMessageBox(_T("Error in key combo index. 'Enter' key was set to default."), MB_OK | MB_ICONERROR);
		return VK_RETURN;
	}
}

void CPPgShortcuts::UpdateCheckBoxesState(void)
{
	UpdateData();
	shortcutsAltCheck.EnableWindow(shortcutsEnable);
	shortcutsCtrlCheck.EnableWindow(shortcutsEnable);
	shortcutsShiftCheck.EnableWindow(shortcutsEnable);
	shortcutsWinCheck.EnableWindow(shortcutsEnable);
	shortcutsStaticLbl.EnableWindow(shortcutsEnable);
	shortcutsKeyCombo.EnableWindow(shortcutsEnable);
}

// This function will prevent the user to select root items
void CPPgShortcuts::OnTvnSelchangingShortcutsTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (shortcutsTree.ItemHasChildren(pNMTreeView->itemNew.hItem))
		*pResult = TRUE;
	else
	{
		LoadShortcut(pNMTreeView->itemNew.hItem);
		*pResult = FALSE;
	}
}
