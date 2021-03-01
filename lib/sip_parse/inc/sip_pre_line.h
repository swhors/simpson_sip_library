#ifndef _SIP_REQUEST_LINE_H
#define _SIP_REQUEST_LINE_H

class sip_pre_line{
public:
    int					hdtype;    //0=invalid, 1=request, 2=response
    uri_addr_t				sipUri; // in case of 1
    SIPINF				sipInf; // in case of 1 or 2
    sip_method_type_t	msg_type;// in case of 1
    int					state;  // in case of 2
    string				text;   // in case of 2
	int					lr;
	string				psp;
	sip_pre_line();
	~sip_pre_line();
	sip_pre_line& operator=(const sip_pre_line& tmp);
	void free();

//	Status-Line: SIP/2.0 202 Accepted
//	Request-Line: INVITE sip:+8581001000@210.121.190.121 SIP/2.0
	int					req_build(BYTE highVer,BYTE lowVer, sip_method_type_t msg_type,
									char *_uid, char *ip,int port=0, protocol_type_e protoType=sip_none_proto);
	int					res_build(BYTE highVer,BYTE lowVer,
									int state=200, char *text = NULL, protocol_type_e protoType=sip_none_proto);
    PBYTE				parse(IN PBYTE inStr);
    int				print(OUT PBYTE outStr=NULL);
};
typedef sip_pre_line* p_sip_pre_line;
typedef sip_pre_line pre_head_line_t;
typedef sip_pre_line* p_pre_head_line_t;


typedef sip_pre_line SIPPRELINE;
typedef p_sip_pre_line PSIPPRELINE;
typedef sip_pre_line SIPREQLINE;
typedef p_sip_pre_line PSIPREQLINE;

PBYTE parse_sippreline(IN PBYTE inStr,OUT PVOID _pPre);
VOID  print_sippreline(IN PVOID _pPre,OUT PBYTE outStr=NULL);

#endif // _SIP_REQUEST_LINE_H

