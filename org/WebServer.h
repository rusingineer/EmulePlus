//	This file is part of eMule Plus
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either
//	version 2 of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once

#include "Loggable.h"

#define WEB_GRAPH_HEIGHT		120
#define WEB_GRAPH_WIDTH			500

#define SESSION_TIMEOUT_SECS	300	// 5 minutes session expiration
#define SHORT_LENGTH_MAX		60	// Max size for strings maximum
#define SHORT_LENGTH			40	// Max size for strings
#define SHORT_LENGTH_MIN		30	// Max size for strings minimum

class CWebSocket;

typedef struct
{
	uint32	dwDownRate;
	uint32	dwUpRate;
	uint32	dwConnections;
} UpDown;

typedef struct
{
	long	lSession;
	CTime	startTime;
	bool	admin;
} Session;

typedef struct
{
	CString	sFileName;
	LPCTSTR	pcFileType;
	CString	sFileNameJS;
	LPCTSTR	pcFileState;
	CString	sCategory;
	uint64	m_qwFileSize;
	uint64	m_qwFileTransferred;
	uint32	dwFileSpeed;
	long	lSourceCount;
	long	lNotCurrentSourceCount;
	long	lTransferringSourceCount;
	double	m_dblCompleted;
	int		nFileStatus;
	int		nFilePrio;
	CString	sFileHash;
	CString	sED2kLink;
	CString	sFileInfo;
	CString	sFakeCheck;
	bool	bFileAutoPrio;
	bool	bIsComplete;
	bool	bIsPreview;
	bool	bIsGetFLC;
	char	cSortRank;
} DownloadFiles;

typedef struct
{
	uint64	m_qwFileSize;
	uint64	nFileTransferred;
	uint64	nFileAllTimeTransferred;
	double	dblFileCompletes;
	LPCTSTR	pcFileState;
	CString	sFileName;
	LPCTSTR	pcFileType;
	CString	sFileCompletes;
	CString	sFilePriority;
	CString	sFileHash;
	CString	sED2kLink;
	uint32	nFileAllTimeRequests;
	uint32	nFileAllTimeAccepts;
	uint16	nFileRequests;
	uint16	nFileAccepts;
	byte	nFilePriority;
	bool	bIsPartFile;
	bool	bFileAutoPriority;
} SharedFiles;

typedef struct
{
	LPCTSTR	pcClientState;
	CString	sUserHash;
	LPCTSTR	pcActive;
	CString	sFileInfo;
	CString	sClientSoft;
	LPCTSTR	pcClientExtra;
	CString	sUserName;
	CString	sFileName;
	CString	sClientNameVersion;
	uint32	nTransferredDown;
	uint32	nTransferredUp;
	uint32	dwDataRate;
} UploadUsers;

typedef struct
{
	LPCTSTR	pcClientState;
	LPCTSTR	pcClientStateSpecial;
	CString	sUserHash;
	CString	sClientSoft;
	CString	sUserName;
	CString	sFileName;
	CString	sClientNameVersion;
	CString	sIndex;
	uint32	dwScore;
	int		iClientExtra;	//WS_WQUETYPE_xxx
} QueueUsers;

#define WS_WQUETYPE_NONE		0
#define WS_WQUETYPE_BANNED		1
#define WS_WQUETYPE_FRIEND		2
#define WS_WQUETYPE_CREDIT		3

typedef enum
{
	DOWN_SORT_STATE,
	DOWN_SORT_TYPE,
	DOWN_SORT_NAME,
	DOWN_SORT_SIZE,
	DOWN_SORT_TRANSFERRED,
	DOWN_SORT_SPEED,
	DOWN_SORT_PROGRESS,
	DOWN_SORT_SOURCES,
	DOWN_SORT_PRIORITY,
	DOWN_SORT_CATEGORY,
	DOWN_SORT_FAKECHECK
} DownloadSort;

typedef enum
{
	UP_SORT_CLIENT,
	UP_SORT_USER,
	UP_SORT_VERSION,
	UP_SORT_FILENAME,
	UP_SORT_TRANSFERRED,
	UP_SORT_SPEED
} UploadSort;

typedef enum
{
	QU_SORT_CLIENT,
	QU_SORT_USER,
	QU_SORT_VERSION,
	QU_SORT_FILENAME,
	QU_SORT_SCORE
} QueueSort;

typedef enum
{
	SHARED_SORT_STATE,
	SHARED_SORT_TYPE,
	SHARED_SORT_NAME,
	SHARED_SORT_SIZE,
	SHARED_SORT_TRANSFERRED,
	SHARED_SORT_ALL_TIME_TRANSFERRED,
	SHARED_SORT_REQUESTS,
	SHARED_SORT_ALL_TIME_REQUESTS,
	SHARED_SORT_ACCEPTS,
	SHARED_SORT_ALL_TIME_ACCEPTS,
	SHARED_SORT_COMPLETES,
	SHARED_SORT_PRIORITY
} SharedSort;

typedef struct
{
	CString	sServerName;
	CString	sServerDescription;
	CString	sServerIP;
	CString	sServerFullIP;
	CString	sServerState;
	CString	sServerPriority;
	CString	sServerVersion;
	int		nServerPort;
	int		nServerUsers;
	int		nServerMaxUsers;
	int		nServerFiles;
	int		nServerPing;
	int		nServerFailed;
	int		nServerSoftLimit;
	int		nServerHardLimit;
	bool	bServerStatic;
	byte	nServerPriority;
} ServerEntry;

typedef enum
{
	SERVER_SORT_STATE,
	SERVER_SORT_NAME,
	SERVER_SORT_IP,
	SERVER_SORT_DESCRIPTION,
	SERVER_SORT_PING,
	SERVER_SORT_USERS,
	SERVER_SORT_FILES,
	SERVER_SORT_PRIORITY,
	SERVER_SORT_FAILED,
	SERVER_SORT_LIMIT,
	SERVER_SORT_VERSION
} ServerSort;

typedef struct
{
	DownloadSort	DownloadSort;
	bool			bDownloadSortReverse;
	UploadSort		UploadSort;
	bool			bUploadSortReverse;
	QueueSort		QueueSort;
	bool			bQueueSortReverse;
	ServerSort		ServerSort;
	bool			bServerSortReverse;
	SharedSort		SharedSort;
	bool			bSharedSortReverse;
} GlobalParams;

typedef struct
{
	CString			sURL;
	void			*pThis;
	CWebSocket		*pSocket;
	CString			strAcceptEncoding;
	CString			strIfModifiedSince;
} ThreadData;

typedef struct
{
	CString	sHeader;
	CString	sHeaderStylesheet;
	CString	sFooter;
	CString	sServerList;
	CString	sServerLine;
	CString	sTransferImages;
	CString	sTransferList;
	CString	sTransferDownHeader;
	CString	sTransferDownFooter;
	CString	sTransferDownLine;
	CString	sTransferUpHeader;
	CString	sTransferUpFooter;
	CString	sTransferUpLine;
	CString	sTransferUpQueueShow;
	CString	sTransferUpQueueHide;
	CString	sTransferUpQueueLine;
	CString	sTransferUpQueueBannedShow;
	CString	sTransferUpQueueBannedHide;
	CString	sTransferUpQueueBannedLine;
	CString	sTransferUpQueueFriendShow;
	CString	sTransferUpQueueFriendHide;
	CString	sTransferUpQueueFriendLine;
	CString	sTransferUpQueueCreditShow;
	CString	sTransferUpQueueCreditHide;
	CString	sTransferUpQueueCreditLine;
	CString	sSharedList;
	CString	sSharedLine;
	CString	sGraphs;
	CString	sLog;
	CString	sServerInfo;
	CString sDebugLog;
	CString sStats;
	CString sPreferences;
	CString	sLogin;
	CString	sAddServerBox;
	CString	sSearch;
	CString	sProgressbarImgs;
	CString sProgressbarImgsPercent;
	CString sSearchResultLine;
	CString sSearchHeader;
	CString sCatArrow;
	CString sDownArrow;
	CString sUpArrow;
	CString strDownDoubleArrow;
	CString strUpDoubleArrow;
	uint16	iProgressbarWidth;
} WebTemplates;

class CWebServer : public CLoggable
{
public:
	CWebServer(void);
	~CWebServer(void);

	inline void SetIP(ULONG ip) { m_ulCurIP = ip; }
	int	 UpdateSessionCount();
	void StartServer(void);
	void RestartServer();
	void AddStatsLine(UpDown *line);
	void ReloadTemplates();
	bool IsRunning()	{ return m_bServerWorking; }
	CArray<UpDown, UpDown&>* GetPointsForWeb()	{return &PointsForWeb;} // MobileMule
	void		ProcessGeneralReq(const ThreadData &Data);
	void		ProcessFileReq(const ThreadData &Data);

private:
	CString	GetHeader(const ThreadData &Data, long lSession);
	static CString	_GetFooter(const ThreadData &Data);
	CString	GetServerList(const ThreadData &Data);
	CString	GetTransferList(const ThreadData &Data);
	CString	GetSharedFilesList(const ThreadData &Data);
	CString	GetGraphs(const ThreadData &Data);
	CString	GetLog(const ThreadData &Data);
	CString	GetServerInfo(const ThreadData &Data);
	CString	GetDebugLog(const ThreadData &Data);
	static CString	_GetStats(const ThreadData &Data);
	CString	GetPreferences(const ThreadData &Data);
	static CString	_GetLoginScreen(const ThreadData &Data, bool bIsUseGzip);
	static CString	_GetConnectedServer(const ThreadData &Data);
	CString 	GetAddServerBox(const ThreadData &Data);
	static void		_RemoveServer(const CString &strIP, uint16 uPort);
	static void		_AddToStatic(const CString &strIP, uint16 uPort);
	static void		_RemoveFromStatic(const CString &strIP, uint16 uPort);

	static CString	_GetWebSearch(const ThreadData &Data);
	CString	GetSearch(const ThreadData &Data);

	static CString	_ParseURL(const CString &URL, const TCHAR *pcFieldName);
	static CString	_ParseURLArray(CString URL, CString fieldname);
	static void		_ConnectToServer(const CString &strIP, uint16 uPort);
	bool		IsLoggedIn(const ThreadData &Data, long lSession);
	bool		RemoveSession(const ThreadData &Data, long lSession);
	static CString	_SpecialChars(CString str, bool noquote = false);
	static void		SpecialChars(CString *str);
	static CString	_GetPlainResString(UINT nID, bool noquote = false);
	static void		_GetPlainResString(CString *pstrOut, UINT nID, bool noquote = false);
	static int		GzipCompress(byte *pbyteDst, unsigned *puiDstLen, const byte *pbyteSrc, unsigned uiSrcLen, int level);
	static void		_SetSharedFileJumpstart(const CString &strFileHash);
	static void		_SetSharedFileNoJumpstart(const CString &strFileHash);
	static CString	_GetWebCharSet();
	CString			_LoadTemplate(const CString &sAll, const TCHAR *pcTemplateName);
	bool		GetSessionByID(Session *ses, long lSessionID, bool bUpdateTime);
	bool		IsSessionAdmin(const ThreadData &Data, const CString &strSsessionID);
	static CString	_GetDownloadGraph(const ThreadData &Data, const byte *pbyteHash);
	static void		InsertCatBox(CString &Out,int preselect, const CString &strBoxLbl, bool jump,bool extraCats, const CString &strSession, const byte *pbyteHash);
	static CString	GetSubCatLabel(int iCat);
	long		DoAuthentication(const ThreadData &Data, Session *ses);

	bool			GetIsTempDisabled() { return m_bIsTempDisabled; }
	void			SetHiddenColumnState(const CString &strURL, bool *pbCols, unsigned uiArrSz);

private:
	CArray<Session, Session&>	Sessions;
	CArray<UpDown, UpDown&>		PointsForWeb;
	WSPrefParams	*m_pWSPrefs;
	GlobalParams	m_Params;
	WebTemplates	m_Templates;
	uint32			m_nStartTempDisabledTime;
	ULONG			m_ulCurIP;
	int				m_iSearchSortby;
	uint16			m_nIntruderDetect;
	bool			m_bServerWorking;
	bool			m_bSearchAsc;
	bool			m_bIsTempDisabled;
};
