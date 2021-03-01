#pragma once

#include "sip_header.h"
#define TO_TAG_IP(a) a->call->sipfield.to.uri.haddr.eadr.ip.sadr
#define FROM_TAG_IP(a) a->call->sipfield.from.uri.haddr.eadr.ip.sadr
#define TYPE_TO_ADDR(a) a->call->sipfield.to.uri.haddr.type
#define TYPE_FROM_ADDR(a) a->call->sipfield.from.uri.haddr.type

#define PRINT_TO(a) {\
        PHCALL hcall = a;\
        if( a->call->sipfield.to.uri.haddr.type == 0)\
            printf("to : %s\n", TO_TAG_IP(a));\
        }
