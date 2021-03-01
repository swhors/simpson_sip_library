#ifndef sip_sdp_H
#define sip_sdp_H
#include <vector>

#define MATT_MAX	12

typedef enum _e_net_type
{
	e_net_type_no = 0,
	e_net_type_in,
	e_net_type_out
}e_net_type;

typedef enum _e_addr_type
{
	e_addr_type_no = 0,
	e_addr_type_ip4,
	e_addr_type_ip6,
	e_addr_type_oth
}e_addr_type;


class sdp_bandwidth_info
{
public:
	sdp_bandwidth_info();
	~sdp_bandwidth_info();
	string Modifier;
	string Value;
	sdp_bandwidth_info &operator=(const sdp_bandwidth_info & sbi );
	int	operator==(const sdp_bandwidth_info& other);
	int operator>(const sdp_bandwidth_info& other);
	int operator<(const sdp_bandwidth_info& other);

	int print(OUT PBYTE outStr);
};

class sdp_media_att
{
public:
    sdp_media_att();
	~sdp_media_att();
    string					fieldname;
	int						format;
	string					type;
	VOID					init();
	VOID					free();
	int						parse(IN PBYTE inStr);
	int						print(OUT PBYTE outStr);
	// operator
	sdp_media_att			&operator=(const sdp_media_att & matt);
	int	operator==(const sdp_media_att & other);
	int	operator>(const sdp_media_att & other);
	int	operator<(const sdp_media_att & other);

    // friendly function
};

typedef sdp_media_att *p_sdp_media_att;

class sdp_connection_info
{
public:
	BOOL					isEmpty();
	sdp_connection_info();
	~sdp_connection_info();
    e_net_type				networktype;
    e_addr_type				AddressType;
    string					Address;
	VOID					init();
	VOID					free();
	void					build(IN char *szAddr,IN e_net_type nType=e_net_type_in,IN e_addr_type aType=e_addr_type_ip4);
	int						parse(IN PBYTE inStr);
	int						print(OUT PBYTE outStr);
	BOOL					bEmpty;
	// operator
	sdp_connection_info		&operator=(const sdp_connection_info & cinf);
	int	operator==(const sdp_connection_info & other);
	int	operator>(const sdp_connection_info & other);
	int	operator<(const sdp_connection_info & other);
};

typedef sdp_connection_info SDPCONNECTIONINF;
typedef sdp_connection_info *PSDPCONNECTIONINF;

class sdp_time_description
{
public:
	sdp_time_description();
	~sdp_time_description();
    int						start;
    int						stop;
	VOID					init();
	int						parse(IN PBYTE inStr);
	int					print(OUT PBYTE outStr);
	void					build(int _start,int _stop);
	// operator
	sdp_time_description	&operator=(const sdp_time_description & oinf);
	int	operator==(const sdp_time_description & other);
	int	operator>(const sdp_time_description & other);
	int	operator<(const sdp_time_description & other);
};

typedef sdp_time_description SDPTIMEDESC;
typedef sdp_time_description *PSDPTIMEDESC;

class sdp_owner_info
{
public:
	sdp_owner_info();
	~sdp_owner_info();
    string					name;
    unsigned int			session_id;
    unsigned int			session_version;
    e_net_type				network_type;
    e_addr_type				address_type;
    string					address;
	VOID					init();
	VOID					free();
	int						build(IN char *szName,IN unsigned int s_id,
									IN unsigned int s_v,IN char *szHost,
									IN e_net_type nType=e_net_type_in,
									IN e_addr_type aType=e_addr_type_ip4);
	PBYTE					parse(IN PBYTE inStr);
	int						print(OUT PBYTE outStr);
    
	// operator
	sdp_owner_info			&operator=(const sdp_owner_info & oinf);
	int						operator==(const sdp_owner_info & other);
	int						operator>(const sdp_owner_info & other);
	int						operator<(const sdp_owner_info & other);
};

typedef sdp_owner_info SDPOWNERINF;
typedef sdp_owner_info *PSDPOWNERINF;

class sdp_media_description
{
public:
	sdp_media_description();
	~sdp_media_description();
    string					stype;
    int						port;
    string					proto;
    int						count;
    int						format[MATT_MAX];
	VOID					init();
	VOID					free();
	int						HaveMyMedia(int iMyMedia);
	PBYTE					parse(IN PBYTE inStr);
	int						print(OUT PBYTE outStr);
	int						IsAudio();
	int						build(char *_stype, char *_proto, int _port, int _count, int _iFormat, ...);
	
	// operator
	sdp_media_description	&operator=(const sdp_media_description &md);
	int	operator==(const sdp_media_description &other);
	int	operator>(const sdp_media_description &other);
	int	operator<(const sdp_media_description &other);
};

//typedef sdp_media_description SDPMEDIADESC;
typedef sdp_media_description *p_sdp_media_description;

class sdp_media_info
{
public:
    sdp_media_info();
    ~sdp_media_info();
    sdp_media_description	m_desc;
	sdp_bandwidth_info		bandWidthInfo;
	SDPCONNECTIONINF		connection;
	BOOL					HaveCodec(int iCodec);
#ifdef LINUX
#else
#ifndef WINCE
	vector<tr1::shared_ptr<sdp_media_att>>	m_att;
#else
	vector<sdp_media_att>	m_att;
#endif
#endif
    VOID					init();
    VOID					free();
    int						print(OUT PBYTE outStr);
    int						appendMediaAtt(char *szField,int iFormat,char *szType);
    //sdp_media_att				*GetLastMATT();
    void					InsertMATT(IN sdp_media_att *pMAtt=NULL);
    // operator
    sdp_media_info			&operator=(const sdp_media_info & mi);
	int	operator==(const sdp_media_info & other);
	int	operator>(const sdp_media_info & other);
	int	operator<(const sdp_media_info & other);
	//friendly function
};

typedef sdp_media_info SDPMEDIAINF;
typedef sdp_media_info *PSDPMEDIAINF;

/*
v=x
o=x
s=x
c=x
t=x

or
v=x
o=x
s=x
t=x
m=x
c=x
a=x
 :
 :
 :
*/

class sip_sdp
{
public:
	sip_sdp();
	~sip_sdp();
	// operation
	VOID					free();
	PBYTE					parse(IN PBYTE inStr);
	int						print(OUT PBYTE outStr);
	VOID					init();
	int						equal(PSDPOWNERINF owner);
	SDPCONNECTIONINF		connection;
	// variable
	int						version;
	SDPOWNERINF				owner;
	string					session_name;
	SDPTIMEDESC				time;
#ifdef LINUX
#else
#ifndef WINCE
	vector<tr1::shared_ptr<sdp_media_info>>	media;
#else
	vector<sdp_media_info>	media;
#endif
#endif
	
	int						dtmf_session;
	//operator
	int					operator==(const sip_sdp sdp);
	int					operator<(const sip_sdp sdp);
	int					operator>(const sip_sdp sdp);
	sip_sdp				&operator=(const sip_sdp & sdp);
};//}SIPSDP,*PSIPSDP;

typedef sip_sdp SIPSDP;
typedef sip_sdp *PSIPSDP;
typedef sip_sdp sdp_info_t;
typedef sip_sdp * p_sdp_info_t;

PBYTE parse_sdp(IN PBYTE inStr,OUT PVOID _pSdp);
int  print_sdp(IN PVOID _pSdp,OUT PBYTE outStr);

#endif // sip_sdp_H

///////////////////////////////////////////////////////////////////
// end of file..
