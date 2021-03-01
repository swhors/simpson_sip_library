#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
typedef void * SKYTHREAD;
typedef void * SKYINVAL;
typedef int    SKYRET;
typedef int    SKYHANDLE;
#define THREAD_RETURN NULL
#elif   defined  WIN32 || defined WINCE
//typedef DWORD  WINAPI SKYTHREAD;
typedef DWORD  SKYTHREAD;
typedef LPVOID SKYINVAL;
typedef HANDLE SKYRET;
typedef HANDLE SKYHANDLE;
#define THREAD_RETURN 0
#endif


SKYRET sky_thread_create(IN void *p_func,IN void *p_arg,IN int ext_opt, int memsize);

void sky_thread_close(SKYHANDLE id);
