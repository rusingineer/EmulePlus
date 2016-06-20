// PPgStats.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgStats.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CPPgStats dialog

IMPLEMENT_DYNAMIC(CPPgStats, CPropertyPage)
CPPgStats::CPPgStats()
	: CPropertyPage(CPPgStats::IDD)
	, graphsUpdateInterval(0)
	, averageGraphTime(0)
	, statisticsUpdateInterval(0)
{
}

CPPgStats::~CPPgStats()
{
}

void CPPgStats::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPPgStats)
	DDX_Control(pDX, IDC_COLOR_BUTTON, colorButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COLORSELECTOR, colorsCombo);
	DDX_Control(pDX, IDC_RATIO, ratioCombo);
	DDX_Text(pDX, IDC_ACSTAT, acStat);
	DDX_Control(pDX, IDC_SLIDER, updateIntervalSlider);
	DDX_Control(pDX, IDC_SLIDER3, averageGraphTimeSlider);
	DDX_Control(pDX, IDC_SLIDER2, statisticsUpdateIntervalSlider);
	DDX_Slider(pDX, IDC_SLIDER, graphsUpdateInterval);
	DDX_Slider(pDX, IDC_SLIDER3, averageGraphTime);
	DDX_Slider(pDX, IDC_SLIDER2, statisticsUpdateInterval);
	DDX_Text(pDX, IDC_SLIDERINFO, graphUpdateIntervalDesc);
	DDX_Text(pDX, IDC_SLIDERINFO2, statisticsUpdateIntervalDesc);
	DDX_Text(pDX, IDC_SLIDERINFO3, avgGraphDesc);
	DDX_Text(pDX, IDC_STREE, streeDesc);
	DDX_Text(pDX, IDC_ACSTAT_LBL, acStatDesc);
	DDX_Text(pDX, IDC_ACRATIO_LBL, acRatioDesc);
	DDX_Text(pDX, IDC_PREFCOLORS, prefColorsDesc);
	DDX_Text(pDX, IDC_GRAPHS, graphsDesc);
}


BEGIN_MESSAGE_MAP(CPPgStats, CPropertyPage)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_ACSTAT, OnEnChangeACStats)
	ON_CBN_SELCHANGE(IDC_COLORSELECTOR, OnCbnSelchangeColorselector)
	ON_MESSAGE(CPN_SELCHANGE, OnSelChange)
	ON_BN_CLICKED(IDC_COLOR_BUTTON, OnBnClickedColorButton)
	ON_CBN_SELCHANGE(IDC_RATIO, OnCbnSelchangeRatio)
END_MESSAGE_MAP()


BOOL CPPgStats::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	for (int i = 0; i < 12; i++) {
		statcolors[i] = g_App.m_pPrefs->GetStatsColor(i);
	}

	updateIntervalSlider.SetRange(0,120,false);
	statisticsUpdateIntervalSlider.SetRange(4,120,false);
	averageGraphTimeSlider.SetRange(1,60,false);

	graphsUpdateInterval = m_pPrefs->GetTrafficOMeterInterval();
	statisticsUpdateInterval = m_pPrefs->GetStatsInterval();
	averageGraphTime = m_pPrefs->GetStatsAverageMinutes();
	
	acStat.Format(_T("%u"), m_pPrefs->GetStatsMax());
	
	ratioCombo.AddString(_T("1:1"));
	ratioCombo.AddString(_T("1:2"));
	ratioCombo.AddString(_T("1:3"));
	ratioCombo.AddString(_T("1:4"));
	ratioCombo.AddString(_T("1:5"));
	ratioCombo.AddString(_T("1:10"));
	ratioCombo.AddString(_T("%"));
	int n = m_pPrefs->GetGraphRatio();
	if (n <= 5)
		ratioCombo.SetCurSel(n - 1);
	else if (n == 10)
		ratioCombo.SetCurSel(5); // item 5 = 1:10
	else
		ratioCombo.SetCurSel(6); // item 6 = %
	colorButton.TrackSelection	= TRUE;
	Localize();
	UpdateData(FALSE);
	SetModified(FALSE);

	return TRUE;
}

BOOL CPPgStats::OnApply()
{
	if(m_bModified)
	{
		UpdateData(TRUE);

		for (int i = 0; i < 12; i++)
		{
			g_App.m_pPrefs->SetStatsColor(i, statcolors[i]);
		}
		g_App.m_pPrefs->SetTrafficOMeterInterval(graphsUpdateInterval);
		g_App.m_pPrefs->SetStatsInterval(statisticsUpdateInterval);
		g_App.m_pPrefs->SetStatsAverageMinutes(averageGraphTime);
		int n = ratioCombo.GetCurSel();
		m_pPrefs->SetGraphRatio((n == 6) ? 255 : ((n == 5) ? 10 : n + 1));
		g_App.m_pMDlg->m_dlgStatistics.Localize();
		g_App.m_pMDlg->m_dlgStatistics.ShowInterval();

		uint16		uActiveConnScale = static_cast<uint16>(_tstoi(acStat));

		if(uActiveConnScale > m_pPrefs->GetMaxConnections())
		{
			uActiveConnScale = m_pPrefs->GetMaxConnections();
			acStat.Format(_T("%u"), uActiveConnScale);
		}
		m_pPrefs->SetStatsMax(uActiveConnScale);

		UpdateData(FALSE);

		g_App.m_pMDlg->m_dlgStatistics.UpdateActConScale();
		g_App.m_pMDlg->m_dlgStatistics.RepaintMeters();

		SetModified(FALSE);
	}

	return CPropertyPage::OnApply();
}

void CPPgStats::Localize(void)
{
	static const UINT s_adwResTbl[] =
	{
		IDS_SP_BACKGROUND, IDS_SP_GRID,
		IDS_SP_DL1, IDS_SP_DL2, IDS_SP_DL3,
		IDS_SP_UL1, IDS_SP_UL2, IDS_SP_UL3,
		IDS_SP_ACTCON, IDS_SP_ACTUL, IDS_SP_ACTDL
	};

	if(::IsWindow(m_hWnd))
	{
		GetResString(&graphsDesc, IDS_GRAPHS);
		GetResString(&streeDesc, IDS_STREE);
		GetResString(&acStatDesc, IDS_ACSTAT_LBL);
		GetResString(&acRatioDesc, IDS_ACRATIO_LBL);
		SetWindowText(GetResString(IDS_STATISTICS));

		GetResString(&prefColorsDesc, IDS_COLORS);

		colorsCombo.ResetContent();
		for (unsigned ui = 0; ui < ELEMENT_COUNT(s_adwResTbl); ui++)
			colorsCombo.AddString(GetResString(s_adwResTbl[ui]));
		colorsCombo.SetCurSel(0);

		UpdateData(FALSE);

		ShowInterval();
		colorButton.CustomText = GetResString(IDS_COL_MORECOLORS);
		colorButton.DefaultText = GetResString(IDS_COL_AUTOMATIC);
		OnCbnSelchangeColorselector();
	}
}

void CPPgStats::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetModified(TRUE);
	UpdateData(TRUE);

	ShowInterval();
	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPPgStats::ShowInterval()
{
	if (graphsUpdateInterval == 0)
		GetResString(&graphUpdateIntervalDesc, IDS_DISABLED);
	else
		graphUpdateIntervalDesc.Format(GetResString(IDS_STATS_UPDATELABEL), graphsUpdateInterval);
	
	statisticsUpdateIntervalDesc.Format(GetResString(IDS_STATS_UPDATELABEL), statisticsUpdateInterval);
	avgGraphDesc.Format(GetResString(IDS_STATS_AVGLABEL), averageGraphTime);

	UpdateData(FALSE);
}

void CPPgStats::OnCbnSelchangeColorselector()
{
	int	iIdx = colorsCombo.GetCurSel();

	colorButton.SetColor((COLORREF)statcolors[iIdx]);
	colorButton.SetDefaultColor(m_pPrefs->GetDefaultStatsColor(iIdx));
}

LONG CPPgStats::OnSelChange(UINT newColor, LONG /*wParam*/)
{
	statcolors[colorsCombo.GetCurSel()] = (COLORREF)newColor;
	SetModified(TRUE);
	return TRUE;
}
