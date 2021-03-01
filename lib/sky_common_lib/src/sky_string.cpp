#include <stdio.h>
#include <stdarg.h>

#include "sky_string.h"

#ifdef WIN32
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

#include "skytool.h"

char * findString(char *tbuf, char *key, int keylen)
{
	char * pt=tbuf;
    while(*pt)
	{
		if( *pt == *key)
		{
			if( keylen == 1)
				return pt;
			if(!memcmp(pt, key, keylen))
			{
				return pt;
			}
		}
		pt++;
	}
	return NULL;
}

int strprintf(OUT char *buff,IN int buflen,const char * fmt,...)
{
    va_list ap;
    memset(buff, 0, buflen);
    va_start(ap, fmt);
#ifdef LINUX
    vsnprintf(buff, buflen, fmt, ap);
#elif defined WIN32
    _vsnprintf(buff, buflen, fmt, ap);
#endif // osdef
    va_end(ap);
    return strlen( buff);
}

char * strcatfm(char * buf, const char * fmt, ...)
{
    char buff[1024];
    char * ttp;
    va_list ap;
    memset( buff, 0, 1024);
    // start - get parameter
    va_start(ap, fmt);
#ifdef LINUX
    vsnprintf(buff,1024,fmt, ap);
#elif defined WIN32
    _vsnprintf(buff,1024,fmt, ap);
#endif
    va_end(ap);
    // end - get parameter
    ttp = strcat(buf, buff);
    if( ttp )
        return ttp;
    return NULL;
}

char * skystrlwr(char * inbuf)
{
        char * tbuf = inbuf;
	while(*tbuf)
	{
		if( *tbuf>= 65 && *tbuf <=90 ) *tbuf+=32;
		tbuf++;
	}
	return inbuf;
}

char * skystrupr(char *inbuf)
{
    char * tbuf=inbuf;
    while(*tbuf)
    {
        if( *tbuf >= 97 && *tbuf <= 122 ) *tbuf-=32;
            tbuf++;
    }
    return inbuf;
}
