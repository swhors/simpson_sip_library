// XConManagerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;
#include "skysipua_g.h"
#include "skysip.h"
#include "sip_parse.h"
#include "SipCallStack.h"

#include "skysock.h"
#include "SkyAsyncSock.h"
#include "StunClient.h"
#include "sipuac.h"
#include "SkySipUaManager.h"
#include "SIPMain.h"

#include "sky_rtp.h"

#include "DlgGroupList.h"

#include "XConManager.h"
#include "InputDialog.h"
#include "XConManagerDlg.h"

// Memory Leak detect
//#include "vld.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UM_END_SIPSTACK WM_USER+2500

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

END_MESSAGE_MAP()

// CXConManagerDlg dialog


CXConManagerDlg::CXConManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXConManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXConManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTLOG, m_listLog);
}

BEGIN_MESSAGE_MAP(CXConManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TOOL_START, &CXConManagerDlg::OnToolStart)
	ON_COMMAND(ID_TOOL_STOP, &CXConManagerDlg::OnToolStop)
	ON_COMMAND(ID_HELP_ABOUT, &CXConManagerDlg::OnHelpAbout)
	ON_COMMAND(ID_TOOL_SE, &CXConManagerDlg::OnToolSe)
	ON_COMMAND(ID_FILE_EXIT, &CXConManagerDlg::OnFileExit)
	ON_UPDATE_COMMAND_UI(ID_TOOL_START, &CXConManagerDlg::OnUpdateToolStart)
	ON_UPDATE_COMMAND_UI(ID_TOOL_STOP, &CXConManagerDlg::OnUpdateToolStop)
	ON_COMMAND(ID_TEST_INVITE, &CXConManagerDlg::OnTestInvite)
	ON_COMMAND(ID_TEST_REGISTER, &CXConManagerDlg::OnTestRegister)
	ON_COMMAND(ID_TEST_UNREGISTER, &CXConManagerDlg::OnTestUnregister)
	ON_COMMAND(ID_TEST_BYE, &CXConManagerDlg::OnTestBye)
	ON_MESSAGE(WM_SIPMSG_SD_REG_SUCC,	EventRegister)
	ON_MESSAGE(WM_SIPMSG_SD_REG_FAIL,	EventRegister)
	ON_MESSAGE(WM_SIPMSG_RV_INV_SUCC,	EventConnectionSuccess)
	ON_MESSAGE(WM_SIPMSG_SD_INV,		EventConnectionRequest)
	ON_MESSAGE(WM_SIPMSG_RV_INV_FAIL,	EventConnectionFail)
	ON_MESSAGE(WM_SIPMSG_SD_INV_FAIL,	EventConnectionFail)
	ON_MESSAGE(WM_SIPMSG_RV_BYE,		EventDisConnection)
	ON_MESSAGE(WM_SIPMSG_SD_BYE,		EventDisConnection)
	ON_MESSAGE(UM_END_SIPSTACK,			EventEndSipStack)
	ON_MESSAGE(WM_USER_LIST_INVITE,		EventGroupCall)
#if 1 //muk test
	ON_MESSAGE(WM_SIPMSG_SENDACK,		EventSendAck)
	ON_MESSAGE(WM_SIPMSG_RECVINFO_OK,	EventRecvInfoOk)
	ON_MESSAGE(WM_SIPMSG_BEFORESEND_INVITE,	EventBeforeSendInvite)
#endif
	ON_COMMAND(ID_TEST_CANCEL,			&CXConManagerDlg::OnTestCancel)
	ON_UPDATE_COMMAND_UI(ID_TEST_INVITE, &CXConManagerDlg::OnUpdateTestInvite)
	ON_COMMAND(ID_TOOL_CALLGROUP, &CXConManagerDlg::OnToolCallgroup)
	ON_COMMAND(ID_TEST_MEMORYTEST, &CXConManagerDlg::OnTestMemorytest)
	ON_COMMAND(ID_TEST_INFO, &CXConManagerDlg::OnTestInfo)
	ON_COMMAND(ID_TEST_INVITE32786, &CXConManagerDlg::OnTestInvite32786)
	ON_COMMAND(ID_TEST_MEMORYTEST2, &CXConManagerDlg::OnTestMemorytest2)
END_MESSAGE_MAP()

// CXConManagerDlg message handlers

BOOL CXConManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pManager = NULL;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
		CMenu *subMenu1 = pSysMenu->GetSubMenu(1);
		if( subMenu1 )
		{
			subMenu1->EnableMenuItem(ID_TOOL_STOP, 0);
		}

	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_bRequestStackEnd = FALSE;
	OperateMenuSet(FALSE);
	// TODO: Add extra initialization here
#if 1 //muk test Callid 관리 위해
	memset(m_szMSCallID,0,512);
	m_bConfRoomOK = FALSE;
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXConManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXConManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXConManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CXConManagerDlg::OnToolStart()
{
	if( !this->m_pManager )
	{
		this->m_pManager = new CSIPMain();
		if( this->m_pManager )
		{
			m_bRequestStackEnd = FALSE;
			wsprintf(m_pManager->proxyip, "%s", "211.115.66.98");
			m_pManager->proxyport = 5060;
			wsprintf(m_pManager->callid, "%s", "77777");
			wsprintf(m_pManager->localip, "%s", "125.131.77.154");

			m_pManager->LoadConf();

			if( start_rtp_manager(10, 10000) )
			{
				SetEchoEnable(TRUE);

				if( !this->m_pManager->start_manager(0) )
				{
					this->m_listLog.AddString("스택 생성 에러");
					delete m_pManager;
					m_pManager = NULL;
					//stop_rtp_manager();
				}
				OperateMenuSet();
			}
			else
			{
				stop_rtp_manager();
				delete m_pManager;
				m_pManager = NULL;
			}
		}
	}
}

void CXConManagerDlg::OnToolStop()
{
	if( this->m_pManager )
	{
		m_bRequestStackEnd = TRUE;
		this->m_pManager->send_register(0);
		//stop_rtp_manager();
	}
}

void CXConManagerDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CXConManagerDlg::OnToolSe()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CXConManagerDlg::OnFileExit()
{
	exit(0);
}

void CXConManagerDlg::OnUpdateToolStart(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable(this->m_pManager?false:true);
}

void CXConManagerDlg::OnUpdateToolStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(this->m_pManager?true:false);
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CXConManagerDlg::OnTestInvite()
{
	if( this->m_pManager )
	{
		CInputDialog dlg;
		if( dlg.DoModal() == IDOK )
		{
			cs_TargetNum = dlg.m_strUInputVar;
			int iPort = getEmptyChannel((char*)cs_TargetNum.GetString());
			if( iPort > 0 )
			{
				//this->m_pManager->send_invite((char*)"90164524816", NULL, iPort);
				//this->m_pManager->send_invite((char*)"90230163643", NULL, iPort);
				this->m_pManager->send_invite((char*)cs_TargetNum.GetString(), NULL, iPort);
			}
		}
	}
}

void CXConManagerDlg::OnTestRegister()
{
	if( this->m_pManager )
	{
		m_pManager->send_register(360);
	}
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CXConManagerDlg::OnTestUnregister()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CXConManagerDlg::OnTestBye()
{
	if( this->m_pManager )
	{
		CInputDialog dlg;
		if( dlg.DoModal() == IDOK )
		{
			cs_TargetNum = dlg.m_strUInputVar;
			this->m_pManager->send_bye((char*)cs_TargetNum.GetString());
		}
	}
}

void CXConManagerDlg::PrintLog(char *szChar, char *szChar1, int iValue)
{
#if 1
	char tbuf[1024];

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	
	wsprintf(tbuf, "%02d:%02d:%02d %s %s%s%d%s.", 
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
		szChar, 
		(szChar1?szChar1:""),
		(szChar1?"[":""),
		iValue,
		(szChar1?"]":"")
		);

	if( m_listLog.GetCount() == 12 )
		m_listLog.DeleteString(0);
	this->m_listLog.AddString(tbuf);
#endif
}

LRESULT CXConManagerDlg::EventDisConnection(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		tr1::shared_ptr<CONNECTIONINFO> pCon((CONNECTIONINFO*)wParam);
		//CONNECTIONINFO * pCon = (CONNECTIONINFO*)wParam;
		reset_rtp_info_byTarget(pCon->szTarget);
		PrintLog("호 연결 해제", pCon->szTarget, pCon->iStateCode);
		//delete pCon;
		return NULL;
	}
	PrintLog("호 연결 해제", NULL, 1);

	return NULL;
}

LRESULT CXConManagerDlg::EventConnectionFail(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		//CONNECTIONINFO * pCon = (CONNECTIONINFO*)wParam;
		tr1::shared_ptr<CONNECTIONINFO> pCon((CONNECTIONINFO*)wParam);
		reset_rtp_info_byTarget(pCon->szTarget);
		PrintLog("호 연결 실패", pCon->szTarget, pCon->iStateCode);
		//delete pCon;
		return NULL;
	}
	PrintLog("호 연결 실패", NULL, 1);
	return NULL;
}

LRESULT CXConManagerDlg::EventConnectionRequest(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		//CONNECTIONINFO * pCon = (CONNECTIONINFO*)wParam;
		tr1::shared_ptr<CONNECTIONINFO> pCon((CONNECTIONINFO*)wParam);
		set_rtp_in_byTarget(pCon->szRtpIP, pCon->szRtpPort, pCon->szTarget);
		PrintLog("호 연결 요청", pCon->szTarget, 1);
		//delete pCon;
#if 1 //muk test Callid 관리 위해
		if (lParam != NULL)
		{
			char *pCallID = NULL;
			sip_hd_t *pHd = NULL;

			pHd = (sip_hd_t *)lParam;
			pCallID = (char *)pHd->sipfield.cid.callid.c_str();

			if (pHd && pCallID)
			{
				if (strcmp(pCon->szTarget, "msml") == 0)
				{
					if (!m_bConfRoomOK)
					{
						strcpy(m_szMSCallID, pCallID);
						strcpy(m_szMSMyNum, (char *)pHd->sipfield.from.uri.uid.c_str());
					}
				}
				else
				{
					st_callinfo callinfo;

					strcpy(callinfo.szCallid, (char *)pHd->sipfield.cid.callid.c_str());
					strcpy(callinfo.szTargetNum, pCon->szTarget);
					callinfo.pHd = (void *)pHd;
					
					m_listCallinfo.push_back(callinfo);
				}
			}
			/*
			char *pCallID = NULL;
			pCallID = (char *)lParam;

			//if (strcmp(pCon->szTarget, "msml") == 0 && !strlen(m_szMSCallID))
			if (strcmp(pCon->szTarget, "msml") == 0)
			{
				strcpy(m_szMSCallID, pCallID);
			}
			*/
		}
#endif
		return NULL;
	}
	PrintLog("호 연결 성공", NULL, 1);

	return NULL;
}


LRESULT CXConManagerDlg::EventConnectionSuccess(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		CONNECTIONINFO * pCon = (CONNECTIONINFO*)wParam;
		set_rtp_in_byTarget(pCon->szRtpIP, pCon->szRtpPort, pCon->szTarget);
		PrintLog("호 연결 성공", pCon->szTarget, 1);
#if 0 // muk test
		char *pCallID;
		pCallID = (char *)lParam;

		if (strcmp(m_szMSCallID, pCallID) == 0)
		{
			MSCreateCreateConference();
		}
#endif
#if 1 //muk test
		// ms 로부터 받은 응답이 아닌 경우
		if ( strcmp(pCon->szTarget, "msml") != 0 )
		{
			this->m_pManager->send_invite(pCon->szTarget, pCon->szRtpIP, pCon->szRtpPort, "211.115.66.94");
		}
		//*
		else
		{
			sip_hd_t *pHd = NULL;
			pHd = (sip_hd_t *)lParam;

			if (pHd)
			{
				if ( strcmp((char *)pHd->sipfield.to.uri.uid.c_str(), "msml") == 0 )
				{
					this->m_pManager->send_ack((char *)pHd->sipfield.cid.callid.c_str());
					//this->m_pManager->send_ack(pHd);
				}

				char *pFrom = NULL;

				pFrom = (char *)pHd->sipfield.from.uri.uid.c_str();
				if (pFrom)
				{
					if ( strcmp(pFrom, m_szMSMyNum) != 0 )
					{
						list <st_callinfo>::const_iterator itr = m_listCallinfo.begin();

						for (; itr != m_listCallinfo.end() ; itr++)
						{
							if ( strcmp(itr->szTargetNum, pFrom) == 0 )
							{
								//this->m_pManager->send_ack_withSDP((char *)itr->szCallid);
								this->m_pManager->send_ack_withSDP((sip_hd_t*)itr->pHd);
								break;
							}
						}	
						//send_ack_withSDP((char *)pHd->sipfield.cid.callid.c_str());
					}
				}
			}
		}
		//*/
#endif
		delete pCon;
		return NULL;
	}
	PrintLog("호 연결 성공", NULL, 1);

	return NULL;
}
LRESULT	CXConManagerDlg::EventRegister(WPARAM wParam,LPARAM lParam)
{
	PrintLog( (wParam == 1?(lParam==0?"CCS에 등록되었습니다.":"CCS에 등록이 해제 되었습니다."):"CCS 등록에 실패하였습니다."), NULL, 1);
	if( wParam == 1 && lParam == 1 && m_bRequestStackEnd == TRUE) 
	{
		PostMessage(UM_END_SIPSTACK, 0, 0 );
	}
	return NULL;
}

LRESULT CXConManagerDlg::EventEndSipStack(WPARAM wParam,LPARAM lParam)
{
	this->m_pManager->end_manager(0);
	Sleep(800);
	delete m_pManager;
	m_pManager = NULL;
	OperateMenuSet(FALSE);
	return NULL;
}

void CXConManagerDlg::OnTestCancel()
{
	if( this->m_pManager )
	{
		CInputDialog dlg;
		if( dlg.DoModal() == IDOK )
		{
			cs_TargetNum = dlg.m_strUInputVar;
			this->m_pManager->send_cancel((char*)cs_TargetNum.GetString());
		}
	}
}

void CXConManagerDlg::OnUpdateTestInvite(CCmdUI *pCmdUI)
{
}

void CXConManagerDlg::OperateMenuSet(BOOL bRun)
{
	MenuItemEnable(ID_FILE_EXIT, !bRun);
	MenuItemEnable(ID_TOOL_START, !bRun);
	MenuItemEnable(ID_TOOL_SE, !bRun);
	MenuItemEnable(ID_TOOL_STOP, bRun);
	MenuItemEnable(ID_TEST_INVITE, bRun);
	MenuItemEnable(ID_TEST_REGISTER, bRun);
	MenuItemEnable(ID_TEST_UNREGISTER, bRun);
	MenuItemEnable(ID_TEST_BYE, bRun);
	MenuItemEnable(ID_TEST_CANCEL, bRun);
}

void CXConManagerDlg::MenuItemEnable(DWORD dwID, BOOL bEnable)
{
	int subMenuNum = 0;

	switch(dwID)
	{
	case ID_TOOL_STOP:
	case ID_TOOL_START:
	case ID_TOOL_SE:
		subMenuNum = 1;
		break;
	case ID_TEST_INVITE:
	case ID_TEST_REGISTER:
	case ID_TEST_UNREGISTER:
	case ID_TEST_BYE:
	case ID_TEST_CANCEL:
		subMenuNum = 2;
		break;
	case ID_HELP_ABOUT:
		subMenuNum = 3;
	default :
		subMenuNum = 0;
	}
	CMenu *pMenu = GetMenu();
	if( pMenu )
	{
		CMenu *pSub1 = pMenu->GetSubMenu(subMenuNum);
		if( pSub1 )
		{
			pSub1->EnableMenuItem(dwID, (bEnable?MF_ENABLED:MF_DISABLED|MF_GRAYED));
		}
	}
}

void CXConManagerDlg::OnToolCallgroup()
{
	CDlgGroupList pDlg;
	if( this->m_pManager )
		pDlg.m_bMainRun = this->m_pManager->bRun;
	if( pDlg.DoModal() == IDOK )
	{

	}
}


LRESULT CXConManagerDlg::EventGroupCall(WPARAM wParam,LPARAM lParam)
{
	if( wParam )
	{
#if 1 //muk test MS로 보내는 경우
		if (lParam)
		{
			char *szCalled = (char *)wParam;
			char *szTemp = (char *)lParam;
			if( this->m_pManager )
			{
				if( this->m_pManager->bRun )
				{
					int iPort = getEmptyChannel((char*)cs_TargetNum.GetString());
					if( iPort > 0 )
					{
						if( !memcmp(szCalled, "msml", 4 ) )
						{

							//this->m_pManager->send_invite_withNoSDP("msml", 0, 10000, "211.115.66.94");
							//this->m_pManager->send_ack_withSDP("msml", 0, NULL);
							this->m_pManager->send_invite("msml", 0, 10000, "211.115.66.94");
							//this->m_pManager->send_invite("msml", 0, 10000, "211.115.66.94", m_szMSCallID);
						}
						else
						{
							this->m_pManager->send_invite(szCalled, NULL, iPort);
						}
					}
				}
			}
		}
		else
		{
			char *szCalled = (char *)wParam;
			if( this->m_pManager )
			{
				if( this->m_pManager->bRun )
				{
					int iPort = getEmptyChannel((char*)cs_TargetNum.GetString());
					if( iPort > 0 )
					{
						if( !memcmp(szCalled, "msml", 4 ) )
						{

							//this->m_pManager->send_invite_withNoSDP("msml", 0, 10000, "211.115.66.94");
							//this->m_pManager->send_ack_withSDP("msml", 0, NULL);
							this->m_pManager->send_invite("msml", 0, 10000, "211.115.66.94");
							//this->m_pManager->send_invite("msml", 0, 10000, "211.115.66.94", m_szMSCallID);
						}
						else
						{
							// m_bConfRoomOK 조건 추가
							this->m_pManager->send_invite(szCalled, NULL, iPort);
							//this->m_pManager->send_invite_withNoSDP(szCalled, NULL, iPort);
						}
					}
				}
			}
			delete szCalled;
		}
#else
		char *szCalled = (char *)wParam;
		if( this->m_pManager )
		{
			if( this->m_pManager->bRun )
			{
				int iPort = getEmptyChannel((char*)cs_TargetNum.GetString());
				if( iPort > 0 )
				{
					this->m_pManager->send_invite(szCalled, NULL, iPort);
				}
			}
		}
		delete szCalled;
#endif
	}
	return NULL;
}


void CXConManagerDlg::OnTestMemorytest2()
{
}


void CXConManagerDlg::OnTestMemorytest()
{

}

void CXConManagerDlg::OnTestInfo()
{
	if( this->m_pManager )
	{
		this->m_pManager->send_sip_info(sipInfo_undefined, "90164524816", -1, NULL, "<XML>1</XML>", 12);
	}

}

void CXConManagerDlg::OnTestInvite32786()
{
	if( this->m_pManager )
	{
		this->m_pManager->send_invite("000100", 0, 10000, "211.115.66.96");
	}
}

#if 1 // muk test
LRESULT CXConManagerDlg::EventSendAck(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		char *pCallID;
		pCallID = (char *)wParam;

		if (strcmp(m_szMSCallID, pCallID) == 0)
		{
			MSCreateCreateConference();
			//PrintLog("호 연결 성공", NULL, 1);
		}
		return NULL;
	}
	return NULL;
}

LRESULT CXConManagerDlg::EventRecvInfoOk(WPARAM wParam,LPARAM lParam)
{
	if( wParam != NULL )
	{
		char *pResult = NULL;

		pResult = (char *)wParam;

		if (strcmp(pResult, "Success") == 0 )
		{
			m_bConfRoomOK = TRUE;
		}
	}
	return NULL;
}

LRESULT CXConManagerDlg::EventBeforeSendInvite(WPARAM wParam,LPARAM lParam)
{
	sip_hd_t *pHd = NULL;
	//char *pRegID = NULL;
	pHd = (sip_hd_t *)wParam;
	//pRegID = (char *)lParam;
	

	//if (pHd && pRegID)
	if (pHd)
	{
		// ms로 보내는 패킷
		if (strcmp((char *)pHd->sipfield.to.uri.haddr.hostAddr.c_str(), "211.115.66.94") == 0 )
		{
			// to 값이 msml로 셋팅된 것이 아닌경우
			if (strcmp((char *)pHd->sipfield.to.uri.uid.c_str(), "msml") != 0 )
			{
				char szTo[256];

				memset(szTo,0,256);

				strcpy(szTo, pHd->sipfield.to.uri.uid.c_str());

				pHd->sipfield.to.uri.uid = "msml";
				pHd->sipfield.to.display = "";

				pHd->sipfield.from.uri.uid = szTo;
				pHd->sipfield.from.display = szTo;

				pHd->preheadline.sipUri.uid = "msml";

			}
		}
	}

	return 0;
}

void CXConManagerDlg::MSCreateCreateConference()
{
	if (!m_bConfRoomOK)
	{
		if( this->m_pManager )
		{
			CString szMSCreateXML;
			CString szXMLTemp = "<?xml version=\"1.0\" encoding=\"US-ASCII\"?>\r\n"\
					"<msml version=\"1.1\">\r\n"\
					"<createconference name=\"%s\" deletewhen=\"nomedia\" mark=\"1\">\r\n"\
					"<audiomix id=\"%smix\">\r\n"\
					"<n-loudest n=\"3\" />\r\n"\
					"</audiomix>\r\n"\
					"</createconference>\r\n"\
					"</msml>\r\n\r\n";

			szMSCreateXML.Format(szXMLTemp, m_szMSCallID, m_szMSCallID);

			this->m_pManager->send_sip_info(sipInfo_undefined, "msml", -1, "211.115.66.94", (char*)szMSCreateXML.GetString(), szMSCreateXML.GetLength(), "application/msml+xml");
		}
	}
}
#endif