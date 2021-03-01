////////////////////////////////////////////////////////////
//
// File : sip_cseq.c
// 설명 : sip의 필드의 c-sequence.
// 날짜 : (1) 2004/11/xx swhors@skycom.ne.kr - 설명
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////

#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

#include <iostream>
#include <string>

#include "peg_def.h"
#include "peg_xsip.h"

using namespace std;
#include "host_addr.h"
#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"
#include "SIpFieldBase.h"

#include "sip_method.h"

#include "sip_cseq.h"

using namespace peg_templ;

void LogWrite(char *buf);

////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////

void init_cseq_buf(PVOID p_buf)
{
    p_cseq_hd_t pcseq = (p_cseq_hd_t)p_buf;
    if( pcseq )
    {
        pcseq->type = msg_invalid;
        pcseq->cseq = 0;
    }
}

int sip_parse_cseq(PUCHAR dump, p_cseq_hd_t hd, int len)
{
	if( hd )
	{
		hd->parse(dump);
	    return 1;
	}
	return 0;
}

int make_cseq(PVOID p_hd, PUCHAR buff)
{
    p_cseq_hd_t phd = (p_cseq_hd_t)p_hd;
    if( phd)
    {
		phd->print(buff);
        //strcatfm((char*)(char*)buff, "CSeq: %d %s\r\n", phd->cseq, get_sip_method_name(phd->type));
        return 1;
    }
    return 0;
}

int build_cseq(OUT PVOID _pCseq,IN int cseq,IN sip_method_type_t method)
{
	if( _pCseq )
	{
		p_cseq_hd_t pCseq = (p_cseq_hd_t)_pCseq;
		pCseq->cseq =cseq;
		pCseq->type = method;
		return 1;
	}
	return 0;
}

cseq_hd_t::cseq_hd_t()
{
	cseq = 0;
}

cseq_hd_t::~cseq_hd_t()
{
	cseq = 0;
}

unsigned char* cseq_hd_t::parse(unsigned char * inStr)
{
    PBYTE tmp;
    PBYTE start;
    C c;
    typedef And<Char<' '>,PlusRepeat<ALPHA> > s;
    typedef FindRepeat<s> fType;
    if( inStr <= NULL )
        return inStr;
    
    tmp = inStr;
    
    while(*tmp==' ') tmp++;
    
	this->cseq = atoi((char*)tmp);
    
    if( fType::Matches(tmp, &c, start))
    {
		if( *start == ' ' )
			start ++;
        parse_msg_type(start, this->type);
    }
    return tmp;
}
int cseq_hd_t::print(unsigned char * outStr)
{
   if( outStr <= NULL )
        return 0;
    else
    {
		if( this->cseq >= 0 )
		{
			strcatfm((char*)outStr,"CSeq: %d %s\r\n",
                 this->cseq,get_msg_type_string(this->type));
		}
    }
   return 1;
}
cseq_hd_t &cseq_hd_t::operator=(const cseq_hd_t &other)
{
	type = other.type;
    cseq = other.cseq;
	return *this;
}
int	cseq_hd_t::operator==(const cseq_hd_t &other)
{
	if( type == other.type )
	{
		if( cseq == other.cseq)
			return 1;
	}
	return 0;
}

//
////////////////////////////////////////////////////////////
//
