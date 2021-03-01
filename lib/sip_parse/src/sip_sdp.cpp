#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "skytool.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <iostream>

#include <stdarg.h>

using namespace std;
using namespace std::tr1;
#include "host_addr.h"
#include "sip_tool.h"
#include "peg_def.h"
#include "peg_xsip.h"

#include "sip_sdp.h"

using namespace peg_templ;

/*
a=rtpmap:18 G729/8000/1
a=rtpmap:4 G723/8000/1
a=rtpmap:0 PCMU/8000/1
a=rtpmap:8 PCMA/8000/1
*/

typedef struct _media_num
{
	char	*name;
	int		num;
	int		len;
}MEDIANUM;

#define MEDIANUMMAX	8


MEDIANUM mediaNum[MEDIANUMMAX]={{"G729",18,4},{"g729",18,4},{"G723",4,4},{"g723",4,4}, 
								{"PCMU",0,4},{"pcmu",0,4},{"PCMA",8,4},{"pcma",8,4}};
sdp_media_att::sdp_media_att()
{
    this->init();
}

sdp_media_att::~sdp_media_att()
{
    this->free();
}

sdp_media_att &sdp_media_att::operator=(const sdp_media_att & matt)
{
    this->fieldname = matt.fieldname;
	this->format = matt.format;
	this->type = matt.type;
    return *this;
}

int	sdp_media_att::operator==(const sdp_media_att & other)
{
	if( this->fieldname == other.fieldname )
	{
		if( this->format == other.format)
		{
			if( this->type == other.type)
				return 1;
		}
	}
	return 0;
}
int	sdp_media_att::operator>(const sdp_media_att & other)
{
	if( this->fieldname > other.fieldname )
	{
		if( this->format > other.format)
			if( this->type > other.type)
				return 1;
	}
	return 0;
}
int	sdp_media_att::operator<(const sdp_media_att & other)
{
	if( this->fieldname < other.fieldname )
	{
		if( this->format < other.format)
			if( this->type < other.type)
				return 1;
	}
	return 0;
}

VOID sdp_media_att::init()
{
}

VOID sdp_media_att::free()
{
    this->type = "";
	this->format = -1;
    this->fieldname = "";
}

/*
    CMSTR    fieldname;
    CMSTR    value;
*/
//PBYTE sdp_media_att::parse(IN PBYTE inStr)
int sdp_media_att::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    C c;
    typedef And<PlusRepeat<Or<ALPHA,DIGIT> >,Char<':'>> field_str;
	typedef And<Char<':'>, PlusRepeat<ALPHA>, Char<' '> >format_str;
	typedef And<Char<' '> , PlusRepeat<Or<ALPHA,DIGIT,Char<'/'>,Char<'-'>,Char<'='>,Char<':'>,Char<' '>, Char<'.'>,Char<','> > > >type_str;
    typedef FindRepeat<field_str > fField;
    typedef FindRepeat<type_str > fType;
	typedef FindRepeat<format_str> fFormat;
    tmp = inStr;
    if( fField::Matches(tmp,&c,start))
    {
		fieldname = "";
		fieldname.append((char*)start,((int)(tmp-start-1) ));
		format = atoi((char*)tmp);
    }
    tmp = inStr;
	if( fType::Matches(tmp,&c,start) )
    {
		this->type = "";
		this->type.append ((char*)start+1,((int)(tmp-start-1)) );
    }
    //return tmp;
	return 1;
}

int sdp_media_att::print(OUT PBYTE outStr)
{
    if( !outStr )
    {
        return 0;
    }
    else
	{
		if( this->fieldname.length() > 0 )
		{
			if( format >= 0 && this->fieldname.size() > 0 && this->type.size() > 0 )
			{
				sprintf((char*)outStr,"a=%s:%d %s\r\n",
							this->fieldname.data(),
							this->format,
							this->type.data());
				return 1;
			}
			else if( this->fieldname.size() > 0 && this->type.size() > 0 && format <0 )
			{
				sprintf((char*)outStr,"a=%s:%s\r\n", fieldname.data(), type.data() );
				return 1;
			}
			else if( this->fieldname.size() > 0 && this->type.size() <= 0 && format< 0 )
			{
				sprintf((char*)outStr,"a=%s\r\n", fieldname.data() );
			}
			else if( this->fieldname.size() > 0 && this->type.size() <= 0 && format>= 0 )
			{
				sprintf((char*)outStr,"a=%s:%d\r\n", fieldname.data(), format );
				return 1;
			}
		}
    }
	return 0;
}

sdp_connection_info::sdp_connection_info()
{
    this->init();
}

sdp_connection_info::~sdp_connection_info()
{
    this->free();
}

sdp_connection_info &sdp_connection_info::operator=(const sdp_connection_info & cinf)
{
    this->Address = cinf.Address;
    this->AddressType = cinf.AddressType;
    this->networktype = cinf.networktype;
    return *this;
}

BOOL sdp_connection_info::isEmpty()
{
	if( this->Address.size() > 0 )
	{
		return false;
	}
	return true;
}

int	sdp_connection_info::operator==(const sdp_connection_info & other)
{
	if( this->Address == other.Address )
	{
		if( this->AddressType == other.AddressType)
		{
			if( this->networktype == other.networktype)
				return 1;
		}
	}
	return 0;
}
int	sdp_connection_info::operator>(const sdp_connection_info & other)
{
	if( this->Address > other.Address )
	{
		if( this->AddressType > other.AddressType)
		{
			if( this->networktype > other.networktype)
				return 1;
		}
	}
	return 0;
}
int	sdp_connection_info::operator<(const sdp_connection_info & other)
{
	if( this->Address < other.Address )
	{
		if( this->AddressType < other.AddressType)
		{
			if( this->networktype < other.networktype)
				return 1;
		}
	}
	return 0;
}


VOID sdp_connection_info::init()
{
    this->AddressType = e_addr_type_no;
    this->networktype = e_net_type_no;
	this->bEmpty= TRUE;

}

VOID sdp_connection_info::free()
{
	this->bEmpty= TRUE;
	this->Address="";
}

void sdp_connection_info::build(IN char *szAddr,IN e_net_type nType,IN e_addr_type aType)
{
	if( !szAddr )
		this->Address = "0.0.0.0";
	else
		this->Address = szAddr;
	this->AddressType = aType;
	this->networktype = nType;
}

//PBYTE sdp_connection_info::parse(IN PBYTE inStr)
int sdp_connection_info::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    C c;
    typedef And<CHAR_I,CHAR_N > in_str;
    typedef And<CHAR_O,CHAR_U,CHAR_T > out_str;
    typedef And<CHAR_I,CHAR_P,CHAR_4 > ip4_str;
    typedef And<CHAR_I,CHAR_P,CHAR_6 > ip6_str;
    typedef And<CHAR_O,CHAR_T,CHAR_H,CHAR_E,CHAR_R > other_str;
    typedef FindRepeat<in_str> fIn;
    typedef FindRepeat<out_str> fOut;
    typedef FindRepeat<ip4_str> fIp4;
    typedef FindRepeat<ip6_str > fIp6;
    typedef FindRepeat<other_str > fOther;
    typedef FindRepeat<hostaddr1 > fHost;
    tmp = inStr;
    if( fIn::Matches(tmp,&c,start))
    {
        this->networktype = e_net_type_in;
    }
    else if( fOut::Matches(tmp,&c,start))
    {
        this->networktype = e_net_type_out;
    }
    
    if( fIp4::Matches(tmp,&c,start))
    {
        this->AddressType = e_addr_type_ip4;
    }
    else if( fIp6::Matches(tmp,&c,start))
    {
        this->AddressType = e_addr_type_ip6;
    }
    else if( fOther::Matches(tmp,&c,start))
    {
        this->AddressType = e_addr_type_oth;
    }
    
    if( fHost::Matches(tmp,&c,start))
    {
		this->Address="";
		this->Address.append((char*)start,((int)(tmp-start)));
    }
    
    //return tmp;
	return 1;
}

int sdp_connection_info::print(IN PBYTE outStr)
{
    if( outStr )
    {
		if( this->Address.size() > 0 )
        {
			char tbuf1[12];
			if( this->AddressType == e_addr_type_ip4 )
			{
				sprintf(tbuf1, "%s", "IP4");
			}
			else if(this->AddressType == e_addr_type_ip6 )
			{
				sprintf(tbuf1, "%s", "IP6");
			}
			else if( this->AddressType == e_addr_type_oth )
			{
				sprintf(tbuf1, "%s", "OTHER");
			}
			else
			{
				return 0;
			}
			sprintf((PCHAR)outStr,"c=%s %s %s\r\n",
				(networktype == e_net_type_in?"IN":"OUT"),
				tbuf1,
				(Address.size()?Address.data():"127.0.0.1"));
			return 1;
		}
    }
	return 0;
}

sdp_time_description::sdp_time_description()
{
    this->init();
}

sdp_time_description::~sdp_time_description()
{

}

sdp_time_description &sdp_time_description::operator=(const sdp_time_description & tinf)
{
    this->start = tinf.start;
    this->stop = tinf.stop;
    return *this;
}


int	sdp_time_description::operator==(const sdp_time_description & other)
{
	if( this->start == other.start )
	{
		if( this->stop == other.stop )
		{
			return 1;
		}
	}
	return 0;
}
int	sdp_time_description::operator>(const sdp_time_description & other)
{
	if( this->start > other.start )
	{
		if( this->stop > other.stop )
		{
			return 1;
		}
	}
	return 0;
}
int	sdp_time_description::operator<(const sdp_time_description & other)
{
	if( this->start < other.start )
	{
		if( this->stop < other.stop )
		{
			return 1;
		}
	}
	return 0;
}


VOID sdp_time_description::init()
{
    this->start = 0;
    this->stop = 0;
}

//PBYTE sdp_time_description::parse(IN PBYTE inStr)
int sdp_time_description::parse(IN PBYTE inStr)
{
    PBYTE start,tmp;
    C c;
    typedef PlusRepeat<DIGIT > num_str;
    typedef FindRepeat<num_str > fNum;
    tmp = inStr;
    if( fNum::Matches(tmp,&c,start) )
    {
        this->start = atoi((PCHAR)start);
    }
    if( fNum::Matches(tmp, &c,start ) )
    {
        this->stop = atoi((PCHAR)start);
    }
    //return inStr;
	return 1;
}

void sdp_time_description::build(int _start,int _stop)
{
	this->start = _start;
	this->stop = _stop;
}

int  sdp_time_description::print(OUT PBYTE outStr)
{
    if( outStr )
    {
        sprintf((PCHAR)outStr,"t=%d %d\r\n", this->start, this->stop);
		return 1;
    }
	return 0;
}

sdp_owner_info::sdp_owner_info()
{
    this->init();
}

sdp_owner_info::~sdp_owner_info()
{
    this->free();
}

sdp_owner_info &sdp_owner_info::operator=(const sdp_owner_info & oinf)
{
    this->address = oinf.address;
    this->address_type = oinf.address_type;
    this->name = oinf.name;
    this->network_type = oinf.network_type;
    this->session_id = oinf.session_id;
    this->session_version = oinf.session_version;
    return *this;
}

int	sdp_owner_info::operator==(const sdp_owner_info & other)
{
    if( address == other.address )
	{
		if( this->address_type == other.address_type)
		{
			if( this->name == other.name )
			{
				if( this->network_type == other.network_type )
				{
					if( this->session_id == other.session_id )
					{
						if( this->session_version == other.session_version )
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
int	sdp_owner_info::operator>(const sdp_owner_info & other)
{
    if( address > other.address )
	{
		if( this->address_type > other.address_type)
		{
			if( this->name > other.name )
			{
				if( this->network_type > other.network_type )
				{
					if( this->session_id > other.session_id )
					{
						if( this->session_version > other.session_version )
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
int	sdp_owner_info::operator<(const sdp_owner_info & other)
{
    if( address < other.address )
	{
		if( this->address_type < other.address_type)
		{
			if( this->name < other.name )
			{
				if( this->network_type < other.network_type )
				{
					if( this->session_id < other.session_id )
					{
						if( this->session_version < other.session_version )
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

VOID sdp_owner_info::init()
{
    this->session_id = 0;
    this->session_version = 0;
    this->address_type = e_addr_type_no;
    this->network_type = e_net_type_no;
}

VOID sdp_owner_info::free()
{
    this->address="";
}

/*
    CMSTR        name;
    int            session_id;
    int            session_version;
    e_net_type    network_type;
    e_addr_type    address_type;
    CMSTR        address;
*/
PBYTE sdp_owner_info::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    C c;
    typedef PlusRepeat<Or<DIGIT,Char<'+'>,Char<'-'>,Char<'*'> > > name_str;
    typedef PlusRepeat<DIGIT > int_str;
    typedef And<CHAR_I,CHAR_N > in_str;
    typedef And<CHAR_O,CHAR_U,CHAR_T > out_str;
    typedef And<CHAR_I,CHAR_P,CHAR_4 > ip4_str;
    typedef And<CHAR_I,CHAR_P,CHAR_6 > ip6_str;
    typedef And<CHAR_O,CHAR_T,CHAR_H,CHAR_E,CHAR_R > other_str;
    typedef FindRepeat<name_str > fName;
    typedef FindRepeat<int_str > fInt;
    typedef FindRepeat<in_str> fIn;
    typedef FindRepeat<out_str> fOut;
    typedef FindRepeat<ip4_str> fIp4;
    typedef FindRepeat<ip6_str > fIp6;
    typedef FindRepeat<other_str > fOther;
    typedef FindRepeat<hostaddr1 > fHost;
    
    tmp = inStr;

    if( fName::Matches(tmp,&c,start) )
    {
		this->name = "";
		this->name.append((char*)start,((int)(tmp-start) ));
    }

    if( fInt::Matches(tmp,&c,start) )
    {
        this->session_id = (unsigned int)atoi((PCHAR)start);
    }

    if( fInt::Matches(tmp,&c,start) )
    {
        this->session_version = (unsigned int)atoi((PCHAR)start);
    }
    if( fIn::Matches(tmp,&c,start))
    {
        this->network_type = e_net_type_in;
    }
    else if( fOut::Matches(tmp,&c,start))
    {
        this->network_type = e_net_type_out;
    }
    
    if( fIp4::Matches(tmp,&c,start))
    {
        this->address_type = e_addr_type_ip4;
    }
    else if( fIp6::Matches(tmp,&c,start))
    {
        this->address_type = e_addr_type_ip6;
    }
    else if( fOther::Matches(tmp,&c,start))
    {
        this->address_type = e_addr_type_oth;
    }
    
    if( fHost::Matches(tmp,&c,start))
    {
		this->address = "";
		this->address.append((char*)start,((int)(tmp-start)));
    }
    return tmp;
}

int  sdp_owner_info::print(OUT PBYTE outStr)
{
    if( outStr )
    {
		if( !this->address.empty() && !this->name.empty() && this->session_id > 0 && this->session_version>0 )
        {
			sprintf((PCHAR)outStr,"o=%s %u %u ",
				(!name.empty()?name.data():""),
				this->session_id, this->session_version);
            if( this->network_type == e_net_type_in )
                strcat((PCHAR)outStr,"IN ");
            else if( this->network_type == e_net_type_out )
                strcat((PCHAR)outStr,"OUT ");
            if( this->address_type == e_addr_type_ip4 )
                strcat((PCHAR)outStr,"IP4");
            else if( this->address_type == e_addr_type_ip6 )
                strcat((PCHAR)outStr,"IP6");
            else if( this->address_type == e_addr_type_oth )
                strcat((PCHAR)outStr,"OTHER");
			strcatfm((PCHAR)outStr,"%s%s\r\n",(!address.empty()?" ":""),
				(!address.empty()?address.data():""));
        }
		return 1;
    }
	return 0;
}

int sdp_owner_info::build(IN char *szName,IN unsigned int s_id,IN unsigned int s_v,IN char *szHost,IN e_net_type nType,IN e_addr_type aType)
{
	this->name = szName;
	this->session_id = s_id;
	this->session_version = s_v;
	this->network_type = nType;
	this->address_type = aType;
	if( !szHost )
		this->address = "0.0.0.0";
	else
		this->address = szHost;
	return 1;
}

sdp_media_description::sdp_media_description()
{
    this->init();
}

sdp_media_description::~sdp_media_description()
{
    this->free();
}


sdp_media_description &sdp_media_description::operator = (const sdp_media_description &md)
{
    this->count = md.count;
    for(int i=0 ; i < MATT_MAX; i++)
        this->format[i] = md.format[i];
    this->port = md.port;
    this->proto = md.proto;
    this->stype = md.stype;
    return *this;
}


int	sdp_media_description::operator==(const sdp_media_description & other)
{
	if( this->port == other.port)
	{
		if( this->proto == other.proto )
		{
			if( this->stype == other.stype )
			{
				if( other.count == this->count )
				{
					if( !memcmp(this->format, other.format, sizeof(int)*count) )
						return 1;
				}
			}
		}
	}
	return 0;
}
int	sdp_media_description::operator>(const sdp_media_description & other)
{
	if( this->port > other.port)
	{
		if( this->proto > other.proto )
		{
			if( this->stype > other.stype )
			{
				if( other.count > this->count )
				{
					return 1;
				}
			}
		}
	}
	return 0;
}
int	sdp_media_description::operator<(const sdp_media_description & other)
{
	if( this->port < other.port)
	{
		if( this->proto < other.proto )
		{
			if( this->stype < other.stype )
			{
				if( other.count < this->count )
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

int sdp_media_description::build(char *_stype, char *_proto, int _port, int _count, int _iFormat, ...)
{
	int c = 0, iFormat = _iFormat;
	va_list iVal;

	memset(this->format,0, sizeof(int)*12);
	//if( count >= 12)
	//	return 0;
	
	this->count = _count;
	this->port = _port;
	this->stype = _stype;
	this->proto = _proto;

	va_start(iVal, _iFormat);
	
	while(iFormat >=0 )
	{
		this->format[c++] = iFormat;
		iFormat = va_arg(iVal, int);
	}
	va_end(iVal);
	return 1;
}

VOID sdp_media_description::init()
{
    this->port = 0;
    this->count = 0;
    for(int i = 0; i< MATT_MAX; i++)
        format[i] = -1;
}

VOID sdp_media_description::free()
{
    this->stype="";
    this->proto="";
}

/*
    CMSTR    stype;
    int        port;
    CMSTR    proto;
    int        count;
    int        format[MATT_MAX];
*/
PBYTE sdp_media_description::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    C c;
    typedef PlusRepeat<ALPHA> type_str;
    typedef And<BLANK, PlusRepeat<DIGIT> > num_str;
    typedef And<BLANK, PlusRepeat<Or<ALPHA,SLASH> > > proto_str;
    tmp = inStr;
    start = tmp;
    if( type_str::Matches(tmp,&c) )
    {
		stype="";
		stype.append((char*)start, (int)(tmp-start));
    }
    start = tmp;
    if( num_str::Matches(tmp, &c) )
    {
        port = atoi((PCHAR)(start+1));
    }
    start = tmp;
    if( proto_str::Matches(tmp, &c ) )
    {
		proto = "";
		proto.append((char*)start+1, (int)(tmp-start-1));
    }
    start = tmp;
    this->count = 0;
    while( num_str::Matches(tmp,&c) )
    {
        this->format[count++] = atoi((PCHAR)(start+1));
        start = tmp;
    }
    return tmp;
}

int	 sdp_media_description::HaveMyMedia(int iMyMedia)
{
	for( int i = 0; i < MATT_MAX; i++)
	{
		if( this->format[i] == iMyMedia )
			return 1;
	}
	return 0;
}

int sdp_media_description::IsAudio()
{
	if( !this->stype.compare("audio") ||
		!this->stype.compare("AUDIO") )
		return 1;
	if( !this->stype.compare("Audio") )
		return 1;
	return 0;
}
int  sdp_media_description::print(OUT PBYTE outStr)
{
    if( outStr )
    {
		if( !this->stype.empty() )
        {
			if( !this->proto.empty() )
				sprintf((PCHAR)outStr,"m=%s %d %s",this->stype.data(), this->port, this->proto.data());
			else //if( this->proto.empty() )
				sprintf((PCHAR)outStr,"m=%s %d",this->stype.data(), this->port);
			//else if( !this->proto.empty() )
			//	sprintf((PCHAR)outStr,"m=%s %d %s",this->stype.data(), this->proto.data());
            for(int i =0; i< this->count ; i++)
            {
                if( this->format[i] >= 0 )
                    strcatfm((PCHAR)outStr, " %d", this->format[i]);
            }
            strcat((PCHAR)outStr, "\r\n");
        }
		return 1;
    }
	return 0;
}

sdp_media_info::sdp_media_info()
{
    this->init();
}

sdp_media_info::~sdp_media_info()
{
    this->free();
}

VOID sdp_media_info::free()
{
	//bandWidthInfo.f
#if 0
	this->connection.init();
#endif
    //this->m_desc.free();
	if( m_att.size() > 0 )
		m_att.erase(m_att.begin(),m_att.end());
}

VOID sdp_media_info::init()
{
#if 0
	this->connection.init();
#endif
    this->m_desc.init();
	if( m_att.size() > 0 )
		m_att.erase(m_att.begin(),m_att.end());
}

sdp_media_info & sdp_media_info::operator = (const sdp_media_info & smi)
{
	free();
	sdp_media_info *pSMI=(sdp_media_info *)&smi;
    this->m_desc = smi.m_desc;
	vector<
#if defined LINUX || defined MACOS
		sdp_media_att
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<sdp_media_att>
#elif defined WINCE
		sdp_media_att
#endif
#endif
		>::iterator f_f;
	if( this->m_att.size() > 0 )
	{
		m_att.erase(m_att.begin(), m_att.end());
	}
	for(f_f=pSMI->m_att.begin();f_f!=pSMI->m_att.end(); f_f++)
	{
		//sdp_media_att *pMAtt;
#if defined LINUX || defined MACOS
		sdp_media_att * pMAtt = new sdp_media_att;
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<sdp_media_att> pMAtt(new sdp_media_att);
#elif defined WINCE
		sdp_media_att * pMAtt = new sdp_media_att;
#endif
#endif
		pMAtt->init();
#if defined LINUX || defined MACOS
		*pMAtt = *f_f;
		m_att.push_back(*pMAtt);
#elif defined WINCE || defined WIN32
#ifdef WIN32
		*pMAtt = *f_f->get();
		m_att.push_back(pMAtt);
#elif defined WINCE
		*pMAtt = *f_f;
		m_att.push_back(*pMAtt);
#endif
#endif
	}
	this->bandWidthInfo = smi.bandWidthInfo;
	this->connection = smi.connection;
	return *this;
}

int	sdp_media_info::operator==(const sdp_media_info &other)
{
	if( this->m_att.size() == other.m_att.size() )
	{
		if( this->m_desc == other.m_desc )
		{
			sdp_media_info *pOther= (sdp_media_info *)&other;
#if defined LINUX || defined MACOS
			vector<sdp_media_att>::iterator c;
			vector<sdp_media_att>::iterator b;
#elif defined WINCE || defined WIN32
#ifdef WIN32
			vector<tr1::shared_ptr<sdp_media_att>>::iterator c;
			vector<tr1::shared_ptr<sdp_media_att>>::iterator b;
#elif defined WINCE
			vector<sdp_media_att>::iterator c;
			vector<sdp_media_att>::iterator b;
#endif
#endif
			c = m_att.begin();
			b = pOther->m_att.begin();
			for(;c != this->m_att.end(); c++, b++)
			{
#if defined LINUX || defined MACOS
				if( *c == *b )
#elif defined WINCE || defined WIN32
#ifdef WIN32
				if( *c->get() == *b->get() )
#elif defined WINCE
				if( *c == *b )
#endif
#endif
				{
					return 1;
				}
			}
			return 0;
		}
	}
	return 0;
}
int	sdp_media_info::operator>(const sdp_media_info & other)
{
	if( this->m_att.size() > other.m_att.size() )
	{
		return 1;
	}
	return 0;
}
int	sdp_media_info::operator<(const sdp_media_info & other)
{
	if( this->m_att.size() < other.m_att.size() )
	{
		return 1;
	}
	return 0;
}

BOOL sdp_media_info::HaveCodec(int iCodec)
{
	if( this->m_desc.count > 0 )
	{
		for( int i = 0; i < m_desc.count; i++ )
		{
			if( m_desc.format[i] == iCodec )
				return TRUE;
		}
	}
	return FALSE;
}

int sdp_media_info::print(OUT PBYTE outStr)
{
    if( outStr )
    {
		char tBuff[256];

	    memset(tBuff,0, 256);
	    this->m_desc.print((PBYTE)&tBuff);
		strcat((PCHAR)outStr, tBuff);
		// connetion info
        memset(tBuff, 0, 256);
        if( connection.print((PBYTE)&tBuff) >0)
			strcat((PCHAR)outStr,tBuff);
		
		// BandWidth Information
		memset(tBuff, 0, 256);
		if( this->bandWidthInfo.print((PBYTE)&tBuff) > 0 )
		{
	        strcat((PCHAR)outStr,tBuff);
		}
#if defined LINUX || defined MACOS
		vector<sdp_media_att>::iterator f_f;
#elif defined WINCE || defined WIN32
#ifdef WIN32
		vector<tr1::shared_ptr<sdp_media_att>>::iterator f_f;
#elif defined WINCE
		vector<sdp_media_att>::iterator f_f;
#endif
#endif
		for(f_f=m_att.begin();f_f!=m_att.end();f_f++)
		{
		    memset(tBuff,0, 256);
#if defined LINUX || defined MACOS
			f_f->print((PBYTE)&tBuff);
#elif defined WINCE || defined WIN32
#ifdef WIN32
			f_f->get()->print((PBYTE)&tBuff);
#elif defined WINCE
			f_f->print((PBYTE)&tBuff);
#endif
#endif
			strcat((PCHAR)outStr,tBuff);
		}
		return 1;
    }
	return 0;
}
/*
    int						port;
    CMSTR					proto;
    int						count;
    int						format[MATT_MAX];
*/
/*
a=sendonly
a=rtpmap:18 G729/8000/1
a=rtpmap:4 G723/8000/1
a=rtpmap:0 PCMU/8000/1
a=rtpmap:8 PCMA/8000/1
*/
//m=audio 7080 RTP/AVP 18 4 0 8
int sdp_media_info::appendMediaAtt(char *szField,int iFormat,char *szType)
{
#if defined LINUX || defined MACOS
	sdp_media_att *pMAtt = new sdp_media_att;
#elif defined WIN32 || defined WINCE
#ifdef WIN32
	tr1::shared_ptr<sdp_media_att> pMAtt(new sdp_media_att);
#elif defined WINCE
	sdp_media_att *pMAtt = new sdp_media_att;
#endif
#endif
	pMAtt->init();
	pMAtt->fieldname = szField;
	if( szType )
		pMAtt->type = szType;
	pMAtt->format = iFormat;
#if defined LINUX || defined MACOS
	m_att.push_back(*pMAtt);
#elif defined WINCE || defined WIN32
#ifdef WIN32
	m_att.push_back(pMAtt);
#elif defined WINCE
	m_att.push_back(*pMAtt);
#endif
#endif
	return 1;
}

void sdp_media_info::InsertMATT(sdp_media_att *pMAtt)
{
	if( pMAtt )
	{
		int iFind=0;
		if( this->m_att.size() == 0 )
		{
#if defined LINUX || defined MACOS
			m_att.push_back(*pMAtt);
#elif defined WIN32 || defined WINCE
#ifdef WIN32
			tr1::shared_ptr<sdp_media_att> p(pMAtt);
			m_att.push_back(p);
#elif defined WINCE
			m_att.push_back(*pMAtt);
#endif
#endif
		}
		else
		{
#if defined LINUX || defined MACOS
			vector<sdp_media_att>::iterator c =
#elif defined WINCE || defined WIN32
#ifdef WIN32
			vector<shared_ptr<sdp_media_att>>::iterator c =
#elif defined WINCE
			vector<sdp_media_att>::iterator c =
#endif
#endif
#if defined LINUX || defined MACOS
									find(m_att.begin(),
											m_att.end(), *pMAtt );
#elif defined WINCE || defined WIN32
#ifdef WIN32
									find_if(m_att.begin(),
											m_att.end(),
											tr1::bind( equal_to<string>(),
											          tr1::bind(&sdp_media_att::fieldname, tr1::placeholders::_1),
													  pMAtt->fieldname) );
#elif defined WINCE
									find(m_att.begin(),
											m_att.end(), *pMAtt );
#endif
#endif
			if( c == m_att.end() )
			{
#if defined LINUX || defined MACOS
				m_att.push_back(*pMAtt);
#elif defined WINCE || defined WIN32
#ifdef WIN32
				tr1::shared_ptr<sdp_media_att> p(pMAtt);
				m_att.push_back(p);
#elif WINCE
				m_att.push_back(*pMAtt);
#endif
#endif
			}
		}
	}
}

sdp_bandwidth_info::sdp_bandwidth_info()
{
}

sdp_bandwidth_info::~sdp_bandwidth_info()
{
}

sdp_bandwidth_info &sdp_bandwidth_info::operator=(const sdp_bandwidth_info & sbi )
{
	this->Value = sbi.Value;
	this->Modifier = sbi.Modifier;
	return *this;
}

int	sdp_bandwidth_info::operator==(const sdp_bandwidth_info& other)
{
	if( !this->Modifier.compare(other.Modifier))
	{
		if( !this->Value.compare(other.Value))
			return 1;
	}
	return 0;
}

int sdp_bandwidth_info::operator>(const sdp_bandwidth_info& other)
{
	if( this->Modifier.compare(other.Modifier)>0)
	{
		if( this->Value.compare(other.Value)>0)
			return 1;
	}
	return 0;
}

int sdp_bandwidth_info::operator<(const sdp_bandwidth_info& other)
{
	if( this->Modifier.compare(other.Modifier)<0)
	{
		if( this->Value.compare(other.Value)<0)
			return 1;
	}
	return 0;
}

int sdp_bandwidth_info::print(OUT PBYTE outStr)
{
	if( this->Modifier.size() > 0 && this->Value.size() >0)
	{
		sprintf((char*)outStr, "b=%s:%s\r\n", (char*)Modifier.c_str(), (char*)Value.c_str());
		return 1;
	}
	return 0;
}


sip_sdp::sip_sdp()
{
    this->init();
}

sip_sdp::~sip_sdp()
{
    this->free();
}
VOID sip_sdp::init()
{
    this->version = 0;
    this->owner.init();
    this->connection.init();
	this->time.init();
	vector<
#if defined LINUX || defined MACOS
		SDPMEDIAINF
#elif defined WINCE || defined WIN32
#ifdef WIN32
		shared_ptr<SDPMEDIAINF>
#elif defined WINCE
		SDPMEDIAINF
#endif
#endif
		>::iterator f_f;
	if( media.size () > 0 )
	{
		for(f_f=media.begin();f_f!=media.end();f_f++)
		{
#if defined LINUX || defined MACOS
			f_f->free();
#elif defined WINCE || defined WIN32
#ifdef WIN32
			f_f->get()->free();
#elif defined WINCE
			f_f->free();
#endif
#endif
		}
	}
	media.erase(media.begin(),media.end());
    this->dtmf_session = 0;
    session_name = "";
}

VOID sip_sdp::free()
{
	init();
}

typedef Or<unreserved, reserved, ALPHA, DIGIT, special> sdp_chr;
typedef PlusRepeat<sdp_chr > sdp_str;
typedef And<ALPHA,sdp_str, cnrl > sdp_line;
typedef And<CHAR_V,EQUAL > sdp_version;
typedef And<CHAR_O,EQUAL > sdp_owner;
typedef And<CHAR_S,EQUAL > sdp_subject;
typedef And<CHAR_C,EQUAL > sdp_connection;
typedef And<CHAR_T,EQUAL > sdp_time;
typedef And<CHAR_M,EQUAL > sdp_media;
typedef And<CHAR_A,EQUAL > sdp_att;
typedef And<CHAR_B,EQUAL > sdp_bi;

PBYTE sip_sdp::parse(IN PBYTE inStr)
{
    PBYTE tmp,start;
    tmp = inStr;
    C c;
    
	typedef FindRepeat<sdp_line > fLine;
    while(fLine::Matches(tmp,&c,start) )
    {
        unsigned char tbuff[256];
		typedef FindRepeat< cnrl > endOfLine;
        PBYTE tbuff1,endPtr,endStart;
        int len,lenofendline;
        tbuff1 = &tbuff[0];
		endPtr = start;
		if( endOfLine::Matches( endPtr, &c, endStart))
			lenofendline = (int )(endPtr - endStart);
		else
			lenofendline = 2;
		len = (int)(tmp-start-lenofendline);
		memset(tbuff1, 0, 256);
        memcpy(tbuff1, start, len);
        
		if( sdp_version::Matches(tbuff1, &c))
        {
            version = atoi((PCHAR)tbuff1);
        }
        else if( sdp_owner::Matches(tbuff1, &c))
        {
            owner.parse(tbuff1);
        }
        else if( sdp_subject::Matches(tbuff1, &c))
        {
			session_name = "";
			session_name.append((char*)tbuff1, len-2);
        }
        else if( sdp_connection::Matches(tbuff1, &c))
        {
			if( media.size() == 0 )
			{
				this->connection.parse(tbuff1);
			}
			else
			{
				vector<
#if defined LINUX || defined MACOS
					SDPMEDIAINF
#elif defined WINCE || defined WIN32
#ifdef WIN32
					shared_ptr<SDPMEDIAINF>
#elif defined WINCE
					SDPMEDIAINF
#endif
#endif
						>::reference pMedia = this->media.back();
#if defined LINUX || defined MACOS
				if( pMedia.connection.isEmpty() ) pMedia.connection.parse(tbuff1);
#elif defined WINCE || defined WIN32
#ifdef WIN32
				if( pMedia->connection.isEmpty() ) pMedia->connection.parse(tbuff1);
#elif defined WINCE
				if( pMedia.connection.isEmpty() ) pMedia.connection.parse(tbuff1);
#endif
#endif
			}
        }
        else if( sdp_time::Matches(tbuff1, &c))
        {
            time.parse(tbuff1);
        }
        else if( sdp_media::Matches(tbuff1, &c))
        {
#if defined LINUX || defined MACOS
			SDPMEDIAINF * pMedia = new SDPMEDIAINF;
#elif defined WINCE || defined WIN32
#ifdef WIN32
			tr1::shared_ptr<SDPMEDIAINF> pMedia(new SDPMEDIAINF);
#elif defined WINCE
			SDPMEDIAINF * pMedia = new SDPMEDIAINF;
#endif
#endif
			if( pMedia->m_desc.parse(tbuff1) )
				media.push_back(
#if defined LINUX || defined MACOS
							*pMedia
#elif defined WINCE || defined WIN32
#ifdef WIN32
							pMedia
#elif defined WINCE
							*pMedia
#endif
#endif
							);
        }
		else if( sdp_att::Matches(tbuff1, &c))
		{
			if( media.size() > 0 )
			{
				vector<
#if defined LINUX || defined MACOS
					SDPMEDIAINF
#elif defined WINCE || defined WIN32
#ifdef WIN32
					shared_ptr<SDPMEDIAINF>
#elif defined WINCE
					SDPMEDIAINF
#endif
#endif
					>::reference pMedia = this->media.back();
#if defined LINUX || defined MACOS
				sdp_media_att *pMAtt = new sdp_media_att;
#elif defined WINCE || defined WIN32
#ifdef WIN32
				tr1::shared_ptr<sdp_media_att> pMAtt(new sdp_media_att);
#elif defined WINCE
				sdp_media_att *pMAtt = new sdp_media_att;
#endif
#endif
				pMAtt->init();
				if( pMAtt->parse(tbuff1) )
				{
#if defined LINUX || defined MACOS
					pMedia.m_att.push_back(*pMAtt);
#elif defined WINCE || defined WIN32
#ifdef WIN32
					pMedia->m_att.push_back(pMAtt);
#elif defined WINCE
					pMedia.m_att.push_back(*pMAtt);
#endif
#endif
				}
			}
		}
		else if( sdp_bi::Matches(tbuff1, &c))
		{
			char *sep = (char*)strstr((char*)tbuff1, ":");
			if( sep )
			{
				vector<
#if defined LINUX || defined MACOS
					SDPMEDIAINF
#elif defined WINCE || defined WIN32
#ifdef WIN32
					shared_ptr<SDPMEDIAINF>
#elif defined WINCE
					SDPMEDIAINF
#endif
#endif
						>::reference pMedia = this->media.back();
				char tbuf[128];
				memset(tbuf,0, 128);
				memcpy(tbuf, (char*)(tbuff1), (sep-(char*)tbuff1));
#if defined LINUX || defined MACOS
				pMedia.bandWidthInfo.Modifier = tbuf;
				pMedia.bandWidthInfo.Value = ++sep;
#elif defined WIN32 || defined WINCE
#ifdef WIN32
				pMedia->bandWidthInfo.Modifier = tbuf;
				pMedia->bandWidthInfo.Value = ++sep;
#elif defined WINCE
				pMedia.bandWidthInfo.Modifier = tbuf;
				pMedia.bandWidthInfo.Value = ++sep;
#endif
#endif
			}
		}
    }
    return tmp;
}

int sip_sdp::print(OUT PBYTE outStr)
{
    if( outStr )
    {
		vector<
#if defined LINUX || defined MACOS
			SDPMEDIAINF
#elif defined WINCE || defined WIN32
#if defined WIN32
			shared_ptr<SDPMEDIAINF>
#elif defined WINCE
			SDPMEDIAINF
#endif
#endif
				>::iterator f_f;
		
        char tbuf[1024];
        sprintf((PCHAR)outStr,"v=%d\r\n",this->version);
        
        //owner
        memset(tbuf, 0, 1024);
        if( owner.print((PBYTE)&tbuf) )
			strcat((PCHAR)outStr,tbuf);
        
        // session name
		if( !session_name.empty())
			strcatfm((PCHAR)outStr,"s=%s\r\n",session_name.data());

		// connection
		memset(tbuf, 0, 1024);
		if( connection.print((PBYTE)&tbuf))
			strcat((PCHAR)outStr, tbuf);
		
        // time
        memset(tbuf, 0, 1024);
        time.print((PBYTE)&tbuf);
        strcat((PCHAR)outStr,tbuf);
		// media info
		for(f_f = media.begin();f_f!=media.end();f_f++)
		{
            memset(tbuf, 0, 1024);
#if defined LINUX || defined MACOS
			f_f->print((PBYTE)&tbuf);
#elif defined WINCE || defined WIN32
#ifdef WIN32
			f_f->get()->print((PBYTE)&tbuf);
#elif defined WINCE
			f_f->print((PBYTE)&tbuf);
#endif
#endif
            strcat((PCHAR)outStr,tbuf);
			//if( f_f->get()->m_desc.IsAudio() )
			//{
		     //   // time
			//	memset(tbuf, 0, 1024);
			//	time.print((PBYTE)&tbuf);
			//	strcat((PCHAR)outStr,tbuf);
			//}
		}
		return 1;
    }
	return 0;
}

sip_sdp & sip_sdp::operator=(const sip_sdp &sdp)
{
	sip_sdp * pSDP = (sip_sdp *)&sdp;
    this->dtmf_session = sdp.dtmf_session;
	vector<
#if defined LINUX || defined MACOS
		SDPMEDIAINF
#elif defined WINCE || defined WIN32
#if defined WIN32
		shared_ptr<SDPMEDIAINF>
#elif defined WINCE
		SDPMEDIAINF
#endif
#endif
			>::iterator f_f;
	for(f_f=pSDP->media.begin();f_f!=pSDP->media.end(); f_f++)
	{
#if defined LINUX || defined MACOS
		SDPMEDIAINF *pMI = new SDPMEDIAINF;
#elif defined WINCE || defined WIN32
#ifdef WIN32
		tr1::shared_ptr<SDPMEDIAINF> pMI(new SDPMEDIAINF);
#elif defined WINCE
		SDPMEDIAINF *pMI = new SDPMEDIAINF;
#endif
#endif
		pMI->init();
#if defined LINUX || defined MACOS
		*pMI = *f_f;
		media.push_back(*pMI);
#elif defined WINCE || defined WIN32
#ifdef WIN32
		*pMI = *f_f->get();
		media.push_back(pMI);
#elif defined WINCE
		*pMI = *f_f;
		media.push_back(*pMI);
#endif
#endif
	}
	//this->bandWidthInfo = sdp.bandWidthInfo;
    this->owner = sdp.owner;
    this->session_name = sdp.session_name;
    this->time = sdp.time;
    this->version = sdp.version;
	this->connection = sdp.connection;
    return *this;
}

int sip_sdp::operator==(const sip_sdp sdp)
{
	if( this->session_name == sdp.session_name )
	{
		if( this->version == sdp.version)
		{
			if( this->dtmf_session == sdp.dtmf_session )
			{
				if( this->owner == sdp.owner )
				{
					if( this->time == sdp.time )
					{
						return 1;
					}
				}
			}
		}
	}
    return 0;
}

int sip_sdp::operator <(const sip_sdp sdp)
{
	if( this->session_name < sdp.session_name )
	{
		if( this->owner < sdp.owner )
		{
			if( this->time < sdp.time)
			{
				return 1;
			}
		}
	}
    return 0;
}

int sip_sdp::operator >(const sip_sdp sdp)
{
	if( this->session_name > sdp.session_name )
	{
		if( this->owner > sdp.owner )
		{
			if( this->time > sdp.time)
			{
				return 1;
			}
		}
	}
    return 0;
}

int sip_sdp::equal(PSDPOWNERINF powner)
{
    return 0;
}

/*
    CMSTR					stype;
    int						port;
    CMSTR					proto;
*/
/*
m=audio 7080 RTP/AVP 18 4 0 8
a=sendonly
a=rtpmap:18 G729/8000/1
a=rtpmap:4 G723/8000/1
a=rtpmap:0 PCMU/8000/1
a=rtpmap:8 PCMA/8000/1
*/

PBYTE parse_sdp(IN PBYTE inStr,OUT PVOID _pSdp)
{
    PSIPSDP pSdp = (PSIPSDP)_pSdp;
    if( inStr <= NULL ||  _pSdp <= NULL )
        return inStr;
    return pSdp->parse(inStr);
}

int  print_sdp(IN PVOID _pSdp,OUT PBYTE outStr)
{
   PSIPSDP pSdp = (PSIPSDP)_pSdp;
   if( !pSdp )
       return 0;
   return pSdp->print(outStr);
}

/////////////////////////////////////////////////////////////
// end of file
//
