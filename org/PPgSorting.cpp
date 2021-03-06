// PPgSorting.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgSorting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// PPgSorting dialog

IMPLEMENT_DYNAMIC(CPPgSorting, CPropertyPage)
CPPgSorting::CPPgSorting()
	: CPropertyPage(CPPgSorting::IDD)
	, bUseSorting(FALSE)
	, bUseSourcesSorting(FALSE)
	, bPausedStoppedLast(FALSE)
{
}

CPPgSorting::~CPPgSorting()
{
}

void CPPgSorting::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SORT_SERVERS, serverColsCombo);
	DDX_Control(pDX, IDC_SORT_FILES, fileColsCombo);
	DDX_Control(pDX, IDC_SORT_DOWNLOADS, downloadColsCombo);
	DDX_Control(pDX, IDC_SORT_UPLOADS, uploadColsCombo);
	DDX_Control(pDX, IDC_SORT_QUEUE, queueColsCombo);
	DDX_Control(pDX, IDC_SORT_SEARCH, searchColsCombo);
	DDX_Control(pDX, IDC_SORT_IRC, ircColsCombo);
	DDX_Control(pDX, IDC_SORT_CLIENTLIST, clientListColsCombo);
	DDX_Control(pDX, IDC_SORT_SOURCES1, sourceCols1Combo);
	DDX_Control(pDX, IDC_SORT_SOURCES2, sourceCols2Combo);
	DDX_Check(pDX, IDC_SORT_USEIT, bUseSorting);
	DDX_Check(pDX, IDC_SORT_SOURCES2_BOX, bUseSourcesSorting);
	DDX_Check(pDX, IDC_SORT_NONACTIVELAST, bPausedStoppedLast);
	DDX_Text(pDX, IDC_SORT_SERVERS_LBL, sortServersDesc);
	DDX_Text(pDX, IDC_SORT_DOWNLOADS_LBL, sortDownloadsDesc);
	DDX_Text(pDX, IDC_SORT_UPLOADS_LBL, sortUploadsDesc);
	DDX_Text(pDX, IDC_SORT_QUEUE_LBL, sortQueueDesc);
	DDX_Text(pDX, IDC_SORT_SEARCH_LBL, sortSearchDesc);
	DDX_Text(pDX, IDC_SORT_IRC_LBL, sortIrcDesc);
	DDX_Text(pDX, IDC_SORT_CLIENTLIST_LBL, sortClientListDesc);
	DDX_Text(pDX, IDC_SORT_FILES_LBL, sortFilesDesc);
	DDX_Text(pDX, IDC_SORT_SOURCES1_LBL, sortSources1Desc);
	DDX_Text(pDX, IDC_SORT_GRP, sortGrpDesc);
	DDX_Control(pDX, IDC_SORT_SOURCES2_BOX, sortSources2Check);
	DDX_Control(pDX, IDC_SORT_USEIT, useItCheck);
	DDX_Control(pDX, IDC_SORT_NONACTIVELAST, pausedStoppedLastCheck);
}

BEGIN_MESSAGE_MAP(CPPgSorting, CPropertyPage)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SORT_USEIT, OnBnClickedUseSort)
	ON_CBN_SELCHANGE(IDC_SORT_SERVERS, OnCbnSelchangeSortServers)
	ON_CBN_SELCHANGE(IDC_SORT_FILES, OnCbnSelchangeSortFiles)
	ON_CBN_SELCHANGE(IDC_SORT_DOWNLOADS, OnCbnSelchangeSortDownloads)
	ON_CBN_SELCHANGE(IDC_SORT_SOURCES1, OnCbnSelchangeSortSources1)
	ON_CBN_SELCHANGE(IDC_SORT_SOURCES2, OnCbnSelchangeSortSources2)
	ON_CBN_SELCHANGE(IDC_SORT_UPLOADS, OnCbnSelchangeSortUploads)
	ON_CBN_SELCHANGE(IDC_SORT_QUEUE, OnCbnSelchangeSortQueue)
	ON_CBN_SELCHANGE(IDC_SORT_SEARCH, OnCbnSelchangeSortSearch)
	ON_CBN_SELCHANGE(IDC_SORT_IRC, OnCbnSelchangeSortIrc)
	ON_CBN_SELCHANGE(IDC_SORT_CLIENTLIST, OnCbnSelchangeSortClientList)
	ON_BN_CLICKED(IDC_SORT_SOURCES2_BOX, OnBnClickedSortSources2Box)
	ON_BN_CLICKED(IDC_SORT_NONACTIVELAST, OnBnClickedPausedStoppedLast)
END_MESSAGE_MAP()

BOOL CPPgSorting::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadSettings();
	Localize();
	return TRUE;
}

void CPPgSorting::LoadSettings(void)
{
	bUseSorting = m_pPrefs->DoUseSort();
	bUseSourcesSorting = m_pPrefs->DoUseSrcSortCol2();
	bPausedStoppedLast = m_pPrefs->DoPausedStoppedLast();

	UpdateData(FALSE);
	OnBnClickedUseSort();
	SetModified(FALSE);
}

BOOL CPPgSorting::OnApply()
{
	if (m_bModified)
	{
		UpdateData(TRUE);

		m_pPrefs->SetUseSort(bUseSorting);
		m_pPrefs->SetUseSrcSortCol2(bUseSourcesSorting);
		m_pPrefs->SetPausedStoppedLast(bPausedStoppedLast);
		m_pPrefs->SetServerSortCol((uint16)serverColsCombo.GetItemData(serverColsCombo.GetCurSel()));
		m_pPrefs->SetUploadSortCol((uint16)uploadColsCombo.GetItemData(uploadColsCombo.GetCurSel()));
		m_pPrefs->SetQueueSortCol((uint16)queueColsCombo.GetItemData(queueColsCombo.GetCurSel()));
		m_pPrefs->SetSearchSortCol((uint16)searchColsCombo.GetItemData(searchColsCombo.GetCurSel()));
		m_pPrefs->SetIrcSortCol((uint16)ircColsCombo.GetItemData(ircColsCombo.GetCurSel()));
		m_pPrefs->SetClientListSortCol((uint16)clientListColsCombo.GetItemData(clientListColsCombo.GetCurSel()));
		m_pPrefs->SetFileSortCol((uint16)fileColsCombo.GetItemData(fileColsCombo.GetCurSel()));
		m_pPrefs->SetDownloadSortCol((uint16)downloadColsCombo.GetItemData(downloadColsCombo.GetCurSel()));
		m_pPrefs->SetSrcSortCol1((uint16)sourceCols1Combo.GetItemData(sourceCols1Combo.GetCurSel()));
		m_pPrefs->SetSrcSortCol2((uint16)sourceCols2Combo.GetItemData(sourceCols2Combo.GetCurSel()));

		SetModified();

		if (m_pPrefs->DoUseSort())
		{
			g_App.m_pMDlg->m_wndServer.m_ctlServerList.SortInit(m_pPrefs->GetServerSortCol());
			g_App.m_pMDlg->m_wndTransfer.m_ctlUploadList.SortInit(m_pPrefs->GetUploadSortCol());
			g_App.m_pMDlg->m_wndTransfer.m_ctlQueueList.SortInit(m_pPrefs->GetQueueSortCol());
			g_App.m_pMDlg->m_wndTransfer.m_ctlClientList.SortInit(m_pPrefs->GetClientListSortCol());
			g_App.m_pMDlg->m_dlgSearch.m_ctlSearchList.SortInit(m_pPrefs->GetSearchSortCol());
			g_App.m_pMDlg->m_wndIRC.SortInit(m_pPrefs->GetIrcSortCol());
			g_App.m_pMDlg->m_wndSharedFiles.m_ctlSharedFilesList.SortInit(m_pPrefs->GetFileSortCol());
			SetModified(FALSE); //needs the settings for downloadlistcontrol
			g_App.m_pMDlg->m_wndTransfer.m_ctlDownloadList.SortInit(DL_OVERRIDESORT); //major override...
		}
	}

	return CPropertyPage::OnApply();
}

void CPPgSorting::Localize(void)
{
	if(::IsWindow(m_hWnd))
	{
		SetWindowText(GetResString(IDS_PW_SORTING));
		useItCheck.SetWindowText(GetResString(IDS_SORT_USEIT));
		pausedStoppedLastCheck.SetWindowText(GetResString(IDS_SORT_NONACTIVELAST));
		GetResString(&sortServersDesc, IDS_SORT_SERVERS_LBL);
		GetResString(&sortDownloadsDesc, IDS_SORT_DOWNLOADS_LBL);
		GetResString(&sortUploadsDesc, IDS_SORT_UPLOADS_LBL);
		GetResString(&sortQueueDesc, IDS_SORT_QUEUE_LBL);
		GetResString(&sortSearchDesc, IDS_SORT_SEARCH_LBL);
		GetResString(&sortFilesDesc, IDS_SORT_FILES_LBL);
		GetResString(&sortIrcDesc, IDS_SORT_IRC_LBL);
		GetResString(&sortClientListDesc, IDS_SORT_CLIENTLIST_LBL);

		GetResString(&sortGrpDesc, IDS_SORT_GRP);
		GetResString(&sortSources1Desc, IDS_SORT_SOURCES1_LBL);
		sortSources2Check.SetWindowText(GetResString(IDS_SORT_SOURCES2_LBL));

		LoadCombo();

		UpdateData(FALSE);
	}
}

void CPPgSorting::OnBnClickedSortSources2Box()
{
	UpdateData(TRUE);

	sourceCols2Combo.EnableWindow(bUseSourcesSorting);

	SetModified();
}

void CPPgSorting::OnBnClickedUseSort()
{
	UpdateData(TRUE);

	serverColsCombo.EnableWindow(bUseSorting);
	fileColsCombo.EnableWindow(bUseSorting);
	downloadColsCombo.EnableWindow(bUseSorting);
	uploadColsCombo.EnableWindow(bUseSorting);
	queueColsCombo.EnableWindow(bUseSorting);
	searchColsCombo.EnableWindow(bUseSorting);
	ircColsCombo.EnableWindow(bUseSorting);
	clientListColsCombo.EnableWindow(bUseSorting);
	sourceCols1Combo.EnableWindow(bUseSorting);
	sortSources2Check.EnableWindow(bUseSorting);
	sourceCols2Combo.EnableWindow((bUseSorting && bUseSourcesSorting) ? true : false);

	SetModified();
}

void CPPgSorting::LoadCombo(void)
{
	static const uint16 s_auServerCols[][2] =
	{
		{ IDS_SL_SERVERNAME,	SL_COLUMN_SERVERNAME },
		{ IDS_IP,				SL_COLUMN_SERVERIP },
		{ IDS_DESCRIPTION,		SL_COLUMN_DESCRIPTION },
		{ IDS_PING,				SL_COLUMN_PING },
		{ IDS_UUSERS,			SL_COLUMN_NUMUSERS },
		{ IDS_MAXUSERS,			SL_COLUMN_NUMUSERS | MLC_SORTALT },
		{ IDS_PW_FILES,			SL_COLUMN_NUMFILES },
		{ IDS_PRIORITY,			SL_COLUMN_PREFERENCES },
		{ IDS_UFAILED,			SL_COLUMN_FAILEDCOUNT },
		{ IDS_STATICSERVER,		SL_COLUMN_STATIC },
		{ IDS_SERVER_SOFTLIMIT,	SL_COLUMN_SOFTFILELIMIT },
		{ IDS_SERVER_HARDLIMIT,	SL_COLUMN_SOFTFILELIMIT | MLC_SORTALT },
		{ IDS_SERVER_VERSION,	SL_COLUMN_SOFTWAREVER },
		{ IDS_COUNTRY,			SL_COLUMN_COUNTRY },
		{ IDS_LOWIDUSERS,		SL_COLUMN_LOWIDUSERS }
	};
	static const uint16 s_auSharedFilesCols[][2] =
	{
		{ IDS_DL_FILENAME,		SFL_COLUMN_FILENAME },
		{ IDS_DL_SIZE,			SFL_COLUMN_FILESIZE },
		{ IDS_TYPE,				SFL_COLUMN_TYPE },
		{ IDS_PRIORITY,			SFL_COLUMN_PRIORITY },
		{ IDS_PERMISSION,		SFL_COLUMN_PERMISSION },
		{ IDS_FILEHASH,			SFL_COLUMN_FILEID },
		{ IDS_SF_REQUESTS,		SFL_COLUMN_REQUESTS },
		{ IDS_SF_REQUESTS,		SFL_COLUMN_REQUESTS | MLC_SORTALT },
		{ IDS_SF_ACCEPTS,		SFL_COLUMN_ACCEPTED },
		{ IDS_SF_ACCEPTS,		SFL_COLUMN_ACCEPTED | MLC_SORTALT },
		{ IDS_SF_TRANSFERRED,	SFL_COLUMN_TRANSFERRED },
		{ IDS_SF_TRANSFERRED,	SFL_COLUMN_TRANSFERRED | MLC_SORTALT },
		{ IDS_SF_PARTTRAFFIC,	SFL_COLUMN_PARTTRAFFIC },
		{ IDS_SF_PARTTRAFFIC,	SFL_COLUMN_PARTTRAFFIC | MLC_SORTALT },
		{ IDS_SF_COLUPLOADS,	SFL_COLUMN_UPLOADS },
		{ IDS_SF_COLUPLOADS,	SFL_COLUMN_UPLOADS | MLC_SORTALT },
		{ IDS_SF_COMPLETESRC,	SFL_COLUMN_COMPLETESRC },
		{ IDS_SF_COMPLETESRC,	SFL_COLUMN_COMPLETESRC | MLC_SORTALT },
		{ IDS_SF_FOLDER,		SFL_COLUMN_FOLDER }
	};
	static const uint16 s_auUploadCols[][2] =
	{
		{ IDS_QL_USERNAME,	ULCOL_USERNAME },
		{ IDS_FILE,			ULCOL_FILENAME },
		{ IDS_DL_SPEED,		ULCOL_SPEED },
		{ IDS_DL_TRANSF,	ULCOL_TRANSFERRED },
		{ IDS_WAITED,		ULCOL_WAITED },
		{ IDS_UPLOADTIME,	ULCOL_UPLOADTIME },
		{ IDS_STATUS,		ULCOL_STATUS },
		{ IDS_UP_PARTS,		ULCOL_PARTS },
		{ IDS_DL_PROGRESS,	ULCOL_PROGRESS },
		{ IDS_COMPRESSION,	ULCOL_COMPRESSION },
		{ IDS_COUNTRY,		ULCOL_COUNTRY }
	};
	static const uint16 s_auQueueCols[][2] =
	{
		{ IDS_QL_USERNAME,	QLCOL_USERNAME },
		{ IDS_FILE,			QLCOL_FILENAME },
		{ IDS_FILEPRIO,		QLCOL_FILEPRIORITY },
		{ IDS_UP_PARTS,		QLCOL_PARTS },
		{ IDS_DL_PROGRESS,	QLCOL_PROGRESS },
		{ IDS_RATING,		QLCOL_QLRATING },
		{ IDS_SCORE,		QLCOL_SCORE },
		{ IDS_SFRATIO,		QLCOL_SFRATIO },
		{ IDS_RFRATIO,		QLCOL_RFRATIO },
		{ IDS_ASKED,		QLCOL_TIMESASKED },
		{ IDS_LASTSEEN,		QLCOL_LASTSEEN },
		{ IDS_ENTERQUEUE,	QLCOL_ENTEREDQUEUE },
		{ IDS_BANNED,		QLCOL_BANNED },
		{ IDS_COUNTRY,		QLCOL_COUNTRY }
	};
	static const uint16 s_auSearchCols[][2] =
	{
		{ IDS_DL_FILENAME,		SL_COLUMN_FILENAME },
		{ IDS_DL_SIZE,			SL_COLUMN_SIZE },
		{ IDS_DL_SOURCES,		SL_COLUMN_SOURCES },
		{ IDS_DL_SOURCES,		SL_COLUMN_SOURCES | MLC_SORTALT },
		{ IDS_TYPE,				SL_COLUMN_TYPE },
		{ IDS_FILEHASH,			SL_COLUMN_FILEHASH },
		{ IDS_LASTSEENCOMPLETE,	SL_COLUMN_LASTSEENCOMPLETE },
		{ IDS_LENGTH,			SL_COLUMN_LENGTH },
		{ IDS_BITRATE,			SL_COLUMN_BITRATE },
		{ IDS_CODEC,			SL_COLUMN_CODEC }
	};
	static const uint16 s_auIrcChanCols[][2] =
	{
		{ IDS_IRC_NAME,		IRC2COL_NAME },
		{ IDS_UUSERS,		IRC2COL_USERS },
		{ IDS_DESCRIPTION,	IRC2COL_DESCRIPTION }
	};
	static const uint16 s_auClientCols[][2] =
	{
		{ IDS_QL_USERNAME,					CLCOL_USERNAME },
		{ IDS_CL_UPLOADSTATUS,				CLCOL_UPLOADSTATUS },
		{ IDS_CL_TRANSFUP,					CLCOL_TRANSFERREDUP },
		{ IDS_CL_DOWNLSTATUS,				CLCOL_DOWNLOADSTATUS },
		{ IDS_CL_TRANSFDOWN,				CLCOL_TRANSFERREDDOWN },
		{ IDS_INFLST_USER_CLIENTSOFTWARE,	CLCOL_CLIENTSOFTWARE },
		{ IDS_CONNECTED,					CLCOL_CONNECTEDTIME },
		{ IDS_INFLST_USER_USERHASH,			CLCOL_USERHASH },
		{ IDS_COUNTRY,						CLCOL_COUNTRY }
	};
	static const uint16 s_auDownloadCols[][2] =
	{
		{ IDS_DL_FILENAME,			DLCOL_FILENAME },
		{ IDS_DL_SIZE,				DLCOL_SIZE },
		{ IDS_DL_TRANSF,			DLCOL_TRANSFERRED },
		{ IDS_SF_COMPLETED,			DLCOL_COMPLETED },
		{ IDS_DL_SPEED,				DLCOL_SPEED },
		{ IDS_DL_PROGRESS,			DLCOL_PROGRESS },
		{ IDS_DL_SOURCES,			DLCOL_NUMSOURCES },
		{ IDS_PRIORITY,				DLCOL_PRIORITY },
		{ IDS_STATUS,				DLCOL_STATUS },
		{ IDS_DL_REMAINS,			DLCOL_REMAINING },
		{ IDS_DLCOL_REMAININGTIME,	DLCOL_REMAININGTIME },
		{ IDS_LASTSEENCOMPLETE,		DLCOL_LASTSEENCOMPLETE },
		{ IDS_LASTRECEPTION,		DLCOL_LASTRECEIVED },
		{ IDS_CAT,					DLCOL_CATEGORY },
		{ IDS_WAITED,				DLCOL_WAITED },
		{ IDS_DLCOL_AVGSPEED,		DLCOL_AVGSPEED },
		{ IDS_DLCOL_AVGREMTIME,		DLCOL_AVGREMTIME },
		{ IDS_DLCOL_ETA,			DLCOL_ETA },
		{ IDS_DLCOL_AVGETA,			DLCOL_AVGETA }
	};
	static const uint16 s_auDownloadSrcCols[][2] =
	{
		{ IDS_QL_USERNAME,					DLCOL_FILENAME },
		{ IDS_DL_TRANSF,					DLCOL_TRANSFERRED },
		{ IDS_SF_COMPLETED,					DLCOL_COMPLETED },
		{ IDS_DL_SPEED,						DLCOL_SPEED },
		{ IDS_DL_PROGRESS,					DLCOL_PROGRESS },
		{ IDS_INFLST_USER_CLIENTSOFTWARE,	DLCOL_NUMSOURCES },
		{ 0,								DLCOL_PRIORITY },	//QR
		{ IDS_STATUS,						DLCOL_STATUS },
		{ IDS_DL_REMAINS,					DLCOL_REMAINING },
		{ IDS_DLCOL_REMAININGTIME,			DLCOL_REMAININGTIME },
		{ IDS_DL_ULDL,						DLCOL_ULDLRATIO },
		{ IDS_RATING,						DLCOL_QLRATING }
	};
	CString	strResStr, strAux;
	int		iRc, iCurSel;
	uint32	dwCode;

	serverColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auServerCols); ui++)
	{
		GetResString(&strResStr, s_auServerCols[ui][0]);
		strAux = strResStr;
		if ((s_auServerCols[ui][1] & MLC_SORTALT) != 0)
		{
			strAux += _T("[^^]");
			strResStr += _T("[vv]");
		}
		else
			strAux += _T("[^]");

		serverColsCombo.SetItemData(iRc = serverColsCombo.AddString(strAux), dwCode = s_auServerCols[ui][1]);
		if (dwCode == m_pPrefs->GetServerSortCol())
			iCurSel = iRc;
		serverColsCombo.SetItemData(iRc = serverColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetServerSortCol())
			iCurSel = iRc;
	}
	serverColsCombo.SetCurSel(iCurSel);

	fileColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auSharedFilesCols); ui++)
	{
		GetResString(&strResStr, s_auSharedFilesCols[ui][0]);
		strAux = strResStr;
		if ((s_auSharedFilesCols[ui][1] & MLC_SORTALT) != 0)
		{
			strAux += _T("[^^]");
			strResStr += _T("[vv]");
		}
		else
			strAux += _T("[^]");

		fileColsCombo.SetItemData(iRc = fileColsCombo.AddString(strAux), dwCode = s_auSharedFilesCols[ui][1]);
		if (dwCode == m_pPrefs->GetFileSortCol())
			iCurSel = iRc;
		fileColsCombo.SetItemData(iRc = fileColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetFileSortCol())
			iCurSel = iRc;
	}
	fileColsCombo.SetCurSel(iCurSel);

	uploadColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auUploadCols); ui++)
	{
		GetResString(&strResStr, s_auUploadCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		uploadColsCombo.SetItemData(iRc = uploadColsCombo.AddString(strAux), dwCode = s_auUploadCols[ui][1]);
		if (dwCode == m_pPrefs->GetUploadSortCol())
			iCurSel = iRc;
		uploadColsCombo.SetItemData(iRc = uploadColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetUploadSortCol())
			iCurSel = iRc;
	}
	uploadColsCombo.SetCurSel(iCurSel);

	queueColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auQueueCols); ui++)
	{
		GetResString(&strResStr, s_auQueueCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		queueColsCombo.SetItemData(iRc = queueColsCombo.AddString(strAux), dwCode = s_auQueueCols[ui][1]);
		if (dwCode == m_pPrefs->GetQueueSortCol())
			iCurSel = iRc;
		queueColsCombo.SetItemData(iRc = queueColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetQueueSortCol())
			iCurSel = iRc;
	}
	queueColsCombo.SetCurSel(iCurSel);

	searchColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auSearchCols); ui++)
	{
		GetResString(&strResStr, s_auSearchCols[ui][0]);
		strAux = strResStr;
		if ((s_auSearchCols[ui][1] & MLC_SORTALT) != 0)
		{
			strAux += _T("[^^]");
			strResStr += _T("[vv]");
		}
		else
			strAux += _T("[^]");

		searchColsCombo.SetItemData(iRc = searchColsCombo.AddString(strAux), dwCode = s_auSearchCols[ui][1]);
		if (dwCode == m_pPrefs->GetSearchSortCol())
			iCurSel = iRc;
		searchColsCombo.SetItemData(iRc = searchColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetSearchSortCol())
			iCurSel = iRc;
	}
	searchColsCombo.SetCurSel(iCurSel);

	ircColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auIrcChanCols); ui++)
	{
		GetResString(&strResStr, s_auIrcChanCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		ircColsCombo.SetItemData(iRc = ircColsCombo.AddString(strAux), dwCode = s_auIrcChanCols[ui][1]);
		if (dwCode == m_pPrefs->GetIrcSortCol())
			iCurSel = iRc;
		ircColsCombo.SetItemData(iRc = ircColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetIrcSortCol())
			iCurSel = iRc;
	}
	ircColsCombo.SetCurSel(iCurSel);

	clientListColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auClientCols); ui++)
	{
		GetResString(&strResStr, s_auClientCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		clientListColsCombo.SetItemData(iRc = clientListColsCombo.AddString(strAux), dwCode = s_auClientCols[ui][1]);
		if (dwCode == m_pPrefs->GetClientListSortCol())
			iCurSel = iRc;
		clientListColsCombo.SetItemData(iRc = clientListColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetClientListSortCol())
			iCurSel = iRc;
	}
	clientListColsCombo.SetCurSel(iCurSel);

	downloadColsCombo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auDownloadCols); ui++)
	{
		GetResString(&strResStr, s_auDownloadCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		downloadColsCombo.SetItemData(iRc = downloadColsCombo.AddString(strAux), dwCode = s_auDownloadCols[ui][1]);
		if (dwCode == m_pPrefs->GetDownloadSortCol())
			iCurSel = iRc;
		downloadColsCombo.SetItemData(iRc = downloadColsCombo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetDownloadSortCol())
			iCurSel = iRc;
	}
	downloadColsCombo.SetCurSel(iCurSel);

	sourceCols1Combo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auDownloadSrcCols); ui++)
	{
		if (s_auDownloadSrcCols[ui][1] == DLCOL_PRIORITY)
			strResStr = _T("QR");
		else
			GetResString(&strResStr, s_auDownloadSrcCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		sourceCols1Combo.SetItemData(iRc = sourceCols1Combo.AddString(strAux), dwCode = s_auDownloadSrcCols[ui][1]);
		if (dwCode == m_pPrefs->GetSrcSortCol1())
			iCurSel = iRc;
		sourceCols1Combo.SetItemData(iRc = sourceCols1Combo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetSrcSortCol1())
			iCurSel = iRc;
	}
	sourceCols1Combo.SetCurSel(iCurSel);

	sourceCols2Combo.ResetContent();
	iCurSel = 0;
	for (unsigned ui = 0; ui < ELEMENT_COUNT(s_auDownloadSrcCols); ui++)
	{
		if (s_auDownloadSrcCols[ui][1] == DLCOL_PRIORITY)
			strResStr = _T("QR");
		else
			GetResString(&strResStr, s_auDownloadSrcCols[ui][0]);
		strAux = strResStr;
		strAux += _T("[^]");

		sourceCols2Combo.SetItemData(iRc = sourceCols2Combo.AddString(strAux), dwCode = s_auDownloadSrcCols[ui][1]);
		if (dwCode == m_pPrefs->GetSrcSortCol2())
			iCurSel = iRc;
		sourceCols2Combo.SetItemData(iRc = sourceCols2Combo.AddString(strResStr), dwCode |= MLC_SORTDESC);
		if (dwCode == m_pPrefs->GetSrcSortCol2())
			iCurSel = iRc;
	}
	sourceCols2Combo.SetCurSel(iCurSel);
}
