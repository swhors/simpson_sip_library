////////////////////////////////////////////////////////////
//
// File : sip_contact.c
// 설명 : sip의 필드의 contact 정보.
// 날짜 : (1) 2004/11/xx swhors@skycom.ne.kr - 설명
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////

#include <stdio.h>

#ifdef WIN32
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <iostream>
#include <string>

using namespace std;

#include "host_addr.h"

#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "SIpFieldBase.h"

#include "sip_method.h"
#include "sip_uri.h"

#include "sip_contact.h"

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;


////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define SIP_CONTACT_ACTION "action="
#define SIP_CONTACT_ACTION_LEN 7
#define SIP_CONTACT_EXPIRES "expires="
#define SIP_CONTACT_EXPIRES_LEN 8

////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////

void init_contact_buf(PVOID _pContact)
{
    p_sip_contact_hd_t pcontact = (p_sip_contact_hd_t) _pContact;
    
    if( pcontact )
    {
		pcontact->free();
    }
}

void free_contact(p_sip_contact_hd_t hd)
{
	if( hd)
	{
		hd->free();
	}
}

int sip_parse_contact(PUCHAR dump, p_sip_contact_hd_t hd, int len)
{
	if( hd && dump )
	{
		return (hd->parse(dump)?1:0);
	}
	return 0;
}

int make_contact(IN PVOID _phCall,IN PVOID p_hd, OUT PUCHAR buff,IN int msg_type,IN char *hostip)
{
    if( _phCall )
	{
		sip_contact_hd_t* phHd = (sip_contact_hd_t*)_phCall;
		if( hostip)
		{
			phHd->uri.haddr.hostAddr=hostip;
		}
		return phHd->print(buff);
	}
	return 0;
}

int build_contact(IN PVOID _pContact,
                  IN char *action,
                  IN int expire,
                  IN char *hostip,
                  IN int port,
                  IN char *u_number)
{
    p_sip_contact_hd_t pContact = (p_sip_contact_hd_t) _pContact;
	if( pContact)
	{
		return pContact->build(u_number, hostip, port, action, 0, expire, NULL);
	}
	return 1;
}

int contactcpy(IN PVOID _pDes,IN PVOID _pSrc)
{
	p_sip_contact_hd_t pDes, pSrc;
	if( !_pDes || !_pSrc )
		return 0;
	pDes = (p_sip_contact_hd_t)_pDes;
    pSrc = (p_sip_contact_hd_t)_pSrc;
	pDes->free();
	*pDes = *pSrc;
	return 1;
}

sip_contact_hd_t::sip_contact_hd_t()
{
	free();
}

sip_contact_hd_t::~sip_contact_hd_t()
{
	free();
	if( this->action.size() > 0 )
		this->action.erase(action.begin(), action.end());
}

void sip_contact_hd_t::free()
{
	uri.free();
	action="";
	q = 0;
	expires = 0;
	checked = 0;
}

sip_contact_hd_t &sip_contact_hd_t::operator=(const sip_contact_hd_t &other)
{
	this->uri = other.uri;
	this->action = other.action;
	this->expires = other.expires;
	this->checked = other.checked;
	this->q = other.q;
	return *this;
}

PBYTE sip_contact_hd_t::parse(IN PBYTE inStr)
{
	PBYTE tmp1,tmp2,start;
    C c;
    typedef And<CHAR_Q,Char<'='>,
                PlusRepeat<AllNum> > qStr;
    typedef And<CHAR_A,CHAR_C,CHAR_T,CHAR_I,
                CHAR_O,CHAR_N,Char<'='>,
                PlusRepeat<AllChar> > action;
    typedef And<CHAR_E,CHAR_X,CHAR_P,CHAR_I,
                CHAR_R,CHAR_E,CHAR_S,Char<'='>,
                PlusRepeat<AllNum> > expires;
    
    typedef FindRepeat<expires > fExpires;
    typedef FindRepeat<action > fAction;
    typedef FindRepeat<qStr > fQ;
    
    if( inStr <= NULL )
        return NULL;
    tmp1 = inStr;
	tmp1 = this->uri.parse(tmp1);
    //tmp1 = parse_sip_uri(tmp1, &this->sipUri);
    tmp2 = tmp1;
    if( fExpires::Matches(tmp2, &c, start))
    {
        this->expires = atoi((char*)(start+8));
    }
    tmp2 = tmp1;
    if( fQ::Matches(tmp2, &c, start))
    {

        this->q = atoi((char*)(start+2));
    }
    tmp2 = tmp1;
    if( fAction::Matches(tmp2, &c, start ))
    {
		this->action.append((char*)(start+7) , (int)(start-7-tmp2));
    }
    return tmp1;
}
int sip_contact_hd_t::print(OUT PBYTE outStr)
{
    if( outStr <= NULL )
        return 0;
    else
    {
	    char *tmp = (char*)outStr;
        sprintf(tmp,"%s", "Contact: ");
        char tbuf[256];

		this->uri.print((PBYTE)&tbuf);
        strcatfm(tmp, "<%s>",tbuf);
        if( this->q > 0 )
        {
            strcatfm(tmp, ";q=%d", this->q);
        }
        if( this->expires >= 0) 
        {
            strcatfm(tmp, ";expires=%d", this->expires);
        }
		if( !this->action.empty())
        {
			strcatfm(tmp, ";action=%s", action.data());
        }
        strcat(tmp, "\r\n");
    }
	return 1;
}

int sip_contact_hd_t::build(char *uid,char *ip,int _port,char * _action,int _q,int _expires,char *s_display)
{
	if(!this->uri.build(ip,_port,uid) )
		return 0;
	this->action = (_action?_action:"");
	this->q = _q;
	this->expires = _expires;
	return 1;
}

//
////////////////////////////////////////////////////////////
//
