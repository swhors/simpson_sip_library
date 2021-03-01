#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "sky_debug.h"
#include "sky_string.h"
#include "sky_lock.h"

#define LOG_FNAME_PREFIX	"skyproxyd-"
#define LOG_FNAME_EXT		"log"

static SKY_LOCK fwrite_mutex=SKY_MUTEX_INI;

static void file_write_lock_init_internal()
{
#ifdef WIN32
	fwrite_mutex = 0;
#endif
}

void file_write_lock_init()
{
    file_write_lock_init_internal();
}


void file_write_lock_release()
{
    ;
}

static void lock_fwrite()
{
    sky_mutex_lock(&fwrite_mutex);
}

static void unlock_fwrite()
{
    sky_mutex_unlock(&fwrite_mutex);
}

static void write_log_internal(char * sbuff, int dbg_level, int req_level, int btime)
{
    char fname[128];
    char date[56];
    time_t ltime0;
    struct tm * ltime;
    FILE * fp;
    if( dbg_level >= req_level)
    {
        if( (ltime0 = time(NULL)) < 0 )
            return;
        ltime = localtime(&ltime0);
        if( btime )
            sprintf(date,"%02d/%02d/%02d-%02d-%02d : ",ltime->tm_mon+1,
                                ltime->tm_mday, ltime->tm_hour,
                                ltime->tm_min, ltime->tm_sec);
        sprintf(fname, "%s%04d%02d%02d.%s", 
            LOG_FNAME_PREFIX, ltime->tm_year+1900, ltime->tm_mon+1, 
            ltime->tm_mday, LOG_FNAME_EXT);
	    lock_fwrite();
        fp = fopen( fname, "a+t");
        if( fp )
        {
            if( btime )
                fwrite(date,1,strlen(date), fp);
            fwrite(sbuff,1,strlen(sbuff), fp);
            fwrite("\n",1,1, fp);
            fclose(fp);
        }
        unlock_fwrite();
    }
}

void write_log_withname(char * name, char * buff, int dbg_level, int req_level)
{
    char ibuff[3124];
    sprintf(ibuff, "[%s]\n%s", name, buff);
    write_log_internal(ibuff, dbg_level, req_level, 1); 
}
void write_log(char * sbuff, int dbg_level, int req_level)
{
    write_log_internal(sbuff, dbg_level, req_level, 0);
}

void write_log_withtime(char * sbuff, int dbg_level, int req_level)
{
    write_log_internal(sbuff, dbg_level, req_level, 1);
}

void sip_write_log(char * in_ip, char * out_ip, int state, int dbg_level, int req_level)
{
    char data[128];
    sprintf(data, "%s->%s : 0x%p\n", in_ip, out_ip, state);
    write_log_withtime(&data[0], dbg_level, req_level);
}
