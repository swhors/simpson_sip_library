#include <stdio.h>
#include "skytool.h"
#include "skysipua_g.h"
#include "SkySock.h"
#include "sky_thread.h"
#include "SkyAsyncSock.h"
#include "SkyRtp.h"

CRtp::CRtp()
{
	rtpPort = 0;
	rtcpPort = 0;
	ctype = SKY_SOCK_UDP;
}

CRtp::CRtp(int rtp, int rtcp)
{
	rtpPort = rtp;
	rtcpPort = rtcp;
}

CRtp::~CRtp()
{
	CSkyAsyncSock::~CSkyAsyncSock();
}

// RTP �����͸� �ް� �ȴ�.
void CRtp::ReadEvent(int iVal)
{
	// RTP Read Event.
	char rbuff[2048];
	int len=2048;
	recv_udp(rbuff, &len);
	// �����͸� �޾Ҵ��� �����
	//rintf("%s - %d", rbuff, len);
}

int CRtp::Create(int port)
{
	return CSkyAsyncSock::Create(port);
}