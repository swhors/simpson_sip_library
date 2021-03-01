////////////////////////////////////////////////////////////////////////////
// File : sip_sdp_parse.c
// 설명 : sdp를 파싱하고, 메세지를 생성하는 함수.
// 날짜 : 2005/09/15 ( swhors@skycom.ne.kr )
// 
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Include
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <memory.h>
#endif

#include <time.h>

#include <iostream>
#include <string>

using namespace std;

#include "host_addr.h"
#include "skytool.h"
#include "debug.h"
#include "SIpFieldBase.h"
#include "sip_tool.h"


#include "sip_sdp.h"
#include "sdp_media.h"
#include "sip_sdp_parse.h"

////////////////////////////////////////////////////////////////////////////
// Defination & Variables
////////////////////////////////////////////////////////////////////////////
#define SDP_ITEM_COUNT    7

// global define
static sdp_media_t sdpMedia[4]=
{
    {SDP_MEDIA_G711U_N,SDP_MEDIA_G711U_S},
    {SDP_MEDIA_G729_N,SDP_MEDIA_G729_S},
    {SDP_MEDIA_G723_N,SDP_MEDIA_G723_S},
    {0,NULL}
};

static sdp_parse_t sdpParse[SDP_ITEM_COUNT];

////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////

void bzero_sdp_item(OUT PVOID _pSdp)
{
    int i = 0;
    p_sdp_info_t pSdp = (p_sdp_info_t)_pSdp;
    if(! pSdp )
        return;
    pSdp->att_count = 0;
    memset(pSdp->basic.owner_address, 0, 64);
    pSdp->basic.owner_address_type = OWNER_ADDR_TYPE_NO;
    memset(pSdp->basic.owner_name, 0, 48);
    pSdp->basic.owner_network_type = OWNER_NET_TYPE_NO;
    pSdp->basic.session_id = 0;
    pSdp->basic.session_version = 0;
    memset( pSdp->con_info.Address, 0, 64);
    pSdp->con_info.networktype = OWNER_NET_TYPE_NO;
    pSdp->con_info.AddressType = OWNER_ADDR_TYPE_NO;
    pSdp->media_des.media_count = 0;
    for(i=0;i<10;i++)
        pSdp->media_des.media_format[i] = 0;
    memset( pSdp->media_des.media_proto, 0, 16);
    memset( pSdp->media_des.media_type, 0, 16);
    pSdp->media_des.port = 0;
    for(i = 0; i < 13; i++)
    {
        memset(pSdp->pMediaAtt[i].attr_fieldname, 0, 16);
        memset(pSdp->pMediaAtt[i].attr_value, 0, 32);
    }
}

int get_sdp_item(char *inbuf, char *tok, void *p, int count, int buflen)
{
    int i = 0;
    char *stok,*outbuf;
    outbuf = (char *)p;
    
    memset(outbuf, 0, buflen * count);
    stok = strtok(inbuf, tok);
    while(stok)
    {
        if( i++ < count )
        {
            memcpy(outbuf, stok, strlen(stok));
            outbuf = outbuf + buflen;
        }
        stok = strtok(NULL, tok);
    }
    return i;
}

// ..
int sdp_parse_mediaatt(char *pbuff, void *_phd)
{
    p_sdp_media_att phd = (p_sdp_media_att)_phd;
    char sfind[2][64],*tbuf;
    int i = 0;
    if( *pbuff == ' ')
        pbuff++;
    tbuf = pbuff;
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;

    }

    i = get_sdp_item(pbuff, ":", sfind, 2, 64);
#ifdef WIN32
    sprintf_s(phd->attr_fieldname,16, "%s", sfind[0]);
#else
	sprintf(phd->attr_fieldname, "%s", sfind[0]);
#endif
    if(i > 1 )
#ifdef WIN32
        sprintf_s(phd->attr_value, 32,"%s", sfind[1]);
#else
		sprintf(phd->attr_value, "%s", sfind[1]);
#endif
    else
        memset(phd->attr_value, 0, 32);
    return 1;
}

int sdp_parse_mediades(char *pbuff, void *_phd)
{
    p_sdp_media_des_t phd = (p_sdp_media_des_t)_phd;
    char sfind[20][64],*tbuf;
    int i = 0;
    if( *pbuff == ' ')
        pbuff++;
    tbuf = pbuff;
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;
    }
    i = get_sdp_item(pbuff, " ", sfind, 20, 64);
    if( i < 4 )
        return 0;
#ifdef WIN32
    sprintf_s(phd->media_type, 16,"%s", sfind[0]);
#else
    sprintf(phd->media_type, "%s", sfind[0]);
#endif
    phd->port = atoi(sfind[1]);
#ifdef WIN32
    sprintf_s(phd->media_proto, 16,"%s", sfind[2]);
#else
    sprintf(phd->media_proto, "%s", sfind[2]);
#endif
    phd->media_count = 0;
    for( i = 3; i < 10; i++)
    {
        if( strlen( sfind[i]) > 0)
            phd->media_format[phd->media_count++] = atoi(sfind[i]);
        else
            break;
    }
    return 1;
}

int sdp_parse_timedes(char *pbuff, void *_phd)
{
    p_sdp_time_dse_t phd = (p_sdp_time_dse_t)_phd;
    char sfind[2][64],*tbuf;
    int i=0;
    if( *pbuff == ' ')
        pbuff++;
    tbuf = pbuff;
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;
    }

    i = get_sdp_item(pbuff, " ", sfind, 2, 64);
    
    if( i != 2)
        return 0;
    
    phd->start_time = atoi(sfind[0]);
    phd->stop_time = atoi(sfind[1]);
    
    return 1;
}

int sdp_parse_connection_info(char *pbuff, void *_phd)
{
    p_sdp_connection_info_t phd = (p_sdp_connection_info_t)_phd;
    char sfind[3][64],*tbuf;
    int i=0;
    
    if( *pbuff == ' ')
        pbuff++;
    
    tbuf = pbuff;
    
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;

    }
    
    i = get_sdp_item(pbuff, " ", sfind, 3, 64);
    
    if( i != 3 )
        return 0;
    
    if(!memcmp(sfind[0], "IN", 2))
        phd->networktype = OWNER_NET_TYPE_IN;
    else if( !memcmp(sfind[0], "OUT", 3))
        phd->networktype = OWNER_NET_TYPE_OUT;
    else
        phd->networktype = OWNER_NET_TYPE_NO;
    
    if( !memcmp(sfind[1], "IP4", 3))
        phd->AddressType = OWNER_ADDR_TYPE_IP4;
    else if( !memcmp(sfind[1], "IP6", 3))
        phd->AddressType = OWNER_ADDR_TYPE_IP6;
    else
        phd->AddressType = OWNER_ADDR_TYPE_OTH;
#ifdef WIN32
    sprintf_s(phd->Address,64,
#else
    sprintf(phd->Address,
#endif
		"%s", sfind[2]);
    
    return 1;
}

int sdp_parse_owner_info(char *pbuff, void *_phd)
{
    p_sdp_owner_info_t phd = (p_sdp_owner_info_t)_phd;
    char sfind[6][64],*tbuf;
    int i=0;
    if( *pbuff == ' ')
        pbuff++;
    tbuf = pbuff;
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;

    }
    i = get_sdp_item(pbuff, " ", sfind, 6, 64);
    if(i <=5)
        return 0;
    
    // owner name 
#ifdef WIN32
    sprintf_s(phd->owner_name, 48,"%s", sfind[0]);
#else
    sprintf(phd->owner_name, "%s", sfind[0]);
#endif
    // id
    phd->session_id = atoi(sfind[1]);
    
    // version
    phd->session_version = atoi(sfind[2]);
    
    // network type
    if( !memcmp(sfind[3], "IN", 2))
        phd->owner_network_type = OWNER_NET_TYPE_IN;
    else if( !memcmp(sfind[3], "OUT", 3))
        phd->owner_network_type = OWNER_NET_TYPE_OUT;
    else
        phd->owner_network_type = OWNER_NET_TYPE_NO;
    
    // address type
    if( !memcmp(sfind[4], "IP4", 3))
        phd->owner_address_type = OWNER_ADDR_TYPE_IP4;
    else if( !memcmp(sfind[4], "IP6", 3))
        phd->owner_address_type = OWNER_ADDR_TYPE_IP6;
    else
        phd->owner_address_type = OWNER_ADDR_TYPE_OTH;
    // owner address
#ifdef WIN32
    sprintf_s(phd->owner_address, 64,"%s", sfind[5]);
#else
    sprintf(phd->owner_address, "%s", sfind[5]);
#endif
    return 1;
}

int sdp_parse_version(char *pbuff, void *phd)
{
    int * i = (int*)phd;
    *i = atoi(pbuff);
    return *i;
}

int sdp_parse_session(char *pbuff, void *phd)
{
    char * session = (char*)phd;
    char *tbuf;
    tbuf = pbuff;
    while(tbuf)
    {
        if( *tbuf== '\r' || *tbuf == '\n')
        {
            *tbuf = 0;
            break;
        }
        else
            tbuf++;

    }
    sprintf(session, "%s", pbuff);
    return 1;
}

void modify_sdp(char *ip, int port, void *_phd)
{
    p_sdp_info_t phd = (p_sdp_info_t)_phd;
    sprintf(phd->basic.owner_address, "%s", ip);
    sprintf(phd->con_info.Address, "%s", ip);
    phd->media_des.port = port;
}

int make_sdp_msg(void *_phd, char *msg, int buflen)
{
    int i;

    p_sdp_info_t phd = (p_sdp_info_t)_phd;

    if( !_phd )
        return 0;
    if( strlen(phd->basic.owner_address)<=0)
        return 0;
    // version information
    strcatfm(msg, "%s%d\r\n", SDP_VER_PRE, phd->version);
    // owner informaion
    strcatfm(msg, "%s%s %d %d", SDP_OWNERINFO_PRE, 
        phd->basic.owner_name,
        phd->basic.session_id,
        phd->basic.session_version);
    if( phd->basic.owner_network_type == OWNER_NET_TYPE_IN )
        strcat(msg," IN");
    else
        strcat(msg," OUT");
    if( phd->basic.owner_address_type == OWNER_ADDR_TYPE_IP4 )
        strcat(msg, " IP4");
    else
        strcat(msg, " IP6");
    strcatfm(msg, " %s\r\n", phd->basic.owner_address);
    // session name
    strcatfm(msg, "%s%s\r\n", SDP_SESSION_PRE, phd->session_name);
    // connection information
    strcat(msg, SDP_CONINFO_PRE);
    if( phd->con_info.networktype == OWNER_NET_TYPE_IN )
        strcat(msg,"IN");
    else
        strcat(msg,"OUT");
    if( phd->con_info.AddressType == OWNER_ADDR_TYPE_IP4 )
        strcat(msg, " IP4");
    else
        strcat(msg, " IP6");
    strcatfm(msg, " %s\r\n", phd->con_info.Address);
    // Time description
    strcatfm(msg, "%s%d %d\r\n", 
        SDP_TIMEDES_PRE,
        phd->time_des.start_time, 
        phd->time_des.stop_time);
    // Media Description
    strcatfm(msg, "%s%s %d %s", SDP_MEDIADES_PRE,
        phd->media_des.media_type, phd->media_des.port,
        phd->media_des.media_proto);
    for( i = 0; i < phd->media_des.media_count; i++)
    {
        strcatfm(msg, " %d", phd->media_des.media_format[i]);
    }
    strcat(msg, "\r\n");
    // Media  Attribute
    for( i = 0; i < phd->att_count; i++)
    {
        int vallen;
        strcatfm(msg, "%s%s",
                   SDP_MEDIAATT_PRE,
                   phd->pMediaAtt[i].attr_fieldname);
        vallen = strlen(phd->pMediaAtt[i].attr_value);
        if( vallen > 0)
        {
            char *cur = &msg[strlen(msg)];
            *cur++ = ':';
            memcpy(cur, phd->pMediaAtt[i].attr_value, vallen);
            //strcatfm(msg, ": %s\r\n", phd->pMediaAtt[i].attr_value);
            strcat(msg, "\r\n");
        }
        else
            strcat(msg, "\r\n");
    }
    return 1;
}

// main parse..
int sdp_parse(char *inbuf, int inlen, void * _pSip)
{
    p_sdp_info_t pSip = (p_sdp_info_t)_pSip;
    char p_inbuf[512],*tbuf;
    int i,iitem,b, media_att_count = 0,ilen;
    char sline[25][128];
    memset(p_inbuf, 0, 512);
    memcpy(p_inbuf, inbuf, inlen);

	sdpParse[0].fn = sdp_parse_mediaatt;
	sdpParse[0].prefix = SDP_MEDIAATT_PRE;

	sdpParse[1].fn = sdp_parse_mediades;
	sdpParse[1].prefix = SDP_MEDIADES_PRE;
	
	sdpParse[2].fn = sdp_parse_timedes;
	sdpParse[2].prefix = SDP_TIMEDES_PRE;
	
	sdpParse[3].fn = sdp_parse_connection_info;
	sdpParse[3].prefix = SDP_CONINFO_PRE;
	
	sdpParse[4].fn = sdp_parse_owner_info;
	sdpParse[4].prefix = SDP_OWNERINFO_PRE;
	
	sdpParse[5].fn = sdp_parse_version;
	sdpParse[5].prefix = SDP_VER_PRE;
	
	sdpParse[6].fn = sdp_parse_session;
	sdpParse[6].prefix = SDP_SESSION_PRE;

    iitem = get_sdp_item(&p_inbuf[0], "\n", sline, 25, 128);
    if( iitem <= 0)
        return 0;
    for( i = 0; i<iitem; i++)
    {
		tbuf = &p_inbuf[i];
		ilen = strlen(tbuf);
		for( b = 0; b < ilen; b++)
		{
			if( *tbuf == '\n' || *tbuf == 'n')
			{
				*tbuf = 0;
			}
			tbuf++;
		}
        for( b = 0; b < SDP_ITEM_COUNT; b++)
        {
            if(!memcmp( sline[i], sdpParse[b].prefix, 2))
            {
                SDP_PARSE_FN *pfn;
                pfn = (SDP_PARSE_FN*) sdpParse[b].fn;
                if( pfn )
                {
                    switch(b)
                    {
                    case 0:
                        pfn( sline[i]+2, &pSip->pMediaAtt[media_att_count++]);
                        break;
                    case 1:
                        pfn( sline[i]+2, &pSip->media_des);
                        break;
                    case 2:
                        pfn( sline[i]+2, &pSip->time_des);
                        break;
                    case 3:
                        pfn( sline[i]+2, &pSip->con_info);
                        break;
                    case 4:
                        pfn( sline[i]+2, &pSip->basic);
                        break;
                    case 5:
                        pfn( sline[i]+2, &pSip->version);
                        break;
                    case 6:
                        pfn( sline[i]+2, &pSip->session_name);
                        break;
                    }
                }
            }
        }
    }
    if( media_att_count > 0)
        pSip->att_count = media_att_count;
    else
        pSip->att_count = 0;
    return 1;
}

static int generate_session_id()
{
    time_t tv;
    time(&tv);
    srand((unsigned int)tv);
    return rand();
}

void build_sdp_ipv4_out_info(OUT PVOID _pSdp,
                             IN char *hostip,
                             IN char* u_num,
                             IN int attcount)
{
    // fill basic information;
    p_sdp_info_t pSdp = (p_sdp_info_t)_pSdp;
#ifdef WIN32
    sprintf_s(pSdp->basic.owner_address,64, "%s",hostip);
#else
    sprintf(pSdp->basic.owner_address,"%s",hostip);
#endif

    pSdp->basic.owner_address_type = OWNER_ADDR_TYPE_IP4;
#ifdef WIN32
    sprintf_s(pSdp->basic.owner_name,48, "%s", u_num);
#else
    sprintf(pSdp->basic.owner_name,"%s", u_num);
#endif
    pSdp->basic.session_id = generate_session_id();
    pSdp->basic.session_version = pSdp->basic.session_id;
    pSdp->basic.owner_network_type = OWNER_NET_TYPE_IN;
    //fill timer information;
    //pSdp->time_des.start_time = time(&tv);
	pSdp->time_des.start_time = 0;
    pSdp->time_des.stop_time = 0;
    // fill connection iformation
    sprintf(pSdp->con_info.Address,"%s",hostip);
    pSdp->con_info.AddressType = OWNER_ADDR_TYPE_IP4;
    pSdp->con_info.networktype = OWNER_NET_TYPE_IN;
    // att count
    pSdp->att_count = attcount;
    // session name
#ifdef WIN32
    sprintf_s(pSdp->session_name,128, "%s", "SKY SIP UserAgent SDP");
#else
    sprintf(pSdp->session_name,"%s", "SKY SIP UserAgent SDP");
#endif
    // version
    pSdp->version = 0;
}

void build_sdp_media_attribute(OUT PVOID _pMediaAtt,IN char *fieldName,IN char *value)
{
    p_sdp_media_att pMediaAtt = (p_sdp_media_att)_pMediaAtt;
#ifdef WIN32
    sprintf_s(pMediaAtt->attr_fieldname,16,"%s",fieldName);
#else
    sprintf(pMediaAtt->attr_fieldname,"%s",fieldName);
#endif
	if( value )
	{
		if( strlen( value ) > 0)
#ifdef WIN32
            sprintf_s(pMediaAtt->attr_value,32,"%s",value);
#else
            sprintf(pMediaAtt->attr_value,"%s",value);
#endif
		else
			memset(pMediaAtt->attr_value, 0, 32);
	}
}

void build_sdp_media_description(OUT PVOID _pMediaDesc,IN int count,IN int *format)
{
    int i;
    p_sdp_media_des_t pMediaDesc = (p_sdp_media_des_t)_pMediaDesc;
    pMediaDesc->media_count = count;
#ifdef WIN32
    sprintf_s(pMediaDesc->media_proto,16, "%s", "RTP/AVP");
    sprintf_s(pMediaDesc->media_type,16, "%s","audio");
#else
    sprintf(pMediaDesc->media_proto,"%s", "RTP/AVP");
    sprintf(pMediaDesc->media_type,"%s","audio");
#endif
    for(i = 0; i< 10; i++)
    {
        pMediaDesc->media_format[i] = 0;
    }
    for( i = 0; i< count ; i++)
    {
		if( format[i] >= 0)
            pMediaDesc->media_format[i] = format[i];
    }
}

_sdp_parse_t::_sdp_parse_t()
{
	fn = NULL;
	prefix=NULL;
}


//
////////////////////////////////////////////////////////////////////////////
//
