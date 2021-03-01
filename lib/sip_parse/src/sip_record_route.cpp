//////////////////////////////////////////////////////////////////
// sip_record_route.c
//
// create : 2005/3/2 swhors@skycom.ne.kr
//          리코드-라우트와 관련된 파싱과 관리 함수 구현.
// modify : 2005/3/7 swhors@skycom.ne.kr
//          check_record_route 추가-설멸은 해당 함수 참조
//////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#endif // WIN32

#include <iostream>
#include <string>

using namespace std;
#include "host_addr.h"
#include "skytool.h"
#include "debug.h"
#include "SIpFieldBase.h"
#include "sip_tool.h"

#include "sip_answer_code.h"
#include "sip_method.h"
#include "sip_uri.h"

#include "sip_record_route.h"

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;

#define FTAG_PRE "ftag="
#define LR_PRE "lr="
#define RVRRPARAM "rvRRParam="

#define RECORD_ROUTE_ENABLE

//#ifdef RECORD_ROUTE_ENABLE
//#undef RECORD_ROUTE_ENABLE
//#endif
void free_record_route(PVOID _pRoute)
{
	record_route_uri_t * pRoute = (record_route_uri_t*)_pRoute;
	if( pRoute )
	{
		pRoute->free();
	}
}


static int make_record_route_internal(IN PVOID _pRoute,OUT PUCHAR buff)
{
#if 0
#ifdef RECORD_ROUTE_ENABLE
    p_record_route_t pRoute = (p_record_route_t)_pRoute;
    p_record_route_uri_t puri;
    puri = pRoute->route_uri;
	while( puri )
	{
        if( strlen( puri->ip )> 0) 
		{
        	if( puri->type == 1)
				strcat((char*)buff, "Record-Route: <sip:");
		    else if( puri->type == 2)
				strcat((char*)buff, "Route: <sip:");
			else
                strcat((char*)buff, "Record-Route: <sip:");
			if( strlen(puri->u_num) > 0)
			{
				strcatfm((char*)buff, "%s@", puri->u_num);
			}
			if( puri->port > 0)
				strcatfm((char*)buff,"%s:%d", puri->ip, puri->port);
			else
			    strcat((char *)buff, puri->ip);
			if( strlen( puri->tag ) > 0)
				strcatfm((char*)buff, ";ftag=%s", puri->tag);
			if( strlen( puri->lr)> 0 )
				strcatfm((char*)buff, ";lr=%s", puri->lr);
			else
                strcat((char*)buff, ";lr");
			if( strlen( puri->rvRRParam) > 0 )
				strcatfm((char*)buff, ";rvRRParam=%s", puri->rvRRParam);
            strcat((char*)buff, ">\r\n");
        }
		puri = (p_record_route_uri_t )puri->p_next;
    }
#endif// RECORD_ROUTE_ENABLE
#endif
    return 0;
}
int make_record_route(IN PVOID _pRoute,OUT PUCHAR buff)
{
    return make_record_route_internal(_pRoute, buff);
}


void record_route_uri_t::free()
{
	uri.free();
	//this->haddr.init();
	tag="";
	rvRRParam="";
	this->lr = 0;
	psp = "";
	type = 0;	// 0-Record-Route, 1-Route
}

record_route_uri_t	&record_route_uri_t::operator=(const record_route_uri_t&other)
{
	uri = other.uri;
	this->lr = other.lr;
	this->psp = other.psp;
	this->rvRRParam = other.rvRRParam;
	this->tag = other.tag;
	this->type = other.type;
	return *this;
}

int	record_route_uri_t::operator==(const record_route_uri_t&other)
{
	if(this->uri == other.uri)
	{
		if( this->rvRRParam == other.rvRRParam)
		{
			if( this->psp == other.psp)
			{
				if( this->lr == other.lr)
					return 1;
			}
		}
	}
	return 0;
}

int record_route_uri_t::build(int _type, char *szIp, int port, int _lr, char *_rvRRParam, char *ftag, char *_psp)
{
	this->type = _type;
	uri.build(szIp, port,"");
	//this->haddr.type = ip;
	//sprintf_s(this->haddr.eadr.ip.sadr, 24, "%s", szIp);
	//this->haddr.eadr.ip.port = port;
	this->lr = _lr;
	this->rvRRParam = _rvRRParam;
	this->tag = ftag;
	this->psp = _psp;
	return 1;
}

record_route_uri_t::record_route_uri_t()
{
	free();
}

PBYTE record_route_uri_t::parse(PBYTE inStr)
{
	PBYTE tmp,start;
	C c;
	typedef And<Char<';'>,CHAR_L,CHAR_R > lr_str;
	typedef And<CHAR_P,CHAR_S,CHAR_P,Char<'='> > s_psp_prefix;
	typedef PlusRepeat<AllChar > s_psp_mid;
	typedef And<s_psp_prefix,s_psp_mid > s_psp;
	typedef And<CHAR_F,CHAR_T,CHAR_A,CHAR_G, Char<'='>> s_ftag;

	typedef And<Char<';'>,CHAR_R,CHAR_V,
		CHAR_R,CHAR_R,CHAR_P,CHAR_A,
		CHAR_R,CHAR_A,CHAR_M,Char<'='>,
		PlusRepeat<DIGIT> > rvrrparam_str;

	typedef FindRepeat<s_psp > fPsp;
	typedef FindRepeat<lr_str > fLr;
	typedef FindRepeat<rvrrparam_str > fRvrrparam;

	tmp = inStr;
	uri.parse(tmp);
	tmp = inStr;

	if( fLr::Matches(tmp,&c,start))
		lr = 1;

	tmp = inStr;
	if( fPsp::Matches(tmp,&c,start))
	{
		psp = "";
		psp.append((char*)(start+4),(int)(tmp-start-4) );
	}
	
	tmp = inStr;

	if( fRvrrparam::Matches(tmp,&c,start))
	{
		rvRRParam = "";
		rvRRParam.append((char*)(start+11), ((int)(tmp-start-11)));
	}
	return tmp;
}
int	record_route_uri_t::print(PBYTE outStr)
{
	char *tOutStr= (char *)outStr;
	if( outStr )
	{
		char tbuf[128];
		memset(tbuf, 0, 128);
		if( uri.print((PBYTE)&tbuf))
		{
			sprintf(tOutStr, "%s <", (this->type?"Route:":"Record-Route:"));
			strcat(tOutStr, tbuf);
			strcatfm(tOutStr, "%s%s%s%s%s%s%s>\r\n",
				(this->rvRRParam.empty()?"":";rvRRParam="),
				(this->rvRRParam.empty()?"":(char*)this->rvRRParam.data()),
				(this->psp.empty()?"":";psp="),
				(this->psp.empty()?"":(char*)this->psp.data()),
				(this->tag.empty()?"":";ftag="),
				(this->tag.empty()?"":(char*)this->tag.data()),
				(this->lr?";lr":""));
			return 1;
		}
	}
	return 0;
}


//////////////
