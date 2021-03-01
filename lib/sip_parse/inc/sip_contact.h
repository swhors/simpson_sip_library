#pragma once

#ifndef _SIP_CONTACT_H
#define _SIP_CONTACT_H

//#include "sip_tool.h"
//#include "sip_uri.h"

class sip_contact_hd_t
{
public:
	sip_contact_hd_t();
	~sip_contact_hd_t();
	uri_addr_t uri;
    string action;
    int expires;
    int checked;
	int q;
	
	PBYTE parse(IN PBYTE inStr);
	int print(OUT PBYTE outStr);
	int build(char *uid,char *ip,int _port,char * _action,int _q,int _expires,char *s_display);
	sip_contact_hd_t &operator=(const sip_contact_hd_t &other);
	void	free();

};
typedef sip_contact_hd_t *p_sip_contact_hd_t;

void init_contact_buf(PVOID p_buf);
int contactcpy(IN PVOID _pDes,IN PVOID _pSrc);
//int parse_contact_action(PUCHAR dump, PSTRING action);
//int sip_parse_contact(PUCHAR dump, p_sip_contact_hd_t hd, int len);
//int parse_contact_user(PUCHAR dump, PSTRING user);
//int parse_contact_host(PUCHAR dump, p_uri_addr_t uri);
//int parse_contact_expires(PUCHAR dump);
void free_contact(p_sip_contact_hd_t hd);
int build_contact(IN PVOID _pContact,
                  IN char *action,
                  IN int expire,
                  IN char *hostip,
                  IN int port,
                  IN char *u_number);

// Make message
int make_contact(IN PVOID _phCall, IN PVOID p_hd, OUT PUCHAR buff,IN int msg_type,IN char *hostip);

#endif //_SIP_CONTACT_H
