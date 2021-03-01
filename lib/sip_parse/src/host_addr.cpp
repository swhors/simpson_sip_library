#include <iostream>
#include <string>
#include <vector>
#include "peg_def.h"
#include "peg_xsip.h"

using namespace std;
using namespace peg_templ;

#include "host_addr.h"

hostaddr_t::hostaddr_t()
{
	init();
}
hostaddr_t::~hostaddr_t()
{
	init();
}
hostaddr_t &hostaddr_t::operator=(const hostaddr_t &other)
{
	this->type = other.type;
	this->hostAddr= other.hostAddr;
	this->port = other.port;
	return *this;
}
int	hostaddr_t::operator==(const hostaddr_t &other)
{
	if( this->type == other.type )
	{
		if( hostAddr == other.hostAddr)
		{
			if( this->port == other.port)
				return 1;
		}
	}
	return 0;
}
void hostaddr_t::init()
{
    type =ip;
	//hostAddr = "";
	port = -1;;
}

typedef FindRepeat<host>findHost;
typedef FindRepeat<IPv4address> findIPAddress;

typedef And<Char<':'>, PlusRepeat<DIGIT> >portNum;
typedef FindRepeat<portNum> findPortNum;

unsigned char *hostaddr_t::parse(unsigned char *_inStr)
{
	C c;
	unsigned char * inStr= _inStr;
	unsigned char * fStr = NULL;
	int dLen;

	// 
	if( findHost::Matches(inStr, &c, fStr))
	{
		dLen = (int)(inStr-fStr);
		this->hostAddr = "";
		this->hostAddr.append((char*)fStr, dLen);
		if( findPortNum::Matches(inStr, &c, fStr))
		{
			this->port = atoi((char*)(fStr + 1));
		}
		else
			this->port = 0;
		return inStr;
	}
	return NULL;
}
int hostaddr_t::print(unsigned char *outStr)
{
	if( outStr )
	{
		if( this->port > 0 )
		{
			sprintf((char*)outStr, "%s:%d", (!hostAddr.empty()?(char*)hostAddr.data():"127.0.0.1"), this->port);
		}
		else
			sprintf((char*)outStr, "%s", (!hostAddr.empty()?(char*)hostAddr.data():"127.0.0.1"));
		return 1;
	}
	return 0;
}
int hostaddr_t::build(int iType, char *szIP, int _port)
{
	this->type = (addr_type_t)iType;
	hostAddr = szIP;
	this->port = _port;
	return 1;
}