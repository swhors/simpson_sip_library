#pragma once

#if 1 // muk test
#define WM_SIPMSG_SENDACK				WM_USER+0x0500
#define WM_SIPMSG_RECVINFO_OK			WM_USER+0x0501
#define WM_SIPMSG_BEFORESEND_INVITE		WM_USER+0x0502
#endif
typedef struct _ConnectionInfo
{
	_ConnectionInfo()
	{
		memset(szRtpIP,0, 24);
		memset(szTarget, 0, 64);
		szRtpPort = 0;
		szInPort = 0;
		iStateCode=200;
	}
	char szRtpIP[24];
	int	 szRtpPort;
	char szTarget[64];
	int	 szInPort;
	int		iStateCode;
	string	strText;
}CONNECTIONINFO;

class CSIPMain :
	public sky_sip_manager
{
public:
	CSIPMain();
	~CSIPMain();
public:// function.
	void voice_stop();
	int voice_start(HWND hWndDlg);
	void PSetNum(char inputChar);
	void ClearNum();
	void DelNum();
public:// Virtual Event Message Function
	int	OnRecvNewCall(IN void * pArg);
	int	OnRecvEndCall(IN void * pArg);
	int	OnRecvNewCallAck(IN void *pArg);
	int	OnRecvCancelCall(IN void *pArg);
	int	OnRecvTrying(IN void *pArg);
	int	OnRecvRinging(IN void *pArg);
	int	OnRecvInfo(IN void *pArg);

	int  OnSendReqCall(IN void *pArg);
	int  OnSendReqCallAck(IN void *pArg);
	int  OnSendEndCall(IN void *pArg);
	int  OnSendTrying(IN void *pArg);
	int  OnSendRinging(IN void *pArg);
	// On received ok messge.
	void	OnRecvMsgByeOK(IN void *pArg); // OK Message -BYE
	void	OnRecvCancelOK(IN void *pArg); // OK Message - CANCEL
	void	OnRecvOK(IN void *pArg); // OK Message - General
	int		OnRecvReqCallOk(IN void * pArg);
	int		OnRecvRegisterOk(IN void *pArg);
	// On send ok message
	int		OnSendByeOk(IN void *pArg); // OK Message - BYE
	int		OnSendNewCallOk(IN void *pArg);

	// On Fail Message
	int  OnFailNewCall(IN void *pArg);
	int  OnFailReqCall(IN void *pArg);

	// On Register Message
	void OnRegisterFail(IN void *pArg);
	int  OnSendRegister(IN void *pArg);
	
	// On Time Event
	void OnTimeEvent(IN void *pArg);

	// timer event
	void OnTimer(IN UINT nIDEvent);

	// 180 event
	void	OnRecv180Process(IN void *pArg);
	void	OnRecv180Invite(IN void *pArg);
	
	// error event
	int		OnSendFailMsg(IN void *pArg);
	int		OnRecvFailMsg(IN void *parg);
	int		OnRecvInviteFail(IN void *pArg);

#if 1 //muk test SedndAck Event
	int	OnSendAck(IN void *pArg);
	int	OnBeforeSendAck(IN void *pArg);
	int	OnBeforeSendReqCall(void *pArg);
#endif

public:// voice function
#ifdef _REAL_RTP_USE
	MMRESULT SetVolume(DWORD LineType, int Volume);
	int		 SetValue(MIXERCONTROL& mxc, int Volume, int Mode);
    int      LoadWaveFile(LPSTR filename, int nMode);
	int      wAllocMemory(int nMode);
	void     wFreeMemory();
#endif // _REAL_RTP_USE
#ifdef _REAL_RTP_USE
public: // rtp information
	char	precalledNum[50];
	HMIXER		hmx;
	MIXERLINE	mxl;
	DWORD		m_dwChannels;
	int			OldMasterVolume, OldWaveOutVolume;
	PCMWAVEFORMAT			WaveRecord;
	HANDLE					waveOutDataBlock;
	HANDLE					waveInDataBlock;
	HANDLE					waveBusyDataBlock;
	long					lOutDatasize;
	long					lInDatasize;
	long					lBusyDatasize;
	LPBYTE					pOutWave;
	LPBYTE					pInWave;
	LPBYTE					pBusyWave;
	CPCMWave *USBwave;
	CPCMWave *USBOutwave;
	CPCMWave *SPwave;	
	CPCMWave *BUSYwave;
	int						erroCode;
	MMCKINFO				MMCkInfoParent;
	MMCKINFO				MMCkInfoChild;
#endif //_REAL_RTP_USE
	// 스택의 종료를 요청한지의 상태를 갖는 플래그
	bool bRequestEnding;
};
