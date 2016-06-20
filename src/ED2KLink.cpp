//this file is part of eMule
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
#include "otherfunctions.h"
#include "ED2KLink.h"
#include "emule.h"
#include "StringConversion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


namespace {
	struct autoFree {
		autoFree(TCHAR* p) : m_p(p) {}
		~autoFree() { free(m_p); }
	private:
		TCHAR * m_p;
	};
}

CED2KLink::~CED2KLink()
{
}

/////////////////////////////////////////////
// CED2KServerListLink implementation
/////////////////////////////////////////////
CED2KServerListLink::CED2KServerListLink(const CString &strAddress) : m_strAddress(strAddress)
{
}

CED2KServerListLink::~CED2KServerListLink()
{
}

void CED2KServerListLink::GetLink(CString& lnk) const
{
	lnk.Format(_T("ed2k://|serverlist|%s|/"), m_strAddress);
}

CED2KServerListLink* CED2KServerListLink::GetServerListLink()
{
	return this;
}

CED2KLink::LinkType CED2KServerListLink::GetKind() const
{
	return kServerList;
}

/////////////////////////////////////////////
// CED2KServerLink implementation
/////////////////////////////////////////////
CED2KServerLink::CED2KServerLink(const CString &strAddr, const TCHAR *port) : m_strAddress(strAddr)
{
	unsigned long ul = _tcstoul(port, 0, 10);
	if ((ul > 0xFFFF) || (ul == 0))
		throw GetResString(IDS_ERR_BADPORT);
	m_uPort = static_cast<uint16>(ul);
	m_strDefName.Format(_T("Server %s:%s"), strAddr, port);
}

CED2KServerLink::~CED2KServerLink()
{
}

void CED2KServerLink::GetLink(CString& lnk) const
{
	lnk.Format(_T("ed2k://|server|%s|%u|/"), m_strAddress, m_uPort);
}

CED2KServerLink* CED2KServerLink::GetServerLink()
{
	return this;
}

CED2KLink::LinkType CED2KServerLink::GetKind() const
{
	return kServer;
}

///////////////////////////////////////////// 
// CED2KSearchLink implementation 
///////////////////////////////////////////// 
CED2KSearchLink::CED2KSearchLink(const CString &strSearchTerm) : m_strSearchTerm(strSearchTerm)
{
	m_strSearchTerm.Remove(_T('&'));
	m_strSearchTerm.Remove(_T('!'));
	m_strSearchTerm.Remove(_T('?'));
	m_strSearchTerm.Replace(_T('+'), _T(' '));
	m_strSearchTerm.Trim();
}

CED2KSearchLink::~CED2KSearchLink()
{
} 

void CED2KSearchLink::GetLink(CString& lnk) const
{
	lnk.Format(_T("ed2k://|search|%s|/"), Ed2kURIEncode(m_strSearchTerm));
}

CED2KSearchLink* CED2KSearchLink::GetSearchLink()
{
	return this;
}

CED2KLink::LinkType CED2KSearchLink::GetKind() const
{
	return (m_strSearchTerm.IsEmpty()) ? kInvalid : kSearch;
}

/////////////////////////////////////////////
// CED2KFileLink implementation
/////////////////////////////////////////////
CED2KFileLink::CED2KFileLink(const CString &strName, const TCHAR *pcSize, const CString &strHash, const CStringArray &astrParams, const TCHAR *pcSources)
: m_strName(strName)
{
	uint64	qwSz = _tstoi64(pcSize);

	if (qwSz > MAX_EMULE_FILE_SIZE)
		throw GetResString(IDS_ERR_TOOLARGEFILE);
	m_qwSize = qwSz;

	if (md4cmp0(StringToHash(strHash, m_hash)) == 0)
		throw GetResString(IDS_ERR_ILLFORMEDHASH);

	m_strName.Trim();
	if (m_strName.IsEmpty())
		throw GetResString(IDS_ERR_LINKERROR);

	ClientSourcesList = NULL;
	m_bAICHHashValid = false;

	for (int i = 0; i < astrParams.GetCount(); i++)
	{
		const CString	&strParam = astrParams.GetAt(i);
		int	iPos = strParam.Find(_T('='));
		const TCHAR	*pcHash;

		if (iPos < 0)
			continue;
		if ((iPos == CSTRLEN(_T("h"))) && (*strParam.GetString() == _T('h')))
		{
			pcHash = &strParam.GetString()[iPos + 1];
			if (*pcHash != 0)
			{
				if (DecodeBase32(pcHash, strParam.GetLength() - CSTRLEN(_T("h=")), m_abyteAICHHash, SHA_HASHSIZE) == SHA_HASHSIZE)
					m_bAICHHashValid = true;
			}
		}
	}

	if (pcSources)
	{
		TCHAR* pNewString = _tcsdup(pcSources);
		autoFree liberator(pNewString);
		TCHAR* pCh = pNewString;
		TCHAR* pEnd;
		TCHAR* pIP;
		TCHAR* pPort;

		bool bAllowSources;
		TCHAR date[3];
		COleDateTime expirationDate;
		int nYear, nMonth, nDay;

		uint32	dwIP, dwHybridID, dwCount = 0;
		uint16	uPort;
		unsigned long ul;

		int nInvalid = 0;

		pCh = stristr(pCh, _T("sources"));
		if (pCh != NULL)
		{
			pCh = pCh + CSTRLEN(_T("sources")); // point to char after "sources"
			pEnd = pCh;
			while (*pEnd != _T('\0'))
				pEnd++;	//	Make pEnd point to the terminating NULL

			bAllowSources = true;
			// if there's an expiration date...
			if( *pCh == _T('@') && (pEnd-pCh) > 7 )
			{
				pCh++; // after '@'
				date[2] = 0; // terminate the two character string
				date[0] = *(pCh++); date[1] = *(pCh++);
				nYear = _tcstol( date, 0, 10 ) + 2000;
				date[0] = *(pCh++); date[1] = *(pCh++);
				nMonth = _tcstol( date, 0, 10 );
				date[0] = *(pCh++); date[1] = *(pCh++);
				nDay = _tcstol( date, 0, 10 );
				bAllowSources = ( expirationDate.SetDate(nYear,nMonth,nDay) == 0 );
				if (bAllowSources)
					bAllowSources=(COleDateTime::GetCurrentTime() < expirationDate);
			}

			// increment pCh to point to the first "ip:port" or "address" and check for sources
			if ( bAllowSources && ++pCh < pEnd )
			{
				ClientSourcesList = new CTypedPtrList<CPtrList, CClientSource*>(); //<<-- enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
				// for each "ip:port"/"name:port" source string until the end
				// limit max number to protect against surprisingly long links
				while (*pCh != 0 && (dwCount < 512))
				{
					pIP = pCh;
					// find the end of this ip:port string & start of next ip:port string.
					if ((pCh = _tcschr(pCh, _T(','))) != NULL)
					{
						*pCh = 0; // terminate current "ip:port"/"name:port"
						pCh++;    // point to next "ip:port"/"name:port"
					}
					else
						pCh = pEnd;

					// if port is not present for this ip/name, go to the next ip/name.
					if( (pPort = _tcschr(pIP, _T(':'))) == NULL )
					{
						nInvalid++;	continue;
					}

					*pPort = 0;     // terminate ip/name string
					pPort++;	// point pPort to port string.

					dwIP = inet_addr( pIP );
					// START enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
					ul = _tcstoul( pPort, 0, 10 );
					uPort = static_cast<uint16>(ul);

					// skip bad ports
					if (ul > 0xFFFF || ul == 0)
					{
						nInvalid++;
						continue;
					}

					CClientSource* newsource = new CClientSource;
				//	could it be a hostname? We will assume this for later checking...
					if (dwIP == INADDR_NONE)
					{
						if (_tcslen(pIP) >= ARRSIZE(newsource->sourceHostname))
						{
							nInvalid++;
							delete newsource;
							continue;
						}
						newsource->sourceType = ED2KLINK_SOURCE_HOSTNAME;
						_tcscpy(newsource->sourceHostname, pIP);   //save hostname
					}
					else
					{
						dwHybridID = fast_ntohl(dwIP);
						if(IsLowID(dwHybridID))
						{
							nInvalid++;
							delete newsource;
							continue;
						}
						newsource->sourceType = ED2KLINK_SOURCE_IP;
						newsource->dwSrcIDHybrid = dwHybridID;
					}
					newsource->sourcePort = uPort; //save port
					md4cpy(newsource->filehashkey, m_hash); //save file hash

					ClientSourcesList->AddTail(newsource);
					dwCount++;
				}
				if (dwCount == 0)
				{
					delete ClientSourcesList;
					ClientSourcesList = NULL;
				}
				// END enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
			}
		}
	}
}


CED2KFileLink::~CED2KFileLink()
{
	delete ClientSourcesList;
	ClientSourcesList = NULL;
}

void CED2KFileLink::GetLink(CString& lnk) const
{
	TCHAR	acHashStr[MAX_HASHSTR_SIZE];

	lnk.Format(_T("ed2k://|file|%s|%I64u|%s|/"), Ed2kURIEncode(m_strName), m_qwSize, md4str(m_hash, acHashStr));
}

CED2KFileLink* CED2KFileLink::GetFileLink()
{
	return this;
}

CED2KLink::LinkType CED2KFileLink::GetKind() const
{
	return kFile;
}

CED2KLink* CED2KLink::CreateLinkFromUrl(const CString &strURI)
{
	CString strURIDecoded = URLDecode(strURI);
	int iPos = 0;
	CString strTok = strURIDecoded.Tokenize(_T("|"), iPos);

	if (strTok.CompareNoCase(_T("ed2k://")) == 0)
	{
		strTok = strURIDecoded.Tokenize(_T("|"), iPos);
		if (strTok.CompareNoCase(_T("file")) == 0)
		{
			CString	strName = strURIDecoded.Tokenize(_T("|"), iPos);
			if (!strName.IsEmpty())
			{
				CString	strSize = strURIDecoded.Tokenize(_T("|"), iPos);
				if (!strSize.IsEmpty())
				{
					CString	strHash = strURIDecoded.Tokenize(_T("|"), iPos);
					if (!strHash.IsEmpty())
					{
						CStringArray astrEd2kParams;
						bool bEmuleExt = false, bLastTokSlash = false;
						CString	strEmuleExt;

						for (;;)
						{
							strTok = strURIDecoded.Tokenize(_T("|"), iPos);
							if (strTok.IsEmpty())
								break;

							if (strTok == _T("/"))
							{
								bLastTokSlash = true;
								if (bEmuleExt)
									break;
								bEmuleExt = true;
							}
							else
							{
								bLastTokSlash = false;
								if (bEmuleExt)
								{
									if (!strEmuleExt.IsEmpty())
										strEmuleExt += _T('|');
									strEmuleExt += strTok;
								}
								else
									astrEd2kParams.Add(strTok);
							}
						}

						if (bLastTokSlash)
							return new CED2KFileLink(strName, strSize, strHash, astrEd2kParams, strEmuleExt.IsEmpty() ? (LPCTSTR)NULL : (LPCTSTR)strEmuleExt);
					}
				}
			}
		}
		else if (strTok.CompareNoCase(_T("serverlist")) == 0)
		{
			strTok = strURIDecoded.Tokenize(_T("|"), iPos);	//	URL
			if (!strTok.IsEmpty() && strURIDecoded.Tokenize(_T("|"), iPos) == _T("/"))
				return new CED2KServerListLink(strTok);
		}
		else if (strTok.CompareNoCase(_T("server")) == 0)
		{
			strTok = strURIDecoded.Tokenize(_T("|"), iPos);	//	Server
			if (!strTok.IsEmpty())
			{
				CString	strPort = strURIDecoded.Tokenize(_T("|"), iPos);
				if (!strPort.IsEmpty() && strURIDecoded.Tokenize(_T("|"), iPos) == _T("/"))
					return new CED2KServerLink(strTok, strPort);
			}
		}
		else if (strTok.CompareNoCase(_T("search")) == 0)
		{
			strTok = strURIDecoded.Tokenize(_T("|"), iPos);
			// might be extended with more parameters in future versions
			if (!strTok.IsEmpty() && !strURIDecoded.Tokenize(_T("|"), iPos).IsEmpty())
				return new CED2KSearchLink(strTok);
		}
	}

	throw GetResString(IDS_ERR_LINKERROR);
	return NULL;
}
