#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include <memory.h>

#include <iostream>
#include <string>

using namespace std;
#include "host_addr.h"
#include "peg_def.h"
#include "peg_xsip.h"
#include "skytool.h"
#include "sip_tool.h"
#include "sip_info.h"

using namespace peg_templ;

PBYTE parse_sipinfo(IN PBYTE inStr,OUT PVOID _pBuff)
{
	if( _pBuff ) return ((sip_info*)_pBuff)->parse(inStr);
    return inStr;
}

VOID print_sipinfo(IN PVOID _pBuff,OUT PBYTE _pOutBuf)
{
	if( _pBuff ) ((sip_info*)_pBuff)->print(_pOutBuf);
}

int sip_info::build(protocol_type_e proto,BYTE highVer,BYTE lowVer)
{
	this->proto_type = proto;
	this->v.l = lowVer;
	this->v.u = highVer;
	return 1;
}

PBYTE sip_info::parse(IN PBYTE inStr)
{
    PBYTE oStr,tStr,start;
    C c;
    typedef And<CHAR_S,CHAR_I,CHAR_P,CHAR_SL > SIPI_Prefix;
    typedef And<In<'0','9'>,Char<'.'>,In<'0','9'> > SIPI_Ver;
    typedef And<CHAR_T,CHAR_C,CHAR_P > tcp;
    typedef And<CHAR_U,CHAR_D,CHAR_P > udp;
    typedef Or< tcp, udp > transport;
    typedef And<SIPI_Prefix,
                Or<SIPI_Ver,
                   And<SIPI_Ver,CHAR_SL,transport >,
                   transport > > sipInfo;
    typedef FindRepeat<sipInfo> fSipinfo;
    oStr = inStr;
    proto_type = sip_none_proto;
    if( fSipinfo::Matches(oStr,&c,start) )
    {
        oStr = start;
        if( SIPI_Prefix::Matches(oStr,&c))
        {
            tStr = oStr;
            if( SIPI_Ver::Matches(oStr,&c))
            {
                v.u=*tStr-48;
                tStr=tStr+2;
                v.l=*tStr-48;
                if( CHAR_SL::Matches(oStr,&c))
                {
                    // none
                    ;
                }
            }
            if( tcp::Matches(oStr,&c))
            {
                proto_type = sip_tcp;
            }
            else if( udp::Matches(oStr,&c))
            {
                proto_type = sip_udp;
            }
			return oStr;
            //return get_cnrl(oStr);
        }
    }
	return inStr;
    //return parse_sipinfo(inStr,_pBuff);
}
VOID  sip_info::print(OUT PBYTE _pOutBuf)
{
    if( _pOutBuf != NULL )
    {
        if( proto_type != sip_none_proto )
        {
            if( v.u == 0 && v.l == 0)
                sprintf((char*)_pOutBuf, "SIP/%s",
                    (proto_type==sip_udp?"UDP":"TCP"));
            else
                sprintf((char*)_pOutBuf, "SIP/%d.%d/%s",
                    v.u, v.l,
                    (proto_type==sip_udp?"UDP":"TCP"));
        }
        else
        {
            if( v.u == 0 && v.l == 0)
                sprintf((char*)_pOutBuf,"SIP");
            else
                sprintf((char*)_pOutBuf,"SIP/%d.%d",v.u, v.l);
        }
    }
}

sip_info & sip_info::operator=(const sip_info& other)
{
	this->v.u = other.v.u;
	this->v.l = other.v.l;
	this->proto_type = other.proto_type;
	return *this;
}

int sip_info::operator==(const sip_info& other)
{
	if( this->v.u == other.v.u )
	{
		if( this->v.l == other.v.l)
		{
			if( this->proto_type == other.proto_type )
				return 1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////
// end of file
//

