#pragma once

#ifndef _SKYSIPUAMANAGER_H
#define _SKYSIPUAMANAGER_H

//#include "skysipua_g.h"

////////////////////////
// sip basic information
////////////////////////
//#include "sky_sip_lib.h"

///////////////////////
// SIP UA server/client
///////////////////////
//#include "sipuac.h"
//#include "sip_method.h"
//#include "sip_via.h"
//#include "stunclient.h"

///////////////////////
// RTP 
///////////////////////
//#include "Rtp.h"


// sky sip uac state msg (wParam)(lParam) , wParam's value
// __ wParam start ______________
#define WM_SIPMSG_SD_BYE		WM_USER+0x0010
#define WM_SIPMSG_SD_BYE_SUCC	WM_USER+0x0011

#define WM_SIPMSG_SD_INV		WM_USER+0x0020
#define WM_SIPMSG_SD_INV_SUCC	WM_USER+0x0021
#define WM_SIPMSG_SD_INV_FAIL	WM_USER+0x0022

#define WM_SIPMSG_SD_REG		WM_USER+0x0030
#define WM_SIPMSG_SD_REG_SUCC	WM_USER+0x0031
#define WM_SIPMSG_SD_REG_FAIL	WM_USER+0x0032

#define WM_SIPMSG_SD_INF		WM_USER+0x0040
#define WM_SIPMSG_SD_INF_SUCC	WM_USER+0x0041
#define WM_SIPMSG_SD_INF_FAIL	WM_USER+0x0042

#define WM_SIPMSG_SD_CAN		WM_USER+0x0050
#define WM_SIPMSG_SD_CAN_SUCC	WM_USER+0x0051
#define WM_SIPMSG_SD_CAN_FAIL	WM_USER+0x0052

#define WM_SIPMSG_SD_FAI		WM_USER+0x0060
#define WM_SIPMSG_SD_FAI_SUCC	WM_USER+0x0061
#define WM_SIPMSG_SD_FAI_FAIL	WM_USER+0x0062

#define WM_SIPMSG_RV_BYE		WM_USER+0x0110

#define WM_SIPMSG_RV_INV		WM_USER+0x0120
#define WM_SIPMSG_RV_INV_SUCC	WM_USER+0x0121
#define WM_SIPMSG_RV_INV_FAIL	WM_USER+0x0122

#define WM_SIPMSG_RV_REG		WM_USER+0x0130
#define WM_SIPMSG_RV_REG_SUCC	WM_USER+0x0131
#define WM_SIPMSG_RV_REG_FAIL	WM_USER+0x0132

#define WM_SIPMSG_RV_INF		WM_USER+0x0140
#define WM_SIPMSG_RV_INF_SUCC	WM_USER+0x0141
#define WM_SIPMSG_RV_INF_FAIL	WM_USER+0x0142

#define WM_SIPMSG_RV_CAN		WM_USER+0x0150
#define WM_SIPMSG_RV_CAN_SUCC	WM_USER+0x0151
#define WM_SIPMSG_RV_CAN_FAIL	WM_USER+0x0152

#define WM_SIPMSG_RV_FAI		WM_USER+0x0160
#define WM_SIPMSG_RV_FAI_SUCC	WM_USER+0x0161
#define WM_SIPMSG_RV_FAI_FAIL	WM_USER+0x0162

#define WM_SIPMSG_RV_ANSWER		WM_USER+0x0170


#define WM_SIPMSG_SD_NEW_CALL_OK	WM_USER+0x0210
#define WM_SIPMSG_UNSUPP_METHOD     WM_USER+0x0220
// __ wParam end   ______________

// register method timer's id
#define REGIST_TIMER 3040

#define SKY_MAJOR_VER 2
#define SKY_MINOR_VER 0

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

typedef enum _sip_dtmf_type
{
	dtmf_unknown = 0,
	dtmf_sipinfo,
	dtmf_inband,
	dtmf_rfc2833
}SipDtmfType;

typedef enum _sip_info_type
{
	sipInfo_undefined = 0,
	sipInfo_dtmf
}SipInfoType;

typedef struct _sky_media_att{
    char name[64];
    char value[64];
    int  num;
    int  bAudioCodec;
} sky_media_att,*p_sky_media_att;

typedef struct _hookfn{
        void *rinvite;
        void *rringing;
        void *rwaiting;
        void *rok;
        void *rbye;
		void *rcancel;
		void *rfail;
		void *rinfo;
        void *sinvite;
        void *sringing;
        void *swaiting;
        void *sok;
		void *scancel;
        void *sbye;
		void *sfail;
		void *sinfo;
		void *sregister;
		void *eManager;
}hookfn;

typedef struct _register_info
{
	int contact_point;
	sip_contact_hd_t contact[MAX_CONTACT_NUM];
}REGISTERINFO,*PREGISTERINFO;


typedef struct _stun_info
{
	char stunip[24];
	int  stunport;
	int  stun_state;
}STUNINFO, *PSTUNINFO;

typedef int(SKY_HOOK_FN)(void *pSipHd);

class SKYLIB sky_sip_manager
{
public:
    sky_sip_manager();
    sky_sip_manager(IN int _sipPort,IN int _rtpPort,IN int _rtpEnable);
    ~sky_sip_manager();
    void init_manager(int opt);
    int start_manager(int iOpt);
    void end_manager(int opt);
    int ua_connect(IN char *dip,IN int dport);
    int build_msg(IN OUT sip_hd_t *_pSipHd, IN header_type_e request,IN int err_code,IN sip_method_type_t msgType,IN int bAuth,IN char opt, IN void *requestedMsg=NULL,char *targetNum=NULL, void * callDialog=NULL);
    int build_req_msg(IN OUT sip_hd_t *_pSipHd, IN sip_method_type_t msgType,char *targetNum=NULL, void * callDialog=NULL);
    int build_req_msg(IN OUT sip_hd_t *_pSipHd, IN sip_method_type_t msgType,void * reqHd, void * callDialog);
	int build_req_msg(IN OUT sip_hd_t *_pSipHd, IN sip_method_type_t msgType,IN int bAuth, char *targetNum=NULL);
	int build_req_msg(IN OUT sip_hd_t *_pSipHd, IN sip_method_type_t msgType,IN int bAuth,IN char opt, char *targetNum=NULL);
    int build_res_msg(IN OUT sip_hd_t *_pSipHd, IN int stateCode,IN sip_method_type_t preType, IN p_sip_hd_t reqHd,IN char *szText, void *_pCallDialog=NULL);
    int build_pre_head(IN OUT p_pre_head_line_t pPreHead,IN header_type_e request,IN sip_method_type_t msgType,IN char *pszHostAddr=NULL,IN char *targetNum=NULL);
	int build_pre_head(IN OUT p_pre_head_line_t pPreHead,IN header_type_e request,IN char *pszHostAddr=NULL,IN int err_code=200);
	int build_pre_head_main(OUT p_pre_head_line_t pPreHead,IN char *pszHostAddr=NULL, IN char *targetNum=NULL);
    int build_body(IN OUT p_sip_hd_t pSipHd,IN OUT p_sip_field_t pSipFIeld,IN header_type_e request,IN sip_method_type_t msgType,IN int bAuth,IN char opt, p_sip_hd_t requestedMsg=NULL,char* targetNum=NULL,void *pcallDialog = NULL);
	// authentication.
	void build_www_auth_response(IN void * pField,IN OUT void *_pAuth,IN char *sMsgType);
	int build_www_auth_info(IN void* pField,IN OUT void * _pAuth,IN char *sMsgType, char *targetNumber, char *contactIP);
	int build_register_a3_auth_info(IN void * _pAuth,OUT unsigned char *hexVal,IN char *sMsgType);

	void get_send_ip(void *_pField);

    void init_sky_media_info();
    int generate_cid(IN OUT call_id_t *_callId, int type=0);
    int ParseData(IN char *mem,IN int len);
	int	PutInData(IN char *mem, IN int len);
    void set_hook(IN hookfn *hook);
    void init_hook();
    int sdp_process(IN p_sdp_info_t r_sdpInfo,IN int direction);
    void zero_target_info();
	void LoadConf();
	int GetVersion();

	int stun_process();
    CStunClient client;
public: // send msg
	int send_update_registra(void *pSipHd);
	int send_ok_to_invite(char * szCallID, int selectCodec=0);
    int send_ok(void * reqHd, void *_pCallDialog=NULL);
    int send_bye(char *szTarget);
	int	send_invite(char *szTarget, char *szRtpIP=NULL, int rtpPort=10000, char *szDestIP=NULL);
	int	send_invite_withNoSDP(char *szTarget, char *szRtpIP=NULL, int rtpPort=10000, char *szDestIP=NULL);
    int send_trying(void *_reqHd);
    int send_ringing(void *_reqHd);
    int send_register(int ttl);
    int send_ack(void * reqHd);
    int send_ack(char *szCallID);
	int send_ack_withSDP(void * reqHd,int reqCodec=0,char *szRtpIP=NULL, int rtpPort=10000);
	int send_ack_withSDP(char *szCallID,int reqCodec=0,char *szRtpIP=NULL, int rtpPort=10000);
	int send_ok_to_invite(int selectCodec=0);
    int send_fail(IN void *pSipHd,IN int reason,IN char *text);
    int send_cancel(char *szTarget);
	int send_sip_info(SipInfoType sipType, char *szTargetID, char opt=-1, char *szTargetIP=NULL, char *szBody=NULL, int bodylength=0, char *msgType=NULL);
	int send_dtmf(char dtmf);
public:// voice function

public:    // local Variables
	REGISTERINFO RegisterInfo;
    //CALLINFO CallInfo;
	// SIP UA server/client
    CSipuac * sipUAC;
    int SIPUASport;
	int SIPUACport;
	// 보낸 패킷의 seq 카운트
	int	iSipCseqCount;
	BOOL	m_bAutoResponse;
	BOOL	m_bSendAckToAllResponse;
	// call information.
    //sip_hd_t sip_info;
	//sip_hd_t sip_info_send;
    
	// error state
    // 현재 요청된 메세지에 대한 처리 코드 : 200 180 100 등..
	int bOpenedRtpIn;
	int bOpenedRtpOut;
public: // UA의 환경 변수
    int rtpEnable;
	DWORD	dwThreadId[2];
	BOOL	bThreadState[2];
	char localip[24];
	char hostip[24];
    int  port;
    char u_number[64];
	char u_branch[128];
    char allow_method[10][32];
    char callid[128];
	char callid_ip[24];
    int  csequence;
    int  timeout;
    int  media_count;
    int  voice_codec;
	int  AutoAnswer;
    sky_media_att mediaAtt[10];
	char sendip[24];

    char proxyip[24];
    int  proxyport;
    
	int  reg_ttl_val;
    int  register_flag;
    int  bRegistered;
    int  i_session_expires;
	int  retrial_count;
    
	char registra_cid[128];
    
	char realm[64];
	char uri[128];
	char auth_passwd[64];
	char www_nonce[48];
	char proxy_nonce[48];
	int  nc_count;
	int  proxy_auth;
	SipDtmfType dtmf_type;
// stun information;
	int  stun_enable;
	STUNINFO stunInfo;
public:// Virtual Event Message Function
	virtual int		OnRecvNewCall(IN void * pArg);
	virtual int		OnRecvEndCall(IN void * pArg);
	virtual int		OnRecvNewCallAck(IN void *pArg);
	virtual int		OnRecvCancelCall(IN void *pArg);
	virtual int		OnRecvTrying(IN void *pArg);
	virtual int		OnRecvRinging(IN void *pArg);
	virtual int		OnRecvInfo(IN void *pArg);
	//2009/11/05 swhors@naver.com
	virtual int		OnRecvAck(IN void *pArg);
	
	virtual int		OnBeforeSendReqCall(void *pArg);
	virtual int		OnSendReqCall(IN void *pArg);
	virtual int		OnBeforeSendReqCallAck(IN void *pArg);
	virtual int		OnSendReqCallAck(IN void *pArg);
	virtual int		OnBeforeSendEndCall(IN void *pArg);
	virtual int		OnSendEndCall(IN void *pArg);
	virtual int		OnBeforeSendTrying(IN void *pArg);
	virtual int		OnSendTrying(IN void *pArg);
	virtual int		OnBeforeSendRinging(IN void *pArg);
	virtual int		OnSendRinging(IN void *pArg);

	// 2009/11/05 swhors@naver.com
	virtual int		OnBeforeSendSipInfo(IN void *pArg);
	virtual int		OnSendSipInfo(IN void *pArg);
	virtual int		OnBeforeSendAck(IN void *pArg);
	virtual int		OnSendAck(IN void *pArg);

	// On received ok messge.
	virtual void	OnRecvMsgByeOK(IN void *pArg); // OK Message -BYE
	virtual void	OnRecvCancelOK(IN void *pArg); // OK Message - CANCEL
	virtual void	OnRecvOK(IN void *pArg); // OK Message - General
	virtual int		OnRecvReqCallOk(IN void * pArg);
	virtual int		OnRecvRegisterOk(IN void *pArg);
	// On send ok message
	virtual int		OnBeforeSendByeOk(IN void *pArg);
	virtual int		OnSendByeOk(IN void *pArg); // OK Message - BYE
	virtual int		OnBeforeSendNewCallOk(IN void *pArg);
	virtual int		OnSendNewCallOk(IN void *pArg);

	// On Fail Message
	virtual int  OnFailNewCall(IN void *pArg);
	virtual int  OnFailReqCall(IN void *pArg);

	// On Register Message
	virtual void OnRegisterFail(IN void *pArg);
	virtual int	 OnBeforeSendRegister(IN void *pArg);
	virtual int  OnSendRegister(IN void *pArg);
	
	// On Time Event
	virtual void OnTimeEvent(IN void *pArg);

	// timer event
	virtual void OnTimer(IN UINT nIDEvent);

	// 180 event
	virtual void	OnRecv180Process(IN void *pArg);
	virtual void	OnRecv180Invite(IN void *pArg);
	
	// error event
	virtual int		OnBeforeSendFailMsg(IN void *pArg);
	virtual int		OnSendFailMsg(IN void *pArg);
	virtual int		OnRecvFailMsg(IN void *parg);
	virtual int		OnRecvInviteFail(IN void *pArg);

public: // UA Manager의 관리 플래그
	int  bRun;
	SKYHANDLE hManagerThread;
	SKYHANDLE hRegisterThread;
    hookfn hookFN;
	//int use_www_auth;
private:
	int	send_ack_internal(void *_pSipCallStack, void * _sipHd=NULL, BOOL bWithSDP=FALSE,int reqCodec = 0, char *szRtpIP=NULL, int rtpPort=10000);
	int	send_invite_internal(char *szTarget, char *szRtpIP, int rtpPort, char *szDestIP, bool withSDP=TRUE);
    void build_SDP(OUT void *_reqHd, IN int _RTPport=10000, char *szRtpIP=NULL, IN void * InSDP= NULL, IN int inCodecSelect=0);
	void build_cnonce(char * cnonce);
	void build_mytag(char *stag, int len);
	// none
};

#endif // _SKYSIPUAMANAGER_H
