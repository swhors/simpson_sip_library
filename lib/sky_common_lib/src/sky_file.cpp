#include <stdio.h>
#include <time.h>

#include "sky_file.h"

#ifdef WIN32
#include <string.h>
#endif

int file_name_by_date(char * file, char *ext, char *dest)
{
    time_t ltime;
    struct tm *ltm;
    int len = 0;
    time(&ltime);
    ltm = localtime(&ltime);
    sprintf(dest, "%s", file);
    len = (int)strlen(dest);
    strftime(&dest[len], 128-len, "-%m-%k-%M-%S-%Y.", ltm);
    strcat(dest, ext);
    len = (int)strlen(dest);
    return len;
}

// if opt == 1, append. if opt == 0, new write.
int write_file_txt(char * filename, char * wbuf, int opt)
{
    FILE *fp;
    char w_opt[12];
    int len;
    if( !opt )
       sprintf(w_opt, "w+t");
    else
       sprintf(w_opt, "a+t");
    fp = fopen( filename, w_opt);
    len = (int)strlen( wbuf);
    if( ( len > 0 ) || !fp)
    {
        return 0;
    }
    if(fwrite(wbuf, len, 1, fp) == len )
    {
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}

int write_file_by_date(char * file, char *ext, char *wbuf, int wlen, int text)
{
    FILE *fp;
    char w_opt[24], dest[128];
    int i;
    
    if( text )
        sprintf(w_opt, "w+t");
    else
        sprintf(w_opt, "w+b");
    
    file_name_by_date(file, ext, &dest[0]);
    fp = fopen(dest, w_opt);
    if( fp )
    {
        if( text)
            fwrite(wbuf, wlen, 1, fp);
        else
        {
            for( i=0; i< wlen; i++)
            {
                fwrite(wbuf++, 1, 1,fp);
            }
        }
        fclose(fp);
        return 1;
    }
    return 0;
}

void back_file(char * file, char *ext, int text)
{
    FILE *fp1, *fp2;
    char dest[128], rbuf[256];
    char or_file[64];
    
    if( text )
        sprintf(rbuf, "r+t");
    else
        sprintf(rbuf, "r+b");
    
    sprintf(or_file, "%s.%s", file, ext);
    file_name_by_date(file, ext, &dest[0]);
    
    fp1 = fopen(or_file, rbuf);
    
    if( text )
        sprintf(rbuf, "a+t");
    else
        sprintf(rbuf, "a+b");
    
    fp2 = fopen(dest, rbuf);
    if( fp1 && fp2)
    {
        memset( rbuf,0, 256);
        while(fgets(rbuf, 256, fp1))
        {
            fputs(rbuf, fp2);
            memset(rbuf, 0, 256);
        }
    }
    
    if( fp1 )
        fclose(fp1);
    if( fp2 )
        fclose(fp2);
}
