#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <iostream>
#include <string>

using namespace std;
#include "host_addr.h"
#include "peg_def.h"
#include "peg_xsip.h"
#include "skytool.h"
#include "sip_tool.h"
#include "SipFieldBase.h"

#include "sip_method.h"
#include "sip_uri.h"
#include "sip_info.h"

#include "sip_pre_line.h"

using namespace peg_templ;

PBYTE parse_sippreline(IN PBYTE inStr,OUT PVOID _pPre)
{
    PSIPPRELINE pPre = (PSIPPRELINE)_pPre;
    C c;
    PBYTE tStr;
    if( inStr<=NULL || _pPre<=NULL )
        return inStr;
    
    tStr = inStr;
    tStr=parse_msg_type(tStr,pPre->msg_type);
    if( pPre->msg_type == msg_invalid )
    {
        PBYTE start;
        typedef And<Char<' '>, AllNum,AllNum,AllNum,Char<' '> > state;
        typedef Or<AllChar,Char<' '>,Char<'-'>,Char<'.'> > str;
        typedef And<AllChar,PlusRepeat<str>,cnrl > text;
        typedef FindRepeat<state > fState;
        typedef FindRepeat<text > fText;
        tStr = inStr;
		pPre->sipInf.parse(tStr);
        if( fState::Matches(tStr,&c,start))
        {
            pPre->state = atoi((char*)start);
			pPre->hdtype = 2;
        }
        else
        {
            return inStr;
        }
        if( fText::Matches(tStr,&c,start))
        {
            int len;
            if( *(tStr-2)=='\r' )
                len = tStr-start -2;
            else
                len = tStr-start -1;
			pPre->text="";
			pPre->text.append((char*)start, len);
            //pPre->text.CopyStr(start, len);
            //return tStr;
        }
		return tStr;
    }
    else
    {
        PBYTE start;
		typedef And<Char<';'>,CHAR_L,CHAR_R> s_lr;
		typedef FindRepeat<s_lr > fSlr;
		typedef And<CHAR_P,CHAR_S,CHAR_P,Char<'='> > s_psp_prefix;
		typedef PlusRepeat<AllChar > s_psp_mid;
		typedef And<s_psp_prefix,s_psp_mid,Or<Char<' '>, Char<';'>, Char<','> > > s_psp;
		typedef FindRepeat<s_psp > fPsp;
        pPre->hdtype = 1;
		pPre->sipUri.parse(tStr);
        //tStr=parse_sip_uri(tStr,&pPre->sipUri);
        tStr=parse_sipinfo(tStr,&pPre->sipInf);
		tStr = inStr;
		if( fSlr::Matches(tStr,&c,start))
			pPre->lr = 1;
		tStr = inStr;
		if( fPsp::Matches(tStr,&c,start))
		{
			//pPre->psp.CopyStr(start+4, (tStr-start-5) );
			pPre->psp.append((char*)(start+4), (tStr-start-5));
		}
        return tStr;
    }
    return inStr;
}

VOID print_sippreline(IN PVOID _pPre,OUT PBYTE outStr)
{
	if( !_pPre && !outStr)
	{
		((sip_pre_line*)_pPre)->print(outStr);
	}
}
//
//	hdtype;    //0=invalid, 1=request, 2=response
//	Status-Line: SIP/2.0 202 Accepted
//	Request-Line: INVITE sip:+8581001000@210.121.190.121 SIP/2.0
int sip_pre_line::req_build(BYTE highVer,BYTE lowVer, sip_method_type_t _msg_type,
	      char *_uid, char *ip,int port,protocol_type_e protoType)
{
	this->sipInf.build(protoType, highVer,lowVer);
	this->sipUri.build(ip,port,_uid);
	this->hdtype = 1;
	this->msg_type = _msg_type;
	return 1;
}

int	sip_pre_line::res_build(BYTE highVer,BYTE lowVer,
		int _state, char *_text , protocol_type_e protoType)
{
	this->hdtype = 2;
	this->sipInf.build(protoType,highVer,lowVer);
	this->state = _state;
	this->text = (_text?_text:"");
	return 1;
}

PBYTE sip_pre_line::parse(IN PBYTE inStr)
{
    return parse_sippreline(inStr,this);
}

int sip_pre_line::print(OUT PBYTE outStr)
{
	char sUri[128];
	char sInfo[128];
	memset(sUri, 0, 128);
	memset(sInfo, 0, 128);
	print_sipinfo(&sipInf,(PBYTE)&sInfo);
	if( hdtype == 1 )
	{
		sipUri.print((PBYTE)&sUri);
		//strcat((char*)outStr, sInfo
		sprintf((char*)outStr, "%s%s%s%s%s%s %s\r\n",
							(char*)msgtypetostr(msg_type),
							(strlen(sUri)>0?" ":""),
							(strlen(sUri)>0?sUri:""),
							(lr==1?";lr":""), 
							(!psp.empty()?";psp=":""),
							(!psp.empty()?psp.data():""),
							sInfo);
		return 1;
	}
	else if( hdtype == 2)
	{
		sprintf((char*)outStr, "%s %d%s%s\r\n",
			sInfo,state,
			(!text.empty()?" ":""),
			(!text.empty()?text.data():""));
		return 1;
	}

	return 0;
}

sip_pre_line::sip_pre_line()
{
	this->msg_type=msg_invalid;
	this->hdtype=0;
	this->state=-1;
	this->lr = -1;
	this->psp="";
	this->text="";
	this->sipUri.free();
}
sip_pre_line::~sip_pre_line()
{
	this->free();
}
sip_pre_line& sip_pre_line::operator=(const sip_pre_line& tmp)
{
	this->lr = tmp.lr;
	this->sipUri = tmp.sipUri;
	this->sipInf = tmp.sipInf;
	this->msg_type = tmp.msg_type;
	this->hdtype =tmp.hdtype;
	this->state = tmp.state;
	this->text = tmp.text;
	return *this;
}
void sip_pre_line::free()
{
	this->sipUri.free();
	text = "";
	this->lr = -1;
	this->psp="";
}

//////////////////////////////////////////////////
// end of file
 
