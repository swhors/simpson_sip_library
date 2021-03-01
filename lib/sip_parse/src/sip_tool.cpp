//
// sip_tool.c
// hostaddr�� �޸𸮸� �Ҵ��ϰ�, �ı��ϰų�, �����͸� ���� �ִ� �Լ�.
// sip�� ������ �Ľ��ϰų�, �������� Ÿ���� �Ľ��ϴ� �Լ�.
// user_id�� �Ľ��ϴ� �Լ�.
//
// 2004/12/xx by swhors@skycom.ne.kr
//

#include <stdio.h>
#if defined WIN32 || defined WINCE
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#ifdef WINCE
#include <winbase.h>
#include "WinCETimeFixups.h"
#endif
#endif
#include <time.h>

#include <iostream>
#include <string>
using namespace std;
#include "host_addr.h"

#include "skytool.h"
#include "debug.h"

#include "sip_allow.h"
#include "sip_tool.h"

void fill_sip_version(PVOID p_ver, int major, int minor)
{
    p_sip_version_t pver = (p_sip_version_t)p_ver;
    pver->major = major;
    pver->minor = minor;
}
//SIP�� ������ ��ȯ�Ѵ�.
//�Է� :
//     PUCHAR dump - �Է� �޸� 
//��� :
//     p_sip_version_t ver;
//��ȯ :
//     int - success 1, fail 0
char * parse_sip_version(PUCHAR dump,p_sip_version_t ver)
{
    PUCHAR cur=NULL,dot=NULL;
    cur = (PUCHAR)strstr((char*)dump,SIP_INFO_PREFIX);
    ver->major = 0;
    ver->minor = 0;
    if( cur )
    {
        cur = cur + 4;
        dot = (PUCHAR)strstr((char*)cur, ".");
        if( dot)
        {
            ver->minor = atoi( (char*)(++dot));
        }
        ver->major = atoi((char*)cur);
        return (char*)(cur + 3);
    }
    return NULL;
}

/*
 * sip_protocol_type
 * �Է� : 
 *      PUCHAR dump - �Է� �޸�
 * ��ȯ :
 *      �������� Ÿ�� - 1:TCP 2:UDP
*/
char * parse_sip_protocol_type(PUCHAR dump, int * type)
{
    PUCHAR cur = dump;
    if( ++cur )
    {
        if( *cur == 'T' && *(cur+1) == 'C' && *(cur+2) == 'P')
        {
            *type = 1;
            return (char*)(cur+3);
        }
        if( *cur == 'U' && *(cur+1) == 'D' && *(cur+2) == 'P')
        {
            *type = 2;
            return (char*)(cur+3);
        }
    }
    *type = 0;
    return NULL;
}

int	HostaddrToStr(PVOID p_src, char *tbuf, int buflen)
{
	p_hostaddr_t psrc = (p_hostaddr_t)p_src;
	if( psrc && tbuf )
	{
		char ttbuf[32];
		memset(tbuf, 0, buflen);
		sprintf_s(ttbuf, 32, "%s%d", (psrc->port > 0?":":""),psrc->port);
		sprintf_s(tbuf, buflen, "%s%s", (psrc->hostAddr.empty()?"":(char*)psrc->hostAddr.data()), (psrc->port > 0?ttbuf:""));
		return strlen(tbuf);
	}
	return 0;
}

// �־��� ���ۿ� �ش� ȣ��Ʈ �ּ��� ������
// �����Ѵ�.
// create 2005/03/10 pm2:07 by swhors@skycom.ne.kr
// PVOID p_dest : ������ ����
// PVOID p_src  : ���� ����
int hostaddrcpy(PVOID p_dest,PVOID p_src)
{
    p_hostaddr_t psrc, pdest;
    pdest = (p_hostaddr_t)p_dest;
    psrc = (p_hostaddr_t)p_src;
    if( !pdest || !psrc )
        return 0;
	pdest->type = (addr_type_t)psrc->type;
    pdest->port = psrc->port;
	pdest->hostAddr = psrc->hostAddr;
    return 1;
}

// �־��� �� ���� ȣ��Ʈ ��巹���� ���Ͽ�
// ��ġ �� ��� 0��, �ƴ� ��� 1�� ��ȯ�Ѵ�.
// create 2005/03/09 swhors@skycom.ne.kr
// PVOID p_src : p_hostaddr_t ������ �ּ�
// PVOID p_dest : p_hostaddr_t ������ �ּ�
int hostaddrcomp(PVOID p_src, PVOID p_dest)
{
    p_hostaddr_t psrc,pdest;
    psrc = (p_hostaddr_t) p_src;
    pdest = (p_hostaddr_t)p_dest;
    if( !psrc || !pdest )
        return 1;
	if( *psrc == *pdest )
		return 1;
    return 0;
}

void init_hostaddr_buf(PVOID p_buf)
{
    p_hostaddr_t psrc = (p_hostaddr_t) p_buf;
	if( psrc )
		psrc->init();
}

/* 
 SIP requestline�� host_addr ������ ��ȯ 
 input  : 
        PUCHAR dump_mem - �Է� �޸�
 output :
        p_hostaddt_t host - ȣ��Ʈ�� �ּ�
 return :
       0-fail 1-success
*/
int parse_hostaddr( PUCHAR dump_mem, p_hostaddr_t host)
{
	if( host && dump_mem)
	{
		return (host->parse(dump_mem)?1:0);
	}
	return 0;
}

int build_ip_hostaddr(OUT PVOID _pHost,IN char *hostip,IN int port)
{
    p_hostaddr_t pHost = (p_hostaddr_t)_pHost;
	if( pHost )
	{
		return pHost->build(ip, hostip, port);
	}
	return 0;
}

/*
 * �Է� :
 *      unsigned char * dump - �Է� �޸� 
 * ��ȯ :
 *      new_line pointer(fail: null)
*/
unsigned char *get_new_line(unsigned char * dump)
{
    return (PUCHAR)strstr( (char*)dump, "\n");
}

/*������� ID�� ������.*/
int parse_userid(PUCHAR dump, char* user)
{
    PUCHAR prefix,endfix;
    int len = 0;
    prefix = (PUCHAR)strstr( (char*)dump, USER_ID_PREFIX);
    if(!prefix)
        return 0;
    endfix = (PUCHAR)strstr((char*)prefix, ";");
    if( !endfix)
    {
        endfix = (PUCHAR)strstr((char*)prefix,">");
        if(!endfix )
            return 0;
    }
    len = (int)strlen((char*)prefix) - (int)strlen((char*)endfix)-5;
	memcpy(user, prefix+5, len);
	return strlen(user);
    //return copy_n_str(user, len, prefix+5);
}

int sip_parse_int(PUCHAR dump, int len)
{
    if( !dump )
        return 0;
    dump = dump + len+1;
    if( len > 20 )
        return 0;
    while(*dump == ' ')
        dump++;
    //printf("%s %d : %s [%d]\n", __FILE__, __LINE__, dump,atoi(dump));
    return atoi((char*)dump);
}

void free_hostaddr(p_hostaddr_t host)
{
#if 0
    if( host->type == h_name )
    {
        //free_str(&host->eadr.name.sadr);
    }
#endif
}


// �ƿ� �޼����� date �ʵ��� �޼����� �����Ѵ�.
// out : ��� ����
// in  : p_date_t Ÿ���� ������ ����
void make_date_msg(OUT PUCHAR msg, IN p_date_t pDate)
{
    char tbuf[128];
    int len;
    if( pDate->ichecked )
    {
        memset(tbuf, 0, 128);
        len = strftime(tbuf, 128, "%a, %d %b %Y %H:%M:%S", &pDate->ltm);
        if( len>0 )
        {
            strcatfm((char*)(char*)msg, "Date: %s %s\r\n", tbuf, pDate->tm_zone);
        }
    }
}

void build_date_msg(OUT PVOID _pDate)
{
    time_t tv;
    struct tm * tt;
    p_date_t pDate = (p_date_t)_pDate;
    pDate->ichecked = 1;
    time(&tv);
    tt = localtime(&tv);
    memcpy(&pDate->ltm, tt, sizeof(struct tm));
    sprintf_s(pDate->tm_zone,12, "%s","GMT");
}

#include "peg_def.h"
#include "peg_xsip.h"

using namespace peg_templ;

PBYTE parse_displayname(IN PBYTE inStr, OUT PBYTE outStr,int len)
{
    PBYTE tmp, start;
    C c;
	typedef Or<ALPHA,DIGIT,BLANK, Char<'-'>, Char<'*'>, Char<'+'>, Char<'\''>,Char<'\"'>,Char<'['>,Char<']'> > dis_chr1;
	typedef Or<dis_chr1, Char<' '> > dis_chr2;
	typedef And<dis_chr1, PlusRepeat<dis_chr2>,dis_chr1> display1;
	typedef And<dis_chr1, PlusRepeat<dis_chr2>, Char<'<'> > display4;
    typedef FindRepeat<display1> findDisplay1;
    typedef FindRepeat<display4> findDisplay4;
    tmp = inStr;
    if( tmp <= NULL || outStr <= NULL || len <= 0)
        return inStr;
    if(findDisplay4::Matches(tmp, &c,start))
    {
        memset(outStr, 0, len);
		if( *(tmp - 2 ) == ' ' )
			memcpy(outStr, start, (int)(tmp-start-2));
		else
			memcpy(outStr, start, (int)(tmp-start-1));
        if( *(tmp-1) == '<' )
            return tmp-1;
        return tmp;
    }
	else if(findDisplay1::Matches(tmp,&c,start))
	{
        memset(outStr, 0, len);
		memcpy(outStr, start, (int)(tmp-start));
		return tmp;
	}
    return inStr;
}

typedef FindRepeat<host> findHost;
typedef FindRepeat<IPv4address>findIPAddress;

typedef And<Char<':'>,PlusRepeat<DIGIT>> portNum;
typedef FindRepeat<portNum> findPortNum;

PBYTE parse_siphost(IN PBYTE inStr,OUT PVOID _pSipHost)
{
	if( _pSipHost && inStr)
	{
		return ((hostaddr_t*)_pSipHost)->parse(inStr);
	}
	return NULL;
}

int print_siphost(IN PBYTE outStr,OUT PVOID _pSipHost)
{

	if( _pSipHost && outStr)
	{
		return ((hostaddr_t*)_pSipHost)->print(outStr);
	}
	return 0;
}


/////////////////////
