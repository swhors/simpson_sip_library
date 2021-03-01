//////////////////////////////////////////////////////////
// File : sip_pre_head.c
// 설명 : sip의 첫 줄에 대한 파싱.
// 날짜 : (1) 2005/4 swhors@skycom.ne.kr
//
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////

#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#endif

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "skytool.h"
// basic include - start
#include "sip_tool.h"
#include "Sip_Info.h"
#include "sip_field.h"
// basic include - end
#include "SipFieldBase.h"
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
#include "sip_content.h"
#include "sip_header.h"
#include "sip_parse.h"
#include "sip_method.h"
#include "sip_pre_head.h"

#include "sip_answer_code.h"

//////////////////////////////////////////////////////////
// Definitions 
//////////////////////////////////////////////////////////

extern sip_answer_code_t sipAnswerCode[ANSWER_CODE_NUM];

//////////////////////////////////////////////////////////
// Functions 
//////////////////////////////////////////////////////////

static void free_sip_preheader_internal(PVOID _pHd)
{
    p_pre_head_line_t phd = (p_pre_head_line_t)_pHd;
    if( _pHd )
    {
        free_uri(&phd->uri);
    }
}

void free_sip_preheader(PVOID _pHd)
{
    free_sip_preheader_internal(_pHd);
}

static void init_sip_pre_hd_field_internal(IN PVOID _pPrefield)
{
    if( _pPrefield )
    {
        p_pre_head_line_t pprefield = (p_pre_head_line_t)_pPrefield;
        init_uri(&pprefield->uri);
        pprefield->msg_type =0;
        pprefield->sip_version.major = 0;
        pprefield->sip_version.minor = 0;
        pprefield->hdtype = sip_request;
        pprefield->proto_type = sip_udp;
        pprefield->state_code = 0;
    }
}

void init_sip_pre_hd_field(IN PVOID _pPrefield)
{
    init_sip_pre_hd_field_internal(_pPrefield);
}

static int parse_sip_preheader_internal(PUCHAR dump, PVOID _pHd)
{
    int i = 0, length=0;
    UCHAR copybuf[512];
    PUCHAR cur,end;
    p_sip_hd_t phd=NULL;
    
    phd = (p_sip_hd_t)_pHd;
    
    if( !dump )
        return 0;
    
    memset( copybuf, 0, 512);
    
    end = dump;
    
    //while(end)
    //{
    //    if( *end ==  '\r' || *end == '\n')
    //        break;
    //    copybuf[length++] = *end++;
    //}
    
	i = parse_msg_type(copybuf, phd->preheadline.msg_type);

    if( i >0 && i < (SIP_METHOD_COUNT-2))
    {
        char * next=NULL;
        
        // 2005/03/23 swhors@skycom.ne.kr
        // -- start
        //memset(&phd->preheadline, 0, sizeof( request_line_t));
        // -- end
        
        cur = &copybuf[get_sip_method_name_len(i)+1];
        
        // get uri
        uri_parse_user_number(cur,&phd->preheadline.uri);
        uri_parse_hostaddr(cur,&phd->preheadline.uri.haddr);
        
        // get sip version
        next = parse_sip_version(cur, &phd->preheadline.sip_version);
        
        // get sip protocol type : udp or tcp
#if MSVC_VER==6 && defined(WIN32)
        if( next )
            next = parse_sip_protocol_type((PUCHAR)next,
                                (int*)&phd->preheadline.proto_type);
        else
            next = parse_sip_protocol_type(cur,
                                (int*)&phd->preheadline.proto_type);
#else
        if( next )
            next = parse_sip_protocol_type((PUCHAR)next,
                                (int*)&phd->preheadline.proto_type);
        else
            next = parse_sip_protocol_type(cur,
                                (int*)&phd->preheadline.proto_type);
#endif
        
        phd->preheadline.hdtype = sip_request;
        phd->preheadline.msg_type = i;
    }
    else if( i == (SIP_METHOD_COUNT-2))
    {
        char * next;
        cur = &copybuf[0];
        
        // get sip version
        next = parse_sip_version(cur, &phd->preheadline.sip_version);
        
        cur = (PUCHAR)strstr( (char*)cur, " " );
        
        if( cur++)
        {
            phd->preheadline.state_code = atoi((char*)cur);
        }
        
        phd->preheadline.hdtype = sip_response;
    }
    else if( i== (SIP_METHOD_COUNT-1))
    {
        cur = &copybuf[4];
        
        // get uri
        uri_parse_user_number(cur,&phd->preheadline.uri);
        
        uri_parse_hostaddr(cur,&phd->preheadline.uri.haddr);
        
        // get sip version
        parse_sip_version(cur, &phd->preheadline.sip_version);
    }
    return i;
}

int parse_sip_preheader(IN PUCHAR dump,OUT PVOID _pHd)
{
    return parse_sip_preheader_internal(dump, _pHd);
}

static int make_state_code_msg_internal(int code, PUCHAR buf)
{
    int i=0, found=0;
    
    for( i = 0; i < ANSWER_CODE_NUM; i++)
    {
        if( code == sipAnswerCode[i].code )
        {
            found = 1;
            break;
        }
    }
    if( found )
    {
        strcatfm((char*)buf," %d %s",
                    sipAnswerCode[i].code, sipAnswerCode[i].str);
        return 1;
    }
    return 0;
}

int make_state_code_msg(int code, PUCHAR buf)
{
    return make_state_code_msg_internal(code,buf);
}

static int make_res_prefix_internal(PVOID p_hd, PUCHAR buf)
{
    p_sip_hd_t phd = (p_sip_hd_t) p_hd;
    p_pre_head_line_t pres = &phd->preheadline;
    strcatfm((char*)buf,"SIP/%d.%d",
              pres->sip_version.major,pres->sip_version.minor);
    if( !make_state_code_msg(pres->state_code,buf))
        return 0;
    strcat((char*)buf,"\r\n");
    return 0;
}

int make_res_prefix(PVOID p_hd, PUCHAR buf)
{
    return make_res_prefix_internal(p_hd,buf);
}

static int make_req_prefix_internal(IN PVOID p_hd,
                                    OUT PUCHAR buf,
                                    IN char *u_num,
                                    IN int u_num_len,
                                    IN char *ip,
                                    IN int port)
{
    p_pre_head_line_t preq = (p_pre_head_line_t)p_hd;
    PUCHAR cur;
    
    sprintf((char*)buf, "%s ", get_sip_method_name(preq->msg_type));
    strcat((char*)buf, "sip:");
    
    if( u_num_len > 0)
    {
        strcat((char*) buf, u_num);
        strcat((char*) buf, "@");
    }
    
	// swhors@naver.com
	// strcat((char*) buf, "gmarket.co.kr");
	strcat((char *)buf, ip);
    
    if( port > 0)
    {
        strcat((char*) buf, ":");
        sprintf((char*)&buf[strlen((char*)buf)], "%d", port);
    }
    
    strcatfm((char*)buf," SIP/%d.%d",preq->sip_version.major,preq->sip_version.minor);
    
    cur = &buf[strlen((char*)buf)];

    if(preq->proto_type == 1)
    {
        strcat((char*)cur, "/TCP");
    }
    else if(preq->proto_type == 2)
    {
        strcat((char*)cur, "/UDP");
    }
    
    strcat((char*)buf,"\r\n");
    return 0;
}

int make_req_prefix_at(PVOID p_hd, PUCHAR buf, char *ip, int port)
{
    p_sip_hd_t phd = (p_sip_hd_t) p_hd;
    return make_req_prefix_internal(&phd->preheadline, buf,
		phd->preheadline.uri.user_number.str,
		phd->preheadline.uri.user_number.len,
//        phd->sipfield.to.uri.user_number.str,
//        phd->sipfield.to.uri.user_number.len,
        ip, port);
}

int make_req_prefix(PVOID p_hd, PUCHAR buf)
{
    p_sip_hd_t phd = (p_sip_hd_t) p_hd;
    return make_req_prefix_internal(&phd->preheadline, buf,
        phd->sipfield.to.uri.user_number.str,
        phd->sipfield.to.uri.user_number.len,
        phd->preheadline.uri.haddr.eadr.ip.sadr,
        phd->preheadline.uri.haddr.eadr.ip.port);
}

//
//////////////////////////////////////////////////////////
//

