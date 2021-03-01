#include <stdio.h>
#include <stdlib.h>

#include "sky_def.h"
#include "sky_thread.h"

#ifdef WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#endif

static SKYRET sky_thread_create_internal(IN void *p_func,IN void *p_arg,IN int ext_opt, int memsize)
{
#ifdef LINUX
    pthread_t reg_thr;
    pthread_attr_t attr;
    bzero(&reg_thr, sizeof( reg_thr));
    bzero(&attr, sizeof(attr));
    pthread_attr_init(&attr);
	
	if( ext_opt )
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
	if(!pthread_create(&reg_thr, &attr, p_func, p_arg))
    {
        return 1;
    }
#ifdef MAIN_TERMINAL_MODE
    perror("pthread_create");
#endif
#elif defined WIN32
    DWORD dwId;
	HANDLE hd;
	hd = CreateThread(NULL,memsize,(LPTHREAD_START_ROUTINE)p_func, p_arg, 0, &dwId);
    if(hd)
        return hd;
#endif
    return 0;
}

SKYRET sky_thread_create(IN void *p_func,IN void *p_arg,IN int ext_opt, int memsize)
{
	return sky_thread_create_internal(p_func, p_arg,ext_opt,memsize);
}

void sky_thread_close(SKYHANDLE thread)
{
#ifdef WIN32
	CloseHandle(thread);
#elif defined LINUX
	pthread_exit(NULL);
	;
#endif
}

