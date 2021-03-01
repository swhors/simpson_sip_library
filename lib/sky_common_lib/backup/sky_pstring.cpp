// file : sky_pstring.c
//      : pstring buffer�� �Ҵ��ϰ�, ����, �����ϴ� �Լ� ����.
// date : 2004/XX/XX swhors@skycom.ne.kr
//      : 2005/03/23 swhors@skycom.ne.kr
//      :   -> malloc�� ������ ���, ���̸� 0���� ��.
//      :   -> free_str���� ���̰� 0�� ���� �������� �ʵ��� ��.
// 
#include <stdio.h>
#include "sky_def.h"
#include "sky_pstring.h"
#ifdef WIN32
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

void init_string(PSTRING str)
{
	str->str = NULL;
	str->len = 0;
	str->p_reserve = NULL;
}

int copy_n_str(PSTRING str, int ilen, unsigned char * buff)
{
    str->len = 0;
    str->str = NULL;
    
    if( !buff )
        return 0;
    str->str = (char*)malloc(ilen+1);
    if( str->str )
    {
        memset( str->str, 0, ilen+1);
        memcpy( str->str, buff, ilen);
        str->len = ilen; 
        return ilen;
    }
    return 0;
}

int copy_str(PSTRING str, char *buff)
{
    int len;
    if( buff )
    {
        len = (int)strlen(buff);
        return copy_n_str(str, len, (PUCHAR)buff);
    }
    return 0;
}

int pstrcpy(PSTRING des_str, PSTRING src_str)
{
    // 2005/03/25 15:xx �Ʒ��� ���� ��ƾ�� ����.
    // by swhors@skycom.ne.kr
    //if( !src_str->len ) --> if( !src_str->len || !src_str->str)
    
    if( (src_str->len<=0) || !src_str->str)
        return 0;
    des_str->len = src_str->len;
    if( des_str->len > MAX_PS_BUF_LEN)
        des_str->len = MAX_PS_BUF_LEN;
	des_str->str = (char*)malloc( src_str->len+1);
    if(des_str->str)
    {
		memset(&des_str->str[0], 0, des_str->len+1);
        memcpy(des_str->str, src_str->str, des_str->len);
    }
    else
    {
        // 2005/03/23 insetered..
        // -- start
        des_str->len = 0;
        // -- end
    }
    return src_str->len;
}

void free_str(PSTRING buff)
{
    if(buff->str)
    {
        free( buff->str);
        buff->str = NULL;
        buff->len = 0;
    }
}

int sip_parse_string(unsigned char * dump, PSTRING subject, int len)
{
    int llen;
    PUCHAR cur;
    subject->str = NULL;
    subject->len = 0;
    llen = 0;
    if( !dump )
        return 0;
    dump = dump + len;
    while(*dump==' ' || *dump == '=' || *dump == ':')
        dump++;
    cur = dump;
    while(cur)
    {
        if( *cur == ' ' || *cur=='\r' || *cur=='\n' || *cur==0x0)
        {
            break;
        }
        cur++;
        llen ++;
    }
    if(llen <= 0)
        return 0;
    return copy_n_str(subject, llen, dump);
}

str_buf::str_buf()
{
	len =0;
	str=NULL;
	p_reserve=NULL;
}

str_buf &str_buf::operator=(str_buf const &other)
{
	if( str > 0 && len > 0 )
	{
		free(str);
	}
	this->len = other.len +1;
	str = (char*)malloc(len+1);
	if( str )
	{
		memset(str, 0, len+1);
		memcpy(str,other.str, len);
	}
	return *this;
}

str_buf &str_buf::operator=(char const *tbuf)
{
	if( str > 0 && len > 0 )
	{
		free(str);
	}
	this->len = strlen(tbuf);
	str = (char*)malloc(len+1);
	if( str )
	{
		memset(str, 0, len+1);
		memcpy(str, tbuf, len);
	}
	return *this;
}

int	str_buf::operator==(str_buf const &other)
{
	if( len == other.len )
	{
		if(memcmp(str, other.str, len) )
			return 1;
	}
	return 0;
}
int	str_buf::operator>(str_buf const &other)
{
	if( len == other.len )
	{
		if( memcmp(str, other.str, len ) > 0 )
			return 1;
	}
	return 0;
}
int	str_buf::operator<(str_buf const &other)
{
	if( len == other.len )
	{
		if( memcmp(str, other.str, len ) < 0 )
			return 1;
	}
	return 0;
}
int	str_buf::operator==(char const *other)
{
	if( other )
	{
		if( len == strlen(other))
		{
			if( memcmp(this->str, other, len ) == 0 )
				return 1;
		}
	}
	return 0;
}
int	str_buf::operator>(char const *other)
{
	if( other )
	{
		if( len == strlen(other))
		{
			if( memcmp(this->str, other, len ) > 0 )
				return 1;
		}
	}
	return 0;
}
int	str_buf::operator<(char const *other)
{
	if( other )
	{
		if( len == strlen(other))
		{
			if( memcmp(this->str, other, len ) < 0 )
				return 1;
		}
	}
	return 0;
}

//sec@voip-forum.or.kr