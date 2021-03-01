#pragma once

typedef enum to_from_enum
{
    none_tag=0,
    to_tag ,
    from_tag
}to_from_enum_t;

class tofrom_tag_t{
public:
	tofrom_tag_t();
	~tofrom_tag_t();
	void free();

    to_from_enum_t type;
    string display;
    uri_addr_t uri;
    string passwd;
    string tag;

	tofrom_tag_t &operator=(const tofrom_tag_t &other);
	int			operator==(const char *ip);
	int			operator==(const tofrom_tag_t &other);
	PBYTE		parse(IN PBYTE inStr);
	int			print(OUT PBYTE outStr);
	int			build(char *_uid,char *ip,int _port,char *_display,char *_tag,int dir, int _brace);
};
typedef tofrom_tag_t *p_tofrom_tag_t;

int tofrom_parse(PUCHAR dump, p_tofrom_tag_t hd);

typedef int (parse_tofrom)(PUCHAR dump, PVOID phd, int to); 

int check_to_tag_by_ip(PVOID p_to, PUCHAR s_ip);
int check_tofrom_unum(PVOID p_to, PVOID p_from);

void init_tofrom_buf(PVOID p_buf);
int to_from_parse_upwd(PUCHAR dump, p_tofrom_tag_t tofrom);
int to_from_parse_displayinfo(PUCHAR dump, p_tofrom_tag_t tofrom);
int to_from_parse_tag(PUCHAR dump, p_tofrom_tag_t tofrom);

void free_tofrom(p_tofrom_tag_t hd);

// Make message
int make_to_from_by_ip(IN PVOID _phCall, IN PVOID p_hd,OUT PUCHAR msg,IN int self,IN char *ip,IN int port);
int make_to_from(IN PVOID _phCall, IN PVOID p_hd,OUT PUCHAR msg,IN int self);

int build_to_tag(IN PVOID _pTag,
                 IN char *hostip,
                 IN char *u_num,
                 IN int port,
                 IN char *display,
				 IN char *u_tag);

int build_from_tag(IN PVOID _pTag,
                   IN char *hostip,
                   IN char *u_num,
                   IN int  port,
                   IN char *display,
				   IN char *u_tag);
