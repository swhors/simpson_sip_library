#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#endif

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "sip_tool.h"
#include "sip_header.h"
#include "sip_parse.h"
#include "sip_method.h"

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
    while(end)
    {
        if( *end ==  '\r' || *end == '\n')
            break;
        copybuf[length++] = *end++;
    }
    i = parse_method_type(copybuf);

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
            next = parse_sip_protocol_type((PUCHAR)next, (int*)&phd->preheadline.proto_type);
        else
            next = parse_sip_protocol_type(cur, (int*)&phd->preheadline.proto_type);
#else
        if( next )
            next = parse_sip_protocol_type((PUCHAR)next, (int*)&phd->preheadline.proto_type);
        else
            next = parse_sip_protocol_type(cur, (int*)&phd->preheadline.proto_type);
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