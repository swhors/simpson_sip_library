///////////////////////////////////////////////////////////////////////
// 
// ���� :
//       sipuas.cpp
// ���� :
//       �� ������ SIP UA�� ���� ������ �ϴ� Ŭ������ �Լ���
//       ������ �����̴�.
// ��¥ : 2005�� 9�� 9�� ( swhors@skycom.ne.kr )
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
// ������ 1
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

// ������ 2
// ��Ʈ ��ȣ�� Ÿ���� �Է� ������ ����.
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

// �Ҹ���
CSipuas::~CSipuas()
{
	CSkyAsyncSock::~CSkyAsyncSock();
}

// �������� ���� �� �޼����� �о� ���̴� �Լ�.
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
