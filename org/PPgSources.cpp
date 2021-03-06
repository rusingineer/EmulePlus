// PPgSources.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgSources.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CPPgSources dialog

IMPLEMENT_DYNAMIC(CPPgSources, CPropertyPage)
CPPgSources::CPPgSources()
	: CPropertyPage(CPPgSources::IDD)
	, autoSourcesEnabled(FALSE)
	, minAutoSourcesPerFile(0)
	, maxAutoSourcesPerFile(0)
	, maxAutoSourcesTotal(0)
	, maxAutoExchangeSources(0)
	, disableXS(FALSE)
	, enableXSUpTo(FALSE)
{
}

CPPgSources::~CPPgSources()
{
}

void CPPgSources::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_AUTOSRC_ENABLED, autoSourcesEnabled);
	DDX_Check(pDX, IDC_DISABLE_XS, disableXS);
	DDX_Check(pDX, IDC_ENABLE_XSUPTO, enableXSUpTo);
	DDX_Control(pDX, IDC_MAXSOURCEPERFILE, maxSourcePerFileEdit);
	DDX_Control(pDX, IDC_AUTOSRC_ENABLED, autoSourcesEnabledCheck);
	DDX_Control(pDX, IDC_AUTOSRC_MIN_PER_FILE, minAutoSourcesPerFileEdit);
	DDX_Control(pDX, IDC_AUTOSRC_MAX_PER_FILE, maxAutoSourcesPerFileEdit);
	DDX_Control(pDX, IDC_AUTOSRC_MAXTOTAL, maxAutoSourcesTotalEdit);
	DDX_Control(pDX, IDC_AUTOSRC_SE_UPTO, maxAutoExchangeSourcesEdit);
	DDX_Control(pDX, IDC_DISABLE_XS, disableXsCheck);
	DDX_Control(pDX, IDC_ENABLE_XSUPTO, enableXsUpToCheck);
	DDX_Control(pDX, IDC_XSUPTO, xsUpToEdit);
	DDX_Text(pDX, IDC_MAXSOURCEPERFILE, maxSourcePerFile);
	DDX_Text(pDX, IDC_MAXSRC_LBL, maxSrcDesc);
	DDX_Text(pDX, IDC_AUTOSRC_MIN_PER_FILE, minAutoSourcesPerFile);
	DDX_Text(pDX, IDC_AUTOSRC_MAX_PER_FILE, maxAutoSourcesPerFile);
	DDX_Text(pDX, IDC_AUTOSRC_MAXTOTAL, maxAutoSourcesTotal);
	DDX_Text(pDX, IDC_AUTOSRC_SE_UPTO, maxAutoExchangeSources);
	DDX_Text(pDX, IDC_AUTOSRC_GRP_LBL, autosrcLblGrp);
	DDX_Text(pDX, IDC_AUTOSRC_FROM_LBL, autosrcLblFrom);
	DDX_Text(pDX, IDC_AUTOSRC_TO_LBL, autosrcLblTo);
	DDX_Text(pDX, IDC_AUTOSRC_MAXTOTAL_LBL, autosrcLblMaxtotal);
	DDX_Text(pDX, IDC_AUTOSRC_SE_UPTO_LBL, autosrcLblSeUpto);
	DDX_Text(pDX, IDC_XSSOURCES, xsSourcesDesc);
	DDX_Text(pDX, IDC_XSUPTO, xsUpTo);
}

BEGIN_MESSAGE_MAP(CPPgSources, CPropertyPage)
	ON_EN_CHANGE(IDC_MAXSOURCEPERFILE, OnSettingsChange)
	ON_EN_KILLFOCUS(IDC_MAXSOURCEPERFILE, OnSourcesChange)
	ON_BN_CLICKED(IDC_AUTOSRC_ENABLED, OnBnClickedAutoSources)
	ON_BN_CLICKED(IDC_DISABLE_XS, OnBnClickedDisableXS)
	ON_EN_CHANGE(IDC_AUTOSRC_MIN_PER_FILE, OnSettingsChange)
	ON_EN_CHANGE(IDC_AUTOSRC_MAX_PER_FILE, OnSettingsChange)
	ON_EN_CHANGE(IDC_AUTOSRC_MAXTOTAL, OnSettingsChange)
	ON_EN_CHANGE(IDC_AUTOSRC_SE_UPTO, OnSettingsChange)
	ON_BN_CLICKED(IDC_ENABLE_XSUPTO, OnBnClickedDisableXSUpTo)
	ON_EN_CHANGE(IDC_XSUPTO, OnSettingsChange)
END_MESSAGE_MAP()

BOOL CPPgSources::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	LoadSettings();
	Localize();

	return TRUE;
}

void CPPgSources::LoadSettings(void)
{
	if(m_pPrefs->GetMaxSourcePerFile() == 0xFFFF)
		maxSourcePerFile = _T("99999");
	else
		maxSourcePerFile.Format(_T("%d"), m_pPrefs->GetMaxSourcePerFile());

	autoSourcesEnabled = m_pPrefs->IsAutoSourcesEnabled();
	minAutoSourcesPerFile = m_pPrefs->GetMinAutoSourcesPerFile();
	maxAutoSourcesPerFile = m_pPrefs->GetMaxAutoSourcesPerFile();
	maxAutoSourcesTotal = m_pPrefs->GetMaxAutoSourcesTotal();
	maxAutoExchangeSources = m_pPrefs->GetMaxAutoExchangeSources();

	disableXS = m_pPrefs->IsDisabledXS();
	enableXSUpTo = m_pPrefs->DisableXSUpTo();
	xsUpTo.Format(_T("%d"), m_pPrefs->XSUpTo());

	UpdateData(FALSE);

	OnBnClickedAutoSources();
	OnBnClickedDisableXS();

	SetModified(FALSE);
}

BOOL CPPgSources::OnApply()
{
	if(m_bModified)
	{
		int	iVal, iMax;

		UpdateData(TRUE);

		if(!maxSourcePerFile.IsEmpty())
		{
			iVal = _tstoi(maxSourcePerFile);
			m_pPrefs->SetMaxSourcePerFile((iVal != 0) ? iVal : 10);
		}

		m_pPrefs->SetAutoSourcesEnabled(autoSourcesEnabled);
		m_pPrefs->SetMinAutoSourcesPerFile(minAutoSourcesPerFile);
		m_pPrefs->SetMaxAutoSourcesPerFile(maxAutoSourcesPerFile);
		m_pPrefs->SetMaxAutoSourcesTotal(maxAutoSourcesTotal);
		m_pPrefs->SetMaxAutoExchangeSources(maxAutoExchangeSources);

		m_pPrefs->SetDisabledXS(disableXS);
		m_pPrefs->SetDisableXSUpTo(enableXSUpTo);

		if ((iVal = _tstoi(xsUpTo)) > (iMax = m_pPrefs->GetMaxSourcePerFileSoft()))
			xsUpTo.Format(_T("%u"), iVal = iMax);

		m_pPrefs->SetXSUpTo(iVal);

		UpdateData(FALSE);
		SetModified(FALSE);
	}
	return CPropertyPage::OnApply();
}

void CPPgSources::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_DL_SOURCES));
		autoSourcesEnabledCheck.SetWindowText(GetResString(IDS_ENABLED));
		disableXsCheck.SetWindowText(GetResString(IDS_DISABLE_XS));
		enableXsUpToCheck.SetWindowText(GetResString(IDS_ENABLE_XSUPTO));

		GetResString(&autosrcLblGrp, IDS_AUTOSRC_GRP_LBL);
		GetResString(&autosrcLblFrom, IDS_AUTOSRC_FROM_LBL);
		GetResString(&autosrcLblTo, IDS_AUTOSRC_TO_LBL);
		GetResString(&autosrcLblMaxtotal, IDS_AUTOSRC_MAXTOTAL_LBL);
		GetResString(&autosrcLblSeUpto, IDS_AUTOSRC_SE_UPTO_LBL);
		GetResString(&maxSrcDesc, IDS_PW_MAXSOURCES);
		GetResString(&xsSourcesDesc, IDS_SOURCES_PF_FILES);

		UpdateData(FALSE);
	}
}

void CPPgSources::OnSourcesChange()
{
	UpdateData(TRUE);

	uint16 num = 10;
	if(!maxSourcePerFile.IsEmpty())
	{
		num = _tstoi(maxSourcePerFile);
		if (num < 10)
			num = 10;
	}
	if (num != m_pPrefs->GetMaxSourcePerFile())
	{
		maxSourcePerFile.Format(_T("%u"),num);
		UpdateData(FALSE);
	}
}

void CPPgSources::OnBnClickedAutoSources()
{
	UpdateData(TRUE);

	minAutoSourcesPerFileEdit.EnableWindow(autoSourcesEnabled);
	maxAutoSourcesPerFileEdit.EnableWindow(autoSourcesEnabled);
	maxAutoSourcesTotalEdit.EnableWindow(autoSourcesEnabled);
	maxAutoExchangeSourcesEdit.EnableWindow(autoSourcesEnabled);
	maxSourcePerFileEdit.EnableWindow(!autoSourcesEnabled);
	enableXsUpToCheck.EnableWindow(!autoSourcesEnabled && !disableXS);
	xsUpToEdit.EnableWindow(enableXSUpTo && !autoSourcesEnabled && !disableXS);

	SetModified();
}

void CPPgSources::OnBnClickedDisableXS()
{
	UpdateData(TRUE);

	if (disableXS)
	{
		enableXsUpToCheck.EnableWindow(!disableXS && !autoSourcesEnabled);
		xsUpToEdit.EnableWindow(!disableXS && !autoSourcesEnabled);
	}
	else
	{
		enableXsUpToCheck.EnableWindow(!disableXS && !autoSourcesEnabled);
		xsUpToEdit.EnableWindow(enableXSUpTo && !autoSourcesEnabled);
	}
	SetModified();
}

void CPPgSources::OnBnClickedDisableXSUpTo()
{
	UpdateData(TRUE);
	xsUpToEdit.EnableWindow(enableXSUpTo && !autoSourcesEnabled);

	SetModified();
}
