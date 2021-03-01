/*
 SkyTimer.cpp
 이 파일은 SetTimer에 관련된 공용 함수의 선언 부분입니다.
 
 2005/10/13 swhors@skycom.ne.kr
*/
#include <stdio.h>
#include "skytool.h"
#ifdef LINUX
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#elif defined WIN32
#include <io.h>
#include <windows.h>
#include <string.h>
#include <memory.h>
#endif

void SkySetTimer(IN void *handle,IN int id,IN int interval,IN void * pFunc)
{
#ifdef LINUX
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = *pFunc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);
    alarm(interval);
#elif defined WIN32
	HWND *phWnd = (HWND*)handle;
	SetTimer(*phWnd, id, interval, (TIMERPROC)pFunc);
#endif
}

//////////////////////////////////////////////////////
// End of File 
//////////////////////////////////////////////////////
