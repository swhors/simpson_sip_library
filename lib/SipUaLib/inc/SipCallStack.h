#pragma once

/* General Method State */
/*
UAC : 
e_msg_st_none->e_msg_st_sd_request->
e_msg_st_rv_preans->+e_msg_st_rv_answer_0 (200 OK)
                    +e_msg_st_rv_answer_1 (else )
UAS :
e_msg_st_none->e_msg_st_rv_request->
e_msg_st_sd_preans->+e_msg_st_sd_answer_0 (200 OK)
					+e_msg_st_sd_answer_1 (else )

*/

typedef enum _e_msg_state
{
	e_msg_st_none = 0,
	e_msg_st_rv_request,
	e_msg_st_sd_request,
	e_msg_st_rv_preans,
	e_msg_st_sd_preans,
	e_msg_st_rv_ack,
	e_msg_st_sd_ack,
	e_msg_st_rv_answer_0, // success
	e_msg_st_rv_answer_1, // fail
	e_msg_st_sd_answer_0, // success
	e_msg_st_sd_answer_1, // fail
}e_msg_state;

class CSipCallStack
{
public:
	CSipCallStack(void);
	CSipCallStack(sip_hd_t *_pSipHd);
	virtual			~CSipCallStack(void);
	string			cid;

#if defined WIN32 || defined WINCE
	HANDLE			hWakeEvent;
	HANDLE			hThreadHandle;
	DWORD			dwThreadID;
#endif
	sip_hd_t		*pSipHd;
	e_msg_state		eState; /*	e_msg_st_none = 0,
								e_msg_st_rv_request,
								e_msg_st_sd_request,
								e_msg_st_rv_preans,
								e_msg_st_sd_preans,
								e_msg_st_rv_ack,
								e_msg_st_sd_ack,
								e_msg_st_rv_answer_0, // success
								e_msg_st_rv_answer_1, // fail
								e_msg_st_sd_answer_0, // success
								e_msg_st_sd_answer_1, // fail
								*/
	time_t			startTime;
	string			szTarget;
	string			szFromTagInformation;
	int				retrialCount;
	bool			bDelete;
	bool			bReqAnswer;
	int				iAnswerCode;
	void			*pManager;
	int				seqNumber;
	
	string			getCallId(){return cid;};

	CSipCallStack	&operator=(const CSipCallStack &other);

	int				operator==(const string &_cid);
	int				operator>(const string &_cid);
	int				operator<(const string &_cid);
	int				operator==(const char *_cid);
	int				operator>(const char *_cid);
	int				operator<(const char *_cid);
	int				operator==(const cseq_hd_t &cSeq);
	int				operator==(const int iSeqNumber);
};
