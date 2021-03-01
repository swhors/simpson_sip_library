////////////////////////////////////////////////////////////
//
// File : sip_make_msg.c
// 설명 : sip 메세지를 생성하는 함수 구현.
// 날짜 : (1) 2005/09/09 swhors@skycom.ne.kr
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////
#include <stdio.h>

#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#endif

#include <iostream>
#include <string>
#include <vector>

using namespace std;
#include "host_addr.h"
#include "strptime.h"
#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"
#include "SIpFieldBase.h"


#include "sip_answer_code.h"
#include "sip_method.h"
#include "sip_tool.h"
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
#include "sip_body.h"
#include "sip_field.h"
#include "sip_pre_line.h"
#include "sip_header.h"
#include "sip_parse.h"

#include "sip_make_msg.h"


////////////////////////////////////////////////////////////
//
// Function.
//
////////////////////////////////////////////////////////////


// if opt is 0, msg is trying. if opt is 1, msg is ringing.
static int make_pre_invite_msg_internal(PVOID p_call,
                                        PUCHAR msg,
                                        int opt,
                                        char *hostip)
{
	/*
    p_sip_hd_t pcall = (p_sip_hd_t)p_call;
    
    if( !pcall )
        return 0;
    
    memset(msg, 0, sizeof(msg));
    
    // 프리픽스 복사
    if( opt == 0)
        sprintf((char*)msg, "SIP/2.0 100 Trying\r\n");
    else if( opt == 1)
        sprintf((char*)msg, "SIP/2.0 180 Ringing\r\n");
    
    // to tag 복사
    if(!make_to_from(p_call, &pcall->sipfield.to,msg, 0))
        return 0;
    
    // from taf 복사 
    if( !make_to_from(p_call, &pcall->sipfield.from, msg,0))
        return 0;
    
    // C Seq 복사 
    if( !make_cseq(&pcall->sipfield.cseq, msg))
        return 0;
    
    // Call-ID 복사 
    if( !make_callid(&pcall->sipfield.cid, msg))
        return 0;
    
    // Via 복사
    make_via(&pcall->sipfield.via,msg, 0,hostip);
    make_content_length(msg,0);
    strcat((char*)msg, "\r\n");
	*/
    return 1;
}

// make_trying_msg
// input :
//        PVOID p_call - call_information
// output:
//        PUCHAR msg - printed message
// return:
//        If success, return 1.
int make_trying_msg(IN PVOID _pSipHd,
                    OUT PUCHAR msg,
                    IN char *hostip)
{
    return make_pre_invite_msg_internal(_pSipHd, msg, 0, hostip);
}

int make_ringing_msg(IN PVOID _pSipHd,
                     OUT PUCHAR msg,
                     IN char *hostip)
{
    return make_pre_invite_msg_internal(_pSipHd, msg, 1, hostip);
}

// make_ok_msg
// input : 
//        PVOID p_call - call_information
// output:
//        PUCHAR msg - printed message
// return:
//        if success, return 1.
int make_ua_ok_msg(IN PVOID _pSipHd,
				   OUT PUCHAR msg,
				   IN char *hostip)
{
    return make_pre_answer_msg(_pSipHd, msg, 200, 0, hostip, NULL,1);
}
int make_ok_msg(IN PVOID _pSipHd,
                OUT PUCHAR msg,
                IN char *hostip)
{
    return make_pre_answer_msg(_pSipHd, msg, 200, 0, hostip, NULL,0);
}


int make_ok_msg_with_sdp(IN PVOID _pSipHd,
                         OUT PUCHAR msg,
                         IN char *hostip,
                         IN PVOID _pSdp)
{
    return make_pre_answer_msg(_pSipHd, msg, 200, 0, hostip, _pSdp,0);
}

int make_ok_msg_ua_with_sdp(IN PVOID _pSipHd,
                         OUT PUCHAR msg,
                         IN char *hostip,
                         IN PVOID _pSdp)
{
    return make_pre_answer_msg(_pSipHd, msg, 200, 0, hostip, _pSdp,1);
}

int make_fail_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN int error_code,
                  IN int opt,
                  IN char *hostip)
{
    return make_pre_answer_msg(_pSipHd, msg, error_code, opt, hostip, NULL,0);
}

int make_fail_ua_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN int error_code,
                  IN int opt,
                  IN char *hostip)
{
    return make_pre_answer_msg(_pSipHd, msg, error_code, opt, hostip, NULL,1);
}

int make_pre_answer_msg(IN PVOID _pSipHd,
                        OUT PUCHAR msg,
                        IN int code,
                        IN int opt,
                        IN char *hostip,
                        IN PVOID _pSdp,
						IN int via_opt )
{
#if 0
    p_sip_hd_t pcall = (p_sip_hd_t) _pSipHd;
    
    if( !pcall )
        return 0;
    
    memset(msg, 0, sizeof(msg));
    
    // 프리픽스 복사
    sprintf((char*)msg, "SIP/2.0");
    
    if(!make_state_code_msg(code , msg))
        return 0;
    
    strcat((char*)msg, "\r\n");
    
    // to tag 복사
    if(!make_to_from(_pSipHd, &pcall->sipfield.to,msg,opt))
        return 0;
    
    // from taf 복사 
    if( !make_to_from(_pSipHd, &pcall->sipfield.from, msg,0))
        return 0;
    
    // C Seq 복사 
    if( !make_cseq(&pcall->sipfield.cseq, msg))
        return 0;
    
    // Call-ID 복사 
    if( !make_callid(&pcall->sipfield.cid, msg))
        return 0;
    
    // Contact 복사
    if( !make_contact(_pSipHd, &pcall->sipfield.contact[0], msg, pcall->preheadline.msg_type, hostip))
    {
        if( pcall->sipfield.cseq.type == msg_invite
            //|| pcall->sipfield.cseq.type == msg_bye
            //|| pcall->sipfield.cseq.type == msg_ack
			)
            return 0;
    }
    
    // Via 복사
    make_via(&pcall->sipfield.via, msg, via_opt,hostip);
    // record_route
    make_record_route(&pcall->sipfield.route, msg);
    
	if( pcall->preheadline.msg_type == msg_register)
	{
		make_expires_msg(msg, pcall->sipfield.expire, 1);
	}
	else
	{
		make_expires_msg(msg, pcall->sipfield.expire, 0);
	}
	// session-expires
    make_session_expires_msg(msg, &pcall->sipfield.session_expires);


	// allow 생성 invite에 대한 OK인 경우.
    if( code == 200 &&
		pcall->sipfield.cseq.type == msg_invite )
	{
        //Allow
        make_allow_field(msg, pcall->sipfield.allow);
		
		// User Agent
		strcat((char*)msg, "User-Agent: SKY_SIP_UserAgent\r\n");

		// Supported
		strcat((char*)msg, "Supported: replaces\r\n");
        
        // Accept-Language
        make_accept_language_msg(msg, &pcall->sipfield.accept_language);
	    
		// content type 복사
        make_content_type(msg, &pcall->sipfield.content_type);
	}
    
    // sdp information 복사
    if( !_pSdp )
    {
        make_content_length(msg,0);
        strcat((char*)msg, "\r\n");
    }
    else
    {
        char sdpmsg[2048];
		memset(sdpmsg, 0, 2048);
        if( make_sdp_msg(_pSdp, sdpmsg, 2048) > 0)
        {
            int len = strlen(sdpmsg);
            make_content_length(msg,len);
            strcatfm((char*)msg, "\r\n%s", sdpmsg);
            return 1;
        }
        make_content_length(msg,0);
        strcat((char*)msg, "\r\n");
    }
#endif
    return 1;
}

// if contact is 1, contact is required..
static int make_msg_body_by_pcall_internal(
                                IN PVOID _pSipHd,
                                OUT PUCHAR msg,
                                IN int www_auth,
                                IN int contact,
                                IN char *to_ip,
                                IN int to_port,
                                IN char *hostip,
                                IN int rtp_proxy,
                                PVOID p_sdp,
                                IN int viaopt,
								IN int b_ua
								)
{
#if 0
    p_sip_hd_t pcall = (p_sip_hd_t)_pSipHd;
	int i;
    if( !pcall )
        return 0;
    // To tag
	if( pcall->preheadline.msg_type != msg_register && to_ip)
        make_to_from_by_ip(_pSipHd, &pcall->sipfield.to,msg,0, to_ip, to_port);
	else
        make_to_from(_pSipHd, &pcall->sipfield.to,msg,0);
    
	// From tag
	if( !b_ua )
        make_to_from(_pSipHd, &pcall->sipfield.from,msg,0);
	else
	    make_to_from(_pSipHd, &pcall->sipfield.from,msg,2);
    
    // CSeq
    make_cseq(&pcall->sipfield.cseq, msg);
    
    // Call id
    make_callid(&pcall->sipfield.cid, msg);
    
    // Via 
    // append itself..
    //
    // 자기 자신을 추가하는 부분은 앞단의 프로세스 모듈에서 
    // append_viacom으로 구현됨. 따라서, 이 부분에서 
    // 임으로 추가하는 부분은 삭제됨.
    // 2005/02/28 pm 2:00 by swhors@skycom.ne.kr
    //
    if( viaopt )
        make_via(&pcall->sipfield.via,msg, 1,hostip);
	else
        make_via1(msg, hostip, &pcall->preheadline.sip_version,hostip);

    
    // Subject
    if( pcall->preheadline.msg_type != msg_ack)
        make_subject(msg, &pcall->sipfield.subject);
    
    // Contact
	if( pcall->preheadline.hdtype == sip_request && pcall->preheadline.msg_type == msg_register )
	{
		for( i = 0; i < pcall->sipfield.contact_count; i++)
		{
			make_contact(_pSipHd, &pcall->sipfield.contact[i], 
				msg, pcall->preheadline.msg_type, hostip);
		}
	}
    else 
	{
		if( !make_contact(_pSipHd, &pcall->sipfield.contact[0], msg, pcall->preheadline.msg_type, hostip))
        {
            if(contact && pcall->preheadline.hdtype==sip_request)
            {
                if( pcall->preheadline.msg_type!=msg_bye && pcall->preheadline.msg_type!=msg_ack)
                    return 0;
            }
            else if( contact && pcall->preheadline.hdtype==sip_response)
            {
                if( pcall->preheadline.state_code == 200 )
                   return 0;
            } 
		}
    }
    
    // Date
    if( pcall->preheadline.msg_type != msg_ack)
        make_date_msg(msg, &pcall->sipfield.date);
    
    // record_route
    make_record_route(&pcall->sipfield.route, msg);
    
    //Accept
    if( pcall->preheadline.msg_type != msg_ack)
        make_str_field(msg, "Accept", &pcall->sipfield.accept);
	
	if( pcall->preheadline.msg_type == msg_register)
	{
		make_expires_msg(msg, pcall->sipfield.expire, 1);
	}
	else
	{
		make_expires_msg(msg, pcall->sipfield.expire, 0);
	}

	// session-expires
    make_session_expires_msg(msg, &pcall->sipfield.session_expires);
    
    // etc information
    if( pcall->preheadline.msg_type != msg_bye)
    {
        if( pcall->preheadline.msg_type != msg_register && pcall->preheadline.msg_type != msg_ack)
        {
            //Allow
            make_allow_field(msg, pcall->sipfield.allow);
            
            // Accept-Language
            make_accept_language_msg(msg, &pcall->sipfield.accept_language);
        }
        if( pcall->preheadline.msg_type != msg_register && pcall->preheadline.msg_type != msg_ack)
        {
            // Supported
            make_supported_msg(msg, &pcall->sipfield.supported);
        }
        if( pcall->preheadline.msg_type != msg_ack)
        // Content-Type
        make_content_type(msg, &pcall->sipfield.content_type);
        // user agent
        if( pcall->preheadline.msg_type != msg_ack)
            make_user_agent_msg(msg, &pcall->sipfield.user_agent);
    }
    
    // Max-forward;
    make_int_field(msg, "Max-Forwards", pcall->sipfield.max_forward);
	// Allow
	if( pcall->sipfield.allow > 0 )
	{
		make_allow(pcall->sipfield.allow, msg);
	}
    if( www_auth )
	//if( pcall->sipfield.www_auth.type > 0)
	    make_sip_authentication(&pcall->sipfield.www_auth, msg);
    // Content-Length
    // sdp
    if((pcall->sipfield.content_length > 0 || p_sdp) && pcall->preheadline.msg_type!=msg_ack)
    {
        if(!rtp_proxy)
        {
            if( pcall->sipfield.sdp.len > 2 )
            {
                if( pcall->sipfield.sdp.str[0] == '\r'
                    && pcall->sipfield.sdp.str[1] == '\n')
                {
                    make_content_length(msg,pcall->sipfield.content_length);
                    strcatfm((char*) msg, "%s", pcall->sipfield.sdp.str);
                }
				else
				{
                    make_content_length(msg,pcall->sipfield.content_length);
                    strcatfm((char*) msg, "\r\n%s", pcall->sipfield.sdp.str);
				}
            }
            else
            {
                make_content_length(msg,pcall->sipfield.content_length);
                strcatfm((char*) msg, "\r\n%s", pcall->sipfield.sdp.str);
            }
        }
        else
        {
            if( p_sdp )
            {
                char sdpbuff[5120];
                int len;
                memset(sdpbuff, 0, 5120);
                make_sdp_msg(p_sdp, sdpbuff, 5120);
                len = strlen(sdpbuff);
                make_content_length(msg,len);
                strcatfm((char*)msg, "\r\n%s", sdpbuff);
            }
            else
            {
                make_content_length(msg,pcall->sipfield.content_length);
                strcatfm((char*) msg, "\r\n%s", pcall->sipfield.sdp.str);
            }
        }
    }
    else
    {
        make_content_length(msg,0);
        strcat((char *)msg, "\r\n");
    }
#endif
    return 1;
}

int make_msg_body_by_pcall(IN PVOID _pSipHd,
                           OUT PUCHAR msg,
                           IN int www_auth,
                           IN int contact,
                           IN char *to_ip,
                           IN int to_port,
                           IN char *hostip,
                           IN int rtp_proxy,
                           IN PVOID _psdp,
                           IN int viaopt,
						   IN int b_ua)
{
    return make_msg_body_by_pcall_internal(_pSipHd, 
        msg, 
        www_auth, 
        contact, 
        to_ip, 
        to_port,
        hostip,
        rtp_proxy,
        _psdp,
        viaopt,
		b_ua);
}


// make_forward_msg
// input :
//        PVOID _pSipHd - call_information
// output:
//        UCHAR msg[] - printed message
// return:
//        If success, return 1.
static int make_forward_msg_internal(IN PVOID _pSipHd,
                                     OUT PUCHAR msg,
                                     IN int buflen,
                                     IN int contact,
                                     IN char *to_ip,
                                     IN int port,
                                     IN char *hostip,
                                     IN int rtp_proxy,
                                     IN PVOID _psdp)
{
#if 0
    p_sip_hd_t pcall = (p_sip_hd_t)_pSipHd;
    
    if( !pcall )
        return 0;
    memset(msg, 0, buflen);
    // 1 Prefix
    // 
    if( pcall->type == sip_request)
        make_req_prefix_at(pcall,msg, to_ip, 0);
    else if( pcall->type == sip_response)
        make_res_prefix(pcall,msg);
    else
        return 0;
#endif
    return make_msg_body_by_pcall(_pSipHd, msg, 0, contact, to_ip, port,hostip, rtp_proxy, _psdp, 1, 0);
}

// UA 자신의 메세지를 생성한다.
static int make_ua_msg_internal(IN PVOID _pSipHd,
                                OUT PUCHAR msg,
								IN int www_auth,
                                IN int buflen,
                                IN int contact,
                                IN char *to_ip,
                                IN int port,
                                IN char *hostip,
                                IN PVOID _psdp)
{
    p_sip_hd_t pSipHd = (p_sip_hd_t)_pSipHd;
#if 0
    if( !_pSipHd )
        return 0;
    memset(msg, 0, buflen);
    // 1 Prefix
    // 
    if( pSipHd->type == sip_request)
	{
		if( pSipHd->preheadline.msg_type == msg_ack || pSipHd->preheadline.msg_type==msg_bye || !to_ip)
		{
            make_req_prefix_at(pSipHd,msg, 
				pSipHd->preheadline.uri.haddr.eadr.ip.sadr,
				pSipHd->preheadline.uri.haddr.eadr.ip.port);
		}
		else
		{
            make_req_prefix_at(pSipHd,msg, to_ip, port);
		}
	}
    else if( pSipHd->type == sip_response)
        make_res_prefix(pSipHd,msg);
    else
        return 0;
    if( pSipHd->type == sip_request)
        return make_msg_body_by_pcall(pSipHd, msg, www_auth, contact, to_ip, port,hostip, 1, _psdp, 1, 1);
#endif
    return make_msg_body_by_pcall(pSipHd, msg, www_auth, contact, to_ip, port,hostip, 1, _psdp, 0, 1);
}

int make_ua_msg(IN PVOID _pSipHd,
                OUT PUCHAR msg,
				IN int www_auth,
                IN int buflen,
                IN int contact,
                IN char *to_ip,
                IN int port,
                IN char *hostip,
                IN PVOID _psdp)
{
    p_sip_hd_t pSipHd = (p_sip_hd_t)_pSipHd;
	if( pSipHd->preheadline.msg_type == msg_ack ||
		pSipHd->preheadline.hdtype == sip_response )

        return make_ua_msg_internal(_pSipHd, msg, www_auth, buflen, contact, to_ip, port, hostip, NULL);
    return make_ua_msg_internal(_pSipHd, msg, www_auth, buflen, contact, to_ip, port, hostip, _psdp);
}
                            
int make_forward_msg_to(IN PVOID _pSipHd,
                        OUT PUCHAR msg,
                        IN int buflen,
                        IN int contact,
                        IN char *to_ip,
                        IN int port,
                        IN char *hostip,
                        IN int rtp_proxy,
                        IN PVOID _psdp)
{
    return make_forward_msg_internal(_pSipHd, msg, buflen, contact, to_ip, port, hostip, rtp_proxy, _psdp);
}

int make_forward_msg(IN PVOID _pSipHd,
                     OUT PUCHAR msg,
                     IN int buflen,
                     IN int contact,
                     IN char *hostip,
                     IN int rtp_proxy,
                     IN PVOID _psdp)
{
    p_sip_hd_t phCall = (p_sip_hd_t)_pSipHd;
    return make_forward_msg_internal(_pSipHd, msg, buflen, contact, 
			(!phCall->sipfield.to.uri.haddr.hostAddr.empty()?(char*)phCall->sipfield.to.uri.haddr.hostAddr.data():hostip),
			phCall->sipfield.to.uri.haddr.port, hostip, rtp_proxy, _psdp);
}

int make_send_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN PUCHAR sfield,
                  IN char *hostip)
{
    p_sip_hd_t pcall = (p_sip_hd_t) _pSipHd;
    memset( msg, 0, sizeof( msg));
    //
    // 자기 자신을 추가하는 부분은 앞단의 프로세스 모듈에서 
    // append_viacom으로 구현됨. 따라서, 이 부분에서 
    // 임으로 추가하는 부분은 삭제됨.
    // 2005/02/28 pm 2:00 by swhors@skycom.ne.kr
    //
    //strcatfm((char*)msg,"%s sip:%s@211.56.186.98 SIP/2.0\r\n",sfield, call_num);
    // To tag
    if(!make_to_from(_pSipHd, &pcall->sipfield.to,msg,0))
        return 0;

    // From tag
    if(!make_to_from(_pSipHd, &pcall->sipfield.from,msg,0))
        return 0;
    
    // via tag
    //make_via(&pcall->sipfield.via,msg, 1,hostip);
    if( !make_cseq(&pcall->sipfield.cseq, msg))
        return 0;
    
    // Call id
    if( !make_callid(&pcall->sipfield.cid, msg))
        return 0;
    strcat((char*)msg, "Content-Length: 0\r\n");
    return 1;
}

//
////////////////////////////////////////////////////////////
//
