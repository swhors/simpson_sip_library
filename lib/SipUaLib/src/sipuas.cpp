///////////////////////////////////////////////////////////////////////
// 
// 파일 :
//       sipuas.cpp
// 설명 :
//       이 파일은 SIP UA의 서버 역할을 하는 클래스의 함수를
//       정의한 파일이다.
// 날짜 : 2005년 9월 9일 ( swhors@skycom.ne.kr )
// 
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "skysipua_g.h"
#include "skysip.h"

#include "SkySock.h"
#include "SkyAsyncSock.h"
#include "stunclient.h"
#include "sipuac.h"
#include "sipuas.h"
#include "SkySipUaManager.h"
///////////////////////////////////////////////////////////////////////
// 생성자 1
CSipuas::CSipuas(void *pMan)
{
	pManager = pMan;
    init_variable();
    bzeroVal();
    ctype = SKY_SOCK_UDP;
	dport = 5060;
    asyncflag = 0;
	//CSkyAsyncSock::CSkyAsyncSock();
}

// 생성자 2
// 포트 번호와 타입을 입력 변수로 가짐.
CSipuas::CSipuas(void *pMan,int _port, int _ctype)
{
	pManager = pMan;
    asyncflag = 0;
    init_variable();
    bzeroVal();
	if( _port == 0 )
		sport = 5060;
	else
	    sport = _port;
	ctype = _ctype;
	//CSkyAsyncSock::CSkyAsyncSock();
}

// 소멸자
CSipuas::~CSipuas()
{
	CSkyAsyncSock::~CSkyAsyncSock();
}

// 상대방으로 부터 온 메세지를 읽어 들이는 함수.
void CSipuas::ReadEvent(int iVal)
{
	sky_sip_manager *pMan;
	char rBuff[3076];
	int len = 3076;
    len = recv_udp(rBuff, &len);
	if( len >= 0)
	{
		//if( pManager )
		//{
		//	pMan = (sky_sip_manager*)pManager;
		//}
		//pMan->ParseData(rBuff,len);
	}
}

//
///////////////////////////////////////////////////////////////////////
//
