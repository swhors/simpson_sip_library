#pragma once


typedef enum addr_type{
       ip=0,
       h_name
}addr_type_t;

class hostaddr_t
{
public:
	hostaddr_t();
	~hostaddr_t();
    addr_type_t type;
	string		hostAddr;
	int			port;
	hostaddr_t &operator=(const hostaddr_t &other);
	int			operator==(const hostaddr_t &other);
	unsigned char *parse(unsigned char *inStr);
	int				print(unsigned char *outStr);
	int			build(int iType, char *szIP, int port=0);
	void	init();
};

typedef hostaddr_t *p_hostaddr_t;
