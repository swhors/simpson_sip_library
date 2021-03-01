////////////////////////////////////////////////////////////
//
// File : sip_field.c
// 설명 : sip의 필드를 파싱하는 함수 구현.
// 날짜 : (1) 2005/09/09 swhors@skycom.ne.kr - 설명
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include "strptime.h"
#include <memory.h>
#endif

#include <iostream>
#include <string>

#include <vector>

using namespace std;
#include "host_addr.h"

#include "skytool.h"

#include "debug.h"
#include "SIpFieldBase.h"
#include "sip_tool.h"
#include "sip_info.h"
#include "sip_answer_code.h"
#include "sip_method.h"
#include "sip_uri.h"
#include "sip_to_from.h"
#include "sip_via.h"
#include "sip_callid.h"
#include "sip_cseq.h"
#include "sip_contact.h"
#include "sip_to_from.h"
#include "sip_authentication.h"
#include "sip_record_route.h"
#include "sip_sdp.h"
//#include "sip_sdp_parse.h"
#include "sip_allow.h"
#include "sip_content.h"

#include "sip_field.h"
#include "sip_pre_line.h"
#include "sip_header.h"


#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;
void LogWrite(char *fbuf);

static int contact_count;

typedef int(ParseFn)(PBYTE , PVOID );

typedef struct _PARSEFN
{
	e_sip_field_t field;
	ParseFn *fn;
}PARSEFN;

////////////////////////////////////////////////////////////
// Local Variable
////////////////////////////////////////////////////////////

#define MAX_FIELD_NUM    73

PARSEFN parsefnPoint[MAX_FIELD_NUM]=
{
	{e_field_unknown,NULL},
	{e_field_accept,parse_accept},
	{e_field_accept_encoding,parse_accept_encoding},
	{e_field_accept_language,parse_accept_language},
	{e_field_alert_info,NULL},
	{e_field_allow,parse_allow},
	{e_field_allow_events, NULL},
	{e_field_authentication_info, NULL},
	{e_field_authorization,parse_authorization},
	{e_field_call_id,parse_call_id},
	{e_field_call_info,NULL},
	{e_field_contact,parse_contact},
	{e_field_content_disposition,parse_content_disposition},
	{e_field_content_encoding,NULL},
	{e_field_content_language,parse_content_language},
	{e_field_content_length,parse_content_length},
	{e_field_content_type,parse_content_type},
	{e_field_cseq,parse_cseq},
	{e_field_date,parse_date},
	{e_field_error_info,NULL},
	{e_field_event,NULL},
	{e_field_expires,parse_expire},
	{e_field_from,parse_from},
	{e_field_in_reply_to,NULL},
	{e_field_max_forwards,parse_max_forward},
	{e_field_mime_version,NULL},
	{e_field_min_expires,NULL},
	{e_field_organization,NULL},
	{e_field_p_access_network_info,NULL},
	{e_field_p_asserted_identity,NULL},
	{e_field_p_associated_uri,NULL},
	{e_field_p_called_party_id,NULL},
	{e_field_p_charging_function_address,  NULL},
	{e_field_p_charging_vector,NULL},
	{e_field_p_dcs_trace_party_id,NULL},
	{e_field_p_dcs_osps,NULL},
	{e_field_p_dcs_billing_info,NULL},
	{e_field_p_dcs_laes,NULL},
	{e_field_p_media_authorization,NULL},
	{e_field_p_preferred_identity,NULL},
	{e_field_p_visited_network_id,NULL},
	{e_field_path,NULL},
	{e_field_privacy,NULL},
	{e_field_priority,NULL},
	{e_field_proxy_authenticate, parse_proxy_authenticate},
	{e_field_proxy_authorization,parse_proxy_authorization},
	{e_field_proxy_require,NULL},
	{e_field_rack,NULL},
	{e_field_reason,NULL},
	{e_field_record_route,parse_record_route},
	{e_field_refer_to,NULL},
	{e_field_referred_by,NULL},
	{e_field_replaces,NULL},
	{e_field_reply_to,NULL},
	{e_field_require,NULL},
	{e_field_retry_after,NULL},
	{e_field_route,parse_route},
	{e_field_rseq,NULL},
	{e_field_server,parse_server},
	{e_field_service_route,NULL},
	{e_field_session_expire,parse_session_expires},
	{e_field_sip_etag, NULL},
	{e_field_sip_if_match, NULL},
	{e_field_subject, parse_subject},
	{e_field_subscription_state, NULL},
	{e_field_supported,NULL},
	{e_field_timestamp,NULL},
	{e_field_to,parse_to},
	{e_field_unsupported, NULL},
	{e_field_user_agent,parse_user_agent},
	{e_field_via, parse_via},
	{e_field_warning, NULL},
	{e_field_www_authenticate, NULL},
};
////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////
static int parse_subject(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField)
	{
		pField->subject = (char*)&dump[9];
		return 1;
	}
	return 0;
}

static int parse_content_type(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->content_type= (char*)&dump[14];
		return 1;
	}
	return 0;
    //return get_all_string(dump, &pField->content_type,strlen((char*)dump));
}


static int parse_content_length(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField)
	{
		pField->content_length = atoi((char*)&dump[15]);
		return 0;
	}
    return 0;
}

static int parse_max_forward(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->max_forward = atoi((char*)(dump+13));
		return 1;
	}
    //pField->max_forward = sip_parse_int(dump, strlen((char*)dump)); 
    return 0;
}

static int parse_to(PUCHAR dump,PVOID _pField)
{
    int ret=0;
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->to.type = to_tag;
		return (pField->to.parse((PBYTE)(dump+4))!=NULL?1:0);
	}
    return 0;
}

static int parse_from(PUCHAR dump,PVOID _pField)
{
    int ret=0;
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		return (pField->from.parse((PBYTE)(dump+6))!=NULL?1:0);
	}
    return 0;
}

static int parse_call_id(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->cid.parse((PBYTE)(dump+9));
		return 1;
	}
	return 0;
    //return sip_parse_call_id(dump, &pField->cid, strlen((char*)dump)); 
}

static int parse_cseq(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->cseq.parse((PBYTE)(dump+6));
		return 1;
	}
	return 0;
    //return sip_parse_cseq(dump, &pField->cseq, strlen((char*)dump)); 
}

static int parse_via(PBYTE dump, void *_pField)
{
	p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
#ifdef LINUX
#else
#ifdef WIN32
		tr1::shared_ptr<sip_via_t> pVia(new sip_via_t);
#elif defined WINCE
		sip_via_t* pVia=new sip_via_t;
#endif
#endif
		//sip_via_t *pVia = new sip_via_t();
		if( pVia->parse((PBYTE)(dump+5)) )
		{
#ifdef LINUX
#else
#ifdef WIN32
			pField->via_list.push_back(pVia);
#elif defined WINCE
			pField->via_list.push_back(*pVia);
#endif
#endif
			return 1;
		}
	}
	return 0;
    
}

static int parse_contact(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
#ifdef LINUX
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<sip_contact_hd_t> pCon (new sip_contact_hd_t);
#elif defined WINCE
		sip_contact_hd_t *pCon =new sip_contact_hd_t;
#endif
#endif
		pCon->parse((PBYTE)(dump+9));
#ifdef LINUX
#elif defined WINCE || defined WIN32
#ifdef WIN32
		pField->contact_list.push_back(pCon);
#elif defined WINCE
		pField->contact_list.push_back(*pCon);
#endif
#endif
		return 1;
	}
	return 0;
    //return sip_parse_contact(dump, &pField->contact[contact_count++], strlen((char*)dump));
}

static int parse_server(unsigned char *dump, void *_pField)
{
	p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField)
	{
		pField->server = (char*)(dump+8);
		return 1;
	}
	return 0;
}

static int parse_expire(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->expire = atoi((char*)(dump+8));
		return 1;
	}
    return 0;
}

static int parse_user_agent(PUCHAR dump,PVOID _pField)
{
    //p_sip_field_t pField = (p_sip_field_t)_pField;
    //return get_all_string(dump, &pField->user_agent, strlen((char*)dump));
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		//pField->user_agent = atoi((char*)(dump+12));
		return 1;
	}
	return 0;
}

static int parse_date(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
    PUCHAR cur,at; 
    char *point;
	int len = strlen((char*)(dump+6));
    cur = dump + len + 1;
    
    while(*cur == ' ')
        cur++;
    
    memset(&pField->date, 0, sizeof(date_t));
    
    point = (char*)strptime((char*)cur, "%a, %d %b %Y %H:%M:%S", &pField->date.ltm);
    
    pField->date.ichecked = 1;
    
    if( point )
    {
        while(*point== ' ')
            point++;
        
        strcpy(pField->date.tm_zone, point);
        
        at = (PUCHAR)strstr((char*)pField->date.tm_zone,"\n");
        
        if( at ) 
            *at = 0;
        
        at = (PUCHAR)strstr((char*)pField->date.tm_zone,"\r");
        
        if( at )
            *at = 0;
    }
    return 1;
}

static int parse_www_auth(PUCHAR dump,PVOID _pField)
{
    return parse_auth(dump, _pField, strlen((char*)dump), 1);
}

static int parse_authorization(PUCHAR dump,PVOID _pField)
{
    return parse_auth(dump, _pField, strlen((char*)(dump+13)), 2);
}

static int parse_accept(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->accept = (char*)(dump + 8);
		return 1;
	}
	return 0;
}

static int parse_session_expires(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->session_expires = (char*)(dump + 17);
		return 1;
	}
	return 0;
}

static int parse_accept_language(PUCHAR dump, PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->accept_language = (char*)(dump+17);
	}
	return 0;
    //return sip_parse_string(dump, &pField->accept_language,strlen((char*)dump));
}

static int parse_accept_encoding(PUCHAR dump,PVOID _pField)
{
    return 1;
}

static int parse_supported(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
		pField->supported = (char*)(dump + 11);
	}
	return 0;
  //  return get_all_string(dump, &pField->supported, strlen((char*)dump));
}


static int parse_content_disposition(PUCHAR dump,PVOID _pField)
{
    return 1;
}


static int parse_content_encoding_internal(PUCHAR dump,PVOID _pField)
{
    return 1;
}

int parse_content_encoding(PUCHAR dump,PVOID _pField)
{
    return parse_content_encoding_internal(dump,_pField);
}

static int parse_content_language_internal(PUCHAR dump,PVOID _pField)
{
    return 1;
}

static int parse_proxy_authenticate(PUCHAR dump,PVOID _pField)
{
	return parse_auth(dump, _pField, strlen((char*)dump), PROXY_AUTHENTICATE);
}


static int parse_proxy_authorization(PUCHAR dump,PVOID _pField)
{
	return parse_auth(dump, _pField, strlen((char*)dump), PROXY_AUTHORIZATION);
}

static int parse_content_language(PUCHAR dump,PVOID _pField)
{
    return 0;
}

static int parse_allow(PUCHAR dump,PVOID _pField)
{
    p_sip_field_t pField = (p_sip_field_t)_pField;
    pField->allow = parse_allow_item(dump, &pField->accept,strlen((char*)dump),0);
    return 1;
}

static int parse_record_route(PUCHAR dump,PVOID _pField)
{
	p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
#if defined LINUX || defined MACOS
#ifdef LINUX
#elif defined MACOS
#endif
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<record_route_uri_t> p(new record_route_uri_t);
#elif defined WINCE
		record_route_uri_t* p= new record_route_uri_t;
#endif // WINCE || WIN32
#endif // LINUX
		//record_route_uri_t *p = new record_route_uri_t();
		p->type = 0;
		if( p->parse((PBYTE)(dump+14)) )
		{
#if defined LINUX || MACOS
#ifdef LINUX
#elif defined MACOS
#endif // LINUX
#elif defined WINCE || defined WIN32
#ifdef WIN32
			pField->record_list.push_back(p);
#elif defined WINCE
			pField->record_list.push_back(*p);
#endif // WINCE||WIN32
#endif // LINUX||MACOS
			return 1;
		}
	}
    return 0;
}

static int parse_route(PUCHAR dump,PVOID _pField)
{
	p_sip_field_t pField = (p_sip_field_t)_pField;
	if( pField )
	{
#if defined LINUX || defined MACOS
#ifdef LINUX
#elif defined MACOS
#endif
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<record_route_uri_t> p(new record_route_uri_t);
#elif defined WINCE
		record_route_uri_t *p = new record_route_uri_t();
#endif
#endif
		p->type = 1;
		if( p->parse((PBYTE)(dump+7)) )
		{
#if defined LINUX || defined MACOS
#elif defined WINCE || defined WIN32
#ifdef WIN32
			pField->record_list.push_back(p);
#elif defined WINCE
			pField->record_list.push_back(*p);
#endif
#endif
			return 1;
		}
	}
    return 0;
}

/* SIP의 헤더 필드를 찾아서 반환한다.
 * 입력 : 
 *      unsigned char *dump - 입력 데이터
 * 출력 :
 * 반환 :
 *      필드의 타입
 */ 
unsigned char * parse_sip_field_name(unsigned char * inStr,ESIPFIELD & type)
{
    unsigned char * tStr;
    C c;
    tStr = inStr;
    type = e_field_unknown;
    //typedef Char<':'> CHAR_END;
    typedef HCOLON CHAR_END;
    typedef Char<'-'> CHAR_HY;
    switch( *tStr )
    {
    case 'a':
    case 'A':
        {
            typedef And<CHAR_A,CHAR_C,CHAR_C,CHAR_E,CHAR_P,CHAR_T > accept;
            typedef And<CHAR_A,CHAR_L,CHAR_E,CHAR_R,
                        CHAR_T,CHAR_HY,CHAR_I,CHAR_N,
                        CHAR_F,CHAR_O,CHAR_END > alert_info;
            typedef And<CHAR_A,CHAR_L,CHAR_L,CHAR_O,
                        CHAR_W > allow;
			typedef And<CHAR_A,CHAR_U,CHAR_T,CHAR_H,CHAR_O,CHAR_R,CHAR_I,
						CHAR_Z,CHAR_A,CHAR_T,CHAR_I,CHAR_O,CHAR_N,CHAR_END > authorization;
            if( accept::Matches(tStr, &c ))
            {
                typedef And<CHAR_HY,CHAR_E,CHAR_N,CHAR_C,
                            CHAR_O,CHAR_D,CHAR_I,CHAR_N,
                            CHAR_G,CHAR_END > encoding;
                typedef And<CHAR_HY,CHAR_L,CHAR_A,CHAR_N,
                            CHAR_G,CHAR_U,CHAR_A,CHAR_G,
                            CHAR_E,CHAR_END > language;
                if( CHAR_END::Matches(tStr, &c))
                    type = e_field_accept;
                else if( encoding::Matches(tStr, &c))
                    type = e_field_accept_encoding;
                else if( language::Matches(tStr, &c))
                    type = e_field_accept_language;
            }
            else if( alert_info::Matches(tStr, &c))
            {
                type = e_field_alert_info;
            }
            else if( allow::Matches(tStr, &c ))
            {
                typedef And<CHAR_HY,CHAR_E,CHAR_V,CHAR_E,
                            CHAR_N,CHAR_T,CHAR_S,CHAR_END > allow_events;
                if( CHAR_END::Matches(tStr, &c))
                    type = e_field_allow;
                else if( allow_events::Matches(tStr,&c))
                    type = e_field_allow_events;
            }
			else if( authorization::Matches(tStr, &c ))
			{
                    type = e_field_authorization;
			}
        }
        break;
    case 'c':
    case 'C':
        {
            typedef And<Char<'c'>,CHAR_END > c_type;
            typedef And<CHAR_C,CHAR_A,CHAR_L,CHAR_L,CHAR_HY > call;
            typedef And<CHAR_C,CHAR_O,CHAR_N,CHAR_T,
                        CHAR_A,CHAR_C,CHAR_T,CHAR_END > contact;
            typedef And<CHAR_C,CHAR_O,CHAR_N,CHAR_T,
                        CHAR_E,CHAR_N,CHAR_T,CHAR_HY > content;
            typedef And<CHAR_C,CHAR_S,CHAR_E,CHAR_Q,
                        CHAR_END > cseq;
            
            if( c_type::Matches(tStr,&c) )
            {
                type = e_field_content_type;
            }
            else if( call::Matches(tStr,&c))
            {
                typedef And<CHAR_I,CHAR_D,CHAR_END > id;
                typedef And<CHAR_I,CHAR_N,CHAR_F,
                            CHAR_O,CHAR_END > info;
                if( id::Matches(tStr, &c ))
                    type = e_field_call_id;
                else if( info::Matches(tStr, &c) )
                    type = e_field_call_info;
            }
            else if( contact::Matches(tStr, &c ))
            {
                type = e_field_contact;
            }
            else if( content::Matches(tStr, &c ))
            {
                typedef And<CHAR_D,CHAR_I,CHAR_S,CHAR_P,
                            CHAR_O,CHAR_S,CHAR_I,CHAR_T,
                            CHAR_I,CHAR_O,CHAR_N,CHAR_END > disposition;
                typedef And<CHAR_E,CHAR_N,CHAR_C,CHAR_O,
                            CHAR_D,CHAR_I,CHAR_N,CHAR_G,
                            CHAR_END > encoding;
                typedef And<CHAR_L,CHAR_A,CHAR_N,CHAR_G,
                            CHAR_U,CHAR_A,CHAR_G,CHAR_E,
                            CHAR_END > language;
                typedef And<CHAR_L,CHAR_E,CHAR_N,CHAR_G,
                            CHAR_T,CHAR_H,CHAR_END > length;
                typedef And<CHAR_T,CHAR_Y,CHAR_P,CHAR_E,
                            CHAR_END > typel;
                if( disposition::Matches(tStr,&c))
                    type = e_field_content_disposition;
                else if( encoding::Matches(tStr, &c))
                    type = e_field_content_encoding;
                else if( language::Matches(tStr, &c))
                    type = e_field_content_language;
                else if( length::Matches(tStr, &c))
                    type = e_field_content_length;
                else if( typel::Matches(tStr, &c ))
                    type = e_field_content_type;
            }
            else if( cseq::Matches(tStr, &c ))
            {
                type = e_field_cseq;
            }
        }
        break;
    case 'd':
    case 'D':
        {
            typedef And<CHAR_D,CHAR_A,CHAR_T,CHAR_E > date;
            if( date::Matches(tStr,&c))
                type = e_field_date;
        }
        break;
    case 'e':
    case 'E':
        {
            typedef And<CHAR_E,CHAR_R,CHAR_R,CHAR_O,
                        CHAR_R,CHAR_HY,CHAR_I,CHAR_N,
                        CHAR_F,CHAR_O,CHAR_END > error_info;
            typedef And<CHAR_E,CHAR_V,CHAR_E,CHAR_N,
                        CHAR_T,CHAR_END > event;
            typedef And<CHAR_E,CHAR_X,CHAR_P,CHAR_I,
                        CHAR_R,CHAR_E,CHAR_S,CHAR_END > expires;
            if( error_info::Matches(tStr,&c))
                type = e_field_error_info;
            else if( event::Matches(tStr, &c ))
                type = e_field_event;
            else if( expires::Matches(tStr, &c ))
                type = e_field_expires;
        }
        break;
    case 'f':
    case 'F':
        {
            typedef And<CHAR_F,CHAR_R,CHAR_O,
                        CHAR_M,CHAR_END > from;
            if( from::Matches(tStr, &c ))
                type = e_field_from;
        }
        break;
    case 'i':
    case 'I':
        {
            typedef And<CHAR_I,CHAR_N,CHAR_HY,
                        CHAR_R,CHAR_E,CHAR_P,
                        CHAR_L,CHAR_Y,CHAR_HY,
                        CHAR_T,CHAR_O,CHAR_END >inreplyto;
            typedef And<Char<'i'>, CHAR_END > callid;
            if( inreplyto::Matches(tStr,&c))
                type = e_field_in_reply_to;
            else if( callid::Matches(tStr, &c))
                type = e_field_call_id;
        }
        break;
    case 'l':
        {
            typedef And<Char<'l'>, CHAR_END > length;
            if( length::Matches(tStr,&c))
                type = e_field_content_length;
        }
        break;
    case 'm':
    case 'M':
        {
            typedef And<Char<'m'>, CHAR_END > contact;
            typedef And<CHAR_M,CHAR_A,CHAR_X,CHAR_HY,
                        CHAR_F,CHAR_O,CHAR_R,CHAR_W,
                        CHAR_A,CHAR_R,CHAR_D,CHAR_S,
                        CHAR_END > max_forwards;
            typedef And<CHAR_M,CHAR_I,CHAR_M,CHAR_E,
                        CHAR_HY,CHAR_V,CHAR_E,CHAR_R,
                        CHAR_S,CHAR_I,CHAR_O,CHAR_N,
                        CHAR_END > mime_version;
            typedef And<CHAR_M,CHAR_I,CHAR_N,CHAR_HY,
                        CHAR_E,CHAR_X,CHAR_P,CHAR_I,
                        CHAR_R,CHAR_E,CHAR_S,CHAR_END > min_expires;
            if( contact::Matches(tStr, &c))
                type = e_field_contact;
            else if( max_forwards::Matches(tStr,&c))
                type = e_field_max_forwards;
            else if( mime_version::Matches(tStr, &c ))
                type = e_field_mime_version;
            else if( min_expires::Matches(tStr, &c ))
                type = e_field_min_expires;
        }
        break;
    case 'o':
    case 'O':
        {
            typedef And<CHAR_O,CHAR_R,CHAR_G,CHAR_A,
                        CHAR_N,CHAR_I,CHAR_Z,CHAR_A,
                        CHAR_T,CHAR_I,CHAR_O,CHAR_N,
                        CHAR_END > organization;
            if( organization::Matches(tStr,&c))
                type = e_field_organization;
        }
        break;
    case 'p':
    case 'P':
        {
            tStr++;
            if( *tStr < 0)
                break;
            switch(*tStr)
            {
            case '-':
                {
// ---------------------- '-' start
                    tStr++;
                    if( *tStr < 0 )
                        break;
                    switch(*tStr)
                    {
                    case 'a':
                    case 'A':
                        {
                            typedef And<CHAR_A,CHAR_C,CHAR_C,
                                        CHAR_E,CHAR_S,CHAR_S,
                                        CHAR_HY > access;
                            typedef And<CHAR_A,CHAR_S,CHAR_S > ass;
                            if( access::Matches(tStr,&c))
                            {
                                typedef And<CHAR_N,CHAR_E,CHAR_T,
                                            CHAR_W,CHAR_O,CHAR_R,
                                            CHAR_K,CHAR_HY,CHAR_I,
                                            CHAR_N,CHAR_F,CHAR_O,
                                            CHAR_END > network_info;
                                if( network_info::Matches(tStr,&c))
                                    type = e_field_p_access_network_info;
                            }
                            else if( ass::Matches(tStr,&c))
                            {
                                typedef And<CHAR_E,CHAR_R,CHAR_T,
                                            CHAR_E,CHAR_D,CHAR_HY,
                                            CHAR_I,CHAR_D,CHAR_E,
                                            CHAR_N,CHAR_T,CHAR_I,
                                            CHAR_T,CHAR_Y,CHAR_END
                                            > asserted_identity;
                                typedef And<CHAR_O,CHAR_C,CHAR_I,
                                            CHAR_A,CHAR_T,CHAR_E,
                                            CHAR_D,CHAR_HY,CHAR_U,
                                            CHAR_R,CHAR_I,CHAR_END
                                            > associated_uri;
                                if( asserted_identity::Matches(tStr,&c))
                                    type = e_field_p_asserted_identity;
                                else if( associated_uri::Matches(tStr,&c))
                                    type = e_field_p_associated_uri;
                            }
                        }
                        break;
                    case 'c':
                    case 'C':
                        {
                            typedef And<CHAR_C,CHAR_A,CHAR_L,CHAR_L,
                                        CHAR_E,CHAR_D,CHAR_HY,CHAR_P,
                                        CHAR_A,CHAR_R,CHAR_T,CHAR_Y,
                                        CHAR_HY,CHAR_I,CHAR_D > called_party_id;
                            typedef And<CHAR_C,CHAR_H,CHAR_A,
                                        CHAR_R,CHAR_G,CHAR_I,
                                        CHAR_N,CHAR_G,CHAR_HY > charging;
                            if( called_party_id::Matches(tStr,&c))
                            {
                                type = e_field_p_called_party_id;
                            }
                            else if( charging::Matches(tStr,&c))
                            {
                                typedef And<CHAR_F,CHAR_U,CHAR_N,
                                            CHAR_C,CHAR_T,CHAR_I,
                                            CHAR_O,CHAR_N,CHAR_HY > function;
                                typedef And<CHAR_V,CHAR_E,CHAR_C,
                                            CHAR_T,CHAR_O,CHAR_R,
                                            CHAR_END > vectord;
                                if( function::Matches(tStr, &c ))
                                {
                                    typedef And<CHAR_A,CHAR_D,CHAR_D,
                                                CHAR_R,CHAR_E,CHAR_S,
                                                CHAR_S,CHAR_END > address;
                                    if( address::Matches(tStr,&c))
                                       type=e_field_p_charging_function_address;
                                }
                                else if( vectord::Matches(tStr,&c ))
                                {
                                    type = e_field_p_charging_vector;
                                }
                            }
                        }
                        break;
                    case 'd':
                    case 'D':
                        {
                            typedef And<CHAR_D,CHAR_C,
                                        CHAR_S,CHAR_HY > dcs;
                            if(dcs::Matches(tStr,&c))
                            {
                                typedef And<CHAR_T,CHAR_R,CHAR_A,
                                          CHAR_C,CHAR_E,CHAR_HY,
                                          CHAR_P,CHAR_A,CHAR_R,
                                          CHAR_T,CHAR_Y,CHAR_HY,
                                          CHAR_I,CHAR_D,CHAR_END > tracePartyId;
                                typedef And<CHAR_O,CHAR_S,CHAR_P,
                                          CHAR_S,CHAR_END > osps;
                                typedef And<CHAR_B,CHAR_I,CHAR_L,
                                          CHAR_L,CHAR_I,CHAR_N,
                                          CHAR_G,CHAR_HY,CHAR_I,
                                          CHAR_N,CHAR_F,CHAR_O,
                                          CHAR_END > billingInfo;
                                typedef And<CHAR_L,CHAR_A,CHAR_E,
                                          CHAR_S,CHAR_END> laes;
                                if(tracePartyId::Matches(tStr,&c))
                                    type = e_field_p_dcs_trace_party_id;
                                else if( osps::Matches(tStr,&c))
                                    type = e_field_p_dcs_osps;
                                else if( billingInfo::Matches(tStr,&c))
                                    type = e_field_p_dcs_billing_info;
                                else if( laes::Matches(tStr,&c))
                                    type = e_field_p_dcs_laes;
                            }
                        }
                        break;
                    case 'm':
                    case 'M':
                        {
                            typedef And<CHAR_M,CHAR_E,CHAR_D,
                                        CHAR_I,CHAR_A,CHAR_HY > media;
                            if( media::Matches(tStr,&c))
                            {
                                typedef And<CHAR_A,CHAR_U,CHAR_T,
                                           CHAR_H,CHAR_O,CHAR_R,
                                           CHAR_I,CHAR_Z,CHAR_A,
                                           CHAR_T,CHAR_I,CHAR_O,
                                           CHAR_N,CHAR_END > authorization;
                                if( authorization::Matches(tStr,&c))
                                    type = e_field_p_media_authorization;
                            }
                        }
                        break;
                    case 'p':
                    case 'P':
                        {
                            typedef And<CHAR_P,CHAR_R,CHAR_E,
                                        CHAR_F,CHAR_E,CHAR_R,
                                        CHAR_R,CHAR_E,CHAR_D,
                                        CHAR_HY > preferred;
                            if( preferred::Matches(tStr,&c))
                            {
                                typedef And<CHAR_I,CHAR_D,CHAR_E,
                                            CHAR_N,CHAR_T,CHAR_I,
                                            CHAR_T,CHAR_Y,CHAR_END > identity;
                                if( identity::Matches(tStr,&c))
                                    type = e_field_p_preferred_identity;
                            }
                        }
                        break;
                    case 'v':
                    case 'V':
                        {
                            typedef And<CHAR_V,CHAR_I,CHAR_S,
                                        CHAR_I,CHAR_T,CHAR_E,
                                        CHAR_D,CHAR_HY > visited;
                            if( visited::Matches(tStr,&c))
                            {
                                typedef And<CHAR_N,CHAR_E,CHAR_T,
                                            CHAR_W,CHAR_O,CHAR_R,
                                            CHAR_K,CHAR_HY,CHAR_I,
                                            CHAR_D,CHAR_END > network_id;
                                if( network_id::Matches(tStr,&c))
                                    type = e_field_p_visited_network_id;
                            }
                        }
                        break;
                    default:
                        break;
                    }
// ---------------------- '-' end
                }
                break;
            case 'a':
            case 'A':
                {
                    typedef And<CHAR_A,CHAR_T,
                        CHAR_H,CHAR_END > path;
                    if( path::Matches(tStr,&c))
                        type = e_field_path;
                }
                break;
            case 'r':
            case 'R':
                {
                    typedef And<CHAR_R,CHAR_O,CHAR_X,
                                CHAR_Y,CHAR_HY > proxy;
                    typedef And<CHAR_R,CHAR_I>pri;
                    if( pri::Matches(tStr,&c))
                    {
                        typedef And<CHAR_V,CHAR_A,CHAR_C,
                                    CHAR_Y,CHAR_END > privacy;
                        typedef And<CHAR_O,CHAR_R,CHAR_I,
                                    CHAR_T,CHAR_Y,CHAR_END > priority;
                        if( priority::Matches(tStr, &c ))
                            type = e_field_priority;
                        else if( privacy::Matches(tStr,&c))
                            type = e_field_privacy;
                    }
                    else if( proxy::Matches(tStr, &c ))
                    {
                        typedef And<CHAR_A,CHAR_U,CHAR_T,
                                    CHAR_H,CHAR_E,CHAR_N,
                                    CHAR_T,CHAR_I,CHAR_C,
                                    CHAR_A,CHAR_T,CHAR_E,
                                    CHAR_END > authenticate;
                        typedef And<CHAR_A,CHAR_U,CHAR_T,
                                    CHAR_H,CHAR_O,CHAR_R,
                                    CHAR_I,CHAR_Z,CHAR_A,
                                    CHAR_T,CHAR_I,CHAR_O,
                                    CHAR_N,CHAR_END > authorization;
                        typedef And<CHAR_R,CHAR_E,CHAR_Q,
                                    CHAR_U,CHAR_I,CHAR_R,
                                    CHAR_E,CHAR_END > require;
                        if( authenticate::Matches(tStr,&c))
                            type=e_field_proxy_authenticate;
                        if( authorization::Matches(tStr,&c))
                            type=e_field_proxy_authorization;
                        if( require::Matches(tStr,&c))
                            type=e_field_proxy_require;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    case 'r':
    case 'R':
        {
            tStr++;
            switch(*tStr)
            {
            case 'a':
            case 'A':
                {
                    typedef And<CHAR_A,CHAR_C,CHAR_K,
                                CHAR_END > ack;
                    if( ack::Matches(tStr,&c))
                        type = e_field_rack;
                }
                break;
            case 'e':
            case 'E':
                {
                    tStr++;
                    switch(*tStr)
                    {
                    case 'a':
                    case 'A':
                        {
                            typedef And<CHAR_A,CHAR_S,CHAR_O,
                                        CHAR_N,CHAR_END > reason;
                            if( reason::Matches(tStr,&c))
                                type = e_field_reason;
                        }
                        break;
                    case 'c':
                    case 'C':
                        {
                            typedef And<CHAR_C,CHAR_O,CHAR_R,
                                        CHAR_D,CHAR_HY,CHAR_R,
                                        CHAR_O,CHAR_U,CHAR_T,
                                        CHAR_E,CHAR_END > record_route;
                            if( record_route::Matches(tStr,&c))
                                type = e_field_record_route;
                        }
                        break;
                    case 'f':
                    case 'F':
                        {
                            typedef And<CHAR_F,CHAR_E,CHAR_R,
                                        CHAR_HY,CHAR_T,CHAR_O,
                                        CHAR_END > refer_to;
                            typedef And<CHAR_F,CHAR_E,CHAR_R,
                                        CHAR_R,CHAR_E,CHAR_D,
                                        CHAR_HY,CHAR_B,CHAR_Y,
                                        CHAR_END > referred_by;
                            if(refer_to::Matches(tStr,&c))
                                type = e_field_refer_to;
                            else if( referred_by::Matches(tStr,&c))
                                type = e_field_referred_by;
                        }
                        break;
                    case 'p':
                    case 'P':
                        {
                            typedef And<CHAR_P,CHAR_L,CHAR_A,
                                        CHAR_C,CHAR_E,CHAR_S,
                                        CHAR_END > replaces;
                            typedef And<CHAR_P,CHAR_L,CHAR_Y,
                                        CHAR_HY,CHAR_T,CHAR_O,
                                        CHAR_END > reply_to;
                            if( replaces::Matches(tStr,&c))
                                type = e_field_replaces;
                            else if( reply_to::Matches(tStr,&c))
                                type = e_field_reply_to;
                        }
                        break;
                    case 'q':
                    case 'Q':
                        {
                            typedef And<CHAR_Q,CHAR_U,CHAR_I,
                                        CHAR_R,CHAR_E,CHAR_END > require;
                            if( require::Matches(tStr,&c))
                                type = e_field_require;
                        }
                        break;
                    case 't':
                    case 'T':
                        {
                            typedef And<CHAR_T,CHAR_R,CHAR_Y,
                                        CHAR_HY,CHAR_A,CHAR_F,
                                        CHAR_T,CHAR_E,CHAR_R,
                                        CHAR_END > retry_after;
                            if( retry_after::Matches(tStr,&c))
                                type = e_field_retry_after;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case 'o':
            case 'O':
                {
                    typedef And<CHAR_O,CHAR_U,CHAR_T,
                                CHAR_E,CHAR_END > route;
                    if( route::Matches(tStr,&c))
                        type = e_field_route;
                }
                break;
            case 's':
            case 'S':
                {
                    typedef And<CHAR_S,CHAR_E,CHAR_Q,
                                CHAR_END > rseq;
                    if( rseq::Matches(tStr,&c))
                        type = e_field_rseq;
                }
                break;
            default:
                break;
            }
        }
        break;
    case 's':
    case 'S':
        {
            typedef And<CHAR_S,CHAR_E > se;
            typedef And<CHAR_S,CHAR_U,CHAR_B > sub;
            typedef And<CHAR_S,CHAR_U,CHAR_P > sup;
            typedef And<CHAR_S,CHAR_I,CHAR_P,
                        CHAR_HY > sip_p;
            if( se::Matches(tStr, &c))
            {
                typedef And<CHAR_R,CHAR_V,CHAR_E,
                            CHAR_R,CHAR_END > server;
                typedef And<CHAR_S,CHAR_S,CHAR_I,
                            CHAR_O,CHAR_N,CHAR_HY,
                            CHAR_E,CHAR_X,CHAR_P,CHAR_I,
                            CHAR_R,CHAR_E,CHAR_S,CHAR_END > session_exp;
                typedef And<CHAR_R,CHAR_V,CHAR_I,
                            CHAR_C,CHAR_E,CHAR_HY,
                            CHAR_R,CHAR_O,CHAR_U,
                            CHAR_T,CHAR_E,CHAR_END > service_route;
                if( server::Matches(tStr,&c))
                    type = e_field_server;
                else if( session_exp::Matches(tStr,&c))
                    type = e_field_session_expire;
                else if( service_route::Matches(tStr,&c))
                    type = e_field_service_route;
            }
            else if( sip_p::Matches(tStr,&c))
            {
                typedef And<CHAR_E,CHAR_T,CHAR_A,
                            CHAR_G,CHAR_END > etag;
                typedef And<CHAR_I,CHAR_F,CHAR_HY,
                            CHAR_M,CHAR_A,CHAR_T,
                            CHAR_C,CHAR_H,CHAR_END > if_match;
                if( etag::Matches(tStr,&c))
                    type = e_field_sip_etag;
                else if( if_match::Matches(tStr,&c))
                    type = e_field_sip_if_match;
            }
            else if( sub::Matches(tStr, &c ))
            {
                typedef And<CHAR_J,CHAR_E,CHAR_C,
                            CHAR_T,CHAR_END > subject;
                typedef And<CHAR_S,CHAR_C,CHAR_R,
                            CHAR_I,CHAR_P,CHAR_T,
                            CHAR_I,CHAR_O,CHAR_N,
                            CHAR_HY,CHAR_S,CHAR_T,
                            CHAR_A,CHAR_T,CHAR_E,
                            CHAR_END > subscript;
                if( subject::Matches(tStr,&c))
                    type = e_field_subject;
                else if( subscript::Matches(tStr,&c))
                    type = e_field_subscription_state;
            }
            else if( sup::Matches(tStr, &c ))
            {
                typedef And<CHAR_P,CHAR_O,CHAR_R,
                            CHAR_T,CHAR_E,CHAR_D > supported;
                if( supported::Matches(tStr,&c))
                    type = e_field_supported;
            }
        }
        break;
    case 't':
    case 'T':
        {
            typedef And<CHAR_T,CHAR_I,CHAR_M,CHAR_E,
                        CHAR_S,CHAR_T,CHAR_A,CHAR_M,
                        CHAR_P,CHAR_END > timestamp;
            typedef And<CHAR_T,CHAR_O,CHAR_END > to;
            if( timestamp::Matches(tStr,&c))
                type = e_field_timestamp;
            if( to::Matches(tStr,&c))
                type = e_field_to;
        }
        break;
    case 'u':
    case 'U':
        {
            typedef And<CHAR_U,CHAR_N,CHAR_S,CHAR_U,
                        CHAR_P,CHAR_P,CHAR_O,CHAR_R,
                        CHAR_T,CHAR_E,CHAR_D,CHAR_END > unsupported;
            typedef And<CHAR_U,CHAR_S,CHAR_E,CHAR_R,
                        CHAR_HY,CHAR_A,CHAR_G,CHAR_E,
                        CHAR_N,CHAR_T,CHAR_END > useragent;
            if( unsupported::Matches(tStr,&c))
                type = e_field_unsupported;
            else if( useragent::Matches(tStr,&c))
                type = e_field_user_agent;
        }
        break;
    case 'v':
    case 'V':
        {
            typedef And<Char<'v'>, CHAR_END > via1;
            typedef And<CHAR_V,CHAR_I,CHAR_A,CHAR_END > via2;
            typedef Or< via1, via2 > via;
            if( via::Matches(tStr,&c))
                type = e_field_via;
        }
        break;
    case 'w':
    case 'W':
        {
            typedef And<CHAR_W,CHAR_A,CHAR_R,CHAR_N,
                        CHAR_I,CHAR_N,CHAR_G,CHAR_END > warning;
            typedef And<CHAR_W,CHAR_W,CHAR_W,CHAR_HY > www;
            if( warning::Matches(tStr,&c))
            {
                type = e_field_warning;
            }
            else if( www::Matches(tStr,&c))
            {
                typedef And<CHAR_A,CHAR_U,CHAR_T,CHAR_H,
                            CHAR_E,CHAR_N,CHAR_T,CHAR_I,
                            CHAR_C,CHAR_A,CHAR_T,CHAR_E,
                            CHAR_END > authenticate;
                if( authenticate::Matches(tStr, &c ))
                    type = e_field_www_authenticate;
            }
        }
        break;
    default:
        break;
    }
    if( type != e_field_unknown )
        return tStr;
    return NULL;
}
unsigned char * get_field_name(ESIPFIELD type)
{
    switch(type)
    {
    case e_field_accept:
        return (PBYTE)("Accept");
    case e_field_accept_encoding:
        return (PBYTE)("Accept-Encoding");
    case e_field_accept_language:
        return (PBYTE)("Accept-Language");
    case e_field_session_expire:
        return (PBYTE)("Session-Expire");
    case e_field_alert_info:
        return (PBYTE)("Alert-Info");
    case e_field_allow:
        return (PBYTE)("Allow");
    case e_field_allow_events:
        return (PBYTE)("Allow-Events");
    case e_field_authentication_info:
        return (PBYTE)("Authentication-Info");
    case e_field_authorization:
        return (PBYTE)("Authorization");
    case e_field_call_id:
        return (PBYTE)("Call-Id");
    case e_field_call_info:
        return (PBYTE)("Call-Info");
    case e_field_contact:
        return (PBYTE)("Contact");
    case e_field_content_disposition:
        return (PBYTE)("Content-Disposition");
    case e_field_content_encoding:
        return (PBYTE)("Content-Encoding");
    case e_field_content_language:
        return (PBYTE)("Content-Language");
    case e_field_content_type:
        return (PBYTE)("Content-Type");
    case e_field_cseq:
        return (PBYTE)("CSeq");
    case e_field_date:
        return (PBYTE)("Date");
    case e_field_error_info:
        return (PBYTE)("Error-Info");
    case e_field_event:
        return (PBYTE)("Event");
    case e_field_expires:
        return (PBYTE)("Expires");
    case e_field_from:
        return (PBYTE)("From");
    case e_field_in_reply_to:
        return (PBYTE)("In-Reply-To");
    case e_field_max_forwards:
        return (PBYTE)("Max-Forwards");
    case e_field_mime_version:
        return (PBYTE)("MIME-Version");
    case e_field_min_expires:
        return (PBYTE)("Min-Expires");
    case e_field_organization:
        return (PBYTE)("Organization");
    case e_field_priority:
        return (PBYTE)("Priority");
    case e_field_proxy_authenticate:
        return (PBYTE)("Proxy-Authenticate");
    case e_field_proxy_authorization:
        return (PBYTE)("Proxy-Authorization");
    case e_field_proxy_require:
        return (PBYTE)("Proxy-Require");
    case e_field_rack:
        return (PBYTE)("RAck");
    case e_field_rseq:
        return (PBYTE)("RSeq");
    case e_field_record_route:
        return (PBYTE)("Record-Route");
    case e_field_reply_to:
        return (PBYTE)("Reply-To");
    case e_field_require:
        return (PBYTE)("Require");
    case e_field_retry_after:
        return (PBYTE)("Retry-After");
    case e_field_route:
        return (PBYTE)("Route");
    case e_field_server:
        return (PBYTE)("Server");
    case e_field_subject:
        return (PBYTE)("Subject");
    case e_field_subscription_state:
        return (PBYTE)("Subscription-State");
    case e_field_supported:
        return (PBYTE)("Supported");
    case e_field_timestamp:
        return (PBYTE)("Timestamp");
    case e_field_to:
        return (PBYTE)("To");
    case e_field_unsupported:
        return (PBYTE)("Unsupported");
    case e_field_user_agent:
        return (PBYTE)("User-Agent");
    case e_field_via:
        return (PBYTE)("Via");
    case e_field_warning:
        return (PBYTE)("Warning");
    case e_field_www_authenticate:
        return (PBYTE)("WWW-Authenticate");
    case e_field_p_access_network_info:
        return (PBYTE)("P-Access-Network-Info");
    case e_field_p_asserted_identity:
        return (PBYTE)("P-Asserted-Identity");
    case e_field_p_associated_uri:
        return (PBYTE)("P-Associated-URI");
    case e_field_p_called_party_id:
        return (PBYTE)("P-Called-Party-ID");
    case e_field_p_charging_function_address:
        return (PBYTE)("P-Charging-Function-Address");
    case e_field_p_charging_vector:
        return (PBYTE)("P-Charging-Vector");
    case e_field_p_dcs_trace_party_id:
        return (PBYTE)("P-DCS-Trace-Party-ID");
    case e_field_p_dcs_osps:
        return (PBYTE)("P-DCS-OSPS");
    case e_field_p_dcs_billing_info:
        return (PBYTE)("P-DCS-Billing-Info");
    case e_field_p_dcs_laes:
        return (PBYTE)("P-DCS-LAES");
    case e_field_p_media_authorization:
        return (PBYTE)("P-Media-Authorization");
    case e_field_p_preferred_identity:
        return (PBYTE)("P-Preferred-Identity");
    case e_field_p_visited_network_id:
        return (PBYTE)("P-Visited-Network-ID");
    case e_field_path:
        return (PBYTE)("Path");
    case e_field_privacy:
        return (PBYTE)("Privacy");
    case e_field_reason:
        return (PBYTE)("Reason");
    case e_field_refer_to:
        return (PBYTE)("Refer-To");
    case e_field_referred_by:
        return (PBYTE)("Referred-By");
    case e_field_replaces:
        return (PBYTE)("Replaces");
    case e_field_service_route:
        return (PBYTE)("Service-Route");
    case e_field_sip_etag:
        return (PBYTE)("SIP-Etag");
    case e_field_sip_if_match:
        return (PBYTE)("SIP-If-Match");
    default:
        break;
    }
    return (PBYTE)("Unknown");
}

int parse_field(e_sip_field_t field, unsigned char * inStr, PVOID _pSipHd)
{
	if( parsefnPoint[(int)field].fn )
		return parsefnPoint[(int)field].fn(inStr,_pSipHd);
	return 0;
}

sip_field_t::sip_field_t()
{
}
sip_field_t::~sip_field_t()
{
	free();
}

void sip_field_t::free()
{
	if( this->accept.size() > 0 )
		this->accept.erase(accept.begin(), accept.end() );
	if( this->accept_language.size() > 0 )
		this->accept_language.erase(accept_language.begin(), accept_language.end());
	this->cid.free();
	if( this->contact_list.size() > 0 )
	{
#if defined LINUX || defined MACOS
		for( vector<sip_contact_hd_t>::iterator p = contact_list.begin();
#elif defined WIN32 || defined WINCE
#ifdef WIN32
		for( vector<tr1::shared_ptr<sip_contact_hd_t>>::iterator p = contact_list.begin();
#elif defined WINCE
		for( vector<sip_contact_hd_t>::iterator p = contact_list.begin();
#endif
#endif
			p != contact_list.end(); p++)
		{
#if	defined LINUX || defined MACOS
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			p->get()->free();
#elif defined WINCE
			p->free();
#endif
#endif
			//p->free();
		}
		this->contact_list.erase(contact_list.begin(), contact_list.end());
	}
	if( this->content_type.size() > 0 )
		this->content_type.erase(content_type.begin(), content_type.end());
	if( this->record_list.size() > 0 )
		this->record_list.erase(record_list.begin(), record_list.end());
	if( this->server.size() > 0 )
		this->server.erase(server.begin(), server.end());
	if( this->session_expires.size() > 0 )
		this->session_expires.erase(session_expires.begin(), session_expires.end());
	if( this->subject.size() > 0 )
		this->subject.erase(subject.begin(), subject.end());
	if( this->supported.size() > 0 )
		this->supported.erase(this->supported.begin(), this->supported.end());
	if( this->user_agent.size() > 0 )
		this->user_agent.erase(this->user_agent.begin() , this->user_agent.end());
	if( this->via_list.size() > 0 )
	{
		//for( vector<tr1::shared_ptr<sip_via_t>>::iterator p = via_list.begin();
		//	p != via_list.end(); p++)
		//{
		//	p->free();
		//}
		this->via_list.erase(via_list.begin(), via_list.end());
	}
}

PBYTE sip_field_t::pasre(PBYTE inStr)
{
	C c;
    PBYTE tmp0,start, origin;
    typedef FindRepeat<NEWLINE> findNewLine;
    tmp0 = inStr;
	origin = inStr;
	char lineBuf[256];
	while( findNewLine::Matches(tmp0, &c, start) )
	{
		int len = (start - origin);
		if( len >= 2)
		{
			ESIPFIELD type;
			memset(lineBuf, 0, 256);
			memcpy(lineBuf, origin, len);
			origin = tmp0;

			parse_sip_field_name((PBYTE)&lineBuf,type);
			parse_field(type, (PBYTE)&lineBuf, this);
		//	char tbuf[128];
#ifdef MSVC_6
		//	sprintf(tbuf, "content_length : %d", new_hd_buf->sipfield.content_length);
#else
		//	sprintf_s(tbuf, 128, "content_length : %d", new_hd_buf->sipfield.content_length);
#endif
		//    //LogWrite(tbuf);
		//}
		}
		else
			break;
	}
        
    if(content_length)
    {
		// swhors@naver.com 2009/09/17
		// sdp 정보 처리, 처리 구문 추가 처리 요청

		//int c_len = new_hd_buf->sipfield.content_length;
		//copy_n_str(&new_hd_buf->sipfield.sdp,
		//c_len, &dump_mem[mem_len-c_len]);
    }
        
    return tmp0;
}
int	sip_field_t::print(PBYTE outStr)
{
	char szLine[256];
	memset(szLine, 0, 256);
	if( this->via_list.size() > 0 )
	{
		for(
#if defined LINUX || defined MACOS
			vector<sip_via_t>::iterator c = via_list.begin();
#elif defined WINCE || defined WIN32
#ifdef WIN32
			vector<tr1::shared_ptr<sip_via_t>>::iterator c = via_list.begin();
#elif defined WINCE
			vector<sip_via_t>::iterator c = via_list.begin();
#endif
#endif
		 c < via_list.end(); c++)
		{
#ifdef LINUX

#elif defined WINCE || defined WIN32
#ifdef WIN32
			if( c->get()->print((PBYTE)&szLine) )
#elif defined WINCE
			if( c->print((PBYTE)&szLine) )
#endif
#endif
				strcat((char*)outStr, szLine);
			memset(szLine, 0, 256);
		}
	}
	
	strcatfm((char*)outStr, "Max-Forwards: %d\r\n", this->max_forward);
	
	memset(szLine, 0, 256);
	
	if( this->record_list.size() > 0 )
	{
		for(
#if defined LINUX || defined MACOS
			vector<record_route_uri_t>::iterator c= record_list.begin();
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			vector<tr1::shared_ptr<record_route_uri_t>>::iterator c= record_list.begin();
#elif defined WINCE
			vector<record_route_uri_t>::iterator c= record_list.begin();
#endif
#endif
		c!=record_list.end();c++)
		{
#if defined LINUX || defined MACOS
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			if( c->get()->print((PBYTE)&szLine) )
#elif defined WINCE
			if( c->print((PBYTE)&szLine) )
#endif
#endif
				strcat((char*)outStr, szLine);
			memset(szLine, 0, 256);
		}
	}
	
	memset(szLine, 0, 256);
	
	if( this->contact_list.size() )
	{
		for(vector<
#if defined LINUX || defined MACOS
			sip_contact_hd_t
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			tr1::shared_ptr<sip_contact_hd_t>
#elif defined WINCE
			sip_contact_hd_t
#endif
#endif
		>::iterator c = this->contact_list.begin(); c< contact_list.end();c++)
		{
#if defined LINUX || defined MACOS
			if( c->print((PBYTE)&szLine))
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			if( c->get()->print((PBYTE)&szLine))
#elif defined WINCE
			if( c->print((PBYTE)&szLine))
#endif
#endif
				strcat((char*)outStr, szLine);
			memset(szLine, 0, 256);
		}
	}
	
	memset(szLine, 0, 256);
	
	if( this->to.print((PBYTE)&szLine))
		strcat((char*)outStr, szLine);
	
	memset(szLine, 0, 256);
	
	if( this->from.print((PBYTE)&szLine) )
		strcat((char*)outStr, szLine);
	
	memset(szLine, 0, 256);
	
	if( this->cid.print((PBYTE)&szLine))
		strcat((char*)outStr, szLine);
	
	memset(szLine, 0, 256);
	
	if( this->cseq.print((PBYTE)&szLine))
		strcat((char*)outStr, szLine);
	memset(szLine, 0, 256);


	if( print_allow(this->allow, (PBYTE)&szLine) )
		strcat((char*)outStr, szLine);

	memset(szLine, 0, 256);
	//if( this->date.ltm.
	
	if( this->expire >= 0 )
		strcatfm((char*)outStr, "Expires: %d\r\n", this->expire);
	if( !this->user_agent.empty() )
		strcatfm((char*)outStr, "User-Agent: %s\r\n", this->user_agent.data());
	if( !this->content_type.empty())
		strcatfm((char*)outStr, "Content-Type: %s\r\n", this->content_type.data() );
	return 1;
}
int	sip_field_t::build()
{
	return 0;
}
//
////////////////////////////////////////////////////////////
//