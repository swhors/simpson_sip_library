#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#endif
//#include "sip_info_display.h"

#include <iostream>
#include <string>

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

#include "peg_def.h"
#include "peg_xsip.h"
using namespace peg_templ;


#define TO_FROM_TAG_CODE "tag="

void init_tofrom_buf(PVOID p_buf)
{
    p_tofrom_tag_t ptag = (p_tofrom_tag_t)p_buf;
    if( ptag)
    {
        ptag->type = none_tag;
        ptag->display="";
        ptag->passwd="";
        ptag->tag="";
        init_uri(&ptag->uri);
    }
}

int tofrom_parse(PUCHAR dump, p_tofrom_tag_t hd)
{
	if( hd) 
	{
		if( hd->parse(dump) )
			return 1;
	}
	return 0;
}

int check_to_tag_by_ip( PVOID p_to, PUCHAR s_ip)
{
    p_tofrom_tag_t pto  = (p_tofrom_tag_t)p_to;
    if( pto && s_ip)
    {
		if( *pto == (const char*)s_ip)
			return 1;
    }
    return 0;
}

int check_tofrom_unum(PVOID p_to, PVOID p_from)
{
    p_tofrom_tag_t pto,pfrom;
    pto = (p_tofrom_tag_t)p_to;
    pfrom = (p_tofrom_tag_t)p_from;
    if( pto && pfrom )
    {
		if( pto->uri.uid== pfrom->uri.uid)
        {
			return 1;
        }
    }
    return 0;
}


int to_from_parse_upwd(PUCHAR dump, p_tofrom_tag_t tofrom)
{
    return 0;
}

void free_tofrom(p_tofrom_tag_t hd)
{
	hd->display="";
	hd->uri.free();
    //free_str(&hd->user);
	hd->passwd="";
	hd->tag="";
}

int make_to_from_by_ip(IN PVOID _pSipHd,IN PVOID _pTag,OUT PUCHAR msg,IN int self,IN char *ip,IN int port)
{
    return 0;
}

int make_to_from(IN PVOID _pSipHd,IN PVOID _pTag,OUT PUCHAR msg,IN int self)
{
	if( _pSipHd)
	{
		p_tofrom_tag_t pSipHd = (p_tofrom_tag_t)_pSipHd;
		return pSipHd->print(msg);
	}
	return 0;
}

static int build_tag(IN PVOID _pTag,
                 IN char *hostip,
                 IN char *u_num,
                 IN int port,
                 IN char *display,
				 IN char *u_tag,
                 to_from_enum_t type)
{
    p_tofrom_tag_t pTag=(p_tofrom_tag_t)_pTag;
	if( pTag )
	{
		pTag->build(u_num, hostip, port, display, u_tag, type, 0);
	}
    return 1;
}

int build_to_tag(IN PVOID _pTag,
                 IN char *hostip,
                 IN char *u_num,
                 IN int port,
                 IN char *display,
				 IN char *u_tag)
{
    return build_tag( _pTag, hostip, u_num, port, display, u_tag, to_tag);
}

int build_from_tag(IN PVOID _pTag,
                   IN char *hostip,
                   IN char *u_num,
                   IN int  port,
                   IN char *display,
				   IN char *u_tag)
{
    return build_tag( _pTag, hostip, u_num, port, display, u_tag, from_tag);
}

void tofrom_tag_t::free()
{
	if( this->tag.size() > 0 )
		tag.erase(tag.begin(), tag.end() );
	if( this->passwd.size () > 0 )
		passwd.erase(passwd.begin(), passwd.end());
	if( display.size() > 0 )
		display.erase(display.begin(), display.end());
	uri.free();
}

tofrom_tag_t::tofrom_tag_t()
{
	this->tag="";
	this->passwd="";
	this->uri.free();
	this->display="";
	this->type = from_tag;
}

tofrom_tag_t::~tofrom_tag_t()
{
	free();
}

tofrom_tag_t &tofrom_tag_t::operator=(const tofrom_tag_t &other)
{
	this->uri = other.uri;
	display = other.display;
	this->passwd = other.passwd;
	this->tag = other.tag;
	this->type = other.type;
	return *this;
}

int	tofrom_tag_t::operator==(const tofrom_tag_t &other)
{
	if( this->uri == other.uri)
	{
		if( this->display == other.display)
		{
			if( this->tag == other.tag)
			{
				if( this->type == other.type)
					return 0;
			}
		}
	}
	return 0;
}

int	tofrom_tag_t::operator==(const char *szIp)
{
	if( !this->uri.haddr.hostAddr.compare(szIp) )
		return 1;
	if( !this->uri.uid.compare(szIp))
		return 1;
	return 0;
}

PBYTE tofrom_tag_t::parse(IN PBYTE inStr)
{
    C c;
    PBYTE tmp0,tmp1,start,p;
	char		szDisplay[64];
    typedef And<CHAR_T,CHAR_A,CHAR_G,Char<'='> > tag_Pre;
	typedef Or<AllChar, Or<Char<'-'>,Char<'+'>,Char<'.'>,Char<'_'>>, AllNum > tag_char;
    typedef And<tag_Pre, PlusRepeat<tag_char>> ltag;
    typedef FindRepeat<ltag> findTag;
    tmp0 = inStr;
    
    if( tmp0 <= NULL )
        return NULL;
    while(*tmp0 == ' ')tmp0++;
    tmp1 = tmp0;
	memset( szDisplay, 0, 64);
    p = parse_displayname(tmp1, (PBYTE)&szDisplay, 64);
	this->display = szDisplay;
	tmp1 = (p?p:(tmp0?tmp0:inStr));
	tmp0 = this->uri.parse(tmp1);
	//if( *(tmp1-1) == '<' || *tmp1 == '<')
	//	this->brace = 1;
    tmp1 = tmp0;
    if( findTag::Matches(tmp1,&c,start))
    {
		this->tag.append((char*)(start+4),(int)(tmp1-start-4));
    }
    return tmp1;
}

int  tofrom_tag_t::print(OUT PBYTE outStr)
{
	int bSipUri=0;
	if( this->type == to_tag )
		sprintf((char*)outStr,"%s", "To: ");
	else if(this->type == from_tag)
		sprintf((char*)outStr,"%s", "From: ");
	else
		return 0;
	if( !display.empty() )
	{
		bSipUri = 1;
		strcatfm((char*)outStr,"%s ",this->display.data());
	}
	{
		bSipUri = 1;
		char host[256];
		memset(host, 0, 256);
		this->uri.print((PBYTE)&host );
		if( strlen(host) >  0 )
		{
			//sprintf(tbuf,"<%s>", host);
			//if( this->brace )
				strcatfm((char*)outStr, "<%s>",host);
			//else
			//	strcat((char*)outStr, host);
		}
	}
	if( !tag.empty())
	{
		if( bSipUri )
			strcat((char*)outStr,";");
		strcat((char*)outStr,"tag=");
		strcat((char*)outStr,this->tag.data());
	}
	strcat((char*)outStr,"\r\n");
	return 1;
}

int tofrom_tag_t::build(char *_uid,char *ip,int _port,char *_display,char *_tag,int dir, int _brace)
{
	if( !this->uri.build(ip, _port, _uid) )
		return 0;
	this->display = (_display?_display:"");
	this->tag = (_tag?_tag:"");
	this->type = (to_from_enum_t)dir;
	//this->b
	//this->brace = _brace;
	return 1;
}


/////////////////////
