#pragma once

class SKYLIB CSkySock
{
public:
	CSkySock();
	CSkySock(IN char *sIP,IN int sPort);
	CSkySock(IN char *sIP,IN int sPort,IN char *dIP,IN int dPort,IN int cType);
	virtual ~CSkySock();

// Socket Function
	// Create Socket
	int create_udp();
	int create_udp(IN int sPort,IN int iBin=0);
	int create_udp(IN char *sIP,IN int sPort);
	int create_tcp();
	int create_tcp(IN int sPort);
	int create_tcp(IN char *sIP,IN int sPort);
	int create_tcp(IN char *sIP,IN int sPort,IN char *dIP,IN int dPort);

	// Close Socket
	void close_handle();

	// Send Data
	int send_tcp(IN char *dbuff,IN int len);
	int send_udp(IN char *dbuff,IN int len);
	int send_udp(IN char *dbuff,IN int len,IN char *dIP,IN int dPort);

	// Recv Data
	int recv_tcp(OUT char *dbuff,OUT int *len);
	int recv_udp(OUT char *dbuff,OUT int *len);
	int recv_udp(OUT char *dbuff,OUT int *len,IN int ttl);
	int recv_udp(OUT char *dbuff,OUT int *len,OUT char *sIP,OUT int *sPort);
	int recv_udp(OUT char *dbuff,OUT int *len,OUT char *sIP,OUT int *sPort,IN int ttl);

	// accept
	// 클라이언트의 접속 요청을 허용함.
	int accept_tcp();
	// connect
	// 서버로 연결 요청을 함.
	int connect_tcp(IN char *dIP,IN int dPort);

	// socket info
	int getSockName(OUT char *IP,IN int iplen,OUT int *port);
	int getPeerName(OUT char *IP,IN int iplen,OUT int *port);
		
// Variable's function
	
	char *get_dip(){return &dip[0];};
	void set_dip(IN char *sVal);
	
	int get_dport(){return dport;};
	void set_dport(IN int iVal){dport = iVal;};
	
	char *get_sip(){return &sip[0];};
	void set_sip(IN char *sVal);

	int get_sport(){return sport;};
	void set_sport(IN int iVal){sport = iVal;};
	
	int get_tr_ctype(){return ctype;};
	void set_tr_ctype(IN int iVal){ctype = iVal;};

	int get_sockHandle(){return sockHandle;};
public: // variable
	int ctype;     // udp or tcp
	void bzeroVal();    
	char dip[24]; // other ip
	char sip[24]; // created ip
	int dport;    // other  port
	int sport;    // created port port
	int sockHandle;
private:
	// Internal
};

