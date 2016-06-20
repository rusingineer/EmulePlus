//this file is part of eMule
//Copyright (C)2002-2008 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
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
#pragma once

#include "EncryptedDatagramSocket.h"

#ifdef OLD_SOCKETS_ENABLED

class CServerConnect;
struct SServerDNSRequest;
class CUDPSocket;
class Packet;
class CServer;

///////////////////////////////////////////////////////////////////////////////
// CUDPSocketWnd

class CUDPSocketWnd : public CWnd
{
public:
	CUDPSocketWnd() {};
	CUDPSocket* m_pOwner;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDNSLookupDone(WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////////////////////////////////
// CUDPSocket

class CUDPSocket : public CAsyncSocket, public CEncryptedDatagramSocket, CLoggable
{
	friend class CServerConnect;
public:
	CUDPSocket();
	~CUDPSocket();
	bool	Create();
	void	SendPacket(Packet *pPacket, CServer *pServer, uint16 uSpecPort, byte *pInRawPkt, uint32 dwRawLen);
	void	DnsLookupDone(WPARAM wp, LPARAM lp);

protected:
	virtual void OnReceive(int iErrorCode);

private:
	HWND m_hWndResolveMessage;
	CUDPSocketWnd m_udpwnd;
	CTypedPtrList<CPtrList, SServerDNSRequest*> m_aDNSReqs;

	void	SendBuffer(uint32 dwIP, uint16 uPort, byte *pPacket, unsigned uiSize);
	bool	ProcessPacket(byte *pbytePacket, uint32 dwSize, CServer *pServer, byte uOpcode, uint32 dwIP, uint16 uPort);
};
#endif //OLD_SOCKETS_ENABLED
