#pragma once

#ifndef __SIP_FIELD_H
#define __SIP_FIELD_H

//#include "sip_tool.h"

typedef int (parse_func)(unsigned char * dump, void * hd, int len,int opt);

unsigned char * parse_sip_field_name(unsigned char * inStr,ESIPFIELD & type);
unsigned char * get_field_name(ESIPFIELD type);

int parse_from(unsigned char * dump, void * p_hd);
int parse_to(unsigned char * dump, void * p_hd);
int parse_call_id(unsigned char * dump, void * p_hd);
int parse_cseq(unsigned char * dump, void * p_hd);
int parse_contact(unsigned char * dump, void * p_hd);
int parse_max_forward(unsigned char * dump, void * p_hd);
int parse_expire(unsigned char * dump, void * p_hd);
int parse_subject(unsigned char * dump, void * p_hd);
int parse_content_type(unsigned char * dump, void * p_hd);
int parse_content_length(unsigned char * dump, void * p_hd);
int parse_user_agent(unsigned char * dump, void * p_hd);
int parse_date(unsigned char * dump, void * p_hd);
int parse_via(PBYTE dump, void *_pField);
int parse_www_auth(unsigned char * dump, void * p_hd);
int parse_authorization(unsigned char * dump, void * p_hd);
int parse_allow(unsigned char * dump, void * p_hd);
int parse_accept(unsigned char * dump, void * p_hd);
int parse_record_route(unsigned char * dump, void * p_hd);
int parse_route(unsigned char * dump,void * _pField);
int parse_accept_language(unsigned char * dump, void * p_hd);
int parse_session_expires(unsigned char * dump, void * p_hd);
int parse_accept_encoding(unsigned char * dump, void * p_hd);
int parse_content_disposition(unsigned char * dump, void * p_hd);
int parse_content_encoding(unsigned char * dump, void * p_hd);
int parse_content_language(unsigned char * dump, void * p_hd);
int parse_supported(unsigned char * dump, void * p_hd);
int parse_proxy_authenticate(unsigned char * dump,void * _pField);
int parse_proxy_authorization(unsigned char * dump,void * _pField);
int parse_server(unsigned char *dump, void *_pField);

// 파싱 메인 함수
int parse_field(e_sip_field_t field, unsigned char * inStr, PVOID _pSipHd);


class sip_field_t
{
public:
	sip_field_t();
	~sip_field_t();
    tofrom_tag_t to;
    tofrom_tag_t from;
#ifdef LINUX
#else
#ifdef WIN32
	vector<tr1::shared_ptr<sip_via_t>> via_list;
#elif defined WINCE
	vector<sip_via_t> via_list;
#endif
#endif
    call_id_t cid;
    cseq_hd_t cseq;
    string user_agent;
    date_t date;
	int contact_count;
#ifdef LINUX
#else
#ifdef WIN32
	vector<tr1::shared_ptr<sip_contact_hd_t>> contact_list;
	vector<tr1::shared_ptr<record_route_uri_t>> record_list;
#elif defined WINCE
	vector<sip_contact_hd_t> contact_list;
	vector<record_route_uri_t> record_list;
#endif
#endif
    sip_authentication_t www_auth;
    int expire;
    int max_forward;
    string subject;  
    string content_type;
    int content_length;
    string accept;
	string accept_language;
	string supported;
	string session_expires;
    int allow;
    //string sdp;
	string server;

	sip_field_t &operator=(const sip_field_t &other);
	int			operator==(const sip_field_t &other);

	PBYTE	pasre(PBYTE inStr);
	int		print(PBYTE outStr);
	int		build();
	void	free();
};
typedef sip_field_t *p_sip_field_t;

#endif // __SIP_FIELD_H

