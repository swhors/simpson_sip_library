#pragma once

class uri_addr_t
{
public:
	uri_addr_t();
	~uri_addr_t();
	void		free();
    //string     user_number; 
    hostaddr_t haddr;
    string     user;
	string		uid;
    string     passwd;
    string     tag;
	string		maddr;
	int			ttl;
    int        u_num_with_sharp;
	protocol_type_e	transport;
	uri_addr_t  &operator=(const uri_addr_t &other);
	unsigned   char	*  parse(unsigned char* dump);
	int			print(unsigned char *dump);
	int			build(char *_ip,int _port,char *_uid,char *_user=NULL, char *_password=NULL);
	int			operator==(const uri_addr_t &other);
};
typedef uri_addr_t* p_uri_addr_t;

int free_uri(p_uri_addr_t uri);
void init_uri(PVOID p_buf);
int make_uri(IN PVOID _phCall, OUT PUCHAR buff);
/*
void init_uri(PVOID p_buf);
int uri_parse_user(PUCHAR dump, p_uri_addr_t uri);
int uri_parse_user_number(PUCHAR dump, p_uri_addr_t tofrom);
int uri_parse_hostaddr( PUCHAR dump_mem, p_hostaddr_t host);
int uri_parse_tag(PUCHAR dump_mem, p_uri_addr_t p_uri);
int uricomp(PVOID p_src, PVOID p_dest);
int uricpy(PVOID p_dest, PVOID p_src);
int free_uri(p_uri_addr_t uri);
int build_uri(OUT PVOID _pUri,IN char *hostip,IN int port,char *u_number);
// Make Message
int make_uri_str_opt_field_ext(IN PVOID _phCall, 
                               OUT PVOID _pStr,
                               IN char * str,
                               IN int opt);
int make_self_uri_tag_option(OUT PVOID pstr);
int make_self_uri_tag_option_ext(OUT PVOID pstr,IN PVOID _phCall);
int make_self_uri_branch_option_ext(OUT PVOID pstr, IN PVOID _phCall);
int make_uri(IN PVOID _phCall,OUT PUCHAR buff);
int make_uri_by_addr(IN PVOID _phd,OUT PUCHAR buff,IN char *sip,IN int port);
void make_content_type(OUT PUCHAR msg, IN PSTRING sbuf);
int make_user_number(PSTRING user_number,PUCHAR buff);
*/
