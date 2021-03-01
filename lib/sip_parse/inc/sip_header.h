/*
 * 이 파일은 SIP의 메인 헤더를 정의한 것입니다.
 * First : 2004/11/29 swhors@naver.com
 */
#pragma once

#ifndef _SIP_HEADER
#define _SIP_HEADER

#define MAX_CONTACT_NUM		2

typedef enum header_type
{
    sip_invalid,
    sip_request,
    sip_response,
    sip_ack
}header_type_e;
/*
class pre_head_line_t
{
public:
	pre_head_line_t();
	~pre_head_line_t();
    
	int msg_type;
    sip_version_t sip_version;
	header_type_e hdtype;
	protocol_type_e proto_type;
    int state_code;
    uri_addr_t uri;
	PBYTE	parse(IN PBYTE	inStr);
};
*/
//typedef pre_head_line_t* p_pre_head_line_t;


class sip_hd_t
{
public:
	sip_hd_t();
	~sip_hd_t();
    header_type_e type;
    sip_pre_line preheadline;
    sip_field_t sipfield;
	sip_content	sipContent;
	PBYTE	parse(PBYTE	inStr);
	int		print(PBYTE outStr);
};
typedef sip_hd_t* p_sip_hd_t;

#define SIP_HD_SIZE sizeof(sip_hd_t)

#endif //_SIP_HEADER
