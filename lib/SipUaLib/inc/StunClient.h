#pragma once

//class CSkySock;

//#include "skysipua_g.h"
//#include "skysock.h"

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

// message type
#define MSG_BIND_REQUEST		0x0001
#define MSG_BIND_RESPONSE		0x0101

// attribute type;
#define STUN_CHANGE_REQUEST		0x0003
#define STUN_MAPPED_ADDRESS		0x0001
#define STUN_SOURCE_ADDRESS		0x0004
#define STUN_CHANGED_ADDRESS	0x0005

typedef struct _stun_basic
{
	short msg_type;
	short msg_length;
    uchar stunid[16];
}stun_basic,*p_stun_basic;

typedef struct _stun_att_address
{
	ushort	att_type;
	ushort	att_length;
	ushort	p_family;
	ushort	port;
	uint	ip;
}stun_att_address, *p_stun_att_address;

typedef struct _stun_request
{
	stun_basic stunbasic;
	ushort attribute_type;
	ushort attribute_length;
	ushort dummy;
	uchar change_ip;
	uchar change_port;
}stun_request, *p_stun_request;

typedef struct _stun_response
{
	stun_basic stunbasic;
    stun_att_address mapped_add;
	stun_att_address source_add;
	stun_att_address changed_add;
}stun_response,*p_stun_response;

#define STUN_UDP_BLOCKED	0x0100
#define STUN_SYM_UDP_FW		0x0200
#define STUN_SYM_NAT		0x0300
#define STUN_RESTRICTED		0x0400
#define STUN_PORT_RES		0x0500
#define STUN_FULL_CON		0x0600
#define STUN_OPENINTERNET	0x0700

class SKYLIB CStunClient
{
public:
	CStunClient(void);
	~CStunClient(void);
	int Create(char *localip=NULL,int testport=5060,char * stunip=NULL,int port=5060);
	int test_manager();
private:
	int get_answer(int cid,p_stun_response pStunResponse, char *ip, int *port);
    int get_add_info(p_stun_att_address padd,char * sip,int * port);
	int test_1();
	int test_2();
	int test_3();
	int test_4();
	int build_req_body(int type,p_stun_request pReq,int id);
public:
	char	mapped_ip[24];
	int		mapper_port;
	char	src_ip[24];
	int		src_port;
    char	changed_ip[24];
	int		changed_port;
	CSkySock con1;
	char	stun_ip1[24];
    char    stun_ip2[24];
	int		stun_port1;
    int     stun_port2;
	char    local_IP[24];
	int     local_port;
    int		stun_state;
};
