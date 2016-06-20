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
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

/* Basic Obfusicated Handshake Protocol UDP:
	see EncryptedStreamSocket.h

****************************** ED2K Packets

	-Keycreation Client <-> Clinet:
	 - Client A (Outgoing connection):
				Sendkey:	Md5(<UserHashClientB 16><IPClientA 4><MagicValue91 1><RandomKeyPartClientA 2>)  23
	 - Client B (Incomming connection):
				Receivekey: Md5(<UserHashClientB 16><IPClientA 4><MagicValue91 1><RandomKeyPartClientA 2>)  23
	 - Note: The first 1024 Bytes will be _NOT_ discarded for UDP keys to safe CPU time

	- Handshake
			-> The handshake is encrypted - except otherwise noted - by the Keys created above
			-> Padding is currently not used for UDP meaning that PaddingLen will be 0, using PaddingLens up to 16 Bytes is acceptable however
		Client A: <SemiRandomNotProtocolMarker 7 Bits[Unencrypted]><ED2K Marker 1Bit = 1><RandomKeyPart 2[Unencrypted]><MagicValue 4><PaddingLen 1><RandomBytes PaddingLen%16>	
	
	- Additional Comments:
			- For obvious reasons the UDP handshake is actually no handshake. If a different Encryption method (or better a different Key) is to be used this has to be negotiated in a TCP connection
		    - SemiRandomNotProtocolMarker is a Byte which has a value unequal any Protocol header byte. This is a compromiss, turning in complete randomness (and nice design) but gaining
			  a lower CPU usage
		    - Kad/Ed2k Marker are only indicators, which possibility could be tried first, and should not be trusted

****************************** Server Packets

	-Keycreation Client <-> Server:
	 - Client A (Outgoing connection client -> server):
				Sendkey:	Md5(<BaseKey 4><MagicValueClientServer 1><RandomKeyPartClientA 2>)  7
	 - Client B (Incomming connection):
				Receivekey: Md5(<BaseKey 4><MagicValueServerClient 1><RandomKeyPartClientA 2>)  7
	 - Note: The first 1024 Bytes will be _NOT_ discarded for UDP keys to safe CPU time

	- Handshake
			-> The handshake is encrypted - except otherwise noted - by the Keys created above
			-> Padding is currently not used for UDP meaning that PaddingLen will be 0, using PaddingLens up to 16 Bytes is acceptable however
		Client A: <SemiRandomNotProtocolMarker 1[Unencrypted]><RandomKeyPart 2[Unencrypted]><MagicValue 4><PaddingLen 1><RandomBytes PaddingLen%16>	

	- Overhead: 8 Bytes per UDP Packet
	
	- Security for Basic Obfuscation:
			- Random looking packets, very limited protection against passive eavesdropping single packets
	
	- Additional Comments:
			- For obvious reasons the UDP handshake is actually no handshake. If a different Encryption method (or better a different Key) is to be used this has to be negotiated in a TCP connection
		    - SemiRandomNotProtocolMarker is a Byte which has a value unequal any Protocol header byte. This is a compromiss, turning in complete randomness (and nice design) but gaining
			  a lower CPU usage

****************************** KAD Packets
			  
	-Keycreation Client <-> Client:
											(Used in general in request packets)
	 - Client A (Outgoing connection):
				Sendkey:	Md5(<KadID 16><RandomKeyPartClientA 2>)  18
	 - Client B (Incomming connection):
				Receivekey: Md5(<KadID 16><RandomKeyPartClientA 2>)  18
	               -- OR --					(Used in general in response packets)
	 - Client A (Outgoing connection):
				Sendkey:	Md5(<ReceiverKey 4><RandomKeyPartClientA 2>)  6
	 - Client B (Incomming connection):
				Receivekey: Md5(<ReceiverKey 4><RandomKeyPartClientA 2>)  6

	 - Note: The first 1024 Bytes will be _NOT_ discarded for UDP keys to safe CPU time

	- Handshake
			-> The handshake is encrypted - except otherwise noted - by the Keys created above
			-> Padding is currently not used for UDP meaning that PaddingLen will be 0, using PaddingLens up to 16 Bytes is acceptable however
		Client A: <SemiRandomNotProtocolMarker 6 Bits[Unencrypted]><Kad Marker 2Bit = 0 or 2><RandomKeyPart 2[Unencrypted]><MagicValue 4><PaddingLen 1><RandomBytes PaddingLen%16><ReceiverVerifyKey 4><SenderVerifyKey 4>

	- Overhead: 16 Bytes per UDP Packet
	
	- Kad/Ed2k Marker:
		 x 1	-> Most likely an ED2k Packet, try Userhash as Key first
		 0 0	-> Most likely an Kad Packet, try NodeID as Key first
		 1 0	-> Most likely an Kad Packet, try SenderKey as Key first

	- Additional Comments:
			- For obvious reasons the UDP handshake is actually no handshake. If a different Encryption method (or better a different Key) is to be used this has to be negotiated in a TCP connection
		    - SemiRandomNotProtocolMarker is a Byte which has a value unequal any Protocol header byte. This is a compromiss, turning in complete randomness (and nice design) but gaining
			  a lower CPU usage
		    - Kad/Ed2k Marker are only indicators, which possibility could be tried first, and need not be trusted
			- Packets which use the senderkey are prone to BruteForce attacks, which take only a few minutes (2^32)
			  which is while not acceptable for encryption fair enough for obfuscation
*/

#include "stdafx.h"
#include "EncryptedDatagramSocket.h"
#include "emule.h"
#include "MD5Sum.h"
#include "Preferences.h"
#include "opcodes.h"
#include "otherfunctions.h"
#ifdef _KAD_READY
#include "./kademlia/kademlia/prefs.h"
#include "./kademlia/kademlia/kademlia.h"
#endif
// random generator
#pragma warning(push, 3)	// preserve current state, then set warning level 3
#include "crypto51/osrng.h"
#pragma warning(pop)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define CRYPT_HEADER_WITHOUTPADDING		    8
#define	MAGICVALUE_UDP						91
#define MAGICVALUE_UDP_SYNC_CLIENT			0x395F2EC1
#define MAGICVALUE_UDP_SYNC_SERVER			0x13EF24D5
#define	MAGICVALUE_UDP_SERVERCLIENT			0xA5
#define	MAGICVALUE_UDP_CLIENTSERVER			0x6B

static CryptoPP::AutoSeededRandomPool cryptRandomGen;

CEncryptedDatagramSocket::CEncryptedDatagramSocket()
{
}

CEncryptedDatagramSocket::~CEncryptedDatagramSocket()
{
}

int CEncryptedDatagramSocket::DecryptReceivedClient(BYTE* pbyBufIn, int nBufLen, BYTE** ppbyBufOut, uint32 dwIP, uint32* nReceiverVerifyKey, uint32* nSenderVerifyKey) const
{
	int nResult = nBufLen;
	*ppbyBufOut = pbyBufIn;
	
	if (nReceiverVerifyKey == NULL || nSenderVerifyKey == NULL){
		ASSERT( false );
		return nResult;
	}
	
	*nReceiverVerifyKey = 0;
	*nSenderVerifyKey = 0;

	if (nResult <= CRYPT_HEADER_WITHOUTPADDING /*|| !thePrefs.IsClientCryptLayerSupported()*/)
		return nResult;	

	switch (pbyBufIn[0]){
		case OP_EMULEPROT:
		case OP_KADEMLIAPACKEDPROT:
		case OP_KADEMLIAHEADER:
		case OP_UDPRESERVEDPROT1:
		case OP_UDPRESERVEDPROT2:
		case OP_PACKEDPROT:
			return nResult; // not encrypted packet (see description on top)
	}

	// might be an encrypted packet, try to decrypt
	RC4_Key_Struct keyReceiveKey;
	uint32 dwValue = 0;
	// check the marker bit which type this packet could be and which key to test first, this is only an indicator since old clients have it set random
	// see the header for marker bits explanation
	unsigned	uiCurTry = pbyBufIn[0] & 0x03; 
	int		iTries;
	byte	abyteMD5[16];

#ifdef _KAD_READY
	if (Kademlia::CKademlia::GetPrefs() == NULL)
#else
	if (1)
#endif
	{
		// if kad never run, no point in checking anything except for ed2k encryption
		iTries = 1;
		uiCurTry = 1;
	}
	else
	{
		iTries = 3;
		if (uiCurTry == 3)
			uiCurTry = 1;
	}
	bool bKadRecvKeyUsed = false;
	bool bKad = false;
	do{
		if (uiCurTry == 0) {
			// kad packet with NodeID as key
			bKad = true;
			bKadRecvKeyUsed = false;
#ifdef _KAD_READY
			if (Kademlia::CKademlia::GetPrefs()) {
				uchar achKeyData[18];
				memcpy(achKeyData, Kademlia::CKademlia::GetPrefs()->GetKadID().GetData(), 16);
				memcpy(achKeyData + 16, pbyBufIn + 1, 2); // random key part sent from remote client
				MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
			}
#endif
		}
		else if (uiCurTry == 1) {
			// ed2k packet
			bKad = false;
			bKadRecvKeyUsed = false;
			uchar achKeyData[23];
			md4cpy(achKeyData, g_App.m_pPrefs->GetUserHash());
			achKeyData[20] = MAGICVALUE_UDP;
			POKE_DWORD(achKeyData + 16, dwIP);
			memcpy(achKeyData + 21, pbyBufIn + 1, 2); // random key part sent from remote client
			MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
		}
		else if (uiCurTry == 2) {
			// kad packet with ReceiverKey as key
			bKad = true;
			bKadRecvKeyUsed = true;
#ifdef _KAD_READY
			if (Kademlia::CKademlia::GetPrefs()) {
				uchar achKeyData[6];
				POKE_DWORD(achKeyData, Kademlia::CPrefs::GetUDPVerifyKey(dwIP));
				memcpy(achKeyData + 4, pbyBufIn + 1, 2); // random key part sent from remote client
				MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
			}
#endif
		}

		RC4CreateKey(abyteMD5, 16, &keyReceiveKey, true);
		RC4Crypt(pbyBufIn + 3, (uchar*)&dwValue, sizeof(dwValue), &keyReceiveKey);
		if (++uiCurTry >= 3)
			uiCurTry = 0;
	} while ((--iTries > 0) && (dwValue != MAGICVALUE_UDP_SYNC_CLIENT)); // try to decrypt as ed2k as well as kad packet if needed (max 3 rounds)
	
	if (dwValue == MAGICVALUE_UDP_SYNC_CLIENT){
		// yup this is an encrypted packet
		// the following cases are "allowed" but shouldn't happen given that there is only our implementation yet
		if (bKad)
		{
			if ((pbyBufIn[0] & 0x01) != 0)
				g_App.m_pMDlg->AddLogLine(LOG_FL_DBG, _T("Received obfuscated UDP packet from ClientIP: %s with wrong key marker bits (kad packet, ed2k bit)"), ipstr(dwIP));
			else if (!bKadRecvKeyUsed && (pbyBufIn[0] & 0x02) != 0)
				g_App.m_pMDlg->AddLogLine(LOG_FL_DBG, _T("Received obfuscated UDP packet from ClientIP: %s with wrong key marker bits (kad packet, nodeid key, recvkey bit)"), ipstr(dwIP));
			else if (bKadRecvKeyUsed && (pbyBufIn[0] & 0x02) == 0)
				g_App.m_pMDlg->AddLogLine(LOG_FL_DBG, _T("Received obfuscated UDP packet from ClientIP: %s with wrong key marker bits (kad packet, recvkey key, nodeid bit)"), ipstr(dwIP));
		}

		byte byPadLen;
		RC4Crypt(pbyBufIn + 7, &byPadLen, 1, &keyReceiveKey);
		nResult -= CRYPT_HEADER_WITHOUTPADDING;
		if (nResult <= byPadLen){
			g_App.m_pMDlg->AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, _T("Invalid obfuscated UDP packet from ClientIP: %s, Padding size (%u) larger than received bytes"), ipstr(dwIP), byPadLen);
			return nBufLen; // pass through, let the Receive function do the error handling on this junk
		}
		if (byPadLen > 0)
			RC4Crypt(NULL, NULL, byPadLen, &keyReceiveKey);
		nResult -= byPadLen;

		if (bKad){
			if (nResult <= 8){
				g_App.m_pMDlg->AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, _T("Obfuscated Kad packet with size mismatch (verifying keys missing) received from ClientIP: %s"), ipstr(dwIP));
				return nBufLen; // pass through, let the Receive function do the error handling on this junk;
			}
			// read the verifying keys
			RC4Crypt(pbyBufIn + CRYPT_HEADER_WITHOUTPADDING + byPadLen, (uchar*)nReceiverVerifyKey, 4, &keyReceiveKey);
			RC4Crypt(pbyBufIn + CRYPT_HEADER_WITHOUTPADDING + byPadLen + 4, (uchar*)nSenderVerifyKey, 4, &keyReceiveKey);
			nResult -= 8;
		}
		*ppbyBufOut = pbyBufIn + (nBufLen - nResult);
		RC4Crypt(*ppbyBufOut, *ppbyBufOut, nResult, &keyReceiveKey);
#if 0
		theStats.AddDownDataOverheadCrypt(nBufLen - nResult);
#endif
		//DEBUG_ONLY( DebugLog(_T("Received obfuscated UDP packet from clientIP: %s, Key: %s, RKey: %u, SKey: %u"), ipstr(dwIP), bKad ? (bKadRecvKeyUsed ? _T("ReceiverKey") : _T("NodeID")) : _T("UserHash")
		//	, *nReceiverVerifyKey, *nSenderVerifyKey) );
	}
	else{
		g_App.m_pMDlg->AddLogLine(LOG_FL_DBG | LOG_RGB_WARNING, _T("Obfuscated packet expected but magicvalue mismatch on UDP packet from ClientIP: %s"), ipstr(dwIP));
#ifdef _KAD_READY
		"Possible RecvKey: %u", Kademlia::CPrefs::GetUDPVerifyKey(dwIP)
#endif
		// pass through, let the Receive function do the error handling on this junk
	}
	return nResult;
}

// Encrypt packet. Key used:
// pachClientHashOrKadID != NULL									-> pachClientHashOrKadID
// pachClientHashOrKadID == NULL && bKad && nReceiverVerifyKey != 0 -> nReceiverVerifyKey
// else																-> ASSERT
int CEncryptedDatagramSocket::EncryptSendClient(uchar** ppbyBuf, int nBufLen, const uchar* pachClientHashOrKadID, bool bKad, uint32 nReceiverVerifyKey, uint32 nSenderVerifyKey) const
{
	ASSERT( g_App.GetPublicIP() != 0 || bKad );
	ASSERT( g_App.m_pPrefs->IsClientCryptLayerSupported() );
	ASSERT( pachClientHashOrKadID != NULL || nReceiverVerifyKey != 0 );
	ASSERT( (nReceiverVerifyKey == 0 && nSenderVerifyKey == 0) || bKad );

	byte byPadLen = 0;			// padding disabled for UDP currently
	const uint32 nCryptHeaderLen = byPadLen + CRYPT_HEADER_WITHOUTPADDING + (bKad ? 8 : 0);
	
	uint32 nCryptedLen = nBufLen + nCryptHeaderLen;
	uchar* pachCryptedBuffer = new uchar[nCryptedLen];
	bool bKadRecKeyUsed = false;
	byte	abyteMD5[16];
	uint16 nRandomKeyPart = (uint16)cryptRandomGen.GenerateWord32(0x0000, 0xFFFF);

	if (bKad){
		if ((pachClientHashOrKadID == NULL || (md4cmp0(pachClientHashOrKadID) == 0)) && nReceiverVerifyKey != 0) {
			bKadRecKeyUsed = true;
			uchar achKeyData[6];
			POKE_DWORD(achKeyData, nReceiverVerifyKey);
			POKE_WORD(achKeyData + 4, nRandomKeyPart);
			MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
			//DEBUG_ONLY( DebugLog(_T("Creating obfuscated Kad packet encrypted by ReceiverKey (%u)"), nReceiverVerifyKey) );
		}
		else if (pachClientHashOrKadID != NULL && (md4cmp0(pachClientHashOrKadID) != 0)) {
			uchar achKeyData[18];
			md4cpy(achKeyData, pachClientHashOrKadID);
			POKE_WORD(achKeyData + 16, nRandomKeyPart);
			MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
			//DEBUG_ONLY( DebugLog(_T("Creating obfuscated Kad packet encrypted by Hash/NodeID %s"), md4str(pachClientHashOrKadID)) );
		}
		else {
			ASSERT( false );
			delete[] pachCryptedBuffer;
			return nBufLen;
		}
	}
	else{
		uchar achKeyData[23];
		md4cpy(achKeyData, pachClientHashOrKadID);
		POKE_DWORD(achKeyData + 16, g_App.GetPublicIP());
		POKE_WORD(achKeyData + 21, nRandomKeyPart);
		achKeyData[20] = MAGICVALUE_UDP;
		MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5);
	}
	RC4_Key_Struct keySendKey;
	RC4CreateKey(abyteMD5, 16, &keySendKey, true);

	// create the semi random byte encryption header
	byte bySemiRandomNotProtocolMarker = 0;
	int i;
	for (i = 0; i < 128; i++){
		bySemiRandomNotProtocolMarker = cryptRandomGen.GenerateByte();
		bySemiRandomNotProtocolMarker = bKad ? (bySemiRandomNotProtocolMarker & 0xFE) : (bySemiRandomNotProtocolMarker | 0x01); // set the ed2k/kad marker bit
		if (bKad)
			bySemiRandomNotProtocolMarker = bKadRecKeyUsed ? ((bySemiRandomNotProtocolMarker & 0xFE) | 0x02) : (bySemiRandomNotProtocolMarker & 0xFC); // set the ed2k/kad and nodeid/reckey markerbit
		else
			bySemiRandomNotProtocolMarker = (bySemiRandomNotProtocolMarker | 0x01); // set the ed2k/kad marker bit
		
		switch (bySemiRandomNotProtocolMarker){ // not allowed values
			case OP_EMULEPROT:
			case OP_KADEMLIAPACKEDPROT:
			case OP_KADEMLIAHEADER:
			case OP_UDPRESERVEDPROT1:
			case OP_UDPRESERVEDPROT2:
			case OP_PACKEDPROT:
				continue;
		}
		break;
	}
	if (i >= 128){
		// either we have _really_ bad luck or the randomgenerator is a bit messed up
		ASSERT( false );
		bySemiRandomNotProtocolMarker = 0x01;
	}

	uint32 dwMagicValue = MAGICVALUE_UDP_SYNC_CLIENT;
	pachCryptedBuffer[0] = bySemiRandomNotProtocolMarker;
	POKE_WORD(pachCryptedBuffer + 1, nRandomKeyPart);
	RC4Crypt((uchar*)&dwMagicValue, pachCryptedBuffer + 3, 4, &keySendKey);
	RC4Crypt(&byPadLen, pachCryptedBuffer + 7, 1, &keySendKey);

	for (int j = 0; j < byPadLen; j++){
		byte byRand = (byte)rand();	// they actually don't really need to be random, but it doesn't hurt either
		RC4Crypt(&byRand, pachCryptedBuffer + CRYPT_HEADER_WITHOUTPADDING + j, 1, &keySendKey);
	}

	if (bKad){
		RC4Crypt((uchar*)&nReceiverVerifyKey, pachCryptedBuffer + CRYPT_HEADER_WITHOUTPADDING + byPadLen, 4, &keySendKey);
		RC4Crypt((uchar*)&nSenderVerifyKey, pachCryptedBuffer + CRYPT_HEADER_WITHOUTPADDING + byPadLen + 4, 4, &keySendKey);
	}

	RC4Crypt(*ppbyBuf, pachCryptedBuffer + nCryptHeaderLen, nBufLen, &keySendKey);
	delete[] *ppbyBuf;
	*ppbyBuf = pachCryptedBuffer;

#if 0
	theStats.AddUpDataOverheadCrypt(nCryptedLen - nBufLen);
#endif
	return nCryptedLen;
}

int CEncryptedDatagramSocket::DecryptReceivedServer(BYTE* pbyBufIn, int nBufLen, BYTE** ppbyBufOut, uint32 dwBaseKey, uint32 dbgIP) const
{
	int nResult = nBufLen;
	*ppbyBufOut = pbyBufIn;
	
	if (nResult <= CRYPT_HEADER_WITHOUTPADDING || !g_App.m_pPrefs->IsServerCryptLayerUDPEnabled() || dwBaseKey == 0)
		return nResult;
	
	if(pbyBufIn[0] == OP_EDONKEYPROT)
			return nResult; // no encrypted packet (see description on top)

	// might be an encrypted packet, try to decrypt
	uchar achKeyData[7];
	byte	abyteMD5[16];

	memcpy(achKeyData, &dwBaseKey, 4);
	achKeyData[4] = MAGICVALUE_UDP_SERVERCLIENT;
	memcpy(achKeyData + 5, pbyBufIn + 1, 2); // random key part sent from remote server

	RC4_Key_Struct keyReceiveKey;
	RC4CreateKey(MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5), 16, &keyReceiveKey, true);
	
	uint32 dwValue;
	RC4Crypt(pbyBufIn + 3, (uchar*)&dwValue, sizeof(dwValue), &keyReceiveKey);
	if (dwValue == MAGICVALUE_UDP_SYNC_SERVER)	// yup this is an encrypted packet
	{
		byte	byPadLen;
		RC4Crypt(pbyBufIn + 7, &byPadLen, 1, &keyReceiveKey);
		byPadLen &= 15;
		nResult -= CRYPT_HEADER_WITHOUTPADDING;
		if (nResult <= byPadLen)
		{
			g_App.m_pMDlg->AddLogLine(LOG_FL_DBG | LOG_RGB_ERROR, _T("Invalid obfuscated UDP packet from ServerIP: %s, Padding size (%u) larger than received bytes"), ipstr(dbgIP), byPadLen);
			return nBufLen; // pass through, let the Receive function do the error handling on this junk
		}
		if (byPadLen > 0)
			RC4Crypt(NULL, NULL, byPadLen, &keyReceiveKey);
		nResult -= byPadLen;
		*ppbyBufOut = pbyBufIn + (nBufLen - nResult);
		RC4Crypt(*ppbyBufOut, *ppbyBufOut, nResult, &keyReceiveKey);
		
#if 0
		theStats.AddDownDataOverheadCrypt(nBufLen - nResult);
#endif
	}
	else
	{
		g_App.m_pMDlg->AddLogLine(LOG_FL_DBG | LOG_RGB_WARNING, _T("Obfuscated packet expected but magicvalue mismatch on UDP packet from ServerIP: %s"), ipstr(dbgIP));
		// pass through, let the Receive function do the error handling on this junk
	}
	return nResult;
}

int CEncryptedDatagramSocket::EncryptSendServer(uchar** ppbyBuf, int nBufLen, uint32 dwBaseKey) const
{
	ASSERT( thePrefs.IsServerCryptLayerUDPEnabled() );
	ASSERT( dwBaseKey != 0 );
	
	byte	byPadLen = 0;			// padding disabled for UDP currently
	uint32 nCryptedLen = nBufLen + byPadLen + CRYPT_HEADER_WITHOUTPADDING;
	uchar* pachCryptedBuffer = new uchar[nCryptedLen];
	
	uint16 nRandomKeyPart = (uint16)cryptRandomGen.GenerateWord32(0x0000, 0xFFFF);

	uchar achKeyData[7];
	byte	abyteMD5[16];

	memcpy(achKeyData, &dwBaseKey, 4);
	achKeyData[4] = MAGICVALUE_UDP_CLIENTSERVER;
	memcpy(achKeyData + 5, &nRandomKeyPart, 2);

	RC4_Key_Struct keySendKey;
	RC4CreateKey(MD5Sum(achKeyData, sizeof(achKeyData), abyteMD5), 16, &keySendKey, true);

	// create the semi random byte encryption header
	byte	bySemiRandomNotProtocolMarker = 0;
	int i;
	for (i = 0; i < 128; i++){
		bySemiRandomNotProtocolMarker = cryptRandomGen.GenerateByte();
		if (bySemiRandomNotProtocolMarker != OP_EDONKEYPROT) // not allowed values
			break;
	}
	if (i >= 128){
		// either we have _real_ bad luck or the randomgenerator is a bit messed up
		ASSERT( false );
		bySemiRandomNotProtocolMarker = 0x01;
	}

	uint32 dwMagicValue = MAGICVALUE_UDP_SYNC_SERVER;
	pachCryptedBuffer[0] = bySemiRandomNotProtocolMarker;
	memcpy(pachCryptedBuffer + 1, &nRandomKeyPart, 2);
	RC4Crypt((uchar*)&dwMagicValue, pachCryptedBuffer + 3, 4, &keySendKey);
	RC4Crypt(&byPadLen, pachCryptedBuffer + 7, 1, &keySendKey);

	for (int j = 0; j < byPadLen; j++){
		byte	byRand = (byte)rand();	// they actually don't really need to be random, but it doesn't hurt either
		RC4Crypt(&byRand, pachCryptedBuffer + CRYPT_HEADER_WITHOUTPADDING + j, 1, &keySendKey);
	}
	RC4Crypt(*ppbyBuf, pachCryptedBuffer + CRYPT_HEADER_WITHOUTPADDING + byPadLen, nBufLen, &keySendKey);
	delete[] *ppbyBuf;
	*ppbyBuf = pachCryptedBuffer;

#if 0
	theStats.AddUpDataOverheadCrypt(nCryptedLen - nBufLen);
#endif
	return nCryptedLen;
}
