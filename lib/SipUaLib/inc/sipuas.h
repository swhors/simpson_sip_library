#pragma once

#ifndef __SIPUAS_H
#define __SIPUAS_H

#include "SkyAsyncSock.h"

class SKYLIB CSipuas : public CSkyAsyncSock
{
public:
	CSipuas(void *pMan);
	CSipuas(void *pMan,int _port,int _ctype);
	~CSipuas();
	virtual void ReadEvent(int iVal);
private:
	void *pManager;
};

#endif // __SIPUAS_H