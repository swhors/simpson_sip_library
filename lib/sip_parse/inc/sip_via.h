/* 
 * sip_via.h
 *
 * via 필드와 관련된 구조체와 함수를 정의
 *
 * First : 2004 12 
 *       : 2004/12/15/14/51 swhors@naver.com
 */
#pragma once


#define BRANCH_PREFIX "s9hG4bK"
/*
class via_component
{
public:
	via_component();
	~via_component();
    int count;
    sip_version_t ver;
    protocol_type_e type;
    hostaddr_t haddr;
    STRING branch;
    void * next;
    void * pre;
};
class via_t
{
public:
	via_t();
	~via_t();
    via_component *via;
    int num;
};
typedef via_t * p_via_t;

*/

class sip_via_t : public CSipFieldBase
{
public:
    sip_via_t();
	virtual	~sip_via_t();
	
	VOID free();

	//operator
    CSipFieldBase	&operator = (const CSipFieldBase &cFieldBase);
    sip_via_t		&operator = (const sip_via_t &v);
	//int				operator==(const IN CMHOST &host);
    // friendly operator
	SIPINF sipInf;
    hostaddr_t host;
    string branch;
    string received;
    string maddr;
    int ttl;
	int	rport;
    int hidden; //0=disable, 1=enable
    PBYTE			parse(IN PBYTE inStr);
    int				print(OUT PBYTE outBuf=NULL);
	int				build(protocol_type_e protoType,BYTE highVer,BYTE lowVer,
						char *ip,int port,char *sBranch=NULL,char *sReceived=NULL,char *sMaddr=NULL);
	int				build(SIPINF &inf,hostaddr_t &host,char *sBranch=NULL,char *sReceived=NULL,char *sMaddr=NULL);
};

typedef sip_via_t SIPVIA;
typedef sip_via_t *PSIPVIA;

int parse_via(IN PBYTE inStr,OUT PVOID _pVia);
VOID print_via(IN PVOID _pBuff,OUT PBYTE outBuf=NULL);


//typedef via_component via_component_t;
//typedef via_component* p_via_component_t;


#define VIA_PREFIX 	"Via"
#define VIA_PREFIX_LEN 	4
#define VIA_BRANCH 	"branch="

#define VIA(a) a+VIA_PREFIX_LEN
/*
int sip_parse_via(PUCHAR dump, p_via_t via, int len, int opt);

void init_via_buf(PVOID p_buf);
int via_parse_branch(PUCHAR dump, p_via_component_t via_com);
p_via_component_t via_parse_component(PUCHAR dump, p_via_t vias);
p_via_t new_via();
int is_via(PUCHAR dump);

p_via_component_t new_via_component();
p_via_component_t free_via_component(p_via_component_t via_com);
void free_via(p_via_t via);

void copy_via(void * _pdes, void * _psrc);

char* via_parse_sipversion(PUCHAR dump, p_via_component_t via_com);
char* via_parse_protocol_type(PUCHAR dump, p_via_component_t via_com);
int via_parse_hostip(PUCHAR dump, p_via_component_t via_com);
PUCHAR via_calculate_branch(PUCHAR dump);
int via_parse_branch(PUCHAR dump, p_via_component_t via_com);

int append_viacom(IN int msg_type, IN PVOID _phCall,IN OUT PVOID p_via,IN char *sip,IN int port,IN int transport_type,IN char *branch,IN int opt,IN int newgen);
int make_viacom(IN PVOID _phd,OUT UCHAR buff[],IN int opt,IN char *hostip);
int make_via(IN p_via_t phd,OUT PUCHAR msg,IN int opt,IN char *hostip);
int make_via1(OUT PUCHAR msg,IN char *hostip,IN PVOID _pVersion,IN PVOID _ptype);

int check_viacom_branch(IN PVOID _pBranch,IN char * branch, OUT PVOID *_phCall, OUT unsigned int *itime);
int check_via(IN PVOID p_via, IN char *sip, IN int port, IN int transport, IN char * branch, OUT PVOID *_phCall, OUT unsigned int * itime);
int new_via_branch(IN int msg_type, IN PVOID _phCall, IN int add_call, IN OUT PVOID _pstr,IN char * branch);
*/
int append_viacom(IN int msg_type, IN PVOID _phCall,IN OUT PVOID p_via,IN char *sip,IN int port,IN int transport_type,IN char *branch,IN int opt,IN int newgen);
int make_viacom(IN PVOID _phd,OUT UCHAR buff[],IN int opt,IN char *hostip);
int make_via(IN sip_via_t * phd,OUT PUCHAR msg,IN int opt,IN char *hostip);
int make_via1(OUT PUCHAR msg,IN char *hostip,IN PVOID _pVersion,IN PVOID _ptype);


