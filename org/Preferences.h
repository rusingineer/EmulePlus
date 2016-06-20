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
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once

#include "types.h"
#include "opcodes.h"
#include "otherfunctions.h"
#include "MD5Sum.h"
#include "securedvars.h"
#include "Category.h"

//	List columns
enum EnumDownloadListColumns
{
	DLCOL_FILENAME = 0,
	DLCOL_SIZE,
	DLCOL_TRANSFERRED,
	DLCOL_COMPLETED,
	DLCOL_SPEED,
	DLCOL_PROGRESS,
	DLCOL_NUMSOURCES,
	DLCOL_PRIORITY,
	DLCOL_STATUS,
	DLCOL_REMAINING,
	DLCOL_REMAININGTIME,
	DLCOL_ULDLRATIO,
	DLCOL_QLRATING,
	DLCOL_LASTSEENCOMPLETE,
	DLCOL_LASTRECEIVED,
	DLCOL_CATEGORY,
	DLCOL_WAITED,
	DLCOL_AVGSPEED,
	DLCOL_AVGREMTIME,
	DLCOL_ETA,
	DLCOL_AVGETA,

	DLCOL_NUMCOLUMNS
};

enum EnumUploadListColumns
{
	ULCOL_USERNAME = 0,
	ULCOL_FILENAME,
	ULCOL_SPEED,
	ULCOL_TRANSFERRED,
	ULCOL_WAITED,
	ULCOL_UPLOADTIME,
	ULCOL_STATUS,
	ULCOL_PARTS,
	ULCOL_PROGRESS,
	ULCOL_COMPRESSION,
	ULCOL_COUNTRY,

	ULCOL_NUMCOLUMNS
};

enum EnumUploadQueueListColumns
{
	QLCOL_USERNAME = 0,
	QLCOL_FILENAME,
	QLCOL_FILEPRIORITY,
	QLCOL_PARTS,
	QLCOL_PROGRESS,
	QLCOL_QLRATING,
	QLCOL_SCORE,
	QLCOL_SFRATIO,
	QLCOL_RFRATIO,
	QLCOL_TIMESASKED,
	QLCOL_LASTSEEN,
	QLCOL_ENTEREDQUEUE,
	QLCOL_BANNED,
	QLCOL_COUNTRY,

	QLCOL_NUMCOLUMNS
};

enum SearchListColumns
{
	SL_COLUMN_FILENAME = 0,
	SL_COLUMN_SIZE,
	SL_COLUMN_SOURCES,
	SL_COLUMN_TYPE,
	SL_COLUMN_FILEHASH,
	SL_COLUMN_FAKECHECK,
	SL_COLUMN_LASTSEENCOMPLETE,
	SL_COLUMN_FOLDER,
	SL_COLUMN_LENGTH,
	SL_COLUMN_BITRATE,
	SL_COLUMN_CODEC,

	SL_NUMCOLUMNS
};

enum EnumSharedFilesListColumns
{
	SFL_COLUMN_FILENAME = 0,
	SFL_COLUMN_FILESIZE,
	SFL_COLUMN_TYPE,
	SFL_COLUMN_PRIORITY,
	SFL_COLUMN_PERMISSION,
	SFL_COLUMN_FILEID,
	SFL_COLUMN_REQUESTS,
	SFL_COLUMN_ACCEPTED,
	SFL_COLUMN_TRANSFERRED,
	SFL_COLUMN_PARTTRAFFIC,
	SFL_COLUMN_UPLOADS,
	SFL_COLUMN_COMPLETESRC,
	SFL_COLUMN_FOLDER,

	SFL_NUMCOLUMNS
};

enum ServerListColumns
{
	SL_COLUMN_SERVERNAME = 0,
	SL_COLUMN_SERVERIP,
	SL_COLUMN_DESCRIPTION,
	SL_COLUMN_PING,
	SL_COLUMN_NUMUSERS,
	SL_COLUMN_NUMFILES,
	SL_COLUMN_PREFERENCES,
	SL_COLUMN_FAILEDCOUNT,
	SL_COLUMN_STATIC,
	SL_COLUMN_SOFTFILELIMIT,
	SL_COLUMN_SOFTWAREVER,
	SL_COLUMN_COUNTRY,
	SL_COLUMN_LOWIDUSERS,

	SL_COLUMN_NUMCOLUMNS
};

enum EnumIRC2ListColumns
{
	IRC2COL_NAME = 0,
	IRC2COL_USERS,
	IRC2COL_DESCRIPTION,

	IRC2COL_NUMCOLUMNS
};

enum EnumClientListColumns
{
	CLCOL_USERNAME = 0,
	CLCOL_UPLOADSTATUS,
	CLCOL_TRANSFERREDUP,
	CLCOL_DOWNLOADSTATUS,
	CLCOL_TRANSFERREDDOWN,
	CLCOL_CLIENTSOFTWARE,
	CLCOL_CONNECTEDTIME,
	CLCOL_USERHASH,
	CLCOL_COUNTRY,

	CLCOL_NUMCOLUMNS
};

enum EnumFDPartsListColumns
{
	FDPARTSCOL_NUMBER = 0,
	FDPARTSCOL_COMPLETESIZE,
	FDPARTSCOL_SOURCES,
	FDPARTSCOL_STATUS,

	FDPARTSCOL_NUMCOLUMNS
};

enum EnumFriendListColumns
{
	FRIENDCOL_USERNAME = 0,
	FRIENDCOL_LASTSEEN,

	FRIENDCOL_NUMCOLUMNS
};

enum
{
	EP_SEARCHMETHOD_SERV = 0,
	EP_SEARCHMETHOD_GLOB,
	EP_SEARCHMETHOD_WEB1,
	EP_SEARCHMETHOD_WEB2,

	EP_SEARCHMETHOD_COUNT
};

//	WebServer specific parameters
typedef struct
{
	bool	bShowUploadQueue;
	bool	bShowUploadQueueBanned;
	bool	bShowUploadQueueFriend;
	bool	bShowUploadQueueCredit;
	bool	bMenuLocked;
	bool	abDownloadColHidden[9];
	bool	abUploadColHidden[5];
	bool	abQueueColHidden[4];
	bool	abSharedColHidden[7];
	bool	abServerColHidden[10];
	bool	abSearchColHidden[5];
} WSPrefParams;

//	Shortcut indexes
enum
{
	SCUT_WIN_MINIMIZE,
	SCUT_WIN_SWITCH,
	SCUT_WIN_SRV,
	SCUT_WIN_TRANSFER,
	SCUT_WIN_SEARCH,
	SCUT_WIN_SHAREDFILES,
	SCUT_WIN_CHAT,
	SCUT_WIN_IRC,
	SCUT_WIN_STATS,
	SCUT_WIN_PREFS,
	SCUT_DL_CANCEL,
	SCUT_DL_STOP,
	SCUT_DL_PAUSE,
	SCUT_DL_RESUME,
	SCUT_FILE_OPEN,
	SCUT_FILE_OPENDIR,
	SCUT_DL_PREVIEW,
	SCUT_FILE_RENAME,
	SCUT_FILE_COMMENTS,
	SCUT_FILE_DETAILS,
	SCUT_DL_FD_SOURCES,
	SCUT_DL_CLEAR,
	SCUT_DL_CLEARALL,
	SCUT_DL_SHOWALL,
	SCUT_DL_DEFSORT,
	SCUT_DL_PREALLOC,
	SCUT_DL_A4AF,
	SCUT_DL_A4AFAUTO,
	SCUT_DL_A4AFOTHER,
	SCUT_DL_A4AFSAMECAT,
	SCUT_LINK,
	SCUT_LINK_HTML,
	SCUT_LINK_SOURCE,
	SCUT_SRC_DETAILS,
	SCUT_SRC_FRIEND,
	SCUT_SRC_MSG,
	SCUT_SRC_SHAREDFILES,
	SCUT_FILE_EDITCOMMENTS,
	SCUT_LINK_HASH,
	SCUT_FILE_NAMECLEANUP,

	SCUT_COUNT
};

//	Default values
#define PREF_DEF_DOWNCAP			1000
#define PREF_DEF_UPCAP				240
#define PREF_DEF_MM_PORT			80
#define PREF_DEF_WS_PORT			4711
#define PREF_DEF_PROXY_PORT			1080
#define PREF_DEF_STATREE_MASK		_T("111000000100000110000010000011110000010010")

#define PREF_MIN_NEWUSER_RNDPORT	4000


__inline uint32 ValidateDownCapability(uint32 dwCap)
{
	dwCap &= 0xFFFF;
	return (dwCap == 0) ? PREF_DEF_DOWNCAP : dwCap;
}

__inline uint32 ValidateUpCapability(uint32 dwCap)
{
	dwCap &= 0xFFFF;
	return (dwCap == 0) ? PREF_DEF_UPCAP : dwCap;
}


#pragma pack(1)
// DO NOT EDIT VALUES like making a uint16 to uint32, or insert any value. ONLY append new vars
struct Preferences_Ext_Struct
{
	byte	version;
	uchar	abyteUserHash[16];
	WINDOWPLACEMENT EmuleWindowPlacement;
};
#pragma pack()

struct ProxySettings
{
	uint16		m_nType;
	uint16		m_uPort;
	CStringA	m_strName;
	CStringA	m_strUser;
	CStringA	m_strPassword;
	bool		m_bEnablePassword;
	bool		m_bUseProxy;
};

typedef enum
{
	SEE_SHARE_EVERYBODY = 0,
	SEE_SHARE_FRIENDS = 1,
	SEE_SHARE_NOONE = 2
} enSeeShare;

typedef enum
{
	HASH_PRIORITY_IDLE = 0,
	HASH_PRIORITY_LOWER = 1,
	HASH_PRIORITY_STANDARD = 2
} enHashPriority;

// Permission values for shared files
#define PERM_ALL		0
#define PERM_FRIENDS	1
#define PERM_NOONE		2

class CPreferences
{
public:
	enum EnumTable
	{
		TABLE_DOWNLOAD,
		TABLE_DOWNLOAD2,
		TABLE_UPLOAD,
		TABLE_QUEUE,
		TABLE_SEARCH,
		TABLE_SHARED,
		TABLE_SERVER,
		TABLE_IRC,
		TABLE_CLIENTLIST,
		TABLE_PARTSTATUS,
		TABLE_FRIENDLIST
	};

	CPreferences();
	~CPreferences();

	CString	GetAppDir() const							{ return m_strAppDir; }
	CString GetConfigDir() const						{ return m_strConfigDir; }
	bool	Save();

	void	SaveCats();
	void	LoadCats();

	bool	IsShareableDirectory(const CString& rstrDirectory) const;
	bool	IsInstallationDirectory(const CString& rstrDir) const;

	uint16	Profile()									{ return m_nProfile; }
	void	SetProfile(uint16 uProfile)					{ m_nProfile = uProfile; }
	bool	GetUseServerPriorities() const				{ return m_bUseServerPriorities; }
	void	SetUseServerPriorities(bool bVal)			{ m_bUseServerPriorities = bVal; }
	bool	Reconnect()									{ return m_bReconnect; }
	void	SetReconnect(bool bReconnect)				{ m_bReconnect = bReconnect; }
	byte	DeadServer()								{ return m_nDeadServer; }
	void	SetDeadServer(byte iDead)					{ m_nDeadServer = iDead; }
	CString	GetUserNick()								{ return m_sNick.Get(); }
	void	SetUserNick(const CString& strNewNick)
	{
		m_sNick.Put(strNewNick);
		SetUserNickTag();
	}
	uint16	GetPort()									{ return m_uPort; }
	void	SetPort(uint16 uPort)						{ m_uPort = uPort; }

	uint16	GetListenPort()								{ return (m_bUseProxyListenPort) ? m_nListenPort : GetPort(); }
	void	SetListenPort(uint16 uPort)					{ m_nListenPort = uPort; m_bUseProxyListenPort = true; }
	void	ResetListenPort()							{ m_nListenPort = 0; m_bUseProxyListenPort = false; }

	uint16	GetUDPPort()								{ return m_uUDPPort; }
	void	SetUDPPort(uint16 uPort)					{ m_uUDPPort = uPort; }
	bool	GetOpenPorts()						{ return m_bOpenPorts; }
	void	SetOpenPorts(bool bEnable)			{ m_bOpenPorts = bEnable; }
	CString	GetIncomingDir()							{ return m_sIncomingDir.Get(); }
	void	SetIncomingDir(const CString& strDir)		{ m_sIncomingDir.Put(strDir); }
	CString	GetTempDir()								{ return m_sTempDir.Get(); }
	LPCTSTR	GetTempDir() const							{ return m_sTempDir.Get(); }
	void	SetTempDir(const CString& strDir)			{ m_sTempDir.Put(strDir); }
	CString	GetVideoPlayer()  							{ return m_sVideoPlayer.Get(); }
	void    SetVideoPlayer(const CString& str)			{ m_sVideoPlayer.Put(str); }
	uchar*	GetUserHash()								{ return m_userHash; }
	uint16	GetMaxUpload() const						{ return m_uMaxUpload; }

	bool	IsUAPEnabled()								{ return m_bUAP; }
	void	SetUAPEnabled(bool bEnable)					{ m_bUAP = bEnable; }
	bool	IsDAPEnabled()								{ return m_bDAP; }
	void	SetDAPEnabled(bool bEnable)					{ m_bDAP = bEnable; }
	bool	IsDisabledXS()								{ return m_bDisableXS; }
	void	SetDisabledXS(bool bDisable)				{ m_bDisableXS = bDisable; }
	bool	StartDownloadPaused()						{ return m_bStartDownloadPaused; }
	void	SetStartDownloadPaused(bool bPaused)		{ m_bStartDownloadPaused = bPaused; }
	bool	DownloadPausedOnComplete()					{ return m_bDownloadPausedOnComplete; }
	void	SetDownloadPausedOnComplete(bool bPaused)	{ m_bDownloadPausedOnComplete = bPaused; }
	bool	IsResumeOtherCat()							{ return m_bResumeOtherCat; }
	void	SetResumeOtherCat(bool bEnable)				{ m_bResumeOtherCat = bEnable; }
	bool	IsAutoClearCompleted()						{ return m_bAutoClearCompleted; }
	void	SetAutoClearCompleted(bool bClear)			{ m_bAutoClearCompleted = bClear; }
	bool	IsAutoSourcesEnabled()						{ return m_bAutoSourcesEnabled; }
	void	SetAutoSourcesEnabled(bool bAutoS)			{ m_bAutoSourcesEnabled = bAutoS; }
	uint32	GetMinAutoSourcesPerFile()					{ return m_nMinAutoSourcesPerFile; }
	void	SetMinAutoSourcesPerFile(uint32 nMinAuto)	{ m_nMinAutoSourcesPerFile = nMinAuto; }
	uint32	GetMaxAutoSourcesPerFile()					{ return m_nMaxAutoSourcesPerFile; }
	void	SetMaxAutoSourcesPerFile(uint32 nMaxAuto)	{ m_nMaxAutoSourcesPerFile = nMaxAuto; }
	uint32	GetMaxAutoSourcesTotal()					{ return m_nMaxAutoSourcesTotal; }
	void	SetMaxAutoSourcesTotal(uint32 nMaxAuto)		{ m_nMaxAutoSourcesTotal = nMaxAuto; }
	uint32	GetMaxAutoExchangeSources()					{ return m_nMaxAutoExchangeSources; }
	void	SetMaxAutoExchangeSources(uint32 nMaxAuto)	{ m_nMaxAutoExchangeSources = nMaxAuto; }
	bool	IsAutoSourcesLogEnabled()					{ return m_bAutoSourcesLogEnabled; }
	void	SetAutoSourcesLogEnabled(bool bEnabled)		{ m_bAutoSourcesLogEnabled = bEnabled; }
	bool	IsClientTransferLogEnabled()				{ return m_bClientTransferLogEnabled; }
	void	SetClientTransferLogEnabled(bool bEnabled)	{ m_bClientTransferLogEnabled = bEnabled; }
	CString GetToolbarSettings() 						{ return m_sToolbarSettings.Get(); }
	CString GetDefaultToolbarSettings() const 			{ return CString(_T("0099010203040506079908")); }
	void	SetToolbarSettings(const CString &str) 				{ m_sToolbarSettings.Put(str); }
	CString GetToolbarBitmapSettings() 					{ return m_sToolbarBitmap.Get(); }
	void	SetToolbarBitmapSettings(const CString& path) { m_sToolbarBitmap.Put(path); }
	CString GetToolbarBitmapFolderSettings() 			{ return m_sToolbarBitmapFolder.Get(); }
	void	SetToolbarBitmapFolderSettings(const CString& path){ m_sToolbarBitmapFolder.Put(path); }
	byte	GetToolbarLabelSettings()					{ return m_nToolbarLabels; }
	void	SetToolbarLabelSettings(byte settings)		{ m_nToolbarLabels = settings; }
	bool	AutoServerlist()							{ return m_bAutoServerList; }
	void	SetAutoServerlist(bool bAuto)				{ m_bAutoServerList = bAuto; }
	uchar*	GetA4AFHash(void)							{ return m_A4AF_FileHash; }
	void	SetA4AFHash(const uchar* file_hash)			{ md4cpy(m_A4AF_FileHash, file_hash); }

//	WebServer
	uint16	GetWSPort()									{ return m_nWebPort; }
	void	SetWSPort(uint16 uPort)						{ m_nWebPort = uPort; }
	byte*	GetWSPass(byte *pbyteHash)									{ return reinterpret_cast<byte*>(m_sWebPassword.Get(reinterpret_cast<HashType*>(pbyteHash))); }
	void	SetWSPass(const CString& strNewPass)		{ HashType Digest; m_sWebPassword.Put(*reinterpret_cast<const HashType*>(MD5Sum(strNewPass, Digest.hash))); }
	byte*	GetWSLowPass(byte *pbyteHash)								{ return reinterpret_cast<byte*>(m_sLowWebPassword.Get(reinterpret_cast<HashType*>(pbyteHash))); }
	void	SetWSLowPass(const CString& strNewPass)		{ HashType Digest; m_sLowWebPassword.Put(*reinterpret_cast<const HashType*>(MD5Sum(strNewPass, Digest.hash))); }
	bool	GetWSIsEnabled()							{ return m_bWebEnabled; }
	void	SetWSIsEnabled(bool bEnable)				{ m_bWebEnabled = bEnable; }
	bool	GetWSIsLowUserEnabled()						{ return m_bWebLowEnabled; }
	void	SetWSIsLowUserEnabled(bool bEnable)			{ m_bWebLowEnabled = bEnable; }
	int		GetWebPageRefresh()							{ return m_nWebPageRefresh; }
	void	SetWebPageRefresh(int nRefresh)				{ m_nWebPageRefresh = nRefresh; }
	WSPrefParams*	GetWSPrefsPtr()						{ return &m_WSPrefs; }

	bool	UpdateNotify()								{ return m_bUpdateNotify; }
	bool	DoMinToTray()								{ return m_bMinToTray; }
	void	SetMinToTray(bool bMinToTray)				{ m_bMinToTray = bMinToTray; }
	bool*	GetMinTrayPTR()								{ return &m_bMinToTray; }
	bool	DoAutoConnect()								{ return m_bAutoConnect; }
	void	SetAutoConnect(bool bConnect)				{ m_bAutoConnect = bConnect; }
	uint32	BadClientBanTime()							{ return m_nBadClientBanTime; }
	void	SetBadClientBanTime(uint32 uTime)			{ m_nBadClientBanTime = uTime; }
	uint32	BadClientMinRequestTime()					{ return m_nMinRequestTime; }
	void	SetBadClientMinRequestTime(uint32 uTime)	{ m_nMinRequestTime = uTime; }
	byte	BadClientMinRequestNum()					{ return m_nBadClientBan; }
	void	SetBadClientMinRequestNum(byte uVal)		{ m_nBadClientBan = uVal; }
	bool	IsBanMessageEnabled()						{ return m_bBanMessageEnabled; }
	void	SetBanMessageEnabled(bool bEnable)			{ m_bBanMessageEnabled = bEnable; }

	bool	BanEnabled()								{ return m_bBanEnabled; }
	void	SetBanEnable(bool bEnable)					{ m_bBanEnabled = bEnable; }

	bool	ShowRatingIcons()							{ return m_bShowRatingIcons; }
	void	SetShowRatingIcons(bool bEnable)			{ m_bShowRatingIcons = bEnable; }

	uint32	WhenSourcesOutdated()						{ return m_nSLSOutdated; }
	void	SetWhenSourcesOutdated(uint32 uVal)			{ m_nSLSOutdated = uVal;}
	uint32	SLSMaxSourcesPerFile()						{ return m_SLSMaxSourcesPerFile; }
	void	SetSLSMaxSourcesPerFile(uint32 MaxSrc)		{ m_SLSMaxSourcesPerFile = MaxSrc; }
	bool	SLSEnable()									{ return m_bSLSEnable; }
	void	SetSLSEnable(bool bEnable)					{ m_bSLSEnable = bEnable; }

	void	SaveStats(int bBackUp = 0);
	void	SetRecordStructMembers();
	void	SaveCompletedDownloadsStat();
	bool	LoadStats(int loadBackUp = 0, CString sFileName = _T(""));
	void	ResetStatistics();

	//		Functions from base code that update original cumulative stats, now obsolete. (KHAOS)
	//void	Add2TotalDownloaded(uint64 in) {totalDownloadedBytes+=in;}
	//void	Add2TotalUploaded(uint64 in) {totalUploadedBytes+=in;}
	//		End functions from base code.

	//		Add to, increment and replace functions.  They're all named Add2 for the sake of some kind of naming
	//		convention.
	void	Add2DownCompletedFiles()					{ cumDownCompletedFiles++; }
	void	Add2ConnMaxAvgDownRate(double in)			{ cumConnMaxAvgDownRate = in; }
	void	Add2ConnMaxDownRate(double in)				{ cumConnMaxDownRate = in; }
	void	Add2ConnAvgUpRate(double in)				{ cumConnAvgUpRate = in; }
	void	Add2ConnMaxAvgUpRate(double in)				{ cumConnMaxAvgUpRate = in; }
	void	Add2ConnMaxUpRate(double in)				{ cumConnMaxUpRate = in; }
	void	Add2ConnPeakConnections(int in)				{ cumConnPeakConnections = static_cast<uint16>(in); }
	void	Add2UpAvgTime(int in)						{ cumUpAvgTime = in; }
	void	Add2DownSAvgTime(int in)					{ sesDownAvgTime += in; }
	void	Add2DownCAvgTime(int in)					{ cumDownAvgTime = in; }
	void	Add2ConnTransferTime(int in)				{ cumConnTransferTime += in; }
	void	Add2ConnDownloadTime(int in)				{ cumConnDownloadTime += in; }
	void	Add2ConnUploadTime(int in)					{ cumConnUploadTime += in; }
	void	Add2DownSessionCompletedFiles()				{ sesDownCompletedFiles++; }
	void	Add2SessionTransferData (byte uClientID, BOOL bFromPF, EnumTransferType eUpDown, uint32 bytes, BOOL bCommunity, byte byteFilePriority);
	void	Add2DownSuccessfulSessions()				{ sesDownSuccessfulSessions++;
														  cumDownSuccessfulSessions++; }
	void	Add2DownFailedSessions()					{ sesDownFailedSessions++;
														  cumDownFailedSessions++; }
	void	Add2DownFailedSessionsNoRequiredData()		{ sesDownFailedSessionsNoRequiredData++; }
	void	Add2LostFromCorruption(uint32 dwAdd)		{ m_qwSesLostFromCorruption += dwAdd; }
	void	SubLostFromCorruption(uint32 dwSub)			{ if (m_qwSesLostFromCorruption >= dwSub)
															m_qwSesLostFromCorruption -= dwSub;
														  else if (m_qwCumLostFromCorruption)
															m_qwCumLostFromCorruption -= dwSub; }
	void	Add2SavedFromCompression(sint32 iVal)		{ m_qwSesSavedFromCompression += iVal; }
	void	IncSessionPartsSavedByICH()					{ m_dwSesPartsSavedByICH++; }

	//		Functions that return stats stuff...
	//		Saved stats for cumulative downline overhead
	uint64	GetDownOverheadTotal()						{ return cumDownOverheadTotal;}
	uint64	GetDownOverheadFileReq()					{ return cumDownOverheadFileReq;}
	uint64	GetDownOverheadSrcEx()						{ return cumDownOverheadSrcEx;}
	uint64	GetDownOverheadServer()						{ return cumDownOverheadServer;}
	uint64	GetDownOverheadOther()						{ return cumDownOverheadOther;}
	uint64	GetDownOverheadTotalPackets()				{ return cumDownOverheadTotalPackets;}
	uint64	GetDownOverheadFileReqPackets() 			{ return cumDownOverheadFileReqPackets;}
	uint64	GetDownOverheadSrcExPackets()				{ return cumDownOverheadSrcExPackets;}
	uint64	GetDownOverheadServerPackets()				{ return cumDownOverheadServerPackets;}
	uint64	GetDownOverheadOtherPackets()				{ return cumDownOverheadOtherPackets;}

	//		Saved stats for cumulative upline overhead
	uint64	GetUpOverheadTotal()						{ return cumUpOverheadTotal;}
	uint64	GetUpOverheadFileReq()						{ return cumUpOverheadFileReq;}
	uint64	GetUpOverheadSrcEx()						{ return cumUpOverheadSrcEx;}
	uint64	GetUpOverheadServer()						{ return cumUpOverheadServer;}
	uint64	GetUpOverheadOther()						{ return cumUpOverheadOther;}
	uint64	GetUpOverheadTotalPackets()					{ return cumUpOverheadTotalPackets;}
	uint64	GetUpOverheadFileReqPackets()				{ return cumUpOverheadFileReqPackets;}
	uint64	GetUpOverheadSrcExPackets()					{ return cumUpOverheadSrcExPackets;}
	uint64	GetUpOverheadServerPackets()				{ return cumUpOverheadServerPackets;}
	uint64	GetUpOverheadOtherPackets()					{ return cumUpOverheadOtherPackets;}

	//		Saved stats for cumulative upline data
	uint32	GetUpSuccessfulSessions()					{ return cumUpSuccessfulSessions;}
	uint32	GetUpFailedSessions()						{ return cumUpFailedSessions;}
	uint32	GetUpAvgTime()								{ return cumUpAvgTime;}

	//		Saved stats for cumulative downline data
	uint32	GetDownCompletedFiles()						{ return cumDownCompletedFiles;}
	uint16	GetDownC_SuccessfulSessions()				{ return cumDownSuccessfulSessions;}
	uint16	GetDownC_FailedSessions()					{ return cumDownFailedSessions;}
	uint32	GetDownC_AvgTime()							{ return cumDownAvgTime;}
	//		Session download stats
	uint16	GetDownSessionCompletedFiles()				{ return sesDownCompletedFiles;}
	uint16	GetDownS_SuccessfulSessions()				{ return sesDownSuccessfulSessions;}
	uint16	GetDownS_FailedSessions()					{ return sesDownFailedSessions;}
	uint16	GetDownS_FailedSessionsNoRequiredData()		{ return sesDownFailedSessionsNoRequiredData;}
	uint32	GetDownS_AvgTime()							{ return GetDownS_SuccessfulSessions()?sesDownAvgTime/GetDownS_SuccessfulSessions():0;}

	//	Saved stats for corruption/compression
	uint64	GetCumLostFromCorruption() const			{ return m_qwCumLostFromCorruption; }
	uint64	GetSesLostFromCorruption() const			{ return m_qwSesLostFromCorruption; }
	uint64	GetCumSavedFromCompression() const			{ return m_qwCumSavedFromCompression; }
	uint64	GetSesSavedFromCompression() const			{ return m_qwSesSavedFromCompression; }
	uint32	GetCumPartsSavedByICH() const				{ return m_dwCumPartsSavedByICH; }
	uint32	GetSesPartsSavedByICH() const				{ return m_dwSesPartsSavedByICH; }

	// Cumulative client breakdown stats for sent bytes
	uint64	GetUpTotalClientData()						{	uint64 transferred_data = 0;
															for (int i = 0; i<SO_LAST; i++)
															{
																transferred_data += cumUpDataClients[i];
																transferred_data += sesUpDataClients[i];
															}
															return transferred_data;
														}
	uint64	GetCumUpData(EnumClientTypes eClientID)		{ return (cumUpDataClients[eClientID]+sesUpDataClients[eClientID]); }

	// Session client breakdown stats for sent bytes
	uint64	GetUpSessionClientData()					{	uint64 transferred_data = 0;
															for (int i = 0; i<SO_LAST; i++) transferred_data += sesUpDataClients[i];
															return transferred_data;
														}
	uint64	GetUpData(EnumClientTypes eClientID)		{ return sesUpDataClients[eClientID]; }

	// Cumulative DS breakdown stats for sent bytes...
	uint64	GetUpDataByPriority(byte bytePrio)			{ return ((bytePrio < ARRSIZE(sesUpDataPriority)) ? sesUpDataPriority[bytePrio] : 0);}

	// Cumulative DS breakdown stats for sent bytes...
	uint64	GetUpTotalDataFile()						{ return (GetCumUpData_File() + GetCumUpData_PartFile() );}
	uint64	GetCumUpData_File()							{ return (cumUpData_File + sesUpData_File );}
	uint64	GetCumUpData_PartFile()						{ return (cumUpData_PartFile + sesUpData_PartFile );}

	// Session DS breakdown stats for sent bytes...
	uint64	GetUpSessionDataFile()						{ return (sesUpData_File + sesUpData_PartFile );}
	uint64	GetUpData_File()							{ return sesUpData_File;}
	uint64	GetUpData_PartFile()						{ return sesUpData_PartFile;}

	// Cumulative community breakdown stats for sent bytes...
	uint64	GetUpTotalDataCommunity()					{ return (GetCumUpData_Community() + GetCumUpData_NoCommunity() );}
	uint64	GetCumUpData_Community()					{ return (cumUpData_Community + sesUpData_Community );}
	uint64	GetCumUpData_NoCommunity()					{ return (cumUpData_NoCommunity + sesUpData_NoCommunity );}

	// Session community breakdown stats for sent bytes...
	uint64	GetUpSessionDataCommunity()					{ return (sesUpData_Community + sesUpData_NoCommunity );}
	uint64	GetUpData_Community()						{ return sesUpData_Community;}
	uint64	GetUpData_NoCommunity()						{ return sesUpData_NoCommunity;}

	// Cumulative client breakdown stats for received bytes
	uint64	GetDownTotalClientData()					{	uint64 transferred_data = 0;
															for (int i = 0; i<SO_LAST; i++)
															{
																transferred_data += cumDownDataClients[i];
																transferred_data += sesDownDataClients[i];
															}
															return transferred_data;
														}
	uint64	GetCumDownData(EnumClientTypes eClientID)		{ return (cumDownDataClients[eClientID]+sesDownDataClients[eClientID]); }

	// Session client breakdown stats for received bytes
	uint64	GetDownSessionClientData()					{	uint64 transferred_data = 0;
															for (int i = 0; i<SO_LAST; i++) transferred_data += sesDownDataClients[i];
															return transferred_data;
														}
	uint64	GetDownData(EnumClientTypes eClientID)		{ return sesDownDataClients[eClientID]; }

	//		Saved stats for cumulative connection data
	double	GetConnAvgDownRate()						{ return cumConnAvgDownRate;}
	double	GetConnMaxAvgDownRate()						{ return cumConnMaxAvgDownRate;}
	double	GetConnMaxDownRate()						{ return cumConnMaxDownRate;}
	double	GetConnAvgUpRate()							{ return cumConnAvgUpRate;}
	double	GetConnMaxAvgUpRate()						{ return cumConnMaxAvgUpRate;}
	double	GetConnMaxUpRate()							{ return cumConnMaxUpRate;}
	uint32	GetConnRunTime() const						{ return dwCumConnRunTime; }
	uint16	GetConnNumReconnects()						{ return cumConnNumReconnects;}
	uint16	GetConnAvgConnections()						{ return cumConnAvgConnections;}
	uint16	GetConnMaxConnLimitReached()				{ return cumConnMaxConnLimitReached;}
	uint16	GetConnPeakConnections()					{ return cumConnPeakConnections;}
	uint32	GetConnTransferTime()						{ return cumConnTransferTime;}
	uint32	GetConnDownloadTime()						{ return cumConnDownloadTime;}
	uint32	GetConnUploadTime()							{ return cumConnUploadTime;}
	uint32	GetConnServerDuration()						{ return cumConnServerDuration;}

	//		Saved records for servers / network
	uint16	GetSrvrsMostWorkingServers()				{ return cumSrvrsMostWorkingServers;}
	uint32	GetSrvrsMostUsersOnline()					{ return cumSrvrsMostUsersOnline;}
	uint32	GetSrvrsMostFilesAvail()					{ return cumSrvrsMostFilesAvail;}

	//		Saved records for shared files
	uint16	GetSharedMostFilesShared()					{ return cumSharedMostFilesShared; }
	uint64	GetSharedLargestShareSize()					{ return m_qwCumSharedLargestShareSize; }
	uint64	GetSharedLargestAvgFileSize()				{ return m_qwCumSharedLargestAvgFileSize; }
	uint64	GetSharedLargestFileSize()					{ return m_qwCumSharedLargestFileSize; }

	//		Get the long date/time when the stats were last reset
	time_t	GetStatsLastResetLng()						{ return stat_datetimeLastReset;}
	CString	GetStatsLastResetStr();

	//		Get and Set our new preferences
	void		SetExpandedTreeItems(const CString &in)	{ m_strStatsExpandedTreeItems = in; }
	CString	GetExpandedTreeItems() const				{ return m_strStatsExpandedTreeItems; }
	// <-----khaos- End Statistics Methods

	uint16	GetQueueSize()								{ return m_nQueueSize; }
	void	SetQueueSize(uint16 iSize)					{ m_nQueueSize = iSize; }

	bool	GetAddServersFromServer() const				{ return m_bAddServersFromServer; }
	void	SetAddServersFromServer(bool bAdd)			{ m_bAddServersFromServer = bAdd; }
	bool	GetAddServersFromClients() const			{ return m_bAddServersFromClients; }
	void	SetAddServersFromClients(bool bAdd)			{ m_bAddServersFromClients = bAdd; }
	uint16	GetTrafficOMeterInterval() 					{ return m_nTrafficOMeterInterval; }
	void	SetTrafficOMeterInterval(uint16 in) 		{ m_nTrafficOMeterInterval = in; }
	uint16	GetStatsInterval()							{ return m_nStatsInterval; }
	void	SetStatsInterval(uint16 in)					{ m_nStatsInterval = in; }
	uint16	GetDetailColumnWidth()						{ return m_nDetailColumnWidth; }
	void	SetDetailColumnWidth(uint16 in)				{ m_nDetailColumnWidth = in; }
	void	Add2TotalDownloaded(uint64 in)				{ uint64 nSet = m_nTotalDownloadedBytes.Get(); nSet += in; m_nTotalDownloadedBytes.Put(nSet); }
	uint64	GetTotalDownloaded()						{ return m_nTotalDownloadedBytes.Get(); }
	void	Add2TotalUploaded(uint64 in)				{ uint64 nSet = m_nTotalUploadedBytes.Get(); nSet += in; m_nTotalUploadedBytes.Put(nSet); }
	uint64	GetTotalUploaded()							{ return m_nTotalUploadedBytes.Get(); }
	bool	IsErrorBeepEnabled()						{ return m_bBeepOnError; }
	void	SetErrorBeepEnabled(bool bEnable)			{ m_bBeepOnError = bEnable; }
	bool	IsConfirmExitEnabled()						{ return m_bConfirmExit; }
	bool	IsConfirmDisconnectEnabled()				{ return m_bConfirmDisconnect; }
	bool	IsConfirmFriendDelEnabled()					{ return m_bConfirmFriendDel; }
	void	SetConfirmExitEnabled(bool bEnable)			{ m_bConfirmExit = bEnable; }
	void	SetConfirmDisconnectEnabled(bool bEnable)	{ m_bConfirmDisconnect = bEnable; }
	void	SetConfirmFriendDelEnabled(bool bEnable)	{ m_bConfirmFriendDel = bEnable; }
	bool	UseSplashScreen()							{ return m_bSplashScreen; }
	void	SetUseSplashScreen(bool bUse)				{ m_bSplashScreen = bUse; }
	bool	IsFilterBadIPs()							{ return m_bFilterBadIP; }
	void	SetFilterBadIPs(bool bFilter)				{ m_bFilterBadIP = bFilter; }
	bool	IsFilterServersByIP()						{ return m_bFilterServersByIP; }
	void	SetFilterServersByIP (bool bFilter)			{ m_bFilterServersByIP = bFilter; }
	bool	IsOnlineSignatureEnabled()					{ return m_bOnlineSig; }
	void	SetOnlineSignatureEnabled(bool bEnable)		{ m_bOnlineSig = bEnable; }

	uint32	GetMaxGraphUploadRate() const				{ return m_dwMaxGraphUploadRate; }
	void	SetMaxGraphUploadRate(uint32 in)			{ m_dwMaxGraphUploadRate = in; }
	uint32	GetMaxGraphDownloadRate() const				{ return m_dwMaxGraphDownloadRate; }
	void	SetMaxGraphDownloadRate(uint32 in)			{ m_dwMaxGraphDownloadRate = in; }

	uint16	GetMaxDownload();
	uint16	GetMaxConnections()							{ return m_nMaxConnections; }
	void	SetMaxConnections(uint16 uMaxConn)			{ m_nMaxConnections = uMaxConn; }
	uint16	GetMaxSourcePerFile()						{ return m_nMaxSourcePerFile; }
	void	SetMaxSourcePerFile(uint32 dwVal)			{ m_nMaxSourcePerFile = static_cast<uint16>(dwVal); }
	uint16	GetMaxSourcePerFileSoft() const				{ unsigned ui = static_cast<unsigned>(m_nMaxSourcePerFile) * 9u / 10u; return static_cast<uint16>((ui > MAX_SOURCES_FILE_SOFT) ? MAX_SOURCES_FILE_SOFT : ui); }
	uint16	GetMaxSourcePerFileUDP() const				{ unsigned ui = (static_cast<unsigned>(m_nMaxSourcePerFile) * 3u) >> 2u; return static_cast<uint16>((ui > MAX_SOURCES_FILE_UDP)? MAX_SOURCES_FILE_UDP : ui); }
	uint16	GetDeadserverRetries()						{ return m_nDeadServerRetries; }
	void	SetDeadserverRetries(uint16 uRetries)		{ m_nDeadServerRetries = uRetries; }

	int		GetColumnWidth(EnumTable t, int index) const;
	BOOL	GetColumnHidden(EnumTable t, int index) const;
	int		GetColumnOrder(EnumTable t, int index) const;
	void	SetColumnWidth(EnumTable t, int index, int width);
	void	SetColumnHidden(EnumTable t, int index, bool bHidden);
	void	SetColumnOrder(EnumTable t, int *piOrder);

	int		GetColumnSortItem(EnumTable t) const;
	bool	GetColumnSortAscending(EnumTable t) const;
	void	SetColumnSortItem(EnumTable t, int sortItem);
	void	SetColumnSortAscending(EnumTable t, bool sortAscending);

	bool	GetMultiple()								{ return m_bMultiple; }
	void	SetMultiple(bool bEnable)					{ m_bMultiple = bEnable; }

	WORD	GetLanguageID()								{ return m_nLanguageID; }
	void	SetLanguageID(WORD wID)						{ m_nLanguageID = wID; }
	enSeeShare CanSeeShares(void)						{ return m_nSeeShares; }
	void	SetCanSeeShares(enSeeShare iCan)			{ m_nSeeShares = iCan; }
	byte	GetFilePermission(void) const				{ return m_byteFilePermission; }
	void	SetFilePermission(byte bytePerm)			{ m_byteFilePermission = bytePerm; }
	byte	GetToolTipDelay(void)						{ return m_nTooltipDelayTime; }
	void	SetToolTipDelay(byte iDelay)				{ m_nTooltipDelayTime = iDelay; }
	bool	IsBringToFront()							{ return m_bBringToForeground; }
	void	SetBringToFront(bool bBTFront)				{ m_bBringToForeground = bBTFront; }

	uint16	GetStatsMax()								{ return m_nStatsMax; }
	void	SetStatsMax(uint16 uMaxStats)				{ m_nStatsMax = uMaxStats; }
	byte	GetStatsAverageMinutes()					{ return m_nStatsAverageMinutes; }
	void	SetStatsAverageMinutes(byte in)				{ m_nStatsAverageMinutes = in; }

	bool	GetShowToolbarSpeedMeter()					{ return m_bShowToolbarSpeedMeter; }
	void	SetShowToolbarSpeedMeter(bool bShow)		{ m_bShowToolbarSpeedMeter = bShow; }
	bool	GetShowCountryFlag()						{ return m_bShowCountryFlag; }
	void	SetShowCountryFlag(bool bShow)				{ m_bShowCountryFlag = bShow; }
	bool	GetCloseToTray()							{ return m_bCloseToTray; }
	void	SetCloseToTray(bool bClose)					{ m_bCloseToTray = bClose; }
	bool	GetDetailsOnClick()							{ return m_bDetailsOnClick; }
	void	SetDetailsOnClick(bool bSet)				{ m_bDetailsOnClick = bSet; }
	bool	GetBugReport() const						{ return m_bBugReport; }
	bool	AllowLocalHostIP() const					{ return m_bAllowLocalHostIP; }

	bool	GetUseDownloadAddNotifier()					{ return m_bUseDownloadAddNotifier; }
	void	SetUseDownloadAddNotifier(bool bUse)		{ m_bUseDownloadAddNotifier = bUse; }
	bool	GetUseDownloadNotifier()					{ return m_bUseDownloadNotifier; }
	void	SetUseDownloadNotifier(bool bUse)			{ m_bUseDownloadNotifier = bUse; }
	bool	GetUseChatNotifier()						{ return m_bUseChatNotifier; }
	void	SetUseChatNotifier(bool bUse)				{ m_bUseChatNotifier = bUse; }
	bool	GetUseLogNotifier()							{ return m_bUseLogNotifier; }
	void	SetUseLogNotifier(bool bUse)				{ m_bUseLogNotifier = bUse; }
	bool	GetUseSoundInNotifier()						{ return m_bUseSoundInNotifier; }
	void	SetUseSoundInNotifier(bool bUse)			{ m_bUseSoundInNotifier = bUse; }
	bool	GetUseSchedulerNotifier()					{ return m_bUseSchedulerNotifier; }
	void	SetUseSchedulerNotifier(bool bUse)			{ m_bUseSchedulerNotifier = bUse; }
	bool	GetNotifierPopsEveryChatMsg()				{ return m_bNotifierPopsEveryChatMsg; }
	void	SetNotifierPopsEveryChatMsg(bool bPop)		{ m_bNotifierPopsEveryChatMsg = bPop; }
	bool	GetNotifierPopOnImportantError()			{ return m_bNotifierImportantError; }
	void	SetNotifierPopOnImportantError(bool bPop)	{ m_bNotifierImportantError = bPop; }
	bool	GetNotifierPopOnWebServerError()			{ return m_bNotifierWebServerError; }
	void	SetNotifierPopOnWebServerError(bool bPop)	{ m_bNotifierWebServerError = bPop; }
	bool	GetNotifierPopOnServerError()				{ return m_bNotifierServerError; }
	void	SetNotifierPopOnServerError(bool bPop)		{ m_bNotifierServerError = bPop; }
	CString	GetNotifierWavSoundPath()					{ return m_sNotifierSoundFilePath.Get(); }
	void	SetNotifierWavSoundPath(const CString& strPath)	{ m_sNotifierSoundFilePath.Put(strPath); }

	CString	GetIRCNick()								{ return m_sIRCNick.Get(); }
	void	SetIRCNick(const CString &strVal)			{ m_sIRCNick.Put(strVal); }
	CString	GetIRCServer()								{ return m_sIRCServer.Get(); }
	void	SetIRCServer(const CString& in_serv )		{ m_sIRCServer.Put(in_serv); }
	bool	GetIRCAddTimestamp()						{ return m_bIRCAddTimestamp; }
	void	SetIRCAddTimestamp(bool bVal)				{ m_bIRCAddTimestamp = bVal; }
	CString	GetIRCChanNameFilter()						{ return m_sIRCChanNameFilter.Get(); }
	void	SetIRCChanNameFilter(const CString& in_name){ m_sIRCChanNameFilter.Put(in_name); }
	bool	GetIRCUseChanFilter()						{ return m_bIRCUseChanFilter; }
	void	SetIRCUseChanFilter(bool bVal)				{ m_bIRCUseChanFilter = bVal; }
	uint16	GetIRCChannelUserFilter()					{ return m_nIRCChannelUserFilter; }
	void	SetIRCChanUserFilter(uint16 in_user)		{ m_nIRCChannelUserFilter = in_user; }
	CString	GetIrcPerformString()						{ return m_sIRCPerformString.Get(); }
	void	SetIRCPerformString(const CString& in_perf)	{ m_sIRCPerformString.Put(in_perf);}
	bool	GetIrcUsePerform()							{ return m_bIRCUsePerform; }
	void	SetIrcUsePerform(bool flag)					{ m_bIRCUsePerform = flag; }
	bool	GetIRCListOnConnect()						{ return m_bIRCListOnConnect; }
	void	SetIRCListonConnect(bool flag)				{ m_bIRCListOnConnect = flag; }
	bool	GetIrcIgnoreInfoMessage()					{ return m_bIRCIgnoreInfoMessage; }
	void	SetIrcIgnoreInfoMessage(bool flag)			{ m_bIRCIgnoreInfoMessage = flag; }
	bool	GetIrcIgnoreEmuleProtoInfoMessage()			{ return m_bIRCIgnoreEmuleProtoInfoMessage; }
	void	SetIrcIgnoreEmuleProtoInfoMessage(bool flag){ m_bIRCIgnoreEmuleProtoInfoMessage = flag;}
	WORD	GetWindowsVersion();
	bool	GetStartMinimized()							{ return m_bStartMinimized; }
	void	SetStartMinimized(bool bStart)				{ m_bStartMinimized = bStart; }
	bool	GetSmartIdCheck()							{ return m_bSmartIdCheck; }
	void	SetSmartIdCheck(bool in_smartidcheck)		{ m_bSmartIdCheck = in_smartidcheck; }
	byte	GetSmartIdState()							{ return m_nSmartIdState; }
	void	SetSmartIdState(byte in_smartidstate)		{ m_nSmartIdState = in_smartidstate; }

	bool	GetVerbose()								{ return m_bVerbose; }
	void	SetVerbose(bool bVal)						{ m_bVerbose = bVal; }
	bool	GetManuallyAddedServerHighPrio() const		{ return m_bManuallyAddedServerHighPrio; }
	void	SetManuallyAddedServerHighPrio(bool bVal)	{ m_bManuallyAddedServerHighPrio = bVal; }
	bool	ShowOverhead()								{ return m_bShowOverhead;}
	void	SetShowOverhead(bool bit)					{ m_bShowOverhead = bit;}
	bool	BackupPreview()								{ return m_bBackupPreview;}
	void	SetBackupPreview(bool bit)					{ m_bBackupPreview = bit;}

	CString	GetTxtEditor() const						{ return m_strTxtEditor; }

	uint32	GetFakesDatVersion()						{return m_nFakesDatVersion;}
	void	SetFakesDatVersion(uint32 version)			{m_nFakesDatVersion = version;}
	bool	IsUpdateFakeStartupEnabled()				{return m_bUpdateFakeStartup;}
	void	SetUpdateFakeStartup(bool in)				{m_bUpdateFakeStartup = in;}
	CString	GetFakeListURL(void)						{return m_sFakeListURL;}
	void	SetFakeListURL(const CString &strUrl)		{m_sFakeListURL = strUrl;}
	uint32	GetDLingFakeListVersion() const				{ return m_dwDLingFakeListVersion; }
	void	SetDLingFakeListVersion(uint32 dwVersion)	{ m_dwDLingFakeListVersion = dwVersion; }
	CString	GetDLingFakeListLink() const				{ return m_strDLingFakeListLink; }
	void	SetDLingFakeListLink(const CString& strLink){ m_strDLingFakeListLink = strLink; }
	COLORREF	GetFakeListDownloadColor() const		{ return m_clrFakeListDownloadColor; }
	void	SetFakeListDownloadColor(COLORREF clr)		{ m_clrFakeListDownloadColor = clr; }

	bool	IsAutoCheckForNewVersion()					{return m_bAutoCheckForNewVersion;}
	void	SetAutoCheckForNewVersion(bool in)			{m_bAutoCheckForNewVersion = in;}
	int		GetAutoCheckLastTime()						{return m_iAutoCheckLastTime;}
	void	SetAutoCheckLastTime(int in)				{m_iAutoCheckLastTime = in;}

	bool	UseFlatBar() const							{ return (m_byteDepth3D == 0); }
	byte	Get3DDepth() const							{ return m_byteDepth3D; }
	void	Set3DDepth(byte byteStyle)					{ m_byteDepth3D = byteStyle; }
	bool	AutoConnectStaticOnly()						{ return m_bAutoConnectStaticOnly; }
	void	SetAutoConnectStaticOnly(bool bEnabled)		{m_bAutoConnectStaticOnly = bEnabled;}

	COLORREF	GetStatsColor(int iIdx) const			{ return m_dwStatColors[iIdx]; }
	void	SetStatsColor(int iIdx, COLORREF val)		{ m_dwStatColors[iIdx] = val; }
	byte	GetGraphRatio()								{ return m_nGraphRatio; }
	void	SetGraphRatio(byte iNum)					{ m_nGraphRatio = iNum; }
	bool	DoUseSort()									{ return m_bUseSort; }
	void	SetUseSort(bool bUseSort)					{ m_bUseSort = bUseSort; }
	bool	DoUseSrcSortCol2()							{ return m_bUseSrcSortCol2; }
	void	SetUseSrcSortCol2(bool bUseSort)			{ m_bUseSrcSortCol2 = bUseSort; }
	bool	DoPausedStoppedLast()						{ return m_bPausedStoppedLast; }
	void	SetPausedStoppedLast(bool bEnable)			{ m_bPausedStoppedLast = bEnable; }
	byte	GetServerSortCol()							{ return m_nServerSortCol; }
	void	SetServerSortCol(byte iCol)					{ m_nServerSortCol = iCol; }
	byte	GetUploadSortCol()							{ return m_nUploadSortCol; }
	void	SetUploadSortCol(byte iSortCol)				{ m_nUploadSortCol = iSortCol; }
	byte	GetQueueSortCol()							{ return m_nQueueSortCol; }
	void	SetQueueSortCol(byte iSortCol)				{ m_nQueueSortCol = iSortCol; }
	byte	GetSearchSortCol()							{ return m_nSearchSortCol; }
	void	SetSearchSortCol(byte iSortCol)				{ m_nSearchSortCol = iSortCol; }
	byte	GetIrcSortCol()								{ return m_nIRCSortCol; }
	void	SetIrcSortCol(byte iSortCol)				{ m_nIRCSortCol = iSortCol; }
	byte	GetClientListSortCol()						{ return m_nClientListSortCol; }
	void	SetClientListSortCol(byte iSortCol)			{ m_nClientListSortCol = iSortCol; }
	uint16	GetFileSortCol()							{ return m_nFileSortCol; }
	void	SetFileSortCol(uint16 uCol)					{ m_nFileSortCol = uCol; }
	byte	GetDownloadSortCol()						{ return m_nDownloadSortCol; }
	void	SetDownloadSortCol(byte iSortCol)			{ m_nDownloadSortCol = iSortCol; }
	byte	GetSrcSortCol1()							{ return m_nSrcSortCol1; }
	void	SetSrcSortCol1(byte iSortCol)				{ m_nSrcSortCol1 = iSortCol; }
	byte	GetSrcSortCol2()							{ return m_nSrcSortCol2; }
	void	SetSrcSortCol2(byte iSortCol)				{ m_nSrcSortCol2 = iSortCol; }

	void	SetMaxDownloadConperFive(int in)			{ m_nMaxConPerFive = static_cast<uint16>(in); }
	uint16	GetMaxConPerFive()							{ return m_nMaxConPerFive; }
	uint16	GetDefaultMaxConPerFiveSecs();

	COLORREF	GetDefaultStatsColor(unsigned uiIdx);

	//--- xrmb:partTrafficPrefs ---
	bool	DoUsePT()									{ return m_bUsePartTraffic; }
	void	SetUsePT(bool in)							{ m_bUsePartTraffic = in; }
	byte	GetUpbarStyle()								{ return m_nUpbarPartTraffic; }
	void	SetUpbarStyle(byte uStyle)					{ m_nUpbarPartTraffic = uStyle; }
	byte	GetUpbarColor()								{ return m_nUpbarColorPartTraffic; }
	void	SetUpbarColor(byte uColor)					{ m_nUpbarColorPartTraffic = uColor; }
	//--- :xrmb ---

	bool	CommunityEnabled()							{ return m_bCommunityEnabled; }
	void	SetCommunityEnabled(bool bEnable)			{ m_bCommunityEnabled = bEnable; }
	CString	CommunityString()							{ return m_sCommunityString.Get(); }
	void	SetCommunityString(const CString& strComm)	{ m_sCommunityString.Put(strComm); }
	bool	CommunityNoBanEnabled()						{ return m_bCommunityNoBan; }
	void	SetCommunityNoBanEnabled(bool bEnable)		{ m_bCommunityNoBan = bEnable; }
	uint32	NotificationDisplayTime()					{ return m_nNotificationDisplayTime; }
	void	SetNotificationDisplayTime(uint32 uTime)	{ m_nNotificationDisplayTime = uTime; }
	uint32	NotificationFontSize()						{ return m_nNotificationFontSize; }
	void	SetNotificationFontSize(uint32 uFontSize)	{ m_nNotificationFontSize = uFontSize; }
	uint16	PriorityHigh()								{ return m_nPriorityHigh; }
	void	SetPriorityHigh(uint16 iPrio)				{ m_nPriorityHigh = iPrio; }
	uint16	PriorityLow()								{ return m_nPriorityLow; }
	void	SetPriorityLow(uint16 iPrio)				{ m_nPriorityLow = iPrio; }
	bool	RestartWaiting()							{ return m_bRestartWaiting; }
	void	SetRestartWaiting(bool bSet)				{ m_bRestartWaiting = bSet; }
	bool	LogToFile()									{ return m_bLogToFile; }
	void	SetLogToFile(bool bEnable)					{ m_bLogToFile = bEnable; }
	bool	LogUploadToFile()							{ return m_bLogUploadToFile; }
	void	SetLogUploadToFile(bool bEnable)			{ m_bLogUploadToFile = bEnable; }
	bool	LogDownloadToFile() const					{ return m_bLogDownloadToFile; }
	void	SetLogDownloadToFile(bool bEnable)			{ m_bLogDownloadToFile = bEnable; }
	bool	DisableXSUpTo()								{ return m_bDisableXSUpTo; }
	void	SetDisableXSUpTo(bool bDisable)				{ m_bDisableXSUpTo = bDisable; }
	uint16	XSUpTo()									{ return m_nXSUpTo; }
	void	SetXSUpTo(uint32 dwVal)						{ m_nXSUpTo = static_cast<uint16>(dwVal); }
	uint32	SrvConTimeout()								{ return m_nSrvConTimeout; }
	void	SetSrvConTimeout(uint32 timeout)			{ m_nSrvConTimeout = timeout; }
	uint16	SlowCompleteBlockSize()						{ return m_nSlowCompleteBlockSize; }
	void	SetSlowCompleteBlockSize(uint16 uSize)		{ m_nSlowCompleteBlockSize = uSize; }
	CString	GetUsedFont()								{ return m_sUsedFont.Get(); }
	void	SetUsedFont(const CString& font)			{ m_sUsedFont.Put(font); }
	byte	GetFontSize()								{ return m_nFontSize; }
	void	SetFontSize(byte size)						{ m_nFontSize = size; }
	enHashPriority	GetHashingPriority(void)			{ return m_HashingPriority; }
	void	SetHashingPriority(enHashPriority hashing)	{ m_HashingPriority = hashing; }
	bool	IsUploadPartsEnabled()						{ return m_bUploadParts; }
	void	SetUploadPartsEnabled(bool bEnable)			{ m_bUploadParts = bEnable; }
	bool	IsA4AFStringEnabled()						{ return m_bA4AFStringEnabled; }
	void	SetA4AFStringEnabled(bool bEnable)			{ m_bA4AFStringEnabled = bEnable; }
	bool	IsA4AFCountEnabled()						{ return m_bA4AFCountEnabled; }
	void	SetA4AFCountEnabled(bool bEnable)			{ m_bA4AFCountEnabled = bEnable; }
	byte	GetSearchMethod() const						{ return m_byteSearchMethod; }
	void	SetSearchMethod(byte byteValue)				{ m_byteSearchMethod = byteValue; }
	bool	GetKeepSearchHistory() const				{ return m_bKeepSearchHistory; }
	void	SetKeepSearchHistory(bool bVal)				{ m_bKeepSearchHistory = bVal; }
	bool	AutoTakeED2KLinks()							{ return m_bAutoTakeLinks; }
	void	SetAutoTakeED2KLinks(bool bEnable)			{ m_bAutoTakeLinks = bEnable; }
	uint16	GetFileBufferSize()							{ return m_nFileBufferSize; }
	void	SetFileBufferSize(int Value)				{ m_nFileBufferSize = static_cast<uint16>(Value); }
	int		GetMainProcessPriority()					{ return m_nMainProcessPriority; }
	void	SetMainProcessPriority(int Value);

	bool	IsSCHEnabled()								{ return m_bSCHEnabled; }
	void	SetSCHEnabled(bool bEnable)					{ m_bSCHEnabled = bEnable; }
	bool	IsSCHExceptMon()							{ return m_bSCHExceptMon; }
	void	SetSCHExceptMon(bool bEnable)				{ m_bSCHExceptMon = bEnable; }
	bool	IsSCHExceptTue()							{ return m_bSCHExceptTue; }
	void	SetSCHExceptTue(bool bEnable)				{ m_bSCHExceptTue = bEnable; }
	bool	IsSCHExceptWed()							{ return m_bSCHExceptWed; }
	void	SetSCHExceptWed(bool bEnable)				{ m_bSCHExceptWed = bEnable; }
	bool	IsSCHExceptThu()							{ return m_bSCHExceptThu; }
	void	SetSCHExceptThu(bool bEnable)				{ m_bSCHExceptThu = bEnable; }
	bool	IsSCHExceptFri()							{ return m_bSCHExceptFri; }
	void	SetSCHExceptFri(bool bEnable)				{ m_bSCHExceptFri = bEnable; }
	bool	IsSCHExceptSat()							{ return m_bSCHExceptSat; }
	void	SetSCHExceptSat(bool bEnable)				{ m_bSCHExceptSat = bEnable; }
	bool	IsSCHExceptSun()							{ return m_bSCHExceptSun; }
	void	SetSCHExceptSun(bool bEnable)				{ m_bSCHExceptSun = bEnable; }
	uint32	GetSCHShift1()								{ return m_nSCHShift1; }
	void	SetSCHShift1(uint32 Time)					{ m_nSCHShift1 = Time; }
	uint32	GetSCHShift2()								{ return m_nSCHShift2; }
	void	SetSCHShift2(uint32 Time)					{ m_nSCHShift2 = Time; }
	uint16	GetSCHShift1Upload() const					{ return m_uSCHShift1Upload; }
	void	SetSCHShift1Upload(uint16 Value)			{ m_uSCHShift1Upload = Value; }
	uint16	GetSCHShift1Download() const				{ return m_uSCHShift1Download; }
	void	SetSCHShift1Download(uint16 Value)			{ m_uSCHShift1Download = Value; }
	uint16	GetSCHShift1conn()							{ return m_nSCHShift1conn; }
	void	SetSCHShift1conn(uint16 Value)				{ m_nSCHShift1conn = Value; }
	uint16	GetSCHShift15sec()							{ return m_nSCHShift15sec; }
	void	SetSCHShift15sec(uint16 Value)				{ m_nSCHShift15sec = Value; }
	uint16	GetSCHShift2Upload() const					{ return m_uSCHShift2Upload; }
	void	SetSCHShift2Upload(uint16 Value)			{ m_uSCHShift2Upload = Value; }
	uint16	GetSCHShift2Download() const				{ return m_uSCHShift2Download; }
	void	SetSCHShift2Download(uint16 Value)			{ m_uSCHShift2Download = Value; }
	uint16	GetSCHShift2conn()							{ return m_nSCHShift2conn; }
	void	SetSCHShift2conn(uint16 Value)				{ m_nSCHShift2conn = Value; }
	uint16	GetSCHShift25sec()							{ return m_nSCHShift25sec; }
	void	SetSCHShift25sec(uint16 Value)				{ m_nSCHShift25sec = Value; }

	void	SetMaxUpload(uint16 uMaxUpload) 			{ m_uMaxUpload = uMaxUpload; }
	uint32	SetMaxUploadWithCheck(uint32 dwMaxUpload);
	void	SetMaxDownload(uint16 uMaxDownload)			{ m_uMaxDownload = uMaxDownload; }
	uint32	SetMaxDownloadWithCheck(uint32 dwMaxDownload);
	uint16	GetMaxDownloadValue() const					{ return m_uMaxDownload; }

	bool	GetLancastEnabled()							{ return m_LancastEnabled; }
	void	SetLancastEnabled(bool bEnable)				{ m_LancastEnabled = bEnable; }
	bool	GetLancastUseDefaultIP()					{ return m_UseDefaultIP; }
	uint32	GetLancastIP();
	uint32	GetLancastSubnet()							{ return m_LancastSubnet; }
	uint16	GetLancastPort()							{ return m_LancastPort; }

	WINDOWPLACEMENT GetEmuleWindowPlacement() const		{ return m_WindowPlacement; }
	void SetWindowLayout(WINDOWPLACEMENT in) 			{ m_WindowPlacement = in; }
	static int GetRecommendedMaxConnections();

	bool IsWSIntruderDetectionEnabled()					{ return m_bWebIntruderDetection; }
	void SetWSIntruderDetectionEnabled(bool bEn)		{ m_bWebIntruderDetection = bEn; }
	uint32 GetWSTempDisableLogin()						{ return m_dwWebTempDisableLogin; }
	void SetWSTempDisableLogin(uint32 dwVal)			{ m_dwWebTempDisableLogin = dwVal; }
	byte GetWSLoginAttemptsAllowed()					{ return m_nWebLoginAttemptsAllowed; }
	void SetWSLoginAttemptsAllowed(byte n)				{ m_nWebLoginAttemptsAllowed = n; }

	void	LoadServerlistAddresses();

	bool	IsCounterMeasures() const					{ return m_bCounterMeasures; }
	void	SetCounterMeasures(bool bEnable)			{ m_bCounterMeasures = bEnable; }
	bool	IsCMNotLog() const							{ return m_bCMNotLog; }
	void	SetCMNotLog(bool bEnable)					{ m_bCMNotLog = bEnable; }

	const ProxySettings& GetProxySettings() const		{ return m_proxy; }
	void SetProxySettings(const ProxySettings &proxysettings)	{ m_proxy = proxysettings; }

	CStringList m_addressesList;

	void	SetLanguage();
	int		GetIPFilterLevel()							{ return static_cast<int>(m_byteIPFilterLevel); }
	void	SetIPFilterLevel(int iIPFilterLevel)		{ m_byteIPFilterLevel = static_cast<byte>(iIPFilterLevel); }
	CString GetMessageFilter()							{ return m_strMessageFilter; }
	void	SetMessageFilter(const CString& in)			{ m_strMessageFilter = in; }
	bool	ShowRatesOnTitle() const					{ return m_bShowRatesInTitle; }
	void	SetRatesOnTitle(bool bShow)					{ m_bShowRatesInTitle = bShow; }

//	Messaging preferences
	CString   GetAwayStateMessage()						{ return m_sAwayStateMessage.Get(); }
	void	SetAwayStateMessage(const CString& in)		{ m_sAwayStateMessage.Put(in); }
	byte	GetAcceptMessagesFrom()						{ return m_nAcceptMessagesFrom; }
	void	SetAcceptMessagesFrom(byte in)				{ m_nAcceptMessagesFrom = in; }
	bool	GetAwayState() const						{ return m_bAwayState; }
	void	SetAwayState(bool bVal)						{ m_bAwayState = bVal; }

//	MobileMule
	byte*	GetMMPass(byte *pbyteHash)					{ return reinterpret_cast<byte*>(m_sMMPassword.Get(reinterpret_cast<HashType*>(pbyteHash))); }
	void	SetMMPass(const CString& strNewPass)		{ HashType Digest; m_sMMPassword.Put(*reinterpret_cast<const HashType*>(MD5Sum(strNewPass, Digest.hash))); }
	bool	IsMMServerEnabled()							{ return m_bMMEnabled; }
	void	SetMMIsEnabled(bool bEnable)				{ m_bMMEnabled = bEnable; }
	uint16	GetMMPort()									{ return m_uMMPort; }
	void	SetMMPort(uint16 uPort)						{ m_uMMPort = uPort; }

	void	SetUseProxy(bool in)						{ m_proxy.m_bUseProxy = in;}

	CString	GetFilenameCleanups()						{ return m_sFilenameCleanups.Get(); }
	void	SetFilenameCleanups(const CString& in)		{ m_sFilenameCleanups.Put(in); }
	bool	GetAutoFilenameCleanup()					{ return m_bAutoFilenameCleanup; }
	void	SetAutoFilenameCleanup(bool in)				{ m_bAutoFilenameCleanup = in; }
	bool	GetFilenameCleanupTags()					{ return m_bFilenameCleanupTags; }
	void	SetFilenameCleanupTags(bool in)				{ m_bFilenameCleanupTags = in; }

	CString GetCommentFilter()							{ return m_sCommentFilter.Get(); }
	void	SetCommentFilter(const CString& in)			{ m_sCommentFilter.Put(in); }

	CString	GetTemplate() const							{ return m_strTemplateFile; }
	void	SetTemplate(CString &strVal)				{ m_strTemplateFile = strVal; }

	bool	GetShowAverageDataRate()					{ return m_bShowAverageDataRate; }
	void	SetShowAverageDataRate(bool show) 			{ m_bShowAverageDataRate = show; }

	bool	LimitlessDownload()							{ return m_bLimitlessDownload; }
	void	SetLimitlessDownload(bool bEnable)			{ m_bLimitlessDownload = bEnable; }

	bool	ShowFileTypeIcon()							{ return m_bShowFileTypeIcon; }
	void	SetShowFileTypeIcon(bool bEnable)			{ m_bShowFileTypeIcon = bEnable; }

	CString GetSMTPServer()								{ return m_sSMTPServer.Get(); }
	void	SetSMTPServer(const CString& in)			{ m_sSMTPServer.Put(in); }
	CString GetSMTPName()								{ return m_sSMTPName.Get(); }
	void	SetSMTPName(const CString& in)				{ m_sSMTPName.Put(in); }
	CString GetSMTPFrom()								{ return m_sSMTPFrom.Get(); }
	void	SetSMTPFrom(const CString& in)				{ m_sSMTPFrom.Put(in); }
	CString GetSMTPTo()									{ return m_sSMTPTo.Get(); }
	void	SetSMTPTo(const CString& in)				{ m_sSMTPTo.Put(in); }
	CString GetSMTPUserName()							{ return m_sSMTPUserName.Get(); }
	void	SetSMTPUserName(const CString& in)			{ m_sSMTPUserName.Put(in); }
	CString	GetSMTPPassword()							{ return m_sSMTPPassword.Get(); }
	void	SetSMTPPassword(const CString& in)			{ m_sSMTPPassword.Put(in); }
	bool	IsSMTPAuthenticated()						{ return m_bSMTPAuthenticated; }
	void	SetSMTPAuthenticated(bool bEnable)			{ m_bSMTPAuthenticated = bEnable; }
	bool	IsSMTPInfoEnabled()							{ return m_bSMTPInfo; }
	void	SetSMTPInfoEnabled(bool bEnable)			{ m_bSMTPInfo = bEnable; }
	bool	IsSMTPWarningEnabled()						{ return m_bSMTPWarning; }
	void	SetSMTPWarningEnabled(bool bEnable)			{ m_bSMTPWarning = bEnable; }
	bool	IsSMTPMsgInSubjEnabled()					{ return m_bSMTPMsgInSubj; }
	void	SetSMTPMsgInSubjEnabled(bool bEnable)		{ m_bSMTPMsgInSubj = bEnable; }

	bool	IsTransferredOnCompleted()					{ return m_bTransferredOnCompleted; }
	void	SetTransferredOnCompleted(bool bEnable)		{ m_bTransferredOnCompleted = bEnable; }

	bool	GetUseDwlPercentage()						{ return m_bUseDwlPercentage; }
	void	SetUseDwlPercentage(bool bEnable)			{ m_bUseDwlPercentage = bEnable; }

//	Backup feature
	bool	IsAutoBackup()								{ return m_bAutoBackup; }
	void	SetAutoBackup(bool in)						{ m_bAutoBackup =in; }
	bool	GetBackupDatFiles()							{ return m_bDatFiles; }
	void	SetBackupDatFiles(bool in)					{ m_bDatFiles =in; }
	bool	GetBackupMetFiles()							{ return m_bMetFiles; }
	void	SetBackupMetFiles(bool in)					{ m_bMetFiles =in; }
	bool	GetBackupIniFiles()							{ return m_bIniFiles; }
	void	SetBackupIniFiles(bool in)					{ m_bIniFiles =in; }
	bool	GetBackupPartFiles()						{ return m_bPartFiles; }
	void	SetBackupPartFiles(bool in)					{ m_bPartFiles =in; }
	bool	GetBackupPartMetFiles()						{ return m_bPartMetFiles; }
	void	SetBackupPartMetFiles(bool in)				{ m_bPartMetFiles =in; }
	bool	GetBackupPartTxtsrcFiles()					{ return m_bPartTxtsrcFiles; }
	void	SetBackupPartTxtsrcFiles(bool in)			{ m_bPartTxtsrcFiles =in; }
	bool	GetBackupOverwrite()						{ return m_bBackupOverwrite; }
	void	SetBackupOverwrite(bool in) 				{ m_bBackupOverwrite =in; }
	CString	GetBackupDir() const						{ return m_sBackupDir.Get(); }
	void	SetBackupDir(const CString& in)				{ m_sBackupDir.Put(in); }
	void	SetScheduledBackup(bool in)					{ m_bScheduledBackup = in; }
	bool	IsScheduledBackup()							{ return m_bScheduledBackup; }
	void	SetScheduledBackupInterval(uint16 in)		{ m_nScheduledBackupInterval = in; }
	uint16	GetScheduledBackupInterval()				{ return m_nScheduledBackupInterval; }
	CString	GetURLsForICC()								{ return m_sURLsForICC.Get(); }
	bool	ShowFullFileStatusIcons()					{ return m_bShowFullFileStatusIcons; }
	void	SetShowFullFileStatusIcons(bool in)			{ m_bShowFullFileStatusIcons = in; }
	void 	SetURLsForICC(const CString& in)			{ m_sURLsForICC.Put(in); }
	bool	ShowPausedGray()							{ return m_bShowPausedGray; }
	void	SetShowPausedGray(bool in)					{ m_bShowPausedGray = in; }
	bool	ShowRoundSizes()							{ return m_bRoundSizes; }
	void	SetShowRoundSizes(bool in)					{ m_bRoundSizes = in; }
	uint16	GetMaxChatHistoryLines()					{ return 20;}
	bool	IsScanFilterEnabled()						{ return m_bScanFilter; }
	void	SetScanFilterEnabled(bool in)				{ m_bScanFilter = in; }
	bool	IsFakeRxDataFilterEnabled()					{ return m_bFakeRxDataFilter; }
	void	SetFakeRxDataFilterEnabled(bool bVal)		{ m_bFakeRxDataFilter = bVal; }

	bool	GetPreviewSmallBlocks()						{ return m_iPreviewSmallBlocks; }
	void	SetPreviewSmallBlocks(bool in)				{ m_iPreviewSmallBlocks = in; }

	short	GetShortcutCode(uint32 dwIdx) const			{ return (dwIdx < SCUT_COUNT) ? m_anShortcutCode[dwIdx] : /*** should never happen ***/ static_cast<short>(VK_RETURN); }
	void	SetShortcutCode(short nCode, uint32 dwIdx)	{ if (dwIdx < SCUT_COUNT) m_anShortcutCode[dwIdx] = nCode; }

	DWORD	GetServerKeepAliveTimeout()					{ return m_dwServerKeepAliveTimeout; }
	void	SetServerKeepAliveTimeout(DWORD value)		{ m_dwServerKeepAliveTimeout = value; }

	uint16	GetSmartFilterMaxQueueRank()				{ return m_nSmartFilterMaxQR; }
	void	SetSmartFilterMaxQueueRank(uint16 in)		{ m_nSmartFilterMaxQR = in; }
	bool	GetSmartFilterShowOnQueue()					{ return m_bSmartFilterShowOQ; }
	void	SetSmartFilterShowOnQueue(bool in)			{ m_bSmartFilterShowOQ = in; }

	bool	IsWatchClipboard4ED2KLinks() const			{ return m_bWatchClipboard;}
	void	SetWatchClipboard4ED2KLinks(bool in)		{ m_bWatchClipboard = in; }
	byte	GetDetailedPartsFilter() const				{ return m_byteDetailedPartsFilter; }
	void	SetDetailedPartsFilter(byte byteVal)		{ m_byteDetailedPartsFilter = byteVal; }

	void	LoadSharedDirs();
	void	LoadTempDirs();

	bool	SharedDirListCheckAndAdd(const CString &strNewDir, bool bAdd);
	void	SharedDirListRefill(CStringList *pNewList);
	void	SharedDirListCopy(CStringList *pOutputList);
	bool	SharedDirListCmp(CStringList *pNewList);
	int		TempDirListCheckAndAdd(const CString &strNewDir, bool bAdd);
	void	TempDirListRefill(CStringList *pNewList);
	void	TempDirListCopy(CStringList *pOutputList);
	bool	TempDirListCmp(CStringList *pNewList);

	CString	GetFilterWords() const						{ return m_strFilterWords; }	// filter words
	CString	GetDownloadLogName() const					{ return m_strDownloadLogFilePath; }

//	Window splitter settings
	uint32	GetSplitterbarPositionFriend() const		{ return static_cast<uint32>(m_uSplitterPosFriend); }
	void	SetSplitterbarPositionFriend(uint32 dwPos)	{ m_uSplitterPosFriend = static_cast<uint16>(dwPos); }

	BOOL	IsNTBased(void);

	void	WritePreparedNameTag(CFile &file) const	{ file.Write(m_pNameTag, m_dwNameTagSz); }

	bool	IsServerAuxPortUsed() const				{ return m_bUseServerAuxPort; }
	void	SetServerAuxPortUsed(bool bUseAuxPort)	{ m_bUseServerAuxPort = bUseAuxPort; }

	CString	GetIPFilterURL() const					{ return m_strIPFilterURL; }
	void	SetIPFilterURL(const CString& strURL)	{ m_strIPFilterURL = strURL; }
	bool	IsIPFilterUpdateOnStart() const			{ return m_bIPFilterUpdateOnStart; }
	void	SetIPFilterUpdateOnStart(bool bIsUpdate){ m_bIPFilterUpdateOnStart = bIsUpdate; }
	int		GetIPFilterUpdateFrequency() const		{ return m_iIPFilterUpdateFrequency; }
	void	SetIPFilterUpdateFrequency(int iDays)	{ m_iIPFilterUpdateFrequency = iDays; }
	uint32	GetLastIPFilterUpdate()					{ return m_dwLastIPFilterUpdate; }
	void	SetLastIPFilterUpdate(uint32 dwDate)	{ m_dwLastIPFilterUpdate = dwDate; }

	bool	IsAVEnabled() const						{ return m_bAVEnabled; }
	void	SetAVEnabled(bool in)					{ m_bAVEnabled = in; }
	bool	IsAVScanCompleted() const				{ return m_bAVScanCompleted; }
	void	SetAVScanCompleted(bool in)				{ m_bAVScanCompleted = in; }
	CString	GetAVPath() const						{ return m_strAVPath; }
	void	SetAVPath(const CString &str)			{ m_strAVPath = str; }
	CString	GetAVParams() const						{ return m_strAVParams; }
	void	SetAVParams(const CString &str)			{ m_strAVParams = str; }

//	Encryption
	bool	IsClientCryptLayerSupported() const		{ return m_bCryptLayerSupported; }
	bool	IsClientCryptLayerRequested() const		{ return IsClientCryptLayerSupported() && m_bCryptLayerRequested; }
	bool	IsClientCryptLayerRequired() const		{ return IsClientCryptLayerRequested() && m_bCryptLayerRequired; }
	bool	IsServerCryptLayerUDPEnabled() const	{ return IsClientCryptLayerSupported(); }
	bool	IsServerCryptLayerTCPRequested() const	{ return IsClientCryptLayerRequested(); }
	bool	SetClientCryptLayerSupported(bool bVal)	{ return m_bCryptLayerSupported = bVal; }
	bool	GetClientCryptLayerRequested() const	{ return m_bCryptLayerRequested; }
	bool	SetClientCryptLayerRequested(bool bVal)	{ return m_bCryptLayerRequested = bVal; }
	bool	GetClientCryptLayerRequired() const		{ return m_bCryptLayerRequired; }
	bool	SetClientCryptLayerRequired(bool bVal)	{ return m_bCryptLayerRequired = bVal; }

private:
	void	CreateUserHash();
	void	SetStandardValues();

	void	LoadIniPreferences();
	void	LoadDatPreferences();
	void	SaveIniPreferences();

	void	SetUserNickTag();
	bool	LoadFilterFile();

private:
	CString					m_strAppDir;
	CString					m_strConfigDir;
	Preferences_Ext_Struct	m_prefsExt;

	CString 				m_strDownloadLogFilePath;

	uchar					m_userHash[16];
	WORD					m_wWinVer;

	CSecuredVar<CString>	m_sNick;
	WORD					m_nLanguageID;

	uint16					m_uMaxUpload;
	uint16					m_uMaxDownload;
	uint16					m_nMaxConnections;
	uint16					m_nMaxSourcePerFile;

	CSecuredVar<uint64>		m_nTotalDownloadedBytes;
	CSecuredVar<uint64>		m_nTotalUploadedBytes;

	uint32					m_dwMaxGraphDownloadRate;
	uint32					m_dwMaxGraphUploadRate;
	byte					m_nGraphRatio;

	bool					m_bReconnect;
	byte					m_nDeadServer;
	bool					m_bUseServerPriorities;
	bool					m_bAutoServerList;
	bool					m_bAutoConnect;
	bool					m_bAutoConnectStaticOnly;
	bool					m_bAddServersFromServer;
	bool					m_bAddServersFromClients;
	bool					m_bFilterBadIP;
	bool					m_bFilterServersByIP;
	enSeeShare				m_nSeeShares;
	byte					m_byteFilePermission;	// Default shared file permission

	bool					m_bUpdateNotify;
	bool					m_bMinToTray;
	bool					m_bConfirmExit;
	bool					m_bConfirmDisconnect;
	bool					m_bConfirmFriendDel;
	bool					m_bBeepOnError;
	bool					m_bSplashScreen;
	bool					m_bOnlineSig;
	bool					m_bShowOverhead;
	bool					m_bBackupPreview;

	byte					m_byteDepth3D;
	uint16					m_nTrafficOMeterInterval;
	uint16					m_nStatsInterval;
	uint16					m_nDetailColumnWidth;

	CSecuredVar<CString>	m_sIncomingDir;
	CSecuredVar<CString>	m_sTempDir;
	CSecuredVar<CString>	m_sVideoPlayer;

	uint16					m_uPort;
	uint16					m_uUDPPort;
	bool					m_bOpenPorts;

	bool					m_bA4AFStringEnabled;
	bool					m_bA4AFCountEnabled;
	byte					m_byteSearchMethod;
	bool					m_bAutoTakeLinks;
	uint16					m_nFileBufferSize;
	int						m_nMainProcessPriority;

	// Scheduler
	bool					m_bSCHEnabled;
	uint32					m_nSCHShift1;
	uint32					m_nSCHShift2;
	uint16					m_uSCHShift1Upload;
	uint16					m_uSCHShift1Download;
	uint16					m_nSCHShift1conn;
	uint16					m_nSCHShift15sec;
	uint16					m_uSCHShift2Upload;
	uint16					m_uSCHShift2Download;
	uint16					m_nSCHShift2conn;
	uint16					m_nSCHShift25sec;
	bool					m_bSCHExceptMon;
	bool					m_bSCHExceptTue;
	bool					m_bSCHExceptWed;
	bool					m_bSCHExceptThu;
	bool					m_bSCHExceptFri;
	bool					m_bSCHExceptSat;
	bool					m_bSCHExceptSun;

	uint16					m_nProfile;
	bool					m_bUAP;
	bool					m_bDAP;
	bool					m_bDisableXS;
	bool					m_bStartDownloadPaused;
	bool					m_bDownloadPausedOnComplete;
	bool					m_bResumeOtherCat;
	bool					m_bAutoClearCompleted;
	bool					m_bAutoSourcesEnabled;
	uint32					m_nMinAutoSourcesPerFile;
	uint32					m_nMaxAutoSourcesPerFile;
	uint32					m_nMaxAutoSourcesTotal;
	uint32					m_nMaxAutoExchangeSources;
	bool					m_bAutoSourcesLogEnabled;
	bool					m_bClientTransferLogEnabled;

	uint32					m_nBadClientBanTime;
	uint32					m_nMinRequestTime;
	byte					m_nBadClientBan;
	bool					m_bBanMessageEnabled;
	bool					m_bBanEnabled;

	CSecuredVar<CString>	m_sToolbarSettings;

	bool					m_bUseProxyListenPort;
	uint16					m_nListenPort;
	bool					m_bCounterMeasures;
	bool					m_bCMNotLog;

	uint32					m_SLSMaxSourcesPerFile;
	uint32					m_nSLSOutdated;
	bool					m_bSLSEnable;

	uint16					m_nQueueSize;
	uint16					m_nDeadServerRetries;
	bool					m_bSmartIdCheck;
	byte					m_nSmartIdState;
	bool					m_bVerbose;
	uint16					m_nMaxConPerFive;

	bool					m_bMultiple;
	byte					m_nTooltipDelayTime;
	bool					m_bBringToForeground;
	uint16					m_nStatsMax;
	byte					m_nStatsAverageMinutes;
	bool					m_bShowToolbarSpeedMeter;
	bool					m_bShowCountryFlag;
	bool					m_bCloseToTray;
	bool					m_bDetailsOnClick;
	bool					m_bStartMinimized;
	bool					m_bCommunityEnabled;
	CSecuredVar<CString>	m_sCommunityString;
	bool					m_bCommunityNoBan;
	uint16					m_nPriorityHigh;
	uint16					m_nPriorityLow;
	bool					m_bRestartWaiting;
	bool					m_bLogToFile;
	bool					m_bLogUploadToFile;
	bool					m_bLogDownloadToFile;
	bool					m_bDisableXSUpTo;
	uint16					m_nXSUpTo;
	uint32					m_nSrvConTimeout;
	uint16					m_nSlowCompleteBlockSize;
	enHashPriority			m_HashingPriority;
	bool					m_bUploadParts;

	bool					m_bUseDownloadNotifier;
	bool					m_bUseDownloadAddNotifier;
	bool					m_bUseChatNotifier;
	bool					m_bUseLogNotifier;
	bool					m_bUseSoundInNotifier;
	bool					m_bUseSchedulerNotifier;
	bool					m_bNotifierImportantError;
	bool					m_bNotifierWebServerError;
	bool					m_bNotifierServerError;
	CSecuredVar<CString>	m_sNotifierSoundFilePath;
	bool					m_bNotifierPopsEveryChatMsg;
	uint32					m_nNotificationDisplayTime;
	uint32					m_nNotificationFontSize;

	CSecuredVar<CString>	m_sUsedFont;
	byte					m_nFontSize;

	// Sort
	COLORREF				m_dwStatColors[12];
	bool					m_bUseSort;
	bool					m_bUseSrcSortCol2;
	bool					m_bPausedStoppedLast;
	byte					m_nServerSortCol;
	byte					m_nUploadSortCol;
	byte					m_nQueueSortCol;
	byte					m_nSearchSortCol;
	byte					m_nIRCSortCol;
	byte					m_nClientListSortCol;
	uint16					m_nFileSortCol;
	byte					m_nDownloadSortCol;
	byte					m_nSrcSortCol1;
	byte					m_nSrcSortCol2;

	// IRC
	CSecuredVar<CString>	m_sIRCNick;
	CSecuredVar<CString>	m_sIRCServer;
	bool					m_bIRCAddTimestamp;
	CSecuredVar<CString>	m_sIRCChanNameFilter;
	bool					m_bIRCUseChanFilter;
	uint16					m_nIRCChannelUserFilter;
	CSecuredVar<CString>	m_sIRCPerformString;
	bool					m_bIRCUsePerform;
	bool					m_bIRCListOnConnect;
	bool					m_bIRCIgnoreInfoMessage;
	bool					m_bIRCIgnoreEmuleProtoInfoMessage;

//	WebServer
	CSecuredVar<HashType>	m_sWebPassword;
	CSecuredVar<HashType>	m_sLowWebPassword;
	WSPrefParams			m_WSPrefs;
	uint32 					m_dwWebTempDisableLogin;
	uint16					m_nWebPort;
	bool					m_bWebEnabled;
	bool					m_bWebLowEnabled;
	int						m_nWebPageRefresh;
	bool					m_bWebIntruderDetection;
	byte					m_nWebLoginAttemptsAllowed;

	// Part Traffic
	bool					m_bUsePartTraffic;
	byte					m_nUpbarPartTraffic;		// 0=one info 1=2 infos 2=3infos
	byte					m_nUpbarColorPartTraffic;	// 0=rainbow style 1=more rainbow 2=downloadlist-like 3=girlie

	// LANCAST (moosetea) - Settings
	bool  					m_LancastEnabled;
	bool   					m_UseDefaultIP;
	uint32 					m_DefaultLancastIP;
	uint32					m_LancastIP;
	uint32					m_LancastSubnet;
	uint16					m_LancastPort;

	CSecuredVar<CString>	m_sToolbarBitmap;
	CSecuredVar<CString>	m_sToolbarBitmapFolder;
	byte					m_nToolbarLabels;

	//Messaging preferences-
	byte					m_nAcceptMessagesFrom;
	bool					m_bAwayState;
	CSecuredVar<CString>	m_sAwayStateMessage;

	bool					m_bIsNTBased;

	//SyruS: cleanup
	CSecuredVar<CString>	m_sFilenameCleanups;
	bool					m_bAutoFilenameCleanup;
	bool					m_bFilenameCleanupTags;

	CSecuredVar<CString>	m_sCommentFilter;

	bool					m_bShowRatingIcons;
	bool					m_bManuallyAddedServerHighPrio;
	static bool				bWinVerAlreadyDetected;
	bool					m_bShowAverageDataRate;
	bool					m_bLimitlessDownload;
	bool					m_bShowFileTypeIcon;

	CSecuredVar<CString>	m_sSMTPServer;
	CSecuredVar<CString>	m_sSMTPName;
	CSecuredVar<CString>	m_sSMTPFrom;
	CSecuredVar<CString>	m_sSMTPTo;
	CSecuredVar<CString>	m_sSMTPUserName;
	CSecuredVar<CString>	m_sSMTPPassword;
	bool					m_bSMTPAuthenticated;
	bool					m_bSMTPInfo;
	bool					m_bSMTPWarning;
	bool					m_bSMTPMsgInSubj;
	bool					m_bTransferredOnCompleted;
	bool					m_bUseDwlPercentage;

//	Backup feature
	bool					m_bDatFiles;
	bool					m_bMetFiles;
	bool					m_bIniFiles;
	bool					m_bPartFiles;
	bool					m_bPartMetFiles;
	bool					m_bPartTxtsrcFiles;
	bool					m_bAutoBackup;
	bool					m_bBackupOverwrite;
	CSecuredVar<CString>	m_sBackupDir;
	bool					m_bScheduledBackup;
	uint16					m_nScheduledBackupInterval;
	bool					m_bShowFullFileStatusIcons;
	bool					m_bShowPausedGray;
	bool					m_bRoundSizes;

	//FakeCheck
	uint32					m_nFakesDatVersion;
	bool					m_bUpdateFakeStartup;
	CString					m_sFakeListURL;
	uint32					m_dwDLingFakeListVersion;
	CString					m_strDLingFakeListLink;
	COLORREF				m_clrFakeListDownloadColor;

	//AutoCheck for new version
	bool					m_bAutoCheckForNewVersion;
	int						m_iAutoCheckLastTime;

	uint16					m_nSmartFilterMaxQR;
	bool					m_bSmartFilterShowOQ;

	bool					m_bWatchClipboard;

	//stats
	// -khaos--+++> Struct Members for Storing Statistics

	// Saved stats for cumulative downline overhead...
	uint64	cumDownOverheadTotal;
	uint64	cumDownOverheadFileReq;
	uint64	cumDownOverheadSrcEx;
	uint64	cumDownOverheadServer;
	uint64	cumDownOverheadOther;
	uint64	cumDownOverheadTotalPackets;
	uint64	cumDownOverheadFileReqPackets;
	uint64	cumDownOverheadSrcExPackets;
	uint64	cumDownOverheadServerPackets;
	uint64	cumDownOverheadOtherPackets;

	// Saved stats for cumulative upline overhead...
	uint64	cumUpOverheadTotal;
	uint64	cumUpOverheadFileReq;
	uint64	cumUpOverheadSrcEx;
	uint64	cumUpOverheadServer;
	uint64	cumUpOverheadOther;
	uint64	cumUpOverheadTotalPackets;
	uint64	cumUpOverheadFileReqPackets;
	uint64	cumUpOverheadSrcExPackets;
	uint64	cumUpOverheadServerPackets;
	uint64	cumUpOverheadOtherPackets;

	// Saved stats for cumulative upline data...
	uint32	cumUpSuccessfulSessions;
	uint32	cumUpFailedSessions;
	uint32	cumUpAvgTime;

	// Client breakdown stats for sent bytes...
	uint64	cumUpDataClients[SO_LAST];
	uint64	sesUpDataClients[SO_LAST];

	// Session data stats by priority
	uint64	sesUpDataPriority[5];

	// Cumulative source breakdown stats for sent bytes...
	uint64	cumUpData_File;
	uint64	cumUpData_PartFile;

	// Session source breakdown stats for sent bytes...
	uint64	sesUpData_File;
	uint64	sesUpData_PartFile;

	// Cumulative community breakdown stats for sent bytes...
	uint64	cumUpData_Community;
	uint64	cumUpData_NoCommunity;

	// Session community breakdown stats for sent bytes...
	uint64	sesUpData_Community;
	uint64	sesUpData_NoCommunity;

	// Saved stats for cumulative downline data...
	uint32	cumDownCompletedFiles;
	uint16	cumDownSuccessfulSessions;
	uint16	cumDownFailedSessions;
	uint32	cumDownAvgTime;

	// Cumulative statistics for saved due to compression/lost due to corruption
	uint64	m_qwCumLostFromCorruption;
	uint64	m_qwCumSavedFromCompression;
	uint32	m_dwCumPartsSavedByICH;
	uint32	m_dwSesPartsSavedByICH;

	// Session statistics for download sessions
	uint64	m_qwSesLostFromCorruption;
	uint64	m_qwSesSavedFromCompression;
	uint16	sesDownSuccessfulSessions;
	uint16	sesDownFailedSessions;
	uint16	sesDownFailedSessionsNoRequiredData;
	uint32	sesDownAvgTime;
	uint16	sesDownCompletedFiles;

	// Client breakdown stats for received bytes...
	uint64	cumDownDataClients[SO_LAST];
	uint64	sesDownDataClients[SO_LAST];

	// Saved stats for cumulative connection data...
	double	cumConnAvgDownRate;
	double	cumConnMaxAvgDownRate;
	double	cumConnMaxDownRate;
	double	cumConnAvgUpRate;
	double	cumConnMaxAvgUpRate;
	double	cumConnMaxUpRate;
	uint32	dwCumConnRunTime;
	uint16	cumConnNumReconnects;
	uint16	cumConnAvgConnections;
	uint16	cumConnMaxConnLimitReached;
	uint16	cumConnPeakConnections;
	uint32	cumConnTransferTime;
	uint32	cumConnDownloadTime;
	uint32	cumConnUploadTime;
	uint32	cumConnServerDuration;

	// Saved records for servers / network...
	uint16	cumSrvrsMostWorkingServers;
	uint32	cumSrvrsMostUsersOnline;
	uint32	cumSrvrsMostFilesAvail;

	// Saved records for shared files...
	uint16	cumSharedMostFilesShared;
	uint64	m_qwCumSharedLargestShareSize;
	uint64	m_qwCumSharedLargestAvgFileSize;
	uint64	m_qwCumSharedLargestFileSize;

	// Save the date when the statistics were last reset...
	time_t	stat_datetimeLastReset;

	// Save new preferences for PPgStats
	byte	statsConnectionsGraphRatio; // This will store the divisor, i.e. for 1:3 it will be 3, for 1:20 it will be 20.
	// Save the expanded branches of the stats tree (don't convert in Unicode!!!)
	CString m_strStatsExpandedTreeItems;

	// <-----khaos- End Statistics Members
	bool	m_bScanFilter;
	bool	m_bFakeRxDataFilter;

	bool	m_iPreviewSmallBlocks;
	DWORD	m_dwServerKeepAliveTimeout;

	CSecuredVar<CString>	m_sURLsForICC;

	short m_anShortcutCode[SCUT_COUNT];

	bool	m_bUseServerAuxPort;

//	Protect list operations
	CCriticalSection	m_csDirList;

//	Protected lists
	CStringList				m_sharedDirList;	// all entries has trailing '\'
	CStringList				m_tempDirList;		// all entries has trailing '\'

	byte	m_byteIPFilterLevel;
	CString m_strIPFilterURL;
	bool	m_bIPFilterUpdateOnStart;
	uint32	m_dwLastIPFilterUpdate;
	int		m_iIPFilterUpdateFrequency;

	bool	m_bAVEnabled;
	bool	m_bAVScanCompleted;
	CString	m_strAVPath;
	CString	m_strAVParams;

//	MobileMule
	CSecuredVar<HashType>	m_sMMPassword;
	bool	m_bMMEnabled;
	uint16	m_uMMPort;

	uchar	m_A4AF_FileHash[16];
	CString	m_strTxtEditor;
	CString	m_strMessageFilter;
	CString	m_strTemplateFile;

	WINDOWPLACEMENT m_WindowPlacement;
	ProxySettings m_proxy;

	uint16	m_auDownloadColumnWidths[DLCOL_NUMCOLUMNS];
	uint16	m_auUploadColumnWidths[ULCOL_NUMCOLUMNS];
	uint16	m_auQueueColumnWidths[QLCOL_NUMCOLUMNS];
	uint16	m_auSearchColumnWidths[SL_NUMCOLUMNS];
	uint16	m_auSharedColumnWidths[SFL_NUMCOLUMNS];
	uint16	m_auServerColumnWidths[SL_COLUMN_NUMCOLUMNS];
	uint16	m_auIrcColumnWidths[IRC2COL_NUMCOLUMNS];
	uint16	m_auClientListColumnWidths[CLCOL_NUMCOLUMNS];
	uint16	m_auPartStatusColumnWidths[FDPARTSCOL_NUMCOLUMNS];
	uint16	m_auFriendColumnWidths[FRIENDCOL_NUMCOLUMNS];

	int		m_aiDownloadColumnOrder[DLCOL_NUMCOLUMNS];
	int		m_aiUploadColumnOrder[ULCOL_NUMCOLUMNS];
	int		m_aiQueueColumnOrder[QLCOL_NUMCOLUMNS];
	int		m_aiSearchColumnOrder[SL_NUMCOLUMNS];
	int		m_aiSharedColumnOrder[SFL_NUMCOLUMNS];
	int		m_aiServerColumnOrder[SL_COLUMN_NUMCOLUMNS];
	int		m_aiIrcColumnOrder[IRC2COL_NUMCOLUMNS];
	int		m_aiClientListColumnOrder[CLCOL_NUMCOLUMNS];
	int		m_aiPartStatusColumnOrder[FDPARTSCOL_NUMCOLUMNS];
	int		m_aiFriendColumnOrder[FRIENDCOL_NUMCOLUMNS];

	int		m_iTableSortItemDownload;
	int		m_iTableSortItemDownload2;
	int		m_iTableSortItemUpload;
	int		m_iTableSortItemQueue;
	int		m_iTableSortItemSearch;
	int		m_iTableSortItemShared;
	int		m_iTableSortItemServer;
	int		m_iTableSortItemIRCChannel;
	int		m_iTableSortItemClientList;
	int		m_iTableSortItemPartStatus;
	int		m_iTableSortItemFriend;

	bool	m_abDownloadColumnHidden[DLCOL_NUMCOLUMNS];
	bool	m_abUploadColumnHidden[ULCOL_NUMCOLUMNS];
	bool	m_abQueueColumnHidden[QLCOL_NUMCOLUMNS];
	bool	m_abSearchColumnHidden[SL_NUMCOLUMNS];
	bool	m_abSharedColumnHidden[SFL_NUMCOLUMNS];
	bool	m_abServerColumnHidden[SL_COLUMN_NUMCOLUMNS];
	bool	m_abIrcColumnHidden[IRC2COL_NUMCOLUMNS];
	bool	m_abClientListColumnHidden[CLCOL_NUMCOLUMNS];
	bool	m_abPartStatusColumnHidden[FDPARTSCOL_NUMCOLUMNS];
	bool	m_abFriendColumnHidden[FRIENDCOL_NUMCOLUMNS];

	bool	m_bTableSortAscendingDownload;
	bool	m_bTableSortAscendingDownload2;
	bool	m_bTableSortAscendingUpload;
	bool	m_bTableSortAscendingQueue;
	bool	m_bTableSortAscendingSearch;
	bool	m_bTableSortAscendingShared;
	bool	m_bTableSortAscendingServer;
	bool	m_bTableSortAscendingIRCChannel;
	bool	m_bTableSortAscendingClientList;
	bool	m_bTableSortAscendingPartStatus;
	bool	m_bTableSortAscendingFriend;

	bool	m_bBugReport;
	bool	m_bAllowLocalHostIP;	// Allow local sources 127.*.*.*
	bool	m_bShowRatesInTitle;
	byte	m_byteDetailedPartsFilter;
	bool	m_bKeepSearchHistory;	// Name Field History

	void	*m_pNameTag;
	uint32	m_dwNameTagSz;

	CString	m_strFilterWords;	// search filter exclude word list

//	Window splitter settings
	uint16	m_uSplitterPosFriend;

//	Encryption
	bool	m_bCryptLayerRequested;
	bool	m_bCryptLayerSupported;
	bool	m_bCryptLayerRequired;
};
