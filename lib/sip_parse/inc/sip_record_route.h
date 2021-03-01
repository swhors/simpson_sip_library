/////////////////////////////////////////////////////////////
// sip_record_route.h
// create : 2005/3/2 by swhors@skycom.ne.kr
//          리코드 라우트와 관련된 파싱과 관리 함수 선언.
// update : 2005/3/7 by swhors@skycom.ne.kr
//          check_record_route 추가
//
/////////////////////////////////////////////////////////////
#pragma once

class record_route_uri_t
{
public:
	record_route_uri_t();
	uri_addr_t		uri;
	//shostaddr_t		haddr;
	string			tag;
	string			rvRRParam;
	int				lr;
	string			psp;
	int				type; // 0-Record-Route, 1-Route
	record_route_uri_t	&operator=(const record_route_uri_t&other);
	int					operator==(const record_route_uri_t&other);
	PBYTE	parse(PBYTE inStr);
	int		print(PBYTE outStr);
	void	free();
	int		build(int _type, char *ip, int port, int _lr, char *_rvRRParam, char *ftag, char *_psp);
};

typedef record_route_uri_t *p_record_route_uri_t;

void init_record_route_buf(PVOID _pRoute);
void free_record_route(PVOID _pRoute);
int delete_record_route(PVOID _pRoute, PVOID _pRouteUri);
int append_record_route(IN PVOID _pRoute,
						IN char *c_num,
						IN char *ip,
						IN int port,
						IN char * frag,
						IN char *lr,
						IN char *rvrrParam,
						IN int type);

int sip_parse_record_route(PUCHAR dump, PVOID _pRoute, int len, int opt);
int check_record_route(PVOID _pRouteUri, char *c_num, char *ip, int port);

// Make Message
int make_record_route(IN PVOID _pRoute,OUT PUCHAR buff);
