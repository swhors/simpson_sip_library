////////////////////////////////////////////////////////////
//
// File : sip_method.c
// 설명 : sip의 메소드에 따른 데이터 파싱의 메인 부분.
// 날짜 : 2004/11/xx swhors@skycom.ne.kr
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include <stdio.h>
#ifdef WIN32
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#endif

#include <iostream>
#include <vector>
#include <string>

using namespace std;
#include "host_addr.h"
#include "skytool.h"

#include "sip_tool.h"

#include "peg_def.h"
#include "peg_xsip.h"


#include "sip_method.h"

using namespace peg_templ;


////////////////////////////////////////////////////////////
// Local Variables
////////////////////////////////////////////////////////////

PBYTE parse_msg_type(PBYTE strMsg, ESIPMSGTYPE& type)
{
    PBYTE orMsg;
    C c;
    typedef Or<Char<0>,Char<' '>,cnrl,Char<';'>,Char<','>,cnrl > endmsg;
    typedef And<CHAR_A,CHAR_C,CHAR_K, endmsg> M_ACK;

    typedef And<CHAR_B,CHAR_Y,CHAR_E,endmsg > M_BYE;

    typedef And<CHAR_C,CHAR_A,CHAR_N,CHAR_C,
		        CHAR_E,CHAR_L, endmsg > M_CANCEL;
    
    typedef And<CHAR_D,CHAR_O, endmsg > M_DO;
    
    typedef And<CHAR_I,CHAR_N,CHAR_F,CHAR_O, endmsg > M_INFO;
    
    typedef And<Or<Char<'I'>,Char<'i'> >,
		Or<Char<'N'>,Char<'n'> >,
		Or<Char<'V'>,Char<'v'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'L'>,Char<'l'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'D'>,Char<'d'> >, endmsg > M_INVALID;

    typedef And<Or<Char<'I'>,Char<'i'> >,
		Or<Char<'N'>,Char<'n'> >,
		Or<Char<'V'>,Char<'v'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'T'>,Char<'t'> >,
		Or<Char<'E'>,Char<'e'> >, endmsg > M_INVITE;

    typedef And<Or<Char<'M'>,Char<'m'> >,
		Or<Char<'E'>,Char<'e'> >,
		Or<Char<'S'>,Char<'s'> >,
		Or<Char<'S'>,Char<'s'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'G'>,Char<'g'> >,
		Or<Char<'E'>,Char<'e'> >, endmsg > M_MESSAGE;

    typedef And<Or<Char<'N'>,Char<'n'> >,
		Or<Char<'O'>,Char<'o'> >,
		Or<Char<'T'>,Char<'t'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'F'>,Char<'f'> >,
		Or<Char<'Y'>,Char<'y'> >, endmsg > M_NOTIFY;

    typedef And<Or<Char<'O'>,Char<'o'> >,
		Or<Char<'P'>,Char<'p'> >,
		Or<Char<'T'>,Char<'t'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'O'>,Char<'o'> >,
		Or<Char<'N'>,Char<'n'> >,
		Or<Char<'S'>,Char<'s'> >, endmsg > M_OPTIONS;

    typedef And<Or<Char<'P'>,Char<'p'> >,
		Or<Char<'R'>,Char<'r'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'C'>,Char<'c'> >,
		Or<Char<'K'>,Char<'k'> >, endmsg > M_PRACK;

    typedef And<Or<Char<'P'>,Char<'p'> >,
		Or<Char<'U'>,Char<'u'> >,
		Or<Char<'B'>,Char<'b'> >,
		Or<Char<'L'>,Char<'l'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'S'>,Char<'s'> >,
		Or<Char<'H'>,Char<'h'> >, endmsg > M_PUBLISH;

    typedef And<Or<Char<'Q'>,Char<'q'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'U'>,Char<'u'> >,
		Or<Char<'T'>,Char<'t'> >,
		Or<Char<'H'>,Char<'h'> >, endmsg > M_QAUTH;

    typedef And<Or<Char<'R'>,Char<'r'> >,
		Or<Char<'E'>,Char<'e'> >,
		Or<Char<'F'>,Char<'f'> >,
		Or<Char<'E'>,Char<'e'> >,
		Or<Char<'R'>,Char<'r'> >, endmsg > M_REFER;

    typedef And<CHAR_R,CHAR_E,CHAR_G,CHAR_I,CHAR_S,CHAR_T,CHAR_E,CHAR_R, endmsg > M_REGISTER;

    typedef And<Or<Char<'S'>,Char<'s'> >,
		Or<Char<'P'>,Char<'p'> >,
		Or<Char<'R'>,Char<'r'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'C'>,Char<'c'> >,
		Or<Char<'K'>,Char<'k'> >, endmsg > M_SPRACK;

    typedef And<Or<Char<'S'>,Char<'s'> >,
		Or<Char<'U'>,Char<'u'> >,
		Or<Char<'B'>,Char<'b'> >,
		Or<Char<'S'>,Char<'s'> >,
		Or<Char<'C'>,Char<'c'> >,
		Or<Char<'R'>,Char<'r'> >,
		Or<Char<'I'>,Char<'i'> >,
		Or<Char<'B'>,Char<'b'> >,
		Or<Char<'E'>,Char<'e'> >, endmsg > M_SUBSCRIBE;

    typedef And<Or<Char<'U'>,Char<'u'> >,
		Or<Char<'P'>,Char<'p'> >,
		Or<Char<'D'>,Char<'d'> >,
		Or<Char<'A'>,Char<'a'> >,
		Or<Char<'T'>,Char<'t'> >,
		Or<Char<'E'>,Char<'e'> >, endmsg > M_UPDATE;
    
    orMsg = strMsg;
    type = msg_invalid;

    if( M_ACK::Matches(orMsg,&c))
        type = msg_ack;
    else if( M_BYE::Matches(orMsg,&c))
        type = msg_bye;
    else if( M_CANCEL::Matches(orMsg,&c))
        type = msg_cancel;
    else if( M_DO::Matches(orMsg,&c))
        type = msg_do;
    else if( M_INFO::Matches(orMsg,&c))
        type = msg_info;
    else if( M_INVALID::Matches(orMsg,&c))
        type = msg_invalid;
    else if( M_INVITE::Matches(orMsg,&c))
        type = msg_invite;
    else if( M_MESSAGE::Matches(orMsg,&c))
        type = msg_message;
    else if( M_NOTIFY::Matches(orMsg,&c))
        type = msg_notify;
    else if( M_OPTIONS::Matches(orMsg,&c))
        type = msg_options;
    else if( M_PRACK::Matches(orMsg,&c))
        type = msg_prack;
    else if( M_PUBLISH::Matches(orMsg,&c))
        type = msg_publish;
    else if( M_QAUTH::Matches(orMsg,&c))
        type = msg_qauth;
    else if( M_REFER::Matches(orMsg,&c))
        type = msg_refer;
    else if( M_REGISTER::Matches(orMsg,&c))
        type = msg_register;
    else if( M_SPRACK::Matches(orMsg,&c))
        type = msg_sprack;
    else if( M_SUBSCRIBE::Matches(orMsg,&c))
        type = msg_subscribe;
    else if( M_UPDATE::Matches(strMsg,&c))
        type = msg_update;
    
    if( type == msg_invalid)
    {
        typedef Or<BLANK, NEWLINE > gonext;
        if( *orMsg <= 0 ) return NULL;
        while( !gonext::Matches(orMsg,&c))
        {
            orMsg++;
            if( *orMsg <= 0 ) return NULL;
        }
    }
    return orMsg;
}

PBYTE msgtypetostr(ESIPMSGTYPE type)
{
    return get_msg_type_string(type);
}

PBYTE get_msg_type_string(ESIPMSGTYPE type)
{
    switch(type)
    {
    case msg_ack:
        return (PBYTE)"ACK";
    case msg_bye:
        return (PBYTE)"BYE";
    case msg_cancel:
        return (PBYTE)"CANCEL";
    case msg_do:
        return (PBYTE)"DO";
    case msg_info:
        return (PBYTE)"INFO";
    case msg_invalid:
        return (PBYTE)"INVALID";
    case msg_invite:
        return (PBYTE)"INVITE";
    case msg_message:
        return (PBYTE)"MESSAGE";
    case msg_notify:
        return (PBYTE)"NOTIFY";
    case msg_options:
        return (PBYTE)"OPTIONS";
    case msg_prack:
        return (PBYTE)"PRACK";
    case msg_publish:
        return (PBYTE)"PUBLISH";
    case msg_qauth:
        return (PBYTE)"QAUTH";
    case msg_refer:
        return (PBYTE)"REFER";
    case msg_register:
        return (PBYTE)"REGISTER";
    case msg_sprack:
        return (PBYTE)"SPRACK";
    case msg_subscribe:
        return (PBYTE)"SUBSCRIBE";
    case msg_update:
        return (PBYTE)"UPDATE";
    default:
        return (PBYTE)"INVALID";
    }
    return NULL;
}


//
////////////////////////////////////////////////////////////
//
