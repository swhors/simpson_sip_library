
// XConManagerDlg.h : header file
//

#pragma once
#include "afxwin.h"

class CSIPMain;

#if 1 //muk test
#include <list>
typedef struct _st_callinfo{
    char szCallid[128]; // dialog callid
	char szTargetNum[128]; // cid
	void *pHd;
} st_callinfo,*p_st_callinfo;
#endif

// CXConManagerDlg dialog
class CXConManagerDlg : public CDialog
{
// Construction
public:
	CXConManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_XCONMANAGER_DIALOG };

	protected:
	virtual		void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON		m_hIcon;

	CSIPMain	*m_pManager;
	BOOL		m_bRequestStackEnd;

	// Generated message map functions
	virtual		BOOL OnInitDialog();
	afx_msg		void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg		void OnPaint();
	afx_msg		HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg		void OnToolStart();
	afx_msg		void OnToolStop();
	afx_msg		void OnHelpAbout();
	afx_msg		void OnToolSe();
	afx_msg		void OnFileExit();
	afx_msg		void OnUpdateToolStart(CCmdUI *pCmdUI);
	afx_msg		void OnUpdateToolStop(CCmdUI *pCmdUI);
	afx_msg		void OnTestInvite();
	afx_msg		void OnTestRegister();
	afx_msg		void OnTestUnregister();
	afx_msg		void OnTestBye();
	afx_msg		LRESULT	EventRegister(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventEndSipStack(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventConnectionSuccess(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventConnectionRequest(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventConnectionFail(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventDisConnection(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventGroupCall(WPARAM wParam,LPARAM lParam);
	CListBox	m_listLog;
	CString		cs_TargetNum;
	afx_msg		void OnTestCancel();
	afx_msg		void OnUpdateTestInvite(CCmdUI *pCmdUI);
	void		MenuItemEnable(DWORD dwID, BOOL bEnable=TRUE);
	void		OperateMenuSet(BOOL bRun = TRUE);
	void		PrintLog(char *szChar, char *szchar1, int iValue);
	afx_msg void OnToolCallgroup();
	afx_msg void OnTestMemorytest();
	afx_msg void OnTestInfo();
	afx_msg void OnTestInvite32786();
	afx_msg void OnTestMemorytest2();
#if 1 //muk test 
	char		m_szMSCallID[512];
	char		m_szMSMyNum[128];
	void		MSCreateCreateConference();
	BOOL		m_bConfRoomOK;
	list<st_callinfo>	m_listCallinfo;
#endif

#if 1 //muk test
	afx_msg		LRESULT EventSendAck(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventRecvInfoOk(WPARAM wParam,LPARAM lParam);
	afx_msg		LRESULT EventBeforeSendInvite(WPARAM wParam,LPARAM lParam);
#endif
};
