#pragma once

#ifndef __CRTP_H
#define __CRTP_H

class SKYLIB CRtp : public CSkyAsyncSock
{
public:
	// function..
	CRtp();
	CRtp(int rtp, int rtcp);
	~CRtp();
	int Create(int port);
	void set_rtpPort(int iVal){rtpPort=iVal;};
	void set_rtcpPort(int iVal){rtcpPort=iVal;};
	int get_rtpPort(){return rtpPort;};
	int get_rtcpPort(){return rtcpPort;};
	virtual void ReadEvent(int iVal);
private:
	int rtpPort;
	int rtcpPort;
};

#endif // __CRTP_H


