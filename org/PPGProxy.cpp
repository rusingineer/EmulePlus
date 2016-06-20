//this file is part of eMule
//Copyright (C)2002-2006 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
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

#include "stdafx.h"
#include "emule.h"
#include "PPGProxy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CPPGProxy dialog

IMPLEMENT_DYNAMIC(CPPgProxy, CPropertyPage)
CPPgProxy::CPPgProxy()
	: CPropertyPage(CPPgProxy::IDD)
	, enableProxy(FALSE)
	, enableAuth(FALSE)
{
}

CPPgProxy::~CPPgProxy()
{
}

void CPPgProxy::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENABLEPROXY, enableProxyCheck);
	DDX_Control(pDX, IDC_ENABLEAUTH, enableAuthCheck);
	DDX_Control(pDX, IDC_PROXYTYPE, proxyTypeCombo);
	DDX_Control(pDX, IDC_PROXYNAME, proxyNameEdit);
	DDX_Control(pDX, IDC_PROXYPORT, proxyPortEdit);
	DDX_Control(pDX, IDC_USERNAME, userNameEdit);
	DDX_Control(pDX, IDC_PASSWORD, passwordEdit);
	DDX_Check(pDX, IDC_ENABLEPROXY, enableProxy);
	DDX_Check(pDX, IDC_ENABLEAUTH, enableAuth);
	DDX_Text(pDX, IDC_PROXYPORT, proxyPort);
	DDX_Text(pDX, IDC_USERNAME, proxyUser);
	DDX_Text(pDX, IDC_PASSWORD, proxyPassword);
	DDX_Text(pDX, IDC_PROXYNAME, proxyName);
	DDX_Text(pDX, IDC_PROXYTYPE_LBL, proxyTypeDesc);
	DDX_Text(pDX, IDC_PROXYNAME_LBL, proxyNameDesc);
	DDX_Text(pDX, IDC_PROXYPORT_LBL, proxyPortDesc);
	DDX_Text(pDX, IDC_USERNAME_LBL, userNameDesc);
	DDX_Text(pDX, IDC_PASSWORD_LBL, passwordDesc);
	DDX_Text(pDX, IDC_AUTH_LBL, authDesc);
}


BEGIN_MESSAGE_MAP(CPPgProxy, CPropertyPage)
	ON_BN_CLICKED(IDC_ENABLEPROXY, OnBnClickedEnableproxy)
	ON_BN_CLICKED(IDC_ENABLEAUTH, OnBnClickedEnableauth)
	ON_CBN_SELCHANGE(IDC_PROXYTYPE, OnCbnSelchangeProxytype)
	ON_EN_CHANGE(IDC_PROXYNAME, OnEnChangeProxyname)
	ON_EN_CHANGE(IDC_PROXYPORT, OnEnChangeProxyport)
	ON_EN_CHANGE(IDC_USERNAME, OnEnChangeUsername)
	ON_EN_CHANGE(IDC_PASSWORD, OnEnChangePassword)
END_MESSAGE_MAP()


BOOL CPPgProxy::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	proxyTypeCombo.AddString(_T("SOCKS4"));
	proxyTypeCombo.AddString(_T("SOCKS4a"));
	proxyTypeCombo.AddString(_T("SOCKS5"));
	proxyTypeCombo.AddString(_T("HTTP 1.1"));

	proxyPortEdit.SetLimitText(5);
	proxy = m_pPrefs->GetProxySettings();
	LoadSettings();
	Localize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPgProxy::OnApply()
{
	UpdateData(TRUE);

	proxy.m_bUseProxy = enableProxy;
	proxy.m_bEnablePassword = enableAuth;
	proxy.m_nType = proxyTypeCombo.GetCurSel() + 1;	//	PROXYTYPE_NOPROXY not in the list
	
	proxy.m_strName = proxyName;
	if (proxyName.IsEmpty())
		proxy.m_bUseProxy = false;

	unsigned	uiVal = _tstoi(proxyPort);
	
//	Valid values range is 1..65535
	if ((uiVal - 1u) > 0xFFFEu)
		uiVal = PREF_DEF_PROXY_PORT;
	proxy.m_uPort = static_cast<uint16>(uiVal);
	
	proxy.m_strUser = proxyUser;
	if (proxyUser.IsEmpty())
		proxy.m_bEnablePassword = false;
		
	proxy.m_strPassword = proxyPassword;
	if (proxyPassword.IsEmpty())
		proxy.m_bEnablePassword = false;
	
	m_pPrefs->SetProxySettings(proxy);
	LoadSettings();
	return TRUE;
}

void CPPgProxy::OnBnClickedEnableproxy()
{
	SetModified(true);
	UpdateData(TRUE);
		
	enableAuthCheck.EnableWindow(enableProxy);
	proxyTypeCombo.EnableWindow(enableProxy);
	proxyNameEdit.EnableWindow(enableProxy);
	proxyPortEdit.EnableWindow(enableProxy);
	userNameEdit.EnableWindow(enableProxy);
	passwordEdit.EnableWindow(enableProxy);
	if (enableProxy)
	{
		OnBnClickedEnableauth();
		OnCbnSelchangeProxytype();
	}
}

void CPPgProxy::OnBnClickedEnableauth()
{
	SetModified(true);
	UpdateData(TRUE);

	userNameEdit.EnableWindow(enableAuth);
	passwordEdit.EnableWindow(enableAuth);
}

void CPPgProxy::OnCbnSelchangeProxytype()
{
	SetModified(true);

	if (!(proxyTypeCombo.GetCurSel() == PROXYTYPE_SOCKS5 || proxyTypeCombo.GetCurSel() == PROXYTYPE_HTTP11))
	{
		enableAuth = false;
		UpdateData(TRUE);
		OnBnClickedEnableauth();
		enableAuthCheck.EnableWindow(false);
	} 
	else
		enableAuthCheck.EnableWindow(true);
}

void CPPgProxy::LoadSettings()
{
	enableProxy = proxy.m_bUseProxy;
	enableAuth = proxy.m_bEnablePassword;
	proxyTypeCombo.SetCurSel(proxy.m_nType - 1);	//	PROXYTYPE_NOPROXY not in the list
	proxyName = proxy.m_strName;
	proxyPort.Format(_T("%u"), proxy.m_uPort);
	proxyUser = proxy.m_strUser;
	proxyPassword = proxy.m_strPassword;
	UpdateData(FALSE);
	OnBnClickedEnableproxy();
}

void CPPgProxy::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_PW_PROXY));
		enableProxyCheck.SetWindowText(GetResString(IDS_PROXY_ENABLED));	
		GetResString(&proxyTypeDesc, IDS_PROXY_TYPE);	
		GetResString(&proxyNameDesc, IDS_PROXY_ADDRESS);	
		GetResString(&proxyPortDesc, IDS_PROXY_PORT);	
		enableAuthCheck.SetWindowText(GetResString(IDS_PROXY_AUTH_ENABLED));	
		GetResString(&userNameDesc, IDS_PROXY_USERNAME);	
		GetResString(&passwordDesc, IDS_PASSWORD);	
		GetResString(&authDesc, IDS_PROXY_AUTH);	
		UpdateData(FALSE);
	}
}
