#include "StdAfx.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

//#include "netutils.h"

#ifdef _REAL_RTP_USE
#include "mediathread.h"
#include "pcmwave.h"
#include "usermsg.h"
#include "netutils.h"
#include "pcmwave.h"
#include "phonevoice.h"
#include "G729AWave.h"
#include "g723Wave.h"
#endif// _REAL_RTP_USE
HWND gMainWnd = NULL;
#include "skysipua_g.h"
#include "skysip.h"
#include "skysock.h"
#include "skyasyncsock.h"
#include "SipCallStack.h"

#include "StunClient.h"
#include "SipUac.h"

#include "SkySipUaManager.h"


#include "SIPMain.h"

#ifdef _REAL_RTP_USE
extern CG723Wave * gOutWave;
extern CG729AWave * gOut729Wave;
//extern CPhoneVoice * gOutWave;
extern unsigned int gUsedCodec;
int get_rtp_handle(int dir);
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSIPMain::CSIPMain()
: bRequestEnding(false)
{
	this->bRequestEnding = false;
}

CSIPMain::~CSIPMain()
{
}

//////////////////////////////////////////////////////////////////
// Definition of Function
//////////////////////////////////////////////////////////////////

void SendStateMsg2(unsigned int wParam, char * sBuf, unsigned int lParam)
{
#if 0

	char * sbuff=NULL;
#ifdef WIN32
		CUbiDialerDlg * pDlg;
#endif
	if( sBuf )
	{
	    sbuff = (char*) malloc(strlen(sBuf)+1);
		if( sbuff )
	        sprintf(sbuff,"%s", sBuf);
	}
#ifdef WIN32
	if( theApp )
	{
		if( theApp.m_pMainWnd )
		{
			pDlg = (CUbiDialerDlg *)theApp.m_pMainWnd;
			if( sbuff )
			    pDlg->SendMessage(WM_USER_MSG,(WPARAM)wParam,(LPARAM)sbuff);
			else
                pDlg->SendMessage(WM_USER_MSG,(WPARAM)wParam,(LPARAM)lParam);
		}
	}
#elif defined LINUX
	if( sbuff )
	    free(sbuff);
#endif

#endif
}

void SendStateMsg(char * sBuf)
{
#if 0
	//time_t tt;
	//struct tm * ltm;
	char * sbuff;
	sbuff = (char*) malloc(512);
	if(sbuff )
	{
		//time(&tt);
		//ltm = localtime(&tt);
		//sprintf(sbuff, "%02d/%02d/%02d : %s",
		//	      ltm->tm_hour,ltm->tm_min,ltm->tm_sec,sBuf);
		sprintf_s(sbuff,512, "%s", sBuf);
#if 0
#ifdef WIN32
		CUbiDialerDlg * pDlg;
		if( theApp )
		{
			if( theApp.m_pMainWnd )
			{
				pDlg = (CUbiDialerDlg *)theApp.m_pMainWnd;
				pDlg->SendMessage(WM_USER_MSG,(WPARAM)sbuff);
			}
		}
#endif
#elif defined LINUX
		free(sbuff);
#endif
	}
#endif
}

// if success, return Value is rtp port value.
// if -1, fail.
int GetRTPInformation(void * _pMain, void * _pSipHd, char *szIP, int buflen)
{
	sip_hd_t *pHd = (sip_hd_t*)_pSipHd;
	CSIPMain * pMain = (CSIPMain *)_pMain;
	if( pHd  && pMain )
	{
		if( pHd->sipContent.sdp.media.size() > 0 )
		{
			if( pHd->sipContent.sdp.connection.Address.size() > 0)
			{
				sprintf_s(szIP, buflen,"%s",
					(!pHd->sipContent.sdp.connection.Address.empty()?(char*)pHd->sipContent.sdp.connection.Address.data():"127.0.0.1"));
			}
			else
			{
				sprintf_s(szIP, buflen,"%s",
					(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
			}
			return pHd->sipContent.sdp.media.begin()->get()->m_desc.port;
		}
	}
	return -1;
}

void CSIPMain::PSetNum(char inputChar)
{
	char tbuf[32];
	sprintf_s(tbuf, 32, "%c", inputChar);
	//strcat(CallInfo.target_number,tbuf);
	
}

void CSIPMain::ClearNum()
{
	//memset(CallInfo.target_number, 0, 64);

}

void CSIPMain::DelNum()
{
	//int len = sizeof( CallInfo.target_number);
	//CallInfo.target_number[len-1]=0;

}

int CSIPMain::OnRecvNewCall(void * pArg)
{
	p_sip_hd_t phd = (p_sip_hd_t)pArg;
	SendStateMsg2(WM_SIPMSG_RV_INV,(char*)phd->sipfield.from.uri.uid.data(), 0);
	return 1;
}

int CSIPMain::OnRecvEndCall(void * pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		CONNECTIONINFO *pCon = new CONNECTIONINFO();
		GetRTPInformation(this, pHd, pCon->szRtpIP, 24);
		sprintf_s(pCon->szTarget, 64, "%s",
				(!pHd->sipfield.from.uri.uid.empty()?(char*)pHd->sipfield.from.uri.uid.data():this->u_number));
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_BYE,(WPARAM)pCon,0);
	}
	return 1;
}

int CSIPMain::OnRecvReqCallOk(void * pArg)
{
#ifdef _REAL_RTP_USE
	USBOutwave->bInTalk = TRUE;
    USBwave->bInTalk = TRUE;
#endif
	KillTimer(gMainWnd, 10);
	KillTimer(gMainWnd, 20);
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		if( pHd->sipfield.cseq.type == msg_invite )
		{
			CONNECTIONINFO *pCon = new CONNECTIONINFO();

			pCon->szRtpPort = GetRTPInformation(this, pArg, pCon->szRtpIP, 24);
			pCon->szInPort = 10000;
			sprintf_s(pCon->szTarget, 64, "%s",
					(!pHd->sipfield.to.uri.uid.empty()?(char*)pHd->sipfield.to.uri.uid.data():this->u_number));
#if 1 //muk test
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_SUCC,(WPARAM)pCon,(LPARAM)pHd);
#else
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_SUCC,(WPARAM)pCon,0);
#endif
		}
		else
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_SUCC,NULL,0);
#if 0 //muk test
		if ( strcmp((char *)pHd->sipfield.to.uri.uid.c_str(), "msml") == 0 )
		{
			send_ack((char *)pHd->sipfield.cid.callid.c_str());

			// muk 이부분 정리
			if ( strcmp((char *)pHd->sipfield.from.uri.uid.c_str(), "810233503") != 0 )
			{
				send_ack_withSDP((char *)pHd->sipfield.cid.callid.c_str());
			}
		}
#endif
	}
	return 1;
}

int CSIPMain::OnRecvNewCallAck(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		if( pHd->preheadline.msg_type == msg_invite )
		{
			CONNECTIONINFO *pCon = new CONNECTIONINFO();
			if( pHd->sipContent.sdp.media.size() > 0 )
			{
				sprintf_s(pCon->szRtpIP, 24,"%s",
					(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
				pCon->szRtpPort = pHd->sipContent.sdp.media.begin()->get()->m_desc.port;
				pCon->szInPort = 10000;
			}
			sprintf_s(pCon->szTarget, 64, "%s",
					(!pHd->sipfield.from.uri.uid.empty()?(char*)pHd->sipfield.from.uri.uid.data():this->u_number));
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_SUCC,(WPARAM)pCon,0);
		}
		else
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_SUCC,NULL,0);
	}
	return 1;
}

int CSIPMain::OnRecvCancelCall(void *pArg)
{
	SendStateMsg2(WM_SIPMSG_RV_INV_FAIL,NULL,0);
	return 1;
}

int CSIPMain::OnRecvRegisterOk(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t*)pArg;
	if( pHd )
	{
		if( pHd->sipfield.expire > 0 )
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_REG_SUCC, 1, 0);
		else
			SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_REG_SUCC, 1, 1);
	}
	//SendStateMsg2(,"1",0);
	return 1;
}


void CSIPMain::OnRegisterFail(PVOID pArg)
{
	SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_REG_FAIL, 0, 0);
}

int CSIPMain::OnRecvFailMsg(void *parg)
{
	int error_code = (int)parg;	
	switch( error_code )
	{
	case 484:
	case 404: // not found
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		// play dial tone
		break;
	case 408: // time out
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	case 400:
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	case 401: // req WWW Auth
		break;
	case 407: // www auth fail
		break;
	case 405: // unsupported method
		SendStateMsg2(WM_SIPMSG_UNSUPP_METHOD,NULL, error_code);
		break;
	case 489: // busy
		SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		SetTimer(gMainWnd, 50, 500, NULL);
		break;
	default:
	    SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL, error_code);
		break;
	}
	return 1;
}

int CSIPMain::OnSendReqCall(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		CONNECTIONINFO *pCon = new CONNECTIONINFO();
		if( pHd->sipContent.sdp.media.size() > 0 )
		{
			sprintf_s(pCon->szRtpIP, 24,"%s",
				(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
		}
		sprintf_s(pCon->szTarget, 64, "%s",
			(!pHd->sipfield.to.uri.uid.empty()?(char*)pHd->sipfield.to.uri.uid.data():this->u_number));
#if 1 // muk test Callid 관리 위해
		//SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_INV,(WPARAM)pCon,(LPARAM)(char *)pHd->sipfield.cid.callid.c_str());
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_INV,(WPARAM)pCon,(LPARAM)pHd);
#else
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_INV,(WPARAM)pCon,0);
#endif
	}
	return 1;
}

int CSIPMain::OnSendNewCallOk(void *pArg)
{
#ifdef _REAL_RTP_USE
	USBOutwave->bInTalk = TRUE;
    USBwave->bInTalk = TRUE;
	KillTimer(gMainWnd, 10);
	KillTimer(gMainWnd, 20);
    SendStateMsg2(WM_SIPMSG_SD_NEW_CALL_OK,NULL,0);
#endif
	return 1;
}

int CSIPMain::OnSendReqCallAck(void *pArg)
{

	return 1;
}

int CSIPMain::OnSendRegister(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendFailMsg(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendEndCall(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		CONNECTIONINFO *pCon = new CONNECTIONINFO();
		if( pHd->sipContent.sdp.media.size() > 0 )
		{
			sprintf_s(pCon->szRtpIP, 24,"%s",
				(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
		}
		sprintf_s(pCon->szTarget, 64, "%s",
				(!pHd->sipfield.to.uri.uid.empty()?(char*)pHd->sipfield.to.uri.uid.data():this->u_number));
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SD_BYE,(WPARAM)pCon,0);
	}
	return 1;
}

// 에러 처리
int CSIPMain::OnFailNewCall(void *pArg)
{
    //SendStateMsg2(WM_SIPMSG_RV_INV_FAIL,NULL,0);
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		CONNECTIONINFO *pCon = new CONNECTIONINFO();
		if( pHd->sipContent.sdp.media.size() > 0 )
		{
			sprintf_s(pCon->szRtpIP, 24,"%s",
				(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
		}
		sprintf_s(pCon->szTarget, 64, "%s",
			(!pHd->sipfield.to.uri.uid.empty()?(char*)pHd->sipfield.to.uri.uid.data():this->u_number));
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_FAIL,(WPARAM)pCon,0);
	}

	return 1;
}

// 에러 처리
int CSIPMain::OnFailReqCall(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		CONNECTIONINFO *pCon = new CONNECTIONINFO();
		if( pHd->sipContent.sdp.media.size() > 0 )
		{
			sprintf_s(pCon->szRtpIP, 24,"%s",
				(!pHd->sipContent.sdp.media.at(0)->connection.Address.empty()?(char*)pHd->sipContent.sdp.media.at(0)->connection.Address.data():"127.0.0.1"));
		}
		sprintf_s(pCon->szTarget, 64, "%s",
				(!pHd->sipfield.to.uri.uid.empty()?(char*)pHd->sipfield.to.uri.uid.data():this->u_number));
		pCon->iStateCode = pHd->preheadline.state;
		pCon->strText = pHd->preheadline.text;
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RV_INV_FAIL,(WPARAM)pCon,0);
	}
    //SendStateMsg2(WM_SIPMSG_SD_INV_FAIL,NULL,0);
	return 1;
}

int CSIPMain::OnRecvTrying(void *pArg)
{
	return 1;
}

int CSIPMain::OnRecvRinging(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendTrying(void *pArg)
{
	return 1;
}

int CSIPMain::OnSendRinging(void *pArg)
{
	return 1;
}

void CSIPMain::OnTimer(IN UINT nIDEvent)
{
#ifdef _REAL_RTP_USE
	switch(nIDEvent)
	{
	case 9:
		KillTimer(gMainWnd, 9);
		if( !stun_enable )
			if( sipUAC )
		        sipUAC->send_udp("    ", 4, proxyip, 5060);
		//else
        //    sipUAC->send_udp("    ", 4, stunInfo.stunip, stunInfo.stunport);
		SetTimer(gMainWnd, 9, 30000,NULL);
		break;
	case 10:
		{
			KillTimer(gMainWnd, 10);
			if( SPwave )
			{
				SPwave = (CPCMWave *)SPwave;
				SPwave->waveOut(pInWave);
			}
			SetTimer(gMainWnd, 10, 6000, NULL);
		}
		break;
	case 20:
		{
			KillTimer(gMainWnd, 20);
			if( USBOutwave )
			{
				USBOutwave = (CPCMWave *)USBOutwave;
				USBOutwave->waveOut(pOutWave);
			}
			SetTimer(gMainWnd, 20, 6000, NULL);
		}
		break;
	case 30:
		break;
	case 40:
		::KillTimer(gMainWnd, 40);
		::KillTimer(gMainWnd, 60);
		::KillTimer(gMainWnd, 70);
		::KillTimer(gMainWnd, 50);
		break;
	case 50:
		{
			KillTimer(gMainWnd, 50);
		    if( BUSYwave)
		        BUSYwave->waveOut(pBusyWave);
			SetTimer(gMainWnd, 50, 6000, NULL);
		}
		break;
	case 60:
		//PrintCallMsg( "연결중..." );
		break;
	case 70:
		//PrintCallMsg( "연결됨" );
		break;
	}
#endif // _REAL_RTP_USE
}

#ifdef _REAL_RTP_USE
int CSIPMain::SetValue(MIXERCONTROL& mxc, int Volume, int Mode)
{
	MMRESULT		mmresult;
	MIXERCONTROLDETAILS	mxcd={0};
	MIXERCONTROLDETAILS_UNSIGNED	mcd_u = {0};
	int			ret = 0;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cChannels = m_dwChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mcd_u;

	mmresult = mixerGetControlDetails((HMIXEROBJ)hmx,&mxcd,0L);
	if(mmresult == MMSYSERR_NOERROR)
	{
		char szValue[10];
		sprintf(szValue, "%d", mcd_u.dwValue);		
		
		if (Mode == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
			ret = mcd_u.dwValue;
		else if (Mode == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT)
			ret = mcd_u.dwValue;

		mcd_u.dwValue = Volume;
		mmresult = mixerSetControlDetails((HMIXEROBJ)hmx,&mxcd,0L);
	}	
	return ret;
}

int CSIPMain::LoadWaveFile(LPSTR filename, int nMode)
{
	char strFileName[80];
	long	ll_temp;

	memset(strFileName,0,sizeof(strFileName));

	lstrcpy((LPSTR)strFileName,(LPSTR)filename);

	HMMIO hmmio = mmioOpen((LPSTR)strFileName,NULL,MMIO_READ);

	if(!hmmio)
	{
		AfxMessageBox("화일이 없습니다.!");
		return FALSE;
	}

	MMCkInfoParent.fccType = mmioFOURCC('W','A','V','E');
	erroCode = mmioDescend(hmmio,&MMCkInfoParent,NULL,MMIO_FINDRIFF);
	
	if(erroCode)
	{
		AfxMessageBox("웨이브화일이 아닙니다.");
		mmioClose(hmmio,0);
		return FALSE;
	}
	
	MMCkInfoChild.ckid = mmioFOURCC('f','m','t',' '); 
	erroCode = mmioDescend(hmmio,&MMCkInfoChild,&MMCkInfoParent,MMIO_FINDCHUNK);
	
	if(erroCode)
	{
		AfxMessageBox("fmt Descending Error");
		mmioClose(hmmio,0);
		return FALSE;
	}

	DWORD bytesRead = mmioRead(hmmio,(LPSTR)&WaveRecord,MMCkInfoChild.cksize);
	
	if(bytesRead<=0)
	{
		AfxMessageBox("wave format read Error");
		mmioClose(hmmio,0);
		return FALSE;
	}
	
	erroCode = mmioAscend(hmmio,&MMCkInfoChild,0);
	
	if(erroCode)
	{
		AfxMessageBox("Error Ascending in file");
		mmioClose(hmmio,0);
		return FALSE;
	}

	MMCkInfoChild.ckid = mmioFOURCC('d','a','t','a');

	erroCode = mmioDescend(hmmio,&MMCkInfoChild,&MMCkInfoParent,MMIO_FINDCHUNK);

	if(erroCode)
	{
		AfxMessageBox("data 청크를 찾을수 가 없습니다.");
		mmioClose(hmmio,0);
		return FALSE;
	}

	switch(nMode)
	{
	case OUT_WAVE:
		lOutDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))
		{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		if((ll_temp = mmioRead(hmmio,(LPSTR)pOutWave,lOutDatasize)) != lOutDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;
	case IN_WAVE:		
		lInDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))	{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}
		if((ll_temp = mmioRead(hmmio,(LPSTR)pInWave,lInDatasize)) != lInDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;	
	case BUSY_WAVE:
		lBusyDatasize = MMCkInfoChild.cksize;
		if(!wAllocMemory(nMode))
		{
			AfxMessageBox("적재할 메모리가 부족합니다");
			mmioClose(hmmio,0);
			return FALSE;
		}
		if((ll_temp = mmioRead(hmmio,(LPSTR)pBusyWave,lBusyDatasize)) != lBusyDatasize)
		{
			AfxMessageBox("데이타를 읽을수가 없습니다.");
			mmioClose(hmmio,0);
			return FALSE;
		}		
		break;	
	}

	mmioClose(hmmio,0);	 

	return TRUE;

}

void CSIPMain::wFreeMemory()
{
	if(waveOutDataBlock)
	{
		::GlobalUnlock(waveOutDataBlock);
		::GlobalFree(waveOutDataBlock);
		waveOutDataBlock=NULL;
		pOutWave=NULL;
	}

	if(waveInDataBlock)
	{
		::GlobalUnlock(waveInDataBlock);
		::GlobalFree(waveInDataBlock);
		waveInDataBlock=NULL;
		pInWave=NULL;
	}		

	if(waveBusyDataBlock)
	{
		::GlobalUnlock(waveBusyDataBlock);
		::GlobalFree(waveBusyDataBlock);
		waveBusyDataBlock=NULL;
		pBusyWave=NULL;
	}		
}

int CSIPMain::wAllocMemory(int nMode)
{
	switch(nMode)
	{
	case OUT_WAVE:
		waveOutDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lOutDatasize);
		if(waveOutDataBlock == NULL)
			return FALSE;
		pOutWave = (LPBYTE)::GlobalLock(waveOutDataBlock);	
		break;
	case IN_WAVE:			
		waveInDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lInDatasize);
		if(waveInDataBlock == NULL)
			return FALSE;
		pInWave = (LPBYTE)::GlobalLock(waveInDataBlock);		
		break;
	case BUSY_WAVE:		
		waveBusyDataBlock = ::GlobalAlloc(GMEM_MOVEABLE,lBusyDatasize);
		if(waveBusyDataBlock == NULL)
			return FALSE;

		pBusyWave = (LPBYTE)::GlobalLock(waveBusyDataBlock);

	}
	return TRUE;
}
#endif// _REAL_RTP_USE

int CSIPMain::voice_start(HWND hWndDlg)
{
#ifdef _REAL_RTP_USE
	CString		strTemp;
	precalledNum[0] = 0;
	USBwave = NULL;
	USBOutwave = NULL;
	SPwave = NULL;
	BUSYwave = NULL;

	//PrintCallMsg("로그인중...");

	//callstt = CALL_IDLE;
	USBwave = new CPCMWave;
	USBOutwave = new CPCMWave;
	SPwave = new CPCMWave;
	BUSYwave = new CPCMWave;

	StartMediaThread(hWndDlg);
	gMainWnd = hWndDlg;
#if 1
	if(USBwave)
	{
#ifdef UBISTAR
		if( USBwave->searchInDev("UBISTAR AUDIO DEVICE") < 0 )
#else
		if( USBwave->searchInDev(NULL) < 0 )
#endif
		{
			delete USBwave;
			return 0;
		}
		
		USBwave->open(1, 16, 8000, 480);
	}

	if(USBOutwave)
	{
		strTemp.Format("%s","ringout.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()),OUT_WAVE);
		USBOutwave->selectOutDev("UBI");
		USBOutwave->open(1, 8, 11025, lOutDatasize);
	}

	if(SPwave)
	{
		strTemp.Format("%s","ringin.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()), IN_WAVE);
		SPwave->open(1, 8, 11025, lInDatasize);
	}

	if(BUSYwave)
	{
		strTemp.Format("%s","busy.wav");
		LoadWaveFile(strTemp.GetBuffer(strTemp.GetLength()), BUSY_WAVE);
		BUSYwave->open(1, 8, 11025, lBusyDatasize);
	}
#endif
#endif // _REAL_RTP_USE
	return 1;
}

void CSIPMain::voice_stop()
{
#ifdef _REAL_RTP_USE
	//switch(callstt)	{
	//case CALL_CONNECTED:
	//case CALL_REMOTEWAITING:
	//	OnKeyIn(13, 0);
	//	break;
	//}	

	::KillTimer(gMainWnd, 10);
	::KillTimer(gMainWnd, 20);	
	::KillTimer(gMainWnd, 50);	

	if(gMediaThrID) ::SendMessage(gMediaWnd, UM_MEDIAQUIT, 0, 0);
	if(gMediaThrID) ::PostThreadMessage(gMediaThrID, WM_QUIT, 0, 0);
	if(ghMediaThr) ::WaitForSingleObject((HANDLE)ghMediaThr, 1000);
	wFreeMemory();	

	if(USBwave)
	{
		USBwave->close();
		delete USBwave;
		USBwave = NULL;
	}

	if(USBOutwave)
	{
		USBOutwave->close();
		delete USBOutwave;
		USBOutwave = NULL;
	}

	if(SPwave)
	{
		SPwave->close();
		delete SPwave;
		SPwave = NULL;
	}

	if(BUSYwave)
	{
		BUSYwave->close();
		delete BUSYwave;
		BUSYwave = NULL;
	}
#endif // _REAL_RTP_USE
}


void CSIPMain::OnRecvMsgByeOK(void * pArg)
{
    SendStateMsg2(WM_SIPMSG_SD_BYE_SUCC,NULL,0);
}

int CSIPMain::OnRecvInfo(void *pArg)
{
	return 1;
}

void CSIPMain::OnRecvCancelOK(void *pArg)
{
}

void CSIPMain::OnRecvOK(void *pArg)
{
#if 1 //muk test
	sip_hd_t *pHd = (sip_hd_t *)pArg;

	if(pHd )
	{
		if ( pHd->sipfield.cseq.type == msg_info )
		{
			basic_string <char>::size_type stResult;
			stResult = pHd->sipContent.xml.find("<result response=\"200\">");

			if (stResult != string::npos )
			{
				SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RECVINFO_OK,(WPARAM)"Success",NULL);
			}
			else
			{
				SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_RECVINFO_OK,(WPARAM)"Fail",NULL);
			}


			//SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SENDACK,(WPARAM)(char *)pHd->sipfield.cid.callid.c_str(),NULL);
		}
	}
#endif
}

int CSIPMain::OnSendByeOk(void *pArg)
{
	return 1;
}

void CSIPMain::OnTimeEvent(void *pArg)
{
}

void CSIPMain::OnRecv180Invite(void *pArg)
{
}

void CSIPMain::OnRecv180Process(void *pArg)
{
}

int CSIPMain::OnRecvInviteFail(void *pArg)
{
	return 1;
}

#if 1 //muk test
int	CSIPMain::OnSendAck(IN void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_SENDACK,(WPARAM)(char *)pHd->sipfield.cid.callid.c_str(),NULL);
	}
	return 1;
}

int	CSIPMain::OnBeforeSendAck(IN void *pArg)
{
	/*
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	
	if (pHd)
	{
		// 이부분도처리할것
		if (strcmp((char *)pHd->sipfield.from.uri.uid.c_str(),"810233503") != 0 )
		{
			char szTo[256];
			char szFrom[256];

			memset(szTo, 0, 256);
			memset(szFrom, 0, 256);

			strcpy(szTo, pHd->sipfield.to.uri.uid.c_str());
			strcpy(szFrom, pHd->sipfield.from.uri.uid.c_str());

			pHd->sipfield.to.uri.uid = szFrom;
			pHd->sipfield.to.display = szFrom;
			pHd->sipfield.to.uri.haddr.hostAddr = "211.115.66.98";

			pHd->sipfield.from.uri.uid = "810233503";
			pHd->sipfield.from.display = "810233503";

			pHd->preheadline.sipUri.uid = szFrom;
			pHd->preheadline.sipUri.haddr.hostAddr = "211.115.66.98";
		}
	}
	*/
	return 1;
}

int	CSIPMain::OnBeforeSendReqCall(void *pArg)
{
	sip_hd_t *pHd = (sip_hd_t *)pArg;
	if(pHd )
	{
		//SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_BEFORESEND_INVITE,(WPARAM)pHd,(LPARAM)(char *)this->u_number);
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_SIPMSG_BEFORESEND_INVITE,(WPARAM)pHd,NULL);
	}
	return 1;
}
#endif




//////////////////////////////////////////////////////////////////////////
// end of file
//////////////////////////////////////////////////////////////////////////
