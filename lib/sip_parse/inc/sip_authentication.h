#pragma once

#ifndef _SIP_AUTHENTICATION_H
#define _SIP_AUTHENTICATION_H

//#include "sip_tool.h"

#define ENC_MD5 1
#define ENC_MD5_STR "MD5"

#define AUTH_NONE			0
#define WWW_AUTHENTICATE	1
#define AUTHORIZATION		2
#define PROXY_AUTHENTICATE	3
#define PROXY_AUTHORIZATION	4

class sip_authentication_t
{
public:
	sip_authentication_t();
    string username;
    string realm;
    string domain;
    string qop;
    string nonce;
    string cnonce;
    string opaque;
    string response;
    string sip_uri;
    int algorithm;
    int stale;
    int nc;
    int type;
	PBYTE	parse(IN PBYTE inStr);
};
typedef sip_authentication_t *p_sip_authentication_t;

void init_authentication_buf(PVOID p_buf);

void free_authentication(PVOID p_hd);

int parse_auth(PUCHAR dump, PVOID p_hd, int len, int opt);

int auth_user_name(PVOID p_hd, PUCHAR dump);
int auth_realm(PVOID p_hd, PUCHAR dump);
int auth_qop(PVOID p_hd, PUCHAR dump);
int auth_nonce(PVOID p_hd, PUCHAR dump);
int auth_opaque(PVOID p_hd, PUCHAR dump);
int auth_uri(PVOID p_hd, PUCHAR dump);
int auth_response(PVOID p_hd, PUCHAR dump);
int auth_stale(PVOID p_hd, PUCHAR dump);
int auth_algorithm(PVOID p_hd, PUCHAR dump);
int auth_domain(PVOID p_hd, PUCHAR dump);
int auth_cnonce(PVOID p_hd, PUCHAR dump);
int auth_nc(PVOID p_hd, PUCHAR dump);

int make_sip_authentication(PVOID p_hd, PUCHAR buff);
int make_req_reg_auth(IN PVOID _pSipHd, OUT PUCHAR msg, IN char *hostip);


#endif // _SIP_AUTHENTICATION_H
