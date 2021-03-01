#ifndef ___MSVC_UP_
#define ___MSVC_UP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// stract 
#ifdef MSVC_6
#define STRCAT(a,b,c) strcat(a,c)
#define STRCAT1(a,b) strcat(a,b)
#else  // over MSVC_6
#define STRCAT(a,b,c) strcat_s(a,b,c)
#define STRCAT1(a,b) strcat_s(a, strlen(a), b)
#endif

// sprintf(a,b)
#ifdef MSVC_6
#define SPRINTF1b(a,b,c,d) sprintf(a,c,d)
#else  // over MSVC_6
#define SPRINTF1b(a,b,c,d) sprintf_s(a,b,c,d)
#endif

#endif
