/*********************************************
 * SIP 파싱의 메인 소스 
 * Update : (1) 2004/12/1 swhors@skycom.ne.kr
 *        : (2) 2005/2/28 swhors@skycom.ne.kr
 *        : (3) 2005/3/23 swhors@skycom.ne.kr
 *          -- new_hd()에서 memset()수정
 *********************************************/

///////////////////////////////////////////////
// Include
///////////////////////////////////////////////

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
#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "SIpFieldBase.h"
#include "sip_info.h"

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

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;

///////////////////////////////////////////////
// Functions 
///////////////////////////////////////////////

void LogWrite(char *buf);

static void init_sip_hd_field_internal(PVOID _pfield)
{
    p_sip_field_t pfield = (p_sip_field_t)_pfield;
    if( pfield )
    {
        //sip_field의 초기화

		//init_tofrom_buf(&pfield->to);
        //init_tofrom_buf(&pfield->from);

		//init_via_buf(&pfield->via);
		//pfield->via_list
        init_call_id_buf(&pfield->cid);
#if 0
        init_cseq_buf(&pfield->cseq);
        pfield->user_agent.len = 0;
        pfield->user_agent.str = NULL;
		for( i = 0; i < MAX_CONTACT_NUM; i++)
		{
			init_contact_buf(&pfield->contact[i]);
		}
		pfield->contact_count = 0;
        init_authentication_buf(&pfield->www_auth);
        pfield->date.ichecked = 0;
        pfield->expire = 0;
        pfield->max_forward = 70;
        pfield->content_length = 0;
        pfield->allow = 0;
        init_record_route_buf(&pfield->route);
#endif
    }
}

void init_sip_hd_field(IN PVOID _pfield)
{
    init_sip_hd_field_internal(_pfield);
}

static void free_sip_field_internal(PVOID _pfield)
{
    p_sip_field_t pfield = (p_sip_field_t)_pfield;
    
    if( _pfield )
    {
        // field를 해제함.
        free_tofrom(&pfield->to);
        free_tofrom(&pfield->from);
		if( !pfield->via_list.empty())
			pfield->via_list.erase(pfield->via_list.begin(), pfield->via_list.end());
		pfield->cid.free();
        pfield->user_agent= "";
		if( !pfield->contact_list.empty() )
			pfield->contact_list.erase(pfield->contact_list.begin(),pfield->contact_list.end());
		//for(i =0;i< pfield->contact_count; i++)
            //free_contact(&pfield->contact[i]);
        pfield->subject="";
        pfield->content_type="";
        free_authentication(&pfield->www_auth);
		if( pfield->record_list.size() > 0 )
		{
			pfield->record_list.erase(pfield->record_list.begin(), pfield->record_list.end());
		}
        pfield->accept="";
        pfield->accept_language="";
        pfield->supported="";
        pfield->session_expires="";
    }
}

void free_sip_field(IN PVOID _pfield)
{
    free_sip_field_internal(_pfield);
}

int parse_request(PUCHAR dump, PVOID p_hd)
{
#if 0
	int ret;
    p_sip_hd_t siphd = (p_sip_hd_t)p_hd;
    
    // 2005/03/11 18:20 swhors@skycom.ne.kr 
    //ret = parse_header_field(
                (int)siphd->preheadline.msg_type,
                dump, &siphd->sipfield);
	{
		char tbuf[128];
#ifdef MSVC_6
		sprintf(tbuf, "content-length : %d", siphd->sipfield.content_length );
#else
		sprintf_s(tbuf, 128, "content-length : %d", siphd->sipfield.content_length );
#endif
		//LogWrite(tbuf);
	}
	return ret;
#endif
	return 0;
}

int sip_body_parse(IN PUCHAR msg,
                   OUT PVOID _phCall,
                   IN PUCHAR dump_mem,
                   IN int mem_len)
{
    sip_hd_t * new_hd_buf;
    PUCHAR cur;

	
	C c;
    PBYTE tmp0,start;
    typedef FindRepeat<NEWLINE> findNewLine;
    tmp0 = msg;
	char lineBuf[256];
    
    new_hd_buf = (p_sip_hd_t)_phCall;
    
	cur = (PUCHAR)strstr((char*)(msg+1), "\n");
    
    if( cur )
    {
		if( findNewLine::Matches(tmp0, &c, start) )
		{
			ESIPFIELD type;
			memset(lineBuf, 0, 256);
			memcpy(lineBuf, start, (tmp0-start));

			parse_sip_field_name((PBYTE)&lineBuf,type);
			parse_field(type, (PBYTE)&lineBuf, new_hd_buf);
		//	char tbuf[128];
#ifdef MSVC_6
		//	sprintf(tbuf, "content_length : %d", new_hd_buf->sipfield.content_length);
#else
		//	sprintf_s(tbuf, 128, "content_length : %d", new_hd_buf->sipfield.content_length);
#endif
		//    //LogWrite(tbuf);
		//}
		}
        
        //if( new_hd_buf->sipfield.content_length)
        //{
            //int c_len = new_hd_buf->sipfield.content_length;
            //copy_n_str(&new_hd_buf->sipfield.sdp,
                        //c_len, &dump_mem[mem_len-c_len]);
        //}
        
        return 1;
    }
    
    return 0;
}

//
///////////////////////////////////////////////
//

