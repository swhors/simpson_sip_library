/////////////////////////////////////////////////
// File : SkyAsyncSock.h
// 설명 : Async 소켓을 클래스화 한 것.
// 날짜 : 2005/09/05
//

#pragma once

#ifndef __SKY_ASYNC_SOCK_H
#define __SKY_ASYNC_SOCK_H

class SKYLIB CSkyAsyncSock : public CSkySock
{
public:
	// Function ..
	CSkyAsyncSock();
	CSkyAsyncSock(char *sip,int port,int cType);
	CSkyAsyncSock(int cType);
	~CSkyAsyncSock();
	int Create();
	int Create(int port=5060,int iBind=0);
	int Create(int port,char *dIP,int dPort);
	int Accept(CSkyAsyncSock * pSock);
    int Accept(int *Handle);
	int Connect(char *ip,int iport);
    void Close();
	virtual void ReadEvent(int iVal);
	virtual	void AcceptEvent(int iVal);
public:	// Variable ..
	int  close_count;
	int       asyncflag;
	int  stype; // 0 = server, 1 = client
    SKYHANDLE asynchandle;
	//SKYTHREAD rcv_data(SKYINVAL pArg);
	void init_variable();
	void sock_block(){block = 1;};
	void sock_unblock(){block = 0;};
	int  get_block(){return block;};
private:
    int  block;
};

#endif // __SKY_ASYNC_SOCK_H
