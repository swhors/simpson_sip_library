#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <stdio.h>
#include "debug.h"
#ifdef WIN32
#include <memory.h>
#include <string.h>
#endif
#include <iostream>
#include <string>

using namespace std;

#include "host_addr.h"

#include "peg_def.h"
#include "peg_xsip.h"


#include "skytool.h"
#include "SIpFieldBase.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"

#include "sip_uri.h"

using namespace peg_templ;

#define SIP_URI_PREFIX    "sip:"
#define SIP_URI_USER    "user="
#define SIP_URI_TAG        "tag="

//void make_content_type(OUT PUCHAR msg, IN PSTRING sbuf);
//int make_user_number(string *user_number,PUCHAR buff);


int free_uri(p_uri_addr_t uri)
{
	uri->free();  
    return 1;
}

void init_uri(PVOID p_buf)
{
    p_uri_addr_t uri=(p_uri_addr_t)p_buf;
	uri->free();    
}

int make_uri(IN PVOID _phCall, OUT PUCHAR buff)
{
	if( _phCall )
	{
		uri_addr_t * phCall = (uri_addr_t*)_phCall;
		phCall->print(buff);
	}
	return 0;
}
#if 0
void make_content_type(OUT PUCHAR msg, IN PSTRING sbuf)
{
    make_str_field(msg, "Content-Type", sbuf);
}

int make_user_number(string *user_number,PUCHAR buff)
{
	if(!user_number->empty())
		strcatfm((char*)buff,"<sip:%s@",user_number->data());
    else
        strcat((char*)buff,"<sip:");
    return 1;
}
#endif

int build_uri(OUT PVOID _pUri,IN char *hostip,IN int port,char *u_number)
{
    p_uri_addr_t pUri = (p_uri_addr_t)_pUri;
	if( _pUri)
	{
		pUri->build(hostip, port, u_number);
		return 1;
	}
	return 0;
	//if( port < 0)
    //    build_ip_hostaddr(&pUri->haddr, hostip, 0);
	//else
    //    build_ip_hostaddr(&pUri->haddr, hostip, port);
//  //  copy_str(&pUri->user_number, u_number );
    return 1;
}

uri_addr_t::uri_addr_t()
{
	this->transport = sip_none_proto;
}

uri_addr_t::~uri_addr_t()
{
	free();
}

void uri_addr_t::free()
{
	this->haddr.init();
	if( maddr.size() > 0 )
		maddr.erase(maddr.begin(), maddr.end() );
	if( passwd.size() > 0 )
		passwd.erase(passwd.begin(), passwd.end());
	if( this->tag.size() > 0 )
		tag.erase(tag.begin(),tag.end());
	if( this->uid.size() > 0 )
		uid.erase(uid.begin(), uid.end());
	if( this->user.size() > 0 )
		user.erase(user.begin(), user.end());
	//this->passwd
}

uri_addr_t &uri_addr_t::operator =(const uri_addr_t &other)
{
	this->haddr = other.haddr;
	this->passwd = other.passwd;
	this->tag = other.tag;
	this->u_num_with_sharp = other.u_num_with_sharp;
	this->user = other.user;
	this->uid = other.uid;
	//this->user_number = other.user_number;
	return *this;
}

int uri_addr_t::print(unsigned char *outStr)
{
	if( !this->uid.empty())
		sprintf((char*)outStr,"%s%s%s","sip:",
				(this->uid.empty()?"":uid.data()),
				(this->uid.empty()?"":"@"));
	else
		sprintf((char*)outStr,"%s","sip:");
	
	if( this->haddr.port )
		strcatfm((char*)outStr,"%s:%d",(this->haddr.hostAddr.empty()?"127.0.0.1":(char*)this->haddr.hostAddr.data()),this->haddr.port);
	else
		strcatfm((char*)outStr,"%s",(this->haddr.hostAddr.empty()?"127.0.0.1":(char*)this->haddr.hostAddr.data()));
#if 0
	if( !uid.empty() && !passwd.empty() )
	{
		if( strlen(tbuf)>0 )
			strcatfm((char*)outStr,"%s:%s@%s",uid.data(),passwd.data(),tbuf);
		else
			strcatfm((char*)outStr,"%s:%s",uid.data(),passwd.data());
	}
	else if( !uid.empty()&& !passwd.empty())
	{
		if( strlen(tbuf)>0 )
			strcatfm((char*)outStr,"%s@%s",uid.data(),tbuf);
		else
			strcatfm((char*)outStr,"%s",uid.data());
	}
	else
	{
		if( strlen(tbuf)>0 )
			strcatfm((char*)outStr,"%s",tbuf);
	}
#endif
	if( transport != sip_none_proto )
		strcatfm((char*)outStr,";transport=%s",
		(transport==sip_tcp?"TCP":"UDP"));
	if( !user.empty())
		strcatfm((char*)outStr,";user=%s",user.data());
	if( ttl >= 0 )
		strcatfm((char*)outStr,";ttl=%d",ttl);
	if( !maddr.empty())
		strcatfm((char*)outStr,";maddr=%s", maddr.data());
	return 1;
}

unsigned char* uri_addr_t::parse(unsigned char* inStr)
{
	C c;
	PBYTE tmp,start,tmp0;
	typedef Char<'='> CHAR_0;
	typedef Char<'\"'> CHAR_1;
	typedef Char<';'> CHAR_2;
	typedef And<CHAR_S,CHAR_I,CHAR_P,Char<':'> > sipPrefix;
	typedef And<Or<SpeChar,AllChar>,
		PlusRepeat<AllChar>,Char<'@'> > uid_str;
	typedef And<Char<':'>,PlusRepeat<AllChar>,Char<'@'> >passwd_str;
	typedef And<CHAR_2,CHAR_T,CHAR_R,CHAR_A,
		CHAR_N,CHAR_S,CHAR_P,CHAR_O,
		CHAR_R,CHAR_T,CHAR_0,
		PlusRepeat<AllChar> > transport_str;
	typedef And<CHAR_2,CHAR_U,CHAR_S,CHAR_E,CHAR_R,
		CHAR_0,PlusRepeat<AllChar> > user_str;
	typedef And<CHAR_2,CHAR_M,CHAR_E,CHAR_T,CHAR_H,
		CHAR_O,CHAR_D,CHAR_0,PlusRepeat<AllChar> > method_str;
	typedef And<CHAR_2,CHAR_T,CHAR_T,CHAR_L,CHAR_0,
		PlusRepeat<AllNum> > ttl_str;
	typedef And<CHAR_2,CHAR_M,CHAR_A,CHAR_D,CHAR_D,
		CHAR_R,CHAR_0,hostaddr > maddr_str;
	//typedef And<CHAR_2,CHAR_T,CHAR_A,CHAR_G,CHAR_0,
	//	PlusRepeat<AllChar > > tag_str;
	typedef FindRepeat<sipPrefix > fPrefix;
	typedef FindRepeat<uid_str > fUid;
	typedef FindRepeat<passwd_str > fPasswd;
	typedef FindRepeat<transport_str > fTransport;
	typedef FindRepeat<user_str > fUser;
	typedef FindRepeat<ttl_str > fTtl;
	typedef FindRepeat<maddr_str > fMaddr;
	//typedef FindRepeat<tag_str > fTag;

	if( !inStr )
		return inStr;

	tmp = inStr;
	if(!fPrefix::Matches(tmp, &c))
		return inStr;
	if( fUid::Matches(tmp, &c,start) )
	{ 
		uid.append((char*)start, (int)(tmp-start-1));
	}
	
	if( parse_siphost(tmp,(void*)&this->haddr) )
	{

		tmp0 = tmp;

		if( fTransport::Matches(tmp,&c,start))
		{
			PBYTE tt=start+11;;
			typedef And<CHAR_T,CHAR_C,CHAR_P > tcpl;
			typedef And<CHAR_U,CHAR_D,CHAR_P > udpl;
			if( tcpl::Matches(tt, &c))
				transport = sip_tcp;
			else if( udpl::Matches(tt, &c))
				transport = sip_udp;
			else
				transport = sip_none_proto;
		}
		tmp0 = tmp;
		if( fPasswd::Matches(tmp,&c,start) )
		{
			passwd.append((char*)(start+1),(int)(tmp-start-2));
		}
		tmp = tmp0;
		if( fUser::Matches(tmp,&c,start) )
		{
			user.append((char*)(start+6),(tmp-start-6));
		}
		tmp = tmp0;
		if( fTtl::Matches(tmp,&c,start))
		{
			ttl = atoi((char*)(start+5));
		}
		tmp = tmp0;
		if( fMaddr::Matches(tmp,&c,start))
			maddr.append((char*)(start+7),(int)(tmp-start-7));
		return tmp;
	}
	return NULL;
}

int uri_addr_t::build(char *_ip,int _port,char *_uid,char *_user, char *_password)
{
	if( _ip )
	{
		this->haddr.type = (( *_ip >='0' && *_ip <= '9')?ip:h_name);
		this->haddr.hostAddr = _ip;
		this->haddr.port = _port;

		this->uid = (_uid?_uid:"");
		this->user = (_user?_user:"");
		this->passwd = (_password?_password:"");
		return 1;
	}
	return 0;
}

int	uri_addr_t::operator==(const uri_addr_t &other)
{
	if( this->uid == other.uid)
	{
		if( this->haddr == other.haddr)
		{
			return 1;
		}
	}
	return 0;
}

///////////////
