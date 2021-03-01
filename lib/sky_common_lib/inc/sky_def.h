#pragma once

typedef void *			PVOID;

#ifdef WINCE
typedef unsigned long	DWORD;
typedef void *			LPVOID;

typedef PVOID			HANDLE;
#endif

typedef unsigned char UCHAR, * PUCHAR;

typedef unsigned int  UINT, * PUINT;

typedef unsigned char BYTE, *PBYTE;

#if defined WINCE || defined WIN32
#define IN
#endif

#if defined WIN32 || defined WINCE
#define OUT
#endif

#define CRNL	"\r\n"

#ifdef WINCE
#define NULL	0
#endif