// file : StunClient.cpp
// 내용 : 이 파일은 stun의 클라이언트 모듈을 구현한 파일입니다.
//        이 파일은 RFC 3489와 deltathree의 NAT Traversal in SIP를
//        바탕으로 구현되었습니다.
//        swhors@skycom.ne.kr  2005/11/18
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <winsock.h>

#include "skysipua_g.h"
#include "skysock.h"
#include "stunclient.h"
//
// class implementation..
//

CStunClient::CStunClient(void)
{
	memset( mapped_ip , 0, 24);
	memset( src_ip, 0, 24);
	memset( changed_ip, 0, 24);
	src_port = 0;
	changed_port = 0;
	mapper_port = 0;
	memset( stun_ip1, 0, 24);
	stun_port1 = 0;
	memset( stun_ip2, 0, 24);
	stun_port2 = 0;
}

CStunClient::~CStunClient(void)
{
	//con1.close_handle();
}

int CStunClient::Create(char *localip/*=NULL*/,int testport/*=5060*/,char *stunip/*=NULL*/,int port/*=5060*/)
{
	sprintf(local_IP, "%s", localip);
	sprintf(stun_ip1, "%s", stunip);
	stun_port1 = port;
	if( !con1.create_udp(testport))
		return 0;
	return 1;
}

int CStunClient::get_add_info(p_stun_att_address padd, char *sip, int *port)
{
	unsigned char *tip;
	unsigned int uip;
	*port = ntohs(padd->port);
	uip = ntohl(padd->ip);
    tip = (unsigned char *)&uip;
	sprintf(sip, "%d.%d.%d.%d", tip[3], tip[2], tip[1], tip[0]);
	return 1;
}
int CStunClient::build_req_body(int type,p_stun_request pReq,int id)
{
	pReq->attribute_length = htons(4);
	pReq->attribute_type = htons(STUN_CHANGE_REQUEST);
	pReq->dummy = 0;
	switch(type)
	{
	case 1:
		pReq->change_ip = 0;
		pReq->change_port = 0;
		break;
	case 2:
		pReq->change_ip = 1;
		pReq->change_port = 1;
		break;
	case 3:
		pReq->change_ip = 0;
		pReq->change_port = 0;
		break;
	case 4:
		pReq->change_ip = 0;
		pReq->change_port = 1;
		break;
	default:
		return 0;
	}
	pReq->stunbasic.msg_length = htons(sizeof( stun_request ) - sizeof(stun_basic));
	pReq->stunbasic.msg_type = htons(MSG_BIND_REQUEST);
	memset( pReq->stunbasic.stunid, 0, 16);
	pReq->stunbasic.stunid[0] = id;
	return 1;
}
int CStunClient::test_1()
{
	stun_request stunrequest;
	stun_response stunResponse;
	if(!build_req_body(1,&stunrequest,1))
		return 0;
    con1.send_udp((char*)&stunrequest, sizeof(stun_request), stun_ip1, stun_port1);
	return get_answer(1, &stunResponse, stun_ip2, &stun_port2);
}

int CStunClient::test_2()
{
	stun_request stunrequest;
	stun_response stunResponse;
	if( !build_req_body(2,&stunrequest,2))
		return 0;
    con1.send_udp((char*)&stunrequest, sizeof(stun_request), stun_ip1, stun_port1);
	return get_answer(2, &stunResponse, stun_ip2, &stun_port2);
}

int CStunClient::test_3()
{
	char ip[24];
	int port;
	stun_request stunrequest;
	stun_response stunResponse;
	if( !build_req_body(3,&stunrequest,3))
		return 0;
    con1.send_udp((char*)&stunrequest, sizeof(stun_request),stun_ip2, stun_port1);
	return get_answer(3,&stunResponse,ip, &port);
}

int CStunClient::test_4()
{
	stun_request stunrequest;
	stun_response stunResponse;
	if( !build_req_body(4,&stunrequest,4))
		return 0;
    con1.send_udp((char*)&stunrequest, sizeof(stun_request),stun_ip1, stun_port1);
	return this->get_answer(4,&stunResponse, stun_ip2, &stun_port2);
}

int CStunClient::get_answer(int cid,p_stun_response pStunResponse, char *ip, int *port)
{
	char rbuff[2048];
	int len,count,len1;
	len1 = sizeof(stun_response);
	for(count = 0; count < 3; count ++)
	{
		len = 2048;
		memset(rbuff, 0, 2048);
	    con1.recv_udp(rbuff, &len, ip, port);
	    if( len == len1 )
	    {
			memcpy(pStunResponse, rbuff, len1);
			if( pStunResponse->stunbasic.stunid[0] == cid )
			{
		        get_add_info(&pStunResponse->changed_add, (char*)&changed_ip[0], &changed_port);
                get_add_info(&pStunResponse->mapped_add, (char*)&mapped_ip[0], &mapper_port);
		        get_add_info(&pStunResponse->source_add, (char*)&src_ip[0],&src_port);
			    return 1;
			}
		}
	}
	return 0;
}

int CStunClient::test_manager()
{
	if(test_1())
	{
		if(! strcmp(local_IP, mapped_ip) )
		{
			if( test_2() )
			{
				stun_state = STUN_OPENINTERNET;
				return 1;
			}
			else
			{
				stun_state = STUN_SYM_UDP_FW;
				return 0;
			}
		}
		else
		{
			if( test_2() )
			{
				stun_state = STUN_FULL_CON;
				return 1;
			}
			else
			{
				test_3();
                if(! strcmp(local_IP, mapped_ip) )
				{
					if( test_4() )
					{
						stun_state = STUN_RESTRICTED;
						return 1;
					}
					else
					{
						stun_state = STUN_PORT_RES;
					}
				}
				else
				{
					stun_state = STUN_SYM_NAT;
				}
			}
		}
	}
	else
	{
		stun_state = STUN_UDP_BLOCKED;
	}
	return 0;
}

// end of file ..
//
