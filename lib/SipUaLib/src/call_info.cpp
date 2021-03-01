//#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#include "skysip.h"
#include "sip_parse.h"
#include "sipCallStack.h"
#include "call_info.h"

call_info::call_info(void)
{
	init();
}

call_info::~call_info(void)
{
	init();
}

void	call_info::init()
{
	eCallDir = e_call_dir_request;
	eDialogState = e_dialog_start;

    req_eCallState	= e_msg_st_none; // call state
	req_msgType		= 0;
	bUdpSession		= true;
    RTPport=0; // RTP 
	RTPportToSend=0; // RTP 
	memset(RTPipToSend, 0, 24);
	memset(this->RTPip, 0, 24);
	target_number="";
	target_display="";
	target_tag="";
	target_branch="";
	memset(target_callid_ip, 0, 24);
	memset(targetip, 0, 24);
	targetport = 0;
    recv_csequence=0;
	recv_other_msg=0;
	ftag="";
	memset(lr, 0, 4);
	rvRRParam="";
	bRecordRoute=0;
	memset(contact_ip, 0, 24);
	contact_port=0;
	selected_codec=0;
    used_codec=0;
    state_code=0;
	iDelete = 0;
	iSentCseq = 0;
	memset(this->sendIP, 0, 24);
	sipCallid.free();
	bStartWithSDP = FALSE;
	bReInvite = FALSE;
}

call_info	&call_info::operator=(const call_info &other)
{
	eCallDir = other.eCallDir;
	eDialogState = other.eDialogState;

    req_eCallState	= other.req_eCallState; // call state
	req_msgType		= other.req_msgType;
	bUdpSession		= other.bUdpSession;
    RTPport=other.RTPport; // RTP 
	RTPportToSend=other.RTPportToSend; // RTP 
	sprintf_s(RTPip, 24, "%s", other.RTPip);
	sprintf_s(RTPipToSend, 24, "%s", other.RTPipToSend);
	target_number=other.target_number;
	target_display=other.target_display;
	target_tag=other.target_tag;
	self_tag=other.self_tag;
	target_branch=other.target_branch;
	self_branch = other.self_branch;
	sprintf_s(target_callid_ip,24, "%s", other.target_callid_ip);
	sprintf_s(targetip, 24, "%s", other.targetip);
	targetport = other.targetport;
    recv_csequence=other.recv_csequence;
	recv_other_msg=other.recv_other_msg;
	ftag=other.ftag;
	sprintf_s(lr, 4, "%s", other.lr);
	rvRRParam=other.rvRRParam;
	bRecordRoute=other.bRecordRoute;
	sprintf_s(contact_ip, 24, "%s", other.contact_ip);
	contact_port=other.contact_port;
	selected_codec=other.selected_codec;
    used_codec=other.used_codec;
    state_code=other.state_code;
	iDelete = other.iDelete;
	iSentCseq = other.iSentCseq;
	sipCallid = other.sipCallid;
	sprintf_s(this->sendIP, 24, "%s", other.sendIP);
	bStartWithSDP = other.bStartWithSDP;
	return *this;
}

int call_info::operator ==(const int &ival)
{
	if( this->iDelete == ival )
		return 1;
	return 0;
}

int	call_info::operator==(const char *toNumber)
{
	if( !this->target_number.compare(toNumber ))
	{
			return 1;
	}
	return 0;
}
int	call_info::operator<(const char *toNumber)
{
	if( this->target_number.compare(toNumber )<0)
	{
			return 1;
	}
	return 0;
}
int	call_info::operator>(const char *toNumber)
{
	if( this->target_number.compare(toNumber )>0)
	{
			return 1;
	}
	return 0;
}

int	call_info::operator==(const call_id_t &callId)
{
	if( this->sipCallid == callId )
		return 1;
	return 0;
}
