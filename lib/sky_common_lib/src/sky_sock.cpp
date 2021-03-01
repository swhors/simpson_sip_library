// sky_sock.c
// 소켓 인터페이스 함수를 정의한 파일.
// 2005/xx/xx

#include <stdio.h>

#ifdef LINUX
#include "error.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#elif defined WIN32 || WINCE
#include <windows.h>
//#include <winsock2.h>
#include <winsock.h>
#ifndef WINCE
#include <io.h>
#endif
#include <string.h>
#include <memory.h>
#endif

#include "sky_def.h"
#include "sky_sock.h"

#define TEST_HOST "www.naver.com"

#ifdef LINUX
void perror(const char *);
#endif

static int create_sock_internal(int port, int type,int bBind)
{
    int sid;
    struct sockaddr_in server;
    sid = socket(AF_INET, type, 0);
    if( sid <= 0)
        return 0;
    memset(&server, 0, sizeof(struct sockaddr));
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	//if( bBind )
	//{
//#ifdef WIN32
		/* Enable address reuse */
		char on = 1;
		if( setsockopt( sid, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) )
		{
#ifdef LINUX
			close(sid);
#elif defined WIN32
			closesocket(sid);
#endif
			return 0;
		}
//#endif
	//}
    if( bind(sid, (struct sockaddr*)&server, sizeof(server))== -1)
    {
#ifndef WINCE
        perror("bind[q931]:");
#endif

#ifdef LINUX
		close(sid);
#elif defined WIN32
		closesocket(sid);
#endif
        return 0;
    }
    return sid;
}
static int create_sock_withip_internal(char *ip, int port, int type)
{
    int sid;
    struct sockaddr_in server;
    sid = socket(AF_INET, type, 0);
    if( sid <= 0)
        return 0;
    memset(&server, 0, sizeof(struct sockaddr));
    server.sin_port = htons(port);
	if( strlen( ip ) )
	{
		server.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
        server.sin_addr.s_addr = inet_addr(ip);
	}
    server.sin_family = AF_INET;
    if( bind(sid, (struct sockaddr*)&server, sizeof(server))== -1)
    {
#ifndef WINCE
        perror("bind:");
#endif
#ifdef LINUX
		close(sid);
#elif defined WIN32
		closesocket(sid);
#endif
        return 0;
    }
    return sid;
    
}

int get_local_ip(char * ip)
{
	int sockid,len,port;
	sockid = create_tcp_sock(0);
	if( sockid )
	{
        len = sizeof(ip);
		memset(ip, 0, len);
		connect_sock(sockid, "210.121.190.8", 80);
        getsockname_sock(sockid, ip, len, &port);
#ifdef LINUX
		close(sockid);
#elif defined WIN32
		closesocket(sockid);
#endif
		return 1;
	}
	return 0;
	//TEST_HOST
}

int create_sock(int port, int type)
{
    return create_sock_internal(port, type, 0);
}

int create_sock_withip(char *ip, int port, int type)
{
    return create_sock_withip_internal(ip, port, type);
}

int create_tcp_sock(int port)
{
    return create_sock_internal(port, SOCK_STREAM, 0);
}

int create_tcp_withip(char *ip, int port)
{
    return create_sock_withip_internal(ip, port, SOCK_STREAM);
}

int create_udp_withip(char *ip, int port)
{
    return create_sock_withip_internal(ip, port, SOCK_DGRAM);
}

int create_udp_sock_bindopt(int port,int opt)
{
    return create_sock_internal(port, SOCK_DGRAM, opt);
}

int create_udp_sock(int port)
{
    return create_sock_internal(port, SOCK_DGRAM, 0);
}

int create_listen_sock(int port)
{
    int sid;
    sid = create_sock(port, SOCK_STREAM);
    if( sid )
    {
       if(listen(sid, 0)<0)
       {
#ifdef LINUX
			close(sid);
#elif defined WIN32
			closesocket(sid);
#endif
			sid = 0;
       }
    }
    return sid;
}

void close_sock(int sid)
{
	closesocket(sid);
}

int accept_sock(int sid)
{
    struct sockaddr_in client;
    int len = sizeof(struct sockaddr_in);
    memset(&client, 0, len);
    return accept(sid,(struct sockaddr*)&client,&len); 
}

int connect_sock(int sid, char *ip, int port)
{
    struct sockaddr_in client;
	int addr;
	int ret = 0;
	char tbuf[24];
    int slen = sizeof(struct sockaddr_in);
    memset(&client, 0, slen);
    client.sin_port = htons(port);
    addr = inet_addr(ip);
	client.sin_family = AF_INET;
	if( addr <= 0)
	{
		struct hostent *myent;
		long int *add;
		myent = gethostbyname(ip);
		if( myent )
		{
			while(*myent->h_addr_list != NULL)
			{
				add = (long int *)*myent->h_addr_list;
				client.sin_addr.s_addr = *add;
				myent->h_addr_list++;
				ret = connect(sid, (struct sockaddr*)&client, slen);
				//break;
			}
		}
	}
	else
	{
        client.sin_addr.s_addr = inet_addr(ip);
        sprintf(tbuf, "%s", inet_ntoa((in_addr)client.sin_addr));
        ret = connect(sid, (struct sockaddr*)&client, slen);
	}
	return ret;
}

int recv_sock(IN int sid,OUT char *rbuff,IN int len)
{
    if( sid < 1)
        return 0;
    memset( rbuff, 0, len);
    return recv(sid, rbuff, len, 0);
}
int send_sock(IN int sid,IN char *sbuff,IN int len)
{
    if( sid < 1)
        return 0;
    return send(sid, sbuff, len, 0);
}

int recvfrom_sock_ttl(IN int sid,OUT char *rbuff,IN int len,OUT char *sip,OUT int *sport,IN int tval)
{
	fd_set rset;
	timeval tv;
	int ret;
    if( sid < 1)
        return 0;
	FD_ZERO(&rset);
	FD_SET(sid, &rset);
	tv.tv_usec = 0;
	tv.tv_sec = tval;

	ret = select(sid, &rset, NULL, NULL, &tv);
	if( ret <= 0 )
		return 0;
	return recvfrom_sock(sid, rbuff, len, sip, sport);
}

int recvfrom_sock(IN int sid,OUT char *rbuff,IN int len,OUT char *sip,OUT int *sport)
{
    sockaddr_in client;
	int	ret = 0;
    int alen,dlen;
    if( sid < 1)
        return 0;
    alen = sizeof(client);
    dlen = recvfrom(sid, rbuff, len, 0,
#ifdef LINUX
                       (sockaddr*)&client, &alen);
#elif defined WIN32
                       (SOCKADDR*)&client, &alen);
#endif
	if( dlen == SOCKET_ERROR) 
	{
		DWORD dwErr = GetLastError();

		return 0;
	}
	else
	{
		if( sip )
			sprintf(sip, "%s", inet_ntoa(client.sin_addr));
		if( sport )
			*sport = ntohs(client.sin_port);
	}
	return dlen;
}

int sendto_sock(IN int sid,IN char *sip,IN int sport, IN char *sbuff,IN int len)
{
    sockaddr_in client;
    int alen;
	int retcode = 0;
    if( sid < 1)
        return 0;
    ///alen = sizeof(sockaddr_in);
    //memset(&client, 0, alen);
    client.sin_port = htons(sport);
	if( !sip )
		return 0;
	if( strlen(sip )<=0 || sport <= 0)
		return 0;
    client.sin_addr.s_addr = inet_addr(sip);
    client.sin_family = AF_INET;
    retcode = sendto(sid, sbuff, len, 0,
#ifdef LINUX
                     (struct sockaddr*)&client, alen);
#elif defined WIN32
                     (SOCKADDR*)&client, sizeof(client));
#endif
	if( retcode == SOCKET_ERROR )
		return 0;
	else
		return retcode;
}

int getsockname_sock(IN int sid,OUT char *ip,IN int iplen, OUT int *port)
{
    struct sockaddr_in name; 
    int namelen=sizeof(struct sockaddr_in);
    if( !getsockname(sid, (struct sockaddr*)&name, &namelen) )
    {
        memset(ip, 0, iplen);
		
        sprintf(ip, "%s", inet_ntoa(name.sin_addr));
        *port = ntohs(name.sin_port);
        return 1;
    }
    return 0;
}

int getpeername_sock(IN int sid,OUT char *ip,IN int iplen, OUT int *port)
{
    int namelen;
    struct sockaddr_in name;
    namelen = sizeof( struct sockaddr);
    if( sid > 0)
    {
        memset(ip, 0, iplen);
        if(getpeername(sid, (struct sockaddr*)&name, &namelen))
            return 0;
        sprintf(ip, "%s", inet_ntoa(name.sin_addr));
        *port = ntohs(name.sin_port);
        return 1;
    }
    return 0;
}

unsigned long convert_sip_iip(IN char *hostip)
{
	return inet_addr(hostip);
}
