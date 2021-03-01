/*
 * sip_cseq.h
 * cseq의 파싱에 관한 함수들을 정의
 *
 * First : 2004 12 14 swhors@naver.com
 *       : 2004 12 14 swhors@naver.com
 */

#pragma once

#ifndef _SIP_CSEQ_H
#define _SIP_CSEQ_H
//#include "sip_tool.h"
//#include "sip_method.h"

class cseq_hd_t
{
public:
	cseq_hd_t();
	~cseq_hd_t();
    sip_method_type_t type;
    int cseq;
	unsigned char* parse(unsigned char * inStr);
	int print(unsigned char * outStr);
	cseq_hd_t	&operator=(const cseq_hd_t &other);
	int			operator==(const cseq_hd_t &other);
};
typedef cseq_hd_t *p_cseq_hd_t;

void init_cseq_buf(PVOID p_buf);
int sip_parse_cseq(PUCHAR dump, p_cseq_hd_t hd,int len);
int make_cseq(PVOID p_hd, PUCHAR buff);
int build_cseq(OUT PVOID _pCseq,IN int cseq,IN sip_method_type_t method);

#endif // _SIP_CSEQ_H

