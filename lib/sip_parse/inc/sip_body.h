#pragma once

#ifndef __SIP_BODY_H
#define __SIP_BODY_H
//#include "skytool.h"
//#include "sip_tool.h"
//#include "sip_header.h"

void init_sip_hd_field(IN PVOID _pfield);
void free_sip_field(IN PVOID _pfield);
/* SIP ������ ���� ��� ����ü�� ���� ����*/
//int free_hd(void * p);
int sip_body_parse(IN PUCHAR msg,OUT PVOID _phCall,IN PUCHAR dump_mem,IN int mem_len);
int parse_request(IN PUCHAR dump,OUT PVOID p_hd);
#endif // __SIP_BODY_H
