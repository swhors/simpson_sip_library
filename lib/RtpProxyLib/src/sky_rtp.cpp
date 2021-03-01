#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "msvc_up.h"
#include <time.h>
#include <stdlib.h>

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

#include <winsock2.h>
#include "skytool.h"
#include "sky_rtp.h"

#include "pcm.h"

static int rtp_num;
static int rtp_start_port;
static	BOOL	bEchoEnable=FALSE;
static	BOOL	bRtcpCtrlEnable=FALSE;
static	HANDLE	hRtpSleepEvent=NULL;


#define G723FRAMELENGTH	24
#define G729FRAMELENGTH 20
#define G711FRAMELENGTH	160

//#define USE_INRTCP_REPLY
//#define USE_OUTRTCP_RELAY

static int		bEnableRTPChannelRecord=TRUE;

static	unsigned char *vData;
static	unsigned char *vPCM;
int					iReadLength = 0;
int					iTotalDataLength = 0;

SKYHANDLE h_inRTPMANAGER;
SKYHANDLE h_outRTPMANAGER;
SKYHANDLE h_inRTCPMANAGER;
SKYHANDLE h_outRTCPMANAGER;
SKY_LOCK  mutex_rtp_inf=SKY_MUTEX_INI;

#define RTP_DEAD_TIME    7000

static int rtp_manager_flag= 0;

int RTPMAX_USER;

//static p_rtp_proxy_buf_t pRtpProxyBuf;

vector<rtp_proxy_buf_t> m_RtpProxyBuf;


_rtp_proxy_::_rtp_proxy_()
{
	memset(indesip, 0, 24);
	memset(outdesip, 0, 24);
	memset(szTarget, 0, 64);
	indesport =0;
	outdesport =0;
	inRtpPort=0;
	inRtcpPort=0;
	outRtpPort=0;
	outRtcpPort=0;
	inRtpHandle=0;
	inRtcpHandle=0;
	outRtpHandle=0;
	outRtcpHandle=0;
	rtpStamp=0;
	rtpTime=0;
	inssrc=0;
	outssrc=0;
	seq=0;
	infp=NULL;
	outfp=NULL;
	bUse = 0;
}

_rtp_proxy_ &_rtp_proxy_::operator=(const _rtp_proxy_ &other)
{
	memcpy(this->indesip, other.indesip, 24);
	memcpy(this->outdesip, other.outdesip, 24);
	memcpy(this->szTarget, other.szTarget, 64);
	indesport =other.indesport;
	outdesport =other.outdesport;
	inRtpPort=other.inRtpPort;
	inRtcpPort=other.inRtcpPort;
	outRtpPort=other.outRtpPort;
	outRtcpPort=other.outRtcpPort;
	inRtpHandle=other.inRtpHandle;
	inRtcpHandle=other.inRtcpHandle;
	outRtpHandle=other.outRtpHandle;
	outRtcpHandle=other.outRtcpHandle;
	rtpStamp=other.rtpStamp;
	rtpTime=other.rtpTime;
	inssrc=other.inssrc;
	outssrc=other.outssrc;
	seq=other.seq;
	infp=other.infp;
	outfp=other.outfp;	
	bUse = other.bUse;
	return *this;
}

int	_rtp_proxy_::operator ==(const char *szTarget)
{
	if( !szTarget )
		return 0;
	if( !memcmp(this->szTarget, szTarget,strlen(this->szTarget)))
		return 1;
	return 0;
}

int	_rtp_proxy_::operator ==(const int	&inPort)
{
	if( this->inRtpPort==inPort)
		return 1;
	return 0;
}

void SetEchoEnable(const BOOL bValue)
{
	bEchoEnable = bValue;
}

BOOL GetEchoEnable()
{
	return bEchoEnable;
}

void SetRtcpCtrlEnable(BOOL bVal)
{
	bRtcpCtrlEnable = bVal;
}

BOOL GetRtcpCtrlEnable()
{
	return bRtcpCtrlEnable;
}

void SetEnableRTPChannelRecord(BOOL bVal)
{
	bEnableRTPChannelRecord = bVal;
}

void rtp_manager_lock()
{
    sky_mutex_lock(&mutex_rtp_inf);
}

void rtp_manager_unlock()
{
    sky_mutex_unlock(&mutex_rtp_inf);
}

void enable_rtp_manager_flag()
{
    rtp_manager_flag = 1;
}

void disable_rtp_manager_flag()
{
    rtp_manager_flag = 0;
}

int uncapRTPHeader(IN unsigned char * buff,IN int len,OUT rtp_hdr_t *p)
{
	unsigned char *pbuf;
	pbuf = (unsigned char *)p;
	if( len != 12 )
		return 0;
	memset(p, 0, sizeof(*p));
	for(int i = 0; i< len/4 ;i++)
	{
		*(pbuf+i*4+3) = buff[i*4];
		*(pbuf+i*4+2) = buff[i*4+1];
		*(pbuf+i*4+1) = buff[i*4+2];
		*(pbuf+i*4  ) = buff[i*4+3];
	}
	return 1;
}

int capRTPHeader(rtp_hdr_t p, unsigned char * buff,int len)
{
	unsigned char *pbuf;
	pbuf = (unsigned char*)&p;
	if(len <12 )
		return 0;
	for(int i=0; i < len/4;i++)
	{
		buff[i*4] = (char)*(pbuf+i*4+3);
		buff[i*4+1] = (char)*(pbuf+i*4+2);
		buff[i*4+2] = (char)*(pbuf+i*4+1);
		buff[i*4+3] = (char)*(pbuf+i*4);
	}
	return 1;
}

long make_rtp_stamp(char * se164, char *de164)
{
    long d_e164, s_e164;
	if( se164 )
        s_e164 = atol(se164);
	else
		s_e164 = 0;
	if( de164 )
		d_e164 = atol(de164);
	else
		d_e164 = 0;
    return d_e164+s_e164;
}

void set_ssrc(rtp_hdr_t &hd, unsigned int ssrc)
{
	hd.ssrc		=	ssrc;
}

void set_mark(rtp_hdr_t &hd, BOOL mark)
{
	hd.m = mark;
}

int build_rtpPack(rtp_hdr_t& hd, unsigned int timeStamp, short sequenceNum,BOOL mark, unsigned int ssrc)
{
	memset(&hd, 0, sizeof(rtp_hdr_t));
	hd.version	=	2;
	//hd.p = 0;
	//hd.cc = 
	hd.m		=	mark;
	hd.seq		=	sequenceNum;
	hd.ts		=	timeStamp;
	hd.ssrc		=	ssrc;
	return 0;
}

//
	/* iCodecType 
		0 - G7231/64
		1 - G7231/53
		2 - G711Ulaw
		3 - G711Alaw
	*/
//

int rtpWriteFromFile(int iCodec, void *l_pCon, unsigned char *tSendBuf, int dLength,int &iState, int &iReserved)
{
#if 0
	int iSkip_count=0;
	rtp_hdr_t	p;
	int readlen=0;
	int iSession = l_pCon.iCallNumber;
	int iReadLen, SleepTime,iTimeStamp;
	unsigned char	tPacketBuffer[37];
	int	rtp_return = 0;
	memset(&p, 0, 12);
	switch(iCodec )
	{
	case 4: // G729
		p.pt = G729;
		iReadLen = G729FRAMELENGTH;
		SleepTime = 16;
		iTimeStamp = 200;
	case 2: // G711Ulaw
		p.pt = PCMU;
		iReadLen = G711FRAMELENGTH;
		SleepTime = 300;
		iTimeStamp = 120;
		break;
	case 3:// G711Alaw
		p.pt = PCMA;
		iReadLen = G711FRAMELENGTH;
		SleepTime = 300;
		iTimeStamp = 120;
		break;
	case 0: //g7231 - 63
	case 1: // g7231 - 53
	default:
		p.pt=G7231;
		iReadLen = G723FRAMELENGTH;
		SleepTime = 29;
		iTimeStamp = 240;
		break;
	}
	p.m = 1;
	DWORD ini_time = GetTickCount();
	p.ts = ini_time;
	p.version = 2;
	readlen+=4;
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	while( readlen < dLength )
	{
		if( !l_pCon.bOpendChannel )
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 1;
		}
		if( iSkip_count == 10)
		{
			if( iState == 2)
			{
				// current ment stop
				if( hEvent )
					CloseHandle(hEvent);
				return 0;
				//goto end_normal;
			}
			else
			{
				iReserved = FALSE;
			}
		}
		else
		{
			iSkip_count ++;
		}
		if( iState == 1)
		{
			// current step end
			//if( bConvert == TRUE)
			//	pCon[l_pCon.iCallNumber+iMaxAllowedCall].bOpendChannel = TRUE;
			if( hEvent )
				CloseHandle(hEvent);
			return -1;
		}
		if( iState == 3)
		{
			if( hEvent )
				CloseHandle(hEvent);
			return 0;
		}
		int iSendNum = 0;
		int sendHandle;
				
		if( l_pCon.iCallNumber >= rtp_num )
		{
			iSendNum = l_pCon.iCallNumber - rtp_num;
			sendHandle = pRtpProxyBuf[iSendNum].outRtpHandle;
		}
		else
		{
			iSendNum = l_pCon.iCallNumber;
			sendHandle = pRtpProxyBuf[iSendNum].inRtpHandle;
		}
		
		if( sendHandle )
		{
			unsigned char rtpHd[13];
			int sLen = 12 + iReadLen;
			p.ts += iTimeStamp;
			p.seq =pRtpProxyBuf[iSendNum].seq++;
			memset(rtpHd, 0, 13);
			if( iSendNum > rtp_num )
				p.ssrc = pRtpProxyBuf[iSendNum].outssrc;
			else
				p.ssrc = pRtpProxyBuf[iSendNum].inssrc;
			capRTPHeader(p, rtpHd, 12);
			
			memcpy( (char*)&tPacketBuffer, rtpHd, 12);
			memcpy( (char*)&tPacketBuffer[12], (char *)&tSendBuf[readlen], iReadLen);
			if( l_pCon.iCallNumber >= rtp_num )
			{
                sendto_sock(sendHandle, pRtpProxyBuf[iSendNum].outdesip, pRtpProxyBuf[iSendNum].outdesport, (char*)&tPacketBuffer[0], sLen);
                time((time_t *)&pRtpProxyBuf[iSendNum].rtpTime);
			}
			else
			{
                sendto_sock(sendHandle, pRtpProxyBuf[iSendNum].indesip, pRtpProxyBuf[iSendNum].indesport, (char*)&tPacketBuffer[0], sLen);
                time((time_t *)&pRtpProxyBuf[iSendNum].rtpTime);
			}
		}
	    readlen += iReadLen;
		if( p.m == 1)
			p.m = 0;
		if( hEvent )
			WaitForSingleObject(hEvent,SleepTime);
		else
			Sleep(SleepTime);
	}
	if( hEvent )
		CloseHandle(hEvent);
#endif
	return 1;
}

int RTPPlayG723File(int iCodec, void *l_pCon, char * szFile, char * sLanguage, int &iState, int iKeepState, int &iReserved)
{
#if 0
	int		nLen = 0;
	unsigned char	*pSendBuf=NULL;
	int	rtp_return;
	int iFileLanguagePage = -1;
	int iFileCodecPage = -1;
	int iFileNum = -1;
	nLen = GetMentData(szFile, iCodec, sLanguage, iFileLanguagePage, iFileCodecPage, pSendBuf);
	if( nLen <= 0 || !pSendBuf)
		return -1;
	
	//rtp_return = rtpWriteFromFile(l_pCon, tSendBuf, nLen, iState,iReserved);
	rtp_return = rtpWriteFromFile(iCodec,l_pCon, pSendBuf, nLen, iState,iReserved);
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if( hEvent)
	{
		//append
		WaitForSingleObject(hEvent, 300);
		CloseHandle(hEvent);
	}
	else
		Sleep(300);
	return rtp_return;
#else
	return 1;
#endif
}


int RtpPlay(int iCodec, 
			void *pCon, 
			char * szFileName, 
			char * sLanguage, 
			int & iState,
			int iSkipState, 
			int &iReserved)
{
	return RTPPlayG723File(iCodec, pCon, szFileName, sLanguage, iState, iSkipState, iReserved);
}

int getEmptyChannel(char *szTarget)
{
	for(vector<rtp_proxy_buf_t>::iterator f = m_RtpProxyBuf.begin();
		f <  m_RtpProxyBuf.end(); f++)
	{
		if( strlen(f->szTarget) <= 0 )
		{
			sprintf_s(f->szTarget, 64, "%s", szTarget);
			f->bUse = 1;
			return f->inRtpPort;
		}
	}
	return -1;
}

void reset_rtp_info_byTarget(char *szTarget)
{
	vector<rtp_proxy_buf_t>::iterator f = find(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end(), szTarget);
	if( f !=m_RtpProxyBuf.end() )
	{
		memset(f->indesip, 0, 24);
		memset(f->outdesip, 0, 24);
		f->indesport = 0;
		f->outdesport = 0;
		memset(f->szTarget, 0, 64);
	}
}

void reset_rtp_info(int inPort)
{
	vector<rtp_proxy_buf_t>::iterator f = find(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end(), inPort);
	if( f !=m_RtpProxyBuf.end() )
	{
		memset(f->indesip, 0, 24);
		memset(f->outdesip, 0, 24);
		f->indesport = 0;
		f->outdesport = 0;
		f->bUse = 0;
		memset(f->szTarget, 0, 64);
	}
}

int set_rtp_in_byTarget(char *ip, int port, char *szTarget)
{
	vector<rtp_proxy_buf_t>::iterator f = find(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end(), szTarget);
	if( f !=m_RtpProxyBuf.end() )
	{
		sprintf_s(f->indesip, 24, "%s", ip);
		f->indesport = port;
		f->inRtpPort;
	}
    return -1;
}

int set_rtp_in(char *ip, int port, int inPort)
{
	vector<rtp_proxy_buf_t>::iterator f = find(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end(), inPort);
	if( f !=m_RtpProxyBuf.end() )
	{
		sprintf_s(f->indesip, 24, "%s", ip);
		f->indesport = port;
		return inPort;
	}

    return -1;
}

int set_rtp_out(char *ip,int port,int call_num,int iCodec)
{
    int i = 0;
    time_t tval;
    time(&tval);
	vector<rtp_proxy_buf_t>::iterator f = find(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end(), call_num);
	if( f !=m_RtpProxyBuf.end() )
	{
		sprintf_s(f->outdesip, 24, "%s", ip);
		f->outdesport = port;
		//f->
	}
    
#if 0    
	rtp_manager_lock();
	
	pRtpProxyBuf[i].outssrc = tval * port;
    pRtpProxyBuf[i].outdesport = port;
    SPRINTF1b(pRtpProxyBuf[i].outdesip,24, "%s",ip);
	if( bEnableRTPChannelRecord )
	{
		char fname[256];
		char fext[16];
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
/*
		if( memcmp( channelName, "g711Ulaw", 8) == 0)
		{
			s->iChannelCodec = 1;
		}
		else if( memcmp( channelName, "g711Alaw", 8) == 0)
		{
			s->iChannelCodec = 2;
		}
		else if( memcmp( channelName, "g7231", 5) == 0)
		{
			s->iChannelCodec = 0;
		}

*/		
		if( iCodec == 2 )
			sprintf_s(fext,16, "%s", "g711a");
		else if( iCodec == 1)
			sprintf_s(fext,16, "%s", "g711u");
		else if( iCodec == 3)
			sprintf_s(fext,16, "%s", "g729");
		else
			sprintf_s(fext,16, "%s", "g7231");
		sprintf_s(fname, 256, ".\\voice\\%4d%02d%02d%02d%02d%02d%04d-%03d-%s.%s",
								sysTime.wYear, 
								sysTime.wMonth,
								sysTime.wDay,
								sysTime.wHour,
								sysTime.wMinute,
								sysTime.wSecond,
								sysTime.wMilliseconds,
								call_num, ip, fext);
#ifdef MSVC_6
		pRtpProxyBuf[i].outfp = fopen(fname, "a+b");
#else
		if( fopen_s(&pRtpProxyBuf[i].outfp, fname, "a+b") != 0 )
			pRtpProxyBuf[i].outfp = NULL;
#endif
		sprintf_s(fname, 256, ".\\voice\\%4d%02d%02d%02d%02d%02d%04d-%03d-%s.%s",
								sysTime.wYear, 
								sysTime.wMonth,
								sysTime.wDay,
								sysTime.wHour,
								sysTime.wMinute,
								sysTime.wSecond,
								sysTime.wMilliseconds,
								call_num-rtp_num, ip, fext);
#ifdef MSVC_6
		pRtpProxyBuf[i].infp = fopen(fname, "a+b");
#else
		if( fopen_s(&pRtpProxyBuf[i].infp, fname, "a+b") != 0 )
			pRtpProxyBuf[i].infp = NULL;
#endif
	}	
	
    rtp_manager_unlock();
    return pRtpProxyBuf[i].inRtpPort;
#endif
	return NULL;
}

//vData

void	LoadVoiceData()
{
//	FILE *fp = NULL;
//	if( fopen_s(&fp, "voice\\busy.g711u", "r+b")!=0 )
//		return;
//
//	DWORD dwLength = _filelength(fp);
	HANDLE hFile = CreateFile(".\\voice\\busy.g711u", GENERIC_READ, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile != INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL, "READ", "READ", MB_OK);
		DWORD dwFileLength = 0, dwRLength=1;
		iTotalDataLength= GetFileSize(hFile, &dwFileLength);
		vData = new unsigned char[iTotalDataLength+1];
		vPCM = new unsigned char[iTotalDataLength * 2+ 1];
		short *pSPCM = (short *)vPCM;
		memset(vData,0, iTotalDataLength+1);
		memset(vPCM,0, iTotalDataLength*2+1);
		ReadFile(hFile, vData, iTotalDataLength, &dwRLength, NULL );
		for( int i = 0 ; i < iTotalDataLength ; i ++ )
		{
			pSPCM[i] = ulaw2linear(vData[i]);
		}

		CloseHandle(hFile);
	}

}

#define UINT8_MAX 255
#define UINT16_MAX	32767*2+1
# define INT8_MIN   (-128)
# define INT16_MIN    (-32767-1)
# define INT32_MIN    (-2147483647-1)
# define INT64_MIN    (-__INT64_C(9223372036854775807)-1)
/* Maximum of signed integral types.  */
# define INT8_MAX   (127)
# define INT16_MAX    (32767)

short mix_16_mono (short sample_1, short sample_2) {
	unsigned int k;
	k = sample_1| sample_2;
	if (k > UINT16_MAX) { k = UINT16_MAX; }
	return k;
}

static SKYTHREAD rtp_in_manager(SKYINVAL parg)
{
    int ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[512];
    char dip[24];
    int dport;

    tv.tv_sec = 10;
    tv.tv_usec = 0;
	bEchoEnable = true;
    while(rtp_manager_flag)
    {
        FD_ZERO(&rfd);
		for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
			c < m_RtpProxyBuf.end() ; c++)
		{
			if( c->inRtpHandle )
				FD_SET(c->inRtpHandle, &rfd);
		}
        
		ret = select(rtp_start_port+0, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
			for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
				c < m_RtpProxyBuf.end() ; c++)
			{
				if(FD_ISSET(c->inRtpHandle, &rfd))
				{
					if( c->bUse )
					{

						len = recvfrom_sock(c->inRtpHandle,
							(char*)&rbuff[0],
							512, (char*)&dip[0], &dport);
						if( bEchoEnable )
						{
							int iDLen = len-24;
							short sPCM[512];
							short *pPCM = (short*)vPCM;
							memset( sPCM, 0, sizeof( short) * 512);
#if 0
							for( int i = 24, b = 0; i< len; i++ )
							{
								sPCM[b++] = ulaw2linear(rbuff[i]);
							}
#endif
#if 0
							for( int i = 0 ; i< (len-24) ; i++)
							{
								if( iReadLength >= iTotalDataLength )
									iReadLength = 0;
								short sData = (short)pPCM[iReadLength];
								//sPCM[i] = mix_16_mono(sPCM[i], (short)&pPCM[iReadLength]);
								//sPCM[i] = (short)&pPCM[iReadLength];
								sPCM[i] = sData;
								iReadLength +=2;
								//rbuff[i] = ((vData[iReadLength++] + rbuff[i])/2);
								//sPCM[i] = (sPCM[i]|pPCM[iReadLength++]);
								//rbuff[i] = (vData[iReadLength++]);
							}
#else
							if( iReadLength >= iTotalDataLength)
								iReadLength = 0;
							memcpy((void*)&sPCM, &pPCM[iReadLength], iDLen);
							iReadLength += iDLen;
#endif
#if 0
							for( int i = 24, b = 0; i< len ; i++)
							{
								rbuff[i] = linear2ulaw(sPCM[b++]);
							}
#endif
							sendto_sock(c->inRtpHandle,
								c->indesip,
								c->indesport,
								(char*)&rbuff[0], len);
						}
						else
						{
							if( c->outdesport > 0 && len > 12)
							{
								rtp_hdr_t p;
								uncapRTPHeader((unsigned char*)&rbuff[0], 12, &p);
								p.ssrc = c->outssrc;
								capRTPHeader(p, (unsigned char*)&rbuff[0], 12);
								sendto_sock(c->outRtpHandle,
									c->outdesip,
									c->outdesport,
									(char*)&rbuff[0], len);
								time((time_t*)&c->rtpTime);
								if( bEnableRTPChannelRecord )
								{
									int iMax;
									if( c->infp ) 
									{
										if( p.pt == G7231 )
											iMax = G723FRAMELENGTH;
										else if ( p.pt == PCMU || p.pt == PCMA)
											iMax = G711FRAMELENGTH;
										else//(p.pt==G729)
											iMax = G729FRAMELENGTH;
										if( iMax == len-12 )
										{
											time_t t;
											time(&t);
											fwrite(&t, sizeof(time_t), 1, c->infp);
											fwrite(rbuff+12, 1, len-12, c->infp);
										}
									}
								}
							}
						}
					}
					break;
				}
			}
        }
    }
    sky_thread_close(h_inRTPMANAGER);
    return THREAD_RETURN;
}

static SKYTHREAD rtcp_in_manager(SKYINVAL parg)
{
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[256];
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    while(rtp_manager_flag)
    {
		for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
			c < m_RtpProxyBuf.end() ; c++)
		{
			if( c->inRtpHandle )
				FD_SET(c->inRtcpHandle, &rfd);
		}
        ret = select(rtp_start_port+1, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
			for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
				c < m_RtpProxyBuf.end() ; c++)
			{
                if(FD_ISSET(c->inRtcpHandle, &rfd))
                {
                    len = recvfrom_sock(c->inRtcpHandle,
                                        (char*)&rbuff[0],
                                        256, NULL, NULL);
                    if( len > 0)
                    {
                        if( c->outdesport > 0)
                        {
                            sendto_sock(c->outRtcpHandle,
                                c->outdesip,
                                c->outdesport+1,
                                (char*)&rbuff[0], len);
                        }
                    }
                }
            }
        }
    }
    sky_thread_close(h_inRTCPMANAGER);
    return THREAD_RETURN;
}

static SKYTHREAD rtp_out_manager(SKYINVAL parg)
{
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    //unsigned char rbuff[256];
	char rbuff[512];
    //FD_ZERO(&rfd);
	::SetThreadPriority(h_outRTPMANAGER, THREAD_PRIORITY_TIME_CRITICAL);
	
    while(rtp_manager_flag)
    {
	    FD_ZERO(&rfd);
		for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
			c < m_RtpProxyBuf.end() ; c++)
		{
			if( c->outRtpHandle)
				FD_SET(c->outRtpHandle, &rfd);
		}
		tv.tv_sec = 10;
	    tv.tv_usec = 0;
        ret = select(rtp_start_port+0, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
			for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
				c < m_RtpProxyBuf.end() ; c++)
			{
                if(FD_ISSET(c->outRtpHandle, &rfd))
                {
					//rtp_manager_lock();
					memset(rbuff, 0, 512);
					if( c->outRtpPort )
					{
						len = recvfrom_sock(c->outRtpHandle,
							(char*)&rbuff[0],
							256, NULL, NULL);
						rtp_hdr_t p;
						if( c->indesport > 0 && len > 12)
						{
							uncapRTPHeader((unsigned char*)&rbuff[0], 12, &p);
							p.ssrc = c->inssrc;
							capRTPHeader(p, (unsigned char*)&rbuff[0], 12);
							sendto_sock(c->inRtpHandle,
									c->indesip,
									c->indesport,
									(char*)&rbuff[0], len);
							if( bEnableRTPChannelRecord )
							{
								int iMax;
								if( c->outfp )
								{
									if( p.pt == G7231 )
										iMax = G723FRAMELENGTH;
									else if(p.pt==G729)
										iMax = G729FRAMELENGTH;
									else//( p.pt == PCMU || p.pt == PCMA)
										iMax = G711FRAMELENGTH;
									if( iMax == len-12 )
									{
										time_t t;
										time(&t);
										fwrite(&t, sizeof(time_t), 1, c->outfp);
										fwrite(rbuff+12, 1, len-12, c->outfp);
									}
								}
							}
						}
					}
					//rtp_manager_unlock();
					break;
                }
            }
        }
    }
    sky_thread_close(h_outRTPMANAGER);
    return THREAD_RETURN;
}
static SKYTHREAD rtcp_out_manager(SKYINVAL parg)
{
    int i, ret,len;
    fd_set rfd;
    struct timeval tv;
    unsigned char rbuff[256];
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    while(rtp_manager_flag)
    {
		for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
				c < m_RtpProxyBuf.end() ; c++)
            if( c->outRtcpHandle )
                FD_SET(c->outRtcpHandle, &rfd);

        ret = select(rtp_start_port+1, &rfd, NULL, NULL, &tv);
        if( ret > 0)
        {
			for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
				c < m_RtpProxyBuf.end() ; c++)
			{
                if(FD_ISSET(c->outRtcpHandle, &rfd))
                {
                    len = recvfrom_sock(c->outRtcpHandle,
                                        (char*)&rbuff[0],
                                        256, NULL, NULL);
                    if( len > 0)
                    {
                        if( c->indesport > 0)
                        {
                            sendto_sock(c->inRtcpHandle,
                                c->indesip,
                                c->indesport+1,
                                (char*)&rbuff[0], len);
                        }
                    }
                }
            }
        }
    }
    sky_thread_close(h_outRTCPMANAGER);
    return THREAD_RETURN;
}

int	stop_rtp_manager()
{
	iReadLength = 0;
	if( vData )
	{
		delete vData;
		vData = NULL;
	}
	for(vector<rtp_proxy_buf_t>::iterator c = m_RtpProxyBuf.begin();
		c < m_RtpProxyBuf.end() ; c++)
    {
#ifdef LINUX
		if( c->outRtpHandle )
			close(c->outRtpHandle);
		if( c->outRtcpHandle )
			close(c->outRtcpHandle);
		if( c->inRtpHandle )
			close(c->inRtpHandle );
		if( c->inRtcpHandle )
			close(c->inRtcpHandle );
#elif defined WIN32
		if( c->outRtpHandle )
			closesocket(c->outRtpHandle);
		if( c->outRtcpHandle )
			closesocket(c->outRtcpHandle);
		if( c->inRtpHandle )
			closesocket(c->inRtpHandle );
		if( c->inRtcpHandle )
			closesocket(c->inRtcpHandle );
#endif
    }
	if( h_inRTPMANAGER )
		CloseHandle(h_inRTPMANAGER);
	if( h_outRTPMANAGER )
		CloseHandle(h_outRTPMANAGER);
	if( h_inRTCPMANAGER )
		CloseHandle(h_inRTCPMANAGER);
	if( h_outRTCPMANAGER )
		CloseHandle(h_outRTCPMANAGER);
	if(!m_RtpProxyBuf.empty() )
		m_RtpProxyBuf.erase(m_RtpProxyBuf.begin(), m_RtpProxyBuf.end());
	if( hRtpSleepEvent )
		CloseHandle(hRtpSleepEvent);
	return 1;
}

int start_rtp_manager(int u_num, int start_port)
{
    int i;
    rtp_num = u_num;
	RTPMAX_USER = u_num;
    //int memsize = sizeof(rtp_proxy_buf_t) *u_num;
    if( start_port > 0)
        rtp_start_port = start_port;
    else
        rtp_start_port = RTP_OUT_DEF_PORT_START;
    //pRtpProxyBuf = NULL;
    //pRtpProxyBuf = (p_rtp_proxy_buf_t)malloc(memsize);
	//memset( pRtpProxyBuf, 0, memsize);
    //if( !pRtpProxyBuf )
      //  return 0;
    enable_rtp_manager_flag();
	iReadLength = 0;
	//rtp_manager_flag = 1;
	vData = NULL;
	iReadLength = 0;
	LoadVoiceData();
    for(i= 0;i<u_num;i++)
    {
		p_rtp_proxy_buf_t p = new rtp_proxy_buf_t;
		if( p )
		{
			p->inRtpPort = rtp_start_port + i*2;
			p->inRtcpPort = rtp_start_port + i*2 + 1;
			if( bEchoEnable )
			{
				p->outRtpPort = rtp_start_port + /*RTP_RTCP_GAP + */ rtp_num*2 +i*2;
				p->outRtpHandle = create_udp_sock(p->outRtpPort);
				if( bRtcpCtrlEnable )
				{
					p->outRtcpPort = rtp_start_port + /*RTP_RTCP_GAP + */rtp_num*2 + i*2 + 1;
					p->outRtcpHandle = create_udp_sock(p->outRtcpPort);
				}
			}
			p->inRtpHandle = create_udp_sock(p->inRtpPort);
			p->inRtcpHandle = create_udp_sock(p->inRtcpPort);
			m_RtpProxyBuf.push_back(*p);
		}
	}

	hRtpSleepEvent = CreateEvent(NULL, FALSE, FALSE, "RTP_SLEEP_EVENT");

	if( bEchoEnable )
	{
	    h_outRTPMANAGER = sky_thread_create(rtp_out_manager,NULL, 0, 0);
		if( bRtcpCtrlEnable )
		{
			h_outRTCPMANAGER = sky_thread_create(rtcp_out_manager,NULL, 0, 0);
		}
		if( !h_outRTPMANAGER || (bRtcpCtrlEnable && !h_outRTCPMANAGER ) )
		{
			if( hRtpSleepEvent )
			{
				CloseHandle(hRtpSleepEvent);
				hRtpSleepEvent = NULL;
			}
			return 0;
		}
	}
	h_inRTPMANAGER = sky_thread_create(rtp_in_manager,NULL, 0, 0);
	if( bRtcpCtrlEnable )
	{
		h_inRTCPMANAGER = sky_thread_create(rtcp_in_manager,NULL, 0, 0);
	}
	if( !h_inRTPMANAGER || (bRtcpCtrlEnable && !h_inRTCPMANAGER))
	{
		if( hRtpSleepEvent )
		{
			CloseHandle(hRtpSleepEvent);
			hRtpSleepEvent = NULL;
		}
		return 0;
	}

    //disable_rtp_manager_flag();
	if( hRtpSleepEvent )
		WaitForSingleObject(hRtpSleepEvent, 200);
	else
	    Sleep(200);
    return 1;
}