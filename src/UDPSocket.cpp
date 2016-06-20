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

#include "stdafx.h"
#include "emule.h"
#include "packets.h"
#ifdef OLD_SOCKETS_ENABLED
#include "sockets.h"
#endif
#include "UDPSocket.h"
#include "ServerList.h"
#include "otherfunctions.h"
#include "otherstructs.h"
#include "UploadQueue.h"
#include "SafeFile.h"
#include "server.h"
#include "IPFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef OLD_SOCKETS_ENABLED

struct SRawServerPacket
{
	SRawServerPacket(BYTE* pPacket, UINT uSize, uint16 nPort) {
		m_pPacket = pPacket;
		m_uSize = uSize;
		m_nPort = nPort;
	}
	~SRawServerPacket() {
		delete[] m_pPacket;
	}
	BYTE* m_pPacket;
	UINT m_uSize;
	uint16 m_nPort;
};

struct SServerDNSRequest
{
	SServerDNSRequest(HANDLE hDNSTask, CServer *pServer) : m_aPackets(3) {
		m_dwCreated = GetTickCount();
		m_hDNSTask = hDNSTask;
		m_pServer = pServer;
		memzero(m_DnsHostBuffer, sizeof(m_DnsHostBuffer));
	}
	~SServerDNSRequest() {
		if (m_hDNSTask)
			WSACancelAsyncRequest(m_hDNSTask);
		delete m_pServer;
		POSITION pos = m_aPackets.GetHeadPosition();
		while (pos)
			delete m_aPackets.GetNext(pos);
	}
	DWORD m_dwCreated;
	HANDLE m_hDNSTask;
	CServer* m_pServer;
	CTypedPtrList<CPtrList, SRawServerPacket*> m_aPackets;
	char m_DnsHostBuffer[MAXGETHOSTSTRUCT];
};

#define	ERR_UDP_MISCONFIGURED_SERVER _T("Error: Failed to process server UDP packet")

#define WM_DNSLOOKUPDONE WM_USER+280

BEGIN_MESSAGE_MAP(CUDPSocketWnd, CWnd)
	ON_MESSAGE(WM_DNSLOOKUPDONE, OnDNSLookupDone)
END_MESSAGE_MAP()

LRESULT CUDPSocketWnd::OnDNSLookupDone(WPARAM wParam, LPARAM lParam)
{
	EMULE_TRY

	if(m_pOwner)
		m_pOwner->DnsLookupDone(wParam, lParam);
	return true;

	EMULE_CATCH

	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUDPSocket::CUDPSocket() : m_aDNSReqs(3)
{
	m_hWndResolveMessage = NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUDPSocket::~CUDPSocket()
{
	EMULE_TRY

	m_udpwnd.DestroyWindow();

	POSITION pos = m_aDNSReqs.GetHeadPosition();
	while (pos)
		delete m_aDNSReqs.GetNext(pos);

	EMULE_CATCH
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CUDPSocket::Create()
{
	EMULE_TRY

	VERIFY( m_udpwnd.CreateEx(0, AfxRegisterWndClass(0),_T("Emule Socket Wnd"),WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL));
	m_hWndResolveMessage = m_udpwnd.m_hWnd;
	m_udpwnd.m_pOwner = this;

	return (CAsyncSocket::Create(0, SOCK_DGRAM, FD_READ, g_App.m_pPrefs->GetBindAddr())) ? true : false;

	EMULE_CATCH

	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUDPSocket::OnReceive(int iErrorCode)
{
	NOPRM(iErrorCode);
	EMULE_TRY

	byte		abyteBuffer[5000], *pbyteBuf;
	SOCKADDR_IN	sockAddr = {0};
	int			iSockAddrLen = sizeof(sockAddr), iPayloadLen;
	int			iLength = ReceiveFrom(abyteBuffer, sizeof(abyteBuffer), (SOCKADDR*)&sockAddr, &iSockAddrLen);

	if (iLength >= 2)	//SOCKET_ERROR = -1
	{
		pbyteBuf = abyteBuffer;
		iPayloadLen = iLength;

		CServer	*pServer = g_App.m_pServerList->GetServerByIPUDP(sockAddr.sin_addr.S_un.S_addr, fast_ntohs(sockAddr.sin_port), true);

		if (pServer != NULL && g_App.m_pPrefs->IsServerCryptLayerUDPEnabled() &&
			((pServer->GetServerKeyUDP() != 0 && pServer->SupportsObfuscationUDP()) || (pServer->GetCryptPingReplyPending() && pServer->GetChallenge() != 0)))
		{
			uint32 dwKey = 0;
			if (pServer->GetCryptPingReplyPending() && pServer->GetChallenge() != 0 /* && pServer->GetPort() == ntohs(sockAddr.sin_port) - 12 */)
				dwKey = pServer->GetChallenge();
			else
				dwKey = pServer->GetServerKeyUDP();

			ASSERT( dwKey != 0 );
			iPayloadLen = DecryptReceivedServer(abyteBuffer, iLength, &pbyteBuf, dwKey, sockAddr.sin_addr.S_un.S_addr);
		}
		g_App.m_pDownloadQueue->AddDownDataOverheadServer(iPayloadLen - 2);
		if (pbyteBuf[0] == OP_EDONKEYPROT)
			ProcessPacket(pbyteBuf + 2, iPayloadLen - 2, pServer, pbyteBuf[1], sockAddr.sin_addr.s_addr, fast_ntohs(sockAddr.sin_port));
	}
	
	EMULE_CATCH
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CUDPSocket::ProcessPacket(byte *pbytePacket, uint32 dwSize, CServer *pServer, byte uOpcode, uint32 dwIP, uint16 uUDPPort)
{
	bool	bUpdateServerDesc;

	try
	{
		if (pServer == NULL)
			return false;	//	Don't process packets from unknown servers

		if ((bUpdateServerDesc = (pServer->GetFailedCount() != 0)) == true)
			pServer->ResetFailedCount();

		switch (uOpcode)
		{
			case OP_GLOBSEARCHRES:
			{
				CSafeMemFile	pckStream(pbytePacket, dwSize);
				uint32			dwLeft;
				uint16			uResults;
				byte			byteTmp;

				if (g_App.m_pSearchList->AllowUDPSearchAnswer())
				{
					do
					{
						uResults = g_App.m_pSearchList->ProcessUDPSearchAnswer(pckStream, cfUTF8, pServer);

					//	Check if there is another search results packet
						dwLeft = static_cast<uint32>(pckStream.GetLength() - pckStream.GetPosition());
						if (dwLeft >= 2)
						{
							pckStream.Read(&byteTmp, 1);
							dwLeft--;
							if (byteTmp != OP_EDONKEYPROT)
								break;

							pckStream.Read(&byteTmp, 1);
							dwLeft--;
							if (byteTmp != OP_GLOBSEARCHRES)
								break;
						}
					} while(dwLeft != 0);
					g_App.m_pMDlg->m_dlgSearch.AddGlobalEd2kSearchResults(uResults);
				}
				break;
			}
			case OP_GLOBFOUNDSOURCES:
			{
				CSafeMemFile	sources((BYTE*)pbytePacket, dwSize);
				uchar			abyteFileId[16];
				uint32			dwLeft;
				byte			byteTmp;

				do
				{
					sources.Read(abyteFileId, 16);

					CPartFile* 		pPartFile = g_App.m_pDownloadQueue->GetFileByID(abyteFileId);

					if (pPartFile)
						pPartFile->AddServerSources(sources, dwIP, uUDPPort - 4, false);
					else
					{
					//	Skip sources for that file
						sources.Read(&byteTmp, 1);
						sources.Seek(byteTmp * (4 + 2), SEEK_CUR);
					}

				//	Check if there is another source packet
					dwLeft = static_cast<uint32>(sources.GetLength() - sources.GetPosition());
					if (dwLeft >= 2)
					{
						sources.Read(&byteTmp, 1);
						dwLeft--;
						if (byteTmp != OP_EDONKEYPROT)
							break;

						sources.Read(&byteTmp, 1);
						dwLeft--;
						if (byteTmp != OP_GLOBFOUNDSOURCES)
							break;
					}
				} while(dwLeft != 0);
				break;
			}
 			case OP_GLOBSERVSTATRES:
			{
			//	Check minimal allowed size (contain basic information)
				if ((dwSize < 12) || (PEEK_DWORD(pbytePacket) != pServer->GetChallenge()))
					return false;
				pServer->SetChallenge(0);
				pServer->SetCryptPingReplyPending(false);

				uint32	dwCurTime = static_cast<uint32>(time(NULL));

			//	If we used Obfuscated ping, we still need to reset the time properly
				pServer->SetLastPingedTime(dwCurTime - (rand() % UDPSRVSTATREASKRNDTIME));

				pServer->SetUserCount(PEEK_DWORD(pbytePacket + 4));
				pServer->SetFileCount(PEEK_DWORD(pbytePacket + 8));

				pServer->SetMaxUsers((dwSize >= 16) ? PEEK_DWORD(pbytePacket + 12) : 0);
				pServer->SetSoftMaxFiles((dwSize >= 24) ? PEEK_DWORD(pbytePacket + 16) : 0);
				pServer->SetHardMaxFiles((dwSize >= 24) ? PEEK_DWORD(pbytePacket + 20) : 0);

				pServer->SetUDPFlags((dwSize >= 28) ? PEEK_DWORD(pbytePacket + 24) : 0);
				pServer->SetLowIDUsers((dwSize >= 32) ? PEEK_DWORD(pbytePacket + 28) : 0);

				pServer->SetObfuscationPortUDP((dwSize >= 40) ? PEEK_WORD(pbytePacket + 32) : 0);
				pServer->SetObfuscationPortTCP((dwSize >= 40) ? PEEK_WORD(pbytePacket + 34) : 0);
				pServer->SetServerKeyUDP((dwSize >= 40) ? PEEK_DWORD(pbytePacket + 36) : 0);

				pServer->SetPing(::GetTickCount() - pServer->GetLastPinged());

				pServer->SetLastDescPingedCount(false);
				if (pServer->GetLastDescPingedCount() < 2)
				{
				// eserver 16.45+ supports a new OP_SERVER_DESC_RES answer, if the OP_SERVER_DESC_REQ contains a uint32
				// challenge, the server returns additional info with OP_SERVER_DESC_RES. To properly distinguish the
				// old and new OP_SERVER_DESC_RES answer, the challenge has to be selected carefully. The first 2 bytes
				// of the challenge (in network byte order) MUST NOT be a valid string-len-int16!
					Packet	*pPacket = new Packet(OP_SERVER_DESC_REQ, 4);
					uint32	dwChallenge = (rand() << 16) | INV_SERV_DESC_LEN; // 0xF0FF = 'invalid' string length
					pServer->SetDescReqChallenge(dwChallenge);
					POKE_DWORD(pPacket->m_pcBuffer, dwChallenge);
					g_App.m_pUploadQueue->AddUpDataOverheadServer(pPacket->m_dwSize);
					g_App.m_pServerConnect->SendUDPPacket(pPacket, pServer, true);
				}
				else
					pServer->SetLastDescPingedCount(true);

				bUpdateServerDesc = true;
				break;
			}

 			case OP_SERVER_DESC_RES:
			{
				// old packet: <name_len 2><name name_len><desc_len 2><desc desc_len>
				// new packet: <challenge 4><taglist>
				//
				// NOTE: To properly distinguish between the two packets which are both using the same opcode...
				// the first two bytes of <challenge> (in network byte order) have to be an invalid <name_len> at least.

				CSafeMemFile	srvinfo((BYTE*)pbytePacket, dwSize);
				uint32			ui, dwTmp;

				if ((dwSize >= 8) && (PEEK_WORD(pbytePacket) == INV_SERV_DESC_LEN))
				{
					srvinfo.Read(&dwTmp, 4);	 // read challenge
					if ((pServer->GetDescReqChallenge() != 0) && (dwTmp == pServer->GetDescReqChallenge()))
					{
						pServer->SetDescReqChallenge(0);
						srvinfo.Read(&dwTmp, 4);	 // read tag count
						for (ui = 0; ui < dwTmp; ui++)
						{
							CTag	TempTag;

							TempTag.FillFromStream(srvinfo, cfUTF8);
							if (TempTag.GetTagID() == ST_SERVERNAME)
							{
								if (TempTag.IsStr() && !pServer->IsStaticMember())
									pServer->SetListName(TempTag.GetStringValue());
							}
							else if (TempTag.GetTagID() == ST_DESCRIPTION)
							{
								if (TempTag.IsStr())
									pServer->SetDescription(TempTag.GetStringValue());
							}
							else if (TempTag.GetTagID() == ST_DYNIP)
							{
								// Verify that we really received a DN
								if (TempTag.IsStr() && (inet_addr(TempTag.GetStringValue()) == INADDR_NONE))
								{
									int	iRc = pServer->GetDynIP().CompareNoCase(TempTag.GetStringValue());

									pServer->SetDynIP(TempTag.GetStringValue());
									// If a dynIP-server changed its address or, if this is the
									// first time we get the dynIP-address for a server which we
									// already have as non-dynIP in our list, we need to remove
									// an already available server with the same 'dynIP:port'.
									if (iRc != 0)
										g_App.m_pServerList->RemoveDuplicates(pServer, false);
								}
							}
							else if (TempTag.GetTagID() == ST_VERSION)
							{
								if (TempTag.IsStr())
									pServer->SetVersion(TempTag.GetStringValue());
								else if (TempTag.IsInt())
								{
									CString strVersion;

									strVersion.Format(_T("%u.%02u"), TempTag.GetIntValue() >> 16, TempTag.GetIntValue() & 0xFFFF);
									pServer->SetVersion(strVersion);
								}
							}
							else if (TempTag.GetTagID() == ST_AUXPORTSLIST)
							{
								if (TempTag.IsStr())	// <string> = <port> [, <port>...]
								{
								}
							}
						}
					}
				}
				else
				{
					uint16	uStringLen;
					CString	strBuf;

					srvinfo.Read(&uStringLen, 2);
					ReadMB2Str(cfUTF8, &strBuf, srvinfo, uStringLen);

					if(!pServer->IsStaticMember())
						pServer->SetListName(strBuf);

					srvinfo.Read(&uStringLen, 2);
					ReadMB2Str(cfUTF8, &strBuf, srvinfo, uStringLen);

					pServer->SetDescription(strBuf);
				}
				bUpdateServerDesc = true;
				break;
			}
			default:
				return false;
		}

		if (bUpdateServerDesc)
			g_App.m_pMDlg->m_wndServer.m_ctlServerList.RefreshServer(*pServer);

		return true;
	}
	catch(CFileException* error)
	{
		CString	strBuff(ERR_UDP_MISCONFIGURED_SERVER);

		if (pServer != NULL)
			strBuff.AppendFormat(_T(" %s:%u"), pServer->GetAddress(), pServer->GetPort());

		OUTPUT_DEBUG_TRACE();
		AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, strBuff);
		error->Delete();
		if (uOpcode == OP_GLOBSEARCHRES || uOpcode == OP_GLOBFOUNDSOURCES)
			return true;
	}
	catch(CMemoryException* error)
	{
		OUTPUT_DEBUG_TRACE();
		AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, ERR_UDP_MISCONFIGURED_SERVER _T(" - CMemoryException"));
		error->Delete();
		if (uOpcode == OP_GLOBSEARCHRES || uOpcode == OP_GLOBFOUNDSOURCES)
			return true;
	}
	catch(CString error)
	{
		OUTPUT_DEBUG_TRACE();
		AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, ERR_UDP_MISCONFIGURED_SERVER _T(" - ") + error);
	}
	catch(...)
	{
		OUTPUT_DEBUG_TRACE();
		AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, ERR_UDP_MISCONFIGURED_SERVER);
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUDPSocket::DnsLookupDone(WPARAM wp, LPARAM lp)
{
	EMULE_TRY

	// A Winsock DNS task has completed. Search the according application data for that
	// task handle.
	SServerDNSRequest* pDNSReq = NULL;
	HANDLE hDNSTask = (HANDLE)wp;
	POSITION pos = m_aDNSReqs.GetHeadPosition();
	while (pos) {
		POSITION posPrev = pos;
		SServerDNSRequest* pCurDNSReq = m_aDNSReqs.GetNext(pos);
		if (pCurDNSReq->m_hDNSTask == hDNSTask) {
			// Remove this DNS task from our list
			m_aDNSReqs.RemoveAt(posPrev);
			pDNSReq = pCurDNSReq;
			break;
		}
	}
	if (pDNSReq == NULL) {
		return;
	}

	// DNS task did complete successfully?
	if (WSAGETASYNCERROR(lp) != 0)
	{
		AddLogLine( LOG_FL_DBG | LOG_RGB_WARNING,
			_T("Error: Server UDP socket: Failed to resolve address for server '%s' (%s) - %s"),
			pDNSReq->m_pServer->GetListName(), pDNSReq->m_pServer->GetAddress(), GetErrorMessage(WSAGETASYNCERROR(lp), 1) );
		delete pDNSReq;
		return;
	}

	// Get the IP value
	uint32 nIP = INADDR_NONE;
	int iBufLen = WSAGETASYNCBUFLEN(lp);
	if (iBufLen >= sizeof(HOSTENT)) {
		LPHOSTENT pHost = (LPHOSTENT)pDNSReq->m_DnsHostBuffer;
		if (pHost->h_length == 4 && pHost->h_addr_list && pHost->h_addr_list[0])
			nIP = ((LPIN_ADDR)(pHost->h_addr_list[0]))->s_addr;
	}
	if (nIP != INADDR_NONE)
	{
		bool bRemoveServer = false;
#ifdef _CRYPT_READY
		if (!IsGoodIP(nIP)) {
			if (!g_App.m_pPrefs->IsCMNotLog())
				AddLogLine( LOG_FL_DBG | LOG_RGB_DIMMED, _T("Filtered server '%s' (IP=%s) - Invalid IP or LAN address"),
									pDNSReq->m_pServer->GetAddress(), ipstr(nIP) );
			bRemoveServer = true;
		}
		else
#endif
			if (g_App.m_pIPFilter->IsFiltered(nIP))
		{
			if (!g_App.m_pPrefs->IsCMNotLog())
				AddLogLine( LOG_FL_DBG | LOG_RGB_DIMMED, _T("Filtered Server: %s (IP=%s) %hs"),
					pDNSReq->m_pServer->GetAddress(), ipstr(nIP), g_App.m_pIPFilter->GetLastHit() );
			bRemoveServer = true;
		}

		CServer* pServer = g_App.m_pServerList->GetServerByAddress(pDNSReq->m_pServer->GetAddress(), pDNSReq->m_pServer->GetPort());
		if (pServer) {
			pServer->SetIP(nIP);
			// If we already have entries in the server list (dynIP-servers without a DN)
			// with the same IP as this dynIP-server, remove the duplicates.
			g_App.m_pServerList->RemoveDuplicates(pServer, true);
		}

		if (bRemoveServer) {
			if (pServer)
				g_App.m_pMDlg->m_wndServer.m_ctlServerList.RemoveServer(pServer);
			delete pDNSReq;
			return;
		}

		// Send all of the queued packets for this server.
		POSITION posPacket = pDNSReq->m_aPackets.GetHeadPosition();
		while (posPacket) {
			SRawServerPacket* pServerPacket = pDNSReq->m_aPackets.GetNext(posPacket);
			SendBuffer(nIP, pServerPacket->m_nPort, pServerPacket->m_pPacket, pServerPacket->m_uSize);
			// Detach packet data
			pServerPacket->m_pPacket = NULL;
		}
	}
	else
	{
		// still no valid IP for this server
		AddLogLine( LOG_FL_DBG | LOG_RGB_WARNING,
			_T("Error: Server UDP socket: Failed to resolve address for server '%s' (%s)"),
			pDNSReq->m_pServer->GetListName(), pDNSReq->m_pServer->GetAddress() );
	}
	delete pDNSReq;
	EMULE_CATCH
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUDPSocket::SendBuffer(uint32 dwIP, uint16 uPort, byte *pPacket, unsigned uiSize)
{
	SOCKADDR_IN		sockAddr = {0};

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = fast_htons(uPort);
	sockAddr.sin_addr.s_addr = dwIP;
	SendTo(pPacket, uiSize, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUDPSocket::SendPacket(Packet *pPacket, CServer *pServer, uint16 uSpecPort, byte *pbyteInRawPkt, uint32 dwRawLen)
{
	USES_CONVERSION;

	byte		*pbyteRawPkt;
	unsigned	uiRawPktSize;
	uint16		uPort = 0;

	if (pPacket != NULL)
	{
		pbyteRawPkt = new byte[pPacket->m_dwSize + sizeof(UDP_Header_Struct)];
		memcpy(pbyteRawPkt, pPacket->GetUDPHeader(), sizeof(UDP_Header_Struct));
		memcpy2(pbyteRawPkt + sizeof(UDP_Header_Struct), pPacket->m_pcBuffer, pPacket->m_dwSize);
		uiRawPktSize = pPacket->m_dwSize + sizeof(UDP_Header_Struct);

		if ( g_App.m_pPrefs->IsServerCryptLayerUDPEnabled() &&
			(pServer->GetServerKeyUDP() != 0) && pServer->SupportsObfuscationUDP() )
		{
			uiRawPktSize = EncryptSendServer(&pbyteRawPkt, uiRawPktSize, pServer->GetServerKeyUDP());
			uPort = pServer->GetObfuscationPortUDP();
		}
		else
			uPort = pServer->GetPort() + 4;
	}
	else if (pbyteInRawPkt != 0)
	{
		// we don't encrypt rawpackets (!)
		pbyteRawPkt = new byte[dwRawLen];
		memcpy2(pbyteRawPkt, pbyteInRawPkt, dwRawLen);
		uiRawPktSize = dwRawLen;
		uPort = pServer->GetPort() + 4;
	}
	else
		return;

	uPort = (uSpecPort == 0) ? uPort : uSpecPort;

	// Do we need to resolve the DN of this server?
	LPCSTR pszHostAddressA = T2CA(pServer->GetAddress());
	uint32 dwIP = inet_addr(pszHostAddressA);

	if (dwIP == INADDR_NONE)
	{
		POSITION	posPrev, pos = m_aDNSReqs.GetHeadPosition();
		SServerDNSRequest	*pDNSReq;
		SRawServerPacket	*pServerPacket;

		if (pos) {
			DWORD dwNow = GetTickCount();
			do {
				posPrev = pos;
				pDNSReq = m_aDNSReqs.GetNext(pos);
				// Just for safety. Ensure that there are no stalled DNS queries and/or packets
				// hanging endlessly in the queue.
				if (dwNow - pDNSReq->m_dwCreated >= SEC2MS(60*2))
				{
					m_aDNSReqs.RemoveAt(posPrev);
					delete pDNSReq;
				}
				// If there is already a DNS query ongoing or queued for this server, append the
				// current packet to this DNS query. The packet(s) will be sent later after the DNS
				// query has completed.
				else if (stricmp(CStringA(pDNSReq->m_pServer->GetAddress()), pszHostAddressA) == 0)
				{
					pServerPacket = new SRawServerPacket(pbyteRawPkt, uiRawPktSize, uPort);
					pDNSReq->m_aPackets.AddTail(pServerPacket);
					return;
				}
			} while (pos);
		}

		// Create a new DNS query for this server
		pDNSReq = new SServerDNSRequest(NULL, new CServer(pServer));
		pDNSReq->m_hDNSTask = WSAAsyncGetHostByName(m_hWndResolveMessage, WM_DNSLOOKUPDONE,
					pszHostAddressA, pDNSReq->m_DnsHostBuffer, sizeof(pDNSReq->m_DnsHostBuffer));
		if (pDNSReq->m_hDNSTask == NULL) {
			AddLogLine( LOG_FL_DBG | LOG_RGB_WARNING,
				_T("Error: Server UDP socket: Failed to resolve address for '%hs' - %s"),
				pszHostAddressA, GetErrorMessage(GetLastError(), 1) );
			delete pDNSReq;
			delete[] pbyteRawPkt;
			return;
		}

		pServerPacket = new SRawServerPacket(pbyteRawPkt, uiRawPktSize, uPort);
		pDNSReq->m_aPackets.AddTail(pServerPacket);
		m_aDNSReqs.AddTail(pDNSReq);
	}
	else
	{
		// No DNS query needed for this server. Just send the packet.
		SendBuffer(dwIP, uPort, pbyteRawPkt, uiRawPktSize);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //OLD_SOCKETS_ENABLED
