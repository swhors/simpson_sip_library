#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
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
#include "sip_record_route.h"
#include "sip_sdp.h"
//#include "sip_sdp_parse.h"
#include "sip_allow.h"
#include "sip_authentication.h"
#include "sip_content.h"
#include "sip_field.h"
#include "sip_pre_line.h"
#include "sip_header.h"

#define MAX_COM   12

typedef int(auth_parse)(PVOID, PUCHAR);

typedef struct www_auth_comp{
    char * name;
    int len;
    void *func;
}www_auth_comp_t;

www_auth_comp_t auth[MAX_COM]=
{
    {"username", 8, &auth_user_name},
    {"realm", 5, &auth_realm},
    {"domain", 6, &auth_domain},
    {"qop", 3, &auth_qop},
    {"nonce", 5, &auth_nonce},
    {"cnonce", 6, &auth_cnonce},
    {"opaque", 6, &auth_opaque},
    {"uri", 3, &auth_uri},
    {"response", 8, &auth_response},
    {"stale", 5, &auth_stale},
    {"algorithm", 9, &auth_algorithm},
    {"nc", 2, &auth_nc}
};

void init_authentication_buf(PVOID p_buf)
{
    p_sip_authentication_t pauth = (p_sip_authentication_t)p_buf;
    if( pauth)
    {
		pauth->username="";
        pauth->realm="";
        pauth->domain="";
        pauth->qop="";
        pauth->nonce="";
        pauth->cnonce="";
        pauth->opaque="";
        pauth->response="";
        pauth->sip_uri="";
        pauth->algorithm = 0;
        pauth->stale = 0;
        pauth->nc = 0;
        pauth->type = 0;
    }
}

void free_authentication(PVOID _pAuth)
{
	p_sip_authentication_t pAuth = (p_sip_authentication_t)_pAuth;
	if( pAuth)
	{
		pAuth->username="";
		pAuth->realm="";
		pAuth->domain="";
		pAuth->qop="";
		pAuth->nonce="";
		pAuth->cnonce="";
		pAuth->opaque="";
		pAuth->sip_uri="";
		pAuth->response="";
	}
}

int auth_user_name(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->username = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_realm(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->realm = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_qop(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->qop = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_nonce(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->nonce = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_cnonce(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->cnonce = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_opaque(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->opaque = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_response(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->response = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_uri(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->sip_uri = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_domain(PVOID p_hd, PUCHAR dump)
{
	if( p_hd)
	{
		((p_sip_authentication_t)p_hd)->domain = (char*)dump;
		return 1;
	}
	return 0;
}

int auth_nc(PVOID p_hd, PUCHAR dump)
{
    p_sip_authentication_t phd = (p_sip_authentication_t)p_hd;
    PUCHAR cur=dump;
    phd->nc = atoi((char*)dump);
    return 1;
}

int auth_algorithm(PVOID p_hd, PUCHAR dump)
{
    p_sip_authentication_t phd = (p_sip_authentication_t)p_hd;
    PUCHAR cur=dump+1;
    phd->algorithm = 0;
    if( !memcmp(cur, ENC_MD5_STR, 3))
        phd->algorithm = ENC_MD5; 
    return 1;
}

int auth_stale(PVOID p_hd, PUCHAR dump)
{
    PUCHAR cur=dump;
    p_sip_authentication_t phd = (p_sip_authentication_t)p_hd;
    phd->stale = 0;
    cur++;

    if( !memcmp(cur, "TRUE",4))
        phd->stale = 1;
    else if( !memcmp(cur, "FALSE", 5))
        phd->stale = 2;
    return 1;
}

int parse_auth(PUCHAR dump, PVOID p_hd, int len, int opt)
{
	if( p_hd && dump)
	{
		return ( ( ((p_sip_authentication_t)p_hd)->parse(dump))?1:0);
	}
	return 0;
}

int make_sip_authentication(PVOID p_hd,
                            PUCHAR buff)
{
    p_sip_authentication_t phd=(p_sip_authentication_t)p_hd;
    
    if( phd->type == WWW_AUTHENTICATE)
        strcat((char*)buff, "WWW-Authenticate: DIGEST ");
    else if( phd->type == AUTHORIZATION)
        strcat((char*)buff, "Authorization: DIGEST ");
    else if( phd->type == PROXY_AUTHENTICATE )
        strcat((char*)buff, "Proxy-Authenticate: Digest ");
	else if( phd->type ==  PROXY_AUTHORIZATION )
	    strcat((char*)buff, "Proxy-Authorization: Digest ");
	else
        return 0;
    
	if( !phd->username.empty())
		strcatfm((char*)buff, "username=\"%s\", ", (char*)phd->username.data());
    
    if( !phd->realm.empty())
        strcatfm((char*)buff, "realm=\"%s\", ", (char*)phd->realm.data());
    
    if( !phd->domain.empty())
        strcatfm((char*)buff, "domain=\"%s\", ", (char*)phd->domain.data());
    
    if( !phd->qop.empty())
        strcatfm((char*)buff, "qop=%s, ", (char*)phd->qop.data());

    if( !phd->nonce.empty())
        strcatfm((char*)buff, "nonce=\"%s\", ", (char*)phd->nonce.data());
    
    if( !phd->cnonce.empty())
        strcatfm((char*)buff, "cnonce=\"%s\", ", (char*)phd->cnonce.data());
    
    if( !phd->opaque.empty())
        strcatfm((char*)buff, "opaque=\"%s\", ", (char*)phd->opaque.data());
      
	// swhors@naver.com
    if( !phd->sip_uri.empty())
        strcatfm((char*)buff, "uri=\"%s\", ", (char*)phd->sip_uri.data());
    //if( phd->sip_uri.len)
	//	strcatfm((char*)buff, "uri=\"sip:gmarket.co.kr\"");


    if( !phd->response.empty())
        strcatfm((char*)buff, "response=\"%s\", ", (char*)phd->response.data());
    
#if 0
    if( phd->stale == 1)
        strcatfm((char*)buff, "stale=TRUE, ");
    else if (phd->stale == 2)
        strcatfm((char*)buff, "stale=FALSE, ");
#endif    
    if( phd->algorithm == 1)
        strcat((char*)buff, "algorithm=MD5, ");
	else
        strcat((char*)buff, ", ");

	strcatfm((char*)buff, "nc=%08d", phd->nc);
    
    strcat((char*)buff, "\r\n");
    
    return 1;
}

int make_req_reg_auth(IN PVOID _pSipHd,
                      OUT PUCHAR msg,
                      IN char *hostip)
{
    UCHAR buff[512];
    p_sip_hd_t pcall = (p_sip_hd_t)_pSipHd;
    
    if( !pcall )
        return 0;
    
    memset(msg, 0, sizeof(msg));
    
    // 프리픽스 복사
    sprintf((char*)msg, "SIP/2.0 401 Unauthorized\r\n");
    
    // to tag 복사
    if(!make_to_from(_pSipHd, &pcall->sipfield.to,msg,0))
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
    //if( !make_contact(_pSipHd,&pcall->sipfield.contact,msg,pcall->preheadline.msg_type,hostip) )
        return 0;
    
    // Via 복사
    //make_via(&pcall->sipfield.via, msg, 1, hostip);
    
    // WWW-Authenticate 복사
    make_sip_authentication(&pcall->sipfield.www_auth, buff);
    strcat((char*)msg, (char*)&buff[0]);
	strcatfm((char*)msg, "Content-Length: %d\r\n\r\n", 0);
    //make_content_length(msg,0);
    return 1;
}

sip_authentication_t::sip_authentication_t()
{
}

PBYTE sip_authentication_t::parse(IN PBYTE inStr)
{
    auth_parse *fun;
	PBYTE	dump = inStr;
    int i = 0,readlen= 0;
    PUCHAR cur;
    cur = dump;
    //type = opt;
    while( cur )
    {
        for( i = 0; i < MAX_COM ; i++)
        {
            if( !memcmp(cur, auth[i].name, auth[i].len))
            {
                fun = (auth_parse*)auth[i].func;
                if( fun )
                {
                    readlen= fun(this, cur+auth[i].len);
                    if( readlen > 1)
                        cur = cur + readlen + auth[i].len+1;
                    else
                        cur = cur + auth[i].len + 1;
                }
                break;
            }
        }
        cur++;
        if( *cur =='\n' || *cur =='\r')
            break;
    }
    return cur;

}

// end of file
//
