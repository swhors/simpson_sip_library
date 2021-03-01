#pragma once

typedef enum E_DIALOGSTATE
{
	e_dialog_start=0,
	e_dialog_proceeding,
	e_dialog_sucees,
	e_dialog_fail,
	e_dialog_ending,
	e_dialog_end
}EDIALOGSTATE;

typedef enum E_CALL_DIR
{
	e_call_dir_request=0,
	e_call_dir_receive
}ECALLDIR;

class call_info
{
public:
	call_info(void);
	virtual ~call_info(void);
    
	//sdp_info_t sdpInfo;	// sdp information.
	ECALLDIR		eCallDir;
	EDIALOGSTATE	eDialogState;
    e_msg_state		req_eCallState; // call state
	char			sendIP[24];
	int				req_msgType;
    int				bUdpSession; // call tr type : tcp or udp
    int				RTPport; // self RTP Port
    char			RTPip[24]; // self RTP IP
	int				RTPportToSend; // remote RTP Port
	char			RTPipToSend[24];// remote RTP IP
	string			target_number;
	string			target_display;
	string			target_tag;
	string			self_tag;
	string			target_branch;
	string			self_branch;
	char			target_callid_ip[24];
    char			targetip[24];
    int				targetport;
    int				recv_csequence;
	int				recv_other_msg;
	string			ftag;
	char			lr[4];
	string			rvRRParam;
	int				bRecordRoute;
	char			contact_ip[24];
	int				contact_port;
	int				selected_codec;
    int				used_codec;
    int				state_code;
	call_id_t		sipCallid;
	int				iDelete;
	int				iSentCseq;
	BOOL			bStartWithSDP;
	BOOL			bReInvite;
	BOOL			IsReInvite(){return bReInvite;};
	void			ReInVite(BOOL bVal){bReInvite = bVal;};
	
	// function
	void		init();
	// operator
	call_info	&operator=(const call_info &other);
	int			operator==(const int &iVal);
	//int			operator==(const call_info &other);
	int			operator==(const char *toNumber);
	int			operator<(const char *toNumber);
	int			operator>(const char *toNumber);
	int			operator==(const call_id_t &callId);
};
