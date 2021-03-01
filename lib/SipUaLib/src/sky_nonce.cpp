#include <stdio.h>

#ifndef WINCE

#ifdef LINUX
#include <sys/time.h>
#elif defined WIN32
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#endif
#include <iostream>
#include <string>
using namespace std;
#include "host_addr.h"
#include "skytool.h"

#include "sip_tool.h"
#include "sky_nonce.h"

#include "md5.h"

static UCHAR nonce_secret[12];
static UCHAR last_nonce[48];
static UCHAR nonce_stats[3];

static time_t time_nonce;
static int nonce_age;

#ifdef WINCE
#define SKY_MUTEX_INI NULL
#endif

static SKY_LOCK mutex_nonce=SKY_MUTEX_INI;

static char * internal_build_nonce(void);
static long internal_check_nonce(char *nonce);
static void internal_init_nonce();
static void lock_nonce();
static void unlock_nonce();

static void lock_nonce()
{
    sky_mutex_lock(&mutex_nonce);
}

static void unlock_nonce()
{
    sky_mutex_unlock(&mutex_nonce);
}

/*Initializes the nonce generator*/
static void internal_init_nonce()
{
    int sec, usec;
#ifdef LINUX
    struct timeval tv;
#elif defined WIN32 || defined WINCE
    SYSTEMTIME tv;
#endif

    time_nonce = 0;
    nonce_age = 5;
#ifdef LINUX    
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;
    usec = tv.tv_usec;
#elif defined WIN32 || defined WINCE
    GetLocalTime(&tv);
    sec = tv.wSecond;
    usec = tv.wMilliseconds;
#endif
    srand(sec);
    sprintf_s((char*)&nonce_secret[0], 12, "%08x",(int)(rand()^usec));
#if defined WIN32 || defined WINCE
    mutex_nonce = 0;
#endif
    memset(last_nonce, 0, sizeof(last_nonce));
    memset(nonce_stats, 0, sizeof(nonce_stats));
}

void init_nonce()
{
    internal_init_nonce();
}

void close_nonce()
{
}
/*Builds a hashed, timestamped nonce*/
/*
  The nonce is dynamically allocated and 
  must be free()d later. The format is 
  like: MD5("$secret.$time").$time 
*/
static char * internal_build_nonce(void)
{
    char *ret;
    time_t t;

    lock_nonce();
    t = time(NULL);
    /* Regenerate nonce at most once every second */
    if(time_nonce != t)
    {
        unsigned char buf[32];
        unsigned char output[48];
        PUCHAR ptr;
        int i;
        time_nonce = t;
        
		sprintf_s((char*)&buf[0],32, "%s%ld", nonce_secret, (long)t);
        
		memset(output, 0, 48);
        md5_hash(buf, (int)strlen((char*)&buf[0]), output);
        ptr = last_nonce;
        for( i = 0; i < 16; i++)
        {
            ptr+=sprintf((char*)ptr,"%2.2x", output[i]);
        }
        ptr = last_nonce + strlen((char*)&last_nonce[0]);
        sprintf_s((char*)ptr, 32, "%ld", (long)t);
        nonce_stats[0]++;
    }
#ifdef CPP_OLD
    ret = strdup((char*)&last_nonce[0]);
#else
    ret = _strdup((char*)&last_nonce[0]);
#endif
    unlock_nonce();
    return ret;
}

char * build_nonce(void)
{
    return internal_build_nonce();
}

/*Return the age (in seconds) of the provided nonce*/
/*Returns negative if nonce is invalid or lies in the future */
static long internal_check_nonce(char *nonce)
{
    char buf[32], buf2[48],output[48];
    char *ptr;
    char *p;
    int i;
    time_t t;
    if (!(nonce && *nonce))
        return -1;
    /* First quick check if it matches the last generated nonce
      This saves us about 50% from going the long crypto way */
    lock_nonce();
    nonce_stats[1]++;
    t = strcmp(nonce, (char*)&last_nonce[0]) ? -1 : time(NULL) - time_nonce;
    unlock_nonce();
    if (t >= 0)
        return (int)t;
    /* Now isolate the dot and make sure a valid long int follows */
    p = strrchr(nonce, '.');
    if (!p)
        return -1;
    t = strtol(p+1, &p, 10);
    if (!p || *p)
        return -1;
    /* We have a possibly good nonce - recreate and compare */
    lock_nonce();
    nonce_stats[2]++;
    unlock_nonce();
#ifdef LINUX
    snprintf(buf, sizeof(buf), "%s%ld", nonce_secret, (long)t);
#elif defined WIN32
#ifdef CPP_OLD
    sprintf(buf, "%s%ld", nonce_secret, (long)t);
#else
    sprintf_s(buf, 32, "%s%ld", nonce_secret, (long)t);
#endif
#endif
    
    memset(output, 0, 48);
    ptr = &buf2[0];
    md5_hash((PUCHAR)&buf[0], (int)strlen(buf), (PUCHAR)&output[0]);
    for( i = 0; i < 16; i++)
    {
        ptr+=sprintf(ptr,"%2.2x", output[i]);
    }
    
    ptr = buf2 + strlen(buf2);
#ifdef LINUX
    snprintf(ptr, sizeof(buf2) - strlen(buf2), "%ld", (long)t);
#elif defined WIN32
#ifdef CPP_OLD
    sprintf(ptr, "%ld", (long)t);
#else
    sprintf_s(ptr, 32,"%ld", (long)t);
#endif
#endif
    if (strcmp(nonce, buf2) != 0)
        return -1;
    return (int)time(NULL) - (int)t;
}

long check_nonce(char *nonce)
{
    return internal_check_nonce(nonce);
}

#endif /// #ifndef WINCE