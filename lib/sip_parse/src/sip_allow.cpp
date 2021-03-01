#include <stdio.h>

#include <iostream>
#include <string>
using namespace std;
#include "skytool.h"
#include "host_addr.h"
#include "debug.h"
#include "SIpFieldBase.h"
#include "sip_tool.h"
#include "sip_info.h"

#include "sip_allow.h"

st_allow_msg_t allow_msg[ALLOW_ITEM_NUM]=
{
    {ALLOW_INVITE_STR,ALLOW_INVITE_NUM, 6},
    {ALLOW_ACK_STR,ALLOW_ACK_NUM, 3},
    {ALLOW_CANCEL_STR,ALLOW_CANCEL_NUM, 6},
    {ALLOW_OPTIONS_STR,ALLOW_OPTIONS_NUM, 7},
    {ALLOW_BYE_STR,ALLOW_BYE_NUM, 3},
    {ALLOW_UPDATE_STR,ALLOW_UPDATE_NUM, 6},
    {ALLOW_REFER_STR,ALLOW_REFER_NUM, 5},
    {ALLOW_NOTIFY_STR,ALLOW_NOTIFY_NUM, 6},
    {ALLOW_SUBSCRIBE_STR,ALLOW_SUBSCRIBE_NUM, 9},
    {ALLOW_REGISTER_STR,ALLOW_REGISTER_NUM, 8},
    {ALLOW_INFO_STR,ALLOW_INFO_NUM, 4},
};

int get_allow_msg_item_num(IN int num)
{
    if( num >=0 && num < ALLOW_ITEM_NUM)
        return allow_msg[num].ival;
    return 0;
}

int get_allow_msg_item_str(IN int num, OUT char * buf, IN int len)
{
    if( len > 0 )
    {
        if( num >=0 && num < ALLOW_ITEM_NUM)
        {
            memset( buf, 0, len);
#ifdef MSVC_6
            sprintf(buf, "%s", allow_msg[num].str);
#else
            sprintf_s(buf, len, "%s", allow_msg[num].str);
#endif
            return 1;
        }
    }
    return 0;
}

int parse_allow_item(IN PUCHAR dump,OUT PVOID p_hd,IN int len,IN int opt)
{
    int i;
    int item=0;
    for( i = 0; i < ALLOW_ITEM_NUM; i++)
    {
        if( findString( (char*)dump, allow_msg[i].str, allow_msg[i].len))
            item = item +  allow_msg[i].ival;
    }
    return item;
}


static int print_allow_internal(int _phd, PUCHAR buff)
{
    int phd = _phd;
    if( phd > 0)
    {
        strcat((char*)(char*)buff, "Allow: ");
		for( int i = 0 ; i < 11; i++)
		{
			if( phd & allow_msg[i].ival )
			{
				strcatfm((char*)buff, "%s%s", (i==0?"":","),allow_msg[i].str);
            }
        }
		strcat((char*)buff, "\r\n");
        return 1;
    }
    return 0;

}
int print_allow(int _phd, PUCHAR buff)
{
	return print_allow_internal(_phd, buff);
}