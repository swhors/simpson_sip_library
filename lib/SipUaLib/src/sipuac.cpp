///////////////////////////////////////////////////////////////////////
// 
// ���� :
//       sipuac.cpp
// ���� :
//       �� ������ SIP UA�� Ŭ���̾�Ʈ ������ �ϴ� Ŭ������ �Լ���
//       ������ �����̴�.
// ��¥ : 2005�� 9�� 9�� ( swhors@skycom.ne.kr )
//        ó�� ����.
//        2005�� 11�� 1�� ( swhors@skycom.ne.kr )
//        ������ ȣȯ�� �ǵ��� �ҽ� ���� : sport�� dport�� ���� ��ȣ�� �����.
//        �׷��� ���� ������, register������ ��� �޼����� sport�� ������.
///////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef WIN32
#include <windows.h>
#elif defined WINCE
#include <winbase.h>
#endif

#if defined WIN32 || defined WINCE
#include <winsock.h>
#ifdef WINCE
#undef WIN32
#endif
#endif
#include <iostream>
#include <string>
#include <vector>
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

void	WriteManagerLog(int logLevel, char * szTxt1, int iVal = -1, char *szTxt2=NULL);

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// ������

// ������ ( ��Ʈ��ȣ�� Ÿ���� �ʿ�� ��.)
CSipuac::CSipuac(void *pMan/*=NNULL*/,int _localPort/*=5060*/,int _proxyPort/*=5060*/,int _ctype/*=SKY_SOCK_UDP*/)
{
	pManager = pMan;
	init_variable();
	bzeroVal();
	dport = _proxyPort;
	sport = _localPort;
	ctype = _ctype;
//    asyncflag = 1;
	//sock_block();
}
// �Ҹ���
CSipuac::~CSipuac()
{
     CSkyAsyncSock::~CSkyAsyncSock();
}

// Ŭ���̾�Ʈ�� ���� ��Ŵ.
int CSipuac::Create(IN int iBind/*=0*/)
{
    int ret;
    if( sport == 0)
    {
        sport = 5080;
    }
    
    return ret = CSkyAsyncSock::Create(dport, iBind);
    //if( ret )
    //{
    //    //sock_unblock();
    //}
	//return ret;
}

// �޼����� ����� �Լ�
int CSipuac::make_msg(IN PVOID _pHD,
                      IN PVOID _pSdp,
                      IN int www_auth,
                      OUT unsigned char *outmsg,
                      IN int *buflen,
                      IN int sdp_flag)
{
#if 0
    p_sip_hd_t  pSipHd = (p_sip_hd_t) _pHD;
    *buflen = make_ua_msg(_pHD, outmsg,www_auth, *buflen, 1, NULL, 0, sip, _pSdp);
    if( *buflen == 1)
        *buflen = (int)strlen((char*)outmsg );
    return *buflen;
#else
	return 0;
#endif 

}

// ���濡�� �޼����� ������.
int CSipuac::send_msg(IN PVOID _pHD,IN int www_auth)
{
    unsigned char msgBuf[4096];
    int len = 4096;
	int	retCode = 0;
#if 1
    //if( get_block() )
    //    return 0;
	sip_hd_t *pHd = (sip_hd_t*)_pHD;
	if( !pHd )
		return 0;
	memset(msgBuf, 0, 4096);
	len = pHd->print(msgBuf);
    //len = make_msg(_pHD, NULL, www_auth, msgBuf, &len, 0);
    if( len <= 0)
        return 0;
//#define FUL_LOG_MSG
#ifdef FUL_LOG_MSG
    {
        FILE *fp = fopen("test.txt","a+t");
        if( fp )
        {
            fwrite((char*)&msgBuf[0], len, 1, fp);
            fclose(fp);
        }
    }
#endif
    retCode= send_udp((char*)&msgBuf[0], len);
	if( retCode == SOCKET_ERROR )
		return 0;
	else
		return retCode;
#else
	return 0;
#endif
}

// ���濡�� �޼����� ������.
// SDP�� ���� �޼��� ���ۿ� ����.(INVITE or OK-INVITE)
int CSipuac::send_msg(IN PVOID _pHD,IN PVOID _pSdp,IN int www_auth)
{
#if 0
    unsigned char msgBuf[6144];
    int len = 6144;
    if( get_block())
        return 0;
    len = make_msg(_pHD, _pSdp, www_auth, msgBuf, &len, 1);
    if( len <= 0)
        return 0;
    return send_udp((char*)&msgBuf[0], len);
#else
	return 0;
#endif
}

// OK �޼��� ����
int CSipuac::send_ok(IN PVOID _pHD)
{
    char msg[2048];
    int len=2048;
    //if( get_block())
    //    return 0;
    if( !_pHD )
        return 0;
    //if( make_ua_ok_msg(_pHD,(PUCHAR)&msg[0],sip) <= 0)
        return 0;
    len = (int)strlen( msg );
    if( len <= 0)
        return 0;
    return send_udp(msg, len);
}

// SDP�� ���� OK �޼��� ����
int CSipuac::send_ok(IN PVOID _pHD, PVOID _pSdp)
{
#if 0
    char msg[3072];
    int len=3072;
    if( get_block())
        return 0;
    if( !_pHD )
        return 0;
    if( make_ok_msg_ua_with_sdp(_pHD,(PUCHAR)&msg[0],sip,_pSdp) <= 0)
        return 0;
    len = (int)strlen(msg);
    if( len <= 0)
        return 0;
    return send_udp(msg, len);
#else
	return 0;
#endif
}

// Ringing �޼����� ������.
int CSipuac::send_ringing(IN PVOID _pHD)
{
#if 0
    char msg[3072];
    int len=3072;
    if( get_block() )
        return 0;
    if( !_pHD )
        return 0;
    if(make_ringing_msg(_pHD, (PUCHAR)&msg[0], sip) <= 0)
        return 0;
    len = (int)strlen(msg);
    if( len <= 0 )
        return 0;
    return send_udp(msg, len);
#else
	return 0;
#endif
}

// trying �޼��� ����
int CSipuac::send_trying(IN PVOID _pHD)
{
#if 0
    char msg[3072];
    int len = 3072;
    if( get_block() )
        return 0;
    if( !_pHD )
        return 0;
    if(make_trying_msg(_pHD, (PUCHAR)&msg[0], sip) <= 0)
        return 0;
    len = (int)strlen(msg);
    if( len <= 0 )
        return 0;
    return send_udp(msg, len);
#else
	return 0;
#endif
}

// fail msg ����
int CSipuac::send_fail(IN PVOID _pHD,IN int err_code)
{
#if 0
    char msg[2048];
    int len = 2048;
    if( get_block() )
        return 0;
    if( !_pHD )
        return 0;
    if( make_fail_ua_msg( _pHD, (PUCHAR)&msg[0], err_code, 0, sip) <= 0 )
        return 0;
    len = (int)strlen( msg );
    if( len <= 0 )
        return 0;
    return send_udp( msg, len);
#else
	return 0;
#endif
}

// ack msg ����
int CSipuac::send_ack(IN PVOID _pHD)
{
    return send_msg( _pHD,0);
}

// bye msg ����
int CSipuac::send_bye(IN PVOID _pHD)
{
    return send_msg( _pHD,0);
}

void CSipuac::ReadEvent(int iVal)
{
    sky_sip_manager *pMan;
    char rBuff[3076];
    char ip[24];
    int port;
    int len = 3076;
    memset(ip, 0, 24);
    //if( get_block() )
    //    return;
    len = recv_udp(rBuff,&len,ip,&port);
    if( len >= 10)
    {
//#define FILE_WRITE
#ifdef FILE_WRITE
		FILE *fp = fopen("sip_raw.txt", "a+t");
		if( fp )
		{
			fwrite(rBuff, len, 1, fp);
			fclose(fp);
		}
#endif
        if( pManager )
        {
            pMan = (sky_sip_manager*)pManager;
			pMan->PutInData(rBuff,len);
        }
    }
    else if( len == 4)
    {
        char tbuf[4];
        memset(tbuf, 0, 4);
        send_udp(tbuf, 4, ip, port);
    }
}

//
/////////////////////////////////////////////////////////////////////////////////////
//
