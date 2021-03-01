#include <stdio.h>
#ifdef LINUX
#include "error.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#elif defined WIN32 || defined WINCE
#include <windows.h>
#include <winsock.h>
#ifdef WINCE
#undef WIN32
#endif
#ifndef WINCE
#include <io.h>
#endif
#include <string.h>
#include <memory.h>
#endif

#include "skysipua_g.h"

#include "sky_thread.h"
#include "SkySock.h"
#include "SkyAsyncSock.h"
#include "sky_sleep.h"

void	WriteManagerLog(int logLevel, char * szTxt1, int iVal = -1, char *szTxt2=NULL);

SKYTHREAD rcv_data(SKYINVAL pArg);

CSkyAsyncSock::CSkyAsyncSock()
{
	close_count = 0;
    asyncflag = 0;
    init_variable();
    bzeroVal();
}

CSkyAsyncSock::CSkyAsyncSock(char *dIP, int dPort, int cType)
{
    bzeroVal();
    set_dport(dPort);
    set_sport(0);
    set_dip(dIP);
}

CSkyAsyncSock::CSkyAsyncSock(int cType)
{
	bzeroVal();
	ctype = cType;
}

CSkyAsyncSock::~CSkyAsyncSock()
{
	if( sockHandle )
	{
		Close();
	}
	//delete this;
}

void CSkyAsyncSock::init_variable()
{
    asynchandle = THREAD_RETURN;
    stype = 0;
}

int CSkyAsyncSock::Create()
{
    if(ctype == SKY_SOCK_UDP)
	{
        create_udp();
	}
    else if(ctype == SKY_SOCK_TCP)
	{
        create_tcp();
	}
    if( sockHandle )
	{
        asynchandle = sky_thread_create(rcv_data, this, 0, 0);
	}
    return sockHandle;
}

int CSkyAsyncSock::Create(int iPort/*=5060*/,int iBind/*=0*/)
{
    if(ctype == SKY_SOCK_UDP)
        create_udp(iPort,iBind);
    else if( ctype == SKY_SOCK_TCP)
        create_tcp(iPort);
    if( sockHandle )
        asynchandle = sky_thread_create(rcv_data, this, 0, 0);
    return sockHandle;
}

int CSkyAsyncSock::Create(int port, char *dIP, int dPort)
{
    ctype = SKY_SOCK_TCP;
    dport = port;
    sockHandle = create_tcp(port);
    if( sockHandle )
    {
        if( Connect( dIP, dPort) )
        {
            if((asynchandle = sky_thread_create(rcv_data, this, 0, 0)))
                return sockHandle;
        }
    }
    close_handle();
    return sockHandle;
}

void CSkyAsyncSock::Close()
{
	if( sockHandle )
	{
		asyncflag = 1;
		//close_handle();
		//shutdown(sockHandle, 2);
#if defined WIN32 || defined WINCE
#ifdef UNICODE
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, LPCWSTR("Socket Close"));
#else
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, "Socket Close");
#endif
#else
		int hEvent = 0;
#endif
		if( hEvent )
		{
#if defined WIN32 || defined WINCE
			WaitForSingleObject(hEvent, 1000);
			CloseHandle(hEvent);
#endif
		}
		else
			Sleep(1000);
		
#ifdef LINUX
		_close(sockHandle);
#else
		closesocket(sockHandle);
#endif
#if defined WIN32 || defined WINCE
		CloseHandle(asynchandle);
#endif
		sockHandle = NULL;
		asynchandle = NULL;
	}
}

int CSkyAsyncSock::Accept(CSkyAsyncSock * pSock)
{
    pSock->sockHandle =accept_tcp();
    if( pSock->sockHandle )
    {
        getSockName(sip,24,&sport);
        getPeerName(dip,24,&dport);
    }
    return pSock->sockHandle;
}

int CSkyAsyncSock::Accept(int *Handle)
{
    *Handle = accept_tcp();
    return *Handle;
}

int CSkyAsyncSock::Connect(char *ip, int iport)
{
    int ret;
    ret = connect_tcp(ip, iport);
    if( ret > 0 )
        asyncflag  = 1;
    return ret;
}

void CSkyAsyncSock::AcceptEvent(int iVal)
{
	;
}

void CSkyAsyncSock::ReadEvent(int iVal)
{
	;
}

static SKYTHREAD rcv_data(SKYINVAL pArg)
{
    fd_set rfd;//,wfd;
	fd_set	efd;
    int retval;
    static int sockHandle;
    struct timeval tv;
    CSkyAsyncSock * pParent = (CSkyAsyncSock *)pArg;
    sockHandle = pParent->get_sockHandle();
	while(!pParent->asyncflag)
    {
	    tv.tv_sec = 2;
		tv.tv_usec = 0;
		FD_ZERO(&rfd);
		FD_ZERO(&efd);
		FD_SET(sockHandle, &rfd);
		FD_SET(sockHandle, &efd);
		{
			retval = select(sockHandle, &rfd, NULL, &efd, &tv);
			if( retval > 0 )
			{
				if( !pParent->asyncflag)
				{
					if( FD_ISSET(sockHandle, &rfd))
					{
						if( pParent->stype || pParent->ctype == SKY_SOCK_UDP )
						{
							// UDP와 클라이언트 ACCEPT 인경우, 데이터를 읽어 들인다.
							pParent->ReadEvent(0);
							FD_CLR(sockHandle, &rfd);
							//ecvfrom(sockHandle, 
						}
						else
						{
							// TCP의 서버 소켓인 경우, 상대방의 요청을 허용한다.
							pParent->AcceptEvent(0);
						}
					}
					else if(FD_ISSET(sockHandle, &efd))
					{
						break;
					}
				}
			}
			else if( retval == SOCKET_ERROR )
			{
				break;
			}
		}
    }
	WriteManagerLog(1, "rcv_data thread closed.");
	//pParent->asyncflag = 0;
	//closesocket(sockHandle);
    //sky_thread_close(pParent->asynchandle);
    return THREAD_RETURN;
}

//
////////////////////////////////////////////////////////////////////////////
// End of File..
////////////////////////////////////////////////////////////////////////////
//
