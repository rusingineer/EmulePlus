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

#include "stdafx.h"
#include "emule.h"
#include "zlib/zlib.h"
#include "WebServer.h"
#include "WebSocket.h"
#include "PartFile.h"
#include "ED2KLink.h"
#include "MD5Sum.h"
#include "ini2.h"
#include "QArray.h"
#include "HTRichEditCtrl.h"
#include "server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define HTTPInit "Cache-Control: no-cache, no-store, must-revalidate, max-age=0, private\r\nPragma: no-cache\r\nContent-Type: text/html\r\n"

#define WEB_SERVER_TEMPLATES_VERSION	1010

//SyruS CQArray-Sorting operators
bool operator > (QueueUsers & first, QueueUsers & second)
{
	return (first.sIndex.CompareNoCase(second.sIndex) > 0);
}

bool operator < (QueueUsers & first, QueueUsers & second)
{
	return (first.sIndex.CompareNoCase(second.sIndex) < 0);
}

CWebServer::CWebServer(void)
{
	m_pWSPrefs = g_App.m_pPrefs->GetWSPrefsPtr();

	m_Params.DownloadSort = DOWN_SORT_NAME;
	m_Params.bDownloadSortReverse = true;
	m_Params.UploadSort = UP_SORT_FILENAME;
	m_Params.bUploadSortReverse = true;
	m_Params.QueueSort = QU_SORT_FILENAME;
	m_Params.bQueueSortReverse = true;
	m_Params.ServerSort = SERVER_SORT_NAME;
	m_Params.bServerSortReverse = true;
	m_Params.SharedSort = SHARED_SORT_NAME;
	m_Params.bSharedSortReverse = true;
	m_iSearchSortby = 3;
	m_bSearchAsc = false;

	m_bServerWorking = false;
	m_nIntruderDetect = 0;
	m_nStartTempDisabledTime = 0;
	m_bIsTempDisabled = false;
}

void CWebServer::ReloadTemplates()
{
	EMULE_TRY

	CString	strTmp, sFile = g_App.m_pPrefs->GetTemplate();

	if (sFile.IsEmpty() || (sFile.CompareNoCase(_T("emulexp.tmpl")) == 0))
	{
		sFile = g_App.m_pPrefs->GetAppDir();
		sFile += _T("eMuleXP.tmpl");
	}

	CStdioFile file;
	if (file.Open(sFile, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText | CFile::osSequentialScan))
	{
		CString sAll;

		setvbuf(file.m_pStream, NULL, _IOFBF, 16*1024);
		for(;;)
		{
			if(!file.ReadString(strTmp))
				break;

			sAll += strTmp;
			sAll += _T('\n');
		}
		file.Close();

		long lVersion = _tstol(_LoadTemplate(sAll, _T("TMPL_VERSION")));

		if(lVersion < WEB_SERVER_TEMPLATES_VERSION)
		{
			if(g_App.m_pPrefs->GetWSIsEnabled() || m_bServerWorking)
				AddLogLine(true, RGB_LOG_ERROR + GetResString(IDS_WS_ERR_LOADTEMPLATE), sFile);
			if (m_bServerWorking)
				StopSockets();
			m_bServerWorking = false;
			g_App.m_pPrefs->SetWSIsEnabled(false);
		}
		else
		{
			m_Templates.sHeader = _LoadTemplate(sAll,_T("TMPL_HEADER"));
			m_Templates.sHeaderStylesheet = _LoadTemplate(sAll,_T("TMPL_HEADER_STYLESHEET"));
			m_Templates.sFooter = _LoadTemplate(sAll,_T("TMPL_FOOTER"));
			m_Templates.sServerList = _LoadTemplate(sAll,_T("TMPL_SERVER_LIST"));
			m_Templates.sServerLine = _LoadTemplate(sAll,_T("TMPL_SERVER_LINE"));
			m_Templates.sTransferImages = _LoadTemplate(sAll,_T("TMPL_TRANSFER_IMAGES"));
			m_Templates.sTransferList = _LoadTemplate(sAll,_T("TMPL_TRANSFER_LIST"));
			m_Templates.sTransferDownHeader = _LoadTemplate(sAll,_T("TMPL_TRANSFER_DOWN_HEADER"));
			m_Templates.sTransferDownFooter = _LoadTemplate(sAll,_T("TMPL_TRANSFER_DOWN_FOOTER"));
			m_Templates.sTransferDownLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_DOWN_LINE"));
			m_Templates.sTransferUpHeader = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_HEADER"));
			m_Templates.sTransferUpFooter = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_FOOTER"));
			m_Templates.sTransferUpLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_LINE"));
			m_Templates.sTransferUpQueueShow = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_SHOW"));
			m_Templates.sTransferUpQueueHide = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_HIDE"));
			m_Templates.sTransferUpQueueLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_LINE"));
			m_Templates.sTransferUpQueueBannedShow = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_BANNED_SHOW"));
			m_Templates.sTransferUpQueueBannedHide = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_BANNED_HIDE"));
			m_Templates.sTransferUpQueueBannedLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_BANNED_LINE"));
			m_Templates.sTransferUpQueueFriendShow = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_FRIEND_SHOW"));
			m_Templates.sTransferUpQueueFriendHide = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_FRIEND_HIDE"));
			m_Templates.sTransferUpQueueFriendLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_FRIEND_LINE"));
			m_Templates.sTransferUpQueueCreditShow = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_CREDIT_SHOW"));
			m_Templates.sTransferUpQueueCreditHide = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_CREDIT_HIDE"));
			m_Templates.sTransferUpQueueCreditLine = _LoadTemplate(sAll,_T("TMPL_TRANSFER_UP_QUEUE_CREDIT_LINE"));
			m_Templates.sSharedList = _LoadTemplate(sAll,_T("TMPL_SHARED_LIST"));
			m_Templates.sSharedLine = _LoadTemplate(sAll,_T("TMPL_SHARED_LINE"));
			m_Templates.sGraphs = _LoadTemplate(sAll,_T("TMPL_GRAPHS"));
			m_Templates.sLog = _LoadTemplate(sAll,_T("TMPL_LOG"));
			m_Templates.sServerInfo = _LoadTemplate(sAll,_T("TMPL_SERVERINFO"));
			m_Templates.sDebugLog = _LoadTemplate(sAll,_T("TMPL_DEBUGLOG"));
			m_Templates.sStats = _LoadTemplate(sAll,_T("TMPL_STATS"));
			m_Templates.sPreferences = _LoadTemplate(sAll,_T("TMPL_PREFERENCES"));
			m_Templates.sLogin = _LoadTemplate(sAll,_T("TMPL_LOGIN"));
			m_Templates.sAddServerBox = _LoadTemplate(sAll,_T("TMPL_ADDSERVERBOX"));
			m_Templates.sSearch = _LoadTemplate(sAll,_T("TMPL_SEARCH"));
			m_Templates.iProgressbarWidth = static_cast<uint16>(_tstoi(_LoadTemplate(sAll,_T("PROGRESSBARWIDTH"))));
			m_Templates.sSearchHeader = _LoadTemplate(sAll,_T("TMPL_SEARCH_RESULT_HEADER"));
			m_Templates.sSearchResultLine = _LoadTemplate(sAll,_T("TMPL_SEARCH_RESULT_LINE"));
			m_Templates.sProgressbarImgs = _LoadTemplate(sAll,_T("PROGRESSBARIMGS"));
			m_Templates.sProgressbarImgsPercent = _LoadTemplate(sAll,_T("PROGRESSBARPERCENTIMG"));
			m_Templates.sCatArrow= _LoadTemplate(sAll, _T("TMPL_CATARROW"));
			m_Templates.sDownArrow= _LoadTemplate(sAll, _T("TMPL_DOWNARROW"));
			m_Templates.sUpArrow= _LoadTemplate(sAll, _T("TMPL_UPARROW"));
			m_Templates.strDownDoubleArrow = _LoadTemplate(sAll, _T("TMPL_DNDOUBLEARROW"));
			m_Templates.strUpDoubleArrow = _LoadTemplate(sAll, _T("TMPL_UPDOUBLEARROW"));

			m_Templates.sProgressbarImgsPercent.Replace(_T("[PROGRESSGIFNAME]"),_T("%s"));
			m_Templates.sProgressbarImgsPercent.Replace(_T("[PROGRESSGIFINTERNAL]"),_T("%i"));
			m_Templates.sProgressbarImgs.Replace(_T("[PROGRESSGIFNAME]"),_T("%s"));
			m_Templates.sProgressbarImgs.Replace(_T("[PROGRESSGIFINTERNAL]"),_T("%i"));
		}
	}
	else if(m_bServerWorking)
	{
		AddLogLine(true, RGB_LOG_ERROR + GetResString(IDS_WEB_ERR_CANTLOAD), sFile);
		StopSockets();
		m_bServerWorking = false;
		g_App.m_pPrefs->SetWSIsEnabled(false);
	}

	EMULE_CATCH
}

CWebServer::~CWebServer(void)
{
	if (m_bServerWorking) StopSockets();
}

CString CWebServer::_LoadTemplate(const CString &sAll, const TCHAR *pcTemplateName)
{
	EMULE_TRY

	CString	sRet, strTmp = _T("<--");

	strTmp += pcTemplateName;

	int nStart = sAll.Find(strTmp + _T("-->"));
	int nEnd = sAll.Find(strTmp + _T("_END-->"));
	if ((nStart >= 0) && (nEnd >= 0) && (nStart < nEnd))
	{
		nStart += _tcslen(pcTemplateName) + 7;
		sRet = sAll.Mid(nStart, nEnd - nStart - 1);
	}
	else
		AddDebugLogLine(RGB_LOG_ERROR_TXT _T("Webserver: can't load %s template"), pcTemplateName);

	return sRet;

	EMULE_CATCH

	return _T("");
}

void CWebServer::RestartServer()
{	//Cax2 - restarts the server with the new port settings
	StopSockets();
	if (m_bServerWorking)
		StartSockets(this);
}

void CWebServer::StartServer(void)
{
	EMULE_TRY

	if (m_bServerWorking == g_App.m_pPrefs->GetWSIsEnabled())
		return;
	m_bServerWorking = g_App.m_pPrefs->GetWSIsEnabled();

	if (m_bServerWorking)
	{
		ReloadTemplates();
		if (m_bServerWorking)
		{
			StartSockets(this);
			m_nIntruderDetect = 0;
			m_nStartTempDisabledTime = 0;
			m_bIsTempDisabled = false;
		}
	}
	else
		StopSockets();

	if(g_App.m_pPrefs->GetWSIsEnabled() && m_bServerWorking)
		AddLogLine(false, _T("%s: %s"),_GetPlainResString(IDS_PW_HTTPD), _GetPlainResString(IDS_ENABLED).MakeLower());
	else
		AddLogLine(false, _T("%s: %s"),_GetPlainResString(IDS_PW_HTTPD), _GetPlainResString(IDS_DISABLED).MakeLower());

	EMULE_CATCH
}

void CWebServer::_RemoveServer(const CString &strIP, uint16 uPort)
{
	EMULE_TRY

	CServer	*server = g_App.m_pServerList->GetServerByAddress(strIP, uPort);
	if (server != NULL)
		g_App.m_pMDlg->SendMessage(WEB_REMOVE_SERVER, (WPARAM)server, NULL);

	EMULE_CATCH
}

void CWebServer::_AddToStatic(const CString &strIP, uint16 uPort)
{
	EMULE_TRY

	CServer	*server = g_App.m_pServerList->GetServerByAddress(strIP, uPort);
	if (server != NULL)
		g_App.m_pMDlg->SendMessage(WEB_ADD_TO_STATIC, (WPARAM)server, NULL);

	EMULE_CATCH
}

void CWebServer::_RemoveFromStatic(const CString &strIP, uint16 uPort)
{
	EMULE_TRY

	CServer	*server = g_App.m_pServerList->GetServerByAddress(strIP, uPort);
	if (server != NULL)
		g_App.m_pMDlg->SendMessage(WEB_REMOVE_FROM_STATIC, (WPARAM)server, NULL);

	EMULE_CATCH
}

void CWebServer::_SetSharedFileJumpstart(const CString &strFileHash)
{
	EMULE_TRY

	byte	abyteFileHash[16];

	if (md4cmp0(StringToHash(strFileHash, abyteFileHash)) != 0)
	{
		CKnownFile	*pCurFile;

		if ((pCurFile = g_App.m_pSharedFilesList->GetFileByID(abyteFileHash)) != NULL)
		{
#ifdef OLD_SOCKETS_ENABLED
		//	Don't enable JumpStart for small files
			if (pCurFile->GetFileSize() > PARTSIZE)
				pCurFile->SetJumpstartEnabled(true);
#endif //OLD_SOCKETS_ENABLED

			g_App.m_pSharedFilesList->UpdateItem(pCurFile);
		}
	}

	EMULE_CATCH
}

void CWebServer::_SetSharedFileNoJumpstart(const CString &strFileHash)
{
	EMULE_TRY

	byte	abyteFileHash[16];

	if (md4cmp0(StringToHash(strFileHash, abyteFileHash)) != 0)
	{
		CKnownFile	*pCurFile;

		if ((pCurFile = g_App.m_pSharedFilesList->GetFileByID(abyteFileHash)) != NULL)
		{
#ifdef OLD_SOCKETS_ENABLED
			pCurFile->SetJumpstartEnabled(false);
#endif //OLD_SOCKETS_ENABLED

			g_App.m_pSharedFilesList->UpdateItem(pCurFile);
		}
	}

	EMULE_CATCH
}

void CWebServer::AddStatsLine(UpDown *line)
{
	EMULE_TRY

	if (PointsForWeb.GetCount() >= WEB_GRAPH_WIDTH)
		PointsForWeb.RemoveAt(0);
	PointsForWeb.Add(*line);

	EMULE_CATCH
}

CString CWebServer::_SpecialChars(CString str, bool noquote /*=false*/)
{
	EMULE_TRY

	str.Replace(_T("&"), _T("&amp;"));
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));
	str.Replace(_T("\""), _T("&quot;"));
	if(noquote)
	{
		str.Replace(_T("'"), _T("&#8217;"));
		str.Replace(_T("\n"), _T("\\n"));
	}
	return str;

	EMULE_CATCH

	return _T("");
}

void CWebServer::SpecialChars(CString *str)
{
	str->Replace(_T("&"), _T("&amp;"));
	str->Replace(_T("<"), _T("&lt;"));
	str->Replace(_T(">"), _T("&gt;"));
	str->Replace(_T("\""), _T("&quot;"));
}

void CWebServer::_ConnectToServer(const CString &strIP, uint16 uPort)
{
	EMULE_TRY

	CServer	*server = g_App.m_pServerList->GetServerByAddress(strIP, uPort);

	if (server != NULL)
		g_App.m_pMDlg->SendMessage(WEB_CONNECT_TO_SERVER, (WPARAM)server, NULL);

	EMULE_CATCH
}

void CWebServer::ProcessFileReq(const ThreadData &Data)
{
	EMULE_TRY

	CWebServer *pThis = (CWebServer *)Data.pThis;
	if(pThis == NULL)
		return;

	CString		filename = Data.sURL;
	CString		strExt = filename.Right(4).MakeLower();
	CStringA	strHdr;
	const char	*pcContType = "";
	bool		bGzip = false;

	if (strExt == ".gif")
		pcContType = "image/gif";
	else if (strExt == ".jpg" || filename.Right(5).MakeLower() == ".jpeg")
		pcContType = "image/jpg";
	else if (strExt == ".bmp")
	{
		pcContType = "image/bmp";
		bGzip = true;
	}
	else if (strExt == ".png")
		pcContType = "image/png";
	else if (strExt == ".ico")
	{
		pcContType = "image/x-icon";
		bGzip = true;
	}
	else if (strExt == ".css")
	{
		pcContType = "text/css";
		bGzip = true;
	}
	else if (filename.Right(3).MakeLower() == ".js")
	{
		pcContType = "text/javascript";
		bGzip = true;
	}
	else if (strExt == ".xml")
	{
		pcContType = "text/xml";
		bGzip = true;
	}
	else if (strExt == ".txt")
	{
		pcContType = "text/plain";
		bGzip = true;
	}

	bGzip = bGzip && (Data.strAcceptEncoding.Find(_T("gzip")) >= 0);
#if 0
	if (bGzip)
		AddDebugLogLine(_T("WebServer: File Request (name=%s) Should do gzip (AcceptEncoding=%s)"), filename, Data.strAcceptEncoding);
	else
		AddDebugLogLine(_T("WebServer: File Request (name=%s) Should NOT do gzip (AcceptEncoding=%s)"), filename, Data.strAcceptEncoding);
#endif

//	HTTP/1.x response header "Content-Type:"
	strHdr.Format("Content-Type: %s\r\n", pcContType);

// Relative path to file from the eMule application folder
	filename.Delete(0);
	filename.Replace(_T('/'), _T('\\'));
	filename = g_App.m_pPrefs->GetAppDir() + _T("WebServer\\") + filename;

	CFile file;
	if (file.Open(filename, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary))
	{
		char		acGMT[maxTimeBufferSize];
		FILETIME	ftModify;
		struct tm	stTm = {0};
		CString		strPrevLocale(_tsetlocale(LC_TIME, NULL));

		_tsetlocale(LC_TIME, _T("English"));
	// Get the file's modification time
		if (::GetFileTime(file.m_hFile, NULL, NULL, &ftModify))
		{
			CTime		timeFileMod(ftModify);

			if ( (timeFileMod.GetGmtTm(&stTm) != NULL) &&
				(strftime(acGMT, maxTimeBufferSize, "%a, %d %b %Y %H:%M:%S GMT", &stTm) != 0) )
			{
				if (!Data.strIfModifiedSince.IsEmpty() && (CStringA(Data.strIfModifiedSince) == acGMT))
				{
					_tsetlocale(LC_TIME, strPrevLocale);
					file.Close();
					Data.pSocket->SendReply(304);
					return;
				}
			//	HTTP/1.x response header "Last-Modified:"
				strHdr.AppendFormat("Last-Modified: %s\r\n", acGMT);
			}
		}

	//	DonGato: expire images after 30 days (can be override by browser)
	//	Controls for file caching expiry time
		const int	iExp_M = 1; //Months
		const int	iExp_D = 0; //Days
		const int	iExp_h = 0; //Hours
		const int	iExp_m = 0; //Minutes
		const int	iExp_s = 0; //Seconds
	//	Calculate expiry time in seconds
		int		iExpireDelta = 60 * (60 * (24 * (30 * iExp_M + iExp_D) + iExp_h) + iExp_m) + iExp_s;
		CTime	t = CTime::GetCurrentTime() + iExpireDelta;

	//	HTTP/1.0 response header "Expires:" and
	//	HTTP/1.1 response header "Cache-Control: max-age"
	//	Overrides "Expires:" only if the client understands it
	//	Is more accurate than "Expires:"
		if ( (t.GetGmtTm(&stTm) != NULL) &&
			(strftime(acGMT, maxTimeBufferSize, "%a, %d %b %Y %H:%M:%S GMT", &stTm) != 0) )
		{
		//	HTTP/1.x response header "Last-Modified:"
			strHdr.AppendFormat("Expires: %s\r\nCache-Control: max-age=%u\r\n", acGMT, iExpireDelta);
		}
		_tsetlocale(LC_TIME, strPrevLocale);

		uint32	dwFileSz = static_cast<uint32>(file.GetLength());
		char	*buffer = new char[dwFileSz];
		int		size = file.Read(buffer, dwFileSz);

		file.Close();
		Data.pSocket->SendContent(strHdr, buffer, size);
		delete[] buffer;
	}
	else
		Data.pSocket->SendReply(404);	//	File not found

	EMULE_CATCH
}

void CWebServer::ProcessGeneralReq(const ThreadData &Data)
{
	EMULE_TRY

	SetThreadLocale(g_App.m_pPrefs->GetLanguageID());

	//(0.29b)//////////////////////////////////////////////////////////////////
	// Here we are in real trouble! We are accessing the entire emule main thread
	// data without any syncronization!! Either we use the message pump for m_pMDlg
	// or use some hundreds of critical sections... For now, an exception handler
	// shoul avoid the worse things.
	//////////////////////////////////////////////////////////////////////////
	CoInitialize(NULL);

	EMULE_TRY

	bool	isUseGzip = (Data.strAcceptEncoding.Find(_T("gzip")) >= 0);	// Enable gzip only if the browser supports it
	CString	Out, strCmd;
	TCHAR	*gzipOut = NULL;
	unsigned	uiGzipLen = 0;

	DWORD dwCurTick = ::GetTickCount();

	if (GetIsTempDisabled() &&
		((dwCurTick - m_nStartTempDisabledTime) > (g_App.m_pPrefs->GetWSTempDisableLogin() * 60000)))
	{
		m_bIsTempDisabled = false;
		m_nStartTempDisabledTime = 0;
		AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_INTRBLOCKEND));
	}

	Session	ses;
	long	lSession = DoAuthentication(Data, &ses);

	strCmd = _ParseURL(Data.sURL, _T("w"));
#if 0
	AddDebugLogLine(_T("WebServer: start request processing (Cmd=%s)"), strCmd);
#endif
#if 0
	if (isUseGzip)
		AddDebugLogLine(_T("WebServer: Gzip is ON (AcceptEncoding=%s)"), Data.strAcceptEncoding);
	else
		AddDebugLogLine(_T("WebServer: Gzip is OFF (AcceptEncoding=%s)"), Data.strAcceptEncoding);
#endif
	if (strCmd == _T("logout"))
		RemoveSession(Data, lSession);

	if (IsLoggedIn(Data, lSession))
	{
		if (strCmd == _T("close"))
		{
			g_App.m_app_state = g_App.APP_STATE_SHUTTINGDOWN;
			SendMessage(g_App.m_pMDlg->m_hWnd, WM_CLOSE, 0, 0);
		}
		else if (strCmd == _T("shutdown"))
		{
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp;	// Get a token for this process
			try
			{
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
					throw;
				LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
				tkp.PrivilegeCount = 1;  // one privilege to set
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;	// Get the shutdown privilege for this process.
				AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
				ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
			}
			catch(...)
			{
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_SHUTDOWN) + _T(' ') + GetResString(IDS_FAILED));
			}
		}
		else if (strCmd == _T("reboot"))
		{
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp;	// Get a token for this process.
			try
			{
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
					throw;
				LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
				tkp.PrivilegeCount = 1;  // one privilege to set
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;	// Get the shutdown privilege for this process.
				AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
				ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
			}
			catch(...)
			{
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_REBOOT) + _T(' ') + GetResString(IDS_FAILED));
			}
		}

		Out += GetHeader(Data, lSession);
		if (strCmd == _T("server"))
			Out += GetServerList(Data);
		else if (strCmd == _T("shared"))
			Out += GetSharedFilesList(Data);
		else if (strCmd == _T("transfer"))
			Out += GetTransferList(Data);
		else if (strCmd == _T("search"))
			Out += GetSearch(Data);
		else if (strCmd == _T("graphs"))
			Out += GetGraphs(Data);
		else if (strCmd == _T("log"))
			Out += GetLog(Data);
		else if (strCmd == _T("sinfo"))
			Out += GetServerInfo(Data);
		else if (strCmd == _T("debuglog"))
			Out += GetDebugLog(Data);
		else if (strCmd == _T("stats"))
			Out += _GetStats(Data);
		else if (strCmd == _T("options"))
			Out += GetPreferences(Data);
		Out += _GetFooter(Data);
	}
	else
	{
		Out += _GetLoginScreen(Data, isUseGzip);
	}

	if(isUseGzip)
	{
		bool bOk = false;
		try
		{
			unsigned uiDstLen = Out.GetLength() + 1024;
			gzipOut = new TCHAR[uiDstLen];
			if (GzipCompress((byte*)gzipOut, &uiDstLen, (const byte*)(TCHAR*)Out.GetBuffer(0), Out.GetLength(), Z_DEFAULT_COMPRESSION) == Z_OK)
			{
				bOk = true;
				uiGzipLen = uiDstLen;
			}
		}
		catch(...)
		{
		}
		if(!bOk)
		{
			AddDebugLogLine(RGB_LOG_ERROR_TXT _T("WebServer: Gzip FAILED on Cmd=%s (AcceptEncoding=%s)"), strCmd, Data.strAcceptEncoding);
			isUseGzip = false;
			delete[] gzipOut;
			gzipOut = NULL;
		}
	}

#if 0
	AddDebugLogLine(_T("WebServer: request prepared (Sz=%u)"), (!isUseGzip) ? Out.GetLength() : uiGzipLen);
#endif
	// send answer ...
	if(!isUseGzip)
		Data.pSocket->SendContent(HTTPInit, Out, Out.GetLength());
	else
		Data.pSocket->SendContent(HTTPInit "Content-Encoding: gzip\r\n", gzipOut, uiGzipLen);

	delete[] gzipOut;

#if 0
	AddDebugLogLine(_T("WebServer: request sent"));
#endif
	EMULE_CATCH

	CoUninitialize();

	EMULE_CATCH2
}

CString CWebServer::_ParseURLArray(CString URL, CString fieldname)
{
	EMULE_TRY

	CString res,temp;

	while (URL.GetLength()>0)
	{
		int pos=URL.MakeLower().Find(fieldname.MakeLower() + _T('='));
		if (pos >= 0)
		{
			temp=_ParseURL(URL,fieldname);
			if (temp.IsEmpty())
				break;
			res += temp;
			res += _T('|');
			URL.Delete(pos,10);
		}
		else
			break;
	}
	return res;

	EMULE_CATCH
	return _T("");
}

CString CWebServer::_ParseURL(const CString &URL, const TCHAR *pcFieldName)
{
	EMULE_TRY

	CString value;
	int i, findPos, findLength = 0;

	if ((findPos = URL.Find(_T('?'))) >= 0)
	{
		CString	fieldname(pcFieldName);
		CString	Parameter = URL.Mid(findPos + 1, URL.GetLength() - findPos - 1);

	// Search the fieldname beginning / middle and strip the rest...
		fieldname += _T('=');
		findPos = -1;
		if (Parameter.Find(fieldname) == 0)
		{
			findPos = 0;
			findLength = fieldname.GetLength();
		}
		if ((i = Parameter.Find(_T('&') + fieldname)) >= 0)
		{
			findPos = i;
			findLength = fieldname.GetLength() + 1;
		}
		if (findPos >= 0)
		{
			TCHAR fromReplace[4] = { _T('%') };	// decode URL
			TCHAR toReplace[2] = _T("\0");	// decode URL

			Parameter = Parameter.Mid(findPos + findLength, Parameter.GetLength());
			if ((findPos = Parameter.Find(_T('&'))) >= 0)
				Parameter = Parameter.Mid(0, findPos);

			value = Parameter;

			// decode value ...
			value.Replace(_T('+'), _T(' '));
			for (i = 0; i <= 255; i++)
			{
				_stprintf(&fromReplace[1], _T("%02x"), i);
				toReplace[0] = static_cast<TCHAR>(i);
				value.Replace(fromReplace, toReplace);
				_stprintf(&fromReplace[1], _T("%02X"), i);
				value.Replace(fromReplace, toReplace);
			}
		}
	}

	return value;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetHeader(const ThreadData &Data, long lSession)
{
	EMULE_TRY

	CString sSession; sSession.Format(_T("%ld"), lSession);
	CString Out(m_Templates.sHeader);

	Out.Replace(_T("[CharSet]"), _GetWebCharSet());

//	Auto-refresh code
	CString sRefresh, sCat, strVersionCheck;
	CString strTmp = _ParseURL(Data.sURL, _T("cat"));
	CString strCmd = _ParseURL(Data.sURL, _T("w"));
	bool bAdmin = IsSessionAdmin(Data,sSession);
	int cat = _tstoi(strTmp);

	if (cat != 0)
		sCat.Format(_T("&cat=%u"), cat);

	if (strCmd == _T("options") || strCmd == _T("stats") || strCmd == _T("password"))
		sRefresh = _T("0");
	else
		sRefresh.Format(_T("%d"), g_App.m_pPrefs->GetWebPageRefresh()*1000);

	strCmd.AppendFormat(_T("&amp;cat=%s&amp;dummy=%u"), strTmp, rand());
	strVersionCheck.Format(_T("http://emuleplus.info/version_check.php?version=%i&language=%i"),CURRENT_PLUS_VERSION,g_App.m_pPrefs->GetLanguageID());

	Out.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[RefreshVal]"), sRefresh);
	Out.Replace(_T("[wCommand]"), strCmd);

	strCmd.Replace(_T("&amp;"), _T("&"));

	Out.Replace(_T("[wCommandJ]"), strCmd);
	Out.Replace(_T("[Nick]"), _SpecialChars(g_App.m_pPrefs->GetUserNick()));
	Out.Replace(_T("[eMuleAppName]"), CLIENT_NAME);
	Out.Replace(_T("[version]"), CURRENT_VERSION_LONG);
	Out.Replace(_T("[WebControl]"), _GetPlainResString(IDS_WEB_CONTROL));
	Out.Replace(_T("[StyleSheet]"), m_Templates.sHeaderStylesheet);
	Out.Replace(_T("[Transfer]"), _GetPlainResString(IDS_CD_TRANS));
	Out.Replace(_T("[Server]"), _GetPlainResString(IDS_SERVERS));
	Out.Replace(_T("[Shared]"), _GetPlainResString(IDS_SHAREDFILES));
	Out.Replace(_T("[Graphs]"), _GetPlainResString(IDS_GRAPHS));
	Out.Replace(_T("[Log]"), _GetPlainResString(IDS_SV_LOG));
	Out.Replace(_T("[ServerInfo]"), _GetPlainResString(IDS_SV_SERVERINFO));
	Out.Replace(_T("[DebugLog]"), _GetPlainResString(IDS_SV_DEBUGLOG));
	Out.Replace(_T("[Stats]"), _GetPlainResString(IDS_STATISTICS));
	Out.Replace(_T("[Options]"), GetResString(IDS_PREFERENCES));
	Out.Replace(_T("[Ed2klink]"), _GetPlainResString(IDS_SW_LINK));
	Out.Replace(_T("[Close]"), _GetPlainResString(IDS_WEB_CLOSE, true));
	Out.Replace(_T("[Reboot]"), _GetPlainResString(IDS_WEB_REBOOT, true));
	Out.Replace(_T("[Shutdown]"), _GetPlainResString(IDS_WEB_SHUTDOWN, true));
	Out.Replace(_T("[Logout]"), _GetPlainResString(IDS_WEB_LOGOUT));
	Out.Replace(_T("[WebOptions]"), _GetPlainResString(IDS_WEB_OPTIONS));
	Out.Replace(_T("[Search]"), _GetPlainResString(IDS_SEARCH_NOUN));
	Out.Replace(_T("[Download]"), _GetPlainResString(IDS_SW_DOWNLOAD));
	Out.Replace(_T("[Start]"), _GetPlainResString(IDS_SW_START));
	Out.Replace(_T("[Version]"), _GetPlainResString(IDS_SERVER_VERSION));
	Out.Replace(_T("[VersionCheck]"), strVersionCheck);
	if (CCat::GetNumCats() > 1)
		InsertCatBox(Out, 0, m_Templates.sCatArrow, false, false, sSession, NULL);
	else
		Out.Replace(_T("[CATBOX]"), _T(""));

	Out.Replace(_T("[FileIsHashing]"), _GetPlainResString(IDS_HASHING, true));
	Out.Replace(_T("[FileIsErroneous]"), _GetPlainResString(IDS_ERRORLIKE, true));
	Out.Replace(_T("[FileIsCompleting]"), _GetPlainResString(IDS_COMPLETING, true));
	Out.Replace(_T("[FileDetails]"), _GetPlainResString(IDS_FD_TITLE, true));
	Out.Replace(_T("[ClearCompleted]"), _GetPlainResString(IDS_DL_CLEAR, true));
	Out.Replace(_T("[A4AF]"), _GetPlainResString(IDS_ALL_A4AF_TO_HERE, true));
	Out.Replace(_T("[A4AFSameCat]"), _GetPlainResString(IDS_ALL_A4AF_SAMECAT, true));
	Out.Replace(_T("[AUTOA4AF]"), _GetPlainResString(IDS_TREE_DL_A4AF_AUTO, true));
	Out.Replace(_T("[A4AFSwap]"), _GetPlainResString(IDS_ALL_A4AF_TO_OTHER, true));
	Out.Replace(_T("[Resume]"), _GetPlainResString(IDS_RESUME, true));
	Out.Replace(_T("[Stop]"), _GetPlainResString(IDS_STOP_VERB, true));
	Out.Replace(_T("[Pause]"), _GetPlainResString(IDS_PAUSE_VERB, true));
	Out.Replace(_T("[ConfirmCancel]"), _GetPlainResString(IDS_Q_CANCELDL2, true));
	Out.Replace(_T("[Cancel]"), GetResString(IDS_MAIN_BTN_CANCEL));
	Out.Replace(_T("[GetFLC]"), _GetPlainResString(IDS_MOVIE, true));
	Out.Replace(_T("[Rename]"), _GetPlainResString(IDS_RENAME, true));
	Out.Replace(_T("[Connect]"), _GetPlainResString(IDS_IRC_CONNECT, true));
	Out.Replace(_T("[ConfirmRemove]"), _GetPlainResString(IDS_WEB_CONFIRM_REMOVE_SERVER, false));	// don't change (') as a string is for message box
	Out.Replace(_T("[ConfirmClose]"), _GetPlainResString(IDS_MAIN_CLOSE, false));	// don't change (') as a string is for message box
	Out.Replace(_T("[ConfirmReboot]"), _GetPlainResString(IDS_MAIN_REBOOT, false));	// don't change (') as a string is for message box
	Out.Replace(_T("[ConfirmShutdown]"), _GetPlainResString(IDS_MAIN_SHUTDOWN, false));	// don't change (') as a string is for message box
	Out.Replace(_T("[RemoveServer]"), _GetPlainResString(IDS_REMOVETHIS, true));
	Out.Replace(_T("[StaticServer]"), _GetPlainResString(IDS_STATICSERVER, true));
	Out.Replace(_T("[ConfirmJumpstart]"), _GetPlainResString(IDS_JS_DISABLE, true) + _T("\\n"));
	Out.Replace(_T("[Friend]"), _GetPlainResString(IDS_FRIEND, true));
	Out.Replace(_T("[CatSel]"), sCat);

	Out.Replace(_T("[PriorityVeryLow]"), _GetPlainResString(IDS_PRIOVERYLOW, true));
	Out.Replace(_T("[PriorityLow]"), _GetPlainResString(IDS_PRIOLOW, true));
	Out.Replace(_T("[PriorityNormal]"), _GetPlainResString(IDS_PRIONORMAL, true));
	Out.Replace(_T("[PriorityHigh]"), _GetPlainResString(IDS_PRIOHIGH, true));
	Out.Replace(_T("[PriorityRelease]"), _GetPlainResString(IDS_PRIORELEASE, true));
	Out.Replace(_T("[PriorityAuto]"), _GetPlainResString(IDS_PRIOAUTO, true));

	Out.Replace(_T("[SetTimerOn]"), _GetPlainResString(IDS_TIMER_ON));
	Out.Replace(_T("[SetTimerOff]"), _GetPlainResString(IDS_TIMER_OFF));
	Out.Replace(_T("[SetTimerDisabled]"), _GetPlainResString(IDS_DISABLED));

	CString HTTPConText, HTTPHelp;
	CString HTTPHelpU = _T("0");
	CString HTTPHelpM = _T("0");
	CString HTTPHelpV = _T("0");
	CString HTTPHelpF = _T("0");
	TCHAR HTTPHeader[100] = _T(""), *pcHTTPConState;

	CString sCmd = _ParseURL(Data.sURL, _T("c"));

	if (sCmd.CompareNoCase(_T("togglemenulock")) == 0)
		m_pWSPrefs->bMenuLocked = !m_pWSPrefs->bMenuLocked;

	Out.Replace(_T("[MenuLocked]"), m_pWSPrefs->bMenuLocked ? _T("true") : _T("false"));
	Out.Replace(_T("[MenuLockTitle]"), _GetPlainResString(m_pWSPrefs->bMenuLocked ? IDS_MENULOCK_OFF : IDS_MENULOCK_ON));

	bool disconnectissued = (sCmd.CompareNoCase(_T("disconnect")) == 0);
	bool connectissued = (sCmd.CompareNoCase(_T("connect")) == 0);
	uint32		dwMax;

#ifdef OLD_SOCKETS_ENABLED
	if ((g_App.m_pServerConnect->IsConnecting() && !disconnectissued) || connectissued)
	{
		pcHTTPConState = _T("connecting");
		HTTPConText = _GetPlainResString(IDS_CONNECTING);
	}
	else if (g_App.m_pServerConnect->IsConnected() && !disconnectissued)
	{
		pcHTTPConState = (g_App.m_pServerConnect->IsLowID()) ? _T("low") : _T("high");

		CServer	*pCurServer = g_App.m_pServerConnect->GetCurrentServer();
		CServer	*cur_server = g_App.m_pServerList->GetServerByAddress(
			pCurServer->GetAddress(), pCurServer->GetPort() );

		if(cur_server->GetListName().GetLength() > SHORT_LENGTH)
			HTTPConText = CString(cur_server->GetListName().Left(SHORT_LENGTH-3) + _T("..."));
		else
			HTTPConText = cur_server->GetListName();
		HTTPHelpU = CastItoThousands(cur_server->GetNumUsers());
		HTTPHelpM = CastItoThousands(cur_server->GetMaxUsers());
		HTTPHelpF = CastItoThousands(cur_server->GetFiles());
		if ((dwMax = cur_server->GetMaxUsers()) != 0)
			HTTPHelpV.Format(_T("%u"), (100 * cur_server->GetNumUsers() + dwMax / 2) / dwMax);
	}
	else
#endif //OLD_SOCKETS_ENABLED
	{
		pcHTTPConState = _T("disconnected");
		HTTPConText = _GetPlainResString(IDS_DISCONNECTED);
		if (IsSessionAdmin(Data, sSession))
			HTTPConText.AppendFormat(_T(" (<a href=\"/?ses=%s&amp;w=server&amp;c=connect\">%s</a>)"), sSession, _GetPlainResString(IDS_CONNECTTOANYSERVER));
	}
	int allUsers = 0;
	int allFiles = 0;
	for (uint32 sc = 0; sc < g_App.m_pServerList->GetServerCount(); sc++)
	{
		CServer	*cur_server = g_App.m_pServerList->GetServerAt(sc);

		allUsers += cur_server->GetNumUsers();
		allFiles += cur_server->GetFiles();
	}
	Out.Replace(_T("[AllUsers]"), CastItoThousands(allUsers));
	Out.Replace(_T("[AllFiles]"), CastItoThousands(allFiles));
	Out.Replace(_T("[ConState]"), pcHTTPConState);
	Out.Replace(_T("[ConText]"), HTTPConText);
	_stprintf(HTTPHeader, _T("%.f"), static_cast<double>(100 * g_App.m_pUploadQueue->GetDataRate()) / 102.4 / g_App.m_pPrefs->GetMaxUpload());
	Out.Replace(_T("[UploadValue]"), HTTPHeader);

	if(g_App.m_pPrefs->GetMaxDownload() == UNLIMITED)
		_stprintf(HTTPHeader, _T("%.f"), static_cast<double>(100 * g_App.m_pDownloadQueue->GetDataRate()) / 102.4 / g_App.m_pPrefs->GetMaxGraphDownloadRate());
	else
		_stprintf(HTTPHeader, _T("%.f"), static_cast<double>(100 * g_App.m_pDownloadQueue->GetDataRate()) / 102.4 / g_App.m_pPrefs->GetMaxDownload());
	Out.Replace(_T("[DownloadValue]"), HTTPHeader);
#ifdef OLD_SOCKETS_ENABLED
	dwMax = g_App.m_pPrefs->GetMaxConnections();
	_stprintf(HTTPHeader, _T("%u"), (100 * g_App.m_pListenSocket->GetNumOpenSockets() + dwMax / 2) / dwMax);
	Out.Replace(_T("[ConnectionValue]"), HTTPHeader);
	_stprintf(HTTPHeader, _T("%.1f"), (static_cast<double>(g_App.m_pUploadQueue->GetDataRate())/1024.0));
	Out.Replace(_T("[CurUpload]"), HTTPHeader);
	_stprintf(HTTPHeader, _T("%.1f"), (static_cast<double>(g_App.m_pDownloadQueue->GetDataRate())/1024.0));
	Out.Replace(_T("[CurDownload]"), HTTPHeader);
	_stprintf(HTTPHeader, _T("%u"), g_App.m_pListenSocket->GetNumOpenSockets());
	Out.Replace(_T("[CurConnection]"), HTTPHeader);
#endif //OLD_SOCKETS_ENABLED

	FractionalRate2String(&HTTPHelp, g_App.m_pPrefs->GetMaxUpload());
	Out.Replace(_T("[MaxUpload]"), HTTPHelp);

	dwMax = g_App.m_pPrefs->GetMaxDownload();
	if (dwMax == UNLIMITED)
		HTTPHelp = GetResString(IDS_PW_UNLIMITED);
	else
		FractionalRate2String(&HTTPHelp, dwMax);
	Out.Replace(_T("[MaxDownload]"), HTTPHelp);

	HTTPHelp.Format(_T("%u"), g_App.m_pPrefs->GetMaxConnections());
	Out.Replace(_T("[MaxConnection]"), HTTPHelp);
	Out.Replace(_T("[UserValue]"), HTTPHelpV);
	Out.Replace(_T("[MaxUsers]"), HTTPHelpM);
	Out.Replace(_T("[CurUsers]"), HTTPHelpU);
	Out.Replace(_T("[CurFiles]"), HTTPHelpF);

	COleDateTime	CurDateTime(COleDateTime::GetCurrentTime());

	Out.Replace(_T("[CurDate]"), CurDateTime.Format(_T("%Y.%m.%d")));
	Out.Replace(_T("[CurTime]"), CurDateTime.Format(_T("%H:%M:%S")));
	Out.Replace(_T("[Connection]"), _GetPlainResString(IDS_PW_CONNECTION));
	Out.Replace(_T("[QuickStats]"), _GetPlainResString(IDS_PW_QUICKSTATS));

	Out.Replace(_T("[Users]"), _GetPlainResString(IDS_UUSERS));
	Out.Replace(_T("[Files]"), _GetPlainResString(IDS_FILES));
	Out.Replace(_T("[Con]"), _GetPlainResString(IDS_ST_ACTIVECONNECTIONS));
	Out.Replace(_T("[Up]"), _GetPlainResString(IDS_UPLOAD_NOUN));
	Out.Replace(_T("[Down]"), _GetPlainResString(IDS_DOWNLOAD_NOUN));
	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::_GetFooter(const ThreadData &Data)
{
	EMULE_TRY

	CWebServer *pThis = (CWebServer *)Data.pThis;
	if(pThis == NULL)
		return _T("");

	return pThis->m_Templates.sFooter;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetServerList(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));
	bool bAdmin = IsSessionAdmin(Data,sSession);
	CString sAddServerBox = GetAddServerBox(Data);

	CString sCmd(_ParseURL(Data.sURL, _T("c")));
	CString sIP(_ParseURL(Data.sURL, _T("ip")));
	uint16 uPort = static_cast<uint16>(_tstoi(_ParseURL(Data.sURL, _T("port"))));
	if (bAdmin && sCmd.CompareNoCase(_T("connect")) == 0)
	{
		if(sIP.IsEmpty())
			g_App.m_pMDlg->SendMessage(WM_COMMAND, MP_CONNECT);
		else
			_ConnectToServer(sIP, uPort);
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("disconnect")) == 0)
	{
#ifdef OLD_SOCKETS_ENABLED
		if (g_App.m_pServerConnect->IsConnecting())
		{
			g_App.m_pServerConnect->StopConnectionTry();
			g_App.m_pMDlg->ShowConnectionState(false);
		}
		else
#endif //OLD_SOCKETS_ENABLED
			g_App.m_pMDlg->SendMessage(WM_COMMAND, MP_DISCONNECT);
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("remove")) == 0)
	{
		if(!sIP.IsEmpty())
			_RemoveServer(sIP, uPort);
	}
	else if (bAdmin && sCmd.CollateNoCase(_T("addtostatic")) == 0)
	{
		if(!sIP.IsEmpty())
			_AddToStatic(sIP, uPort);
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("removefromstatic")) == 0)
	{
		if(!sIP.IsEmpty())
			_RemoveFromStatic(sIP, uPort);
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("priolow")) == 0)
	{
		if(!sIP.IsEmpty())
		{
			CServer	*server = g_App.m_pServerList->GetServerByAddress(sIP, uPort);
			server->SetPreference(PR_LOW);
			if (g_App.m_pMDlg->m_wndServer)
				g_App.m_pMDlg->m_wndServer.m_ctlServerList.RefreshServer(*server);
		}
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("prionormal")) == 0)
	{
		if(!sIP.IsEmpty())
		{
			CServer	*server = g_App.m_pServerList->GetServerByAddress(sIP, uPort);
			server->SetPreference(PR_NORMAL);
			if (g_App.m_pMDlg->m_wndServer)
				g_App.m_pMDlg->m_wndServer.m_ctlServerList.RefreshServer(*server);
		}
	}
	else if (bAdmin && sCmd.CompareNoCase(_T("priohigh")) == 0)
	{
		if(!sIP.IsEmpty())
		{
			CServer	*server = g_App.m_pServerList->GetServerByAddress(sIP, uPort);
			server->SetPreference(PR_HIGH);
			if (g_App.m_pMDlg->m_wndServer)
				g_App.m_pMDlg->m_wndServer.m_ctlServerList.RefreshServer(*server);
		}
	}
	else if (sCmd.CompareNoCase(_T("menu")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abServerColHidden, ARRSIZE(m_pWSPrefs->abServerColHidden));

	CString strTmp = _ParseURL(Data.sURL, _T("sortreverse"));
	CString strSort = _ParseURL(Data.sURL, _T("sort"));

	if (!strSort.IsEmpty())
	{
		bool	bDirection = false;

		if(strSort == _T("state"))
			m_Params.ServerSort = SERVER_SORT_STATE;
		else if(strSort == _T("name"))
		{
			m_Params.ServerSort = SERVER_SORT_NAME;
			bDirection = true;
		}
		else if(strSort == _T("ip"))
			m_Params.ServerSort = SERVER_SORT_IP;
		else if(strSort == _T("description"))
		{
			m_Params.ServerSort = SERVER_SORT_DESCRIPTION;
			bDirection = true;
		}
		else if(strSort == _T("ping"))
			m_Params.ServerSort = SERVER_SORT_PING;
		else if(strSort == _T("users"))
			m_Params.ServerSort = SERVER_SORT_USERS;
		else if(strSort == _T("files"))
			m_Params.ServerSort = SERVER_SORT_FILES;
		else if(strSort == _T("priority"))
			m_Params.ServerSort = SERVER_SORT_PRIORITY;
		else if(strSort == _T("failed"))
			m_Params.ServerSort = SERVER_SORT_FAILED;
		else if(strSort == _T("limit"))
			m_Params.ServerSort = SERVER_SORT_LIMIT;
		else if(strSort == _T("version"))
			m_Params.ServerSort = SERVER_SORT_VERSION;

		if (strTmp.IsEmpty())
			m_Params.bServerSortReverse = bDirection;
	}
	if (!strTmp.IsEmpty())
		m_Params.bServerSortReverse = (strTmp == "true");

	CString Out = m_Templates.sServerList;

	Out.Replace(_T("[AddServerBox]"), sAddServerBox);
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[CatSel]"), sCat);

	const TCHAR *pcTmp = (m_Params.bServerSortReverse) ? _T("&amp;sortreverse=false") : _T("&amp;sortreverse=true");

	Out.Replace(_T("[SortState]"), (m_Params.ServerSort == SERVER_SORT_STATE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortName]"), (m_Params.ServerSort == SERVER_SORT_NAME) ? pcTmp : _T(""));
	Out.Replace(_T("[SortIP]"), (m_Params.ServerSort == SERVER_SORT_IP) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDescription]"), (m_Params.ServerSort == SERVER_SORT_DESCRIPTION) ? pcTmp : _T(""));
	Out.Replace(_T("[SortPing]"), (m_Params.ServerSort == SERVER_SORT_PING) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUsers]"), (m_Params.ServerSort == SERVER_SORT_USERS) ? pcTmp : _T(""));
	Out.Replace(_T("[SortFiles]"), (m_Params.ServerSort == SERVER_SORT_FILES) ? pcTmp : _T(""));
	Out.Replace(_T("[SortPriority]"), (m_Params.ServerSort == SERVER_SORT_PRIORITY) ? pcTmp : _T(""));
	Out.Replace(_T("[SortFailed]"), (m_Params.ServerSort == SERVER_SORT_FAILED) ? pcTmp : _T(""));
	Out.Replace(_T("[SortLimit]"), (m_Params.ServerSort == SERVER_SORT_LIMIT) ? pcTmp : _T(""));
	Out.Replace(_T("[SortVersion]"), (m_Params.ServerSort == SERVER_SORT_VERSION) ? pcTmp : _T(""));
	Out.Replace(_T("[ServerList]"), _GetPlainResString(IDS_SERVERS));

	const TCHAR *pcSortIcon = (m_Params.bServerSortReverse) ? m_Templates.sUpArrow : m_Templates.sDownArrow;

	_GetPlainResString(&strTmp, IDS_SL_SERVERNAME, true);
	if (!m_pWSPrefs->abServerColHidden[0])
	{
		Out.Replace(_T("[ServernameI]"), (m_Params.ServerSort == SERVER_SORT_NAME) ? pcSortIcon : _T(""));
		Out.Replace(_T("[ServernameH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[ServernameI]"), _T(""));
		Out.Replace(_T("[ServernameH]"), _T(""));
	}
	Out.Replace(_T("[ServernameM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_IP, true);
	if (!m_pWSPrefs->abServerColHidden[1])
	{
		Out.Replace(_T("[AddressI]"), (m_Params.ServerSort == SERVER_SORT_IP) ? pcSortIcon : _T(""));
		Out.Replace(_T("[AddressH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[AddressI]"), _T(""));
		Out.Replace(_T("[AddressH]"), _T(""));
	}
	Out.Replace(_T("[AddressM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DESCRIPTION, true);
	if (!m_pWSPrefs->abServerColHidden[2])
	{
		Out.Replace(_T("[DescriptionI]"), (m_Params.ServerSort == SERVER_SORT_DESCRIPTION) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DescriptionH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DescriptionI]"), _T(""));
		Out.Replace(_T("[DescriptionH]"), _T(""));
	}
	Out.Replace(_T("[DescriptionM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_PING, true);
	if (!m_pWSPrefs->abServerColHidden[3])
	{
		Out.Replace(_T("[PingI]"), (m_Params.ServerSort == SERVER_SORT_PING) ? pcSortIcon : _T(""));
		Out.Replace(_T("[PingH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[PingI]"), _T(""));
		Out.Replace(_T("[PingH]"), _T(""));
	}
	Out.Replace(_T("[PingM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_UUSERS, true);
	if (!m_pWSPrefs->abServerColHidden[4])
	{
		Out.Replace(_T("[UsersI]"), (m_Params.ServerSort == SERVER_SORT_USERS) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UsersH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UsersI]"), _T(""));
		Out.Replace(_T("[UsersH]"), _T(""));
	}
	Out.Replace(_T("[UsersM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_FILES, true);
	if (!m_pWSPrefs->abServerColHidden[5])
	{
		Out.Replace(_T("[FilesI]"), (m_Params.ServerSort == SERVER_SORT_FILES) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FilesH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FilesI]"), _T(""));
		Out.Replace(_T("[FilesH]"), _T(""));
	}
	Out.Replace(_T("[FilesM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_PRIORITY, true);
	if (!m_pWSPrefs->abServerColHidden[6])
	{
		Out.Replace(_T("[PriorityI]"), (m_Params.ServerSort == SERVER_SORT_PRIORITY) ? pcSortIcon : _T(""));
		Out.Replace(_T("[PriorityH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[PriorityI]"), _T(""));
		Out.Replace(_T("[PriorityH]"), _T(""));
	}
	Out.Replace(_T("[PriorityM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_UFAILED, true);
	if (!m_pWSPrefs->abServerColHidden[7])
	{
		Out.Replace(_T("[FailedI]"), (m_Params.ServerSort == SERVER_SORT_FAILED) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FailedH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FailedI]"), _T(""));
		Out.Replace(_T("[FailedH]"), _T(""));
	}
	Out.Replace(_T("[FailedM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SERVER_SOFTHARDLIMIT, true);
	if (!m_pWSPrefs->abServerColHidden[8])
	{
		Out.Replace(_T("[LimitI]"), (m_Params.ServerSort == SERVER_SORT_LIMIT) ? pcSortIcon : _T(""));
		Out.Replace(_T("[LimitH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[LimitI]"), _T(""));
		Out.Replace(_T("[LimitH]"), _T(""));
	}
	Out.Replace(_T("[LimitM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SERVER_VERSION, true);
	if (!m_pWSPrefs->abServerColHidden[9])
	{
		Out.Replace(_T("[VersionI]"), (m_Params.ServerSort == SERVER_SORT_VERSION) ? pcSortIcon : _T(""));
		Out.Replace(_T("[VersionH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[VersionI]"), _T(""));
		Out.Replace(_T("[VersionH]"), _T(""));
	}
	Out.Replace(_T("[VersionM]"), strTmp);

	Out.Replace(_T("[Actions]"), _GetPlainResString(IDS_WEB_ACTIONS));

	CArray<ServerEntry, ServerEntry> ServerArray;

#ifdef OLD_SOCKETS_ENABLED
	CServer		*tempServer;
#endif OLD_SOCKETS_ENABLED
	// Populating array
	for (uint32 sc=0;sc<g_App.m_pServerList->GetServerCount();sc++)
	{
		CServer* cur_file = g_App.m_pServerList->GetServerAt(sc);
		ServerEntry Entry;
		Entry.sServerName = _SpecialChars(cur_file->GetListName());
		Entry.sServerIP = cur_file->GetAddress();
		Entry.nServerPort = cur_file->GetPort();
		Entry.sServerDescription = _SpecialChars(cur_file->GetDescription());
		Entry.nServerPing = cur_file->GetPing();
		Entry.nServerUsers = cur_file->GetNumUsers();
		Entry.nServerMaxUsers = cur_file->GetMaxUsers();
		Entry.nServerFiles = cur_file->GetFiles();
		Entry.bServerStatic = cur_file->IsStaticMember();
		if(cur_file->GetPreferences() == PR_HIGH)
		{
			Entry.sServerPriority = _GetPlainResString(IDS_PRIOHIGH);
			Entry.nServerPriority = 2;
		}
		else if(cur_file->GetPreferences() == PR_NORMAL)
		{
			Entry.sServerPriority = _GetPlainResString(IDS_PRIONORMAL);
			Entry.nServerPriority = 1;
		}
		else if(cur_file->GetPreferences() == PR_LOW)
		{
			Entry.sServerPriority = _GetPlainResString(IDS_PRIOLOW);
			Entry.nServerPriority = 0;
		}
		Entry.nServerFailed = cur_file->GetFailedCount();
		Entry.nServerSoftLimit = cur_file->GetSoftMaxFiles();
		Entry.nServerHardLimit = cur_file->GetHardMaxFiles();
		Entry.sServerVersion = cur_file->GetVersion();
		int counter=0;
		CString newip;
		for(int j = 0; j < 4; j++)
		{
			strTmp = Entry.sServerIP.Tokenize(_T("."), counter);
			if (strTmp.GetLength() == 1)
				newip += _T("00");
			else if (strTmp.GetLength() == 2)
				newip += _T("0");
			newip += strTmp;
		}
		Entry.sServerFullIP = newip;

		if (cur_file->GetFailedCount() > 0)
			Entry.sServerState = _T("failed");
		else
			Entry.sServerState = _T("disconnected");
#ifdef OLD_SOCKETS_ENABLED
		if (g_App.m_pServerConnect->IsConnecting())
		{
			tempServer = g_App.m_pServerConnect->GetConnectingServer();
			if (tempServer->GetFullIP() == cur_file->GetFullIP())
				Entry.sServerState = _T("connecting");
		}
		else if (g_App.m_pServerConnect->IsConnected())
		{
			tempServer = g_App.m_pServerConnect->GetCurrentServer();
			if (tempServer->GetFullIP() == cur_file->GetFullIP())
			{
				if (!g_App.m_pServerConnect->IsLowID())
					Entry.sServerState = _T("high");
				else
					Entry.sServerState = _T("low");
			}
		}
#endif //OLD_SOCKETS_ENABLED
		ServerArray.Add(Entry);
	}
	// Sorting (simple bubble sort, we don't have tons of data here)
	bool bSorted = true;
	for(int nMax = 0;bSorted && nMax < ServerArray.GetCount()*2; nMax++)
	{
		bSorted = false;
		for(int i = 0; i < ServerArray.GetCount() - 1; i++)
		{
			bool bSwap = false;
			switch (m_Params.ServerSort)
			{
			case SERVER_SORT_STATE:
				bSwap = ServerArray[i].sServerState.CompareNoCase(ServerArray[i+1].sServerState) > 0;
				break;
			case SERVER_SORT_NAME:
				bSwap = ServerArray[i].sServerName.CompareNoCase(ServerArray[i+1].sServerName) < 0;
				break;
			case SERVER_SORT_IP:
				bSwap = ServerArray[i].sServerFullIP < ServerArray[i+1].sServerFullIP;
				break;
			case SERVER_SORT_DESCRIPTION:
				bSwap = ServerArray[i].sServerDescription.CompareNoCase(ServerArray[i+1].sServerDescription) < 0;
				break;
			case SERVER_SORT_PING:
				bSwap = ServerArray[i].nServerPing < ServerArray[i+1].nServerPing;
				break;
			case SERVER_SORT_USERS:
				bSwap = ServerArray[i].nServerUsers < ServerArray[i+1].nServerUsers;
				break;
			case SERVER_SORT_FILES:
				bSwap = ServerArray[i].nServerFiles < ServerArray[i+1].nServerFiles;
				break;
			case SERVER_SORT_PRIORITY:
				bSwap = ServerArray[i].nServerPriority < ServerArray[i+1].nServerPriority;
				break;
			case SERVER_SORT_FAILED:
				bSwap = ServerArray[i].nServerFailed < ServerArray[i+1].nServerFailed;
				break;
			case SERVER_SORT_LIMIT:
				bSwap = ServerArray[i].nServerSoftLimit < ServerArray[i+1].nServerSoftLimit;
				break;
			case SERVER_SORT_VERSION:
				bSwap = ServerArray[i].sServerVersion < ServerArray[i+1].sServerVersion;
				break;
			}
			if (m_Params.bServerSortReverse)
				bSwap = !bSwap;
			if(bSwap)
			{
				bSorted = true;
				ServerEntry TmpEntry = ServerArray[i];
				ServerArray[i] = ServerArray[i+1];
				ServerArray[i+1] = TmpEntry;
			}
		}
	}

//	Displaying
	const TCHAR	*pcTmp2;
	CString sList, HTTPProcessData, sServerPort, ed2k;
	CString OutE(m_Templates.sServerLine);

	OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));
	OutE.Replace(_T("[session]"), sSession);

	for(int i = 0; i < ServerArray.GetCount(); i++)
	{
		HTTPProcessData = OutE;	// Copy Entry Line to Temp

		sServerPort.Format(_T("%u"), ServerArray[i].nServerPort);
		ed2k.Format(_T("ed2k://|server|%s|%s|/"), ServerArray[i].sServerIP, sServerPort);

		if (ServerArray[i].sServerIP == _ParseURL(Data.sURL,_T("ip")) && sServerPort == _ParseURL(Data.sURL,_T("port")))
			pcTmp = _T("checked");
		else
			pcTmp = _T("checked_no");
		HTTPProcessData.Replace(_T("[LastChangedDataset]"), pcTmp);

		if (ServerArray[i].bServerStatic)
		{
			pcTmp = _T("staticsrv");
			pcTmp2 = _T("static");
		}
		else
		{
			pcTmp = _T("");
			pcTmp2 = _T("none");
		}
		HTTPProcessData.Replace(_T("[isstatic]"), pcTmp);
		HTTPProcessData.Replace(_T("[ServerType]"), pcTmp2);

		const TCHAR *pcSrvPriority = _T("");

		switch(ServerArray[i].nServerPriority)
		{
			case 0:
				pcSrvPriority = _T("Low");
				break;
			case 1:
				pcSrvPriority = _T("Normal");
				break;
			case 2:
				pcSrvPriority = _T("High");
				break;
		}

		HTTPProcessData.Replace(_T("[ed2k]"), ed2k);
		HTTPProcessData.Replace(_T("[ip]"), ServerArray[i].sServerIP);
		HTTPProcessData.Replace(_T("[port]"), sServerPort);
		HTTPProcessData.Replace(_T("[server-priority]"), pcSrvPriority);

		// DonGato: reduced large servernames or descriptions
		if (!m_pWSPrefs->abServerColHidden[0])
		{
			if(ServerArray[i].sServerName.GetLength() > SHORT_LENGTH)
				HTTPProcessData.Replace(_T("[Servername]"), _T("<acronym title=\"") + ServerArray[i].sServerName + _T("\">") + ServerArray[i].sServerName.Left(SHORT_LENGTH-3) + _T("...") + _T("</acronym>"));
			else
				HTTPProcessData.Replace(_T("[Servername]"), ServerArray[i].sServerName);
		}
		else
			HTTPProcessData.Replace(_T("[Servername]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[1])
		{
			CString sPort; sPort.Format(_T(":%d"), ServerArray[i].nServerPort);
			HTTPProcessData.Replace(_T("[Address]"), ServerArray[i].sServerIP + sPort);
		}
		else
			HTTPProcessData.Replace(_T("[Address]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[2])
		{
			if(ServerArray[i].sServerDescription.GetLength() > SHORT_LENGTH)
				HTTPProcessData.Replace(_T("[Description]"), _T("<acronym title=\"") + ServerArray[i].sServerDescription + _T("\">") + ServerArray[i].sServerDescription.Left(SHORT_LENGTH-3) + _T("...") + _T("</acronym>"));
			else
				HTTPProcessData.Replace(_T("[Description]"), ServerArray[i].sServerDescription);
		}
		else
			HTTPProcessData.Replace(_T("[Description]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[3])
		{
			CString sPing; sPing.Format(_T("%d"), ServerArray[i].nServerPing);
			HTTPProcessData.Replace(_T("[Ping]"), sPing);
		}
		else
			HTTPProcessData.Replace(_T("[Ping]"), _T(""));
		CString sT;
		if (!m_pWSPrefs->abServerColHidden[4])
		{
			if(ServerArray[i].nServerUsers > 0)
			{
				if(ServerArray[i].nServerMaxUsers > 0)
					sT.Format(_T("%s (%s)"), CastItoThousands(ServerArray[i].nServerUsers), CastItoThousands(ServerArray[i].nServerMaxUsers));
				else
					sT.Format(_T("%s"), CastItoThousands(ServerArray[i].nServerUsers));
			}
			HTTPProcessData.Replace(_T("[Users]"), sT);
		}
		else
			HTTPProcessData.Replace(_T("[Users]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[5] && (ServerArray[i].nServerFiles > 0))
		{
			HTTPProcessData.Replace(_T("[Files]"), CastItoThousands(ServerArray[i].nServerFiles));
		}
		else
			HTTPProcessData.Replace(_T("[Files]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[6])
			HTTPProcessData.Replace(_T("[Priority]"), ServerArray[i].sServerPriority);
		else
			HTTPProcessData.Replace(_T("[Priority]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[7])
		{
			CString sFailed; sFailed.Format(_T("%d"), ServerArray[i].nServerFailed);
			HTTPProcessData.Replace(_T("[Failed]"), sFailed);
		}
		else
			HTTPProcessData.Replace(_T("[Failed]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[8])
		{
			CString		strTemp;

			strTemp.Format( _T("%s (%s)"), CastItoThousands(ServerArray[i].nServerSoftLimit),
				CastItoThousands(ServerArray[i].nServerHardLimit) );
			HTTPProcessData.Replace(_T("[Limit]"), strTemp);
		}
		else
			HTTPProcessData.Replace(_T("[Limit]"), _T(""));
		if (!m_pWSPrefs->abServerColHidden[9])
		{
			if(ServerArray[i].sServerVersion.GetLength() > SHORT_LENGTH_MIN)
				HTTPProcessData.Replace(_T("[Version]"), _T("<acronym title=\"") + ServerArray[i].sServerVersion + _T("\">") + ServerArray[i].sServerVersion.Left(SHORT_LENGTH-3) + _T("...") + _T("</acronym>"));
			else
				HTTPProcessData.Replace(_T("[Version]"), ServerArray[i].sServerVersion);
		}
		else
			HTTPProcessData.Replace(_T("[Version]"), _T(""));
		HTTPProcessData.Replace(_T("[ServerState]"), ServerArray[i].sServerState);
		sList += HTTPProcessData;
	}
	Out.Replace(_T("[ServersList]"), sList);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetTransferList(const ThreadData &Data)
{
	EMULE_TRY

	CString	strTmp, Out, sCat;
	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	byte	abyteFileHash[16];

	if (iCat != 0)
		sCat.Format(_T("&cat=%u"), iCat);
	Out.Preallocate(50 * 1024);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));

	bool bAdmin = IsSessionAdmin(Data,sSession);

	CString sClear(_ParseURL(Data.sURL, _T("clearcompleted")));
	if (bAdmin && !sClear.IsEmpty())
	{
		if (sClear.CompareNoCase(_T("all")) == 0)
		{
			EnumCategories eCatID = (iCat == 0) ? CAT_ALL : CCat::GetCatIDByUserIndex(iCat);
			_EnumCategories eCatIDforMessage = eCatID;

			g_App.m_pMDlg->SendMessage(WEB_CLEAR_COMPLETED, (WPARAM)0, static_cast<LPARAM>(eCatIDforMessage));

			iCat = (iCat == 0) ? 0 : CCat::UserCatIndexToCatIndex(iCat);
		}
		else if (md4cmp0(StringToHash(sClear, abyteFileHash)) != 0)
		{
			byte	*pabyteFileHash = new uchar[16];

			md4cpy(pabyteFileHash, abyteFileHash);
			g_App.m_pMDlg->SendMessage(WEB_CLEAR_COMPLETED, (WPARAM)1, reinterpret_cast<LPARAM>(pabyteFileHash));
		}
	}

	if (bAdmin)
	{
		strTmp = _ParseURL(Data.sURL, _T("ed2k"));
		if (!strTmp.IsEmpty())
		{
			EnumCategories	eCatID = (iCat == 0) ? CAT_NONE : CCat::GetCatIDByUserIndex(iCat);

			g_App.m_pMDlg->m_dlgSearch.AddEd2kLinksToDownload(strTmp, eCatID);
			iCat = (iCat == 0) ? 0 : CCat::UserCatIndexToCatIndex(iCat);
		}
	}

	strTmp = _ParseURL(Data.sURL, _T("c"));
	if (strTmp.Compare(_T("menudown")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abDownloadColHidden, ARRSIZE(m_pWSPrefs->abDownloadColHidden));
	else if (strTmp.Compare(_T("menuup")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abUploadColHidden, ARRSIZE(m_pWSPrefs->abUploadColHidden));
	else if (strTmp.Compare(_T("menuqueue")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abQueueColHidden, ARRSIZE(m_pWSPrefs->abQueueColHidden));
	else if (strTmp.Compare(_T("menuprio")) == 0)
	{
		if (bAdmin)
		{
			byte	bytePrio;
			
			strTmp = _ParseURL(Data.sURL, _T("p"));
			if(strTmp.CompareNoCase(_T("low")) == 0)
				bytePrio = PR_LOW;
			else if(strTmp.CompareNoCase(_T("high")) == 0)
				bytePrio = PR_HIGH;
			else if(strTmp.CompareNoCase(_T("auto")) == 0)
				bytePrio = PR_AUTO;
			else // _T("normal")
				bytePrio = PR_NORMAL;

			g_App.m_pDownloadQueue->SetCatPrio(iCat, bytePrio);
		}
	}

	if (bAdmin)
	{
		CString	sOp(_ParseURL(Data.sURL, _T("op")));

		if (!sOp.IsEmpty())
		{
			CString	sFile(_ParseURL(Data.sURL, _T("file")));
			byte	abyteHashBuf[16];

			if (!sFile.IsEmpty())
			{
				CPartFile	*pFile;

				if ( md4cmp0(StringToHash(sFile, abyteHashBuf)) != 0
					&& (pFile = g_App.m_pDownloadQueue->GetFileByID(abyteHashBuf)) != NULL )
				{	// All actions require a found file (removed double-check inside)
					if (sOp == _T("a4af"))
						pFile->DownloadAllA4AF();
					else if (sOp == _T("a4afsamecat"))
						pFile->DownloadAllA4AF(true);
					else if (sOp == _T("autoa4af"))
						g_App.m_pDownloadQueue->SetA4AFAutoFile(pFile);
					else if (sOp == _T("noautoa4af"))
						g_App.m_pDownloadQueue->SetA4AFAutoFile(NULL);
					else if (sOp == _T("a4afswap"))
					{
						ClientList	clientListCopy;

						pFile->GetCopySourceLists(SLM_ALLOWED_TO_A4AF_SWAP, &clientListCopy);
						for (ClientList::const_iterator cIt = clientListCopy.begin(); cIt != clientListCopy.end(); cIt++)
							(*cIt)->SwapToAnotherFile(NULL);
					}
					else if (sOp == _T("stop"))
						pFile->StopFile();
					else if (sOp == _T("pause"))
						pFile->PauseFile();
					else if (sOp == _T("resume"))
						pFile->ResumeFile();
					else if (sOp == _T("cancel"))
					{
						pFile->DeleteFile();
						g_App.m_pMDlg->m_wndTransfer.UpdateCatTabTitles();
					}
					else if (sOp == _T("getflc"))
						pFile->GetFirstLastChunk4Preview();
					else if (sOp == _T("rename"))
					{
						strTmp = _ParseURL(Data.sURL, _T("name"));
						g_App.m_pMDlg->SendMessage(WEB_FILE_RENAME, (WPARAM)pFile, (LPARAM)(LPCTSTR)strTmp);
					}
					else if (sOp == _T("priolow"))
					{
						pFile->SetAutoPriority(false);
						pFile->SetPriority(PR_LOW);
					}
					else if (sOp == _T("prionormal"))
					{
						pFile->SetAutoPriority(false);
						pFile->SetPriority(PR_NORMAL);
					}
					else if (sOp == _T("priohigh"))
					{
						pFile->SetAutoPriority(false);
						pFile->SetPriority(PR_HIGH);
					}
					else if (sOp == _T("prioauto"))
					{
						pFile->SetAutoPriority(true);
						pFile->SetPriority(PR_HIGH);
					}
					else if (sOp == _T("setcat"))
					{
						int	iMenu = _tstoi(_ParseURL(Data.sURL, _T("filecat")));

						if (iMenu != 0)
							pFile->SetCatID(CCat::GetCatIDByUserIndex(iMenu));
						else
							pFile->SetCatID(CAT_NONE);
					}
				}
			}
			else
			{
				CUpDownClient	*pClient;

				if ( md4cmp0(StringToHash(_ParseURL(Data.sURL, _T("userhash")), abyteHashBuf)) != 0
					&& (pClient = g_App.m_pClientList->FindClientByUserHash(abyteHashBuf)) != NULL )
				{
					if (sOp.CompareNoCase(_T("addfriend")) == 0)
						g_App.m_pFriendList->AddFriend(pClient);
					else if (sOp.CompareNoCase(_T("removefriend")) == 0)
						g_App.m_pFriendList->RemoveFriend(pClient);
				}
			}
		}
	}

	strTmp = _ParseURL(Data.sURL, _T("sortreverse"));

	CString sSort(_ParseURL(Data.sURL, _T("sort")));
	bool	bDirection;

	if (!sSort.IsEmpty())
	{
		bDirection = false;
		if(sSort.CompareNoCase(_T("dstate")) == 0)
			m_Params.DownloadSort = DOWN_SORT_STATE;
		else if(sSort.CompareNoCase(_T("dtype")) == 0)
			m_Params.DownloadSort = DOWN_SORT_TYPE;
		else if(sSort.CompareNoCase(_T("dname")) == 0)
		{
			m_Params.DownloadSort = DOWN_SORT_NAME;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("dsize")) == 0)
			m_Params.DownloadSort = DOWN_SORT_SIZE;
		else if(sSort.CompareNoCase(_T("dtransferred")) == 0)
			m_Params.DownloadSort = DOWN_SORT_TRANSFERRED;
		else if(sSort.CompareNoCase(_T("dspeed")) == 0)
			m_Params.DownloadSort = DOWN_SORT_SPEED;
		else if(sSort.CompareNoCase(_T("dprogress")) == 0)
			m_Params.DownloadSort = DOWN_SORT_PROGRESS;
		else if(sSort.CompareNoCase(_T("dsources")) == 0)
			m_Params.DownloadSort = DOWN_SORT_SOURCES;
		else if(sSort.CompareNoCase(_T("dpriority")) == 0)
			m_Params.DownloadSort = DOWN_SORT_PRIORITY;
		else if(sSort.CompareNoCase(_T("dcategory")) == 0)
		{
			m_Params.DownloadSort = DOWN_SORT_CATEGORY;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("dfakecheck")) == 0)
			m_Params.DownloadSort = DOWN_SORT_FAKECHECK;
		else if(sSort.CompareNoCase(_T("uuser")) == 0)
		{
			m_Params.UploadSort = UP_SORT_USER;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("uclient")) == 0)
			m_Params.UploadSort = UP_SORT_CLIENT;
		else if(sSort.CompareNoCase(_T("uversion")) == 0)
			m_Params.UploadSort = UP_SORT_VERSION;
		else if(sSort.CompareNoCase(_T("ufilename")) == 0)
		{
			m_Params.UploadSort = UP_SORT_FILENAME;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("utransferred")) == 0)
			m_Params.UploadSort = UP_SORT_TRANSFERRED;
		else if(sSort.CompareNoCase(_T("uspeed")) == 0)
			m_Params.UploadSort = UP_SORT_SPEED;
		else if(sSort.CompareNoCase(_T("qclient")) == 0)
			m_Params.QueueSort = QU_SORT_CLIENT;
		else if(sSort.CompareNoCase(_T("quser")) == 0)
		{
			m_Params.QueueSort = QU_SORT_USER;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("qversion")) == 0)
			m_Params.QueueSort = QU_SORT_VERSION;
		else if(sSort.CompareNoCase(_T("qfilename")) == 0)
		{
			m_Params.QueueSort = QU_SORT_FILENAME;
			bDirection = true;
		}
		else if(sSort.CompareNoCase(_T("qscore")) == 0)
			m_Params.QueueSort = QU_SORT_SCORE;

		if (strTmp.IsEmpty())
		{
			if (sSort.GetAt(0) == 'd')
				m_Params.bDownloadSortReverse = bDirection;
			else if (sSort.GetAt(0) == 'u')
				m_Params.bUploadSortReverse = bDirection;
			else if (sSort.GetAt(0) == 'q')
				m_Params.bQueueSortReverse = bDirection;
		}
	}

	if (!strTmp.IsEmpty())
	{
		bDirection = (strTmp == "true");
		if (sSort.GetAt(0) == 'd')
			m_Params.bDownloadSortReverse = bDirection;
		else if (sSort.GetAt(0) == 'u')
			m_Params.bUploadSortReverse = bDirection;
		else if (sSort.GetAt(0) == 'q')
			m_Params.bQueueSortReverse = bDirection;
	}

	strTmp = _ParseURL(Data.sURL, _T("showuploadqueue"));
	if (strTmp.CompareNoCase(_T("true")) == 0)
		m_pWSPrefs->bShowUploadQueue = true;
	else if (strTmp.CompareNoCase(_T("false")) == 0)
		m_pWSPrefs->bShowUploadQueue = false;

	strTmp = _ParseURL(Data.sURL, _T("showuploadqueuebanned"));
	if (strTmp.CompareNoCase(_T("true")) == 0)
		m_pWSPrefs->bShowUploadQueueBanned = true;
	else if (strTmp.CompareNoCase(_T("false")) == 0)
		m_pWSPrefs->bShowUploadQueueBanned = false;

	strTmp = _ParseURL(Data.sURL, _T("showuploadqueuefriend"));
	if (strTmp.CompareNoCase(_T("true")) == 0)
		m_pWSPrefs->bShowUploadQueueFriend = true;
	else if (strTmp.CompareNoCase(_T("false")) == 0)
		m_pWSPrefs->bShowUploadQueueFriend = false;

	strTmp = _ParseURL(Data.sURL, _T("showuploadqueuecredit"));
	if (strTmp.CompareNoCase(_T("true")) == 0)
		m_pWSPrefs->bShowUploadQueueCredit = true;
	else if (strTmp.CompareNoCase(_T("false")) == 0)
		m_pWSPrefs->bShowUploadQueueCredit = false;

	Out += m_Templates.sTransferImages;
	Out += m_Templates.sTransferList;
	Out.Replace(_T("[DownloadHeader]"), m_Templates.sTransferDownHeader);
	Out.Replace(_T("[DownloadFooter]"), m_Templates.sTransferDownFooter);
	Out.Replace(_T("[UploadHeader]"), m_Templates.sTransferUpHeader);
	Out.Replace(_T("[UploadFooter]"), m_Templates.sTransferUpFooter);
	Out.Replace(_T("[Session]"), sSession);

	InsertCatBox(Out, iCat, _T(""), true, true, sSession, NULL);

	const TCHAR	*pcTmp = (m_Params.bDownloadSortReverse) ? _T("&amp;sortreverse=false") : _T("&amp;sortreverse=true");

	Out.Replace(_T("[SortDState]"), (m_Params.DownloadSort == DOWN_SORT_STATE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDType]"), (m_Params.DownloadSort == DOWN_SORT_TYPE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDName]"), (m_Params.DownloadSort == DOWN_SORT_NAME) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDSize]"), (m_Params.DownloadSort == DOWN_SORT_SIZE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDTransferred]"), (m_Params.DownloadSort == DOWN_SORT_TRANSFERRED) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDSpeed]"), (m_Params.DownloadSort == DOWN_SORT_SPEED) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDProgress]"), (m_Params.DownloadSort == DOWN_SORT_PROGRESS) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDSources]"), (m_Params.DownloadSort == DOWN_SORT_SOURCES) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDPriority]"), (m_Params.DownloadSort == DOWN_SORT_PRIORITY) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDCategory]"), (m_Params.DownloadSort == DOWN_SORT_CATEGORY) ? pcTmp : _T(""));
	Out.Replace(_T("[SortDFakeCheck]"), (m_Params.DownloadSort == DOWN_SORT_FAKECHECK) ? pcTmp : _T(""));

	pcTmp = (m_Params.bUploadSortReverse) ? _T("&amp;sortreverse=false") : _T("&amp;sortreverse=true");

	Out.Replace(_T("[SortUClient]"), (m_Params.UploadSort == UP_SORT_CLIENT) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUUser]"), (m_Params.UploadSort == UP_SORT_USER) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUVersion]"), (m_Params.UploadSort == UP_SORT_VERSION) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUFilename]"), (m_Params.UploadSort == UP_SORT_FILENAME) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUTransferred]"), (m_Params.UploadSort == UP_SORT_TRANSFERRED) ? pcTmp : _T(""));
	Out.Replace(_T("[SortUSpeed]"), (m_Params.UploadSort == UP_SORT_SPEED) ? pcTmp : _T(""));

	const TCHAR *pcSortIcon = (m_Params.bDownloadSortReverse) ? m_Templates.sUpArrow : m_Templates.sDownArrow;

	_GetPlainResString(&strTmp, IDS_DL_FILENAME, true);
	if (!m_pWSPrefs->abDownloadColHidden[0])
	{
		Out.Replace(_T("[DFilenameI]"), (m_Params.DownloadSort == DOWN_SORT_NAME) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DFilename]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DFilenameI]"), _T(""));
		Out.Replace(_T("[DFilename]"), _T(""));
	}
	Out.Replace(_T("[DFilenameM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SIZE, true);
	if (!m_pWSPrefs->abDownloadColHidden[1])
	{
		Out.Replace(_T("[DSizeI]"), (m_Params.DownloadSort == DOWN_SORT_SIZE) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DSize]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DSizeI]"), _T(""));
		Out.Replace(_T("[DSize]"), _T(""));
	}
	Out.Replace(_T("[DSizeM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SF_COMPLETED, true);
	if (!m_pWSPrefs->abDownloadColHidden[2])
	{
		Out.Replace(_T("[DTransferredI]"), (m_Params.DownloadSort == DOWN_SORT_TRANSFERRED) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DTransferred]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DTransferredI]"), _T(""));
		Out.Replace(_T("[DTransferred]"), _T(""));
	}
	Out.Replace(_T("[DTransferredM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_PROGRESS, true);
	if (!m_pWSPrefs->abDownloadColHidden[3])
	{
		Out.Replace(_T("[DProgressI]"), (m_Params.DownloadSort == DOWN_SORT_PROGRESS) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DProgress]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DProgressI]"), _T(""));
		Out.Replace(_T("[DProgress]"), _T(""));
	}
	Out.Replace(_T("[DProgressM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SPEED, true);
	if (!m_pWSPrefs->abDownloadColHidden[4])
	{
		Out.Replace(_T("[DSpeedI]"), (m_Params.DownloadSort == DOWN_SORT_SPEED) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DSpeed]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DSpeedI]"), _T(""));
		Out.Replace(_T("[DSpeed]"), _T(""));
	}
	Out.Replace(_T("[DSpeedM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SOURCES, true);
	if (!m_pWSPrefs->abDownloadColHidden[5])
	{
		Out.Replace(_T("[DSourcesI]"), (m_Params.DownloadSort == DOWN_SORT_SOURCES) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DSources]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DSourcesI]"), _T(""));
		Out.Replace(_T("[DSources]"), _T(""));
	}
	Out.Replace(_T("[DSourcesM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_PRIORITY, true);
	if (!m_pWSPrefs->abDownloadColHidden[6])
	{
		Out.Replace(_T("[DPriorityI]"), (m_Params.DownloadSort == DOWN_SORT_PRIORITY) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DPriority]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DPriorityI]"), _T(""));
		Out.Replace(_T("[DPriority]"), _T(""));
	}
	Out.Replace(_T("[DPriorityM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_CAT, true);
	if (!m_pWSPrefs->abDownloadColHidden[7])
	{
		Out.Replace(_T("[DCategoryI]"), (m_Params.DownloadSort == DOWN_SORT_CATEGORY) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DCategory]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DCategoryI]"), _T(""));
		Out.Replace(_T("[DCategory]"), _T(""));
	}
	Out.Replace(_T("[DCategoryM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_FAKE_CHECK_HEADER, true);
	if (!m_pWSPrefs->abDownloadColHidden[8])
	{
		Out.Replace(_T("[DFakeCheckI]"), (m_Params.DownloadSort == DOWN_SORT_FAKECHECK) ? pcSortIcon : _T(""));
		Out.Replace(_T("[DFakeCheck]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[DFakeCheckI]"), _T(""));
		Out.Replace(_T("[DFakeCheck]"), _T(""));
	}
	Out.Replace(_T("[DFakeCheckM]"), strTmp);

	pcSortIcon = (m_Params.bUploadSortReverse) ? m_Templates.sUpArrow : m_Templates.sDownArrow;

	_GetPlainResString(&strTmp, IDS_QL_USERNAME, true);
	if (!m_pWSPrefs->abUploadColHidden[0])
	{
		Out.Replace(_T("[UUserI]"), (m_Params.UploadSort == UP_SORT_USER) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UUser]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UUserI]"), _T(""));
		Out.Replace(_T("[UUser]"), _T(""));
	}
	Out.Replace(_T("[UUserM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_INFLST_USER_CLIENTVERSION, true);
	if (!m_pWSPrefs->abUploadColHidden[1])
	{
		Out.Replace(_T("[UVersionI]"), (m_Params.UploadSort == UP_SORT_VERSION) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UVersion]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UVersionI]"), _T(""));
		Out.Replace(_T("[UVersion]"), _T(""));
	}
	Out.Replace(_T("[UVersionM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_FILENAME, true);
	if (!m_pWSPrefs->abUploadColHidden[2])
	{
		Out.Replace(_T("[UFilenameI]"), (m_Params.UploadSort == UP_SORT_FILENAME) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UFilename]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UFilenameI]"), _T(""));
		Out.Replace(_T("[UFilename]"), _T(""));
	}
	Out.Replace(_T("[UFilenameM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_ULDL, true);
	if (!m_pWSPrefs->abUploadColHidden[3])
	{
		Out.Replace(_T("[UTransferredI]"), (m_Params.UploadSort == UP_SORT_TRANSFERRED) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UTransferred]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UTransferredI]"), _T(""));
		Out.Replace(_T("[UTransferred]"), _T(""));
	}
	Out.Replace(_T("[UTransferredM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SPEED, true);
	if (!m_pWSPrefs->abUploadColHidden[4])
	{
		Out.Replace(_T("[USpeedI]"), (m_Params.UploadSort == UP_SORT_SPEED) ? pcSortIcon : _T(""));
		Out.Replace(_T("[USpeed]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[USpeedI]"), _T(""));
		Out.Replace(_T("[USpeed]"), _T(""));
	}
	Out.Replace(_T("[USpeedM]"), strTmp);

	Out.Replace(_T("[DownloadList]"), _GetPlainResString(IDS_ST_ACTIVEDOWNLOAD));
	Out.Replace(_T("[UploadList]"), _GetPlainResString(IDS_ST_ACTIVEUPLOAD));
	Out.Replace(_T("[Actions]"), _GetPlainResString(IDS_WEB_ACTIONS));
	Out.Replace(_T("[TotalDown]"), _GetPlainResString(IDS_INFLST_USER_TOTALDOWNLOAD));
	Out.Replace(_T("[TotalUp]"), _GetPlainResString(IDS_INFLST_USER_TOTALUPLOAD));
	Out.Replace(_T("[CatSel]"), sCat);
	Out.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

	Out.Replace(_T("[ClearAllCompleted]"), _GetPlainResString(IDS_TREE_DL_CLEAR_ALL_COMPLETED));

	double	fTotalSpeed = 0, dTmp;
	uint64	qwTotalSize = 0, qwTotalTransferred = 0;
	CArray<DownloadFiles, DownloadFiles> FilesArray;
	bool							bLocalArray = false;
	CDownloadList::PartFileVector	*pvecPartFiles;
	CDownloadList::PartFileVector	vecPartFiles;
	if ((pvecPartFiles = g_App.m_pDownloadList->GetFiles()) == NULL)
	{
		bLocalArray = true;
		pvecPartFiles = &vecPartFiles;
		vecPartFiles.erase(vecPartFiles.begin(),vecPartFiles.end());
	//	Populating array
		for (POSITION pos=g_App.m_pDownloadQueue->m_partFileList.GetHeadPosition(); pos != NULL;)
		{
			CPartFile	*pPartFile = g_App.m_pDownloadQueue->m_partFileList.GetNext(pos);

			if (pPartFile)
				vecPartFiles.push_back(pPartFile);
		}
	}
	for (unsigned int i = 0; i < pvecPartFiles->size(); i++)
	{
		int			iFileStatus;
		CPartFile	*pPartFile = (*pvecPartFiles)[i];

		if (pPartFile != NULL)
		{
			if ( !CCat::FileBelongsToGivenCat( pPartFile, (iCat > CAT_PREDEFINED)
														 ? static_cast<_EnumCategories>(iCat)
														 : CCat::GetCatIDByIndex(iCat), true ) )
				continue;

			if(pPartFile->IsULAutoPrioritized())
				pPartFile->UpdateDownloadAutoPriority();

			DownloadFiles dFile;

			dFile.sFileName = pPartFile->GetFileName();
			SpecialChars(&dFile.sFileName);
			dFile.pcFileType = GetFileTypeForWebServer(dFile.sFileName);
			dFile.sFileNameJS = _SpecialChars(pPartFile->GetFileName(), true);	//for javascript
			dFile.m_qwFileSize = pPartFile->GetFileSize();
			dFile.m_qwFileTransferred = pPartFile->GetCompletedSize();
			dFile.m_dblCompleted = pPartFile->GetPercentCompleted();
			dFile.dwFileSpeed = pPartFile->GetDataRate();

			dFile.bIsComplete = false;
			iFileStatus = pPartFile->GetPartFileStatusID();
			switch (iFileStatus)
			{
				case PS_HASHING:
					dFile.pcFileState = _T("hashing");
					break;
				case PS_WAITINGFORHASH:
					dFile.pcFileState = _T("waitinghash");
					break;
				case PS_ERROR:
					dFile.pcFileState = _T("error");
					break;
				case PS_COMPLETING:
					dFile.bIsComplete = true;
					dFile.pcFileState = _T("completing");
					break;
				case PS_COMPLETE:
					dFile.bIsComplete = true;
					dFile.pcFileState = _T("complete");
					break;
				case PS_STOPPED:
					dFile.pcFileState = _T("stopped");
					break;
				case PS_PAUSED:
					dFile.pcFileState = _T("paused");
					break;
				case PS_DOWNLOADING:
					dFile.pcFileState = _T("downloading");
					break;
				case PS_WAITINGFORSOURCE:
					dFile.pcFileState = _T("waiting");
					break;
				case PS_STALLED:
					dFile.pcFileState = _T("stalled");
					break;
			}

			dFile.bFileAutoPrio = pPartFile->IsAutoPrioritized();
			dFile.nFilePrio = pPartFile->GetPriority();
			CCat		*pCat = CCat::GetCatByID(pPartFile->GetCatID());
			dFile.sCategory = (pCat != NULL) ? pCat->GetTitle() : GetResString(IDS_CAT_UNCATEGORIZED);
			dFile.sCategory.Replace(_T("'"), _T("\'"));
			dFile.sFileHash = HashToString(pPartFile->GetFileHash());
			dFile.lSourceCount = pPartFile->GetSourceCount();
			dFile.lNotCurrentSourceCount = pPartFile->GetNotCurrentSourcesCount();
			dFile.lTransferringSourceCount = pPartFile->GetTransferringSrcCount();
			dFile.bIsPreview = (!dFile.bIsComplete && pPartFile->AllowGet1stLast());
			dFile.bIsGetFLC = (pPartFile->GetMovieMode() != 0);

			dFile.sED2kLink = pPartFile->CreateED2kLink();
			dFile.sFileInfo = pPartFile->GetDownloadFileInfo();
			SpecialChars(&dFile.sFileInfo);
			dFile.sFakeCheck = g_App.m_pFakeCheck->GetFakeCheckComment(HashToString(pPartFile->GetFileHash()),pPartFile->GetFileSize());
			dFile.cSortRank = 0;
			if (g_App.m_pPrefs->DoPausedStoppedLast())
			{
				dFile.cSortRank |= (iFileStatus == PS_PAUSED) ? 1 : 0;
				dFile.cSortRank |= (iFileStatus == PS_STOPPED) ? 2 : 0;
			}
			FilesArray.Add(dFile);
		}
	}

	if (!bLocalArray)
	{
		delete pvecPartFiles;
		pvecPartFiles = NULL;
	}

	// Sorting (simple bubble sort, we don't have tons of data here)
	bool	bSwap, bSorted = true;
	int		iSortRank;
	DownloadFiles	*pFileArr = FilesArray.GetData();

	for(int nMax = 0; bSorted && nMax < FilesArray.GetCount()*2; nMax++)
	{
		bSorted = false;
		for(int i = 0; i < FilesArray.GetCount() - 1; i++)
		{
			if ((iSortRank = (pFileArr[i].cSortRank - pFileArr[i + 1].cSortRank)) != 0)
				bSwap = (iSortRank < 0) ? false : true;
			else
			{
				switch (m_Params.DownloadSort)
				{
					case DOWN_SORT_STATE:
						bSwap = _tcscmp(pFileArr[i].pcFileState, pFileArr[i+1].pcFileState) < 0;
						break;
					case DOWN_SORT_TYPE:
						bSwap = _tcscmp(pFileArr[i].pcFileType, pFileArr[i+1].pcFileType) < 0;
						break;
					case DOWN_SORT_NAME:
						bSwap = pFileArr[i].sFileName.CompareNoCase(pFileArr[i+1].sFileName) < 0;
						break;
					case DOWN_SORT_SIZE:
						bSwap = pFileArr[i].m_qwFileSize < pFileArr[i+1].m_qwFileSize;
						break;
					case DOWN_SORT_TRANSFERRED:
						bSwap = pFileArr[i].m_qwFileTransferred < pFileArr[i+1].m_qwFileTransferred;
						break;
					case DOWN_SORT_SPEED:
						bSwap = pFileArr[i].dwFileSpeed < pFileArr[i+1].dwFileSpeed;
						break;
					case DOWN_SORT_PROGRESS:
						bSwap = pFileArr[i].m_dblCompleted < pFileArr[i+1].m_dblCompleted;
						break;
					case DOWN_SORT_SOURCES:
						bSwap = pFileArr[i].lSourceCount < pFileArr[i+1].lSourceCount;
						break;
					case DOWN_SORT_PRIORITY:
						bSwap = pFileArr[i].nFilePrio < pFileArr[i+1].nFilePrio;
						break;
					case DOWN_SORT_CATEGORY:
						bSwap = pFileArr[i].sCategory.CompareNoCase(pFileArr[i+1].sCategory) < 0;
						break;
					case DOWN_SORT_FAKECHECK:
						bSwap = pFileArr[i].sFakeCheck.CompareNoCase(pFileArr[i+1].sFakeCheck) < 0;
						break;
					default:
						bSwap = false;
						break;
				}
				if (m_Params.bDownloadSortReverse)
					bSwap = !bSwap;
			}
			if(bSwap)
			{
				bSorted = true;
				DownloadFiles TmpFile = pFileArr[i];
				pFileArr[i] = pFileArr[i+1];
				pFileArr[i+1] = TmpFile;
			}
		}
	}

	uint32	dwClientSoft;
	CArray<UploadUsers, UploadUsers> UploadArray;
	ClientList CopyUploadQueueList;

	g_App.m_pUploadQueue->GetCopyUploadQueueList(&CopyUploadQueueList);
	for (ClientList::const_iterator cIt = CopyUploadQueueList.begin(); cIt != CopyUploadQueueList.end(); cIt++)
	{
		CUpDownClient* cur_client = *cIt;
		UploadUsers dUser;

		dUser.sUserHash = HashToString(cur_client->GetUserHash());
		if (cur_client->GetUpDataRate() != 0)
		{
			dUser.pcActive = _T("downloading");
			dUser.pcClientState = _T("uploading");
		}
		else
		{
			dUser.pcActive = _T("waiting");
			dUser.pcClientState = _T("connecting");
		}
		dUser.sFileInfo = cur_client->GetUploadFileInfo();
		SpecialChars(&dUser.sFileInfo);
		dUser.sFileInfo.Replace(_T("\n"), _T("<br />"));
		dUser.sFileInfo.Replace(_T("'"), _T("&#8217;"));

		dwClientSoft = cur_client->GetClientSoft();
		if ( ((dwClientSoft == SO_EMULE) || (dwClientSoft == SO_PLUS)) &&
			(cur_client->m_pCredits != NULL) &&
			(cur_client->m_pCredits->GetCurrentIdentState(cur_client->GetIP()) != IS_IDENTIFIED) )
		{
			dwClientSoft = SO_OLDEMULE;
		}
		dUser.sClientSoft.Format(_T("%u"), dwClientSoft);

		if (cur_client->IsBanned())
			dUser.pcClientExtra = _T("banned");
		else if (cur_client->IsFriend())
			dUser.pcClientExtra = _T("friend");
		else if (cur_client->m_pCredits->HasHigherScoreRatio(cur_client->GetIP()))
			dUser.pcClientExtra = _T("credit");
		else
			dUser.pcClientExtra = _T("none");

		dUser.sUserName = cur_client->GetUserName();
		if(dUser.sUserName.GetLength() > SHORT_LENGTH_MIN)
		{
			dUser.sUserName.Truncate(SHORT_LENGTH_MIN - 3);
			SpecialChars(&dUser.sUserName);
			dUser.sUserName += _T("...");
		}
		else
			SpecialChars(&dUser.sUserName);

		CKnownFile* file = g_App.m_pSharedFilesList->GetFileByID(cur_client->m_reqFileHash);
		if (file != NULL)
		{
			dUser.sFileName = file->GetFileName();
			SpecialChars(&dUser.sFileName);
		}
		else
			dUser.sFileName = _GetPlainResString(IDS_REQ_UNKNOWNFILE);
		dUser.nTransferredDown = cur_client->GetTransferredDown();
		dUser.nTransferredUp = cur_client->GetTransferredUp();
		dUser.dwDataRate = cur_client->GetUpDataRate();
		dUser.sClientNameVersion = cur_client->GetFullSoftVersionString();
		UploadArray.Add(dUser);
	}

	UploadUsers	*pUploadArr = UploadArray.GetData();

	// Sorting (simple bubble sort, we don't have tons of data here)
	bSorted = true;
	for(int nMax = 0; bSorted && nMax < UploadArray.GetCount()*2; nMax++)
	{
		bSorted = false;
		for(int i = 0; i < UploadArray.GetCount() - 1; i++)
		{
			bool bSwap = false;
			switch (m_Params.UploadSort)
			{
			case UP_SORT_CLIENT:
				bSwap = pUploadArr[i].sClientSoft.Compare(pUploadArr[i+1].sClientSoft) < 0;
				break;
			case UP_SORT_USER:
				bSwap = pUploadArr[i].sUserName.CompareNoCase(pUploadArr[i+1].sUserName) < 0;
				break;
			case UP_SORT_VERSION:
				bSwap = pUploadArr[i].sClientNameVersion.CompareNoCase(pUploadArr[i+1].sClientNameVersion) < 0;
				break;
			case UP_SORT_FILENAME:
				bSwap = pUploadArr[i].sFileName.CompareNoCase(pUploadArr[i+1].sFileName) < 0;
				break;
			case UP_SORT_TRANSFERRED:
				bSwap = pUploadArr[i].nTransferredUp < pUploadArr[i+1].nTransferredUp;
				break;
			case UP_SORT_SPEED:
				bSwap = pUploadArr[i].dwDataRate < pUploadArr[i+1].dwDataRate;
				break;
			}
			if (m_Params.bUploadSortReverse)
				bSwap = !bSwap;
			if(bSwap)
			{
				bSorted = true;
				UploadUsers TmpUser = pUploadArr[i];
				pUploadArr[i] = pUploadArr[i+1];
				pUploadArr[i+1] = TmpUser;
			}
		}
	}

	int nCountQueue = 0;
	int nCountQueueBanned = 0;
	int nCountQueueFriend = 0;
	int nCountQueueCredit = 0;
	int nCountQueueSecure = 0;
	int nCountQueueBannedSecure = 0;
	int nCountQueueFriendSecure = 0;
	int nCountQueueCreditSecure = 0;
	int nNextPos = 0;	// position in queue of the user with the highest score -> next upload user
	uint32 dwNextScore = 0;	// highest score -> next upload user

	CQArray<QueueUsers, QueueUsers> QueueArray;
	for (POSITION pos = g_App.m_pUploadQueue->waitinglist.GetHeadPosition(); pos != NULL;)
	{
		CUpDownClient* cur_client = g_App.m_pUploadQueue->waitinglist.GetNext(pos);
		QueueUsers dUser;

		bool bSecure = (cur_client->m_pCredits != NULL) &&
			(cur_client->m_pCredits->GetCurrentIdentState(cur_client->GetIP()) == IS_IDENTIFIED);
		if (cur_client->IsBanned())
		{
			dUser.pcClientState = _T("banned");
			dUser.iClientExtra = WS_WQUETYPE_BANNED;
			nCountQueueBanned++;
			if (bSecure) nCountQueueBannedSecure++;
		}
		else if (cur_client->IsFriend())
		{
			dUser.pcClientState = _T("friend");
			dUser.iClientExtra = WS_WQUETYPE_FRIEND;
			nCountQueueFriend++;
			if (bSecure) nCountQueueFriendSecure++;
		}
		else if (cur_client->m_pCredits->HasHigherScoreRatio(cur_client->GetIP()))
		{
			dUser.pcClientState = _T("credit");
			dUser.iClientExtra = WS_WQUETYPE_CREDIT;
			nCountQueueCredit++;
			if (bSecure) nCountQueueCreditSecure++;
		}
		else
		{
			dUser.pcClientState = _T("none");
			dUser.iClientExtra = WS_WQUETYPE_NONE;
			nCountQueue++;
			if (bSecure) nCountQueueSecure++;
		}

		dUser.sUserName = cur_client->GetUserName();
		if (dUser.sUserName.GetLength() > SHORT_LENGTH_MIN)
		{
			dUser.sUserName.Truncate(SHORT_LENGTH_MIN - 3);
			SpecialChars(&dUser.sUserName);
			dUser.sUserName += _T("...");
		}
		else
			SpecialChars(&dUser.sUserName);
		dUser.sClientNameVersion = cur_client->GetFullSoftVersionString();

		CKnownFile* file = g_App.m_pSharedFilesList->GetFileByID(cur_client->m_reqFileHash);
		if (file != NULL)
		{
			dUser.sFileName = file->GetFileName();
			SpecialChars(&dUser.sFileName);
		}
		else
			dUser.sFileName = _GetPlainResString(IDS_REQ_UNKNOWNFILE);
		dUser.pcClientStateSpecial = _T("connecting");
		if ((dUser.dwScore = cur_client->GetScore()) > dwNextScore)
		{
			nNextPos = QueueArray.GetSize();
			dwNextScore = dUser.dwScore;
		}

		dwClientSoft = cur_client->GetClientSoft();
		if ( ((dwClientSoft == SO_EMULE) || (dwClientSoft == SO_PLUS)) &&
			(cur_client->m_pCredits->GetCurrentIdentState(cur_client->GetIP()) != IS_IDENTIFIED) )
		{
			dwClientSoft = SO_OLDEMULE;
		}
		dUser.sClientSoft.Format(_T("%u"), dwClientSoft);
		dUser.sUserHash = HashToString(cur_client->GetUserHash());
		//SyruS CQArray-Sorting setting sIndex according to param
		switch (m_Params.QueueSort)
		{
		case QU_SORT_CLIENT:
			dUser.sIndex = dUser.sClientSoft;
			break;
		case QU_SORT_USER:
			dUser.sIndex = dUser.sUserName;
			break;
		case QU_SORT_VERSION:
			dUser.sIndex = dUser.sClientNameVersion;
			break;
		case QU_SORT_FILENAME:
			dUser.sIndex = dUser.sFileName;
			break;
		case QU_SORT_SCORE:
			dUser.sIndex.Format(_T("%09u"), dUser.dwScore);
			break;
		default:
			dUser.sIndex.Empty();
		}
		QueueArray.Add(dUser);
	}

	QueueUsers	*pQueueArr = QueueArray.GetData();

	if (g_App.m_pUploadQueue->waitinglist.GetHeadPosition() != 0)
	{
		pQueueArr[nNextPos].pcClientStateSpecial = pQueueArr[nNextPos].pcClientState = _T("next");
	}

	if ((nCountQueue > 0 &&	m_pWSPrefs->bShowUploadQueue) ||
		(nCountQueueBanned > 0 && m_pWSPrefs->bShowUploadQueueBanned) ||
		(nCountQueueFriend > 0 && m_pWSPrefs->bShowUploadQueueFriend) ||
		(nCountQueueCredit > 0 && m_pWSPrefs->bShowUploadQueueCredit))
	{
#ifdef _DEBUG
	DWORD dwStart = ::GetTickCount();
#endif
	QueueArray.QuickSort(m_Params.bQueueSortReverse);
#ifdef _DEBUG
	AddDebugLogLine(_T("WebServer: Waitingqueue with %u elements sorted in %u ms"), QueueArray.GetSize(), ::GetTickCount()-dwStart);
#endif
	}

	// Displaying
	CString	sDownList, HTTPProcessData;
	CString	OutE(m_Templates.sTransferDownLine);
	CString	strFInfo;
	CString	ed2k;
	const TCHAR	*pcDownPrio, *pcIsGetFLC;
	bool	bIsA4AFAutoFile;

	for (int i = 0; i < FilesArray.GetCount(); i++)
	{
		HTTPProcessData = OutE;

		StringToHash(pFileArr[i].sFileHash, abyteFileHash);
		if (pFileArr[i].sFileHash == _ParseURL(Data.sURL,_T("file")))
			HTTPProcessData.Replace(_T("[LastChangedDataset]"), _T("checked"));
		else
			HTTPProcessData.Replace(_T("[LastChangedDataset]"), _T("checked_no"));

		bIsA4AFAutoFile = ( g_App.m_pDownloadQueue->GetA4AFAutoFile() != NULL
			&& md4cmp0(abyteFileHash) != 0
			&& (g_App.m_pDownloadQueue->GetA4AFAutoFile() == g_App.m_pDownloadQueue->GetFileByID(abyteFileHash)) );

		strFInfo = pFileArr[i].sFileInfo;
		strFInfo.Replace(_T("\\"),_T("\\\\"));
		strFInfo.Replace(_T("\n"),_T("\\n"));
		strFInfo.Replace(_T("'"),_T("&#8217;"));

		ed2k = pFileArr[i].sED2kLink;
		ed2k.Replace(_T("'"), _T("&#8217;"));

		if (!pFileArr[i].bIsPreview)
			pcIsGetFLC = _T("");
		else if(pFileArr[i].bIsGetFLC)
			pcIsGetFLC = _T("enabled");
		else
			pcIsGetFLC = _T("disabled");

		if(pFileArr[i].bFileAutoPrio)
			pcDownPrio = _T("Auto");
		else
		{
			switch(pFileArr[i].nFilePrio)
			{
				case PR_LOW:
					pcDownPrio = _T("Low");
					break;
				case PR_NORMAL:
				default:
					pcDownPrio = _T("Normal");
					break;
				case PR_HIGH:
					pcDownPrio = _T("High");
					break;
			}
		}
		HTTPProcessData.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));
		HTTPProcessData.Replace(_T("[finfo]"), strFInfo);
		HTTPProcessData.Replace(_T("[ed2k]"), ed2k);
		HTTPProcessData.Replace(_T("[DownState]"), pFileArr[i].pcFileState);
		HTTPProcessData.Replace( _T("[autoa4af]"),
			(bIsA4AFAutoFile && !pFileArr[i].bIsComplete) ? _T("autoa4af") : _T("") );
		HTTPProcessData.Replace(_T("[isgetflc]"), pcIsGetFLC);
		HTTPProcessData.Replace(_T("[fname]"), pFileArr[i].sFileNameJS);
		HTTPProcessData.Replace(_T("[session]"), sSession);
		HTTPProcessData.Replace(_T("[filehash]"), pFileArr[i].sFileHash);
		HTTPProcessData.Replace(_T("[CatSel]"), sCat);
		HTTPProcessData.Replace(_T("[down-priority]"), pcDownPrio);
		HTTPProcessData.Replace(_T("[FileType]"), pFileArr[i].pcFileType);

		if (bIsA4AFAutoFile && !pFileArr[i].bIsComplete)
			HTTPProcessData.Replace(_T("[FileIsA4AF]"), _T("a4af"));
		else
			HTTPProcessData.Replace(_T("[FileIsA4AF]"), _T("halfnone"));

		if (pFileArr[i].bIsPreview && pFileArr[i].bIsGetFLC)
			HTTPProcessData.Replace(_T("[FileIsGetFLC]"), _T("getflc"));
		else
			HTTPProcessData.Replace(_T("[FileIsGetFLC]"), _T("halfnone"));

		if (!m_pWSPrefs->abDownloadColHidden[0])
		{
			if(pFileArr[i].sFileName.GetLength() > SHORT_LENGTH_MAX)
				HTTPProcessData.Replace(_T("[ShortFileName]"), pFileArr[i].sFileName.Left(SHORT_LENGTH_MAX-3) + _T("..."));
			else
				HTTPProcessData.Replace(_T("[ShortFileName]"), pFileArr[i].sFileName);
		}
		else
			HTTPProcessData.Replace(_T("[ShortFileName]"), _T(""));

		CString sTooltip = pFileArr[i].sFileInfo;
		sTooltip.Replace(_T("\n"), _T("<br />"));
		sTooltip.Replace(_T("'"), _T("&#8217;"));
		HTTPProcessData.Replace(_T("[FileInfo]"), sTooltip);

		qwTotalSize += pFileArr[i].m_qwFileSize;

		if (!m_pWSPrefs->abDownloadColHidden[1])
			HTTPProcessData.Replace(_T("[2]"), CastItoXBytes(pFileArr[i].m_qwFileSize));
		else
			HTTPProcessData.Replace(_T("[2]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[2])
		{
			if(pFileArr[i].m_qwFileTransferred > 0)
			{
				qwTotalTransferred += pFileArr[i].m_qwFileTransferred;
				HTTPProcessData.Replace(_T("[3]"), CastItoXBytes(pFileArr[i].m_qwFileTransferred));
			}
			else
				HTTPProcessData.Replace(_T("[3]"), _T("-"));
		}
		else
			HTTPProcessData.Replace(_T("[3]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[3])
			HTTPProcessData.Replace(_T("[DownloadBar]"), _GetDownloadGraph(Data, abyteFileHash));
		else
			HTTPProcessData.Replace(_T("[DownloadBar]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[4])
		{
			if (pFileArr[i].dwFileSpeed != 0)
			{
				dTmp = static_cast<double>(pFileArr[i].dwFileSpeed) / 1024.0;
				fTotalSpeed += dTmp;
				strTmp.Format(_T("%.2f"), dTmp);
				HTTPProcessData.Replace(_T("[4]"), strTmp);
			}
			else
				HTTPProcessData.Replace(_T("[4]"), _T("-"));
		}
		else
			HTTPProcessData.Replace(_T("[4]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[5])
		{
			if(pFileArr[i].lSourceCount > 0)
			{
				strTmp.Format(_T("%i&nbsp;/&nbsp;%8i&nbsp;(%i)"),
				pFileArr[i].lSourceCount - pFileArr[i].lNotCurrentSourceCount,
				pFileArr[i].lSourceCount,
				pFileArr[i].lTransferringSourceCount);
				HTTPProcessData.Replace(_T("[5]"), strTmp);
			}
			else
				HTTPProcessData.Replace(_T("[5]"), _T("-"));
		}
		else
			HTTPProcessData.Replace(_T("[5]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[6])
		{
			UINT	dwResStrId;

			if(pFileArr[i].bFileAutoPrio)
			{
				switch(pFileArr[i].nFilePrio)
				{
					case PR_LOW:
						dwResStrId = IDS_PRIOAUTOLOW;
						break;
					case PR_NORMAL:
					default:
						dwResStrId = IDS_PRIOAUTONORMAL;
						break;
					case PR_HIGH:
						dwResStrId = IDS_PRIOAUTOHIGH;
						break;
				}
			}
			else
			{
				switch(pFileArr[i].nFilePrio)
				{
					case PR_LOW:
						dwResStrId = IDS_PRIOLOW;
						break;
					case PR_NORMAL:
					default:
						dwResStrId = IDS_PRIONORMAL;
						break;
					case PR_HIGH:
						dwResStrId = IDS_PRIOHIGH;
						break;
				}
			}
			HTTPProcessData.Replace(_T("[PrioVal]"), GetResString(dwResStrId));
		}
		else
			HTTPProcessData.Replace(_T("[PrioVal]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[7])
			HTTPProcessData.Replace(_T("[Category]"), pFileArr[i].sCategory);
		else
			HTTPProcessData.Replace(_T("[Category]"), _T(""));

		if (!m_pWSPrefs->abDownloadColHidden[8])
			HTTPProcessData.Replace(_T("[FakeCheck]"), pFileArr[i].sFakeCheck);
		else
			HTTPProcessData.Replace(_T("[FakeCheck]"), _T(""));

		InsertCatBox(HTTPProcessData, 0, _T(""), false, false, sSession, abyteFileHash);

		sDownList += HTTPProcessData;
	}

	Out.Replace(_T("[DownloadFilesList]"), sDownList);
	Out.Replace(_T("[TotalDownSize]"), CastItoXBytes(qwTotalSize));
	Out.Replace(_T("[TotalDownTransferred]"), CastItoXBytes(qwTotalTransferred));

	strTmp.Format(_T("%.2f"), fTotalSpeed);
	Out.Replace(_T("[TotalDownSpeed]"), strTmp);

	strTmp.Format( _T("%s: %u, %s: %u"), GetResString(IDS_SF_FILE), g_App.m_pDownloadQueue->GetFileCount(),
					 GetResString(IDS_ST_ACTIVE), g_App.m_pDownloadQueue->GetActiveFileCount());
	Out.Replace(_T("[TotalFiles]"), strTmp);

	strTmp.Format(_T("%i"), m_Templates.iProgressbarWidth);
	Out.Replace(_T("[PROGRESSBARWIDTHVAL]"), strTmp);

	qwTotalSize = 0;
	qwTotalTransferred = 0;
	fTotalSpeed = 0;

	CString sUpList;

	OutE = m_Templates.sTransferUpLine;
	OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

	for(int i = 0; i < UploadArray.GetCount(); i++)
	{
		HTTPProcessData = OutE;

		HTTPProcessData.Replace(_T("[UserHash]"), pUploadArr[i].sUserHash);
		HTTPProcessData.Replace(_T("[UpState]"), pUploadArr[i].pcActive);
		HTTPProcessData.Replace(_T("[FileInfo]"), pUploadArr[i].sFileInfo);
		HTTPProcessData.Replace(_T("[ClientState]"), pUploadArr[i].pcClientState);
		HTTPProcessData.Replace(_T("[ClientSoft]"), pUploadArr[i].sClientSoft);
		HTTPProcessData.Replace(_T("[ClientExtra]"), pUploadArr[i].pcClientExtra);

		pcTmp = (!m_pWSPrefs->abUploadColHidden[0]) ? pUploadArr[i].sUserName.GetString() : _T("");
		HTTPProcessData.Replace(_T("[1]"), pcTmp);

		pcTmp = (!m_pWSPrefs->abUploadColHidden[1]) ? pUploadArr[i].sClientNameVersion.GetString() : _T("");
		HTTPProcessData.Replace(_T("[ClientSoftV]"), pcTmp);

		pcTmp = (!m_pWSPrefs->abUploadColHidden[2]) ? pUploadArr[i].sFileName.GetString() : _T("");
		HTTPProcessData.Replace(_T("[2]"), pcTmp);

		pcTmp = _T("");
		if (!m_pWSPrefs->abUploadColHidden[3])
		{
			qwTotalSize += pUploadArr[i].nTransferredDown;
			qwTotalTransferred += pUploadArr[i].nTransferredUp;
			strTmp.Format(_T("%s / %s"), CastItoXBytes(pUploadArr[i].nTransferredDown),CastItoXBytes(pUploadArr[i].nTransferredUp));
			pcTmp = strTmp;
		}
		HTTPProcessData.Replace(_T("[3]"), pcTmp);

		pcTmp = _T("");
		if (!m_pWSPrefs->abUploadColHidden[4])
		{
			dTmp = static_cast<double>(pUploadArr[i].dwDataRate) / 1024.0;
			fTotalSpeed += dTmp;
			strTmp.Format(_T("%.2f"), dTmp);
			pcTmp = strTmp;
		}
		HTTPProcessData.Replace(_T("[4]"), pcTmp);

		sUpList += HTTPProcessData;
	}
	Out.Replace(_T("[UploadFilesList]"), sUpList);
	strTmp.Format(_T("%s / %s"), CastItoXBytes(qwTotalSize), CastItoXBytes(qwTotalTransferred));
	Out.Replace(_T("[TotalUpTransferred]"), strTmp);
	strTmp.Format(_T("%.2f"), fTotalSpeed);
	Out.Replace(_T("[TotalUpSpeed]"), strTmp);

	if (m_pWSPrefs->bShowUploadQueue)
	{
		Out.Replace(_T("[UploadQueue]"), m_Templates.sTransferUpQueueShow);
		Out.Replace(_T("[UploadQueueList]"), _GetPlainResString(IDS_ONQUEUE));

		CString sQueue;

		OutE = m_Templates.sTransferUpQueueLine;
		OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

		for(int i = 0; i < QueueArray.GetCount(); i++)
		{
			TCHAR HTTPTempC[100] = _T("");

			if (pQueueArr[i].iClientExtra == WS_WQUETYPE_NONE)
			{
				HTTPProcessData = OutE;
				pcTmp = (!m_pWSPrefs->abQueueColHidden[0]) ? pQueueArr[i].sUserName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[UserName]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[1]) ? pQueueArr[i].sClientNameVersion.GetString() : _T("");
				HTTPProcessData.Replace(_T("[ClientSoftV]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[2]) ? pQueueArr[i].sFileName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[FileName]"), pcTmp);

				pcTmp = _T("");
				if (!m_pWSPrefs->abQueueColHidden[3])
				{
					_stprintf(HTTPTempC, _T("%u"), pQueueArr[i].dwScore);
					pcTmp = HTTPTempC;
				}
				HTTPProcessData.Replace(_T("[Score]"), pcTmp);
				HTTPProcessData.Replace(_T("[ClientState]"), pQueueArr[i].pcClientState);
				HTTPProcessData.Replace(_T("[ClientStateSpecial]"), pQueueArr[i].pcClientStateSpecial);
				HTTPProcessData.Replace(_T("[ClientSoft]"), pQueueArr[i].sClientSoft);
				HTTPProcessData.Replace(_T("[ClientExtra]"), _T("none"));
				HTTPProcessData.Replace(_T("[UserHash]"), pQueueArr[i].sUserHash);

				sQueue += HTTPProcessData;
			}
		}
		Out.Replace(_T("[QueueList]"), sQueue);
	}
	else
		Out.Replace(_T("[UploadQueue]"), m_Templates.sTransferUpQueueHide);

	if (m_pWSPrefs->bShowUploadQueueBanned)
	{
		Out.Replace(_T("[UploadQueueBanned]"), m_Templates.sTransferUpQueueBannedShow);
		Out.Replace(_T("[UploadQueueBannedList]"), _GetPlainResString(IDS_ONQUEUEBANNED));

		CString sQueueBanned;

		OutE = m_Templates.sTransferUpQueueBannedLine;
		OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

		for(int i = 0; i < QueueArray.GetCount(); i++)
		{
			TCHAR HTTPTempC[100] = _T("");

			if (pQueueArr[i].iClientExtra == WS_WQUETYPE_BANNED)
			{
				HTTPProcessData = OutE;
				pcTmp = (!m_pWSPrefs->abQueueColHidden[0]) ? pQueueArr[i].sUserName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[UserName]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[1]) ? pQueueArr[i].sClientNameVersion.GetString() : _T("");
				HTTPProcessData.Replace(_T("[ClientSoftV]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[2]) ? pQueueArr[i].sFileName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[FileName]"), pcTmp);

				pcTmp = _T("");
				if (!m_pWSPrefs->abQueueColHidden[3])
				{
					_stprintf(HTTPTempC, _T("%u"), pQueueArr[i].dwScore);
					pcTmp = HTTPTempC;
				}
				HTTPProcessData.Replace(_T("[Score]"), pcTmp);

				HTTPProcessData.Replace(_T("[ClientState]"), pQueueArr[i].pcClientState);
				HTTPProcessData.Replace(_T("[ClientStateSpecial]"), pQueueArr[i].pcClientStateSpecial);
				HTTPProcessData.Replace(_T("[ClientSoft]"), pQueueArr[i].sClientSoft);
				HTTPProcessData.Replace(_T("[ClientExtra]"), _T("banned"));
				HTTPProcessData.Replace(_T("[UserHash]"), pQueueArr[i].sUserHash);

				sQueueBanned += HTTPProcessData;
			}
		}
		Out.Replace(_T("[QueueListBanned]"), sQueueBanned);
	}
	else
		Out.Replace(_T("[UploadQueueBanned]"), m_Templates.sTransferUpQueueBannedHide);

	if (m_pWSPrefs->bShowUploadQueueFriend)
	{
		Out.Replace(_T("[UploadQueueFriend]"), m_Templates.sTransferUpQueueFriendShow);
		Out.Replace(_T("[UploadQueueFriendList]"), _GetPlainResString(IDS_ONQUEUEFRIEND));

		CString sQueueFriend;

		OutE = m_Templates.sTransferUpQueueFriendLine;
		OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

		for(int i = 0; i < QueueArray.GetCount(); i++)
		{
			TCHAR HTTPTempC[100] = _T("");

			if (pQueueArr[i].iClientExtra == WS_WQUETYPE_FRIEND)
			{
				HTTPProcessData = OutE;
				pcTmp = (!m_pWSPrefs->abQueueColHidden[0]) ? pQueueArr[i].sUserName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[UserName]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[1]) ? pQueueArr[i].sClientNameVersion.GetString() : _T("");
				HTTPProcessData.Replace(_T("[ClientSoftV]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[2]) ? pQueueArr[i].sFileName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[FileName]"), pcTmp);

				pcTmp = _T("");
				if (!m_pWSPrefs->abQueueColHidden[3])
				{
					_stprintf(HTTPTempC, _T("%u"), pQueueArr[i].dwScore);
					pcTmp = HTTPTempC;
				}
				HTTPProcessData.Replace(_T("[Score]"), pcTmp);

				HTTPProcessData.Replace(_T("[ClientState]"), pQueueArr[i].pcClientState);
				HTTPProcessData.Replace(_T("[ClientStateSpecial]"), pQueueArr[i].pcClientStateSpecial);
				HTTPProcessData.Replace(_T("[ClientSoft]"), pQueueArr[i].sClientSoft);
				HTTPProcessData.Replace(_T("[ClientExtra]"), _T("friend"));
				HTTPProcessData.Replace(_T("[UserHash]"), pQueueArr[i].sUserHash);

				sQueueFriend += HTTPProcessData;
			}
		}
		Out.Replace(_T("[QueueListFriend]"), sQueueFriend);
	}
	else
		Out.Replace(_T("[UploadQueueFriend]"), m_Templates.sTransferUpQueueFriendHide);

	if (m_pWSPrefs->bShowUploadQueueCredit)
	{
		Out.Replace(_T("[UploadQueueCredit]"), m_Templates.sTransferUpQueueCreditShow);
		Out.Replace(_T("[UploadQueueCreditList]"), _GetPlainResString(IDS_ONQUEUECREDIT));

		CString sQueueCredit;

		OutE = m_Templates.sTransferUpQueueCreditLine;
		OutE.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));

		for(int i = 0; i < QueueArray.GetCount(); i++)
		{
			TCHAR HTTPTempC[100] = _T("");

			if (pQueueArr[i].iClientExtra == WS_WQUETYPE_CREDIT)
			{
				HTTPProcessData = OutE;
				pcTmp = (!m_pWSPrefs->abQueueColHidden[0]) ? pQueueArr[i].sUserName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[UserName]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[1]) ? pQueueArr[i].sClientNameVersion.GetString() : _T("");
				HTTPProcessData.Replace(_T("[ClientSoftV]"), pcTmp);

				pcTmp = (!m_pWSPrefs->abQueueColHidden[2]) ? pQueueArr[i].sFileName.GetString() : _T("");
				HTTPProcessData.Replace(_T("[FileName]"), pcTmp);

				pcTmp = _T("");
				if (!m_pWSPrefs->abQueueColHidden[3])
				{
					_stprintf(HTTPTempC, _T("%u"), pQueueArr[i].dwScore);
					pcTmp =  HTTPTempC;
				}
				HTTPProcessData.Replace(_T("[Score]"), pcTmp);

				HTTPProcessData.Replace(_T("[ClientState]"), pQueueArr[i].pcClientState);
				HTTPProcessData.Replace(_T("[ClientStateSpecial]"), pQueueArr[i].pcClientStateSpecial);
				HTTPProcessData.Replace(_T("[ClientSoft]"), pQueueArr[i].sClientSoft);
				HTTPProcessData.Replace(_T("[ClientExtra]"), _T("credit"));
				HTTPProcessData.Replace(_T("[UserHash]"), pQueueArr[i].sUserHash);

				sQueueCredit += HTTPProcessData;
			}
		}
		Out.Replace(_T("[QueueListCredit]"), sQueueCredit);
	}
	else
		Out.Replace(_T("[UploadQueueCredit]"), m_Templates.sTransferUpQueueCreditHide);

	strTmp.Format(_T("%i"), nCountQueue);
	Out.Replace(_T("[CounterQueue]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueBanned);
	Out.Replace(_T("[CounterQueueBanned]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueFriend);
	Out.Replace(_T("[CounterQueueFriend]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueCredit);
	Out.Replace(_T("[CounterQueueCredit]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueSecure);
	Out.Replace(_T("[CounterQueueSecure]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueBannedSecure);
	Out.Replace(_T("[CounterQueueBannedSecure]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueFriendSecure);
	Out.Replace(_T("[CounterQueueFriendSecure]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueCreditSecure);
	Out.Replace(_T("[CounterQueueCreditSecure]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueue+nCountQueueBanned+nCountQueueFriend+nCountQueueCredit);
	Out.Replace(_T("[CounterAll]"), strTmp);
	strTmp.Format(_T("%i"), nCountQueueSecure+nCountQueueBannedSecure+nCountQueueFriendSecure+nCountQueueCreditSecure);
	Out.Replace(_T("[CounterAllSecure]"), strTmp);
	Out.Replace(_T("[CatSel]"), sCat);
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[ShowUploadQueue]"), _GetPlainResString(IDS_VIEWQUEUE));
	Out.Replace(_T("[ShowUploadQueueList]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE, true));
	Out.Replace(_T("[ShowUploadQueueListBanned]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_BANNED, true));
	Out.Replace(_T("[ShowUploadQueueListFriend]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_FRIEND, true));
	Out.Replace(_T("[ShowUploadQueueListCredit]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_CREDIT, true));

	pcTmp = (m_Params.bQueueSortReverse) ? _T("&amp;sortreverse=false") : _T("&amp;sortreverse=true");

	Out.Replace(_T("[SortQClient]"), (m_Params.QueueSort == QU_SORT_CLIENT) ? pcTmp : _T(""));
	Out.Replace(_T("[SortQUser]"), (m_Params.QueueSort == QU_SORT_USER) ? pcTmp : _T(""));
	Out.Replace(_T("[SortQVersion]"), (m_Params.QueueSort == QU_SORT_VERSION) ? pcTmp : _T(""));
	Out.Replace(_T("[SortQFilename]"), (m_Params.QueueSort == QU_SORT_FILENAME) ? pcTmp : _T(""));
	Out.Replace(_T("[SortQScore]"), (m_Params.QueueSort == QU_SORT_SCORE) ? pcTmp : _T(""));

	pcSortIcon = (m_Params.bQueueSortReverse) ? m_Templates.sUpArrow : m_Templates.sDownArrow;

	_GetPlainResString(&strTmp, IDS_QL_USERNAME, true);
	if (!m_pWSPrefs->abQueueColHidden[0])
	{
		Out.Replace(_T("[UserNameTitleI]"), (m_Params.QueueSort == QU_SORT_USER) ? pcSortIcon : _T(""));
		Out.Replace(_T("[UserNameTitle]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[UserNameTitleI]"), _T(""));
		Out.Replace(_T("[UserNameTitle]"), _T(""));
	}
	Out.Replace(_T("[UserNameTitleM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_INFLST_USER_CLIENTVERSION, true);
	if (!m_pWSPrefs->abQueueColHidden[1])
	{
		Out.Replace(_T("[VersionI]"), (m_Params.QueueSort == QU_SORT_VERSION) ? pcSortIcon : _T(""));
		Out.Replace(_T("[Version]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[VersionI]"), _T(""));
		Out.Replace(_T("[Version]"), _T(""));
	}
	Out.Replace(_T("[VersionM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_FILENAME, true);
	if (!m_pWSPrefs->abQueueColHidden[2])
	{
		Out.Replace(_T("[FileNameTitleI]"), (m_Params.QueueSort == QU_SORT_FILENAME) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FileNameTitle]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FileNameTitleI]"), _T(""));
		Out.Replace(_T("[FileNameTitle]"), _T(""));
	}
	Out.Replace(_T("[FileNameTitleM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SCORE, true);
	if (!m_pWSPrefs->abQueueColHidden[3])
	{
		Out.Replace(_T("[ScoreTitleI]"), (m_Params.QueueSort == QU_SORT_SCORE) ? pcSortIcon : _T(""));
		Out.Replace(_T("[ScoreTitle]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[ScoreTitleI]"), _T(""));
		Out.Replace(_T("[ScoreTitle]"), _T(""));
	}
	Out.Replace(_T("[ScoreTitleM]"), strTmp);
	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetSharedFilesList(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	strTmp, sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString	sSession = _ParseURL(Data.sURL, _T("ses"));
	bool	bAdmin = IsSessionAdmin(Data,sSession);
	CString	strSort = _ParseURL(Data.sURL, _T("sort"));

	strTmp = _ParseURL(Data.sURL, _T("sortreverse"));
	if (!strSort.IsEmpty())
	{
		bool	bDirection = false;

		if (strSort == "state")
			m_Params.SharedSort = SHARED_SORT_STATE;
		else if (strSort == "type")
			m_Params.SharedSort = SHARED_SORT_TYPE;
		else if (strSort == "name")
		{
			m_Params.SharedSort = SHARED_SORT_NAME;
			bDirection = true;
		}
		else if (strSort == _T("size"))
			m_Params.SharedSort = SHARED_SORT_SIZE;
		else if (strSort == _T("transferred"))
			m_Params.SharedSort = SHARED_SORT_TRANSFERRED;
		else if (strSort == _T("alltimetransferred"))
			m_Params.SharedSort = SHARED_SORT_ALL_TIME_TRANSFERRED;
		else if (strSort == _T("requests"))
			m_Params.SharedSort = SHARED_SORT_REQUESTS;
		else if (strSort == _T("alltimerequests"))
			m_Params.SharedSort = SHARED_SORT_ALL_TIME_REQUESTS;
		else if (strSort == _T("accepts"))
			m_Params.SharedSort = SHARED_SORT_ACCEPTS;
		else if (strSort == _T("alltimeaccepts"))
			m_Params.SharedSort = SHARED_SORT_ALL_TIME_ACCEPTS;
		else if (strSort == _T("completes"))
			m_Params.SharedSort = SHARED_SORT_COMPLETES;
		else if (strSort == _T("priority"))
			m_Params.SharedSort = SHARED_SORT_PRIORITY;

		if (strTmp.IsEmpty())
			m_Params.bSharedSortReverse = bDirection;
	}
	if (!strTmp.IsEmpty())
		m_Params.bSharedSortReverse = (strTmp == _T("true"));

	if (!_ParseURL(Data.sURL, _T("hash")).IsEmpty() && !_ParseURL(Data.sURL, _T("prio")).IsEmpty() && IsSessionAdmin(Data,sSession))
	{
		byte	abyteFileHash[16];

		if (md4cmp0(StringToHash(_ParseURL(Data.sURL, _T("hash")), abyteFileHash)) != 0)
		{
			CKnownFile	*pCurFile;

			if ((pCurFile = g_App.m_pSharedFilesList->GetFileByID(abyteFileHash)) != NULL)
			{
				pCurFile->SetAutoULPriority(false);
				strTmp = _ParseURL(Data.sURL, _T("prio"));
				if (strTmp == _T("verylow"))
					pCurFile->SetULPriority(PR_VERYLOW);
				else if (strTmp == _T("low"))
					pCurFile->SetULPriority(PR_LOW);
				else if (strTmp == _T("normal"))
					pCurFile->SetULPriority(PR_NORMAL);
				else if (strTmp == _T("high"))
					pCurFile->SetULPriority(PR_HIGH);
				else if (strTmp == _T("release"))
					pCurFile->SetULPriority(PR_RELEASE);
				else if (strTmp == _T("auto"))
				{
					pCurFile->SetAutoULPriority(true);
					pCurFile->UpdateUploadAutoPriority();
				}

				g_App.m_pSharedFilesList->UpdateItem(pCurFile);	// Refresh GUI on priority change
			}
		}
	}

	CString sCmd = _ParseURL(Data.sURL, _T("c"));
	if (sCmd.CompareNoCase(_T("menu")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abSharedColHidden, ARRSIZE(m_pWSPrefs->abSharedColHidden));

	if (bAdmin)
	{
		strTmp = _ParseURL(Data.sURL, _T("jumpstart"));
		if (strTmp == _T("true"))
			_SetSharedFileJumpstart(_ParseURL(Data.sURL, _T("hash")));
		else if (strTmp == _T("false"))
			_SetSharedFileNoJumpstart(_ParseURL(Data.sURL, _T("hash")));
	}
	if(_ParseURL(Data.sURL, _T("reload")) == _T("true"))
		g_App.m_pMDlg->SendMessage(WEB_SHARED_FILES_RELOAD);

	CString Out(m_Templates.sSharedList);
	const TCHAR *pcTmp = (m_Params.bSharedSortReverse) ? _T("&amp;sortreverse=false") : _T("&amp;sortreverse=true");

	Out.Replace(_T("[SortState]"), (m_Params.SharedSort == SHARED_SORT_STATE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortType]"), (m_Params.SharedSort == SHARED_SORT_TYPE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortName]"), (m_Params.SharedSort == SHARED_SORT_NAME) ? pcTmp : _T(""));
	Out.Replace(_T("[SortSize]"), (m_Params.SharedSort == SHARED_SORT_SIZE) ? pcTmp : _T(""));
	Out.Replace(_T("[SortCompletes]"), (m_Params.SharedSort == SHARED_SORT_COMPLETES) ? pcTmp : _T(""));
	Out.Replace(_T("[SortPriority]"), (m_Params.SharedSort == SHARED_SORT_PRIORITY) ? pcTmp : _T(""));
//	Transferred sorting link
	pcTmp = _T("transferred&amp;sortreverse=false");
	if (m_Params.SharedSort == SHARED_SORT_TRANSFERRED)
	{
		pcTmp = (m_Params.bSharedSortReverse) ?
			_T("alltimetransferred&amp;sortreverse=false") : _T("transferred&amp;sortreverse=true");
	}
	else if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_TRANSFERRED)
	{
		if (!m_Params.bSharedSortReverse)
			pcTmp = _T("alltimetransferred&amp;sortreverse=true");
	}
	Out.Replace(_T("[SortTransferred]"), pcTmp);
//	Request sorting link
	pcTmp = _T("requests&amp;sortreverse=false");
	if (m_Params.SharedSort == SHARED_SORT_REQUESTS)
	{
		pcTmp = (m_Params.bSharedSortReverse) ?
			_T("alltimerequests&amp;sortreverse=false") : _T("requests&amp;sortreverse=true");
	}
	else if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_REQUESTS)
	{
		if (!m_Params.bSharedSortReverse)
			pcTmp = _T("alltimerequests&amp;sortreverse=true");
	}
	Out.Replace(_T("[SortRequests]"), pcTmp);
//	Accepts sorting link
	pcTmp = _T("accepts&amp;sortreverse=false");
	if (m_Params.SharedSort == SHARED_SORT_ACCEPTS)
	{
		pcTmp = (m_Params.bSharedSortReverse) ?
			_T("alltimeaccepts&amp;sortreverse=false") : _T("accepts&amp;sortreverse=true");
	}
	else if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_ACCEPTS)
	{
		if (!m_Params.bSharedSortReverse)
			pcTmp = _T("alltimeaccepts&amp;sortreverse=true");
	}
	Out.Replace(_T("[SortAccepts]"), pcTmp);

	if(_ParseURL(Data.sURL, _T("reload")) == "true")
	{
		CString strResLog = _SpecialChars(g_App.m_pMDlg->m_strLogText);	//Pick-up last line of the log
		strResLog.TrimRight('\n');
		int iStringIndex = strResLog.ReverseFind('\n');
		if (iStringIndex >= 0)
			strResLog = strResLog.Mid(iStringIndex);
		Out.Replace(_T("[Message]"), strResLog);
	}
	else
		Out.Replace(_T("[Message]"), _T(""));

	const TCHAR *pcSortIcon = (m_Params.bSharedSortReverse) ? m_Templates.sUpArrow : m_Templates.sDownArrow;
	const TCHAR *pcIconTmp;

	_GetPlainResString(&strTmp, IDS_DL_FILENAME, true);
	if (!m_pWSPrefs->abSharedColHidden[0])
	{
		Out.Replace(_T("[FilenameI]"), (m_Params.SharedSort == SHARED_SORT_NAME) ? pcSortIcon : _T(""));
		Out.Replace(_T("[Filename]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FilenameI]"), _T(""));
		Out.Replace(_T("[Filename]"), _T(""));
	}
	Out.Replace(_T("[FilenameM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SF_TRANSFERRED, true);
	if (!m_pWSPrefs->abSharedColHidden[1])
	{
		pcIconTmp = (m_Params.SharedSort == SHARED_SORT_TRANSFERRED) ? pcSortIcon : _T("");
		if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_TRANSFERRED)
			pcIconTmp = (m_Params.bSharedSortReverse) ? m_Templates.strUpDoubleArrow : m_Templates.strDownDoubleArrow;
		Out.Replace(_T("[FileTransferredI]"), pcIconTmp);
		Out.Replace(_T("[FileTransferred]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FileTransferredI]"), _T(""));
		Out.Replace(_T("[FileTransferred]"),  _T(""));
	}
	Out.Replace(_T("[FileTransferredM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SF_REQUESTS, true);
	if (!m_pWSPrefs->abSharedColHidden[2])
	{
		pcIconTmp = (m_Params.SharedSort == SHARED_SORT_REQUESTS) ? pcSortIcon : _T("");
		if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_REQUESTS)
			pcIconTmp = (m_Params.bSharedSortReverse) ? m_Templates.strUpDoubleArrow : m_Templates.strDownDoubleArrow;
		Out.Replace(_T("[FileRequestsI]"), pcIconTmp);
		Out.Replace(_T("[FileRequests]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FileRequestsI]"), _T(""));
		Out.Replace(_T("[FileRequests]"),  _T(""));
	}
	Out.Replace(_T("[FileRequestsM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SF_ACCEPTS, true);
	if (!m_pWSPrefs->abSharedColHidden[3])
	{
		pcIconTmp = (m_Params.SharedSort == SHARED_SORT_ACCEPTS) ? pcSortIcon : _T("");
		if (m_Params.SharedSort == SHARED_SORT_ALL_TIME_ACCEPTS)
			pcIconTmp = (m_Params.bSharedSortReverse) ? m_Templates.strUpDoubleArrow : m_Templates.strDownDoubleArrow;
		Out.Replace(_T("[FileAcceptsI]"), pcIconTmp);
		Out.Replace(_T("[FileAccepts]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FileAcceptsI]"), _T(""));
		Out.Replace(_T("[FileAccepts]"),  _T(""));
	}
	Out.Replace(_T("[FileAcceptsM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SIZE, true);
	if (!m_pWSPrefs->abSharedColHidden[4])
	{
		Out.Replace(_T("[SizeI]"), (m_Params.SharedSort == SHARED_SORT_SIZE) ? pcSortIcon : _T(""));
		Out.Replace(_T("[Size]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[SizeI]"), _T(""));
		Out.Replace(_T("[Size]"), _T(""));
	}
	Out.Replace(_T("[SizeM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_SF_COMPLETESRC, true);
	if (!m_pWSPrefs->abSharedColHidden[5])
	{
		Out.Replace(_T("[CompletesI]"), (m_Params.SharedSort == SHARED_SORT_COMPLETES) ? pcSortIcon : _T(""));
		Out.Replace(_T("[Completes]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[CompletesI]"), _T(""));
		Out.Replace(_T("[Completes]"),  _T(""));
	}
	Out.Replace(_T("[CompletesM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_PRIORITY, true);
	if (!m_pWSPrefs->abSharedColHidden[6])
	{
		Out.Replace(_T("[PriorityI]"), (m_Params.SharedSort == SHARED_SORT_PRIORITY) ? pcSortIcon : _T(""));
		Out.Replace(_T("[Priority]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[PriorityI]"), _T(""));
		Out.Replace(_T("[Priority]"),  _T(""));
	}
	Out.Replace(_T("[PriorityM]"), strTmp);

	Out.Replace(_T("[Actions]"), _GetPlainResString(IDS_WEB_ACTIONS));
	Out.Replace(_T("[Reload]"), _GetPlainResString(IDS_SF_RELOAD));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[SharedList]"), _GetPlainResString(IDS_SHAREDFILES));
	Out.Replace(_T("[CatSel]"), sCat);

	CString OutE(m_Templates.sSharedLine);
	CArray<SharedFiles, SharedFiles> SharedArray;

	// Populating array
	for (POSITION pos = g_App.m_pSharedFilesList->m_mapSharedFiles.GetStartPosition(); pos != NULL;)
	{
		CCKey bufKey;
		CKnownFile* cur_file;
		uint16 nCountLo, nCountHi;

		g_App.m_pSharedFilesList->m_mapSharedFiles.GetNextAssoc(pos,bufKey,cur_file);

		bool bPartFile = cur_file->IsPartFile();

		SharedFiles dFile;

		dFile.bIsPartFile = cur_file->IsPartFile();
		dFile.sFileName = cur_file->GetFileName();
		if (bPartFile)
			dFile.pcFileState = _T("filedown");
		else
			dFile.pcFileState = _T("file");
		dFile.pcFileType = GetFileTypeForWebServer(dFile.sFileName);
		dFile.m_qwFileSize = cur_file->GetFileSize();
		dFile.sED2kLink = cur_file->CreateED2kLink();
		dFile.nFileTransferred = cur_file->statistic.GetTransferred();
		dFile.nFileAllTimeTransferred = cur_file->statistic.GetAllTimeTransferred();
		dFile.nFileRequests = cur_file->statistic.GetRequests();
		dFile.nFileAllTimeRequests = cur_file->statistic.GetAllTimeRequests();
		dFile.nFileAccepts = cur_file->statistic.GetAccepts();
		dFile.nFileAllTimeAccepts = cur_file->statistic.GetAllTimeAccepts();
		dFile.sFileHash = HashToString(cur_file->GetFileHash());

		if (bPartFile)
		{
			((CPartFile*)cur_file)->GetCompleteSourcesRange(&nCountLo, &nCountHi);
			dFile.sFileCompletes.Format(_T("%u"), ((CPartFile*)cur_file)->GetCompleteSourcesCount());
		}
		else
		{
			cur_file->GetCompleteSourcesRange(&nCountLo, &nCountHi);
			if (nCountLo == 0)
			{
				if (nCountHi == 0)
					dFile.sFileCompletes.Empty();
				else
					dFile.sFileCompletes.Format(_T("< %u"), nCountHi);
			}
			else if (nCountLo == nCountHi)
				dFile.sFileCompletes.Format(_T("%u"), nCountLo);
			else
				dFile.sFileCompletes.Format(_T("%u - %u"), nCountLo, nCountHi);
		}
		dFile.dblFileCompletes = ((nCountLo == 0) ? ((nCountHi == 0) ? ((bPartFile) ? 2.0 : 0.0) : (2.0 - (1.0 / static_cast<double>(nCountHi)))) : static_cast<double>(nCountLo) + 3.0 - (1.0 / static_cast<double>(nCountHi)));
		dFile.sFilePriority = GetKnownFilePriorityString(cur_file);
		dFile.nFilePriority = cur_file->GetULPriority();
		dFile.bFileAutoPriority = cur_file->IsULAutoPrioritized();
		SharedArray.Add(dFile);
	}

	// Sorting (simple bubble sort, we don't have tons of data here)
	bool bSorted = true;

	for(int nMax = 0; bSorted && nMax < SharedArray.GetCount()*2; nMax++)
	{
		bSorted = false;
		for(int i = 0; i < SharedArray.GetCount() - 1; i++)
		{
			bool bSwap = false;
			switch(m_Params.SharedSort)
			{
			case SHARED_SORT_STATE:
				bSwap = _tcscmp(SharedArray[i].pcFileState, SharedArray[i+1].pcFileState) > 0;
				break;
			case SHARED_SORT_TYPE:
				bSwap = _tcscmp(SharedArray[i].pcFileType, SharedArray[i+1].pcFileType) > 0;
				break;
			case SHARED_SORT_NAME:
				bSwap = SharedArray[i].sFileName.CompareNoCase(SharedArray[i+1].sFileName) < 0;
				break;
			case SHARED_SORT_SIZE:
				bSwap = SharedArray[i].m_qwFileSize < SharedArray[i+1].m_qwFileSize;
				break;
			case SHARED_SORT_TRANSFERRED:
				bSwap = SharedArray[i].nFileTransferred < SharedArray[i+1].nFileTransferred;
				break;
			case SHARED_SORT_ALL_TIME_TRANSFERRED:
				bSwap = SharedArray[i].nFileAllTimeTransferred < SharedArray[i+1].nFileAllTimeTransferred;
				break;
			case SHARED_SORT_REQUESTS:
				bSwap = SharedArray[i].nFileRequests < SharedArray[i+1].nFileRequests;
				break;
			case SHARED_SORT_ALL_TIME_REQUESTS:
				bSwap = SharedArray[i].nFileAllTimeRequests < SharedArray[i+1].nFileAllTimeRequests;
				break;
			case SHARED_SORT_ACCEPTS:
				bSwap = SharedArray[i].nFileAccepts < SharedArray[i+1].nFileAccepts;
				break;
			case SHARED_SORT_ALL_TIME_ACCEPTS:
				bSwap = SharedArray[i].nFileAllTimeAccepts < SharedArray[i+1].nFileAllTimeAccepts;
				break;
			case SHARED_SORT_COMPLETES:
				bSwap = SharedArray[i].dblFileCompletes < SharedArray[i+1].dblFileCompletes;
				break;
			case SHARED_SORT_PRIORITY:
				//Very low priority is define equal to 4 ! Must adapte sorting code
				if(SharedArray[i].nFilePriority == 4)
				{
					if(SharedArray[i+1].nFilePriority == 4)
						bSwap = false;
					else
						bSwap = true;
				}
				else if(SharedArray[i+1].nFilePriority == 4)
				{
						if(SharedArray[i].nFilePriority == 4)
							bSwap = true;
						else
							bSwap = false;
				}
				else
					bSwap = SharedArray[i].nFilePriority < SharedArray[i+1].nFilePriority;
				break;
			}
			if (m_Params.bSharedSortReverse)
				bSwap = !bSwap;
			if(bSwap)
			{
				bSorted = true;
				SharedFiles TmpFile = SharedArray[i];
				SharedArray[i] = SharedArray[i+1];
				SharedArray[i+1] = TmpFile;
			}
		}
	}
	// Displaying
	CString ed2k, fname, sSharedList, HTTPProcessData;

	for(int i = 0; i < SharedArray.GetCount(); i++)
	{
		TCHAR HTTPTempC[100] = _T("");
		HTTPProcessData = OutE;

		if (SharedArray[i].sFileHash == _ParseURL(Data.sURL, _T("hash")))
			HTTPProcessData.Replace(_T("[LastChangedDataset]"), _T("checked"));
		else
			HTTPProcessData.Replace(_T("[LastChangedDataset]"), _T("checked_no"));

		const TCHAR	*pcSharedPrio, *pcIsJumpStart = _T("");

		switch (SharedArray[i].nFilePriority)
		{
			case PR_VERYLOW:
				pcSharedPrio = _T("VeryLow");
				break;
			case PR_LOW:
				pcSharedPrio = _T("Low");
				break;
			default:
			case PR_NORMAL:
				pcSharedPrio = _T("Normal");
				break;
			case PR_HIGH:
				pcSharedPrio = _T("High");
				break;
			case PR_RELEASE:
				pcSharedPrio = _T("Release");
				break;
		}
		if (SharedArray[i].bFileAutoPriority)
			pcSharedPrio = _T("Auto");

		ed2k = SharedArray[i].sED2kLink;
		ed2k.Replace(_T("'"),_T("&#8217;"));
		fname = SharedArray[i].sFileName;
		fname.Replace(_T("'"),_T("&#8217;"));

#ifdef OLD_SOCKETS_ENABLED
		byte	abyteFileHash[16];

		if (md4cmp0(StringToHash(SharedArray[i].sFileHash, abyteFileHash)) != 0)
		{
			CKnownFile	*pCurFile;

			HTTPProcessData.Replace(_T("[hash]"), SharedArray[i].sFileHash);
			if ((pCurFile = g_App.m_pSharedFilesList->GetFileByID(abyteFileHash)) != NULL)
			{
 				if (pCurFile->GetJumpstartEnabled())
				{
					pcIsJumpStart = _T("jumpstart");
					HTTPProcessData.Replace(_T("[FileIsPriority]"), _T("jumpstart"));
				}
				else if (pCurFile->GetULPriority() == PR_RELEASE)
					HTTPProcessData.Replace(_T("[FileIsPriority]"), _T("release"));
				else
					HTTPProcessData.Replace(_T("[FileIsPriority]"), _T("none"));
			}
		}
#endif //OLD_SOCKETS_ENABLED

		HTTPProcessData.Replace(_T("[admin]"), (bAdmin) ? _T("admin") : _T(""));
		HTTPProcessData.Replace(_T("[ed2k]"), ed2k);
		HTTPProcessData.Replace(_T("[fname]"), fname);
		HTTPProcessData.Replace(_T("[session]"), sSession);
		HTTPProcessData.Replace(_T("[shared-priority]"), pcSharedPrio);
		HTTPProcessData.Replace(_T("[isjumpstart]"), pcIsJumpStart);

		HTTPProcessData.Replace(_T("[FileName]"), _SpecialChars(SharedArray[i].sFileName));
		HTTPProcessData.Replace(_T("[FileType]"), SharedArray[i].pcFileType);
		HTTPProcessData.Replace(_T("[FileState]"), SharedArray[i].pcFileState);
		if (!m_pWSPrefs->abSharedColHidden[0])
		{
			if(SharedArray[i].sFileName.GetLength() > SHORT_LENGTH)
				HTTPProcessData.Replace(_T("[ShortFileName]"), _SpecialChars(SharedArray[i].sFileName.Left(SHORT_LENGTH-3)) + _T("..."));
			else
				HTTPProcessData.Replace(_T("[ShortFileName]"), _SpecialChars(SharedArray[i].sFileName));
		}
		else
			HTTPProcessData.Replace(_T("[ShortFileName]"), _T(""));
		if (!m_pWSPrefs->abSharedColHidden[1])
		{
			_stprintf(HTTPTempC, _T("%s"),CastItoXBytes(SharedArray[i].nFileTransferred));
			HTTPProcessData.Replace(_T("[FileTransferred]"), CString(HTTPTempC));
			_stprintf(HTTPTempC, _T("%s"),CastItoXBytes(SharedArray[i].nFileAllTimeTransferred));
			HTTPProcessData.Replace(_T("[FileAllTimeTransferred]"), _T(" (") + CString(HTTPTempC) + _T(")"));
		}
		else
		{
			HTTPProcessData.Replace(_T("[FileTransferred]"), _T(""));
			HTTPProcessData.Replace(_T("[FileAllTimeTransferred]"), _T(""));
		}
		if (!m_pWSPrefs->abSharedColHidden[2])
		{
			_stprintf(HTTPTempC, _T("%i"), SharedArray[i].nFileRequests);
			HTTPProcessData.Replace(_T("[FileRequests]"), CString(HTTPTempC));
			_stprintf(HTTPTempC, _T("%i"), SharedArray[i].nFileAllTimeRequests);
			HTTPProcessData.Replace(_T("[FileAllTimeRequests]"), _T(" (") + CString(HTTPTempC) + _T(")"));
		}
		else
		{
			HTTPProcessData.Replace(_T("[FileRequests]"), _T(""));
			HTTPProcessData.Replace(_T("[FileAllTimeRequests]"), _T(""));
		}
		if (!m_pWSPrefs->abSharedColHidden[3])
		{
			_stprintf(HTTPTempC, _T("%i"), SharedArray[i].nFileAccepts);
			HTTPProcessData.Replace(_T("[FileAccepts]"), CString(HTTPTempC));
			_stprintf(HTTPTempC, _T("%i"), SharedArray[i].nFileAllTimeAccepts);
			HTTPProcessData.Replace(_T("[FileAllTimeAccepts]"), _T(" (") + CString(HTTPTempC) + _T(")"));
		}
		else
		{
			HTTPProcessData.Replace(_T("[FileAccepts]"), _T(""));
			HTTPProcessData.Replace(_T("[FileAllTimeAccepts]"), _T(""));
		}
		if (!m_pWSPrefs->abSharedColHidden[4])
		{
			_stprintf(HTTPTempC, _T("%s"),CastItoXBytes(SharedArray[i].m_qwFileSize));
			HTTPProcessData.Replace(_T("[FileSize]"), CString(HTTPTempC));
		}
		else
			HTTPProcessData.Replace(_T("[FileSize]"), _T(""));
		if (!m_pWSPrefs->abSharedColHidden[5])
			HTTPProcessData.Replace(_T("[Completes]"), SharedArray[i].sFileCompletes);
		else
			HTTPProcessData.Replace(_T("[Completes]"), _T(""));
		if (!m_pWSPrefs->abSharedColHidden[6])
			HTTPProcessData.Replace(_T("[Priority]"), SharedArray[i].sFilePriority);
		else
			HTTPProcessData.Replace(_T("[Priority]"), _T(""));

		HTTPProcessData.Replace(_T("[FileHash]"), SharedArray[i].sFileHash);

		sSharedList += HTTPProcessData;
	}
	Out.Replace(_T("[SharedFilesList]"), sSharedList);
	Out.Replace(_T("[Session]"), sSession);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetGraphs(const ThreadData &Data)
{
	NOPRM(Data);
	EMULE_TRY

	CString Out(m_Templates.sGraphs);
	CString strGraphDownload, strGraphUpload, strGraphCons;

	unsigned	uiLmt = (PointsForWeb.GetCount() < WEB_GRAPH_WIDTH) ? PointsForWeb.GetCount() : WEB_GRAPH_WIDTH;
	
	if (uiLmt != 0)
	{
		strGraphDownload.Format(_T("%u"), PointsForWeb[0].dwDownRate);
		strGraphUpload.Format(_T("%u"), PointsForWeb[0].dwUpRate);
		strGraphCons.Format(_T("%u"), PointsForWeb[0].dwConnections);
		for (unsigned ui = 1; ui < uiLmt; ui++)
		{
			strGraphDownload.AppendFormat(_T(",%u"), PointsForWeb[ui].dwDownRate);
			strGraphUpload.AppendFormat(_T(",%u"), PointsForWeb[ui].dwUpRate);
			strGraphCons.AppendFormat(_T(",%u"), PointsForWeb[ui].dwConnections);
		}
	}

	Out.Replace(_T("[GraphDownload]"), strGraphDownload);
	Out.Replace(_T("[GraphUpload]"), strGraphUpload);
	Out.Replace(_T("[GraphConnections]"), strGraphCons);

	Out.Replace(_T("[TxtDownload]"), _GetPlainResString(IDS_ST_ACTIVEDOWNLOAD));
	Out.Replace(_T("[TxtUpload]"), _GetPlainResString(IDS_ST_ACTIVEUPLOAD));
	Out.Replace(_T("[TxtTime]"), _GetPlainResString(IDS_TIME));
	Out.Replace(_T("[KByteSec]"), _GetPlainResString(IDS_KBYTESEC));
	Out.Replace(_T("[TxtConnections]"), _GetPlainResString(IDS_ST_ACTIVECONNECTIONS));

	Out.Replace(_T("[ScaleTime]"), CastSecondsToHM(g_App.m_pPrefs->GetTrafficOMeterInterval() * WEB_GRAPH_WIDTH));

	CString s1;
	s1.Format(_T("%u"), g_App.m_pPrefs->GetMaxGraphDownloadRate() / 10);
	Out.Replace(_T("[MaxDownload]"), s1);
	s1.Format(_T("%u"), g_App.m_pPrefs->GetMaxGraphUploadRate() / 10);
	Out.Replace(_T("[MaxUpload]"), s1);
	s1.Format(_T("%u"), g_App.m_pPrefs->GetMaxConnections());
	Out.Replace(_T("[MaxConnections]"), s1);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetAddServerBox(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));

	if (!IsSessionAdmin(Data, sSession))
		return _T("");

	CString	strTmp, Out(m_Templates.sAddServerBox);

	if (_ParseURL(Data.sURL, _T("addserver")) == _T("true"))
	{
		CString	strSrvAddr(_ParseURL(Data.sURL, _T("serveraddr")));
		CString	strSrvPort(_ParseURL(Data.sURL, _T("serverport")));

		if (!strSrvAddr.IsEmpty() && !strSrvPort.IsEmpty())
		{
			g_App.m_pMDlg->m_wndServer.AddServer(strSrvAddr, strSrvPort, _ParseURL(Data.sURL, _T("servername")));

			CString	strResLog = _SpecialChars(g_App.m_pMDlg->m_strLogText); //Pick-up last line of the log
			uint16	uSrvPort = static_cast<uint16>(_tstoi(strSrvPort));
			CServer	*server = g_App.m_pServerList->GetServerByAddress(strSrvAddr, uSrvPort);

			strTmp = _ParseURL(Data.sURL, _T("priority"));
			if (strTmp == _T("low"))
				server->SetPreference(PR_LOW);
			else if (strTmp == _T("normal"))
				server->SetPreference(PR_NORMAL);
			else if (strTmp == _T("high"))
				server->SetPreference(PR_NORMAL);
			if (g_App.m_pMDlg->m_wndServer)
				g_App.m_pMDlg->m_wndServer.m_ctlServerList.RefreshServer(*server);

			if(_ParseURL(Data.sURL, _T("addtostatic")) == _T("true"))
			{
				_AddToStatic(strSrvAddr, uSrvPort);
				strResLog += _T("<br />");
				strResLog += _SpecialChars(g_App.m_pMDlg->m_strLogText); //Pick-up last line of the log
			}
			strResLog.TrimRight(_T('\n'));
			strResLog = strResLog.Mid(strResLog.ReverseFind(_T('\n')));
			Out.Replace(_T("[Message]"), strResLog);
			if (_ParseURL(Data.sURL, _T("connectnow")) == _T("true"))
				_ConnectToServer(strSrvAddr, uSrvPort);
		}
		else
			Out.Replace(_T("[Message]"), _GetPlainResString(IDS_INVALIDSA));
	}
	else if(_ParseURL(Data.sURL, _T("updateservermetfromurl")) == _T("true"))
	{
		g_App.m_pMDlg->m_wndServer.UpdateServerMetFromURL(_ParseURL(Data.sURL, _T("servermeturl")));
		strTmp = _SpecialChars(g_App.m_pMDlg->m_strLogText);
		strTmp.TrimRight(_T('\n'));
		strTmp = strTmp.Mid(strTmp.ReverseFind(_T('\n')));
		Out.Replace(_T("[Message]"), strTmp);
	}
	else
		Out.Replace(_T("[Message]"), _T(""));
	Out.Replace(_T("[AddServer]"), _GetPlainResString(IDS_SV_NEWSERVER));
	Out.Replace(_T("[IP]"), _GetPlainResString(IDS_SV_ADDRESS));
	Out.Replace(_T("[Port]"), _GetPlainResString(IDS_PORT));
	Out.Replace(_T("[Name]"), _GetPlainResString(IDS_SW_NAME));
	Out.Replace(_T("[Static]"), _GetPlainResString(IDS_STATICSERVER));
	Out.Replace(_T("[ConnectNow]"), _GetPlainResString(IDS_CONNECTNOW));
	Out.Replace(_T("[Priority]"), _GetPlainResString(IDS_PRIORITY));
	Out.Replace(_T("[Low]"), _GetPlainResString(IDS_PRIOLOW));
	Out.Replace(_T("[Normal]"), _GetPlainResString(IDS_PRIONORMAL));
	Out.Replace(_T("[High]"), _GetPlainResString(IDS_PRIOHIGH));
	Out.Replace(_T("[Add]"), _GetPlainResString(IDS_SV_ADD));
	Out.Replace(_T("[UpdateServerMetFromURL]"), _GetPlainResString(IDS_SV_MET));
	Out.Replace(_T("[URL]"), _GetPlainResString(IDS_SV_URL));
	Out.Replace(_T("[Apply]"), _GetPlainResString(IDS_PW_APPLY));

	strTmp.Format(_T("/?ses=%s&amp;w=server&amp;c=disconnect&amp;cat=%s"), sSession, sCat);
	Out.Replace(_T("[URL_Disconnect]"), strTmp);
	strTmp.Format(_T("/?ses=%s&amp;w=server&amp;c=connect&amp;cat=%s"), sSession, sCat);
	Out.Replace(_T("[URL_Connect]"), strTmp);

	Out.Replace(_T("[Disconnect]"), _GetPlainResString(IDS_IRC_DISCONNECT));
	Out.Replace(_T("[Connect]"), _GetPlainResString(IDS_CONNECTTOANYSERVER));
	Out.Replace(_T("[ServerOptions]"), CString(_GetPlainResString(IDS_FSTAT_CONNECTION)));
	Out.Replace(_T("[Execute]"), _GetPlainResString(IDS_IRC_PERFORM));
	Out.Replace(_T("[CatSel]"), sCat);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetLog(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));
	CString Out(m_Templates.sLog);

	if ((_ParseURL(Data.sURL, _T("clear")) == _T("yes")) && IsSessionAdmin(Data,sSession))
	{
		g_App.m_pMDlg->m_wndServer.m_pctlLogBox->Reset();
		LRESULT pDummy;
		g_App.m_pMDlg->m_wndServer.OnTcnSelchangeTab1(NULL, &pDummy);
	}
	Out.Replace(_T("[Clear]"), _GetPlainResString(IDS_PW_RESET));
	Out.Replace(_T("[Log]"), g_App.m_pMDlg->m_wndServer.m_pctlLogBox->GetHtml() + _T("<br /><a name=\"end\"></a>"));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[CatSel]"), sCat);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetServerInfo(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));
	CString Out(m_Templates.sServerInfo);

	if ((_ParseURL(Data.sURL, _T("clear")) == _T("yes")) && IsSessionAdmin(Data,sSession))
	{
		g_App.m_pMDlg->m_wndServer.m_pctlServerMsgBox->Reset();
		LRESULT pDummy;
		g_App.m_pMDlg->m_wndServer.OnTcnSelchangeTab1(NULL, &pDummy);
	}
	Out.Replace(_T("[Clear]"), _GetPlainResString(IDS_PW_RESET));
	Out.Replace(_T("[ServerInfo]"), g_App.m_pMDlg->m_wndServer.m_pctlServerMsgBox->GetHtml() + _T("<br /><a name=\"end\"></a>"));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[CatSel]"), sCat);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetDebugLog(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));
	CString Out(m_Templates.sDebugLog);

	if ((_ParseURL(Data.sURL, _T("clear")) == _T("yes")) && IsSessionAdmin(Data,sSession))
	{
		g_App.m_pMDlg->m_wndServer.m_pctlDebugBox->Reset();
		LRESULT pDummy;
		g_App.m_pMDlg->m_wndServer.OnTcnSelchangeTab1(NULL, &pDummy);
	}
	Out.Replace(_T("[Clear]"), _GetPlainResString(IDS_PW_RESET));
	Out.Replace(_T("[DebugLog]"), g_App.m_pMDlg->m_wndServer.m_pctlDebugBox->GetHtml() + _T("<br /><a name=\"end\"></a>"));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[CatSel]"), sCat);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::_GetStats(const ThreadData &Data)
{
	EMULE_TRY

	CWebServer *pThis = (CWebServer *)Data.pThis;
	if(pThis == NULL)
		return _T("");

	CString sSession = _ParseURL(Data.sURL, _T("ses"));

	// refresh statistics
	g_App.m_pMDlg->ShowStatistics();
	g_App.m_pMDlg->m_dlgStatistics.ShowStatistics(true);

	CString Out = pThis->m_Templates.sStats;
	// eklmn: new stats
	Out.Replace(_T("[Stats]"), g_App.m_pMDlg->m_dlgStatistics.stattree.GetHTMLForExport());

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::GetPreferences(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sCat, strTmp;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sSession = _ParseURL(Data.sURL, _T("ses"));
	CString Out(m_Templates.sPreferences);

	Out.Replace(_T("[Session]"), sSession);

	if ((_ParseURL(Data.sURL, _T("saveprefs")) == _T("true")) && IsSessionAdmin(Data, sSession))
	{
		strTmp = _ParseURL(Data.sURL, _T("showuploadqueue"));
		m_pWSPrefs->bShowUploadQueue = ((strTmp == _T("true")) || (strTmp.CompareNoCase(_T("on")) == 0));

		strTmp = _ParseURL(Data.sURL, _T("showuploadqueuebanned"));
		m_pWSPrefs->bShowUploadQueueBanned = ((strTmp == _T("true")) || (strTmp.CompareNoCase(_T("on")) == 0));

		strTmp = _ParseURL(Data.sURL, _T("showuploadqueuefriend"));
		m_pWSPrefs->bShowUploadQueueFriend = ((strTmp == _T("true")) || (strTmp.CompareNoCase(_T("on")) == 0));

		strTmp = _ParseURL(Data.sURL, _T("showuploadqueuecredit"));
		m_pWSPrefs->bShowUploadQueueCredit = ((strTmp == _T("true")) || (strTmp.CompareNoCase(_T("on")) == 0));

		strTmp = _ParseURL(Data.sURL, _T("refresh"));
		if (!strTmp.IsEmpty())
			g_App.m_pPrefs->SetWebPageRefresh(_tstoi(strTmp));

		strTmp = _ParseURL(Data.sURL, _T("maxcapdown"));
		if (!strTmp.IsEmpty())
			g_App.m_pPrefs->SetMaxGraphDownloadRate(ValidateDownCapability(10 * _tstoi(strTmp)));
		strTmp = _ParseURL(Data.sURL, _T("maxcapup"));
		if (!strTmp.IsEmpty())
			g_App.m_pPrefs->SetMaxGraphUploadRate(ValidateUpCapability(10 * _tstoi(strTmp)));

		uint32	dwSpeed;

		strTmp = _ParseURL(Data.sURL, _T("maxdown"));
		if (!strTmp.IsEmpty())
		{
			dwSpeed = String2FranctionalRate(strTmp);
			if (dwSpeed != 0)
			{
				g_App.m_pPrefs->SetMaxDownloadWithCheck(dwSpeed);
				g_App.m_pPrefs->SetLimitlessDownload(false);	//SyruS disable limitless on setting a specific value
			}
			else
				g_App.m_pPrefs->SetLimitlessDownload(true);	//SyruS enable limitless on setting to 0
		}
		strTmp = _ParseURL(Data.sURL, _T("maxup"));
		if (!strTmp.IsEmpty())
		{
			dwSpeed = String2FranctionalRate(strTmp);
			if (dwSpeed != 0)
				g_App.m_pPrefs->SetMaxUploadWithCheck(dwSpeed);
		}

		if(!_ParseURL(Data.sURL, _T("maxsources")).IsEmpty())
			g_App.m_pPrefs->SetMaxSourcePerFile(_tstoi(_ParseURL(Data.sURL, _T("maxsources"))));
		if(!_ParseURL(Data.sURL, _T("maxconnections")).IsEmpty())
			g_App.m_pPrefs->SetMaxConnections(static_cast<uint16>(_tstoi(_ParseURL(Data.sURL, _T("maxconnections")))));
		if(!_ParseURL(Data.sURL, _T("maxconnectionsperfive")).IsEmpty())
			g_App.m_pPrefs->SetMaxDownloadConperFive(_tstoi(_ParseURL(Data.sURL, _T("maxconnectionsperfive"))));
	}

	// Fill form
	Out.Replace(_T("[ShowUploadQueueVal]"), (m_pWSPrefs->bShowUploadQueue) ? _T("checked") : _T(""));
	Out.Replace(_T("[ShowUploadQueueBannedVal]"), (m_pWSPrefs->bShowUploadQueueBanned) ? _T("checked") : _T(""));
	Out.Replace(_T("[ShowUploadQueueFriendVal]"), (m_pWSPrefs->bShowUploadQueueFriend) ? _T("checked") : _T(""));
	Out.Replace(_T("[ShowUploadQueueCreditVal]"), (m_pWSPrefs->bShowUploadQueueCredit) ? _T("checked") : _T(""));

	strTmp.Format(_T("%d"), g_App.m_pPrefs->GetWebPageRefresh());
	Out.Replace(_T("[RefreshVal]"), strTmp);

	strTmp.Format(_T("%d"), g_App.m_pPrefs->GetMaxSourcePerFile());
	Out.Replace(_T("[MaxSourcesVal]"), strTmp);

	strTmp.Format(_T("%d"), g_App.m_pPrefs->GetMaxConnections());
	Out.Replace(_T("[MaxConnectionsVal]"), strTmp);

	strTmp.Format(_T("%d"), g_App.m_pPrefs->GetMaxConPerFive());
	Out.Replace(_T("[MaxConnectionsPer5Val]"), strTmp);

	Out.Replace(_T("[KBS]"), _GetPlainResString(IDS_KBYTESEC)+_T(":"));
	Out.Replace(_T("[LimitForm]"), _GetPlainResString(IDS_WEB_CONLIMITS)+_T(":"));
	Out.Replace(_T("[MaxSources]"), _GetPlainResString(IDS_PW_MAXSOURCES)+_T(":"));
	Out.Replace(_T("[MaxConnections]"), _GetPlainResString(IDS_PW_MAXC)+_T(":"));
	Out.Replace(_T("[MaxConnectionsPer5]"), _GetPlainResString(IDS_MAXCON5_TEXT)+_T(":"));
	Out.Replace(_T("[UseGzipForm]"), _GetPlainResString(IDS_WEB_GZIP_COMPRESSION));
	Out.Replace(_T("[ShowUploadQueueForm]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE));
	Out.Replace(_T("[ShowUploadQueueBannedForm]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_BANNED));
	Out.Replace(_T("[ShowUploadQueueFriendForm]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_FRIEND));
	Out.Replace(_T("[ShowUploadQueueCreditForm]"), _GetPlainResString(IDS_WEB_SHOW_UPLOAD_QUEUE_CREDIT));
	Out.Replace(_T("[ShowUploadQueueComment]"), _GetPlainResString(IDS_WEB_UPLOAD_QUEUE_COMMENT));
	Out.Replace(_T("[ShowUploadQueueBannedComment]"), _GetPlainResString(IDS_WEB_UPLOAD_QUEUE_BANNED_COMMENT));
	Out.Replace(_T("[ShowUploadQueueFriendComment]"), _GetPlainResString(IDS_WEB_UPLOAD_QUEUE_FRIEND_COMMENT));
	Out.Replace(_T("[ShowUploadQueueCreditComment]"), _GetPlainResString(IDS_WEB_UPLOAD_QUEUE_CREDIT_COMMENT));
	Out.Replace(_T("[ShowQueue]"), _GetPlainResString(IDS_SHOW));
	Out.Replace(_T("[HideQueue]"), _GetPlainResString(IDS_HIDE));
	Out.Replace(_T("[ShowQueueBanned]"), _GetPlainResString(IDS_SHOW));
	Out.Replace(_T("[HideQueueBanned]"), _GetPlainResString(IDS_HIDE));
	Out.Replace(_T("[ShowQueueFriend]"), _GetPlainResString(IDS_SHOW));
	Out.Replace(_T("[HideQueueFriend]"), _GetPlainResString(IDS_HIDE));
	Out.Replace(_T("[ShowQueueCredit]"), _GetPlainResString(IDS_SHOW));
	Out.Replace(_T("[HideQueueCredit]"), _GetPlainResString(IDS_HIDE));
	Out.Replace(_T("[RefreshTimeForm]"), _GetPlainResString(IDS_WEB_REFRESH_TIME));
	Out.Replace(_T("[RefreshTimeComment]"), _GetPlainResString(IDS_WEB_REFRESH_COMMENT));
	Out.Replace(_T("[SpeedForm]"), _GetPlainResString(IDS_SPEED_LIMITS));
	Out.Replace(_T("[MaxDown]"), _GetPlainResString(IDS_ST_ACTIVEDOWNLOAD));
	Out.Replace(_T("[MaxUp]"), _GetPlainResString(IDS_ST_ACTIVEUPLOAD));
	Out.Replace(_T("[SpeedCapForm]"), _GetPlainResString(IDS_CAPACITY_LIMITS));
	Out.Replace(_T("[MaxCapDown]"), _GetPlainResString(IDS_ST_ACTIVEDOWNLOAD));
	Out.Replace(_T("[MaxCapUp]"), _GetPlainResString(IDS_ST_ACTIVEUPLOAD));
	Out.Replace(_T("[WebControl]"), _GetPlainResString(IDS_WEB_CONTROL));
	Out.Replace(_T("[eMuleAppName]"), CLIENT_NAME);
	Out.Replace(_T("[Apply]"), _GetPlainResString(IDS_PW_APPLY));

//	Show always 0 when limitless is enabled otherwise it can be disabled on apply
	uint32	dwNum = (g_App.m_pPrefs->LimitlessDownload()) ? UNLIMITED : g_App.m_pPrefs->GetMaxDownload();

	FractionalRate2String(&strTmp, (dwNum == UNLIMITED) ? 0 : dwNum);
	Out.Replace(_T("[MaxDownVal]"), strTmp);
	FractionalRate2String(&strTmp, g_App.m_pPrefs->GetMaxUpload());
	Out.Replace(_T("[MaxUpVal]"), strTmp);
	strTmp.Format(_T("%u"), g_App.m_pPrefs->GetMaxGraphDownloadRate() / 10);
	Out.Replace(_T("[MaxCapDownVal]"), strTmp);
	strTmp.Format(_T("%u"), g_App.m_pPrefs->GetMaxGraphUploadRate() / 10);
	Out.Replace(_T("[MaxCapUpVal]"), strTmp);

	Out.Replace(_T("[CatSel]"), sCat);

	return Out;

	EMULE_CATCH

	return _T("");
}

CString CWebServer::_GetLoginScreen(const ThreadData &Data, bool bIsUseGzip)
{
	EMULE_TRY

	CWebServer *pThis = (CWebServer *)Data.pThis;
	if(pThis == NULL)
		return _T("");

	CString Out(pThis->m_Templates.sLogin);
	CString strTmp, strTmp2(_SpecialChars(_GetPlainResString(IDS_WEB_VALID), true));

	Out.Replace(_T("[CharSet]"), _GetWebCharSet());
	Out.Replace(_T("[CapsErrorText]"), _SpecialChars(_GetPlainResString(IDS_WEB_CAPSERROR), true));

	strTmp.Format(strTmp2, _T("HTML"));
	Out.Replace(_T("[ValidHTML]"), strTmp);
	strTmp.Format(strTmp2, _T("CSS"));
	Out.Replace(_T("[ValidCSS]"), strTmp);
	strTmp2 = _SpecialChars(_GetPlainResString(IDS_WEB_CHECK), true);
	strTmp.Format(strTmp2, _T("HTML"));
	Out.Replace(_T("[CheckHTML]"), strTmp);
	strTmp.Format(strTmp2, _T("CSS"));
	Out.Replace(_T("[CheckCSS]"), strTmp);

	Out.Replace(_T("[Goto]"), _SpecialChars(_GetPlainResString(IDS_WEB_GOTO), true));
	Out.Replace(_T("[Nick]"), _SpecialChars(g_App.m_pPrefs->GetUserNick()));
	Out.Replace(_T("[eMuleAppName]"), CLIENT_NAME);
	Out.Replace(_T("[version]"), CURRENT_VERSION_LONG);
	Out.Replace(_T("[Login]"), _GetPlainResString(IDS_WEB_LOGIN));
	Out.Replace(_T("[EnterPassword]"), _GetPlainResString(IDS_WEB_ENTER_PASSWORD));
	Out.Replace(_T("[LoginNow]"), _GetPlainResString(IDS_WEB_LOGIN_NOW));
	Out.Replace(_T("[WebControl]"), _GetPlainResString(IDS_WEB_CONTROL));

	if(pThis->m_nIntruderDetect >= 1)
		Out.Replace(_T("[FailedLogin]"), _T("<p class=\"failed\">") + _GetPlainResString(IDS_WEB_BADLOGINATTEMPT) + _T("</p>"));
	else
		Out.Replace(_T("[FailedLogin]"), _T("&nbsp;"));

	Out.Replace(_T("[isGziped]"), GetResString((bIsUseGzip) ? IDS_ENABLED : IDS_DISABLED));

	return Out;

	EMULE_CATCH

	return _T("");
}

// We have to add gz-header and some other stuff
// to standard zlib functions
// in order to use gzip in web pages
int CWebServer::GzipCompress(byte *pbyteDst, unsigned *puiDstLen, const byte *pbyteSrc, unsigned uiSrcLen, int level)
{
	EMULE_TRY

	const static int gz_magic[2] = {0x1f, 0x8b}; // gzip magic header
	int err;
	uLong crc;
	z_stream stream = {0};
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;
	crc = crc32(0L, Z_NULL, 0);
	// init Zlib stream
	// NOTE windowBits is passed < 0 to suppress zlib header
	err = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if (err != Z_OK)
		return err;

	sprintf((char*)pbyteDst, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
		Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, 255);
	// wire buffers
	stream.next_in = (Bytef*)pbyteSrc;
	stream.avail_in = (uInt)uiSrcLen;
	stream.next_out = (Bytef*)(pbyteDst + 10);
	stream.avail_out = *puiDstLen - 18;
	// doit
	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END)
	{
		deflateEnd(&stream);
		return err;
	}
	err = deflateEnd(&stream);
	crc = crc32(crc, (const Bytef*)pbyteSrc, uiSrcLen);
	//CRC
	*(pbyteDst + 10 + stream.total_out + 0) = (byte)(crc & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 1) = (byte)((crc >> 8) & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 2) = (byte)((crc >> 16) & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 3) = (byte)((crc >> 24) & 0xFF);
	// Length
	*(pbyteDst + 10 + stream.total_out + 4) = (byte)(uiSrcLen & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 5) = (byte)((uiSrcLen >> 8) & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 6) = (byte)((uiSrcLen >> 16) & 0xFF);
	*(pbyteDst + 10 + stream.total_out + 7) = (byte)((uiSrcLen >> 24) & 0xFF);

	*puiDstLen = 10 + stream.total_out + 8;
	return err;

	EMULE_CATCH

	return -1;
}

bool CWebServer::IsLoggedIn(const ThreadData &Data, long lSession)
{
	NOPRM(Data);
	EMULE_TRY

	UpdateSessionCount();

	if (lSession != 0)
	{
	//	Find current session
		for (int i = 0; i < Sessions.GetSize(); i++)
		{
			if (Sessions[i].lSession == lSession)
				return true;
		}
	}

	EMULE_CATCH

	return false;
}

bool CWebServer::RemoveSession(const ThreadData &Data, long lSession)
{
	NOPRM(Data);

	if (lSession == 0)
		return false;

	EMULE_TRY

//	Find current session
	for (int i = 0; i < Sessions.GetSize(); i++)
	{
		if (Sessions[i].lSession == lSession)
		{
			Sessions.RemoveAt(i);
			CString t_ulCurIP;
			t_ulCurIP.Format( _T("%u.%u.%u.%u"),
				(byte)m_ulCurIP, (byte)(m_ulCurIP>>8), (byte)(m_ulCurIP>>16), (byte)(m_ulCurIP>>24) );
			g_App.m_pMDlg->AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_SESSIONEND),t_ulCurIP);
			return true;
		}
	}

	EMULE_CATCH

	return false;
}

bool CWebServer::GetSessionByID(Session *ses, long lSessionID, bool bUpdateTime)
{
	EMULE_TRY

	if (lSessionID != 0)
	{
		for (int i = 0; i < Sessions.GetSize(); i++)
		{
			if (Sessions[i].lSession == lSessionID)
			{
				if (bUpdateTime)	//	Reset expiration time if required
					Sessions[i].startTime = CTime::GetCurrentTime();
				*ses = Sessions.GetAt(i);
				return true;
			}
		}
	}

	EMULE_CATCH
	return false;
}

bool CWebServer::IsSessionAdmin(const ThreadData &Data, const CString &strSsessionID)
{
	NOPRM(Data);
	EMULE_TRY

	long sessionID = _tstol(strSsessionID);

	for (int i = 0; i < Sessions.GetSize(); i++)
	{
		if (Sessions[i].lSession == sessionID && sessionID != 0)
			return Sessions[i].admin;
	}

	EMULE_CATCH
	return false;
}

CString CWebServer::_GetPlainResString(UINT nID, bool noquote)
{
	EMULE_TRY

	CString sRet = GetResString(nID);
	sRet.Remove(_T('&'));
	if(noquote)
	{
		sRet.Replace(_T("'"), _T("&#8217;"));
		sRet.Replace(_T("\n"), _T("\\n"));
	}
	return sRet;

	EMULE_CATCH

	return _T("");
}

void CWebServer::_GetPlainResString(CString *pstrOut, UINT nID, bool noquote /*=false*/)
{
	GetResString(pstrOut, nID);
	pstrOut->Remove(_T('&'));
	if(noquote)
	{
		pstrOut->Replace(_T("'"), _T("&#8217;"));
		pstrOut->Replace(_T("\n"), _T("\\n"));
	}
}

CString	CWebServer::_GetWebCharSet()
{
	EMULE_TRY

	switch (g_App.m_pPrefs->GetLanguageID())
	{
		case MAKELANGID(LANG_POLISH,SUBLANG_DEFAULT):				return _T("windows-1250");
		case MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT):				return _T("windows-1251");
		case MAKELANGID(LANG_GREEK,SUBLANG_DEFAULT):				return _T("ISO-8859-7");
		case MAKELANGID(LANG_HEBREW,SUBLANG_DEFAULT):				return _T("ISO-8859-8-i");
		case MAKELANGID(LANG_KOREAN,SUBLANG_DEFAULT):				return _T("EUC-KR");
		case MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED):	return _T("GB2312");
		case MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_TRADITIONAL):	return _T("Big5");
		case MAKELANGID(LANG_LITHUANIAN,SUBLANG_DEFAULT):			return _T("windows-1257");
		case MAKELANGID(LANG_ROMANIAN,SUBLANG_DEFAULT):				return _T("windows-1250");
		case MAKELANGID(LANG_TURKISH,SUBLANG_DEFAULT):				return _T("windows-1254");
	}

	EMULE_CATCH

	// Western (Latin) includes Catalan, Danish, Dutch, English, Faeroese, Finnish, French,
	// German, Galician, Irish, Icelandic, Italian, Norwegian, Portuguese, Spanish and Swedish
	return _T("ISO-8859-1");
}

// Ornis: creating the progressbar. colored if ressources are given/available
CString CWebServer::_GetDownloadGraph(const ThreadData &Data, const byte *pbyteHash)
{
	EMULE_TRY

	CWebServer	*pThis = reinterpret_cast<CWebServer *>(Data.pThis);

	if (pThis == NULL)
		return _T("");

	CPartFile	*pPartFile = g_App.m_pDownloadQueue->GetFileByID(pbyteHash);
	CString		Out;
	CString 	progresscolor[12];

	if (pPartFile != NULL && (pPartFile->GetStatus() == PS_PAUSED || pPartFile->GetStatus() == PS_STOPPED))
	{
	//	Color style (paused files)
		progresscolor[0]=_T("p_green.gif");
		progresscolor[1]=_T("p_black.gif");
		progresscolor[2]=_T("p_yellow.gif");
		progresscolor[3]=_T("p_red.gif");
		progresscolor[4]=_T("p_blue1.gif");
		progresscolor[5]=_T("p_blue2.gif");
		progresscolor[6]=_T("p_blue3.gif");
		progresscolor[7]=_T("p_blue4.gif");
		progresscolor[8]=_T("p_blue5.gif");
		progresscolor[9]=_T("p_blue6.gif");
		progresscolor[10]=_T("p_greenpercent.gif");
		progresscolor[11]=_T("transparent.gif");
	}
	else
	{
	//	Color style (active files)
		progresscolor[0]=_T("green.gif");
		progresscolor[1]=_T("black.gif");
		progresscolor[2]=_T("yellow.gif");
		progresscolor[3]=_T("red.gif");
		progresscolor[4]=_T("blue1.gif");
		progresscolor[5]=_T("blue2.gif");
		progresscolor[6]=_T("blue3.gif");
		progresscolor[7]=_T("blue4.gif");
		progresscolor[8]=_T("blue5.gif");
		progresscolor[9]=_T("blue6.gif");
		progresscolor[10]=_T("greenpercent.gif");
		progresscolor[11]=_T("transparent.gif");
	}

	if (pPartFile == NULL || !pPartFile->IsPartFile())
	{
		Out.AppendFormat(pThis->m_Templates.sProgressbarImgsPercent + _T("<br />"), progresscolor[10], pThis->m_Templates.iProgressbarWidth);
		Out.AppendFormat(pThis->m_Templates.sProgressbarImgs,progresscolor[0], pThis->m_Templates.iProgressbarWidth);
	}
	else
	{
		CString s_ChunkBar=pPartFile->GetProgressString(pThis->m_Templates.iProgressbarWidth);

		// and now make a graph out of the array - need to be in a progressive way
		byte lastcolor = 1, byteColor;
		uint16 lastindex = 0;

		int		compl = static_cast<int>((pThis->m_Templates.iProgressbarWidth / 100.0) * pPartFile->GetPercentCompleted());

		Out.AppendFormat(pThis->m_Templates.sProgressbarImgsPercent + _T("<br />"),
			progresscolor[(compl > 0) ? 10 : 11], (compl > 0) ? compl : 5);

		for (uint16 i=0;i<pThis->m_Templates.iProgressbarWidth;i++)
		{
			byteColor = static_cast<byte>(_tstoi(s_ChunkBar.Mid(i, 1)));
			if (lastcolor != byteColor)
			{
				if (i > lastindex)
					Out.AppendFormat(pThis->m_Templates.sProgressbarImgs, progresscolor[lastcolor], i-lastindex);

				lastcolor = byteColor;
				lastindex=i;
			}
		}
		Out.AppendFormat(pThis->m_Templates.sProgressbarImgs, progresscolor[lastcolor], pThis->m_Templates.iProgressbarWidth-lastindex);
	}
	return Out;

	EMULE_CATCH

	return _T("");
}

CString	CWebServer::GetSearch(const ThreadData &Data)
{
	EMULE_TRY

	int		iCat = _tstoi(_ParseURL(Data.sURL, _T("cat"))) & 0xFF;
	CString	sSession = _ParseURL(Data.sURL, _T("ses"));
	CString	Out(m_Templates.sSearch);
	bool	bAdmin = IsSessionAdmin(Data, sSession);

	if (!_ParseURL(Data.sURL, _T("downloads")).IsEmpty() && bAdmin)
	{
		CString	strDownloads = _ParseURLArray(Data.sURL, _T("downloads"));
		EnumCategories	eCatID = (iCat == 0) ? CAT_NONE : CCat::GetCatIDByUserIndex(iCat);
		int		iCurPos = 0;
		CString	strResToken = strDownloads.Tokenize(_T("|"), iCurPos);
		byte	abyteFileHash[16];

		while (!strResToken.IsEmpty())
		{
			if (md4cmp0(StringToHash(strResToken, abyteFileHash)) != 0)
				g_App.m_pSearchList->AddFileToDownloadByHash(abyteFileHash, eCatID);
			strResToken = strDownloads.Tokenize(_T("|"), iCurPos);
		}

		iCat = (iCat == 0) ? 0 : CCat::UserCatIndexToCatIndex(iCat);
	}

	CString sCat;

	if (iCat != 0)
		sCat.Format(_T("%u"), iCat);

	CString sCmd = _ParseURL(Data.sURL, _T("c"));

	if (sCmd.CompareNoCase(_T("update")) == 0 && bAdmin)
	{
		g_App.m_pPrefs->SetFakeListURL(_ParseURL(Data.sURL, _T("url")));
		g_App.m_pFakeCheck->UpdateFakeList();
	}

	if (!_ParseURL(Data.sURL, _T("tosearch")).IsEmpty() && bAdmin)
	{
		// perform search
		g_App.m_pMDlg->m_dlgSearch.DeleteAllSearches();
		g_App.m_pMDlg->m_dlgSearch.DoNewEd2kSearch(
			_ParseURL(Data.sURL, _T("tosearch")),
			_ParseURL(Data.sURL, _T("type")),
			static_cast<uint64>(_tstol(_ParseURL(Data.sURL, _T("min")))) * 1048576ui64,
			static_cast<uint64>(_tstol(_ParseURL(Data.sURL, _T("max")))) * 1048576ui64,
			_tstoi(_ParseURL(Data.sURL, _T("avail"))),
			_ParseURL(Data.sURL, _T("ext")),
			(_ParseURL(Data.sURL, _T("global")) == _T("on")),	_T(""));
		Out.Replace(_T("[Message]"),_GetPlainResString(IDS_SW_SEARCHINGINFO));
	}
	else if (!_ParseURL(Data.sURL, _T("tosearch")).IsEmpty() && !bAdmin)
		Out.Replace(_T("[Message]"),_GetPlainResString(IDS_ACCESSDENIED));
	else
		Out.Replace(_T("[Message]"), _GetPlainResString(IDS_SW_REFETCHRES));

	CString strTmp = _ParseURL(Data.sURL, _T("sort"));

	if (!strTmp.IsEmpty())
		m_iSearchSortby = _tstoi(strTmp);
	strTmp = _ParseURL(Data.sURL, _T("sortAsc"));
	if (!strTmp.IsEmpty())
		m_bSearchAsc = (_tstoi(strTmp) != 0);

	sCmd = _ParseURL(Data.sURL, _T("c"));
	if (sCmd.CompareNoCase(_T("menu")) == 0)
		SetHiddenColumnState(Data.sURL, m_pWSPrefs->abSearchColHidden, ARRSIZE(m_pWSPrefs->abSearchColHidden));

	CString strResultList = m_Templates.sSearchHeader;

	strResultList += g_App.m_pSearchList->GetWebList(m_Templates.sSearchResultLine, m_iSearchSortby, m_bSearchAsc,
		!m_pWSPrefs->abSearchColHidden[0], !m_pWSPrefs->abSearchColHidden[1],
		!m_pWSPrefs->abSearchColHidden[2], !m_pWSPrefs->abSearchColHidden[3], !m_pWSPrefs->abSearchColHidden[4]);
	if (CCat::GetNumCats() > 1)
		InsertCatBox(Out, 0, m_Templates.sCatArrow, false, false, sSession, NULL);
	else
		Out.Replace(_T("[CATBOX]"), _T(""));

	Out.Replace(_T("[SEARCHINFOMSG]"),_T(""));
	Out.Replace(_T("[RESULTLIST]"), strResultList);
	Out.Replace(_T("[Result]"), GetResString(IDS_SW_RESULT));
	Out.Replace(_T("[Session]"), sSession);
	Out.Replace(_T("[Name]"), _GetPlainResString(IDS_SW_NAME));
	Out.Replace(_T("[Type]"), _GetPlainResString(IDS_TYPE));
	Out.Replace(_T("[Any]"), _GetPlainResString(IDS_SEARCH_ANY));
	Out.Replace(_T("[Audio]"), _GetPlainResString(IDS_SEARCH_AUDIO));
	Out.Replace(_T("[Image]"), _GetPlainResString(IDS_SEARCH_PICS));
	Out.Replace(_T("[Video]"), _GetPlainResString(IDS_SEARCH_VIDEO));
	Out.Replace(_T("[Document]"), _GetPlainResString(IDS_SEARCH_DOC));
	Out.Replace(_T("[CDImage]"), _GetPlainResString(IDS_SEARCH_CDIMG));
	Out.Replace(_T("[Program]"), _GetPlainResString(IDS_SEARCH_PRG));
	Out.Replace(_T("[Archive]"), _GetPlainResString(IDS_SEARCH_ARC));
	Out.Replace(_T("[Search]"), _GetPlainResString(IDS_SEARCH_NOUN));
	Out.Replace(_T("[Show]"), _GetPlainResString(IDS_SHOW));
	Out.Replace(_T("[Size]"), _GetPlainResString(IDS_DL_SIZE));
	Out.Replace(_T("[Sort]"), _GetPlainResString(IDS_PW_SORTING));
	Out.Replace(_T("[SizeMin]"), _GetPlainResString(IDS_SEARCHMINSIZE));
	Out.Replace(_T("[SizeMax]"), _GetPlainResString(IDS_SEARCHMAXSIZE));
	Out.Replace(_T("[Availabl]"), _GetPlainResString(IDS_SEARCHAVAIL));
	Out.Replace(_T("[Extention]"), _GetPlainResString(IDS_SEARCHEXTENSION_LBL));
	Out.Replace(_T("[Global]"), _GetPlainResString(IDS_SW_GLOBAL));
	Out.Replace(_T("[MB]"), _GetPlainResString(IDS_MBYTES));
	Out.Replace(_T("[FakeListHeader]"), _GetPlainResString(IDS_FAKE_UPDATE));
	Out.Replace(_T("[FakeListText]"), _GetPlainResString(IDS_SV_ADDRESS));
	Out.Replace(_T("[FakeListUrl]"), g_App.m_pPrefs->GetFakeListURL());
	Out.Replace(_T("[Apply]"), _GetPlainResString(IDS_PW_APPLY));
	Out.Replace(_T("[CatSel]"), sCat);

	Out.Replace(_T("[checked]"),
		(g_App.m_pPrefs->GetSearchMethod() == EP_SEARCHMETHOD_GLOB) ? _T("checked") : _T(""));

	const TCHAR *pcSortIcon = (m_bSearchAsc) ? m_Templates.sUpArrow : m_Templates.sDownArrow;

	_GetPlainResString(&strTmp, IDS_DL_FILENAME);
	if (!m_pWSPrefs->abSearchColHidden[0])
	{
		Out.Replace(_T("[FilenameI]"), (m_iSearchSortby == 0) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FilenameH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FilenameI]"), _T(""));
		Out.Replace(_T("[FilenameH]"), _T(""));
	}
	Out.Replace(_T("[FilenameM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SIZE);
	if (!m_pWSPrefs->abSearchColHidden[1])
	{
		Out.Replace(_T("[FilesizeI]"), (m_iSearchSortby == 1) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FilesizeH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FilesizeI]"), _T(""));
		Out.Replace(_T("[FilesizeH]"), _T(""));
	}
	Out.Replace(_T("[FilesizeM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_FILEHASH);
	if (!m_pWSPrefs->abSearchColHidden[2])
	{
		Out.Replace(_T("[FilehashI]"), (m_iSearchSortby == 2) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FilehashH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FilehashI]"), _T(""));
		Out.Replace(_T("[FilehashH]"), _T(""));
	}
	Out.Replace(_T("[FilehashM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_DL_SOURCES);
	if (!m_pWSPrefs->abSearchColHidden[3])
	{
		Out.Replace(_T("[SourcesI]"), (m_iSearchSortby == 3) ? pcSortIcon : _T(""));
		Out.Replace(_T("[SourcesH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[SourcesI]"), _T(""));
		Out.Replace(_T("[SourcesH]"), _T(""));
	}
	Out.Replace(_T("[SourcesM]"), strTmp);

	_GetPlainResString(&strTmp, IDS_FAKE_CHECK_HEADER);
	if (!m_pWSPrefs->abSearchColHidden[4])
	{
		Out.Replace(_T("[FakeCheckI]"), (m_iSearchSortby == 4) ? pcSortIcon : _T(""));
		Out.Replace(_T("[FakeCheckH]"), strTmp);
	}
	else
	{
		Out.Replace(_T("[FakeCheckI]"), _T(""));
		Out.Replace(_T("[FakeCheckH]"), _T(""));
	}
	Out.Replace(_T("[FakeCheckM]"), strTmp);

	Out.Replace(_T("[Download]"), _GetPlainResString(IDS_DOWNLOAD_VERB));

	Out.Replace(_T("[SORTASCVALUE0]"), ((m_iSearchSortby != 0) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE1]"), ((m_iSearchSortby != 1) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE2]"), ((m_iSearchSortby != 2) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE3]"), ((m_iSearchSortby != 3) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE4]"), ((m_iSearchSortby != 4) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE5]"), ((m_iSearchSortby != 5) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));
	Out.Replace(_T("[SORTASCVALUE6]"), ((m_iSearchSortby != 6) || (m_bSearchAsc == 0)) ? _T("1") : _T("0"));

	return Out;

	EMULE_CATCH
	return _T("");
}

int CWebServer::UpdateSessionCount()
{
	EMULE_TRY

	for (int i = 0; i < Sessions.GetSize();)
	{
		CTimeSpan ts = CTime::GetCurrentTime() - Sessions[i].startTime;
		if(ts.GetTotalSeconds() > SESSION_TIMEOUT_SECS)
			Sessions.RemoveAt(i);
		else
			i++;
	}
	return Sessions.GetCount();

	EMULE_CATCH
	return 0;
}

void CWebServer::InsertCatBox(CString &Out, int preselect, const CString &strBoxLbl, bool jump, bool extraCats, const CString &strSession, const byte *pbyteHash)
{
	EMULE_TRY

	CString	strCategory, strTmp(strBoxLbl);

	strTmp += (jump) ?
		_T("<select name=\"cat\" size=\"1\"")
		_T(" onchange=GotoCat(this.form.cat.options[this.form.cat.selectedIndex].value)>") :
		_T("<select name=\"cat\" size=\"1\">");

	for (int i = 0; i-1 < CCat::GetNumUserCats(); i++)
	{
		strCategory = (i == 0) ? _GetPlainResString(IDS_CAT_UNCATEGORIZED) : CCat::GetCatByUserIndex(i)->GetTitle();
		strCategory.Replace(_T("'"),_T("\'"));
		strTmp.AppendFormat(_T("<option%s value=\"%i\">%s</option>"), (i == preselect) ? _T(" selected") : _T(""), i, strCategory);
	}
	if (extraCats)
	{
		if (CCat::GetNumCats() > 1)
			strTmp += _T("<option>------------</option>");

		for (int i = ((CCat::GetNumCats() > 1) ? 1 : 2); i <= 14; i++)
		{
			strTmp.AppendFormat(_T("<option%s value=\"%i\">%s</option>"), (i == preselect) ? _T(" selected") : _T(""), i+100, GetSubCatLabel(i));
		}
	}
	strTmp += _T("</select>");
	Out.Replace(_T("[CATBOX]"), strTmp);

	CString	tempBuff4;
	TCHAR	acHashStr[MAX_HASHSTR_SIZE], *pcTmp, *pcHashStr;

	strTmp = _T("");

	for (int i = 0; i < CCat::GetNumCats(); i++)
	{
		if (i == preselect)
		{
			pcTmp = _T("checked.gif");
			if (i < CCat::GetNumPredefinedCats())
				tempBuff4 = CCat::GetPredefinedCatTitle(CCat::GetCatIDByIndex(i));
			else
				tempBuff4 = CCat::GetCatByIndex(i)->GetTitle();
		}
		else
			pcTmp = _T("checked_no.gif");

		strCategory = (i < CCat::GetNumPredefinedCats()) ? CCat::GetPredefinedCatTitle(CCat::GetCatIDByIndex(i)) : CCat::GetCatByIndex(i)->GetTitle();
		strCategory.Replace(_T("'"), _T("\\'"));

		strTmp.AppendFormat(_T("<a href=&quot;/?ses=%s&amp;w=transfer&amp;cat=%d&quot;>")
			_T("<div class=menuitems><img class=menuchecked src=%s>%s&nbsp;</div></a>"),
			strSession, i, pcTmp, strCategory);
	}
	if (extraCats)
	{
		for (int i = ((CCat::GetNumCats() > 1) ? 1 : 2); i <= 14; i++)
		{
			if ((i + CAT_PREDEFINED) == preselect)
			{
				pcTmp = _T("checked.gif");
				tempBuff4 = GetSubCatLabel(i);
			}
			else
				pcTmp = _T("checked_no.gif");

			strTmp.AppendFormat(_T("<a href=&quot;/?ses=%s&amp;w=transfer&amp;cat=%d&quot;>")
				_T("<div class=menuitems><img class=menuchecked src=%s>%s&nbsp;</div></a>"),
				strSession, i + CAT_PREDEFINED, pcTmp, GetSubCatLabel(i));
		}
	}
	Out.Replace(_T("[CatBox]"), strTmp);
	Out.Replace(_T("[Category]"), tempBuff4);

	strTmp = _T("");

	CPartFile	*pPartFile;

	if (pbyteHash != NULL)
	{
		pPartFile = g_App.m_pDownloadQueue->GetFileByID(pbyteHash);
		pcHashStr = md4str(pbyteHash, acHashStr);
	}
	else
	{
		pPartFile = NULL;
		pcHashStr = _T("");
	}

//	For each user category index...
	for (int i = 0; i <= CCat::GetNumUserCats(); i++)
	{
	//	Get the user category index of 'pPartFile' in 'preselect'.
		if (pPartFile != NULL)
			preselect = CCat::GetUserCatIndexByID(pPartFile->GetCatID());

		strCategory = (i == 0)? GetResString(IDS_CAT_UNASSIGN) : CCat::GetCatByUserIndex(i)->GetTitle();
		strCategory.Replace(_T("'"),_T("\\'"));

		strTmp.AppendFormat(_T("<a href=&quot;/?ses=%s&amp;w=transfer[CatSel]&amp;op=setcat&amp;file=%s&amp;filecat=%u&quot;>")
			_T("<div class=menuitems><img class=menuchecked src=%s>%s&nbsp;</div></a>"),
			strSession, pcHashStr, i,
			(i == preselect) ? _T("checked.gif") : _T("checked_no.gif"), strCategory);
	}
	Out.Replace(_T("[SetCatBox]"), strTmp);

	EMULE_CATCH
}

CString CWebServer::GetSubCatLabel(int iCat)
{
	EMULE_TRY

	CString sPlain = CCat::GetPredefinedCatTitle(static_cast<_EnumCategories>(iCat+CAT_PREDEFINED));

	sPlain.Remove(_T('&'));
	sPlain.Replace(_T("'"), _T("&#8217;"));

	return sPlain;

	EMULE_CATCH
	return _T("");
}

long CWebServer::DoAuthentication(const ThreadData &Data, Session *ses)
{
	EMULE_TRY

	ses->lSession = 0;
	ses->startTime = 0;
	ses->admin = false;
	
	CString	strTmp(_ParseURL(Data.sURL, _T("ses")));

	if (!strTmp.IsEmpty())
		GetSessionByID(ses, _tstol(strTmp), true);
	else
	{
		if ( (_ParseURL(Data.sURL, _T("w")) == _T("password")) &&
			!GetIsTempDisabled() && _ParseURL(Data.sURL, _T("c")).IsEmpty() )
		{
			byte	abyteDigest[16], abyteWSPass[16];

			strTmp.Format(_T("%u.%u.%u.%u"),(byte)m_ulCurIP,(byte)(m_ulCurIP>>8),(byte)(m_ulCurIP>>16),(byte)(m_ulCurIP>>24));

			if (md4cmp(MD5Sum(_ParseURL(Data.sURL, _T("p")), abyteDigest), g_App.m_pPrefs->GetWSPass(abyteWSPass)) == 0)
			{
				ses->admin = true;
				ses->startTime = CTime::GetCurrentTime();
				ses->lSession = ((rand() << 15) | rand()) + 1;
				Sessions.Add(*ses);
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_ADMINLOGIN), strTmp);
				m_nIntruderDetect = 0;
			}
			else if ( g_App.m_pPrefs->GetWSIsLowUserEnabled() &&
				(md4cmp(abyteDigest, g_App.m_pPrefs->GetWSLowPass(abyteWSPass)) == 0) )
			{
				ses->startTime = CTime::GetCurrentTime();
				ses->lSession = ((rand() << 15) | rand()) + 1;
				Sessions.Add(*ses);
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_GUESTLOGIN), strTmp);
				m_nIntruderDetect = 0;
			}
			else if (g_App.m_pPrefs->IsWSIntruderDetectionEnabled())
			{
				m_nIntruderDetect++;
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_INTRTRIESLEFT), strTmp, g_App.m_pPrefs->GetWSLoginAttemptsAllowed()-(m_nIntruderDetect));
				if (m_nIntruderDetect == g_App.m_pPrefs->GetWSLoginAttemptsAllowed())
				{
					CString MessageText;

					AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_INTRDETECTION), m_nIntruderDetect);
					if (g_App.m_pPrefs->GetWSTempDisableLogin() > 0)
					{
						m_nStartTempDisabledTime = ::GetTickCount();
						m_bIsTempDisabled = true;
						AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_INTRBLOCKTIME), g_App.m_pPrefs->GetWSTempDisableLogin());
						m_nIntruderDetect = 0;
						MessageText.Format(GetResString(IDS_WEB_INTRBLOCKTIME), g_App.m_pPrefs->GetWSTempDisableLogin());
					}
					else
					{
						g_App.m_pPrefs->SetWSIsEnabled(false);
						m_bServerWorking = false;
						StopSockets();
						AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_INTRWEBDISABLE));
						m_nIntruderDetect = 0;
						GetResString(&MessageText, IDS_WEB_INTRWEBDISABLE);
					}
					g_App.m_pMDlg->SendMail(MessageText, g_App.m_pPrefs->GetNotifierPopOnWebServerError(), g_App.m_pPrefs->IsSMTPWarningEnabled());
					g_App.m_pMDlg->ShowNotifier(MessageText, TBN_WEBSERVER, false, g_App.m_pPrefs->GetNotifierPopOnWebServerError());
				}
			}
			else
				AddLogLine(true, RGB_LOG_NOTICE + GetResString(IDS_WEB_BADLOGINATTEMPT));
		}
	}

	return ses->lSession;

	EMULE_CATCH

	ses->lSession = 0;
	return 0;
}

void CWebServer::SetHiddenColumnState(const CString &strURL, bool *pbCols, unsigned uiArrSz)
{
	unsigned	uiMenu = _tstoi(_ParseURL(strURL, _T("m")));
	bool		bValue = (_tstoi(_ParseURL(strURL, _T("v"))) != 0);
	
	if (uiMenu >= uiArrSz)	//outside array boundary
		return;
	pbCols[uiMenu] = bValue;
}
