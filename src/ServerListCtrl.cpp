//this file is part of eMule
//Copyright (C)2002 Merkur ( merkur-@users.sourceforge.net / http://www.emule-project.net )
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
#include "ServerListCtrl.h"
#include "ServerList.h"
#include "server.h"
#include "TitleMenu.h"
#include "otherfunctions.h"
#include "IP2Country.h"
#include "NewServerDlg.h"
#ifdef OLD_SOCKETS_ENABLED
#include "sockets.h"
#endif
#include <share.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CServerListCtrl, CMuleListCtrl)

BEGIN_MESSAGE_MAP(CServerListCtrl, CMuleListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMLdblclk)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CServerListCtrl::CServerListCtrl()
{
	SetGeneralPurposeFind(true);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CServerListCtrl::Init()
{
	static const uint16 s_auIconResID[] =
	{
		IDI_SMALLSERVER,
		IDI_SMALLSERVERSTATIC,
		IDI_SERVERCONNECTED,
		IDI_SERVERCONNECTEDSTATIC,
		IDI_SERVERCONNECTEDFAILED,
		IDI_SERVERCONNECTEDFAILEDSTATIC,
		IDI_SERVERFAILED,
		IDI_SERVERFAILEDSTATIC
	};
	static const uint16 s_auColHdr[][2] =
	{
		{ LVCFMT_LEFT,  150 },	// SRVCOL_SERVERNAME
		{ LVCFMT_LEFT,  140 },	// SRVCOL_SERVERIP
		{ LVCFMT_LEFT,  150 },	// SRVCOL_DESCRIPTION
		{ LVCFMT_RIGHT,  50 },	// SRVCOL_PING
		{ LVCFMT_RIGHT,  50 },	// SRVCOL_NUMUSERS
		{ LVCFMT_RIGHT,  50 },	// SRVCOL_NUMFILES
		{ LVCFMT_LEFT,   60 },	// SRVCOL_PRIORITY
		{ LVCFMT_RIGHT,  50 },	// SRVCOL_FAILEDCOUNT
		{ LVCFMT_LEFT,   50 },	// SRVCOL_STATIC
		{ LVCFMT_RIGHT, 100 },	// SRVCOL_FILELIMIT
		{ LVCFMT_LEFT,  150 },	// SRVCOL_SOFTWAREVER
		{ LVCFMT_LEFT,  150 },	// SRVCOL_COUNTRY
		{ LVCFMT_RIGHT,  60 }	// SRVCOL_LOWIDUSERS
	};

	ModifyStyle(LVS_SINGLESEL, 0);

//	Set Imagelist
	m_imageList.Create(16, 16, g_App.m_iDfltImageListColorFlags | ILC_MASK, ARRSIZE(s_auIconResID), 0);
	m_imageList.SetBkColor(CLR_NONE);
	FillImgLstWith16x16Icons(&m_imageList, s_auIconResID, ARRSIZE(s_auIconResID));
	SetImageList(&m_imageList, LVSIL_SMALL);

	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (unsigned ui = 0; ui < ARRSIZE(s_auColHdr); ui++)
		InsertColumn(ui, _T(""), static_cast<int>(s_auColHdr[ui][0]), static_cast<int>(s_auColHdr[ui][1]));

	LoadSettings(CPreferences::TABLE_SERVER);

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CServerListCtrl::~CServerListCtrl()
{}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::OnDestroy()
{
	m_imageList.DeleteImageList();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SortFirstInit() starts initial sorting after all initial information is added w/o sorting.
//	This is faster than adding every item with sorting rules.
void CServerListCtrl::SortFirstInit()
{
	static const int s_aiDoubleLevelCols[] = { SRVCOL_NUMUSERS, SRVCOL_FILELIMIT };

	SetSortProcedure(SortProc);
	SetDoubleLayerSort(s_aiDoubleLevelCols, ARRSIZE(s_aiDoubleLevelCols));
	if (g_App.m_pPrefs->DoUseSort())
		SortInit(g_App.m_pPrefs->GetServerSortCol());
	else
		SortInit(g_App.m_pPrefs->GetColumnSortCode(CPreferences::TABLE_SERVER));
	ShowFilesCount();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::Localize()
{
	static const uint16 s_auResTbl[] =
	{
		IDS_SL_SERVERNAME,			//SRVCOL_SERVERNAME
		IDS_IP,						//SRVCOL_SERVERIP
		IDS_DESCRIPTION,			//SRVCOL_DESCRIPTION
		IDS_PING,					//SRVCOL_PING
		IDS_UUSERS,					//SRVCOL_NUMUSERS
		IDS_FILES,					//SRVCOL_NUMFILES
		IDS_PRIORITY,				//SRVCOL_PRIORITY
		IDS_UFAILED,				//SRVCOL_FAILEDCOUNT
		IDS_STATICSERVER,			//SRVCOL_STATIC
		IDS_SERVER_SOFTHARDLIMIT,	//SRVCOL_FILELIMIT
		IDS_SERVER_VERSION,			//SRVCOL_SOFTWAREVER
		IDS_COUNTRY,				//SRVCOL_COUNTRY
		IDS_LOWIDUSERS				//SRVCOL_LOWIDUSERS
	};

	if (GetSafeHwnd())
	{
		CHeaderCtrl	*pHeaderCtrl = GetHeaderCtrl();
		CString		strRes;
		HDITEM		hdi;

		hdi.mask = HDI_TEXT;

		for (unsigned ui = 0; ui < ARRSIZE(s_auResTbl); ui++)
		{
			::GetResString(&strRes, static_cast<UINT>(s_auResTbl[ui]));
			if (ui == SRVCOL_NUMUSERS)
				strRes.AppendFormat(_T(" (%s)"), GetResString(IDS_MAXUSERS));
			hdi.pszText = const_cast<LPTSTR>(strRes.GetString());
			pHeaderCtrl->SetItem(static_cast<int>(ui), &hdi);
		}

		for (POSITION pos = g_App.m_pServerList->m_serverList.GetHeadPosition(); pos != NULL;)
		{
			CServer*	pServer = g_App.m_pServerList->m_serverList.GetNext(pos);
			int			iIndex = FindIndex(pServer);

			if (iIndex != -1)
				RefreshServerDesc(iIndex, *pServer);
		}
		ShowFilesCount();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CServerListCtrl::FindIndex(const CServer *pServer) const
{
	LVFINDINFO	find;

	memzero(&find, sizeof(LVFINDINFO));
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)pServer;

	return FindItem(&find);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	RefreshServerDesc() refreshes the server description for the 'iIndex'th server from 'server'.
void CServerListCtrl::RefreshServerDesc(int iIndex, const CServer &server)
{
	CServer		*cur_srv;
	int			iImageIndex = 0;
	CString		strTemp;
	UINT		dwResStrId, iItemState = 0;

//	Disable list sorting, position will be updated either by 'resort' or by last SetItemText()
	SetSortProcedure(NULL);

	if ( g_App.m_pServerConnect->IsConnected()
		&& (cur_srv = g_App.m_pServerConnect->GetCurrentServer()) != NULL
		&& cur_srv->GetPort() == server.GetPort()
		&& _tcsicmp(cur_srv->GetAddress(), server.GetAddress()) == 0 )
	{
		iImageIndex = (server.GetFailedCount() == 0) ? 2 : 4;
		iItemState |= LVIS_GLOW;
	}
	else if (server.GetFailedCount() > 0)
	{
		iImageIndex = 6;
		if (!server.IsStaticMember())
			iItemState |= LVIS_CUT;
	}

	if (server.IsStaticMember())
		iImageIndex++;

	SetItem( iIndex, SRVCOL_SERVERNAME, LVIF_TEXT | LVIF_IMAGE | LVIF_STATE,
		server.GetListName(), iImageIndex, iItemState, LVIS_GLOW | LVIS_CUT, 0, 0 );

	if (server.GetAuxPort() != 0)
		strTemp.Format(_T("%s:%u (%u)"), server.GetAddress(), server.GetPort(), server.GetAuxPort());
	else
		strTemp.Format(_T("%s:%u"), server.GetAddress(), server.GetPort());
	SetItemText(iIndex, SRVCOL_SERVERIP, strTemp);

	SetItemText(iIndex, SRVCOL_DESCRIPTION, server.GetDescription());

	if (server.GetPing() != 0)
	{
		strTemp.Format(_T("%u"), server.GetPing());
		SetItemText(iIndex, SRVCOL_PING, strTemp);
	}
	else
		SetItemText(iIndex, SRVCOL_PING, _T(""));

	if (server.GetNumUsers() != 0)
		strTemp.Format(_T("%u"), server.GetNumUsers());
	else
		strTemp = _T("");

	if (server.GetMaxUsers())
		strTemp.AppendFormat(_T(" (%u)"), server.GetMaxUsers());
	SetItemText(iIndex, SRVCOL_NUMUSERS, strTemp);

	if (server.GetFiles() != 0)
	{
		strTemp.Format(_T("%u"), server.GetFiles());
		SetItemText(iIndex, SRVCOL_NUMFILES, strTemp);
	}
	else
		SetItemText(iIndex, SRVCOL_NUMFILES, _T(""));

	switch (server.GetPreferences())
	{
		case PR_LOW:
			dwResStrId = IDS_PRIOLOW;
			break;
		case PR_NORMAL:
			dwResStrId = IDS_PRIONORMAL;
			break;
		case PR_HIGH:
			dwResStrId = IDS_PRIOHIGH;
			break;
		default:
			dwResStrId = IDS_PRIONOPREF;
	}
	GetResString(&strTemp, dwResStrId);
	SetItemText(iIndex, SRVCOL_PRIORITY, strTemp);

	strTemp.Format(_T("%u"), server.GetFailedCount());
	SetItemText(iIndex, SRVCOL_FAILEDCOUNT, strTemp);

	SetItemText(iIndex, SRVCOL_STATIC, YesNoStr(server.IsStaticMember()));

	if (server.GetHardMaxFiles() != 0)
	{
		if (server.GetSoftMaxFiles() != 0)
			strTemp.Format(_T("%u (%u)"), server.GetSoftMaxFiles(), server.GetHardMaxFiles());
		else
			strTemp.Format(_T("(%u)"), server.GetHardMaxFiles());
		SetItemText(iIndex, SRVCOL_FILELIMIT, strTemp);
	}
	else
		SetItemText(iIndex, SRVCOL_FILELIMIT, _T(""));

	SetItemText(iIndex, SRVCOL_SOFTWAREVER, server.GetVersion());
	SetItemText(iIndex, SRVCOL_COUNTRY, server.GetCountryName());

	SetSortProcedure(SortProc);	//	Enable list sorting

	if (server.GetLowIDUsers() != 0)
	{
		strTemp.Format(_T("%u"), server.GetLowIDUsers());
		SetItemText(iIndex, SRVCOL_LOWIDUSERS, strTemp);
	}
	else
		SetItemText(iIndex, SRVCOL_LOWIDUSERS, _T(""));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::RefreshAllServer()
{
	for(POSITION pos = g_App.m_pServerList->m_serverList.GetHeadPosition(); pos != NULL;)
	{
		CServer	*pServer = g_App.m_pServerList->m_serverList.GetAt(pos);

		RefreshServer(*pServer);
		g_App.m_pServerList->m_serverList.GetNext(pos);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::RemoveServer(CServer *pServer)
{
	int	iIndex = FindIndex(pServer);

	if (iIndex != -1)
	{
		g_App.m_pServerList->RemoveServer(GetServerAt(iIndex));
		DeleteItem(iIndex);
	}
	ShowFilesCount();
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Remove Dead Servers
void CServerListCtrl::RemoveDeadServer()
{
	if (g_App.m_pPrefs->DeadServer())
	{
		SetRedraw(FALSE);

		for (POSITION pos = g_App.m_pServerList->m_serverList.GetHeadPosition(); pos != NULL; g_App.m_pServerList->m_serverList.GetNext(pos))
		{
			CServer	* pServer = g_App.m_pServerList->m_serverList.GetAt(pos);

			if ( pServer->GetFailedCount() >= g_App.m_pPrefs->GetDeadserverRetries()
				&& !pServer->IsStaticMember() )
			{
				RemoveServer(pServer);
				pos = g_App.m_pServerList->m_serverList.GetHeadPosition();
			}
		}
		SetRedraw(TRUE);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::AppendServer(CServer *pServer)
{
	if (::IsWindow(GetSafeHwnd()))
	{
		int	iNumServers = GetItemCount();
		int	iIndex = 0;

		iIndex = InsertItem(LVIF_PARAM, iNumServers, NULL, 0, 0, 1, reinterpret_cast<LPARAM>(pServer));
		RefreshServerDesc(iIndex, *pServer);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CServerListCtrl::AddServer(CServer *pServer, bool bAddToList, bool bBulkLoad, bool bChangeServerInfo/* = false*/)
{
	if (pServer == NULL)
		return false;

	if (!g_App.m_pServerList->AddServer(pServer, bChangeServerInfo))
		return false;

	if (bAddToList && !bChangeServerInfo)
		AppendServer(pServer);
	if (!bBulkLoad)
		ShowFilesCount();
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::RefreshServer(CServer &server)
{
	if (!g_App.m_pMDlg->IsRunning())
		return;

	if (!::IsWindow(m_hWnd))
		return;

	int	iIndex = FindIndex(&server);

	if (iIndex == -1)
		return;
	RefreshServerDesc(iIndex, server);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	CServerListCtrl message handlers

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::OnContextMenu(CWnd *pWnd, CPoint point)
{
//	On right click, we also want to change the current selection like the left click does
	CPoint	p = point;
	NOPRM(pWnd);

	ScreenToClient(&p);

	int	iIndex = HitTest(p);

	if (iIndex != -1)
		SetSelectionMark(iIndex);

//	Create up-to-date popupmenu
	UINT	iMenuFlags;
	CTitleMenu	m_ServerMenu;
	CMenu	m_ServerPrioMenu;

	int			iSelectionMark = GetSelectionMark();
	CServer		*pServer = (iSelectionMark != -1) ? GetSelectedServer() : NULL;
	UINT		dwSelectedCount = GetSelectedCount();
	bool		bSelectedSrv = (dwSelectedCount > 0) && (pServer != NULL);

//	Set state of selection-dependent menuitems
	iMenuFlags = MF_STRING | (((iSelectionMark != -1) && bSelectedSrv) ? 0 : MF_GRAYED);

	m_ServerPrioMenu.CreateMenu();

	bool		bJustOne = (dwSelectedCount == 1);

	m_ServerPrioMenu.AppendMenu( MF_STRING |
		((bJustOne && pServer->GetPreferences() == PR_LOW) ? MF_CHECKED : MF_UNCHECKED),
		MP_PRIOLOW, GetResString(IDS_PRIOLOW) );
	m_ServerPrioMenu.AppendMenu( MF_STRING |
		((bJustOne && pServer->GetPreferences() == PR_NORMAL) ? MF_CHECKED : MF_UNCHECKED),
		MP_PRIONORMAL, GetResString(IDS_PRIONORMAL) );
	m_ServerPrioMenu.AppendMenu( MF_STRING |
		((bJustOne && pServer->GetPreferences() == PR_HIGH) ? MF_CHECKED : MF_UNCHECKED),
		MP_PRIOHIGH, GetResString(IDS_PRIOHIGH) );

	m_ServerMenu.CreatePopupMenu();
	m_ServerMenu.AddMenuTitle(GetResString(IDS_SERVERS));
	m_ServerMenu.AppendMenu(iMenuFlags, MP_CONNECTTO, GetResString(IDS_CONNECTTHIS));
	m_ServerMenu.AppendMenu(iMenuFlags | MF_POPUP, (UINT_PTR)m_ServerPrioMenu.m_hMenu, GetResString(IDS_PRIORITY));

//	Hide greyed-out "add /remove from static server"
	if (bSelectedSrv)
	{
		if (pServer->IsStaticMember())
			m_ServerMenu.AppendMenu(iMenuFlags, MP_REMOVEFROMSTATIC, GetResString(IDS_REMOVEFROMSTATIC));
		else
			m_ServerMenu.AppendMenu(iMenuFlags, MP_ADDTOSTATIC, GetResString(IDS_ADDTOSTATIC));
	}

	m_ServerMenu.AppendMenu(MF_SEPARATOR);
	m_ServerMenu.AppendMenu( iMenuFlags |
		(((dwSelectedCount != 1) || pServer->IsStaticMember()) ? MF_GRAYED : MF_ENABLED),
		MP_EDIT, GetResString(IDS_FNCEDIT) );
	m_ServerMenu.AppendMenu(iMenuFlags, MP_REMOVE, GetResString(IDS_REMOVETHIS));
	m_ServerMenu.AppendMenu(MF_SEPARATOR);
	m_ServerMenu.AppendMenu(iMenuFlags, MP_GETED2KLINK, GetStringFromShortcutCode(IDS_DL_LINK1, SCUT_LINK, SSP_TAB_PREFIX));

	m_ServerMenu.SetDefaultItem(MP_CONNECTTO);
	m_ServerMenu.TrackPopupMenuEx(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

//	Menu objects are destroyed in their destructor
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CServerListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int	iIndex = GetSelectionMark();
	NOPRM(lParam);

	if (iIndex != -1)
	{
		if (GetSelectedServer() != NULL)
		{
			switch (wParam)
			{
				case MP_CONNECTTO:
				{
					if (GetSelectedCount() > 1)
					{
						CServer	*pServer;
#ifdef OLD_SOCKETS_ENABLED

						g_App.m_pServerConnect->Disconnect();
#endif

						POSITION	pos = GetFirstSelectedItemPosition();

					//	Move selected servers to the start of the list, to connect them first
						while (pos != NULL)
						{
							if ((iIndex = GetNextSelectedItem(pos)) >= 0)
							{
								pServer = GetServerAt(iIndex);
								g_App.m_pServerList->MoveServerDown(pServer);
							}
						}
#ifdef OLD_SOCKETS_ENABLED
						g_App.m_pServerConnect->ConnectToAnyServer(g_App.m_pServerList->GetServerCount() - GetSelectedCount(), false);
#else
						g_App.m_pEngine->ConnectToAnyServer();
#endif
					}
					else
					{
#ifdef OLD_SOCKETS_ENABLED
						if (g_App.m_pServerConnect->IsConnected())
						{
							if ( !_tcscmp( GetSelectedServer()->GetAddress(),
								g_App.m_pServerConnect->GetCurrentServer()->GetAddress() ) )
							{
								AddLogLine(0, IDS_ALREADYCONNECTED, GetSelectedServer()->GetListName());
								break;
							}
						}
						g_App.m_pServerConnect->ConnectToServer(GetSelectedServer(), false);
#endif
#ifdef NEW_SOCKETS
						CServer	*pServer = GetSelectedServer();

						if (pServer != NULL)
							g_App.m_pEngine->ConnectToServer(pServer);
#endif
					}
					g_App.m_pMDlg->ShowConnectionState(false);
					break;
				}
				case MP_EDIT:
				{
					CServer	*pServer = GetSelectedServer();
					CNewServerDlg dlgEditServer;
					CString strPort;
					CString strAuxPort;

					dlgEditServer.SetParent(&g_App.m_pMDlg->m_wndServer);
					dlgEditServer.SetServerEditMode();
					strPort.Format(_T("%u"), pServer->GetPort());
					strAuxPort.Format(_T("%u"), pServer->GetAuxPort());
					dlgEditServer.SetLabels(pServer->GetAddress(), strPort, pServer->GetListName(), strAuxPort);
					if (dlgEditServer.DoModal() == IDOK)
					{
						RefreshServer(*pServer);
					}
					break;
				}
				case MP_REMOVE:
				{
					SetRedraw(FALSE);

					POSITION	pos;

					while (GetFirstSelectedItemPosition() != NULL)
					{
						pos = GetFirstSelectedItemPosition();
						iIndex = GetNextSelectedItem(pos);
						g_App.m_pServerList->RemoveServer(GetServerAt(iIndex));
						DeleteItem(iIndex);
					}
					ShowFilesCount();
					SetRedraw(TRUE);
					break;
				}
				case MP_ADDTOSTATIC:
				{
					POSITION	pos = GetFirstSelectedItemPosition();

					while (pos != NULL)
					{
						CServer	*pServer = GetServerAt(GetNextSelectedItem(pos));

						if (!StaticServerFileAppend(pServer))
							return false;
					}
					break;
				}
				case MP_REMOVEFROMSTATIC:
				{
					POSITION	pos = GetFirstSelectedItemPosition();

					while (pos != NULL)
					{
						CServer	*pServer = GetServerAt(GetNextSelectedItem(pos));

						if (!StaticServerFileRemove(pServer))
							return false;
					}
					break;
				}
				case MP_PRIOLOW:
					SetSelectedServersPriority(PR_LOW);
					break;

				case MP_PRIONORMAL:
					SetSelectedServersPriority(PR_NORMAL);
					break;

				case MP_PRIOHIGH:
					SetSelectedServersPriority(PR_HIGH);
					break;

				case MP_GETED2KLINK:
				{
					POSITION	pos = GetFirstSelectedItemPosition();
					CString	strBuffer, strLink;

					while (pos != NULL)
					{
						CServer	*pServer = GetServerAt(GetNextSelectedItem(pos));

						strBuffer.Format(_T("ed2k://|server|%s|%u|/"), pServer->GetAddress(), pServer->GetPort());
						if (strLink.GetLength() > 0)
							strLink += _T("\r\n");
						strLink += strBuffer;
					}
					g_App.CopyTextToClipboard(strLink);
					break;
				}
			}
		}
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::SetSelectedServersPriority(unsigned uiPriority)
{
	POSITION	pos = GetFirstSelectedItemPosition();

	while (pos != NULL)
	{
		CServer	*pServer = GetServerAt(GetNextSelectedItem(pos));

		pServer->SetPreference(static_cast<byte>(uiPriority));
		if (pServer->IsStaticMember())
			StaticServerFileAppend(pServer);	// RefreshServer is called inside
		else
			RefreshServer(*pServer);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	OnNMLdblclk() is the left-double-click message handler
void CServerListCtrl::OnNMLdblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	NOPRM(pNMHDR); NOPRM(pResult);
	if (GetSelectionMark() != -1)
	{
#ifdef OLD_SOCKETS_ENABLED
		if (g_App.m_pServerConnect->IsConnected())
		{
			if ( !_tcscmp( GetSelectedServer()->GetAddress(),
				(g_App.m_pServerConnect->GetCurrentServer())->GetAddress() ) )
			{
				AddLogLine(0, IDS_ALREADYCONNECTED, GetSelectedServer()->GetListName());
				return;
			}
		}
		g_App.m_pServerConnect->ConnectToServer(GetSelectedServer(), false);
#endif //OLD_SOCKETS_ENABLED
#ifdef NEW_SOCKETS

		CServer	*pServer = GetSelectedServer();

		if (pServer != NULL)
			g_App.m_pEngine->ConnectToServer(pServer);
#else
		g_App.m_pMDlg->ShowConnectionState(false);
#endif //NEW_SOCKETS
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CServerListCtrl::SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CServer	*pServer1 = reinterpret_cast<CServer*>(lParam1);
	CServer	*pServer2 = reinterpret_cast<CServer*>(lParam2);

	if (pServer1 == NULL || pServer2 == NULL)
		return 0;

	uint32	dwVal1, dwVal2;
	int		iCompare = 0;
	int		iSortColumn = (lParamSort & MLC_COLUMNMASK);
	int		iSortAltFlag = (lParamSort & MLC_SORTALT);
	int		iSortMod = ((lParamSort & MLC_SORTDESC) == 0) ? 1 : -1;

	for (;;)
	{
		switch (iSortColumn)
		{
			case SRVCOL_SERVERNAME:
			{
				bool	bServerHasNoName1 = !pServer1->HasServerName();
				bool	bServerHasNoName2 = !pServer2->HasServerName();

				if (!bServerHasNoName1 && !bServerHasNoName2)
					iCompare = _tcsicmp(pServer1->GetListName(), pServer2->GetListName());
				else
					iCompare = bServerHasNoName1 ? (bServerHasNoName2 ? 0 : 1) : -1;

				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERIP;
					continue;
				}
				break;
			}

			case SRVCOL_SERVERIP:
			{
				if (pServer1->HasDynIP() && pServer2->HasDynIP())	//	both are dynamic IP...
					iCompare = _tcsicmp(pServer1->GetDynIP(), pServer2->GetDynIP());
				else if (pServer1->HasDynIP())	//	only the first is dynamic IP...
					iCompare = -1;
				else if (pServer2->HasDynIP())	//	only the second is dynamic IP...
					iCompare = 1;
				else	//	both have static IPs...
				{
					dwVal1 = fast_htonl(pServer1->GetIP());
					dwVal2 = fast_htonl(pServer2->GetIP());
					if (dwVal1 < dwVal2)
						iCompare = -1;
					else if (dwVal1 > dwVal2)
						iCompare = 1;
					else
						iCompare = pServer1->GetPort() - pServer2->GetPort();
				}
				break;
			}

			case SRVCOL_DESCRIPTION:
			{
				{	//	required to optimize string destructors
					CString strServerDescription1 = pServer1->GetDescription();
					CString strServerDescription2 = pServer2->GetDescription();

					if (!strServerDescription1.IsEmpty() && !strServerDescription2.IsEmpty())
						iCompare = _tcsicmp(strServerDescription1, strServerDescription2);
					else
					{
						iCompare = strServerDescription1.IsEmpty() ? (strServerDescription2.IsEmpty() ? 0 : 1) : -1;
						iSortMod = 1;		//empty entries at the bottom in any case
					}
				}

				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_PING:
			{
				iCompare = pServer1->GetPing() - pServer2->GetPing();
				if (!pServer1->GetPing() || !pServer2->GetPing())
					iSortMod = -1;
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_NUMUSERS:
			{
				if (iSortAltFlag == 0)
				{
					dwVal1 = pServer1->GetNumUsers();
					dwVal2 = pServer2->GetNumUsers();
				}
				else	//	max. users
				{
					dwVal1 = pServer1->GetMaxUsers();
					dwVal2 = pServer2->GetMaxUsers();
				}
				iCompare = dwVal1 - dwVal2;
				if ((dwVal1 == 0) || (dwVal2 == 0))
					iSortMod = -1;
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_NUMFILES:
			{
				iCompare = pServer1->GetFiles() - pServer2->GetFiles();
				if (!pServer1->GetFiles() || !pServer2->GetFiles())
					iSortMod = -1;
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_PRIORITY:
			{
				iCompare = pServer1->GetPreferences() - pServer2->GetPreferences();
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_FAILEDCOUNT:
			{
				iCompare = pServer1->GetFailedCount() - pServer2->GetFailedCount();
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_STATIC:
			{
				iCompare = pServer1->IsStaticMember() - pServer2->IsStaticMember();
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_FILELIMIT:
			{
				if (iSortAltFlag == 0)
				{
					dwVal1 = pServer1->GetSoftMaxFiles();
					dwVal2 = pServer2->GetSoftMaxFiles();
				}
				else	//	hard files
				{
					dwVal1 = pServer1->GetHardMaxFiles();
					dwVal2 = pServer2->GetHardMaxFiles();
				}
				iCompare = dwVal1 - dwVal2;
				if ((dwVal1 == 0) || (dwVal2 == 0))
					iSortMod = -1;
				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_SOFTWAREVER:
			{
				{	//	required to optimize string destructors
					CString strServerVersion1 = pServer1->GetVersion();
					CString strServerVersion2 = pServer2->GetVersion();

					if (!strServerVersion1.IsEmpty() && !strServerVersion2.IsEmpty())
						iCompare = _tcsicmp(strServerVersion1, strServerVersion2);
					else
					{
						iCompare = strServerVersion1.IsEmpty() ? (strServerVersion2.IsEmpty() ? 0 : 1) : -1;
						iSortMod = 1;		//empty entries at the bottom in any case
					}
				}

				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//	sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_COUNTRY:
			{
				{	//	required to optimize string destructors
					CString strServerCountry1 = pServer1->GetCountryName();
					CString strServerCountry2 = pServer2->GetCountryName();

					if (!strServerCountry1.IsEmpty() && !strServerCountry2.IsEmpty())
						iCompare = _tcsicmp(strServerCountry1, strServerCountry2);
					else
					{
						iCompare = strServerCountry1.IsEmpty() ? (strServerCountry2.IsEmpty() ? 0 : 1) : -1;
						iSortMod = 1;		//empty entries at the bottom in any case
					}
				}

				if (iCompare == 0)
				{
					iSortColumn = SRVCOL_SERVERNAME;
					iSortMod = 1;		//sort always in ascending order
					continue;
				}
				break;
			}

			case SRVCOL_LOWIDUSERS:
				iCompare = pServer1->GetLowIDUsers() - pServer2->GetLowIDUsers();
				if ((pServer1->GetLowIDUsers() == 0) || (pServer2->GetLowIDUsers() == 0))
					iSortMod = -1;
				break;
		}
		break;
	}

	return iCompare * iSortMod;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	StaticServerFileAppend() - save a server as a static one
//	Note: this function can be also called for already static servers to preserve
//	changed properties in the configuration file
bool CServerListCtrl::StaticServerFileAppend(CServer *pServer)
{
	bool	bStatic = pServer->IsStaticMember();

	pServer->SetIsStaticMember(true);

	if (!g_App.m_pServerList->SaveServersToTextFile())
	{
		if (!bStatic)	// revert to the previous state only 
			pServer->SetIsStaticMember(false);
		AddLogLine(LOG_RGB_ERROR, IDS_ERROR_SAVEFILE, CFGFILE_STATICSERVERS);
		return false;
	}

	AddLogLine(0, _T("'%s:%u,%s' %s"), pServer->GetAddress(), pServer->GetPort(), pServer->GetListName(), GetResString(IDS_ADDED2SSF));
	RefreshServer(*pServer);

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CServerListCtrl::StaticServerFileRemove(CServer *pServer)
{
	if (!pServer->IsStaticMember())
		return true;

	pServer->SetIsStaticMember(false);

	if (!g_App.m_pServerList->SaveServersToTextFile())
	{
		pServer->SetIsStaticMember(true);
		AddLogLine(LOG_RGB_ERROR, IDS_ERROR_SAVEFILE, CFGFILE_STATICSERVERS);
		return false;
	}

	AddLogLine(0, _T("'%s:%u,%s' %s"), pServer->GetAddress(), pServer->GetPort(), pServer->GetListName(), GetResString(IDS_REMOVEDFROMSSF));
	RefreshServer(*pServer);

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CServerListCtrl::ShowFilesCount()
{
	CString	strTitle;

	strTitle.Format(_T("%s (%u)"), GetResString(IDS_SERVERS), GetItemCount());
	g_App.m_pMDlg->m_wndServer.SetDlgItemText(IDC_SERVLIST_TEXT, strTitle);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CServerListCtrl::PreTranslateMessage(MSG *pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_SYSKEYDOWN))
	{
		POSITION	posSelClient = GetFirstSelectedItemPosition();

		if (posSelClient != NULL)
		{
			int		iMessage = 0;
			short	nCode = GetCodeFromPressedKeys(pMsg);

			if (nCode == g_App.m_pPrefs->GetShortcutCode(SCUT_LINK))
				iMessage = MP_GETED2KLINK;

			if (iMessage > 0)
			{
				PostMessage(WM_COMMAND, static_cast<WPARAM>(iMessage));
				return TRUE;
			}
		}
	}

	return CMuleListCtrl::PreTranslateMessage(pMsg);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////