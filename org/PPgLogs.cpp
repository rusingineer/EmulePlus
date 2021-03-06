// PPgLogs.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgLogs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CPPgLogs dialog

IMPLEMENT_DYNAMIC(CPPgLogs, CPropertyPage)

//#region message map
BEGIN_MESSAGE_MAP(CPPgLogs, CPropertyPage)
	ON_BN_CLICKED(IDC_LOGTOFILE, OnSettingsChange)
	ON_BN_CLICKED(IDC_VERBOSE, OnChangeDebugLogging)
	ON_BN_CLICKED(IDC_CM_NOTLOG, OnSettingsChange)
	ON_BN_CLICKED(IDC_UPLOAD_LOG, OnSettingsChange)
	ON_BN_CLICKED(IDC_DOWNLOAD_LOG, OnSettingsChange)
	ON_BN_CLICKED(IDC_AUTOSRC_LOG, OnSettingsChange)
	ON_BN_CLICKED(IDC_CLIENT_LOG, OnSettingsChange)
END_MESSAGE_MAP()
//#endregion

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPPgLogs::CPPgLogs()
	: CPropertyPage(CPPgLogs::IDD)
	, writeLogToFile(FALSE)
	, debugLog(FALSE)
	, cmNotLog(FALSE)
	, uploadLog(FALSE)
	, downloadLog(FALSE)
	, autoSourcesLogEnabled(FALSE)
	, clientTransferLogEnabled(FALSE)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPPgLogs::~CPPgLogs()
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPPgLogs::DoDataExchange(CDataExchange *pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGTOFILE, writeLogToFileCheck);
	DDX_Control(pDX, IDC_VERBOSE, debugLogCheck);
	DDX_Control(pDX, IDC_CM_NOTLOG, cmNotLogCheck);
	DDX_Control(pDX, IDC_UPLOAD_LOG, uploadLogCheck);
	DDX_Control(pDX, IDC_DOWNLOAD_LOG, downloadLogCheck);
	DDX_Control(pDX, IDC_AUTOSRC_LOG, autoSourcesLogEnabledCheck);
	DDX_Control(pDX, IDC_CLIENT_LOG, clientTransferLogEnabledCheck);
	DDX_Check(pDX, IDC_LOGTOFILE, writeLogToFile);
	DDX_Check(pDX, IDC_VERBOSE, debugLog);
	DDX_Check(pDX, IDC_CM_NOTLOG, cmNotLog);
	DDX_Check(pDX, IDC_UPLOAD_LOG, uploadLog);
	DDX_Check(pDX, IDC_DOWNLOAD_LOG, downloadLog);
	DDX_Check(pDX, IDC_AUTOSRC_LOG, autoSourcesLogEnabled);
	DDX_Check(pDX, IDC_CLIENT_LOG, clientTransferLogEnabled);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPPgLogs::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	LoadSettings();
	Localize();

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPPgLogs::LoadSettings(void)
{
	writeLogToFile = m_pPrefs->LogToFile();
	debugLog = m_pPrefs->GetVerbose();
	cmNotLog = m_pPrefs->IsCMNotLog();
	uploadLog = m_pPrefs->LogUploadToFile();
	downloadLog = m_pPrefs->LogDownloadToFile();
	autoSourcesLogEnabled = m_pPrefs->IsAutoSourcesLogEnabled();
	clientTransferLogEnabled = m_pPrefs->IsClientTransferLogEnabled();

	UpdateData(FALSE);

	OnChangeDebugLogging();

	SetModified(FALSE);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPPgLogs::OnApply()
{
	if(m_bModified)
	{
		UpdateData(TRUE);

		m_pPrefs->SetLogToFile(writeLogToFile);
		if (m_pPrefs->GetVerbose() != (bool)debugLog)
		{
			m_pPrefs->SetVerbose(debugLog);
			g_App.m_pMDlg->m_wndServer.ToggleDebugWindow();
			if(debugLog)
				AddDebugLogLine(_T("Debug log is active"));
		}
		m_pPrefs->SetCMNotLog(cmNotLog);
		m_pPrefs->SetLogUploadToFile(uploadLog);
		m_pPrefs->SetLogDownloadToFile(downloadLog);
		m_pPrefs->SetAutoSourcesLogEnabled(autoSourcesLogEnabled);
		m_pPrefs->SetClientTransferLogEnabled(clientTransferLogEnabled);

		SetModified(FALSE);
	}

	return CPropertyPage::OnApply();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPPgLogs::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_PW_GENERAL));
		writeLogToFileCheck.SetWindowText(GetResString(IDS_LOGTOFILE));
		debugLogCheck.SetWindowText(GetResString(IDS_VERBOSE));
		cmNotLogCheck.SetWindowText(GetResString(IDS_CM_NOTLOG));
		uploadLogCheck.SetWindowText(GetResString(IDS_UPLOAD_LOG));
		downloadLogCheck.SetWindowText(GetResString(IDS_DOWNLOAD_LOG));
		autoSourcesLogEnabledCheck.SetWindowText(GetResString(IDS_AUTOSRC_LOG_LBL));
		clientTransferLogEnabledCheck.SetWindowText(GetResString(IDS_CLIENT_LOG));
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPPgLogs::OnChangeDebugLogging()
{
	UpdateData(TRUE);
	autoSourcesLogEnabledCheck.EnableWindow(debugLog);
	clientTransferLogEnabledCheck.EnableWindow(debugLog);
	SetModified();
}
