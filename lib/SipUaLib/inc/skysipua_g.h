#pragma once

#ifndef __SKY_SIP_UA_G_H
#define __SKY_SIP_UA_G_H

#ifdef LINUX // OS_DEP_DEF
#define SKYLIB 
#elif defined WIN32 // OS_DEP_DEF
#define SKYLIB __declspec(dllexport)
#elif defined WINCE
#define SKYLIB __declspec(dllexport)
#endif // OS_DEP_DEF

#define SKY_SOCK_TCP    1
#define SKY_SOCK_UDP    2

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#ifndef PVOID
typedef void * PVOID;
#endif // PVOID
#ifndef BYTE
typedef unsigned char BYTE;
typedef unsigned char * PBYTE;
#endif

#endif // __SKY_SIP_UA_G_H