#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;
#include <time.h>

#include "skysip.h"

#include "SipCallStack.h"

void MemStateLog(char *szTxt1, int iVal = -1, char *szTxt2= NULL );


CSipCallStack::CSipCallStack(void)
{
	this->pSipHd = NULL;
	this->eState = e_msg_st_none;
	this->cid.empty();
	startTime = 0;
	retrialCount = 0;
	bDelete = false;
	bReqAnswer = false;
	pManager = NULL;
#ifdef WIN32
	hThreadHandle= NULL;
	dwThreadID = 0;
	hWakeEvent = NULL;
	hWakeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
	MemStateLog("CSipCallStack", -1, "new type1");
}


CSipCallStack::CSipCallStack(sip_hd_t * _pSipHd)
{
	this->pSipHd = _pSipHd;
	this->eState = e_msg_st_sd_request;
	this->cid = _pSipHd->sipfield.cid.callid;
	startTime = time(&startTime);
	retrialCount = 0;
	bDelete = false;
	pManager = NULL;
	szFromTagInformation = _pSipHd->sipfield.from.tag;
	this->seqNumber = pSipHd->sipfield.cseq.cseq *100 + pSipHd->sipfield.cseq.type;
#ifdef WIN32
	hThreadHandle= NULL;
	dwThreadID = 0;
	hWakeEvent = NULL;
	hWakeEvent = CreateEvent(NULL, TRUE, FALSE, (char*)_pSipHd->sipfield.cid.callid.data());
#endif
	MemStateLog("CSipCallStack", -1, "new type2");
}

CSipCallStack::~CSipCallStack(void)
{
	if( cid.length() > 0 )
		cid.erase(cid.begin(), cid.end());
	this->eState = e_msg_st_none;
	bDelete = false;
	if( this->pSipHd )
		delete pSipHd;
	pSipHd = NULL;
#ifdef WIN32
	dwThreadID = 0;
	if( hWakeEvent )
		CloseHandle(hWakeEvent);
	hWakeEvent = NULL;
	if( hThreadHandle )
		CloseHandle(hThreadHandle);
	hThreadHandle = NULL;
#endif
	MemStateLog("CSipCallStack", -1, "delete");
}

CSipCallStack &CSipCallStack::operator =(const CSipCallStack &other)
{
	this->cid = other.cid;
	this->pSipHd = other.pSipHd;
	this->eState = other.eState;
	this->startTime = other.startTime;
	this->retrialCount = other.retrialCount;
	this->bDelete = other.bDelete;
	this->bReqAnswer = other.bReqAnswer;
	szFromTagInformation = other.szFromTagInformation;
#ifdef WIN32
	if( hWakeEvent )
		CloseHandle(hWakeEvent);
	hWakeEvent = other.hWakeEvent;
#endif
	return *this;
}

int CSipCallStack::operator <(const string &_cid)
{
	if( this->cid.compare(_cid ) < 0)
		return 1;
	return 0;
}

int CSipCallStack::operator >(const string &_cid)
{
	if( this->cid.compare(_cid ) > 0)
		return 1;
	return 0;
}

int CSipCallStack::operator ==(const string &_cid)
{
	if( this->cid.compare(_cid ) == 0)
		return 1;
	return 0;
}

int CSipCallStack::operator <(const char* _cid)
{
	if( this->cid.compare(_cid ) < 0)
		return 1;
	return 0;
}

int CSipCallStack::operator >(const char* _cid)
{
	if( this->cid.compare(_cid ) > 0)
		return 1;
	return 0;
}

int CSipCallStack::operator ==(const char* _cid)
{
	if( this->cid.compare(_cid ) == 0)
		return 1;
	return 0;
}

int CSipCallStack::operator ==(const int iSeqNumber)
{
	if( this->seqNumber == iSeqNumber)
		return 1;
	return 0;
}