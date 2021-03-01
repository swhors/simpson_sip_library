#pragma once

#include <time.h>

//#include "sky_def.h"
//#include "sky_pstring.h"

#define SIP_INFO_PREFIX  "SIP/"
#define SIP_PROTOCOL_UDP "/UDP"
#define SIP_PROTOCOL_TCP "/TCP"

#define USER_ID_PREFIX "user="

#ifdef WINCE
#define	sprintf_s	_snprintf
#endif

//메소드별 구현 함수.
typedef struct sip_func
{
    void *func;
}sip_func_t, *p_sip_func_t;

typedef enum protocol_type{
	sip_none_proto,
    sip_tcp=1,
    sip_udp
} protocol_type_e;

typedef struct sip_version
{
    int major;
    int minor;
}sip_version_t, *p_sip_version_t;

typedef struct _date
{
    struct tm ltm;
    char tm_zone[12];
	int ichecked;
}date_t, *p_date_t;

void fill_sip_version(PVOID p_ver, int major, int minor);
int	HostaddrToStr(PVOID p_src, char *tbuf, int buflen);
int hostaddrcomp(PVOID p_src, PVOID p_dest);
int hostaddrcpy(PVOID p_dest,PVOID p_src);
void init_hostaddr_buf(PVOID p_buf);
int parse_hostaddr( PUCHAR dump_mem, p_hostaddr_t host);
int print_siphost(IN PBYTE outStr,OUT PVOID _pSipHost);
int build_ip_hostaddr(OUT PVOID _pHost,IN char *hostip,IN int port);
void free_hostaddr(p_hostaddr_t host);

char * parse_sip_version(PUCHAR dump_mem, p_sip_version_t ver);
PUCHAR get_new_line(PUCHAR dump);
char * parse_sip_protocol_type(PUCHAR dump, int * type);
int parse_userid(PUCHAR dump, char* user);

void display_content_length(int data);
int sip_parse_int(PUCHAR dump, int len);

// 아웃 메세지의 date 필드의 메세지를 생성한다.
// out : 출력 버퍼
// in  : p_date_t 타입의 데이터 정보
void make_date_msg(OUT PUCHAR msg, IN p_date_t pDate);

void build_date_msg(OUT PVOID _pDate);
int make_allow_field(PUCHAR output, int value);

void make_expires_msg(OUT PUCHAR msg,IN int expires,IN int force);
// 출력 메세지에  User-Agent의 정보를 출력한다.
// 입력 : PSTRING 타입의 유저-에이젼트 정보
// 출력 : PUCHAR 타입의 메세지 버퍼
//void make_user_agent_msg(OUT PUCHAR msg, IN PSTRING sbuf);
//세션 익스파이어 추가
//void make_session_expires_msg(OUT PUCHAR msg, IN PSTRING sbuf);
//void make_accept_language_msg(OUT PUCHAR msg, IN PSTRING sbuf);
//void make_supported_msg(OUT PUCHAR msg, IN PSTRING sbuf);
// allow item을 프린트 한다.
// 
//int is_tag_option_myself_internal(PSTRING pstr);
//void make_content_length(PUCHAR msg, int len);
//void make_subject(PUCHAR msg, PSTRING sbuf);
//void make_int_field(PUCHAR output, char *pfield, int value);
//void make_str_field(PUCHAR output, char * pfield, PSTRING p_str);

int is_tag_option_myslef(PVOID pstr);
PBYTE parse_displayname(IN PBYTE inStr, OUT PBYTE outStr,int len);
PBYTE parse_siphost(IN PBYTE inStr,OUT PVOID _pSipHost);
