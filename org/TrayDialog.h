
#pragma once
#include "DialogMinTrayBtn.h"
#include "ResizableLib\ResizableDialog.h"
#include "types.h"
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)


class CTrayDialog : public CDialogMinTrayBtn<CResizableDialog>
{
	// Construction
protected:
    typedef CDialogMinTrayBtn<CResizableDialog> CTrayDialogBase;
public:
	void TraySetMinimizeToTray(byte *bMinimizeToTray);
	bool TraySetMenu(UINT nResourceID,UINT nDefaultPos=0);	
	bool TraySetMenu(HMENU hMenu,UINT nDefaultPos=0);	
	bool TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos=0);	
	bool TrayUpdate();
	bool TrayShow();
	bool TrayHide();
	void TraySetToolTip(LPCTSTR lpszToolTip);
	void TraySetIcon(HICON hIcon, bool bDelete= false);
	/* eklmn: removed unused functions
	void TraySetIcon(UINT nResourceID, bool bDelete= false);
	void TraySetIcon(LPCTSTR lpszResourceName, bool bDelete= false);
	*/
	void TrayMinimizeToTrayChanged();

	bool TrayIsVisible();
	CTrayDialog(UINT uIDD,CWnd* pParent = NULL);   // standard constructor

	virtual void OnTrayLButtonDown(CPoint pt);
	virtual void OnTrayLButtonDblClk(CPoint pt);
	
	virtual void OnTrayRButtonUp(CPoint pt);
	virtual void OnTrayRButtonDblClk(CPoint pt);

	virtual void OnTrayMouseMove(CPoint pt);

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);	
	
	DECLARE_MESSAGE_MAP()	

private:
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);

	byte			*m_bMinimizeToTray;
	bool			m_bCurIconDelete;
	HICON			m_hPrevIconDelete;
	bool			m_bdoubleclicked;
	bool			m_bTrayIconVisible;
	NOTIFYICONDATA	m_nidIconData;
	CMenu			m_mnuTrayMenu;
	UINT			m_nDefaultMenuItem;
};