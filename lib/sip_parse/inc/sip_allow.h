#pragma once

#ifndef _SIP_ALLOW_H
#define _SIP_ALLOW_H

#include <stdio.h>
//#include "sip_tool.h"
//#include "sip_header.h"

typedef struct st_allow_msg{
    char * str;
    int    ival;
	int len;
}st_allow_msg_t;

#define ALLOW_ITEM_NUM	11

#define ALLOW_INVITE_NUM 0x0001
#define ALLOW_INVITE_STR "INVITE"
#define ALLOW_ACK_NUM 0x0002
#define ALLOW_ACK_STR "ACK"
#define ALLOW_CANCEL_NUM 0x0004
#define ALLOW_CANCEL_STR "CANCEL"
#define ALLOW_OPTIONS_NUM 0x0008
#define ALLOW_OPTIONS_STR "OPTIONS"
#define ALLOW_BYE_NUM 0x0010
#define ALLOW_BYE_STR "BYE"
#define ALLOW_UPDATE_NUM 0x0020
#define ALLOW_UPDATE_STR "UPDATE"
#define ALLOW_REFER_NUM 0x0040
#define ALLOW_REFER_STR "REFER"
#define ALLOW_NOTIFY_NUM 0X0080
#define ALLOW_NOTIFY_STR "NOTIFY"
#define ALLOW_SUBSCRIBE_NUM	0x0100
#define ALLOW_SUBSCRIBE_STR	"SUBSCRIBE"
#define ALLOW_REGISTER_NUM	0x0200
#define ALLOW_REGISTER_STR	"REGISTER"
#define ALLOW_INFO_NUM	0x0400
#define ALLOW_INFO_STR	"INFO"

int parse_allow_item(IN PUCHAR dump,OUT PVOID p_hd,IN int len,IN int opt);

int get_allow_msg_item_num(IN int num);
int get_allow_msg_item_str(IN int num, OUT char * buf, IN int len);
int print_allow(int _phd, PUCHAR buff);
#endif // _SIP_ALLOW_H

