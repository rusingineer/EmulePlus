// PPgWindow.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CPPgWindow dialog

IMPLEMENT_DYNAMIC(CPPgWindow, CPropertyPage)
CPPgWindow::CPPgWindow()
	: CPropertyPage(CPPgWindow::IDD)
	, startMin(FALSE)
	, minTray(FALSE)
	, closeToTray(FALSE)
	, bPromptOnExit(FALSE)
	, bPromptOnDisconnect(FALSE)
	, bPromptOnFriendDel(FALSE)
	, bringToForeground(FALSE)
	, showRateOnTitle(FALSE)
	, showSpeedMeterOnToolbar(FALSE)
{
}

CPPgWindow::~CPPgWindow()
{
}

void CPPgWindow::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_STARTMIN, startMin);
	DDX_Check(pDX, IDC_MINTRAY, minTray);
	DDX_Check(pDX, IDC_CB_TBN_CLOSETOTRAY, closeToTray);
	DDX_Check(pDX, IDC_EXIT, bPromptOnExit);
	DDX_Check(pDX, IDC_COMFIRM_DISCONNECT, bPromptOnDisconnect);
	DDX_Check(pDX, IDC_COMFIRM_FRIENDDEL, bPromptOnFriendDel);
	DDX_Check(pDX, IDC_BRINGTOFOREGROUND, bringToForeground);
	DDX_Check(pDX, IDC_SHOWRATEONTITLE, showRateOnTitle);
	DDX_Check(pDX, IDC_TBSPEEDMETER, showSpeedMeterOnToolbar);
	DDX_Text(pDX, IDC_USEDFONT_LBL, usedFontDesc);
	DDX_Control(pDX, IDC_USEDFONT, fontPreviewCombo);
	DDX_Control(pDX, IDC_USEDFONTSIZE, fontSizeCombo);
	DDX_Control(pDX, IDC_STARTMIN, startMinCheck);
	DDX_Control(pDX, IDC_MINTRAY, minTrayCheck);
	DDX_Control(pDX, IDC_CB_TBN_CLOSETOTRAY, closeToTrayCheck);
	DDX_Control(pDX, IDC_EXIT, promptOnExitCheck);
	DDX_Control(pDX, IDC_COMFIRM_DISCONNECT, promptOnDisconnectCheck);
	DDX_Control(pDX, IDC_COMFIRM_FRIENDDEL, promptOnFriendDelCheck);
	DDX_Control(pDX, IDC_BRINGTOFOREGROUND, bringToForegroundCheck);
	DDX_Control(pDX, IDC_SHOWRATEONTITLE, showRateOnTitleCheck);
	DDX_Control(pDX, IDC_TBSPEEDMETER, showSpeedMeterOnToolbarCheck);
}

BEGIN_MESSAGE_MAP(CPPgWindow, CPropertyPage)
	ON_BN_CLICKED(IDC_STARTMIN, OnSettingsChange)
	ON_BN_CLICKED(IDC_MINTRAY, OnSettingsChange)
	ON_BN_CLICKED(IDC_EXIT, OnSettingsChange)
	ON_BN_CLICKED(IDC_COMFIRM_DISCONNECT, OnSettingsChange)
	ON_BN_CLICKED(IDC_COMFIRM_FRIENDDEL, OnSettingsChange)
	ON_BN_CLICKED(IDC_BRINGTOFOREGROUND, OnSettingsChange)
	ON_BN_CLICKED(IDC_CB_TBN_CLOSETOTRAY, OnSettingsChange)
	ON_CBN_SELCHANGE(IDC_USEDFONT, OnSettingsChange)
	ON_CBN_SELCHANGE(IDC_USEDFONTSIZE, OnSettingsChange)
	ON_BN_CLICKED(IDC_SHOWRATEONTITLE, OnSettingsChange)
	ON_BN_CLICKED(IDC_TBSPEEDMETER, OnSettingsChange)
END_MESSAGE_MAP()

BOOL CPPgWindow::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	fontPreviewCombo.m_style = CFontPreviewCombo::NAME_ONLY;
	fontPreviewCombo.Init();

	fontSizeCombo.AddString(_T("8"));
	fontSizeCombo.AddString(_T("10"));
	fontSizeCombo.AddString(_T("12"));
	LoadSettings();
	Localize();

	return TRUE;
}

void CPPgWindow::LoadSettings(void)
{
	startMin = m_pPrefs->GetStartMinimized();
	minTray = m_pPrefs->DoMinToTray();
	bPromptOnExit = m_pPrefs->IsConfirmExitEnabled();
	bPromptOnDisconnect = m_pPrefs->IsConfirmDisconnectEnabled();
	bPromptOnFriendDel = m_pPrefs->IsConfirmFriendDelEnabled();
	bringToForeground = m_pPrefs->IsBringToFront();
	closeToTray = m_pPrefs->GetCloseToTray();
	showRateOnTitle = m_pPrefs->ShowRatesOnTitle();
	showSpeedMeterOnToolbar = m_pPrefs->GetShowToolbarSpeedMeter();

	int iFontIdx = fontPreviewCombo.FindStringExact(0, m_pPrefs->GetUsedFont());

	if (iFontIdx == LB_ERR)
		iFontIdx = 0;
	fontPreviewCombo.SetCurSel(iFontIdx);
	fontSizeCombo.SetCurSel(0);
	if (m_pPrefs->GetFontSize())
		fontSizeCombo.SetCurSel((int)((m_pPrefs->GetFontSize()-80)/20));

	UpdateData(FALSE);

	SetModified(FALSE);
}

BOOL CPPgWindow::OnApply()
{
	if(m_bModified)
	{
		UpdateData(TRUE);

		byte mintotray_old = m_pPrefs->DoMinToTray();

		m_pPrefs->SetStartMinimized(startMin);
		m_pPrefs->SetMinToTray(minTray);
		m_pPrefs->SetConfirmExitEnabled(bPromptOnExit);
		m_pPrefs->SetConfirmDisconnectEnabled(bPromptOnDisconnect);
		m_pPrefs->SetConfirmFriendDelEnabled(bPromptOnFriendDel);
		m_pPrefs->SetBringToFront(bringToForeground);
		m_pPrefs->SetCloseToTray(closeToTray);
		m_pPrefs->SetRatesOnTitle(showRateOnTitle);
		m_pPrefs->SetShowToolbarSpeedMeter(showSpeedMeterOnToolbar);

		if (mintotray_old != (byte)m_pPrefs->DoMinToTray())
		{
			g_App.m_pMDlg->TraySetMinimizeToTray(m_pPrefs->GetMinTrayPTR());
			g_App.m_pMDlg->TrayMinimizeToTrayChanged();
		}

		CString buffer;
		fontPreviewCombo.GetWindowText(buffer);
		m_pPrefs->SetUsedFont(buffer);
		m_pPrefs->SetFontSize(fontSizeCombo.GetCurSel()*20 + 80);
		// Font update
		g_App.m_pMDlg->m_fontDefault.DeleteObject();
		g_App.m_pMDlg->m_fontDefault.CreatePointFont(m_pPrefs->GetFontSize(),buffer);
		g_App.m_pMDlg->m_wndServer.InitFont();
		g_App.m_pMDlg->m_wndIRC.UpdateFont();
		g_App.m_pMDlg->m_wndChat.m_ctlChatSelector.UpdateFont();

		g_App.m_pMDlg->m_wndTransfer.m_ctlDownloadList.SetStyle();
		g_App.m_pMDlg->m_ctlToolBar.ShowSpeedMeter(m_pPrefs->GetShowToolbarSpeedMeter());

		SetModified(FALSE);

		if (!g_App.m_pPrefs->ShowRatesOnTitle())
			g_App.m_pMDlg->SetWindowText(CLIENT_NAME_WITH_VER);
	}

	return CPropertyPage::OnApply();
}

void CPPgWindow::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_PW_WINDOW));
		startMinCheck.SetWindowText(GetResString(IDS_PREF_STARTMIN));
		minTrayCheck.SetWindowText(GetResString(IDS_MINIMIZE_TO_SYSTRAY));
		promptOnExitCheck.SetWindowText(GetResString(IDS_PW_PROMPT));
		promptOnDisconnectCheck.SetWindowText(GetResString(IDS_PW_PROMPT_DISCONNECT));
		promptOnFriendDelCheck.SetWindowText(GetResString(IDS_PW_PROMPT_FRIENDDEL));
		bringToForegroundCheck.SetWindowText(GetResString(IDS_PW_FRONT));
		closeToTrayCheck.SetWindowText(GetResString(IDS_PW_TBN_CLOSETOTRAY));
		GetResString(&usedFontDesc, IDS_USEDFONT_LBL);
		showRateOnTitleCheck.SetWindowText(GetResString(IDS_SHOWRATEONTITLE));
		showSpeedMeterOnToolbarCheck.SetWindowText(GetResString(IDS_PW_SHOWTOOLBARSPEEDMETER));

		UpdateData(FALSE);
	}
}
