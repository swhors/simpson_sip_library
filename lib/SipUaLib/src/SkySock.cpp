#include <stdio.h>
#include <memory.h>
#include "skytool.h"
#include "skysipua_g.h"
#include "SkySock.h"
#include "sky_sock.h"

CSkySock::CSkySock()
{
	bzeroVal();
}

CSkySock::CSkySock(IN char *sIP,IN int sPort,IN char *dIP,IN int dPort,IN int cType)
{
    sockHandle = 0;
	sprintf(sip, "%s", sIP);
	sprintf(dip, "%s", dIP);
	sport = sPort;
	dport = dPort;
	ctype = cType;
	if( ctype == SKY_SOCK_TCP )
	{
		create_tcp();
	}
	else if( ctype == SKY_SOCK_UDP)
	{
		create_udp();
	}
}

CSkySock::CSkySock(IN char *sIP,IN int sPort)
{
	bzeroVal();
	sprintf(sip, "%s", sIP);
	sport = sPort;
}

CSkySock::~CSkySock()
{
	close_handle();
}

void CSkySock::bzeroVal()
{
	memset(dip, 0, 24);
	memset(sip, 0, 24);
	dport = 0;
	sport = 0;
	sockHandle = 0;
	ctype = 0;
}

void CSkySock::set_dip(IN char *sVal)
{
    sprintf(dip, "%s", sVal);
}

void CSkySock::set_sip(IN char *sVal)
{
	sprintf(sip, "%s", sVal);
}

int CSkySock::create_tcp()
{
	if( !sockHandle )
        sockHandle = create_tcp_sock(dport);
	if( sockHandle )
		getSockName(sip, 24, &sport);
	return sockHandle;
}

int CSkySock::create_tcp(int sPort)
{
	sport = sPort;
	return create_tcp();
}

int CSkySock::create_tcp(IN char *sIP,IN int sPort)
{
	set_sip(sIP);
	sport = sPort;
	sockHandle = create_tcp_withip(sIP, sPort);
	if( sockHandle )
		getSockName(sip, 24, &sport);
	return sockHandle;
}

int CSkySock::create_tcp(IN char *sIP,IN int sPort,IN char *dIP,IN int dPort)
{
	set_sip(sIP);
	set_dip(dIP);
	sport = sPort;
	dport = dPort;
	if(! sockHandle )
	    sockHandle = create_tcp_withip(sIP, sPort);
	if( sockHandle )
	{
		if( !connect_sock(sockHandle,sIP,dPort) )
		{
			close_sock(sockHandle);
			sockHandle = 0;
			return 0;
		}
		getSockName(sip, 24, &sport);
	}
	return sockHandle;
}

int CSkySock::create_udp()
{
	if(! sockHandle )
	    sockHandle = create_udp_sock(sport);
	if(sockHandle)
		getSockName(sip, 24, &sport);
	return sockHandle;
}

int CSkySock::create_udp(IN int sPort,IN int iBind/*=0*/)
{
	sport = sPort;
	if( !sockHandle )
	{
        sockHandle = create_udp_sock_bindopt(sport,iBind);
	}
	if(sockHandle)
		getSockName(sip, 24, &sport);
	return sockHandle;
}

//int CSkySock::create_udp(int sPort)
//{
//	sport = sPort;
//	return create_udp();
//}

int CSkySock::create_udp(IN char *sIP,IN int sPort)
{
    set_sip(sIP);
    sport = sPort;
	if( !sockHandle )
        sockHandle = create_udp_withip(sIP, sPort);
	return sockHandle;
}

int CSkySock::send_tcp(IN char *dbuff,IN int len)
{
    return send_sock(sockHandle, dbuff, len);
}

int CSkySock::send_udp(IN char *dbuff,IN int len)
{
    return sendto_sock(sockHandle,dip,dport,dbuff,len);
}

int CSkySock::send_udp(IN char *dbuff,IN int len,IN char *dIP,IN int dPort)
{
    return sendto_sock(sockHandle,dIP,dPort,dbuff,len);
}

int CSkySock::recv_tcp(OUT char *dbuff,OUT int *len)
{
    *len = recv_sock(sockHandle,dbuff,*len);
	return *len;
}

int CSkySock::recv_udp(OUT char *dbuff,OUT int *len,OUT char *sIP,OUT int *sPort,IN int tVal)
{
	*len = recvfrom_sock_ttl(sockHandle,dbuff,*len,sIP,sPort,tVal);
	return *len;
}

int CSkySock::recv_udp(OUT char *dbuff,OUT int *len,IN int tVal)
{
	*len = recvfrom_sock_ttl(sockHandle,dbuff,*len,sip,&sport,tVal);
	return *len;
}

int CSkySock::recv_udp(OUT char *dbuff,OUT int *len)
{
    *len = recvfrom_sock(sockHandle,dbuff,*len,sip,&sport);
	return *len;
}

int CSkySock::recv_udp(OUT char *dbuff,OUT int *len,OUT char *sIP,OUT int *sPort)
{
    *len = recvfrom_sock(sockHandle,dbuff,*len,sIP,sPort); 
    return *len;
}

void CSkySock::close_handle()
{
    //if( sockHandle )
    //    close_sock(sockHandle);
    //sockHandle = 0;
}

int CSkySock::getPeerName(OUT char *IP,IN int iplen,OUT int *port)
{
    return getpeername_sock(sockHandle, IP, iplen, port);
}

int CSkySock::getSockName(OUT char *IP,IN int iplen, OUT int *port)
{
	return getsockname_sock(sockHandle, IP, iplen, port);
}

// 클라이언트의 접속 요청을 허용함.
int CSkySock::accept_tcp()
{
	return accept_sock(sockHandle);
}

// 서버로 접속 요청을 함
int CSkySock::connect_tcp(char *dIP, int dPort)
{
	return connect_sock(sockHandle,dIP,dPort);
}
