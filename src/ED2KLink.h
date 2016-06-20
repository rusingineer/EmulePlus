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
#pragma once

#define ED2KLINK_SOURCE_NOT_DEFINED	0
#define ED2KLINK_SOURCE_IP			1
#define ED2KLINK_SOURCE_HOSTNAME	2

class CClientSource;

class CED2KLink
{
public:
	static CED2KLink* CreateLinkFromUrl(const CString &strURI);
	virtual ~CED2KLink();

	typedef enum { kServerList, kServer, kFile, kSearch, kInvalid } LinkType;

	virtual LinkType GetKind() const = 0;
	virtual void GetLink(CString& lnk) const = 0;
	virtual class CED2KServerListLink* GetServerListLink()	{ return NULL; }
	virtual class CED2KServerLink* GetServerLink()			{ return NULL; }
	virtual class CED2KFileLink* GetFileLink()				{ return NULL; }
	virtual class CED2KSearchLink* GetSearchLink()			{ return NULL; }
};

class CED2KServerLink : public CED2KLink
{
public:
	CED2KServerLink(const CString &strIP, const TCHAR *port);
	virtual ~CED2KServerLink();
	// inherited from CED2KLink
	virtual LinkType GetKind() const;
	virtual CED2KServerLink* GetServerLink();
	virtual void GetLink(CString& lnk) const;

	// Accessors
	const CString& GetAddress() const	{ return m_strAddress; }
	uint16 GetPort() const				{ return m_uPort; }
	void GetDefaultName(CString &strDefName) const { strDefName = m_strDefName; }
private:
	CString	m_strAddress;	//	support non-ip ed2k server link
	CString	m_strDefName;
	uint16	m_uPort;
};

class CED2KFileLink : public CED2KLink
{
public:
	CED2KFileLink(const CString &strName, const TCHAR *pcSize, const CString &strHash, const CStringArray &astrParams, const TCHAR *pcSources);
	virtual ~CED2KFileLink();
	virtual LinkType GetKind() const;
	virtual CED2KFileLink* GetFileLink();
	virtual void GetLink(CString& lnk) const;
	const CString& GetName() const			{ return m_strName; }
	uint64 GetSize() const { return m_qwSize; }
	const uchar* GetHashKey() const { return m_hash; }
	const byte* GetAICHHash() const			{ return m_abyteAICHHash; }
	bool HasValidSources() const			{ return (ClientSourcesList != NULL); } //<<-- enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
	bool HasValidAICHHash() const			{ return m_bAICHHashValid; }

	CTypedPtrList<CPtrList, CClientSource*>* ClientSourcesList; //<<-- enkeyDEV(kei-kun) -hostname sources in ED2K superLink-

private:
	CString	m_strName;
	uint64	m_qwSize;
	uchar	m_hash[16];
	byte	m_abyteAICHHash[SHA_HASHSIZE];
	bool	m_bAICHHashValid;
};

class CED2KServerListLink : public CED2KLink
{
public:
	CED2KServerListLink(const CString &strAddress);
	virtual ~CED2KServerListLink();
	virtual LinkType GetKind() const;
	virtual CED2KServerListLink* GetServerListLink();
	virtual void GetLink(CString& lnk) const;
	CString GetAddress() const { return m_strAddress; }
private:
	CString m_strAddress;
};

class CED2KSearchLink : public CED2KLink
{
public:
	CED2KSearchLink(const CString &strSearchTerm);
	virtual ~CED2KSearchLink();

	virtual LinkType GetKind() const;
	virtual void GetLink(CString& lnk) const;
	virtual CED2KSearchLink* GetSearchLink();

	const TCHAR* GetSearchTerm() const { return m_strSearchTerm.GetString(); }

private:
	CString m_strSearchTerm;
};

// START - enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
class CClientSource
{
public:
	CClientSource()
	{
		sourceType = ED2KLINK_SOURCE_NOT_DEFINED;
		md4clr(filehashkey);
		md4clr(achUserHash);
		dwSrcIDHybrid = 0;
		sourcePort = 0;
		serverIP = 0;
		serverPort = 0;
	}

	uint32	dwSrcIDHybrid;   //not used for hostname sources
	uint32	serverIP;
	uint16	sourcePort;
	uint16	serverPort;
	byte	sourceType;   //0=not a valid source 1=IP source 2=hostname source
	uchar	achUserHash[16];
	uchar	filehashkey[16];
	TCHAR	sourceHostname[512]; //not used for IP sources
};
// END - enkeyDEV(kei-kun) -hostname sources in ED2K superLink-
