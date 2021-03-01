//#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#if defined WIN32
#include <windows.h>
#elif defined WINCE
#undef WIN32
#include <winsock2.h>
#undef WIN32
#include <winbase.h>
#endif

// sky sip basic
//
#include "skysipua_g.h"
#include "skysip.h"
#include "sip_parse.h"
//

#include "skysock.h"
#include "SkyAsyncSock.h"
#include "sipCallStack.h"
#include "call_info.h"
#include "sky_nonce.h"
#include "cProfile.h"
//#include "md5.h"
#include "vmd5.h"
#include "StunClient.h"

#include "BlowFish.h"
#include "sipuac.h"

#include "SkySipUaManager.h"

using namespace std::tr1;
// Memory Leak detect
//#include "vld.h"
//#include "vldapi.h"

#define SIPCALLER_CONF    "./conf/sky_ua.cfg"

#define __STUN_USE_ENABLED

static 	int isocket=0;

#if defined WIN32 || defined WINCE
static CRITICAL_SECTION sky_sip_critical;
static CRITICAL_SECTION sky_sip_critical_msg;
static CRITICAL_SECTION sky_sip_critical_dlg;
#endif

class RcvDataPtr
{
public:
	RcvDataPtr()
	{
		len = 0;
		Ptr = NULL;
		hProcessHandle = NULL;
		pManager = NULL;
		dwThreadId = 0;
	}
	~RcvDataPtr()
	{
		if( hProcessHandle )
		{
			CloseHandle(hProcessHandle);
		}
		if( Ptr )
			delete Ptr;
	}
	int				len;
	char			*Ptr;
	sky_sip_manager	* pManager;
	DWORD			dwThreadId;
#ifdef LINUX
#else
	HANDLE	hProcessHandle;
#endif
};

bool win32_getMyIP(int nFamily, char * myip);

//#define DEBUG
void LogWrite(char *buf);
#ifdef DEBUG
#define LOGWRITE(A) LogWrite(A)
#else
#define LOGWRITE(A)
#endif

#if defined LINUX || defined MACOS
#elif defined WIN32 || defined WINCE
#ifdef WIN32
vector<shared_ptr<CSipCallStack> > m_MsgBuffer;
vector<shared_ptr<call_info>> m_CallDialogBuffer;
#elif defined WINCE
vector<CSipCallStack> m_MsgBuffer;
vector<call_info> m_CallDialogBuffer;
#endif
#endif
list<RcvDataPtr> m_InputedDataBuffer;
//vector<RcvDataPtr>

static SKYHANDLE	m_handleRegisterThread=NULL;
static SKYHANDLE	m_handleStateThread=NULL;
static	SKYHANDLE	m_handleDialog=NULL;

static int m_DefaultLogLevel = 3;

#define _DEBUG1
void WriteLog(char *pFileName, int logLevel, char * szTxt1, int iVal = -1, char *szTxt2=NULL)
{
#ifdef _DEBUG1
	FILE *fp = NULL;
	SYSTEMTIME st;

	char fileName[256];
	GetLocalTime(&st);

	sprintf_s(fileName, 256, "log\\%s_%04d%02d%02d.txt", pFileName, st.wYear, st.wMonth, st.wDay);
	if( m_DefaultLogLevel >= logLevel )
	{
#if defined LINUX || defined MACOS || defined WINCE
		if( (fp=fopen(fileName, "a+t")) != NULL )
#elif defined WIN32
		if( fopen_s(&fp, fileName, "a+t") == 0)
#endif
		{
			if( iVal <0 )
				fprintf(fp, "%02d:%02d:%02d:%04d - %s%s%s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,szTxt1, (szTxt2?" ":""), szTxt2);
			else
				fprintf(fp, "%02d:%02d:%02d:%04d - %s %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,szTxt1, iVal);
			fclose(fp);
		}
	}
#endif
}
void WriteManagerLog(int logLevel, char * szTxt1, int iVal = -1, char *szTxt2=NULL)
{
	WriteLog("manager", logLevel, szTxt1, iVal, szTxt2);
}

void MemStateLog(char *szTxt1, int iVal = -1, char *szTxt2= NULL )
{
	WriteLog("mem_state", 3, szTxt1, iVal, szTxt2);
}

//
// 콜다이얼로그의 상태를 감시하는 쓰레드 매니져.
//
#ifdef LINUX
static SKYTHREAD dialog_manager(SKYINVAL pArg)
#else
static DWORD WINAPI dialog_manager(LPVOID pArg)
#endif
{
	return THREAD_RETURN;
}

static DWORD WINAPI	received_msg_Process(LPVOID pArg)
{
	WriteManagerLog(1, "received_msg_Process", -1, "enter");
	if( pArg == NULL )
	{
#ifdef LINUX
		sky_thread_close(m_handleStateThread);
		ExitThread(0);
#endif
		return THREAD_RETURN;
	}
#if defined LINUX || defined MACOS || defined WINCE
	RcvDataPtr *p_Data = (RcvDataPtr*)pArg;
#elif defined WIN32
	RcvDataPtr * pData = (RcvDataPtr*)pArg;
	tr1::shared_ptr<RcvDataPtr> p_Data(pData);
#endif
	if( p_Data->pManager != NULL && p_Data->Ptr != NULL && p_Data->len > 0)
	{
		p_Data->pManager->ParseData(p_Data->Ptr, p_Data->len );
	}
	WriteManagerLog(1, "received_msg_Process", -1, "exit");
#if defined LINUX || defined MACOS || defined WINCE
	delete p_Data;
#endif
	return THREAD_RETURN;
}

#ifdef WIN32
const CSipCallStack &dref(const shared_ptr<CSipCallStack> &p)
{
	return *p;
}

const string &dref2(const shared_ptr<CSipCallStack> &p)
{
	return p->cid;
}

const BOOL dref1(const shared_ptr<CSipCallStack> &p )
{
	if( p->cid == "1")
		return true;
	return false;
}
#endif

static DWORD WINAPI sended_msg_Process(LPVOID pArg)
{
	HANDLE hThreadHandle=NULL;
	if( pArg == NULL )
	{
#ifdef LINUX
		sky_thread_close(m_handleStateThread);
		ExitThread(0);
#endif
		return THREAD_RETURN;
	}
	if( !((CSipCallStack*)pArg)->pManager )
		return THREAD_RETURN;
	EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
	tr1::shared_ptr<CSipCallStack> pData((CSipCallStack*)pArg);
	m_MsgBuffer.push_back(pData);
#elif defined LINUX || defined MACOS || defined WINCE
	CSipCallStack* pData = (CSipCallStack*)pArg;
	m_MsgBuffer.push_back(*pData);
#endif
	LeaveCriticalSection(&sky_sip_critical);
	string callId = pData->pSipHd->sipfield.cid.callid;
	WriteManagerLog(1, "sended_msg_Process", -1, (char*)callId.data());
	int dummycount= 0;
	int	iSeqlNumber = pData->pSipHd->sipfield.cseq.cseq * 100 + pData->pSipHd->sipfield.cseq.type;
	for( int i = 0; i < 540; i++)
	{
		if( ((sky_sip_manager*)pData->pManager)->bRun )
		{
			DWORD dwCode = WaitForSingleObject(pData->hWakeEvent,200);
			if( dwCode == WAIT_TIMEOUT )
			{
				//retry;
				if( dummycount++ >=89)
				{
					EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
					vector<shared_ptr<CSipCallStack>>::iterator c 
								= find_if(  m_MsgBuffer.begin(),
											m_MsgBuffer.end(),
											tr1::bind(equal_to<int>(),
												bind(&CSipCallStack::seqNumber,tr1::placeholders::_1),
												iSeqlNumber)
										);
#elif defined LINUX || defined MACOS || defined WINCE
					vector<CSipCallStack>::iterator c
								= find(  m_MsgBuffer.begin(), m_MsgBuffer.end(), iSeqlNumber);
#endif
					if( c != m_MsgBuffer.end() )
					{
						((sky_sip_manager*)pData->pManager)->sipUAC->send_msg(pData->pSipHd,0);
						WriteManagerLog(1, "Retry to send Message", -1, (char*)callId.data());
					}
					LeaveCriticalSection(&sky_sip_critical);
					dummycount = 0;
				}
				else
				{
					WriteManagerLog(3, "Retry to send Message1", -1, (char*)callId.data());
				}
			}
			else if( dwCode == WAIT_OBJECT_0)
			{
				if( ((sky_sip_manager*)pData->pManager)->bRun )
				{
					EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
					vector<shared_ptr<CSipCallStack>>::iterator c 
						= find_if(  m_MsgBuffer.begin(),
									m_MsgBuffer.end(),
									tr1::bind(equal_to<int>(), 
											bind(&CSipCallStack::seqNumber,tr1::placeholders::_1),
											iSeqlNumber)
								);
#elif defined WINCE || defined LINUX || defined MACOS
					vector<CSipCallStack>::iterator c 
						= find( m_MsgBuffer.begin(), m_MsgBuffer.end(), iSeqlNumber);
#endif
					if( c != m_MsgBuffer.end() )
					{
#ifdef WIN32
						if( c->get()->pSipHd->preheadline.msg_type == msg_invite )
#elif defined WINCE || defined LINUX || defined MACOS
						if( c->pSipHd->preheadline.msg_type == msg_invite )
#endif
						{
							WriteManagerLog(1, "Msg Process Done(invite)", -1, (char*)callId.data());
							//auto_ptr<call_info> pDialog(new call_info);
							//pDialog->
							//m_CallDialogBuffer.push_back(pDialog);
						}
						WriteManagerLog(1, "Msg Process Done", -1, (char*)callId.data());
						m_MsgBuffer.erase(c);
#ifdef WIN32
						pData.reset();
#elif defined LINUX || defined MACOS || defined WINCE
#endif
						//pData.re
					}
					LeaveCriticalSection(&sky_sip_critical);
				}
				return THREAD_RETURN;
			}
		}
		else
		{
			WriteManagerLog(1, "Msg Process Error", -1, (char*)callId.data());
		}
	}
	if( ((sky_sip_manager*)pData->pManager)->bRun )
	{
		EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
		vector<shared_ptr<CSipCallStack>>::iterator c 
			= find_if(  m_MsgBuffer.begin(),
						m_MsgBuffer.end(),
						tr1::bind(equal_to<int>(), 
								bind(&CSipCallStack::seqNumber,
								tr1::placeholders::_1), iSeqlNumber
						)
					);
#elif defined LINUX || defined WINCE || defined MACOS
		vector<CSipCallStack>::iterator c 
			= find(m_MsgBuffer.begin(),m_MsgBuffer.end(),iSeqlNumber);
#endif
		if( c != m_MsgBuffer.end() )
		{
			WriteManagerLog(1, "Msg Process TimeOut", -1, (char*)callId.data());
			m_MsgBuffer.erase(c);
			//pData.release();
		}
		LeaveCriticalSection(&sky_sip_critical);
	}
	return THREAD_RETURN;
}
//
// 메세지 스택을 감시하는 쓰레드 매니져.
//
#ifdef LINUX
static SKYTHREAD sky_state_manager(SKYINVAL pArg)
#else
static DWORD WINAPI sky_state_manager(LPVOID pArg)
#endif
{
	sky_sip_manager * pManager = (sky_sip_manager *)pArg;
	if( !pManager )
        return NULL;
	WriteManagerLog(1, "sky_state_manager", -1, "enter");

    //SKY_HOOK_FN *hookfn;
    int iInterval=0,curtime=0, icount=0, iStateVal=0;
#if defined WIN32 || defined WINCE
#ifdef UNICODE
	HANDLE	hEvent=CreateEvent(NULL, FALSE, FALSE, LPCWSTR("Sky_state_Manager"));
#else
	HANDLE	hEvent=CreateEvent(NULL, FALSE, FALSE, "Sky_state_Manager");
#endif
#endif
    
	pManager->bThreadState[0] = TRUE;
	if( m_MsgBuffer.size() > 0 )
		m_MsgBuffer.erase(m_MsgBuffer.begin(), m_MsgBuffer.end());
	
	if( m_CallDialogBuffer.size() > 0 )
		m_CallDialogBuffer.erase(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end());

	while( pManager->bRun )
    {
		BOOL bTrue = TRUE;
#ifdef WIN32
		vector<shared_ptr<call_info>>::iterator d;
#elif defined WINCE || defined LINUX || defined MACOS
		vector<call_info>::iterator d;
#endif
		EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
		d = find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
										tr1::bind(equal_to<int>(), 
										bind(&call_info::iDelete,
										tr1::placeholders::_1), 1));
#elif defined WINCE || defined LINUX || defined MACOS
		d = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),1);
#endif
		while( d != m_CallDialogBuffer.end() )
		{
			m_CallDialogBuffer.erase(d);
#ifdef WIN32
			d = find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
										tr1::bind(equal_to<int>(), 
										bind(&call_info::iDelete,
										tr1::placeholders::_1), 1));
#elif defined LINUX || defined MACOS || defined WINCE
			d = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),1);
#endif
		}
		LeaveCriticalSection(&sky_sip_critical_dlg);
		if( hEvent )
			WaitForSingleObject(hEvent, 100);
		else
			Sleep(100);

    }

    //sky_thread_close(m_handleStateThread);
	//ExitThread(0);
	if( hEvent )
		CloseHandle(hEvent);
	pManager->bThreadState[0] = FALSE;
	WriteManagerLog(1, "sky_state_manager", -1, "exit");
	return THREAD_RETURN;
}

//
// sky_register_manager
//
#ifdef LINUX
static SKYTHREAD sky_register_manager(SKYINVAL pArg)
#else
static DWORD WINAPI  sky_register_manager(LPVOID pArg)
#endif
{
	int chk_interval;
    sky_sip_manager *pManager = (sky_sip_manager *)pArg;
    chk_interval = 0;
	int iTotalTimeVal = (pManager->reg_ttl_val-2)*5;
	WriteManagerLog(1, "sky_register_manager", -1, "enter");
    if( pManager )
    {
		pManager->bThreadState[1] = TRUE;
#if defined WIN32 || defined WINCE
#ifdef UNICODE
		HANDLE hSleepEvent = CreateEvent(NULL, FALSE, FALSE, LPCWSTR("sky_register_manager_sleep"));
#else
		HANDLE hSleepEvent = CreateEvent(NULL, FALSE, FALSE, "sky_register_manager_sleep");
#endif
#endif
        while( pManager->bRun )
        {
			if( chk_interval >0 )
			{
				chk_interval --;
			}
			else
			{
				pManager->send_register(pManager->reg_ttl_val);
				chk_interval = iTotalTimeVal;
			}
			if( hSleepEvent)
				WaitForSingleObject(hSleepEvent, 200);
			else
				Sleep( 200);
		}
		if( hSleepEvent )
			CloseHandle(hSleepEvent);
		pManager->bThreadState[1] = FALSE;
	}
	//sky_thread_close(m_handleRegisterThread);
	//ExitThread(0);
	WriteManagerLog(1, "sky_register_manager", -1, "exit");
	return THREAD_RETURN;
}

sky_sip_manager::sky_sip_manager()
{
	m_bAutoResponse = TRUE;
	m_bSendAckToAllResponse = FALSE;
    init_manager(0);
}

sky_sip_manager::sky_sip_manager(IN int _sipPort,IN int _RTPport,IN int _rtpEnable)
{
	m_bAutoResponse = TRUE;
    init_manager(0);
    SIPUASport = _sipPort;
    //CallInfo.RTPport = _RTPport;
    rtpEnable = _rtpEnable;
}

sky_sip_manager::~sky_sip_manager()
{
    //if( sipUAC /*|| rtpCS*/)
    //    end_manager(0);
	if( sipUAC )
    {
		delete sipUAC;
		sipUAC = NULL;
	}

}

void sky_sip_manager::build_mytag(char *stag, int len)
{
    time_t tt;
    time(&tt);
#ifdef TAG_STR
    sprintf_s(stag, len,"%s+%d", u_branch,tt);
#else
    sprintf_s(stag, len,"%d", tt);
#endif
}

void sky_sip_manager::LoadConf()
{
    char sVal[128];
    int i=0,iVal;
    AutoAnswer = LGetPrivateProfileInt("AutoAnswer", 0, SIPCALLER_CONF);
    media_count = 0;
    if( LGetPrivateProfileInt("USE_G729", 0, SIPCALLER_CONF) )
    {
        sprintf_s(mediaAtt[i].name,64, "%s","rtpmap");
        sprintf_s(mediaAtt[i].value,64,"%s","18 G729/8000");
        mediaAtt[i].num = 18;
        mediaAtt[i].bAudioCodec = 1;
        i++;
    }
    if( LGetPrivateProfileInt("USE_G723", 0, SIPCALLER_CONF) )
    {
        sprintf_s(mediaAtt[i].name,64, "%s","rtpmap");
        sprintf_s(mediaAtt[i].value,64, "%s","4 G723/8000");
        mediaAtt[i].num = 4;
        mediaAtt[i].bAudioCodec = 1;
        //this->fo
        i++;
    }
    if( LGetPrivateProfileInt("USE_DTMF", 0, SIPCALLER_CONF) )
    {
        if( LGetPrivateProfileInt("DTMF_RFC2833_SEND", 0, SIPCALLER_CONF))
        {
            sprintf_s(mediaAtt[i].name,64, "%s","rtpmap");
            sprintf_s(mediaAtt[i].value,64, "%s","101 telephone-event/8000");
            mediaAtt[i].num = 101;
            mediaAtt[i].bAudioCodec = 0;
            i++;
            dtmf_type = dtmf_rfc2833;
        }
        else if( LGetPrivateProfileInt("DTMF_INBAND", 0, SIPCALLER_CONF))
        {
            dtmf_type = dtmf_inband;
        }
        else if( LGetPrivateProfileInt("DTMF_SIPINFO", 0, SIPCALLER_CONF))
        {
            dtmf_type = dtmf_sipinfo;
        }
    }
    sprintf_s(mediaAtt[i].name,64, "%s","sendrecv");
    memset(mediaAtt[i].value,0, 64);
    //sprintf(mediaAtt[i].value,"%s","");
	mediaAtt[i].num = -1;
    mediaAtt[i].bAudioCodec = 0;
    i++;
    media_count = i;
    memset(sVal, 0, 128);
    //CallInfo.RTPport = LGetPrivateProfileInt("StartRtp",1200, SIPCALLER_CONF);
    
    LGetPrivateProfileString("ProxyIP", "127.0.0.1", sVal, 128, SIPCALLER_CONF);
    {
        struct hostent *myent;
        struct in_addr *myen;
        long int *add;
        myen = (struct in_addr *)malloc( sizeof( struct in_addr ));
        memset(myen, 0, sizeof( struct in_addr ));
        myent = gethostbyname(sVal);
				
        if( myent )
        {
            while(*myent->h_addr_list != NULL)
            {
                add = (long int *)*myent->h_addr_list;
                myen->s_addr = *add;
                sprintf_s(sVal, 128,"%s",inet_ntoa(*myen));
                myent->h_addr_list++;
                break;
            }
        }
        free(myen);
    }
    //sprintf_s(CallInfo.targetip, 24, "%s", sVal);
    sprintf_s(proxyip, 24, "%s", sVal);
    win32_getMyIP(AF_INET, hostip);
	sprintf_s(localip, 24, "%s", hostip);
    memset(sVal, 0, 128);
    LGetPrivateProfileString("E164Prefix", "1000", sVal, 24, SIPCALLER_CONF);
    sprintf_s(u_number,64, "%s", sVal);
    SIPUASport = LGetPrivateProfileInt("HostPort", 5060, SIPCALLER_CONF);
    proxyport = LGetPrivateProfileInt("LocalPort", 5060, SIPCALLER_CONF);
    port = proxyport;
    reg_ttl_val = LGetPrivateProfileInt("RegistTTL", 180, SIPCALLER_CONF);
    if( reg_ttl_val < 3)
        reg_ttl_val = 3;
    timeout = LGetPrivateProfileInt("DefaultTTL", 180, SIPCALLER_CONF);
    i_session_expires = timeout;
    retrial_count = LGetPrivateProfileInt("RetrialCount", 1, SIPCALLER_CONF);
    memset(sVal, 0, 128);
    LGetPrivateProfileString("AUTHPASSWD", "", sVal, 128, SIPCALLER_CONF);
    iVal = strlen( sVal );
    if( iVal > 0 )
    {
        unsigned char output[128], input[128];
        int iEncLen=0;
        CBlowFish fish((unsigned char*)("sky_sip_ua"), 10);
        memset(output, 0, 128);
        memset(input, 0, 128);
        charToHex(sVal, iVal, input, 128);
        for(int i = 0; i< iVal/2; i=i+8 )
        {
            fish.Decrypt((unsigned char *)&input[i], &output[i], 8);
            iEncLen = iEncLen + strlen( (char*)&output[i]);
        }
        sprintf_s(auth_passwd, 64,"%s", output);
    }

    memset(sVal, 0, 128);
    LGetPrivateProfileString("USER_BRANCH", "SIP_UA", sVal, 128, SIPCALLER_CONF);
    sprintf_s(u_branch,128, "%s", sVal);
#ifdef __STUN_USE_ENABLED
    if( LGetPrivateProfileInt("STUNEnable", 0, SIPCALLER_CONF) )
    {
        stun_enable = 1;
        stunInfo.stunport = LGetPrivateProfileInt("STUNport", 0, SIPCALLER_CONF);
        memset(stunInfo.stunip, 0, 24);
        memset(sVal, 0, 128);
        LGetPrivateProfileString("STUNip", "127.0.0.1", sVal, 128, SIPCALLER_CONF);
        {
            struct hostent *myent;
            struct in_addr *myen;
            long int *add;
            myen = (struct in_addr *)malloc( sizeof( struct in_addr ));
            memset(myen, 0, sizeof( struct in_addr ));
            myent = gethostbyname(sVal);
            if( myent )
            {
                while(*myent->h_addr_list != NULL)
                {
                    add = (long int *)*myent->h_addr_list;
                    myen->s_addr = *add;
#ifdef MSVC_6
                    sprintf(stunInfo.stunip,"%s",inet_ntoa(*myen));
#else
                    sprintf_s(stunInfo.stunip,24,"%s",inet_ntoa(*myen));
#endif
                    myent->h_addr_list++;
                    break;
                }
            }
            free(myen);
        }

    }
    else
    {
        stun_enable = 0;
        stunInfo.stunport = 0;
        memset(stunInfo.stunip, 0, 24);
    }
#endif// __STUN_USE_ENABLED
}

int sky_sip_manager::GetVersion()
{
    return 0;
}

void sky_sip_manager::end_manager(int opt/*=0 */)
{
	bRun = 0;
#if defined WIN32 || defined WINCE
#ifdef UNICODE
	HANDLE hEvent = CreateEvent(NULL, FALSE,FALSE, LPCWSTR("End_MANAGER"));
#else
	HANDLE hEvent = CreateEvent(NULL, FALSE,FALSE, "End_MANAGER");
#endif
#endif
	if( sipUAC )
	{
		sipUAC->Close();
	}
    if( opt == 0)
    {
	    //register_flag = 0;
		while(bThreadState[0] || bThreadState[1] )
		{
			if( hEvent )
				WaitForSingleObject(hEvent,200);
			else
				Sleep(200);
		}
    }

	if( m_MsgBuffer.size() > 0 )
	{
#ifdef WIN32
		for(vector<shared_ptr<CSipCallStack>>::iterator mCur = m_MsgBuffer.begin();
#elif defined WINCE || defined LINUX || defined MACOS
		for(vector<CSipCallStack>::iterator mCur = m_MsgBuffer.begin();
#endif
			mCur != m_MsgBuffer.end(); mCur++)
		{
#ifdef WIN32
			SetEvent(mCur->get()->hWakeEvent);
#elif defined LINUX || defined MACOS || defined WINCE
			SetEvent(mCur->hWakeEvent );
#endif
		}
		if( hEvent )
			WaitForSingleObject(hEvent,200);
		else
			Sleep(200);

		m_MsgBuffer.erase(m_MsgBuffer.begin(), m_MsgBuffer.end());
	}
	
	if( hEvent )
		WaitForSingleObject(hEvent,300);
	else
		Sleep(300);
	if( m_CallDialogBuffer.size() > 0 )
	{
		m_CallDialogBuffer.erase(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end());
	}
	CloseHandle(m_handleRegisterThread);
	CloseHandle(m_handleStateThread);
	CloseHandle(m_handleDialog);

    DeleteCriticalSection(&sky_sip_critical);
	DeleteCriticalSection(&sky_sip_critical_dlg);

	if( hEvent )
	{
		CloseHandle(hEvent );
	}
	
}

int sky_sip_manager::start_manager(int iOpt/*=0*/)
{
    if( rtpEnable )
    {
#if 0
        if( rtpCS )
        {
            rtpCS = new CRtp(CallInfo.RTPport,CallInfo.RTPport+1);
            if( !rtpCS )
                return 0;
        }
#endif
    }
#ifdef WIN32
	::CreateDirectory("log", NULL);
#endif
    port = 5060;
    if( iOpt == 0 )
    {
#ifdef WIN32
        InitializeCriticalSection(&sky_sip_critical);
		InitializeCriticalSection(&sky_sip_critical_dlg);
#endif
		
        if( stun_enable )
        {
            if( !stun_process() )
            {
                return 0;
            }
        }
        if( !sipUAC )
        {
            sipUAC = new CSipuac(this, port, SIPUASport, SKY_SOCK_UDP);
            if( sipUAC )
            {
                //sipUAC->uac_unblock();
#ifdef MSVC_6
                sprintf(sipUAC->dip,
#else
                sprintf_s(sipUAC->dip, 24,
#endif
					"%s",proxyip);
                if( stun_enable )
				{
					sipUAC->sockHandle = client.con1.sockHandle;
                    if( sipUAC->Create(1) <= 0 )
						return 0;
				}
				else
				{
                    if( sipUAC->Create() <= 0)
						return 0;
				}
#ifdef MSVC_6
                sprintf(sipUAC->sip,
#else
                sprintf_s(sipUAC->sip,24,
#endif
					"%s",hostip);
                register_flag = 1;
                bRun = 1;
#ifdef LINUX
                m_handleRegisterThread = sky_thread_create(sky_register_manager, this, 0, 0);
				//m_handleDialogThread = sky_thread_create(sky_dialog_manager, this, 0, 0 );
				//m_handleDialog = sky_thread_create(dialog_manager, this, 0, 0 );
                if( ( m_handleStateThread = sky_thread_create(sky_state_manager, this, 0, 0) ) )
#else
				DWORD dwId = 0;
				//m_handleDialog = CreateThread(NULL,0,&dialog_manager, this, 0, &dwId);
				m_handleRegisterThread = CreateThread(NULL,0,&sky_register_manager, this, 0, &this->dwThreadId[0]);
                if( ( m_handleStateThread = CreateThread(NULL, 0, &sky_state_manager, this, 0, &this->dwThreadId[1]) ) )
#endif
                {
                    init_nonce();
                    return 1;
                }
                else
                {
                    end_manager(0);
                    return 0;
                }
            }
        }
	}
    else
    {
        sipUAC->uac_block();
        memset(sipUAC->sip, 0, 24);
#ifdef MSVC_6
        sprintf(sipUAC->dip,"%s",proxyip);
        sprintf(sipUAC->sip,"%s",hostip);
#else
        sprintf_s(sipUAC->dip,24,"%s",proxyip);
        sprintf_s(sipUAC->sip,24,"%s",hostip);
#endif
        register_flag = 1;
        bRun = 1;
        if( stun_enable )
        {
            if( !stun_process() )
            {
                return 0;
            }
			sipUAC->uac_unblock();
        }

#ifdef LINUX
		m_handleRegisterThread = sky_thread_create(sky_register_manager, this, 0, 0);
		m_handleDialog = sky_thread_create(dialog_manager, this, 0, 0 );
        if( ( m_handleStateThread = sky_thread_create(sky_state_manager, this, 0, 0)) )
#else
		DWORD dwId = 0;
		m_handleDialog = CreateThread(NULL,0,&dialog_manager, this, 0, &dwId);
		m_handleRegisterThread = CreateThread(NULL,0,&sky_register_manager, this, 0, &this->dwThreadId[0]);
		if( ( m_handleStateThread = CreateThread(NULL, 0, &sky_state_manager, this, 0, &this->dwThreadId[1]) ) )
		if( 1)
#endif
        {
            init_nonce();
            return 1;
        }
        else
        {
            end_manager(0);
            return 0;
        }
    }
    return 0;
}

void sky_sip_manager::init_manager(int opt)
{
    int i;
    bRun = 0;
	iSipCseqCount = 0;
    retrial_count = 1;
    if( opt == 0)
    {
        sipUAC = NULL;
        m_handleRegisterThread = NULL;
        m_handleStateThread = NULL;
    }
	bThreadState[0] = FALSE;
	bThreadState[1] = FALSE;
	this->dwThreadId[0] = 0;
	this->dwThreadId[1] = 0;
    rtpEnable = 0;
    //CallInfo.RTPport = 0;
    SIPUACport = 0;
    SIPUASport = 5060;
    //init_sip_pre_hd_field((void*)&sip_info.preheadline);
    //init_sip_hd_field((void*)&sip_info.sipfield);
//    init_sip_pre_hd_field((void*)&sip_info_send.preheadline);
//    init_sip_hd_field((void*)&sip_info_send.sipfield);
    //sip_info.type = sip_invalid;
    //CallInfo.eCallState = e_call_ready;
    memset(hostip, 0, 24);
    memset(u_number, 0, 64);
    port = 0;
    reg_ttl_val = 360000;
    register_flag = 0;
    bRegistered = 0;
    csequence = 1;
    i_session_expires = 60;
//    use_www_auth = 0;
    init_hook();
    for( i = 0 ; i< 10 ; i++ )
        memset(allow_method[i], 0, 32);
#ifdef MSVC_6
    sprintf(allow_method[0],"%s","INVITE");
    sprintf(allow_method[1],"%s","ACK");
    sprintf(allow_method[2],"%s","CANCEL");
    sprintf(allow_method[3],"%s","BYE");
    sprintf(allow_method[4],"%s","INFO");
    sprintf(allow_method[5],"%s","REGISTER");
#else
    sprintf_s(allow_method[0],32,"%s","INVITE");
    sprintf_s(allow_method[1],32,"%s","ACK");
    sprintf_s(allow_method[2],32,"%s","CANCEL");
    sprintf_s(allow_method[3],32,"%s","BYE");
    sprintf_s(allow_method[4],32,"%s","INFO");
    sprintf_s(allow_method[5],32,"%s","REGISTER");
#endif
    memset(callid, 0, 128);
//    memset( u_tag,0, 256);
    memset( u_branch,0, 128);
    //memset( CallInfo.target_number, 0, 64);
    memset( realm, 0, 64);
    memset( uri, 0, 128);
    memset( auth_passwd, 0, 64);
    memset( www_nonce, 0, 48);
    memset( proxy_nonce, 0, 48);
    zero_target_info();
    AutoAnswer = 0;
    // rtp info
    bOpenedRtpIn = 0;
    bOpenedRtpOut = 0;
    //CallInfo.RTPportToSend = 0;
    //CallInfo.selected_codec = 0;
    //CallInfo.contact_port = 0;
    for( i = 0; i < MAX_CONTACT_NUM ; i++)
    {
        init_contact_buf(&RegisterInfo.contact[i]);
        RegisterInfo.contact_point = 0;
    }
    //memset(CallInfo.contact_ip, 0, 24);
    //memset(CallInfo.RTPipToSend, 0, 24);
    memset(registra_cid, 0, 128);
    memset(sendip, 0, 24);
    nc_count = 0;
    //memset( CallInfo.ftag, 0, 32);
    //memset( CallInfo.rvRRParam, 0, 32);
    //memset( CallInfo.lr, 0, 4);
    //CallInfo.bRecordRoute = 0;
    proxy_auth = 0;
    dtmf_type = dtmf_unknown;
    //init_via_buf(&CallInfo.recv_via);
    //CallInfo.pre_msg_type = msg_invalid;
    stun_enable = 0;
    memset(stunInfo.stunip, 0, 24);
    stunInfo.stunport = 0;
}

void sky_sip_manager::zero_target_info()
{
    memset( callid_ip, 0, 24);
    //memset( CallInfo.target_display, 0, 64);
    //memset( CallInfo.target_tag, 0, 128);
    //memset( CallInfo.target_branch, 0, 128);
    //memset( CallInfo.target_callid_ip, 0, 24);
    //memset( CallInfo.rvRRParam, 0, 32);
    //memset( CallInfo.ftag, 0, 32);
    //memset( CallInfo.lr, 0, 4);
    memset( u_branch, 0, 128);
}

int sky_sip_manager::ua_connect(IN char *dip,IN int dport)
{
    return 0;
}


int sky_sip_manager::send_fail( IN void *_reqHd, IN int reason, IN char *text)
{
	p_sip_hd_t reqHd = (p_sip_hd_t )_reqHd;
	
	if(this->bRun && this->bRegistered )
	{
		sip_hd_t *msgFail = new sip_hd_t();
		if( msgFail )
		{
			if( build_res_msg(msgFail, reason, reqHd->sipfield.cseq.type,reqHd, text) > 0)
			{
				this->OnSendFailMsg(msgFail);
				sipUAC->send_msg(msgFail,0);
				delete msgFail;
				return 1;
			}
			delete msgFail;
		}
	}

    return 0;
}

int sky_sip_manager::send_ok(void* _reqHd, void *_pCallDialog)
{

	p_sip_hd_t reqHd = (p_sip_hd_t )_reqHd;
    SKY_HOOK_FN *fn;
	
	if(this->bRun && this->bRegistered )
	{
		sip_hd_t *msgOk = new sip_hd_t();
		if( msgOk )
		{
			if( build_res_msg(msgOk, 200, reqHd->sipfield.cseq.type,reqHd, "OK", _pCallDialog) > 0)
			{
				if( hookFN.sok )
				{
					fn = (SKY_HOOK_FN *)hookFN.sok;
					if(!fn(&msgOk) )
					{
						delete msgOk;
						return 0;
					}
				}
				sipUAC->send_msg(msgOk,0);
				delete msgOk;
				return 1;
			}
			delete msgOk;
		}
	}

    return 0;
}

int sky_sip_manager::send_cancel(char *szTarget)
{
	SKY_HOOK_FN *fn;
	char mip[24];
	int mport;
	memset(mip, 0, 24);
	mport = 0;
	if(this->bRun && this->bRegistered )
	{
		EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
		vector<shared_ptr<call_info>>::iterator pDlg
							= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
										tr1::bind(equal_to<string>(), 
										bind(&call_info::target_number,
										tr1::placeholders::_1), szTarget));
#elif defined LINUX || defined MACOS || defined WINCE
		vector<call_info>::iterator pDlg = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),szTarget);
#endif
		if( pDlg != m_CallDialogBuffer.end() )
		{

			sip_hd_t *msgCancel = new sip_hd_t();
			if( msgCancel )
			{
#if defined LINUX || defined MACOS || defined WINCE
				if( build_req_msg(msgCancel, msg_cancel,szTarget, (void*)&pDlg) > 0 )
#elif defined WIN32
				if( build_req_msg(msgCancel, msg_cancel,szTarget, pDlg->get()) > 0 )
#endif
				{
					if( hookFN.scancel )
					{
						fn = (SKY_HOOK_FN *)hookFN.scancel;
						if(!fn(msgCancel) )
						{
							delete msgCancel;
							return 0;
						}
					}
#if defined LINUX || defined MACOS || defined WINCE
					sprintf_s(sipUAC->dip, 24,"%s", pDlg->sendIP);
#elif defined WIN32
					sprintf_s(sipUAC->dip, 24,"%s", pDlg->get()->sendIP);
#endif
					if( sipUAC->send_msg(msgCancel,0))
					{
						CSipCallStack * pStack = new CSipCallStack(msgCancel);
						pStack->pManager = this;
						pStack->hThreadHandle = CreateThread(NULL, 0, sended_msg_Process, pStack, 0, &pStack->dwThreadID);
						LeaveCriticalSection(&sky_sip_critical_dlg);
						// swhors
						return 1;
					}
				}
				delete msgCancel;
			}
			//LeaveCriticalSection(&sky_sip_critical);
		}
		LeaveCriticalSection(&sky_sip_critical_dlg);
	}
	return 0;
}

int sky_sip_manager::send_bye(char *szTarget)
{
	SKY_HOOK_FN *fn;
	char mip[24];
	int mport;
	memset(mip, 0, 24);
	mport = 0;
	if(this->bRun && this->bRegistered )
	{
		EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
		vector<shared_ptr<call_info>>::iterator pDlg
						= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
									tr1::bind(equal_to<string>(), 
									bind(&call_info::target_number,
									tr1::placeholders::_1), szTarget));
#elif defined LINUX || defined MACOS || defined WINCE
		vector<call_info>::iterator pDlg
						= find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),szTarget);
#endif
		if( pDlg != m_CallDialogBuffer.end() )
		{
			sip_hd_t *msgBye = new sip_hd_t();
			if( msgBye )
			{
#ifdef WIN32
				if( build_req_msg(msgBye, msg_bye,szTarget, pDlg->get()) > 0 )
#elif defined LINUX || defined MACOS || defined WINCE
				// swhors@naver.com 2009/11/17
				// 나중에 다시 점검 할 것.
				// 아래의 (void*)&pDlg에서 잘못된 메모리 번지를 참조 할 수 있음.
				if( build_req_msg(msgBye, msg_bye,szTarget, (void*)&pDlg) > 0 )
#endif
				{
					if( hookFN.sbye )
					{
						fn = (SKY_HOOK_FN *)hookFN.sbye;
						if(!fn(msgBye) )
						{
							LeaveCriticalSection(&sky_sip_critical_dlg);
							delete msgBye;
							return 0;
						}
					}
					OnSendEndCall(msgBye);
#ifdef WIN32
					msgBye->sipfield.to.uri.haddr.hostAddr = pDlg->get()->sendIP;
					msgBye->preheadline.sipUri.haddr.hostAddr = pDlg->get()->sendIP;
					msgBye->sipfield.from.tag = pDlg->get()->self_tag;
					msgBye->sipfield.to.tag = pDlg->get()->target_tag;
					sprintf_s(sipUAC->dip, 24,"%s", pDlg->get()->sendIP);
#elif defined LINUX || defined WINCE || defined MACOS
					msgBye->sipfield.to.uri.haddr.hostAddr = pDlg->sendIP;
					msgBye->preheadline.sipUri.haddr.hostAddr = pDlg->sendIP;
					msgBye->sipfield.from.tag = pDlg->self_tag;
					msgBye->sipfield.to.tag = pDlg->target_tag;
					sprintf_s(sipUAC->dip, 24,"%s", pDlg->sendIP);
#endif
					if( sipUAC->send_msg(msgBye,0))
					{
						CSipCallStack * pStack = new CSipCallStack(msgBye);
						pStack->pManager = this;
						LeaveCriticalSection(&sky_sip_critical_dlg);
						pStack->hThreadHandle = CreateThread(NULL, 0, sended_msg_Process, pStack, 0, &pStack->dwThreadID);
						// swhors
						return 1;
					}
				}
				delete msgBye;
			}
		}
		LeaveCriticalSection(&sky_sip_critical_dlg);
	}
	return 0;
}

int sky_sip_manager::send_register(int ttl)
{
    SKY_HOOK_FN *fn;
    int auth = 0;
	
	sip_hd_t *msgRegister = new sip_hd_t();
	if( msgRegister )
	{
		if( build_req_msg(msgRegister, msg_register,auth) > 0 )
		{
			WriteManagerLog(1, "register : build", -1, (char*)msgRegister->sipfield.cid.callid.c_str());
			msgRegister->sipfield.session_expires = "";
			msgRegister->sipfield.expire = ttl;
			if( !msgRegister->sipfield.contact_list.empty() )
#ifdef WIN32
				msgRegister->sipfield.contact_list.begin()->get()->expires= ttl;
#elif defined LINUX || defined MACOS || defined WINCE
				msgRegister->sipfield.contact_list.begin()->expires= ttl;
#endif
			if( hookFN.sregister )
			{
				fn = (SKY_HOOK_FN *)hookFN.sregister;
				if(!fn(msgRegister) )
				{
					delete msgRegister;
					return 0;
				}
			}
			sprintf_s(sipUAC->dip, 24,"%s", proxyip);
			msgRegister->preheadline.sipUri.uid = "";
			if( sipUAC->send_msg(msgRegister,auth))
			{
				CSipCallStack * pStack = new CSipCallStack(msgRegister);
				pStack->pManager = this;
				WriteManagerLog(1, "register : send", -1, (char*)msgRegister->sipfield.cid.callid.c_str());
				pStack->hThreadHandle = CreateThread(NULL, 0, sended_msg_Process, pStack, 0, &pStack->dwThreadID);
				// swhors
				return 1;
			}
		}
		OnRegisterFail(msgRegister);
		delete msgRegister;
	}
	return 0;
}


int sky_sip_manager::send_ok_to_invite(char * szCallID, int selectCodec)
{
	return 0;
}

int sky_sip_manager::send_ack_withSDP(char *szCallID, int reqCodec, char *szRtpIP, int rtpPort)
{
	EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
	vector<shared_ptr<CSipCallStack>>::iterator c 
					= find_if(  m_MsgBuffer.begin(),
								m_MsgBuffer.end(),
								tr1::bind(equal_to<string>(), 
										bind(&CSipCallStack::cid,
										tr1::placeholders::_1),
								szCallID)
							);
#elif defined LINUX || defined MACOS || defined WINCE
	vector<CSipCallStack>::iterator c 
					= find(  m_MsgBuffer.begin(),
								m_MsgBuffer.end(),
								szCallID
							);
#endif
	if( c != m_MsgBuffer.end())
	{
#ifdef WIN32
		send_ack_withSDP(c->get()->pSipHd, reqCodec, szRtpIP, rtpPort);
#elif defined LINUX || defined MACOS || defined WINCE
		send_ack_withSDP(c->pSipHd, reqCodec, szRtpIP, rtpPort);
#endif
		LeaveCriticalSection(&sky_sip_critical);
		return 1;

	}
	LeaveCriticalSection(&sky_sip_critical);
	return 0;
}

int sky_sip_manager::send_ack_withSDP(void * _reqHd,int reqCodec,char *szRtpIP, int rtpPort)
{
	return send_ack_internal(NULL, _reqHd, TRUE, reqCodec, szRtpIP, rtpPort);
}

int sky_sip_manager::send_ack(char * szCallID)
{
	BOOL bfind = FALSE;
	//EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
	vector<shared_ptr<CSipCallStack>>::iterator c 
					= find_if(  m_MsgBuffer.begin(),
								m_MsgBuffer.end(),
								tr1::bind(equal_to<string>(),
											bind(&CSipCallStack::cid,
											tr1::placeholders::_1),
								szCallID)
							);
#elif defined LINUX || defined MACOS || defined WINCE
	vector<CSipCallStack>::iterator c = find(m_MsgBuffer.begin(),m_MsgBuffer.end(),szCallID);
#endif
	if( c != m_MsgBuffer.end())
	{
#ifdef WIN32
		send_ack(c->get()->pSipHd);
		c->get()->bDelete = 1;
		SetEvent(c->get()->hWakeEvent);
#elif defined LINUX || defined MACOS || defined WINCE
		send_ack(c->pSipHd);
		c->bDelete = 1;
		SetEvent(c->hWakeEvent);
#endif
		//LeaveCriticalSection(&sky_sip_critical);
		return 1;

	}
	//LeaveCriticalSection(&sky_sip_critical);
	return 0;
}
int sky_sip_manager::send_ack(void *_reqHd)
{
	return send_ack_internal(NULL, _reqHd);
}

int	sky_sip_manager::send_ack_internal(void *_pSipCallStack, void * _sipHd, BOOL bWithSDP,int reqCodec, char *szRtpIP, int rtpPort)
{
	CSipCallStack *pSipCallStack = (CSipCallStack*)_pSipCallStack;
	p_sip_hd_t reqHd = (p_sip_hd_t) (_sipHd?_sipHd:pSipCallStack->pSipHd);
	if( reqHd )
	{
		char mip[24];
		int mport;
		memset(mip, 0, 24);
		mport = 0;
		if(this->bRun && this->bRegistered )
		{
			sip_hd_t *msgAck = new sip_hd_t();
			if( msgAck )
			{
				if( build_req_msg(msgAck, msg_ack,reqHd, NULL) > 0 )
				{
					//sprintf_s(sipUAC->dip, 24,"%s", proxyip);
					if( bWithSDP )
						build_SDP(msgAck, rtpPort, szRtpIP,&reqHd->sipContent.sdp, reqCodec);
					msgAck->preheadline.sipUri.haddr.hostAddr
						= reqHd->preheadline.sipUri.haddr.hostAddr;
					sprintf_s(sipUAC->dip, 24,"%s", (char*)reqHd->sipfield.to.uri.haddr.hostAddr.c_str());
					if( !msgAck->sipfield.to.uri.uid.compare("msml") )
					{
						msgAck->sipfield.contact_list.erase(
									msgAck->sipfield.contact_list.begin(),
									msgAck->sipfield.contact_list.end());
						msgAck->sipfield.to.display="";
						msgAck->preheadline.sipUri.transport = sip_none_proto;
					}
					//msgAck->sipfield.cid = reqHd->sipfield.cid;
					
					if( sipUAC->send_msg(msgAck,0))
					{
						this->OnSendAck(msgAck);
						delete msgAck;
						// swhors
						return 1;
					}
				}
				delete msgAck;
			}
		}
		return 1;
	}
	return 0;
}

int	sky_sip_manager::send_invite_internal(char *szTarget, char *szRtpIP, int rtpPort, char *szDestIP, bool withSDP)
{
	BOOL bCall_Exist = FALSE;
	SKY_HOOK_FN *fn;
	char mip[24];
	int mport;
	memset(mip, 0, 24);
	mport = 0;
	if(this->bRun && this->bRegistered )
	{
		call_info * callInfo = new call_info();
		EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
		vector<shared_ptr<call_info>>::iterator dlg
			= find_if(m_CallDialogBuffer.begin(),
						m_CallDialogBuffer.end(),
						bind( equal_to<string>(), bind(&call_info::target_number, tr1::placeholders::_1)
						, szTarget));
#elif defined LINUX || defined MACOS || defined WINCE
		vector<call_info>::iterator dlg
			= find(m_CallDialogBuffer.begin(),
						m_CallDialogBuffer.end(),szTarget);
#endif
		if( dlg != m_CallDialogBuffer.end() )
		{
#ifdef WIN32
			*callInfo = *(dlg->get());
#elif defined LINUX || defined MACOS || defined WINCE
			*callInfo = *dlg;
#endif
			bCall_Exist = true;
		}
		LeaveCriticalSection(&sky_sip_critical_dlg);

		if( callInfo )
		{
			sprintf_s(callInfo->RTPip, 24, "%s", (szRtpIP?szRtpIP:this->localip));
			callInfo->RTPport = rtpPort;
			callInfo->bStartWithSDP = withSDP;
			sip_hd_t *msgInvite = new sip_hd_t();
			if( msgInvite )
			{
				if( build_req_msg(msgInvite, msg_invite,szTarget, callInfo) > 0 )
				{
					if( szDestIP )
					{
						msgInvite->preheadline.sipUri.haddr.hostAddr = szDestIP;
						msgInvite->sipfield.to.uri.haddr.hostAddr = szDestIP;
					}
					
					if( hookFN.sinvite )
					{
						fn = (SKY_HOOK_FN *)hookFN.sinvite;
						if(!fn(msgInvite) )
						{
							delete msgInvite;
							delete callInfo;
							return 0;
						}
					}
					
					char tagBuf[128];
					sprintf_s(tagBuf, 128, "%ld", GetRandomDouble(GetTickCount()));
					msgInvite->sipfield.from.tag = tagBuf;
					callInfo->self_tag = tagBuf;
					
					sprintf_s(sipUAC->dip, 24,"%s", (szDestIP?szDestIP:proxyip));
					sprintf_s(callInfo->sendIP, 24, "%s", sipUAC->dip);

					if( memcmp(callInfo->sendIP, proxyip, strlen(proxyip) ) )
					{
						msgInvite->sipfield.from.uri.haddr.hostAddr = this->localip;
					}
					if( msgInvite->sipfield.to.uri.uid == "msml")
					{
						msgInvite->sipfield.allow = 0;
						msgInvite->sipfield.to.display="";
					}
					this->OnBeforeSendReqCall(msgInvite);
					if( sipUAC->send_msg(msgInvite,0))
					{
						this->OnSendReqCall(msgInvite);
						CSipCallStack * pStack = new CSipCallStack(msgInvite);
						pStack->pManager = this;
						pStack->hThreadHandle = CreateThread(NULL, 0, sended_msg_Process, pStack, 0, &pStack->dwThreadID);
						if( !bCall_Exist )
						{
							EnterCriticalSection(&sky_sip_critical_dlg);
							callInfo->sipCallid = msgInvite->sipfield.cid;
							callInfo->target_number = msgInvite->sipfield.to.uri.uid;
#ifdef WIN32
							tr1::shared_ptr<call_info> ci(callInfo);
							m_CallDialogBuffer.push_back(ci);
#elif defined LINUX || defined MACOS || defined WINCE
							m_CallDialogBuffer.push_back(*callInfo);
#endif
							LeaveCriticalSection(&sky_sip_critical_dlg);
						}
						else
						{
							delete callInfo;
						}
						// swhors
						return 1;
					}
				}
				this->OnFailReqCall(msgInvite);
				delete msgInvite;
			}
			delete callInfo;
		}
		//LeaveCriticalSection(&sky_sip_critical);
	}
	return 0;
}


int	sky_sip_manager::send_invite_withNoSDP(char *szTarget, char *szRtpIP, int rtpPort, char *szDestIP)
{
	return send_invite_internal(szTarget, szRtpIP, rtpPort, szDestIP, FALSE);
}

int sky_sip_manager::send_invite(char *szTarget, char *szRtpIP, int rtpPort, char *szDestIP)
{
	return send_invite_internal(szTarget, szRtpIP, rtpPort, szDestIP);
}

int sky_sip_manager::stun_process()
{
    EnterCriticalSection(&sky_sip_critical);
	if( sipUAC )
	{
		client.con1.sockHandle = sipUAC->sockHandle;
		Sleep(100);
	}
    if( client.Create(hostip, 5060, stunInfo.stunip, stunInfo.stunport) )
    {
        if( client.test_manager() )
        {
			stunInfo.stun_state = client.stun_state;
            if( client.stun_state == STUN_OPENINTERNET )
            {
                LeaveCriticalSection(&sky_sip_critical);
                return 1;
            }
            else if( client.stun_state == STUN_FULL_CON )
            {
#ifdef MSVC_6
                sprintf(hostip, "%s", client.mapped_ip);
#else
                sprintf_s(hostip, 24,"%s", client.mapped_ip);
#endif
                port = client.mapper_port;
                SIPUACport = client.con1.sport;
                LeaveCriticalSection(&sky_sip_critical);
                return 1;
            }
        }
        //client.con1.close_handle();
        //client.con1.sockHandle = 0;
    }
    LeaveCriticalSection(&sky_sip_critical);
    return 0;
}


int sky_sip_manager::send_trying(void *_reqHd)
{
	p_sip_hd_t reqHd = (p_sip_hd_t )_reqHd;
	
	if(this->bRun && this->bRegistered )
	{
		sip_hd_t *msgTrying = new sip_hd_t();
		if( msgTrying )
		{
			if( build_res_msg(msgTrying, 100, reqHd->sipfield.cseq.type,reqHd, "Trying") > 0)
			{
				this->OnSendTrying(msgTrying);
				sipUAC->send_msg(msgTrying,0);
				delete msgTrying;
				return 1;
			}
			delete msgTrying;
		}
	}
    return 0;
}

int sky_sip_manager::send_ringing(void *_reqHd)
{
	p_sip_hd_t reqHd = (p_sip_hd_t )_reqHd;
	
	if(this->bRun && this->bRegistered )
	{
		sip_hd_t *msgRinging = new sip_hd_t();
		if( msgRinging )
		{
			if( build_res_msg(msgRinging, 180, reqHd->sipfield.cseq.type,reqHd, "Ringing") > 0)
			{
				this->OnSendRinging(msgRinging);
				sipUAC->send_msg(msgRinging,0);
				delete msgRinging;
				return 1;
			}
			delete msgRinging;
		}
	}
    return 0;
}

int sky_sip_manager::build_req_msg(IN OUT sip_hd_t *_pSipHd, IN sip_method_type_t msgType,void * reqHd, void * callDialog)
{
	return build_msg ( _pSipHd, sip_request, 0,msgType, 0, 0,reqHd, 
		(char*)(((p_sip_hd_t)reqHd)->sipfield.to.uri.uid.size() > 0? ((p_sip_hd_t)reqHd)->sipfield.to.uri.uid.data():this->u_number), callDialog);
}

int sky_sip_manager::build_req_msg(IN OUT sip_hd_t *_pSipHd,IN sip_method_type_t msgType, char *targetNum, void * callDialog)
{
    return build_msg(_pSipHd, sip_request, 0,msgType, 0, 0,NULL, targetNum,callDialog);
}

int sky_sip_manager::build_req_msg(IN OUT sip_hd_t *_pSipHd,IN sip_method_type_t msgType,IN int bAuth, char *targetNum)
{
    return build_msg(_pSipHd, sip_request, 0,msgType, bAuth, 0,NULL,targetNum);
}

int sky_sip_manager::build_req_msg(IN OUT sip_hd_t *_pSipHd,IN sip_method_type_t msgType,IN int bAuth,IN char opt, char *targetNum)
{
    return build_msg(_pSipHd, sip_request, 0,msgType, bAuth, opt,NULL,targetNum);
}

int sky_sip_manager::build_res_msg(IN OUT sip_hd_t *_pSipHd,IN int stateCode,IN sip_method_type_t preType, IN p_sip_hd_t reqHd, IN char *szText, void *_pCallDialog)
{
    return build_msg(_pSipHd, sip_response,stateCode,preType, 0, 0,reqHd,szText, _pCallDialog);
}

int sky_sip_manager::build_pre_head_main(OUT p_pre_head_line_t pPreHead,IN char *pszHostAddr, IN char *targetNum)
{
    if( strlen(u_number)<=0 || strlen(hostip)<=0 )
        return 0;
    
    // Version
    //fill_sip_version(&pPreHead, SKY_MAJOR_VER, SKY_MINOR_VER);
	pPreHead->sipInf.build(sip_none_proto, SKY_MAJOR_VER, SKY_MINOR_VER);

    // uri information
    //if( pPreHead->msg_type == msg_register )
    //    build_uri(&pPreHead->uri, hostip, 0, u_number);
    //else
	pPreHead->sipUri.build(this->proxyip, 0, u_number);
    //pPreHead->proto_type = sip_none_proto;
    return 1;
}

int sky_sip_manager::build_pre_head(IN OUT p_pre_head_line_t pPreHead,
									IN header_type_e request,IN sip_method_type_t msgType,IN char *pszHostAddr, IN char *targetNum)
{
    if( pPreHead )
	{
		if( request == sip_request)
		{
			pPreHead->req_build(SKY_MAJOR_VER, SKY_MINOR_VER, msgType, targetNum, pszHostAddr, 0, sip_none_proto);
		}
		else
		{
			//pPreHead->res_build(SKY_MAJOR_VER, SKY_MINOR_VER, err_code);
			return 1;
		}
		return 1;
	}
	return 0;
    //return build_pre_head_main(pPreHead,pszHostAddr);
}

int sky_sip_manager::build_pre_head(IN OUT p_pre_head_line_t pPreHead,IN header_type_e request,IN char *szText,IN int err_code)
{
    pPreHead->hdtype = request;
    // State code
	pPreHead->state = err_code;
    //pPreHead->state_code = err_code;
	pPreHead->msg_type = msg_invalid;

    if( pPreHead )
	{
		if( request == sip_request)
		{
			return 0;
		}
		else
		{
			pPreHead->res_build(SKY_MAJOR_VER, SKY_MINOR_VER, err_code);
			pPreHead->text = szText;
			//pPreHead->res_build(SKY_MAJOR_VER, SKY_MINOR_VER);
		}
		return 1;
	}
	return 0;
}

int sky_sip_manager::build_register_a3_auth_info(IN void * _pAuth, OUT unsigned char *hexVal,IN char *sMsgType)
{
    struct MD5Context Md5Ctx;
    HASH HA2;
    HASHHEX HA2Hex;
    p_sip_authentication_t wwwauth = (p_sip_authentication_t )_pAuth;
    // calculate H(A2)
    MD5Init(&Md5Ctx);
    
    MD5Update(&Md5Ctx,  (unsigned char*)sMsgType, strlen(sMsgType));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
	MD5Update(&Md5Ctx,(unsigned char *)wwwauth->sip_uri.data(),wwwauth->sip_uri.size());
    //if (strcmp(pszQop, "auth-int") == 0)
    //{
    //    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    //    MD5Update(&Md5Ctx, (unsigned char*)(HEntity), HASHHEXLEN);
    //}
    MD5Final((unsigned char*)(HA2), &Md5Ctx);
    CvtHex(HA2, HA2Hex);
    sprintf((char*)hexVal, "%s", HA2Hex);
    return 1;
}

void sky_sip_manager::build_cnonce(char * cnonce)
{
    time_t tt;
    time(&tt);
    sprintf(cnonce, "%x", tt);
}

void sky_sip_manager::build_www_auth_response(IN void *pField,IN OUT void *_pAuth,IN char *sMsgType)
{
    struct MD5Context Md5Ctx;
    HASH HA1;
    HASH RespHash;
    unsigned char hexVal[48], a3Val[48], a1Val[48];
    char tbuf[32];
    p_sip_authentication_t wwwauth = (p_sip_authentication_t )_pAuth;
    // Step_1 : Calculate A1 H(u_number:auth_passwd:realm)
    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, (unsigned char*)(u_number), strlen(u_number));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(realm), strlen(realm));
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    MD5Update(&Md5Ctx, (unsigned char*)(auth_passwd), strlen(auth_passwd));
    MD5Final((unsigned char*)(HA1), &Md5Ctx);
    memset(a1Val, 0, 48);
    CvtHex(HA1, (char*)&a1Val[0]);
    // Step_2 : H(Step_1:nonce) Calculate A2
    memset(a3Val, 0, 48);
    build_register_a3_auth_info(_pAuth,a3Val,sMsgType);
    // Step_3 : calculate response
    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, a1Val, HASHHEXLEN);
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
	MD5Update(&Md5Ctx, (unsigned char*)wwwauth->nonce.data(), wwwauth->nonce.size());
    MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    //if (*pszQop)
    //{
#ifdef MSVC_6
        sprintf(tbuf,"%08d", wwwauth->nc);
#else
		sprintf_s(tbuf, 32, "%08d", wwwauth->nc);
#endif
        MD5Update(&Md5Ctx, (unsigned char*)&tbuf[0], strlen(tbuf));
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
#if 0
        if( wwwauth->type == AUTHORIZATION )
            MD5Update(&Md5Ctx, (unsigned char*)(CNONCE), 8);
        else if( wwwauth->type == PROXY_AUTHORIZATION )
            MD5Update(&Md5Ctx, (unsigned char*)(P_CNONCE), 16);
#else
		MD5Update(&Md5Ctx, (unsigned char*)wwwauth->cnonce.data(), wwwauth->cnonce.size());
        //MD5Update(&Md5Ctx, (unsigned char*)&tbuf[0], strlen(tbuf));
#endif
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
		MD5Update(&Md5Ctx, (unsigned char*)(wwwauth->qop.data()), wwwauth->qop.size());
        MD5Update(&Md5Ctx, (unsigned char*)(":"), 1);
    //}
    MD5Update(&Md5Ctx, a3Val, HASHHEXLEN);
    MD5Final((unsigned char*)(RespHash), &Md5Ctx);
    CvtHex(RespHash, (char*)&hexVal[0]);
    // copy response

    wwwauth->response=(char*)&hexVal[0];
}

int sky_sip_manager::build_www_auth_info(IN void * _pField,IN OUT void *_pAuth,IN char *sMsgType, char *targetNumber, char *contactIP)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
    p_sip_authentication_t wwwauth = (p_sip_authentication_t )_pAuth;
    char tbuf[128];
    if( wwwauth )
    {
        wwwauth->username=u_number;
        wwwauth->realm=realm;
        
        if( wwwauth->type == AUTHORIZATION )
        {	
			// swhors@naver.com
            //sprintf(tbuf, "sip:%s", "gmarket.co.kr");
            sprintf_s(tbuf, 128, "sip:%s", proxyip);
            wwwauth->sip_uri=tbuf;
            memset( tbuf, 0, 128);
            build_cnonce(tbuf);
            //copy_str(&wwwauth->cnonce, CNONCE);
            wwwauth->cnonce=tbuf;
            wwwauth->nonce=(char*)&www_nonce[0];
        }
        else if ( wwwauth->type == PROXY_AUTHORIZATION )
        {
            if(!memcmp( sMsgType, "INVITE", 6))
                sprintf_s(tbuf,128, "sip:%s@%s",targetNumber,proxyip);
            else
            {
                if( targetNumber[0] == '*')
                    sprintf_s(tbuf,128, "sip:%s@%s", targetNumber+1, contactIP);
                else
                    sprintf_s(tbuf,128,
						"sip:%s@%s", targetNumber, contactIP);
            }
            wwwauth->sip_uri=tbuf;
            memset( tbuf, 0, 128);
            build_cnonce(tbuf);
            //copy_str(&wwwauth->cnonce, P_CNONCE);
            wwwauth->cnonce=tbuf;
            wwwauth->nonce=(char*)&proxy_nonce[0];
        }
        
        wwwauth->qop="auth";
        wwwauth->algorithm = 1;
        wwwauth->stale = 2;

        wwwauth->nc = ++nc_count;
        build_www_auth_response(pField, _pAuth, sMsgType);
        return 1;
    }
    return 0;
}
//
// generate_cid()
// 콜 아이디를 생성하는 함수
//
int sky_sip_manager::generate_cid(IN OUT call_id_t *_callId, int type/*=0*/)
{
    //if( type == 0)
    return generate_callid(_callId,callid,128,hostip);
    //return generate_callid(registra_cid,128,hostip);
}

//
// build_body
// 프로토콜 스택의 바디를 채우는 함수
// IN header_type_e request
// IN sip_method_type_t msgType
//
int sky_sip_manager::build_body(IN OUT p_sip_hd_t pSipHd,IN OUT p_sip_field_t pSipField,IN header_type_e request,IN sip_method_type_t msgType,IN int bAuth,IN char opt, IN p_sip_hd_t requestedMsg, char *targetNum,void *_pcallDialog)
{
    char sbuff[256];
    int i;
	char szHostAddress[128];
	char szTargetNum[128];
	char szTag[128];
    // alloow
    memset(sbuff, 0, 256);
	call_info *pCallDialog = (call_info*)_pcallDialog;
		
    for(i = 0; i < 10; i++)
    {
        strcatfm(sbuff, "%s",allow_method[i]);
        if( i < 9 )
        {
            if( strlen( allow_method[i+1]) > 0 )
                strcat_s(sbuff,256,",");
            else
                break;
        }
    }

	if( pSipHd->preheadline.hdtype == sip_response )
	{
		sprintf_s(szTargetNum, 128, "%s", 
			(!requestedMsg->preheadline.sipUri.uid.empty()?requestedMsg->preheadline.sipUri.uid.data():u_number));
		HostaddrToStr(&requestedMsg->preheadline.sipUri.haddr, szHostAddress, 128);
	}
	else
	{
		sprintf_s(szTargetNum, 128, "%s", (targetNum?targetNum:u_number));
	}
    
	if( pSipHd->preheadline.hdtype == sip_response)
		pSipField->allow = requestedMsg->sipfield.allow;
	else
	{
		if( pSipHd->preheadline.msg_type == msg_invite ||
			pSipHd->preheadline.msg_type == msg_options ||
			pSipHd->preheadline.msg_type == msg_info ||
			pSipHd->preheadline.msg_type == msg_register)
		pSipField->allow = parse_allow_item((PUCHAR)&sbuff[0], NULL, (int)strlen(sbuff), 0);
	}
    
    // Max - Forwards
	if( pSipHd->preheadline.hdtype == sip_request )
		pSipField->max_forward = 60;
	else
		pSipField->max_forward = (requestedMsg?requestedMsg->sipfield.max_forward:60);
    
    // call id
	if( pSipHd->preheadline.hdtype == sip_request && pSipHd->preheadline.msg_type!= msg_ack)
	{
		if( generate_cid(&pSipField->cid) <= 0)
		{
			return 0;
		}
		if( pCallDialog && pSipHd->preheadline.msg_type != msg_invite)
			pSipHd->sipfield.cid = pCallDialog->sipCallid;
	}
	else
	{
		if( requestedMsg )
			pSipHd->sipfield.cid = requestedMsg->sipfield.cid;
	}

	if( pSipHd->preheadline.hdtype == sip_request )
	{
		if( msgType == msg_invite 
			|| msgType == msg_info || msgType == msg_ack ||
			msgType == msg_bye)
		{
#ifdef WIN32
			tr1::shared_ptr<sip_contact_hd_t> contact(new sip_contact_hd_t);
#elif defined LINUX || defined MACOS || defined WINCE
			sip_contact_hd_t *contact = new sip_contact_hd_t();
#endif

			if( stun_enable )
			{
				contact->build(u_number, hostip, port, NULL, 0, -1, u_number);
			}else
			{
				contact->build(u_number, hostip, SIPUASport, NULL, 0, -1, u_number);
			}
#ifdef WIN32
			pSipField->contact_list.push_back(contact);
#elif defined LINUX || defined MACOS || defined WINCE
			pSipField->contact_list.push_back(*contact);
#endif
		}
		else if ( msgType == msg_register)
		{
			int uasport,ic=0;
			if( stun_enable )
				uasport = port;
			else
				uasport = SIPUASport;
#ifdef WIN32
			tr1::shared_ptr<sip_contact_hd_t>contact(new sip_contact_hd_t);
#elif defined LINUX || defined MACOS || defined WINCE
			sip_contact_hd_t *contact = new sip_contact_hd_t();
#endif
			contact->build(u_number, hostip, uasport, NULL, 0, this->reg_ttl_val, NULL);
#ifdef WIN32
			pSipField->contact_list.push_back(contact);
#elif defined LINUX || defined MACOS || defined WINCE
			pSipField->contact_list.push_back(*contact);
#endif
		}
	}
	else
	{
	}
    
    // csequence information
	if( pSipHd->preheadline.hdtype == sip_request && pSipHd->preheadline.msg_type != msg_ack)
	{
		build_cseq(&pSipField->cseq,iSipCseqCount++,msgType);
	}
	else
	{
		if( !requestedMsg )
			build_cseq(&pSipField->cseq,iSipCseqCount++,msgType);
		else
			pSipField->cseq = requestedMsg->sipfield.cseq;
		if( pSipHd->preheadline.msg_type == msg_ack )
		{
			pSipField->cseq.type = msg_ack;
		}

	}
    
    // date information 
    if( msgType != msg_bye)
        build_date_msg(&pSipField->date);
    
    // expire information
	if( pSipHd->preheadline.hdtype == sip_request)
	{
		if( pSipHd->preheadline.msg_type == msg_register )
		{
			pSipField->expire = reg_ttl_val;
		}
		else
		{
			pSipField->expire = -1;
		}
	}
    
	// append via 
	if( pSipHd->preheadline.hdtype == sip_request )
	{
#ifdef WIN32
		tr1::shared_ptr<sip_via_t> pVia(new sip_via_t);
#elif defined LINUX || defined MACOS || defined WINCE
		sip_via_t *pVia = new sip_via_t();
#endif
		//if( pVia )
		//{
		if( stun_enable )
			pVia->build(sip_udp, 2, 0, this->hostip, this->port, u_branch, NULL, NULL);
		else
			pVia->build(sip_udp, 2, 0, this->hostip, 0, u_branch, NULL, NULL);
#ifdef WIN32
		pSipHd->sipfield.via_list.push_back(pVia);
#elif defined LINUX || defined MACOS || defined WINCE
		pSipHd->sipfield.via_list.push_back(*pVia);
#endif
		//}
	}
	else
	{
		if( requestedMsg->sipfield.via_list.size() > 0 )
		{
#ifdef WIN32
			for( vector<shared_ptr<sip_via_t>>::iterator v 
#elif defined LINUX || defined MACOS || defined WINCE
			for( vector<sip_via_t>::iterator v 
#endif
				= requestedMsg->sipfield.via_list.begin();
				v != requestedMsg->sipfield.via_list.end() ; v++ )
			{
#ifdef WIN32
				tr1::shared_ptr<sip_via_t> pVia(new sip_via_t);
				*pVia = *(v->get());
				pSipHd->sipfield.via_list.push_back(pVia);
#elif defined LINUX || defined MACOS || defined WINCE
				sip_via_t* pVia = new sip_via_t;
				*pVia = *v;
				pSipHd->sipfield.via_list.push_back(*pVia);
#endif
			}
		}
	}
	// build to/from uri information
    if( msgType == msg_register )
    {
		build_mytag(szTag, 128);
		pSipField->from.build(u_number, proxyip, SIPUASport, u_number, szTag,from_tag , 1);
		pSipField->to.build(u_number, proxyip,  SIPUASport, u_number, "",to_tag , 1);
    }
    else
    {
		// build from uri information
		if( pSipHd->preheadline.hdtype == sip_request )
		{
			build_mytag(szTag, 128);
			pSipField->from.build(u_number, proxyip, SIPUASport, u_number, szTag, from_tag, 1);
			if( pSipHd->preheadline.msg_type != msg_invite )
			{
				if( pCallDialog )
				{
					pSipField->from.tag = pCallDialog->self_tag;
				}
			}
		}
        else
		{
			if( requestedMsg )
			{
				pSipField->from = requestedMsg->sipfield.from;
			}
		}
        
        // build to uri information
        if( pSipHd->preheadline.hdtype == sip_request )
        {
			if( HostaddrToStr(&pSipHd->preheadline.sipUri.haddr, (char*)&szHostAddress, 128) == 0 )
				sprintf_s(szHostAddress, 128, "%s", this->proxyip);
			pSipField->to.build(szTargetNum, szHostAddress, 0, szTargetNum, "", to_tag, 1);
			//pCallDialog
			if( pSipHd->preheadline.msg_type != msg_invite )
			{
				if( pCallDialog )
				{
					pSipField->to.tag = pCallDialog->target_tag;
				}
			}
        }
        else
        {
			pSipField->to = requestedMsg->sipfield.to;
			if( requestedMsg->sipfield.cseq.type == msg_invite
					&& pSipHd->preheadline.state == 200 )
			{
				build_mytag(szTag, 128);
				pSipField->to.tag = szTag;
				if(pCallDialog )
				{
					pCallDialog->self_tag = szTag;
				}
			}
        }
    }
    
    // subject information
    if( pSipHd->preheadline.msg_type == msg_invite )
        pSipField->subject="Invite";
    
    // user agent information
    pSipField->user_agent="Simpson User Agent";
	
    if( bAuth )
    {
        if( msgType == msg_register )
        {
//            if( use_www_auth )
//              pSipHd->sipfield.www_auth.type = AUTHORIZATION;
    //        else if( proxy_auth )
                pSipHd->sipfield.www_auth.type = PROXY_AUTHORIZATION;
            
			build_www_auth_info(&pSipHd->sipfield, &pSipHd->sipfield.www_auth,"REGISTER", szTargetNum, this->proxyip);
        }
        else
        {
            pSipHd->sipfield.www_auth.type = PROXY_AUTHORIZATION;
            if( msgType == msg_invite )
                build_www_auth_info(&pSipHd->sipfield, &pSipHd->sipfield.www_auth,"INVITE", szTargetNum, this->proxyip);
            else if( msgType == msg_bye )
                build_www_auth_info(&pSipHd->sipfield, &pSipHd->sipfield.www_auth,"BYE", szTargetNum, this->proxyip);
            else if( msgType == msg_ack )
                build_www_auth_info(&pSipHd->sipfield, &pSipHd->sipfield.www_auth,"ACK", szTargetNum, this->proxyip);
            else if( msgType == msg_info )
                build_www_auth_info(&pSipHd->sipfield, &pSipHd->sipfield.www_auth,"INFO", szTargetNum, this->proxyip);
        }
    }

#if 0
    //if( CallInfo.bRecordRoute )
    {
        append_record_route(&pSipHd->sipfield.route, szTargetNum, sendip, 0, CallInfo.ftag, CallInfo.lr,"", 2);
        if( pSipHd->sipfield.route.route_uri )
        {
            if( strlen( CallInfo.rvRRParam ) > 0)
            {
#ifdef MSVC_6
                sprintf(pSipHd->sipfield.route.route_uri->rvRRParam, "%s", CallInfo.rvRRParam);
#else
                sprintf_s(pSipHd->sipfield.route.route_uri->rvRRParam, 32,"%s", CallInfo.rvRRParam);
#endif
            }
            pSipHd->sipfield.route.route_uri->port = 5060;
            memset(pSipHd->sipfield.route.route_uri->u_num, 0, 64);
        }
    }
#endif
    if( pSipHd->preheadline.hdtype == sip_request &&
        pSipHd->preheadline.msg_type == msg_info)
    {
        char tbuf[64];
        pSipField->content_type ="application/dtmf-relay";

#ifdef MSVC_6
        sprintf(tbuf, "Signal=%c\r\nDuration=960",opt);
#else
        sprintf_s(tbuf, 64,"Signal=%c\r\nDuration=960",opt);
#endif
        pSipHd->sipfield.content_length = strlen(tbuf);
    }
	else if( pSipHd->preheadline.msg_type == msg_invite ||
		(pSipHd->preheadline.hdtype == sip_response && pSipHd->sipfield.cseq.type == msg_invite) )
	{
		if( pCallDialog )
		{
			if( pCallDialog->bStartWithSDP )
			{
				build_SDP(pSipHd, (pCallDialog?pCallDialog->RTPport:10000), (pCallDialog?pCallDialog->RTPip:this->localip));
			}
			else if( pCallDialog->IsReInvite() )
			{
				build_SDP(pSipHd, 
							(pCallDialog?pCallDialog->RTPport:10000),
							(pCallDialog?pCallDialog->RTPip:this->localip),
							&requestedMsg->sipContent.sdp, 0);
			}
			pSipHd->sipfield.content_length = 0;
		}
		else
			build_SDP(pSipHd, (pCallDialog?pCallDialog->RTPport:10000), (pCallDialog?pCallDialog->RTPip:this->localip));	
	}
	else
	{
		pSipHd->sipfield.content_length = 0;
	}
    return 1;
}

// build_msg
// sip 스택 버퍼에 정보를 채우는 함수
// 인자 : IN header_type_e request
//        IN sip_method_type_t msgType
//        IN int err_code
int sky_sip_manager::build_msg(IN OUT sip_hd_t * _pSipHd, IN header_type_e request,IN int err_code,IN sip_method_type_t msgType,IN int bAuth,IN char opt, IN void * _requestedMsg, char *targetNum,void * _callDialog)
{
	p_sip_hd_t requestedMsg = (p_sip_hd_t)_requestedMsg;
	call_info * pCallDialog = (call_info *)_callDialog;
	p_sip_hd_t pSipInfo = _pSipHd;
	char hostAddr[128];	
	
	if( request == sip_response && requestedMsg==NULL) 
		return 0;
	pSipInfo->preheadline.free();
	//free_sip_preheader(&pSipInfo->preheadline);
	//init_sip_pre_hd_field(&pSipInfo->preheadline);
	
	// build header type
	pSipInfo->type = request;
	
	// build message
	// build session expires
	if( request == sip_request )
	{
		if( msgType == msg_register)
			build_pre_head(&pSipInfo->preheadline, request, msgType, this->proxyip, u_number);
		else
			build_pre_head(&pSipInfo->preheadline, request, msgType, this->proxyip, targetNum);
	}
	else if( request == sip_response )
	{
		if( HostaddrToStr(&requestedMsg->preheadline.sipUri.haddr, hostAddr, 128) == 0 )
			sprintf_s(hostAddr, 128, "%s", this->proxyip);
		pSipInfo->preheadline.state = err_code;
		build_pre_head(&pSipInfo->preheadline, request, targetNum, err_code);
	}
	else
		return 0;
	
	free_sip_field(&pSipInfo->sipfield);
	//init_sip_hd_field(&pSipInfo->sipfield);
	//pSipInfo->sipfield.session_expires = sbuff;
	// build message body
	if( !build_body(pSipInfo, &pSipInfo->sipfield, request, msgType,bAuth, opt, requestedMsg, targetNum, pCallDialog) )
		return 0;
	if( request == sip_request)
	{
		switch(msgType)
		{
		case msg_invite:
			if( pSipInfo->sipfield.contact_list.size() > 0 )
			{
#ifdef WIN32
				pSipInfo->sipfield.contact_list.begin()->get()->expires = -1;
#elif defined LINUX || defined MACOS || defined WINCE
				pSipInfo->sipfield.contact_list.begin()->expires = -1;
#endif
			}
			pSipInfo->sipfield.expire = -1;
			break;
		case msg_register:
			pSipInfo->sipfield.allow = ALLOW_INVITE_NUM|ALLOW_BYE_NUM|ALLOW_CANCEL_NUM|ALLOW_ACK_NUM|ALLOW_INFO_NUM|ALLOW_OPTIONS_NUM;
			pSipInfo->sipfield.to.display ="";
			pSipInfo->sipfield.from.display = "";
			if( pSipInfo->sipfield.contact_list.size() > 0 )
			{
#ifdef WIN32
				pSipInfo->sipfield.contact_list.begin()->get()->expires = -1;
#elif defined LINUX || defined MACOS || defined WINCE
				pSipInfo->sipfield.contact_list.begin()->expires = -1;
#endif
			}
			break;
		case msg_ack:
			if( requestedMsg)
			{
				pSipInfo->sipfield.to = requestedMsg->sipfield.to;
			}
			pSipInfo->preheadline.sipUri.transport = sip_udp;
			pSipInfo->preheadline.sipUri.haddr.port = 5060;
			pSipInfo->sipfield.to.uri.transport  = sip_none_proto;
			pSipInfo->sipfield.from.uri.transport  = sip_none_proto;
			pSipInfo->sipfield.from = requestedMsg->sipfield.from;

			if( pSipInfo->sipfield.contact_list.size() > 0 )
			{
#ifdef WIN32
				pSipInfo->sipfield.contact_list.begin()->get()->expires = -1;
				pSipInfo->sipfield.contact_list.begin()->get()->uri.transport = sip_none_proto;
#elif defined LINUX || defined MACOS || defined WINCE
				pSipInfo->sipfield.contact_list.begin()->expires = -1;
				pSipInfo->sipfield.contact_list.begin()->uri.transport = sip_none_proto;
#endif
			}
			if( requestedMsg->sipfield.record_list.size() > 0 )
			{
#ifdef WIN32
				for( vector<tr1::shared_ptr<record_route_uri_t>>::iterator d 
#elif defined LINUX || defined MACOS || defined WINCE
				for( vector<record_route_uri_t>::iterator d 
#endif
					= requestedMsg->sipfield.record_list.begin();
					d != requestedMsg->sipfield.record_list.end(); d++)
				{
					record_route_uri_t *p = new record_route_uri_t();
					if( p )
					{
#ifdef WIN32
						*p = *d->get();
#elif defined LINUX || defined MACOS || defined WINCE
						*p = *d;
#endif
						(*p).type = 1;
						(*p).uri.transport = sip_none_proto;
#ifdef WIN32
						tr1::shared_ptr<record_route_uri_t> p1(p);
						pSipInfo->sipfield.record_list.push_back(p1);
#elif defined LINUX || defined MACOS || defined WINCE
						pSipInfo->sipfield.record_list.push_back(*p);
#endif
					}
				}
			}
			pSipInfo->sipfield.expire = -1;
			pSipInfo->sipfield.allow = 0;

			break;
		case msg_bye:
			break;
		case msg_cancel:
			break;
		default:
			break;
		}
	}
	return 1;
}

void sky_sip_manager::init_sky_media_info()
{
    for( int i = 0 ; i< 10 ; i++ )
    {
        memset( mediaAtt[i].name, 0, 64);
        memset( mediaAtt[i].value, 0, 64);
        mediaAtt[i].bAudioCodec = 0;
        mediaAtt[i].num = 0;
    }
    media_count = 0;
    voice_codec = 0;
}

// 
// build sdp information
// ARG : IN p_sky_media_att pMediaAtt
//       IN int totalnum
//       IN int CallInfo.RTPport
void sky_sip_manager::build_SDP(OUT void *_sipHd, IN int _RTPport, char *szRtpIP, IN void * _pInSDP, IN int inCodecSelect)
{
    int randVal;
	sip_hd_t *pSipHd = (sip_hd_t*)_sipHd;

    srand(GetTickCount());
    randVal= rand();
    // content type
	pSipHd->sipfield.content_type = "application/sdp";
	
	pSipHd->sipContent.eContentType = e_sipcontent_sdp;
		
	// Session Name 
	// s= "~~"
	pSipHd->sipContent.sdp.session_name = "gmarket_sip_session";
	// Owner/Creator, Sessio ID
	// o= (owner name) (session ID) (session version) (owner Network Type) (Owner Address Type) (Owner Address)
	pSipHd->sipContent.sdp.owner.build(u_number, randVal, randVal, (szRtpIP? szRtpIP:localip));
	//pSipHd->sipContent.sdp.owner.build(u_number, randVal, randVal, "211.115.66.98");
	// Time Description
	pSipHd->sipContent.sdp.time.build(0, 0);
	if( !_pInSDP )
	{
		// Connection Information
		// c= IN IP4 x.x.x.x x
		SDPMEDIAINF *m = new SDPMEDIAINF();
		pSipHd->sipContent.sdp.connection.build((szRtpIP? szRtpIP:localip));
		//m->connection.build((szRtpIP? szRtpIP:localip));
		m->appendMediaAtt("rtpmap", 0,"PCMU/8000");
		if( pSipHd->preheadline.hdtype == sip_request)
		{
			m->appendMediaAtt("rtpmap", 8,"PCMA/8000");
		}
		m->appendMediaAtt("fmtp",101, "0-15,36");
		m->appendMediaAtt("rtpmap",101, "telephone-event/8000");
		m->appendMediaAtt("ptime",20, "");
		m->appendMediaAtt("sendrecv", -1, "");
		if( pSipHd->preheadline.hdtype== sip_request)
			m->m_desc.build("audio", "RTP/AVP", _RTPport, 3, 0, 8, 101);
		else
			m->m_desc.build("audio", "RTP/AVP", _RTPport, 2, 0, 101);
#ifdef WIN32
		tr1::shared_ptr<SDPMEDIAINF> m1(m);
		pSipHd->sipContent.sdp.media.push_back(m1);
#elif defined LINUX || defined MACOS || defined WINCE
		pSipHd->sipContent.sdp.media.push_back(*m);
#endif
	}
	else
	{
		sip_sdp *pInSDP = (sip_sdp*)_pInSDP;
#ifdef WIN32
		for( vector<tr1::shared_ptr<sdp_media_info>>::iterator p= 
#elif defined LINUX || defined MACOS || defined WINCE
		for( vector<sdp_media_info>::iterator p= 
#endif
				pInSDP->media.begin();p != pInSDP->media.end() ; p++ )
		{
#ifdef WIN32
			if( p->get()->m_desc.IsAudio() )
#elif defined LINUX || defined MACOS || defined WINCE
			if( p->m_desc.IsAudio() )
#endif
			{
				//if( p->get()->m_desc.format
#ifdef WIN32
				shared_ptr<SDPMEDIAINF> m2(new SDPMEDIAINF);
				if( p->get()->m_desc.HaveMyMedia(inCodecSelect) )
#elif defined LINUX || defined MACOS || defined WINCE
				SDPMEDIAINF *m2 = new SDPMEDIAINF;
				if( p->m_desc.HaveMyMedia(inCodecSelect) )
#endif
				{
					int iDTMF = 96;
#ifdef WIN32
					if( !p->get()->m_desc.HaveMyMedia(iDTMF) )
#elif defined LINUX || defined MACOS || defined WINCE
					if( !p->m_desc.HaveMyMedia(iDTMF) )
#endif
					{
						iDTMF = 101;
#ifdef WIN32
						if( p->get()->m_desc.HaveMyMedia(iDTMF) )
#elif defined LINUX || defined MACOS || defined WINCE
						if( p->m_desc.HaveMyMedia(iDTMF) )
#endif
						{
							iDTMF = -1;
						}
					}
#ifdef WIN32
					for(vector<tr1::shared_ptr<sdp_media_att>>::iterator p1= p->get()->m_att.begin();
															p1 != p->get()->m_att.end() ; p1++)
#elif defined LINUX || defined MACOS || defined WINCE
					for(vector<sdp_media_att>::iterator p1=p->m_att.begin(); p1 != p->m_att.end() ; p1++)
#endif
					{
#ifdef WIN32
						if( p1->get()->format == inCodecSelect )
#elif defined LINUX || defined MACOS || defined WINCE
						if( p1->format == inCodecSelect )
#endif
						{
#ifdef WIN32
							m2->appendMediaAtt((char*)p1->get()->fieldname.data(), p1->get()->format, (char*)p1->get()->type.data());
#elif defined LINUX || defined MACOS || defined WINCE
							m2->appendMediaAtt((char*)p1->fieldname.data(), p1->format, (char*)p1->type.data());
#endif
						}
					}
					if( iDTMF > 0 )
					{
#ifdef WIN32
						for(vector<tr1::shared_ptr<sdp_media_att>>::iterator p1=p->get()->m_att.begin();
							p1 != p->get()->m_att.end() ; p1++)
#elif defined LINUX || defined MACOS || defined WINCE
						for(vector<sdp_media_att>::iterator p1=p->m_att.begin(); p1 != p->m_att.end() ; p1++)
#endif
						{
#ifdef WIN32
							if( p1->get()->format == iDTMF )
#elif defined LINUX || defined MACOS || defined WINCE
							if( p1->format == iDTMF )
#endif
							{
								//m2->appendMediaAtt((char*)p1->get()->fieldname.data(), p1->get()->format,(char*) p1->get()->type.data());
							}
						}
					}
#ifdef WIN32
					if( p->get()->bandWidthInfo.Modifier.size() > 0 &&
						p->get()->bandWidthInfo.Value.size() > 0 )
					{
						m2->bandWidthInfo = p->get()->bandWidthInfo;
#elif defined LINUX || defined MACOS || defined WINCE
					if( p->bandWidthInfo.Modifier.size() > 0 &&
						p->bandWidthInfo.Value.size() > 0 )
					{
						m2->bandWidthInfo = p->bandWidthInfo;
#endif
					}
					else
					{
						m2->bandWidthInfo.Modifier = "AS";
						m2->bandWidthInfo.Value = "64";
					}
					m2->appendMediaAtt("rtpmap", 96, "telephone-event/8000");
					m2->appendMediaAtt("fmtp", 96, "0-11");
					m2->appendMediaAtt("ptime", 20, "");
					m2->m_desc.build("audio", "RTP/AVP", _RTPport, 2, inCodecSelect,iDTMF);
					m2->connection.build((szRtpIP? szRtpIP:localip));
#ifdef WIN32
					pSipHd->sipContent.sdp.media.push_back(m2);
#elif defined LINUX || defined MACOS || defined WINCE
					pSipHd->sipContent.sdp.media.push_back(*m2);
#endif
				}
			}
#ifdef WIN32
			else if( !p->get()->m_desc.IsAudio() )
#elif defined LINUX || defined MACOS || defined WINCE
			else if( !p->m_desc.IsAudio() )
#endif
			{
#ifdef WIN32
				shared_ptr<SDPMEDIAINF> m3(new SDPMEDIAINF);
#elif defined LINUX || defined MACOS || defined WINCE
				SDPMEDIAINF *m3 = new SDPMEDIAINF;
#endif
				m3->m_desc.build("video", "RTP/AVP", 0,1,34);
				m3->appendMediaAtt("inactive", -1, "");
				m3->connection.build((szRtpIP? szRtpIP:localip));
#ifdef WIN32
				pSipHd->sipContent.sdp.media.push_back(m3);
#elif defined LINUX || defined MACOS || defined WINCE
				pSipHd->sipContent.sdp.media.push_back(*m3);
#endif
			}
		}
	}
}

int	sky_sip_manager::PutInData(IN char *mem, IN int len)
{
	char *newBuf = new char[len + 1];
	if( newBuf )
	{
		memset(newBuf, 0, len+ 1);
		memcpy(newBuf, mem, len);
		//auto_ptr<RcvDataPtr> p(new RcvDataPtr);
		RcvDataPtr *p = new RcvDataPtr();
		p->len = len;
		p->Ptr = newBuf;
		p->pManager = this;
		p->hProcessHandle = CreateThread(NULL,0,&received_msg_Process, p, 0, &p->dwThreadId);
		return 1;
	}
	return 0;
}

//
// ParseData
// Async 소켓에서 받은 데이터를 파싱하는 함수
// argument : IN char *mem ( 수신받은 데이터 )
//            IN int len ( 수신받은 데이터의 길이 )
//
int sky_sip_manager::ParseData(IN char *mem,IN int len)
{
	SKY_HOOK_FN *hookfn;
	p_sip_hd_t hd;
	if( !bRun )
		return 0;
	
	sip_hd_t *sipHd = new sip_hd_t();
	if( sipHd )
	{
		LOGWRITE("1");
		LOGWRITE(mem);
		if( !bRun )
			return 0;
		if( (hd = parse_sip_msg((unsigned char*)mem, len, sipHd)) != NULL )
		{
			LOGWRITE("2");
			if( hd )
			{
				if( hd->sipfield.cid.callid == "0" )
				{
					// 에러 처리
					//
					LOGWRITE("잘못된 메세지 : CallID=0");
					delete hd;
					return 0;
				}
				if( hd->preheadline.hdtype == sip_request )
				{
					//CallInfo.pre_msg_type = (sip_method_type_t)hd->preheadline.msg_type;
					if( hd->preheadline.msg_type == msg_invite )
					{
						BOOL	bProcess=true;
						if( hookFN.rinvite )
						{
							hookfn = (SKY_HOOK_FN*)hookFN.rinvite;
							bProcess = (!hookfn(hd)?false:true);
						}
						if( bProcess )
						{
							EnterCriticalSection(&sky_sip_critical);
							EnterCriticalSection(&sky_sip_critical_dlg);
							//vector<shared_ptr<call_info>>::iterator d = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), );

#ifdef WIN32
							vector<shared_ptr<call_info>>::iterator d
								= find_if(m_CallDialogBuffer.begin(),
										  m_CallDialogBuffer.end(),
										  tr1::bind( equal_to<string>(),
											         bind(&call_info::target_number,tr1::placeholders::_1),
													 hd->sipfield.from.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
							vector<call_info>::iterator d
								= find(
										m_CallDialogBuffer.begin(),
										m_CallDialogBuffer.end(), 
										(char*)hd->sipfield.from.uri.uid.c_str());
#endif

							if( d == m_CallDialogBuffer.end() )
							{
#ifdef WIN32
								shared_ptr<call_info> p(new call_info());
#elif defined LINUX || defined MACOS || defined WINCE
								call_info *p=new call_info();
#endif
								p->sipCallid = hd->sipfield.cid;
								p->target_number = hd->sipfield.from.uri.uid;
								p->target_tag = hd->sipfield.from.tag;
#ifdef WIN32
								m_CallDialogBuffer.push_back(p);
#elif defined LINUX || defined MACOS || defined WINCE
								m_CallDialogBuffer.push_back(*p);
#endif

#ifdef WIN32
								vector<shared_ptr<call_info>>::iterator d
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,tr1::placeholders::_1),
												hd->sipfield.from.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
								vector<call_info>::iterator	d 
										= find( m_CallDialogBuffer.begin(),
												m_CallDialogBuffer.end(),
												hd->sipfield.cid);
#endif
							}
							else
							{
#ifdef WIN32
								d->get()->ReInVite(TRUE);
#elif defined LINUX || defined MACOS || defined WINCE
								d->ReInVite(TRUE);
#endif
							}
							
							if( this->m_bAutoResponse )
							{
								//this->send_ok_to_invite(
#ifdef WIN32
								send_ok(hd, d->get());
#elif defined LINUX || defined MACOS || defined WINCE
								// swhors@naver.com 
								// 2009/11/17 아래의 (void*)&d 부분에서 잘못된 주소 참조가 
								// 발생할 수 있으므로.. 주의..
								send_ok(hd, (void*)&d);
#endif
							}
							this->OnRecvNewCall(hd);
							LeaveCriticalSection(&sky_sip_critical_dlg);
							//delete hd;
							LeaveCriticalSection(&sky_sip_critical);
							return 1;
						}
						delete hd;
						return 0;
					}
					else if( hd->preheadline.msg_type == msg_bye )
					{
						//EnterCriticalSection(&sky_sip_critical_dlg);

#ifdef WIN32
						vector<shared_ptr<call_info>>::iterator a
   										= find_if(m_CallDialogBuffer.begin(),
													m_CallDialogBuffer.end(),
													tr1::bind(equal_to<string>(), 
													          bind(&call_info::target_number,tr1::placeholders::_1),
													          hd->sipfield.from.uri.uid
															 )
												 );
#elif defined LINUX || defined MACOS || defined WINCE
						vector<call_info>::iterator a = find(
											m_CallDialogBuffer.begin(),
											m_CallDialogBuffer.end(),
											hd->sipfield.cid);
#endif

						if( a != m_CallDialogBuffer.end() )
						{
#ifdef USING_HOOK
							if( hookFN.rbye )
							{
								hookfn = (SKY_HOOK_FN*)hookFN.rbye;
								hookfn(hd);
							}

#else
							this->OnRecvEndCall(hd);
#endif
							send_ok(hd);
#ifdef WIN32
							a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
							a->iDelete = 1;
#endif
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							return 1;
							//m_CallDialogBuffer.erase(a);
						}
						else
						{
							this->send_fail(hd, 418, "Call Leg/Transaction Does Not Exist");
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							return 0;
						}
						//LeaveCriticalSection(&sky_sip_critical_dlg);
					}
					else if( hd->preheadline.msg_type == msg_cancel )
					{
						EnterCriticalSection(&sky_sip_critical);
						//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
						vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.from.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
						vector<call_info>::iterator a = find(
												m_CallDialogBuffer.begin(),
												m_CallDialogBuffer.end(),
												hd->sipfield.cid);
#endif
						if( a != m_CallDialogBuffer.end() )
						{
#ifdef WIN32
							vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<string>(), 
															bind(&CSipCallStack::cid,
															tr1::placeholders::_1), hd->sipfield.cid.callid)
														);
#elif defined LINUX || defined MACOS || defined WINCE
							vector<CSipCallStack>::iterator c 
												= find(  m_MsgBuffer.begin(),
														 m_MsgBuffer.end(),
														 hd->sipfield.cid.callid);
#endif
							if( c!= m_MsgBuffer.end())
							{
#ifdef WIN32
								c->get()->bDelete = true;
#elif defined LINUX || defined MACOS || defined WINCE
								c->bDelete = true;
#endif
							}
							if( hookFN.rcancel )
							{
								hookfn = (SKY_HOOK_FN*)hookFN.rcancel;
								hookfn(hd);
							}
							send_ok(hd);
#ifdef WIN32
							a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
							a->iDelete = 1;
#endif
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							LeaveCriticalSection(&sky_sip_critical);
							return 1;
						}
						this->send_fail(hd, 418, "Call Leg/Transaction Does Not Exist");
						delete hd;
						//LeaveCriticalSection(&sky_sip_critical_dlg);
						LeaveCriticalSection(&sky_sip_critical);
						return 0;
					}
					else if( hd->preheadline.msg_type == msg_info )
					{
						this->OnRecvInfo(hd);
						send_ok(hd);
						//else
						//	send_fail(hd, 481, "Call Leg/Transaction Does Not Exist");
					}
					else if( hd->preheadline.msg_type == msg_ack )
					{
						//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
						vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.from.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
						vector<call_info>::iterator a = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), hd->sipfield.cid);
#endif

						if( a != m_CallDialogBuffer.end() )
						{
							OnRecvNewCallAck(NULL);
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							return 1;
						}
						else
						{
							send_fail(hd, 481, "Session Not Found");
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							return 0;
						}
						//LeaveCriticalSection(&sky_sip_critical_dlg);
					}
					else if( hd->preheadline.msg_type == msg_register )
					{
						delete hd;
						// fault message
						return 0;
					}
					else if( hd->preheadline.msg_type == msg_options )
					{
						if( hd->sipfield.cid.callid.size()> 0)
						{
							if(!hd->sipfield.cid.callid.compare(callid))
							{
								;
							}
						}

					}
					return 0;
				}
				else if( hd->preheadline.hdtype == sip_response )
				{
					//CallInfo.pre_msg_type = hd->sipfield.cseq.type;
					//CallInfo.recv_other_msg = hd->sipfield.cseq.cseq;
					LOGWRITE("5");
					int iSeqNumber = hd->sipfield.cseq.cseq * 100 + hd->sipfield.cseq.type;
					if( hd->preheadline.state == 100 )
					{
						LOGWRITE("51");
						EnterCriticalSection(&sky_sip_critical);
						this->OnRecvTrying(hd);
#ifdef WIN32
						vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<int>(), 
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
														);
#elif defined LINUX || defined MACOS || defined WINCE
						vector<CSipCallStack>::iterator c 
												= find(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															iSeqNumber);
#endif
						if( c != m_MsgBuffer.end())
						{
#ifdef WIN32
							c->get()->eState = e_msg_st_rv_preans;
#elif defined LINUX || defined MACOS || defined WINCE
							c->eState = e_msg_st_rv_preans;
#endif
						}

#if USING_HOOK
						if( hookFN.rwaiting )
						{
							hookfn = (SKY_HOOK_FN*)hookFN.rwaiting;
							hookfn(hd);
						}
#endif
						delete hd;
						LeaveCriticalSection(&sky_sip_critical);
						return 1;
					}
					else if( hd->preheadline.state == 180 )
					{
						if( hd->sipfield.cseq.type == msg_invite )
						{
							//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
							vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
							vector<call_info>::iterator a = 
											find(	m_CallDialogBuffer.begin(),
													m_CallDialogBuffer.end(),
													hd->sipfield.cid);
#endif
							if( a != m_CallDialogBuffer.end() )
							{
								EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
								vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<int>(), 
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
														);
#elif defined LINUX || defined MACOS || defined WINCE
								vector<CSipCallStack>::iterator c 
												= find( m_MsgBuffer.begin(),
														m_MsgBuffer.end(),
														iSeqNumber);
#endif
								if( c != m_MsgBuffer.end())
								{
									//c->eState = e_msg_st_rv_preans;
#ifdef WIN32
									a->get()->self_tag = hd->sipfield.from.tag;
									a->get()->target_tag = hd->sipfield.to.tag;
									a->get()->iSentCseq = hd->sipfield.cseq.cseq;
#elif defined LINUX || defined MACOS || defined WINCE
									a->self_tag = hd->sipfield.from.tag;
									a->target_tag = hd->sipfield.to.tag;
									a->iSentCseq = hd->sipfield.cseq.cseq;
#endif
								}
								OnRecvRinging(hd);
#ifdef USING_HOOK
								if( hookFN.rringing )
								{
									hookfn = (SKY_HOOK_FN*)hookFN.rwaiting;
									hookfn(hd);
								}
#endif
								delete hd;
								//LeaveCriticalSection(&sky_sip_critical_dlg);
								LeaveCriticalSection(&sky_sip_critical);
								return 1;
							}
							this->send_fail(sipHd, 481, "Call Leg/Transaction Does Not Exist");
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							LeaveCriticalSection(&sky_sip_critical);
							return 0;
						}
					}
					else if( hd->preheadline.state == 183 )
					{
						delete hd;
						return 1;
					}
					else if( hd->preheadline.state == 200 )
					{
						/// 200 프로세서 시작
#ifdef USING_HOOK
						if( hookFN.rok )
						{
							hookfn = (SKY_HOOK_FN*)hookFN.rok;
							if(! hookfn(hd) )
								return 0;
						}
#endif
						if( !bRun )
						{
							delete hd;
							return 0;
						}
						if(hd->sipfield.cseq.type ==msg_invite)
						{
							EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
							vector<shared_ptr<CSipCallStack>>::iterator c 
											= find_if(  m_MsgBuffer.begin(),
														m_MsgBuffer.end(),
														tr1::bind(equal_to<int>(), 
														bind(&CSipCallStack::seqNumber,
														tr1::placeholders::_1), iSeqNumber)
													);
#elif defined LINUX || defined MACOS || defined WINCE
							vector<CSipCallStack>::iterator c 
											= find( m_MsgBuffer.begin(),
													m_MsgBuffer.end(),
													iSeqNumber);
#endif
							if( c != m_MsgBuffer.end())
							{
#ifdef WIN32
								c->get()->pSipHd->sipfield.to = hd->sipfield.to;
#elif defined LINUX || defined MACOS || defined WINCE
								c->pSipHd->sipfield.to = hd->sipfield.to;
#endif
								if( hd->sipfield.record_list.size() > 0 )
								{
#ifdef WIN32
									for( vector<shared_ptr<record_route_uri_t>>::iterator d 
#elif defined LINUX || defined MACOS || defined WINCE
									for( vector<record_route_uri_t>::iterator d 
#endif
										= hd->sipfield.record_list.begin();
										d != hd->sipfield.record_list.end(); d++)
									{
										record_route_uri_t *p = new record_route_uri_t();
										if( p )
										{
#ifdef WIN32
											*p = *d->get();
											tr1::shared_ptr<record_route_uri_t> p1(p);
											c->get()->pSipHd->sipfield.record_list.push_back(p1);
#elif defined LINUX || defined MACOS || defined WINCE
											*p = *d;
											c->pSipHd->sipfield.record_list.push_back(*p);
#endif
										}
									}
								}
								BOOL bStartWithSDP = FALSE;
								EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
								vector<shared_ptr<call_info>>::iterator a
									= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
									tr1::bind(equal_to<string>(), 
									bind(&call_info::target_number,
									tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
								vector<call_info>::iterator a
									= find(m_CallDialogBuffer.begin(), 
											m_CallDialogBuffer.end(),
											(char*)hd->sipfield.to.uri.uid.c_str());
#endif
								if( a != m_CallDialogBuffer.end() )
								{
#ifdef WIN32
									a->get()->self_tag = hd->sipfield.from.tag;
									a->get()->target_tag = hd->sipfield.to.tag;
									a->get()->iSentCseq = hd->sipfield.cseq.cseq;
									bStartWithSDP = a->get()->bStartWithSDP;
#elif defined LINUX || defined MACOS || defined WINCE
									a->self_tag = hd->sipfield.from.tag;
									a->target_tag = hd->sipfield.to.tag;
									a->iSentCseq = hd->sipfield.cseq.cseq;
									bStartWithSDP = a->bStartWithSDP;
#endif
								}
								LeaveCriticalSection(&sky_sip_critical_dlg);
			 					OnRecvReqCallOk(hd);
								if( this->m_bAutoResponse )
								{
									if( bStartWithSDP )
#ifdef WIN32
										send_ack(c->get()->pSipHd);
#elif defined LINUX || defined MACOS || defined WINCE
										send_ack(c->pSipHd);
#endif
									else
									{
#ifdef WIN32
										c->get()->pSipHd->sipContent.sdp = hd->sipContent.sdp;
										this->send_ack_withSDP(c->get()->pSipHd, 0);
#elif defined LINUX || defined MACOS || defined WINCE
										c->pSipHd->sipContent.sdp = hd->sipContent.sdp;
										this->send_ack_withSDP(c->pSipHd, 0);
#endif
									}
#ifdef WIN32
									c->get()->bDelete = true;
									SetEvent(c->get()->hWakeEvent);
#elif defined LINUX || defined MACOS || defined WINCE
									c->bDelete = true;
#ifdef WINCE
									SetEvent(c->hWakeEvent);
#endif
#endif
								}
								delete hd;
								LeaveCriticalSection(&sky_sip_critical);
								return 1;
							}
							LeaveCriticalSection(&sky_sip_critical);
							delete hd;
							return 0;
							//a->iDelete = 1;
						}
						else if(hd->sipfield.cseq.type == msg_register )
						{
							// swhors@naver.com 09/09/11
							if( this->bRun == 0)
								return 0;
							EnterCriticalSection(&sky_sip_critical);
							WriteManagerLog(1, "register : recv OK", -1, (char*)hd->sipfield.cid.callid.c_str());
#ifdef WIN32
							vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
#elif defined LINUX || defined MACOS || defined WINCE
							vector<CSipCallStack>::iterator c 
												= find(  m_MsgBuffer.begin(),
#endif
															m_MsgBuffer.end(),
#ifdef WIN32
															tr1::bind(equal_to<int>(),
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
#elif defined LINUX || defined MACOS || defined WINCE
															iSeqNumber
#endif
														);

							if( c != m_MsgBuffer.end() )
							{
								OnRecvRegisterOk(hd);
								if( hd->sipfield.expire > 0 )
									bRegistered = 1;
								else
									bRegistered = 0;
								//RegisterInfo.contact_point = hd->sipfield.contact_count;
#ifdef WIN32
								c->get()->bDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
								c->bDelete = 1;
#endif
								WriteManagerLog(1, "register : remove session OK", -1, (char*)hd->sipfield.cid.callid.c_str());

#if defined WINCE || defined WIN32
#ifdef WIN32
								SetEvent(c->get()->hWakeEvent);
#elif defined WINCE
								SetEvent(c->hWakeEvent);
#endif
#endif
								delete hd;
								LeaveCriticalSection(&sky_sip_critical);
								return 1;
							}
							delete hd;
							LeaveCriticalSection(&sky_sip_critical);
							return 0;
						}
						else if(hd->sipfield.cseq.type == msg_bye )
						{
							//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
							vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
							vector<call_info>::iterator a = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), hd->sipfield.cid);
#endif
							if( a != m_CallDialogBuffer.end() )
							{
								EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
								vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<int>(), 
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
														);
#elif defined LINUX || defined MACOS || defined WINCE
								vector<CSipCallStack>::iterator c 
												= find( m_MsgBuffer.begin(),
														m_MsgBuffer.end(), iSeqNumber);
#endif

								if( c != m_MsgBuffer.end())
								{
#ifdef WIN32
									if( !c->get()->bDelete )
#elif defined LINUX || defined MACOS || defined WINCE
									if( !c->bDelete )
#endif
									{
										this->OnRecvMsgByeOK(hd);
#ifdef WIN32
										c->get()->bDelete = 1;
										a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
										c->bDelete = 1;
										a->iDelete = 1;
#endif
										delete hd;

#if defined WIN32 || defined WINCE
#ifdef WIN32
										SetEvent(c->get()->hWakeEvent);
#elif defined WINCE
										SetEvent(c->hWakeEvent);
#endif
#endif
										LeaveCriticalSection(&sky_sip_critical);
										//LeaveCriticalSection(&sky_sip_critical_dlg);
										return 1;
									}
								}
#ifdef WIN32
								a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || define WINCE
								a->iDelete = 1;
#endif
								delete hd;
								LeaveCriticalSection(&sky_sip_critical);
								//LeaveCriticalSection(&sky_sip_critical_dlg);
								return 0;
							}
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							return 0;

						}
						else if(hd->sipfield.cseq.type == msg_cancel)
						{
							//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
							vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
							vector<call_info>::iterator a = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), hd->sipfield.cid);
#endif

							if( a != m_CallDialogBuffer.end() )
							{
								EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
								vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<int>(), 
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
														);
#elif defined LINUX || defined MACOS || defined WINCE
								vector<CSipCallStack>::iterator c 
												= find( m_MsgBuffer.begin(),
														m_MsgBuffer.end(),
														iSeqNumber
														);
#endif

								if( c != m_MsgBuffer.end())
								{
#ifdef WIN32
									if( !c->get()->bDelete )
#elif defined LINUX || defined MACOS || defined WINCE
									if( !c->bDelete )
#endif
									{
										this->OnRecvCancelOK(hd);
#ifdef WIN32
										c->get()->bDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
										c->bDelete = 1;
#endif
										delete hd;
#if defined WIN32 || defined WINCE
#ifdef WIN32
										SetEvent(c->get()->hWakeEvent);
#elif defined WINCE
										SetEvent(c->hWakeEvent);
#endif
#endif
										LeaveCriticalSection(&sky_sip_critical);
										//LeaveCriticalSection(&sky_sip_critical_dlg);
										return 1;
									}
								}
								delete hd;
								LeaveCriticalSection(&sky_sip_critical);
								//LeaveCriticalSection(&sky_sip_critical_dlg);
								return 0;
							}
							//LeaveCriticalSection(&sky_sip_critical_dlg);
						}
						/// 200 프로세서 종료
						else
						{
							this->OnRecvOK(hd);
							delete hd;
							return 1;
						}
					}
					else if( hd->preheadline.state == 401 )
					{
						//if( strlen( callid ) <= 0)
						//    return 0;
						//if(! memcmp(callid, hd->sipfield.cid.callid.data(), hd->sipfield.cid.callid.size()))
						//{

						OnRecvFailMsg((void *)hd->preheadline.state);
						memset( realm, 0, 64);
						memset( uri, 0, 128);
						memset( www_nonce, 0, 48);
						if( hd->sipfield.www_auth.realm.size() > 0)
							sprintf_s(realm , 64,
							"%s", hd->sipfield.www_auth.realm.data());
						if( hd->sipfield.www_auth.sip_uri.size() > 0)
							sprintf_s(uri, 128,
							"%s", hd->sipfield.www_auth.sip_uri.data());
						if( hd->sipfield.www_auth.nonce.size() > 0)
							sprintf_s(www_nonce, 48,
							"%s", hd->sipfield.www_auth.nonce.data());
						//use_www_auth = 1;
						if( !bRegistered )
							send_register(0);
						else
							send_register(reg_ttl_val);
						delete hd;
						return 1;
					}
					else if( hd->preheadline.state == 407 )
					{
						delete hd;
						return 1;
					}
					else
					{
						EnterCriticalSection(&sky_sip_critical);
#ifdef WIN32
						vector<shared_ptr<CSipCallStack>>::iterator c 
												= find_if(  m_MsgBuffer.begin(),
															m_MsgBuffer.end(),
															tr1::bind(equal_to<int>(), 
															bind(&CSipCallStack::seqNumber,
															tr1::placeholders::_1), iSeqNumber)
														);
#elif defined LINUX || defined MACOS || defined WINCE
						vector<CSipCallStack>::iterator c 
												= find( m_MsgBuffer.begin(), m_MsgBuffer.end(), iSeqNumber );
#endif

						if( c != m_MsgBuffer.end())
						{
							if( hd->sipfield.cseq.type == msg_invite)
							{
								OnFailReqCall(hd);
								//EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
								vector<shared_ptr<call_info>>::iterator a
												= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
													tr1::bind(equal_to<string>(), 
													bind(&call_info::target_number,
													tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
								vector<call_info>::iterator a = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), hd->sipfield.cid);
#endif

								if( a != m_CallDialogBuffer.end() )
								{
#ifdef WIN32
									a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
									a->iDelete = 1;
#endif
								}
								//this->OnRecvInviteFail(hd);
								if( hd->preheadline.state == 489 )
								{
								}
								else if( hd->preheadline.state == 484 
									|| hd->preheadline.state == 404 )
								{
									// not found error    : 404
									// address incomplete : 484
								}
							}
							else if( hd->sipfield.cseq.type == msg_cancel)
							{
								this->OnRecvFailMsg(hd);
								if( hd->preheadline.state == 481 )
								{
#ifdef WIN32
									vector<shared_ptr<call_info>>::iterator a
											= find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
												tr1::bind(equal_to<string>(), 
												bind(&call_info::target_number,
												tr1::placeholders::_1), hd->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
									vector<call_info>::iterator a = find(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(), hd->sipfield.cid);
#endif

									if( a != m_CallDialogBuffer.end() )
									{
#ifdef WIN32
										a->get()->iDelete = 1;
#elif defined LINUX || defined MACOS || defined WINCE
										a->iDelete = 1;
#endif
									}
									else
									{
										this->send_fail(sipHd, 481, "Call Leg/Transaction Does Not Exist");
									}
								}
								send_ack(hd);
							}
							else
							{
								this->OnRecvFailMsg(hd);
							}
							delete hd;
							//LeaveCriticalSection(&sky_sip_critical_dlg);
							LeaveCriticalSection(&sky_sip_critical);
#ifdef WIN32
							c->get()->bDelete = true;
							SetEvent(c->get()->hWakeEvent);
#elif defined LINUX || defined MACOS || defined WINCE
							c->bDelete = true;
#ifdef WINCE
							SetEvent(c->hWakeEvent);
#endif
#endif

							return 1;
						}
						//this->send_fail(sipHd, 481, "Call Leg/Transaction Does Not Exist");
						delete hd;
						LeaveCriticalSection(&sky_sip_critical);
						return 0;
					}
				}
			}
			else
			{
				// swhors@naver.com
				// 파싱 실패
			}
		}
		delete sipHd;
	}
	return 0;
}

// init_hook
// 후킹 데이터의 포인트를 초기화
void sky_sip_manager::init_hook()
{
    hookFN.rbye = NULL;
    hookFN.rinvite = NULL;
    hookFN.rok = NULL;
    hookFN.rringing = NULL;
    hookFN.rwaiting = NULL;
	hookFN.rinfo = NULL;
	hookFN.sinfo = NULL;
    hookFN.sbye = NULL;
    hookFN.sinvite = NULL;
    hookFN.sok = NULL;
    hookFN.sringing = NULL;
    hookFN.swaiting = NULL;
    hookFN.sfail = NULL;
    hookFN.rfail = NULL;
    hookFN.sregister = NULL;
    hookFN.eManager = NULL;
    hookFN.rcancel = NULL;
    hookFN.scancel = NULL;
}

// set_hook
// 후킹 함수의 포인터를 대입하는 함수.
//
// argument : IN hookfn * hook
// 
void sky_sip_manager::set_hook(IN hookfn *hook)
{
    hookFN.rbye = hook->rbye;
    hookFN.rinvite = hook->rinvite;
    hookFN.rok = hook->rok;
    hookFN.rringing = hook->rringing;
    hookFN.rwaiting = hook->rwaiting;
    hookFN.sbye = hook->sbye;
	hookFN.sinfo = hook->sinfo;
	hookFN.rinfo = hook->rinfo;
    hookFN.sinvite = hook->sinvite;
    hookFN.sok = hook->sok;
    hookFN.sringing = hook->sringing;
    hookFN.swaiting = hook->swaiting;
    hookFN.sfail = hook->sfail;
    hookFN.rfail = hook->rfail;
    hookFN.sregister = hook->sregister;
    hookFN.eManager = hook->eManager;
}

//
// sdp_process
// sdp 정보를 비교하여 자신의 것과 맞는지를 비교하는 함수.
// 
// direction이 0인 경우는 자신이 invite를 내 보낸 경우.
// direction이 1인 경우는 자신이 invite를 받은 경우.
int sky_sip_manager::sdp_process(IN p_sdp_info_t r_sdpInfo,IN int direction)
{
#if 0
    int r_media;
    r_media = r_sdpInfo->media_des.media_count;
    if( direction == 0)
    {
        for(int i = 0 ; i < r_media ; i++ )
        {
            for(int b = 0 ; b < media_count ; b++ )
            {
                if( mediaAtt[i].num == r_sdpInfo->media_des.media_format[b])
                {
                    if( mediaAtt[i].num == 18 )
                        CallInfo.used_codec = 2;
                    else if( mediaAtt[i].num == 4 )
                        CallInfo.used_codec = 1;
                    else
                        CallInfo.used_codec = 0;
                    return b+1;
                }
            }
        }
    }
    else if( direction == 1)
    {
        for(int i = 0 ; i < r_media ; i++ )
        {
            for(int b = 0 ; b < media_count ; b++ )
            {
                if( mediaAtt[b].num == r_sdpInfo->media_des.media_format[i])
                {
                    if( mediaAtt[b].num == 18 )
                        CallInfo.used_codec = 2;
                    else if( mediaAtt[b].num == 4 )
                        CallInfo.used_codec = 1;
                    else
                        CallInfo.used_codec = 0;
                    return b+1;
                }
            }
        }
    }
#endif
    return 0;
}

void sky_sip_manager::OnTimer(UINT nIDEvent)
{
    switch(nIDEvent)
    {
	case 1:
    default:
        break;
    }
}


int sky_sip_manager::OnRecvInfo(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnRecvNewCall(void * pArg)
{
    return 1;
}
int sky_sip_manager::OnRecvEndCall(void * pArg)
{
    return 1;
}

int sky_sip_manager::OnRecvNewCallAck(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnRecvCancelCall(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendReqCall(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendReqCallAck(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendRegister(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnRecvFailMsg(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendFailMsg(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnFailNewCall(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnFailReqCall(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnSendRinging(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendTrying(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnRecvRinging(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnRecvTrying(void *pArg)
{
    return 1;
}

int sky_sip_manager::OnSendEndCall(void *pArg)
{
    return 1;
}

void sky_sip_manager::get_send_ip(void *_pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField->record_list.size() > 0 )
	{
#ifdef WIN32
		if( !pField->record_list.begin()->get()->uri.haddr.hostAddr.empty() )
		{
			sprintf_s(sendip, 24, "%s",	(char*)pField->record_list.begin()->get()->uri.haddr.hostAddr.data());
#elif defined LINUX || defined MACOS || defined WINCE
		if( !pField->record_list.begin()->uri.haddr.hostAddr.empty() )
		{
			sprintf_s(sendip, 24, "%s",	(char*)pField->record_list.begin()->uri.haddr.hostAddr.data());
#endif
			return;
		}
	}
    //CallInfo.bRecordRoute = 0;
	if( pField->via_list.size()> 0 )
    {
#ifdef WIN32
		vector<shared_ptr<sip_via_t>>::iterator c = pField->via_list.begin();
#elif defined LINUX || defined MACOS || defined WINCE
		vector<sip_via_t>::iterator c = pField->via_list.begin();
#endif
		if( c != pField->via_list.end() )
		{
#ifdef WIN32
			if( !c->get()->host.hostAddr.empty() )
#elif defined LINUX || defined MACOS || defined WINCE
			if( !c->host.hostAddr.empty() )
#endif
			{
#ifdef WIN32
				sprintf_s(sendip, 24, "%s", (char*)c->get()->host.hostAddr.data());
#elif defined LINUX || defined MACOS || defined WINCE
				sprintf_s(sendip, 24, "%s", (char*)c->host.hostAddr.data());
#endif
		        if( memcmp(sendip, hostip, strlen( hostip)))
			    {
				    return;
				}
			}
		}
    }
	if( pField->contact_list.size() >= 1 )
	{
#ifdef WIN32
		if( !pField->contact_list.begin()->get()->uri.haddr.hostAddr.empty())
#elif defined LINUX || defined MACOS || defined WINCE
		if( !pField->contact_list.begin()->uri.haddr.hostAddr.empty())
#endif
		{
			sprintf_s(sendip, 24,"%s", 
#ifdef WIN32
				pField->contact_list.begin()->get()->uri.haddr.hostAddr.data());
#elif defined LINUX || defined MACOS || defined WINCE
				pField->contact_list.begin()->uri.haddr.hostAddr.data());
#endif
		}
	}
    if( memcmp(sendip, hostip, strlen( hostip)))
    {
        return;
    }
#ifdef MSVC_6
    sprintf(sendip, "%s", proxyip);
#else
    sprintf_s(sendip, 24,"%s", proxyip);
#endif
}

int sky_sip_manager::send_sip_info(SipInfoType sipType, char *szTargetID, char opt, char *szTargetIP, char *szBody, int bodylength, char *msgType)
{
	SKY_HOOK_FN *fn;
	char sendIP[24];
	//int mport;
	memset(sendIP, 0, 24);
	//mport = 0;
	if(this->bRun && this->bRegistered )
	{
		//EnterCriticalSection(&sky_sip_critical);
		sip_hd_t *msgInfo = new sip_hd_t();
		if( msgInfo )
		{
			if( build_req_msg(msgInfo, msg_info,szTargetID) > 0 )
			{
				EnterCriticalSection(&sky_sip_critical_dlg);
#ifdef WIN32
				vector<shared_ptr<call_info>>::iterator dialog = 
					find_if(m_CallDialogBuffer.begin(), m_CallDialogBuffer.end(),
					tr1::bind(equal_to<string>(), bind(&call_info::target_number, tr1::placeholders::_1),
					msgInfo->sipfield.to.uri.uid));
#elif defined LINUX || defined MACOS || defined WINCE
				vector<call_info>::iterator dialog = 
						find(	m_CallDialogBuffer.begin(),
								m_CallDialogBuffer.end(),
								(char*)msgInfo->sipfield.to.uri.uid.c_str());
#endif
				if( dialog != m_CallDialogBuffer.end())
				{
#ifdef WIN32
					msgInfo->sipfield.cid = dialog->get()->sipCallid;
					msgInfo->sipfield.from.tag = dialog->get()->self_tag;
					msgInfo->sipfield.to.tag = dialog->get()->target_tag;
					sprintf_s(sendIP, 24, "%s", (szTargetIP!=NULL?szTargetIP:dialog->get()->sendIP));
#elif defined LINUX || defined MACOS || defined WINCE
					msgInfo->sipfield.cid = dialog->sipCallid;
					msgInfo->sipfield.from.tag = dialog->self_tag;
					msgInfo->sipfield.to.tag = dialog->target_tag;
					sprintf_s(sendIP, 24, "%s", (szTargetIP!=NULL?szTargetIP:dialog->sendIP));
#endif
				}
				else
					sprintf_s(sendIP, 24, "%s", (szTargetIP!=NULL?szTargetIP:proxyip));
				if( msgType )
				{
					msgInfo->sipfield.content_type = msgType;
				}
				LeaveCriticalSection(&sky_sip_critical_dlg);
				if( szTargetIP )
				{
					msgInfo->preheadline.sipUri.haddr.hostAddr = szTargetIP;
					msgInfo->sipfield.to.uri.haddr.hostAddr = szTargetIP;
					msgInfo->sipfield.from.uri.haddr.hostAddr = this->hostip;
				}
				if( szBody )
				{
					msgInfo->sipContent.eContentType = e_sipcontent_xml;
					msgInfo->sipContent.xml = szBody;
				}

				if( hookFN.sinfo )
				{
					fn = (SKY_HOOK_FN *)hookFN.sinfo;
					if(!fn(msgInfo) )
					{
						delete msgInfo;
						return 0;
					}
				}
				
				//OnSendEndCall(msgInfo);
				sprintf_s(sipUAC->dip, 24,"%s", sendIP);
				if( sipUAC->send_msg(msgInfo,0))
				{
					//auto_ptr<CSipCallStack> p(new CSipCallStack(msgInfo));
					//m_MsgBuffer.push_back(*p);
					CSipCallStack * pStack = new CSipCallStack(msgInfo);
					pStack->pManager = this;
					pStack->hThreadHandle = CreateThread(NULL, 0, sended_msg_Process, pStack, 0, &pStack->dwThreadID);
					//LeaveCriticalSection(&sky_sip_critical);
					// swhors
					return 1;
				}
			}
			delete msgInfo;
		}
		//LeaveCriticalSection(&sky_sip_critical);
	}
    return 0;
}
int sky_sip_manager::send_dtmf(char dtmf)
{
    return send_sip_info(sipInfo_dtmf, NULL, dtmf);
}

void sky_sip_manager::OnRegisterFail(IN PVOID parg)
{

}

// On received ok messge.
// OK Message -BYE
void sky_sip_manager::OnRecvMsgByeOK(IN void *pArg)
{ 
}
 // OK Message - CANCEL
void sky_sip_manager::OnRecvCancelOK(IN void *pArg)
{
}
// OK Message - General
void sky_sip_manager::OnRecvOK(IN void *pArg)
{

}
int	sky_sip_manager::OnRecvReqCallOk(IN void * pArg)
{
	return 1;
}
int	sky_sip_manager::OnRecvRegisterOk(IN void *pArg)
{
	return 1;
}
// On send ok message
// OK Message - BYE
int	sky_sip_manager::OnSendByeOk(IN void *pArg)
{
	return 1;
}
int	sky_sip_manager::OnSendNewCallOk(IN void *pArg)
{
	return 1;
}


// On Time Event
void sky_sip_manager::OnTimeEvent(IN void *pArg)
{
}

// 180 Process
void sky_sip_manager::OnRecv180Process(IN void *pArg)
{
}
void sky_sip_manager::OnRecv180Invite(IN void *pArg)
{
}

int	sky_sip_manager::OnRecvInviteFail(IN void *pArg)
{
	return 1;
}

int	sky_sip_manager::OnSendSipInfo(IN void *pArg)
{
	return 1;
}
int	sky_sip_manager::OnSendAck(IN void *pArg)
{
	return 1;
}

int	sky_sip_manager::OnRecvAck(IN void *pArg)
{
	return 1;
}

int	sky_sip_manager::OnBeforeSendReqCall(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendReqCallAck(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendEndCall(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendTrying(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendRinging(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendSipInfo(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendAck(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendByeOk(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendNewCallOk(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendRegister(void *pArg)
{
	return 1;
}

int sky_sip_manager::OnBeforeSendFailMsg(void *pArg)
{
	return 1;
}

//
/////////////////////////////////////////////////////////////////////
// End of File.

