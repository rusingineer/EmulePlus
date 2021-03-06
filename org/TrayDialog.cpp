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

// TrayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "TrayDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTrayDialog dialog


CTrayDialog::CTrayDialog(UINT uIDD,CWnd* pParent /*=NULL*/)
	: CTrayDialogBase(uIDD, pParent)
{
	m_nidIconData.cbSize			= sizeof(NOTIFYICONDATA);
	ASSERT( m_nidIconData.cbSize == NOTIFYICONDATA_V1_SIZE );
	
	m_nidIconData.hWnd				= 0;
	m_nidIconData.uID				= 1;

	m_nidIconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;

	m_nidIconData.hIcon				= 0;
	m_nidIconData.szTip[0]			= 0;	
	m_nidIconData.uFlags			= NIF_MESSAGE;

	m_bTrayIconVisible				= false;
	m_bMinimizeToTray				= 0;
	m_nDefaultMenuItem				= 0;
	m_hPrevIconDelete				= NULL;
	m_bCurIconDelete				= false;
	m_bdoubleclicked				= false;
}


const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

BEGIN_MESSAGE_MAP(CTrayDialog, CTrayDialogBase)
	//{{AFX_MSG_MAP(CTrayDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED,OnTaskBarCreated)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayDialog message handlers

int CTrayDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTrayDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ASSERT( WM_TASKBARCREATED );
	m_nidIconData.hWnd = m_hWnd;
	m_nidIconData.uID = 1;
	
	return 0;
}

void CTrayDialog::OnDestroy()
{
	CTrayDialogBase::OnDestroy();
	// shouldn't that be done before passing the message to DefWinProc?
	if (m_nidIconData.hWnd != NULL && m_nidIconData.uID > 0 && TrayIsVisible())
	{
		VERIFY( Shell_NotifyIcon(NIM_DELETE,&m_nidIconData) );
	}
}

bool CTrayDialog::TrayIsVisible()
{
	return m_bTrayIconVisible;
}

void CTrayDialog::TraySetIcon(HICON hIcon, bool bDelete)
{
	ASSERT(hIcon); 
	if (hIcon != NULL)
	{
		//ASSERT(m_hPrevIconDelete == NULL);
		if (m_bCurIconDelete)
		{
			ASSERT( m_nidIconData.hIcon != NULL && (m_nidIconData.uFlags & NIF_ICON) );
			m_hPrevIconDelete = m_nidIconData.hIcon;
		}
		m_bCurIconDelete = bDelete;
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
}

/* eklmn: removed unused functions
void CTrayDialog::TraySetIcon(UINT nResourceID, bool bDelete)
{
	TraySetIcon(AfxGetApp()->LoadIcon(nResourceID));	
}


void CTrayDialog::TraySetIcon(LPCTSTR lpszResourceName, bool bDelete)
{
	TraySetIcon(AfxGetApp()->LoadIcon(lpszResourceName));	
}
*/

void CTrayDialog::TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT(_tcslen(lpszToolTip) > 0 && _tcslen(lpszToolTip) < 64);
	_tcsncpy(m_nidIconData.szTip, lpszToolTip, ARRSIZE(m_nidIconData.szTip));
	m_nidIconData.szTip[ARRSIZE(m_nidIconData.szTip) - 1] = _T('\0');
	m_nidIconData.uFlags |= NIF_TIP;
}

bool CTrayDialog::TrayShow()
{
	bool	bSuccess = false;

	if (!m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
		if (bSuccess)
			m_bTrayIconVisible = true;
	}
	else
	{
		TRACE0("CTrayDialog::TrayShow: ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

bool CTrayDialog::TrayHide()
{
	EMULE_TRY
		
	bool	bSuccess = false;

	if (m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);
		if (bSuccess)
		{
			m_bTrayIconVisible= false;
		}
	}
	else
	{
		TRACE0("CTrayDialog::TrayHide: ICON ALREADY HIDDEN");
	}
	return bSuccess;
	
	EMULE_CATCH
	return false;
}

bool CTrayDialog::TrayUpdate()
{
	EMULE_TRY

	bool	bSuccess = false;

	if (m_bTrayIconVisible)
	{
            bSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_nidIconData);
            if (!bSuccess)
            {
                //ASSERT(0);
                return false; // don't delete 'm_hPrevIconDelete' because it's still attached to the tray
            }
        }
    else
    {
        //TRACE0("ICON NOT VISIBLE");
    }

    if (m_hPrevIconDelete != NULL)
    {
        VERIFY( ::DestroyIcon(m_hPrevIconDelete) );
        m_hPrevIconDelete = NULL;
    }        

	return bSuccess;

	EMULE_CATCH
	return false;
} 

bool CTrayDialog::TraySetMenu(UINT nResourceID,UINT nDefaultPos)
{	
	bool	bSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);

	ASSERT( bSuccess );
	return bSuccess;
}


bool CTrayDialog::TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos)
{	
	bool	bSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);

	ASSERT( bSuccess );
	return bSuccess;
}

bool CTrayDialog::TraySetMenu(HMENU hMenu,UINT nDefaultPos)
{
	m_mnuTrayMenu.Attach(hMenu);
	return true;
}

LRESULT CTrayDialog::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    UINT	uID = (UINT) wParam;
 
    if (uID != 1)
        return false;
	
    UINT	uMsg = (UINT) lParam;
    CPoint	pt;	

    switch (uMsg)
	{ 
		case WM_MOUSEMOVE:
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			OnTrayMouseMove(pt);
			break;
		case WM_LBUTTONDOWN:
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			OnTrayLButtonDown(pt);
			break;
		case WM_LBUTTONDBLCLK:
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			OnTrayLButtonDblClk(pt);
			break;
		case WM_RBUTTONUP:
		case WM_CONTEXTMENU:
			GetCursorPos(&pt);
			//ClientToScreen(&pt);
			OnTrayRButtonUp(pt);
			break;
		case WM_RBUTTONDBLCLK:
			GetCursorPos(&pt);
			ClientToScreen(&pt);
			OnTrayRButtonDblClk(pt);
			break;
		case WM_LBUTTONUP:
			if (m_bdoubleclicked)
			{
				if (TrayHide())
					ShowWindow(SW_SHOW);
				m_bdoubleclicked = false;
			}
			break;
	} 
	return true; 
}

void CTrayDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (m_bMinimizeToTray && *m_bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
			if(TrayShow())
				ShowWindow(SW_HIDE);
		}
		else
			CTrayDialogBase::OnSysCommand(nID, lParam);	
	}
   	else if ((nID & 0xFFF0) == SC_MINIMIZETRAY)
   	{
       	if(TrayShow())
       		ShowWindow(SW_HIDE);
   	}
	else
		CTrayDialogBase::OnSysCommand(nID, lParam);
}

void CTrayDialog::TraySetMinimizeToTray(byte *bMinimizeToTray)
{
	m_bMinimizeToTray = bMinimizeToTray;
}

void CTrayDialog::TrayMinimizeToTrayChanged()
{
    if (!m_bMinimizeToTray)
       return;

    if (*m_bMinimizeToTray)
       MinTrayBtnHide();
    else
       MinTrayBtnShow();
}

void CTrayDialog::OnTrayRButtonUp(CPoint pt)
{
	//m_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, pt.x, pt.y, this);
	//m_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem, true);
}

void CTrayDialog::OnTrayLButtonDown(CPoint pt)
{
}

void CTrayDialog::OnTrayLButtonDblClk(CPoint pt)
{
	m_bdoubleclicked = true;
}

void CTrayDialog::OnTrayRButtonDblClk(CPoint pt)
{
}

void CTrayDialog::OnTrayMouseMove(CPoint pt)
{
}

LRESULT CTrayDialog::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (m_bTrayIconVisible)
	{
		bool	bResult = Shell_NotifyIcon(NIM_ADD, &m_nidIconData);

		ASSERT( bResult );
		return bResult;
	}
	return true;
}
