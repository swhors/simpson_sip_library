#pragma once

#ifndef __SIP_UAC_H
#define __SIP_UAC_H

//#include "SkyAsyncSock.h"

class SKYLIB CSipuac : public CSkyAsyncSock
{
public:
	CSipuac(void *pMan,int _localPort=5060,int _proxyPort=5060, int _ctype=SKY_SOCK_UDP);
    ~CSipuac();
	int Create(IN int iBind=0);
    int send_msg(IN PVOID _pHD,IN int www_auth);
    int send_msg(IN PVOID _pHD,IN PVOID _pSdp,IN int www_auth);
	int send_ok(IN PVOID _pHD);
	int send_ok(IN PVOID _pHD,IN PVOID _pSdp);
	int make_msg(IN PVOID _pHD,
                 IN PVOID _pSdp,
				 IN int www_auth,
                 OUT unsigned char *outmsg,
                 IN int *buflen,
                 IN int sdp_flag);
	int send_ringing(IN PVOID _pHD);
	int send_trying(IN PVOID _pHD);
	int send_fail(IN PVOID _pHD,IN int err_code);
	int send_bye(IN PVOID _pHD);
	int send_ack(IN PVOID _pHD);
	void ReadEvent(int iVal);
	void uac_block(){sock_block();};
	void uac_unblock(){sock_unblock();};
private:
	void *pManager;
};

#endif // __SIPUAC_H