#include <stdio.h>

#if defined WIN32 || defined WINCE
#include "memory.h"
#include "string.h"
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <winbase.h>
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

#include "sip_callid.h"

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;

void init_call_id_buf(PVOID p_buf)
{
    p_call_id_t pcallid = (p_call_id_t)p_buf;
    if( pcallid)
    {
		pcallid->free();
    }
}

// 두 개의 콜 id를 비교 한다.
// 일치 하면 0을, 다르면 1을 반환한다.
// create 2005/03/10 pm3:03 swhors@skycom.ne.kr
// PVOID p_src, PVOID p_dest : 비교 대상의 콜 아이디
int callidcmp(PVOID p_src, PVOID p_dest)
{
    p_call_id_t pdest, psrc;
    pdest = (p_call_id_t) p_dest;
    psrc = (p_call_id_t)p_src;
	if( p_src && p_dest)
	{
		if( *psrc == *pdest)
			return 1;
	}
    return 0;
}

// 콜 아이디 정보를 다른 버퍼로 복사한다.
// create 2005/03/10 pm 3:04 swhors@skycom.ne.kr
// PVOID p_dest : 목적지 버퍼
// PVOID p_src  : 원본 버퍼
int callidcpy(PVOID p_dest, PVOID p_src)
{
    p_call_id_t pdest, psrc;
    pdest = (p_call_id_t) p_dest;
    psrc = (p_call_id_t)p_src;
    if( pdest && psrc )
    {
        *pdest = *psrc;
    }
    return 0;
}

int sip_parse_call_id(PUCHAR dump, p_call_id_t cid, int len)
{
    if( cid )
	{
		return (cid->parse(dump)?1:0);
	}
    return 0;
}


void free_callid(p_call_id_t hd)
{
    if( hd)
	{
		hd->free();
	}
}

static int make_callid_internal(PVOID _phd, PUCHAR buff)
{
    p_call_id_t phd = (p_call_id_t)_phd;
    if( phd )
    {
		return phd->print(buff);
    }
    return 0;
}

int make_callid(PVOID _phd, PUCHAR buff)
{
    return make_callid_internal(_phd, buff);
}

int build_callid(OUT PVOID _pCid,IN char *callid,IN char *hostip)
{
    p_call_id_t pCid = (p_call_id_t)_pCid;
    if( pCid )
    {
		pCid->build(hostip, 0, callid);
		return 1;
    }
    return 0;
}

int generate_callid(OUT p_call_id_t hd, OUT char *callid,IN int len, char *hostip)
{
    time_t t;
	char tbuf[128];
    unsigned long iip;
#if defined WIN32 || defined WINCE
    int randVal;
	randVal = GetRandomDouble(GetTickCount());
	while(randVal < 1000 )
		randVal = GetRandomDouble(GetTickCount());
    memset(callid, 0, len);
    iip = convert_sip_iip(hostip);
	sprintf_s(tbuf, 128, "%d%s",randVal, callid);
	hd->callid = tbuf;
	return hd->callid.size();
#else
	int randVal;
    srand(t);
    time(&t);
    randVal = rand();
    memset(callid, 0, len);
    iip = convert_sip_iip(hostip);
	sprintf_s(tbuf, 128, "%d%s",randVal, callid);
	hd->callid = tbuf;
	return hd->callid.size();
#endif
}

call_id_t::call_id_t()
{
	//memset(hostaddr, 0, 24);
	this->hostaddr ="";
	this->callid = "";
}

call_id_t::~call_id_t()
{
	//memset(hostaddr, 0, 24);
	this->free();
}


int	call_id_t::build(char *szIp,int port,char *_callid)
{
	this->callid = (_callid?_callid:"");
	this->hostaddr=(szIp?szIp:"");
	return 1;
}

PBYTE call_id_t::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    C c;
    typedef Or<AllChar,Char<'-'>,AllNum,Char<'.'> > cid_chr;
    typedef And<PlusRepeat<cid_chr>, Or<sipNewItem,Char<0>,Char<'@'> > > cid_str;
    typedef FindRepeat<cid_str> cidFind;
	typedef FindRepeat<IPv4address> strHost;


    tmp = inStr;

    if( cidFind::Matches(tmp, &c, start ))
    {
		this->callid = "";
		callid.append((char*)start, (int)(tmp-start-1));
	    tmp = inStr;
	
		if( strHost::Matches(tmp, &c, start ))
		{
			this->hostaddr = "";
			hostaddr.append((char*)start, (int)(tmp-start-1));
		}
		return tmp;
	}
    return NULL;
}

int  call_id_t::print(OUT PBYTE outStr)
{
    if(outStr<=NULL)
        return 0;
	sprintf((char*)outStr,"Call-ID: %s%s%s\r\n", callid.data(), (!hostaddr.empty()?"@":""), (!hostaddr.empty()?hostaddr.data():""));
    return 1;
}

int call_id_t::operator==(const call_id_t &cid)
{
	if( this->callid == cid.callid )
	{
		if( this->hostaddr == cid.hostaddr)
			return 1;
	}
	return 0;
}

int call_id_t::operator >(const call_id_t &cid)
{
	if( this->callid > cid.callid )
	{
		if( this->callid >= cid.callid )
			return 1;
	}
	return 0;
}

int call_id_t::operator <(const call_id_t &cid)
{
	if( this->callid < cid.callid )
	{
		if( this->hostaddr <= cid.hostaddr )
			return 1;
	}
	return 0;
}

call_id_t &call_id_t::operator=(const call_id_t & cid)
{
	this->callid = cid.callid;
	this->hostaddr = cid.hostaddr;
	return *this;
}

VOID call_id_t::free()
{
	this->callid="";
	this->hostaddr ="";
}

char *call_id_t::ToString(char *outStr, int len)
{
	sprintf_s(outStr, len, "%s%s%s", this->callid.data(),
						(!this->hostaddr.empty()?"@":""),
						(!this->hostaddr.empty()?(char*)hostaddr.data():""));
	return outStr;
}

////.
