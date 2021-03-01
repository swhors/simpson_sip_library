#ifndef _SIP_INFO_H
#define _SIP_INFO_H

typedef struct _INTBYTE
{
	int u:8;
	int	l:8;
}INTBYTE;

class sip_info
{
public:
    sip_info(){v.u=0;v.l=0;proto_type=sip_none_proto;}
    ~sip_info(){}
    INTBYTE v;
    protocol_type_e proto_type;
	// operator
    sip_info& operator=(const sip_info& other);
    int operator==(const sip_info& other);
	int	build(protocol_type_e proto=sip_none_proto,BYTE highVer=0,BYTE lowVer=0);
    PBYTE parse(IN PBYTE inStr);
    VOID print(OUT PBYTE _pOutBuf=NULL);
};

typedef sip_info SIPINF;
typedef sip_info *PSIPINF;

PBYTE parse_sipinfo(IN PBYTE inStr,OUT PVOID _pBuff);
VOID print_sipinfo(IN PVOID _pBuff,OUT PBYTE _pOutBuf=NULL);

#endif // _SIP_INFO_H

