#pragma once

#ifndef _SIP_CALL_ID_H
#define _SIP_CALL_ID_H

//#include "sip_tool.h"

class call_id_t
{
public:
	call_id_t();
	~call_id_t();
    string		callid;
    string		hostaddr;
	int		    build(char *ip,int port=5060,char *_callid=NULL);
    VOID        free();
    PBYTE	    parse(IN PBYTE inStr);
    int			print(OUT PBYTE outStr);
    call_id_t	    &operator=(const call_id_t &cid);
    int		    operator==(const call_id_t &cid);
    int		    operator<(const call_id_t &cid);
    int		    operator>(const call_id_t &cid);
	char		*ToString(char *outStr, int len);
};
typedef call_id_t *p_call_id_t;

void init_call_id_buf(PVOID p_buf);

int sip_parse_call_id(PUCHAR dump, p_call_id_t cid, int len);
//int sip_parse_s_callid(PUCHAR dump, PSTRING cid);
int sip_parse_s_host(PUCHAR dump, char *host);

int callidcmp(PVOID p_src, PVOID p_dest);
int callidcpy(PVOID p_dest, PVOID p_src);

void free_callid(p_call_id_t hd);

int make_callid(PVOID _phd, PUCHAR buff);
int build_callid(OUT PVOID _pCid,IN char *callid,IN char *hostip);
int generate_callid(OUT p_call_id_t hd, OUT char *callid,IN int len, char *hostip);

#endif // _SIP_CALL_ID_H

