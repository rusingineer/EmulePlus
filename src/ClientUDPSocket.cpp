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
#include "updownclient.h"
#include "ClientUDPSocket.h"
#include "opcodes.h"
#include "SharedFileList.h"
#include "otherfunctions.h"
#include "UploadQueue.h"
#include "SafeFile.h"
#include "IPFilter.h"
#include "packets.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

struct UDPPack
{
	Packet	*packet;
	uint32	dwIP;
	uint16	nPort;
	bool	bEncrypt;
	bool	bKad;
	uint32	dwReceiverVerifyKey;
	uchar	abyteTargetClientHashORKadID[16];
};

CClientUDPSocket::CClientUDPSocket()
{
	m_bWouldBlock = false;
}

CClientUDPSocket::~CClientUDPSocket()
{
}

void CClientUDPSocket::OnReceive(int iErrorCode)
{
	NOPRM(iErrorCode);
	EMULE_TRY

	byte 		abyteBuffer[5000], *pBuffer;
	SOCKADDR_IN sockAddr = {0};
	int 			iSockAddrLen = sizeof(sockAddr);
	int 			iRealLen = ReceiveFrom(abyteBuffer, sizeof(abyteBuffer), (SOCKADDR*)&sockAddr, &iSockAddrLen);

	if (g_App.m_pIPFilter->IsFiltered(sockAddr.sin_addr.S_un.S_addr))
	{
		InterlockedIncrement(&g_App.m_lIncomingFiltered);
		InterlockedIncrement(&g_App.m_lTotalFiltered);
	}
	else
	{
		const TCHAR	*pcError = NULL;
		uint32		nReceiverVerifyKey;
		uint32		nSenderVerifyKey;
		int			iPacketLen = DecryptReceivedClient(abyteBuffer, iRealLen, &pBuffer, sockAddr.sin_addr.S_un.S_addr, &nReceiverVerifyKey, &nSenderVerifyKey);

		if (iPacketLen >= 2)
		{
			try
			{
				switch (pBuffer[0])
				{
					case OP_EMULEPROT:
						ProcessPacket(pBuffer + 2, iPacketLen - 2, pBuffer[1], sockAddr.sin_addr.S_un.S_addr, fast_ntohs(sockAddr.sin_port));
						break;
					default:
						pcError = _T("Unknown protocol");
						break;
				}
			}
			catch(CFileException* error)
			{
				OUTPUT_DEBUG_TRACE();
				error->Delete();
				pcError = _T("Invalid packet received");
			}
			catch(CMemoryException* error)
			{
				OUTPUT_DEBUG_TRACE();
				error->Delete();
				pcError = _T("Memory exception");
			}
			catch(CString error)
			{
				OUTPUT_DEBUG_TRACE();
				AddLogLine( LOG_FL_DBG | LOG_RGB_INDIAN_RED, _T("Client %s:%u caused UDP packet (prot=%#x opcode=%#x DecryptSz=%u RealSz=%u) processing error: %s"),
					ipstr(sockAddr.sin_addr), fast_ntohs(sockAddr.sin_port), pBuffer[0], pBuffer[1], iPacketLen, iRealLen, error );
			}
			catch (...)
			{
				OUTPUT_DEBUG_TRACE();
				pcError = _T("Unknown exception");
			}
		}
		else if (iPacketLen > 0)
			pcError = _T("Packet too short");
		else if (iPacketLen == SOCKET_ERROR)
		{
		}

		if (pcError != NULL)
		{
			AddLogLine( LOG_FL_DBG | LOG_RGB_INDIAN_RED, _T("Client %s:%u caused UDP packet (prot=%#x opcode=%#x DecryptSz=%u RealSz=%u) processing error: %s"),
				ipstr(sockAddr.sin_addr), fast_ntohs(sockAddr.sin_port), pBuffer[0], pBuffer[1], iPacketLen, iRealLen, pcError );
		}
	}

	EMULE_CATCH
}

bool CClientUDPSocket::ProcessPacket(byte *pbytePacket, uint32 dwSize, byte opcode, uint32 dwIP, uint16 port)
{
	switch (opcode)
	{
		case OP_REASKFILEPING:
		{
			g_App.m_pDownloadQueue->AddDownDataOverheadFileRequest(dwSize);
		//	Check pbytePacket size
			if (dwSize < 16)
			{
				ASSERT (false);
				break;
			}

		//	Look up the requested file hash in our shared files list
			CKnownFile		*pKnownFile = g_App.m_pSharedFilesList->GetFileByID(reinterpret_cast<BYTE*>(pbytePacket));
			bool			bSenderMultipleIpUnknown = false;
			CUpDownClient	*pClient = g_App.m_pUploadQueue->GetWaitingClientByIP_UDP(dwIP, port, &bSenderMultipleIpUnknown);

			if (pKnownFile == NULL)	//	If it's not there...
			{
				Packet		*pResponsePacket = new Packet(OP_FILENOTFOUND, 0, OP_EMULEPROT);

				g_App.m_pUploadQueue->AddUpDataOverheadFileRequest(pResponsePacket->m_dwSize);
				if (pClient != NULL)
					SendPacket(pResponsePacket, dwIP, port, pClient->ShouldReceiveCryptUDPPackets(), pClient->GetUserHash(), false, 0);
				else
					SendPacket(pResponsePacket, dwIP, port, false, NULL, false, 0);
				break;
			}

			if (pClient != NULL)	//	If the client is waiting in our upload queue...
			{
			//	Make sure that the request is still for the same file
				if (md4cmp(pbytePacket, pClient->GetUploadFileID()) == 0)
				{
					pClient->AddAskedCount();
					pClient->SetLastUpRequest();
					pClient->SetLastL2HACExecution();

					if (pClient->GetUDPVersion() > 3)
					{
						//	Don't reply when packet format doesn't fit UDP version or
						//	received packet is for a wrong file
						if (dwSize < 20)
							break;

						CSafeMemFile	pktInStrm(pbytePacket + 16, dwSize - 16);

						if (!pClient->ProcessExtendedInfo(&pktInStrm, pKnownFile))
							break;
					}
					else if (pClient->GetUDPVersion() > 2)
					{
						if (dwSize < 18)
							break;	//don't reply when packet format doesn't fit UDP version
						uint16 nCompleteCountLast = pClient->GetUpCompleteSourcesCount();
						uint16 nCompleteCountNew = PEEK_WORD(pbytePacket + 16);
						pClient->SetUpCompleteSourcesCount(nCompleteCountNew);
						if (nCompleteCountLast != nCompleteCountNew)
							pKnownFile->CalculateCompleteSources();
					}

					CSafeMemFile	packetStream(128);

					if (pClient->GetUDPVersion() > 3)
					{
						if (pKnownFile->IsPartFile())
							((CPartFile*)pKnownFile)->WritePartStatus(&packetStream);
						else if (pKnownFile->GetJumpstartEnabled())
							pKnownFile->WriteJumpstartPartStatus(pClient, &packetStream);
						else if (pKnownFile->HasHiddenParts())
							pKnownFile->WritePartStatus(&packetStream);
						else
						{
							uint16	uNull = 0;

							packetStream.Write(&uNull, 2);
						}
					}

					uint16	uWaitingPos = g_App.m_pUploadQueue->GetWaitingPosition(pClient);

					packetStream.Write(&uWaitingPos, 2);

					Packet	*pPacket = new Packet(&packetStream, OP_EMULEPROT);

					pPacket->m_eOpcode = OP_REASKACK;
					g_App.m_pUploadQueue->AddUpDataOverheadFileRequest(pPacket->m_dwSize);
					SendPacket(pPacket, dwIP, port, pClient->ShouldReceiveCryptUDPPackets(), pClient->GetUserHash(), false, 0);
				}
			}
			else	//	If the client isn't currently in our upload queue...
			{
				// Don't answer him. We probably have him on our queue already, but can't locate him. Force him to establish a TCP connection
				if (!bSenderMultipleIpUnknown)
				{
				//	... and there are less than 50 slots free in our upload queue...
					if ((static_cast<uint32>(g_App.m_pUploadQueue->GetWaitingUserCount()) + 50) > g_App.m_pPrefs->GetQueueSize())
					{
						Packet		*pPacket = new Packet(OP_QUEUEFULL, 0, OP_EMULEPROT);

						g_App.m_pUploadQueue->AddUpDataOverheadFileRequest(pPacket->m_dwSize);
						SendPacket(pPacket, dwIP, port, false, NULL, false, 0);
					}
				}
				else
					AddLogLine(LOG_FL_DBG | LOG_RGB_WARNING, _T("UDP Packet received - multiple clients with the same IP but different UDP port found. Possible UDP Portmapping problem, enforcing TCP connection. IP: %s, Port: %u"), ipstr(dwIP), port);
			}
			break;
		}
		case OP_QUEUEFULL:
		{
			g_App.m_pDownloadQueue->AddDownDataOverheadFileRequest(dwSize);

			CUpDownClient		*pClient = g_App.m_pDownloadQueue->GetDownloadClientByIP_UDP(dwIP, port);

			if ((pClient != NULL) && pClient->IsUDPFileReqPending())
			{
				pClient->UDPReaskACK(0);
			}
			break;
		}
		case OP_REASKACK:
		{
			g_App.m_pDownloadQueue->AddDownDataOverheadFileRequest(dwSize);

			CUpDownClient		*pClient;

		//	Drop the packet if... 
		//	... it has invalid length or sender is unknown
			if ((dwSize < 2) || ((pClient = g_App.m_pDownloadQueue->GetDownloadClientByIP_UDP(dwIP, port)) == NULL))
				break;

		//	... we didn't request any file from this client
			if (pClient->m_pReqPartFile == NULL)
				break;
			if (!pClient->IsUDPFileReqPending())
				break;

			CSafeMemFile packetStream(pbytePacket, dwSize);

			if ((dwSize >= 4) && (pClient->GetUDPVersion() > 3))
			{
				pClient->ProcessFileStatus(packetStream, true);
			}

			uint16		uRank;

			packetStream.Read(&uRank, 2);
			pClient->UDPReaskACK(uRank);
			break;
		}
		case OP_FILENOTFOUND:
		{
			g_App.m_pDownloadQueue->AddDownDataOverheadFileRequest(dwSize);

			CUpDownClient		*pClient = g_App.m_pDownloadQueue->GetDownloadClientByIP_UDP(dwIP, port);

			if ((pClient != NULL) && pClient->IsUDPFileReqPending())
			{
				pClient->UDPReaskFNF();
			}
			break;
		}
		default:
			g_App.m_pDownloadQueue->AddDownDataOverheadOther(dwSize);
			return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	From MSDN:
//	The following error codes apply to the OnSend member function: 
//	0           - The function executed successfully. 
//	WSAENETDOWN - The Windows Sockets implementation detected that the network subsystem failed.
//
//	NOTE: In second case we need also to unblock the socket to be able send a packets on reconnect & to process
//	the packet queue in order to clear the queue in natural way.
void CClientUDPSocket::OnSend(int iErrorCode)
{
	m_bWouldBlock = false;

	EMULE_TRY

	while (!m_controlPacketQueue.IsEmpty() && !IsBusy())
	{
		uint32		dwLen;
		UDPPack		*pCurPacket = m_controlPacketQueue.GetHead();
		byte 		*pbyteSendBuf = NULL;

		if (iErrorCode != 0)
			goto QueCleanup;
		dwLen = pCurPacket->packet->m_dwSize + 2;
		pbyteSendBuf = new byte[dwLen];
		memcpy(pbyteSendBuf, pCurPacket->packet->GetUDPHeader(), 2);
		memcpy2(pbyteSendBuf + 2, pCurPacket->packet->m_pcBuffer, pCurPacket->packet->m_dwSize);

		if (pCurPacket->bEncrypt && ((g_App.GetPublicIP() != 0) || pCurPacket->bKad))
		{
			dwLen = EncryptSendClient(&pbyteSendBuf, dwLen, pCurPacket->abyteTargetClientHashORKadID,
				pCurPacket->bKad, pCurPacket->dwReceiverVerifyKey,
#ifdef _KAD_READY
				(pCurPacket->bKad) ? Kademlia::CPrefs::GetUDPVerifyKey(pCurPacket->dwIP) : 0
#else
				0
#endif
				);
		}
	//	Remove the packet if it was sent or it's impossible to send due to non-handled error
		if (SendTo(pbyteSendBuf, dwLen, pCurPacket->dwIP, pCurPacket->nPort) >= 0)
		{
QueCleanup:
			m_controlPacketQueue.RemoveHead();
			delete pCurPacket->packet;
			delete pCurPacket;
		}
		delete[] pbyteSendBuf;
	}

	EMULE_CATCH
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CClientUDPSocket::SendTo(const void *pcBuf, int iBufLen, uint32 dwIP, uint16 nPort)
{
	EMULE_TRY

	int			iResult;
	uint32			dwError;
	SOCKADDR_IN		sockAddr = {0};

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = fast_htons(nPort);
	sockAddr.sin_addr.s_addr = dwIP;

	iResult = CAsyncSocket::SendTo(pcBuf, iBufLen, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr));
	if (iResult == SOCKET_ERROR)
	{
		dwError = ::WSAGetLastError();
		if (dwError == WSAEWOULDBLOCK)
		{
			m_bWouldBlock = true;
			return -1;
		}
		else
		{
			AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, _T("UDP packet sending error (%u)"), dwError);
			return 0;
		}
	}

#if 1
	if (iResult != iBufLen)
		AddLogLine(LOG_FL_DBG | LOG_RGB_WARNING, _T("Incomplete packet was sent %u from %u"), iBufLen, iResult);
#endif

	return iResult;

	EMULE_CATCH

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CClientUDPSocket::SendPacket(Packet *packet, uint32 dwIP, uint16 nPort, bool bEncrypt, const uchar *pachTargetClientHashORKadID, bool bKad, uint32 nReceiverVerifyKey)
{
	EMULE_TRY
	UDPPack		*newpending = new UDPPack;

	newpending->dwIP = dwIP;
	newpending->nPort = nPort;
	newpending->packet = packet;
	newpending->bEncrypt = bEncrypt && (pachTargetClientHashORKadID != NULL || (bKad && nReceiverVerifyKey != 0));
	newpending->bKad = bKad;
	newpending->dwReceiverVerifyKey = nReceiverVerifyKey;
	if (bEncrypt && (pachTargetClientHashORKadID != NULL))
		md4cpy(newpending->abyteTargetClientHashORKadID, pachTargetClientHashORKadID);
	else
		md4clr(newpending->abyteTargetClientHashORKadID);

//	use a FIFO principle in case if some packets were already queued
	if (IsBusy() || !m_controlPacketQueue.IsEmpty())
	{
		m_controlPacketQueue.AddTail(newpending);
		return true;
	}

	char 		*pcSendBuffer = new char[packet->m_dwSize + 2];

	memcpy(pcSendBuffer, packet->GetUDPHeader(), 2);
	memcpy2(pcSendBuffer + 2, packet->m_pcBuffer, packet->m_dwSize);
	if (SendTo(pcSendBuffer, packet->m_dwSize + 2, dwIP, nPort) < 0)
	{
		m_controlPacketQueue.AddTail(newpending);
	}
	else
	{
		delete newpending->packet;
		delete newpending;
	}
	delete[] pcSendBuffer;

	return true;
	EMULE_CATCH
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CClientUDPSocket::Create()
{
	EMULE_TRY
	if (g_App.m_pPrefs->GetUDPPort())
		return (CAsyncSocket::Create(g_App.m_pPrefs->GetUDPPort(), SOCK_DGRAM, FD_READ | FD_WRITE, g_App.m_pPrefs->GetBindAddr()) != FALSE);
	else
		return true;
	EMULE_CATCH
	return false;
}
