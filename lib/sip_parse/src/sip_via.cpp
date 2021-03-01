#include <stdio.h>
#ifdef LINUX
#include <strings.h>
#endif
#include <iostream>
#include <string>
using namespace std;


#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include "host_addr.h"
#include "skytool.h"
#include "SIpFieldBase.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"

#include "sip_answer_code.h"
#include "sip_method.h"
#include "sip_uri.h"

#include "sip_via.h"

extern int err;


int parse_via_intenal(PBYTE inStr,PVOID _pVia)
{
    PSIPVIA pVia=(PSIPVIA)_pVia;
	if( pVia )
		return (pVia->parse(inStr)?1:0);
	return 0;
}

VOID print_via(IN PVOID _pBuff,OUT PBYTE outBuf)
{
    PSIPVIA pVia = (PSIPVIA)_pBuff;
    if( pVia <= NULL )
        return;
	pVia->print(outBuf);
}

sip_via_t::sip_via_t()
{
	ttl=-1;
	hidden=0;
}

sip_via_t::~sip_via_t()
{
	this->free();
}

int sip_via_t::build(SIPINF &inf,hostaddr_t &host,char *sBranch,char *sReceived,char *sMaddr)
{
	this->sipInf = inf;
	this->host = host;
	if( sBranch )
		this->branch = sBranch;
	if( sReceived )
		this->received = sReceived;
	if( sMaddr )
		this->maddr = sMaddr;
	return 1;
}

int sip_via_t::build(protocol_type_e protoType,
					  BYTE highVer,
					  BYTE lowVer,
					  char *szAddress,
					  int port,
					  char *sBranch,
					  char *sReceived,
					  char *sMaddr)
{
	SIPINF sipInf;
	hostaddr_t cmHost;
	sipInf.build(protoType,highVer,lowVer);
	cmHost.type = ip;
	cmHost.hostAddr = szAddress;
	cmHost.port = port;
	return build(sipInf,cmHost,sBranch,sReceived,sMaddr);
}

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;


VOID sip_via_t::free()
{
	//host.free();
	if( branch.size() > 0 )
		branch.erase(branch.begin(), branch.end());
	if( received.size() > 0 )
		received.erase(received.begin(), received.end());
	if( maddr.size() > 0 )
		maddr.erase(maddr.begin(), maddr.end());
	ttl = -1;
	rport = -1;
	hidden = 0;
}


PBYTE sip_via_t::parse(IN PBYTE inStr)
{
    C c;
    PBYTE tmp0,tmp1,start;
    typedef And<CHAR_H,CHAR_I,CHAR_D,CHAR_D, CHAR_E,CHAR_N > hidden;
	typedef Or<unreserved , Char< '.' > , Char<'+'> > branchStr;
    typedef And<CHAR_B,CHAR_R,CHAR_A,CHAR_N, CHAR_C,CHAR_H,Char<'='>, PlusRepeat< branchStr> > branch;
    typedef And<CHAR_R,CHAR_E,CHAR_C,CHAR_E, CHAR_I,CHAR_V,CHAR_E,CHAR_D, Char<'='>,hostaddr > received;
	typedef And<CHAR_R,CHAR_P,CHAR_O,CHAR_R,CHAR_T > p_rport;
    typedef And<CHAR_M,CHAR_A,CHAR_D,CHAR_D, CHAR_R,Char<'='>, hostaddr > maddr;
    typedef And<CHAR_T,CHAR_T,CHAR_L, Char<'='>,PlusRepeat<AllNum> > ttl;
    typedef FindRepeat<hidden > is_hidden;
    typedef FindRepeat<branch > is_branch;
    typedef FindRepeat<received > is_recv;
    typedef FindRepeat<ttl > is_ttl;
    typedef FindRepeat<maddr > is_maddr;
    typedef FindRepeat<hostaddr > is_sentby;
	typedef FindRepeat<p_rport > is_rport;
    tmp0 = inStr;
    while(*tmp0==' ')tmp0++;
    tmp1 = parse_sipinfo(tmp0, &sipInf);
    if( tmp1 ) tmp0 = tmp1;
    
    if( is_sentby::Matches(tmp0, &c,start))
    {
        parse_siphost(start,&host);
    }
    tmp1 = tmp0;
	
    // hidden
    if( is_hidden::Matches(tmp1, &c, start))
    {
        this->hidden = 1;
    }
    tmp1 = tmp0;

    // ttl
    if( is_ttl::Matches(tmp1, &c, start))
    {
        this->ttl = atoi((char*)(start+4));
    }
    tmp1 = tmp0;
 
    // maddr
    if( is_maddr::Matches(tmp1, &c, start ))
    {
		this->maddr.assign((char*)(start+6),(int)(tmp1-start-6));
		//this->maddr.insert((char*)(start+6),(int)(tmp1-start-6));
		//sprintf(this->maddr, "%s", 
        //this->maddr.CopyStr((char*)(start+6),(int)(tmp1-start-6));
    }
    tmp1 = tmp0;
    
    // received
    if( is_recv::Matches(tmp1, &c, start))
    {
		this->received.assign((char*)(start+9),(int)(tmp1-start-9));
        //this->received.CopyStr((char*)(start+9),(int)(tmp1-start-9));
    }
    tmp1 = tmp0;

	if( is_rport::Matches(tmp1, &c, start ))
	{
		if( *tmp1 == ';' || *tmp1 == ' ' || *tmp1 == '\r' || *tmp1 == 'n')
			rport = 0;
		else
			rport = atoi((char*)tmp1);

	}
    tmp1 = tmp0;    
    // branch
    if( is_branch::Matches(tmp1, &c,start))
    {
		this->branch.assign((char*)(start+7),(int)(tmp1-start-7));
        //this->branch.CopyStr((char*)(start+7),(int)(tmp1-start-7));
    }
    
    return tmp1;
}

int sip_via_t::print(OUT PBYTE outStr)
{
    if( outStr <= NULL )
    {
        return 0;
    }
    else
    {
        char addr[256];
        char info[64];
        int  bOkHost=0;
        memset(addr, 0, 256);
        memset(info, 0, 64);
        print_sipinfo(&sipInf,(PBYTE)&info);
        sprintf((char*)outStr, "Via: %s", info);
		if( host.type == ip )
        {
			if( host.port > 0 )
				sprintf_s(addr,256, " %s:%d",(host.hostAddr.empty()?"127.0.0.1":(char*)host.hostAddr.data()),host.port);
			else
				sprintf_s(addr,256, " %s",(host.hostAddr.empty()?"127.0.0.1":(char*)host.hostAddr.data()));
            strcat((char*)outStr,addr);
            bOkHost = 1;
        }
        // hidden
        if( hidden )
            strcat((char*)outStr,";hidden");
        // ttl 
        if( ttl >= 0 )
            strcatfm((char*)outStr,";ttl=%d",ttl);
        // maddr
		if( !maddr.empty() )
			strcatfm((char*)outStr,";maddr=%s",maddr.data());
        // received
		if( !received.empty())
			strcatfm((char*)outStr,";received=%s",received.data());
		if( rport == 0 )
            strcatfm((char*)outStr,";rport");
		else if( rport > 0 )
            strcatfm((char*)outStr,";rport=",rport);
        //branch
		if( !branch.empty())
			strcatfm((char*)outStr,";branch=%s",branch.data());
        strcat((char*)outStr,"\r\n");
    }
	return 1;
}

CSipFieldBase &sip_via_t::operator =(const CSipFieldBase &cFieldBase)
{
	sip_via_t *pSipVia = (sip_via_t *)&cFieldBase;
	*this = *pSipVia;
	return *this;
}

sip_via_t &sip_via_t::operator =(const sip_via_t &v)
{
    sipInf = v.sipInf;
    host = v.host;
    branch = v.branch;
    received = v.received;
    maddr = v.maddr;
    ttl = v.ttl;
    hidden = v.hidden;
	rport = v.rport;
	return *this;
}


////////////
