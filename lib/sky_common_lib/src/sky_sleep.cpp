#include <stdio.h>
#include <time.h>

#include "sky_sleep.h"

#ifdef WIN32
#include <windows.h>
#endif
#ifdef LINUX
#include <sys/select.h>
#include <sys/types.h>
#endif

void ssleep(int sec, int usec)
{
#ifdef LINUX
    struct timeval tv;
    fd_set readfd;
    tv.tv_usec = usec;
    tv.tv_sec = sec;
    FD_ZERO(&readfd);
    FD_SET(0, &readfd);
    select(1, &readfd, 0, 0, &tv);
    FD_CLR(0, &readfd);
#else
	Sleep(sec*1000+usec);
#endif
}
