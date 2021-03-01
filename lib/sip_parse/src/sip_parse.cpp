/*********************************************
 * SIP 파싱의 메인 소스 
 * Update : (1) 2004/12/1 swhors@skycom.ne.kr
 *        : (2) 2005/2/28 swhors@skycom.ne.kr
 *        : (3) 2005/3/23 swhors@skycom.ne.kr
 *          -- new_hd()에서 memset()수정
 *********************************************/
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <memory.h>
#include <stdlib.h>
#endif


#include <iostream>
#include <string>
#include <vector>

using namespace std;
#include "host_addr.h"
#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"
#include "SIpFieldBase.h"

#include "sip_answer_code.h"
#include "sip_method.h"
#include "sip_uri.h"
#include "sip_to_from.h"
#include "sip_via.h"
#include "sip_callid.h"
#include "sip_cseq.h"
#include "sip_contact.h"
#include "sip_authentication.h"
#include "sip_record_route.h"
#include "sip_sdp.h"
//#include "sip_sdp_parse.h"
#include "sip_allow.h"
#include "sip_content.h"

#include "sip_field.h"
#include "sip_pre_line.h"
#include "sip_header.h"
#include "sip_body.h"

#include "sip_parse.h"


typedef int(HOOK_ERROR_TYPE)(char * sbuff, int error_code);

static PVOID parse_error_report;
static PVOID system_error_report;

void LogWrite(char *buf);


void set_error_hook(PVOID p_parse_error, PVOID p_system_error)
{
    parse_error_report = p_parse_error;
    system_error_report = p_system_error;
}

static int error_report_internal(char *sbuff, int error, PVOID pfn)
{
    HOOK_ERROR_TYPE *func;
    if( pfn )
    {
        func = (HOOK_ERROR_TYPE *)pfn;
        return func(sbuff, error);
    }
    return 0;
}

int parseerror(char * sbuff, int error)
{
    return error_report_internal(sbuff, error, parse_error_report);
}

int systemerror(char *sbuff, int error)
{
    return error_report_internal(sbuff, error, system_error_report);
}

///////////define 
//
//
//////// function
//

/* SIP 정보의 메인 헤더 구조체의 버퍼 삭제*/
int free_hd(void * p)
{
    p_sip_hd_t thd =(p_sip_hd_t)p;
    
    if( !p )
        return 0;
    
    // prehd를 해제함.
	thd->preheadline.free();
    //free_sip_preheader(&thd->preheadline);
    free_sip_field(&thd->sipfield);
    //free(thd);
    return 1;
}

/*SIP 정보의 메인 헤더 구조체 버퍼 생성*/
static p_sip_hd_t new_hd_internal()
{
    sip_hd_t * thd;
    thd = (sip_hd_t*)malloc(sizeof(sip_hd_t));
    if(!thd)
        return NULL;
    return thd;
}

p_sip_hd_t new_hd()
{
    return new_hd_internal();
}

//이 함수는 메모리 형태의 sip 메세지를 받아서 
//분석하고 새로운 정보의 버퍼를 생성하는 메인 함수이다.
//입력 : 
//      1. type(unsinged char *), 내용(메모리)
//출력 : 
//
//리턴 : 성공시 새로이 생성된 버퍼의 주소, 실패시 0

static p_sip_hd_t internal_parse_sip_msg(unsigned char * dump_mem,int len, PVOID p_hd)
{
    sip_hd_t * new_hd_buf;

    PUCHAR cur;
    int msg_type;
    cur = dump_mem;
    if(!dump_mem)
        return NULL;
    
    new_hd_buf = (p_sip_hd_t)p_hd;
    
    if( !new_hd_buf )
    {
        parseerror("internal_parse_sip_msg", 0);
        return NULL;
    }
    else
    {
        // 2005/04/26 swhors@skycom.ne.kr
        // pre_head_line_t의 초기화
        //init_sip_pre_hd_field(&new_hd_buf->preheadline);
		new_hd_buf->preheadline.free();
        init_sip_hd_field(&new_hd_buf->sipfield);
        //sip_field의 초기화
    }
    
    //
    // 2005년 2월 28일 오전 11시 26분 by swhors@skycom.ne.kr
    // Max-Forward를 기본 값으로 -1로 한다.
    // 앞 단의 메세지에서 Max-Forward 값이 오지 않은 경우, 
    // -1로 설정 한다. 이것은 프로세싱 과정에서 70으로 설정
    // 된다.
    //
	new_hd_buf->preheadline.parse(cur);
	msg_type = new_hd_buf->preheadline.msg_type;
    if( msg_type == msg_invalid )
    {
        new_hd_buf->type = sip_response;
    }
    else
    {
        new_hd_buf->type = sip_request;
    }
	cur = new_hd_buf->sipfield.pasre(cur);

	if( !new_hd_buf->sipfield.content_type.compare("application/sdp") )
	{
		new_hd_buf->sipContent.eContentType = e_sipcontent_sdp;
		new_hd_buf->sipContent.sdp.parse(cur);
	}
	else
	{
		if( new_hd_buf->sipfield.content_length > 0 )
		{
			new_hd_buf->sipContent.xml = (char*)cur;
			new_hd_buf->sipContent.eContentType = e_sipcontent_xml;
		}
	}
	if( cur == NULL || cur == dump_mem )
	{
        parseerror("internal_parse_sip_msg", 2);
        free_hd(new_hd_buf);
        return NULL;
    }
    return new_hd_buf;
}

p_sip_hd_t parse_sip_msg(unsigned char * dump_mem,int len, PVOID p_hd)
{
    //LogWrite((char*)"parse_sip_msg");
    return internal_parse_sip_msg(dump_mem,len, p_hd);
}
