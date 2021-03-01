#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "cProfile.h"

static PF_ITEM_VAL_T pfItemVal[MAX_ALLOWED_NUM];

static char * skip_space(IN OUT char *buff)
{
    char * tbuff = buff;
    while(*tbuff == ' ')
        tbuff ++;
    return tbuff;
}

static void remove_cnrl(IN OUT char *buff)
{
    char *tbuff = buff;
    while(*tbuff)
    {
        if( *tbuff=='\n'||*tbuff=='\r')
            *tbuff = 0; 
        *tbuff++;
    }
}

static int ParseIntVal(IN char *buff,IN char * item,IN int item_len,OUT int *iVal)
{
    buff = skip_space(buff);
    *iVal= atoi(buff);
}

static int ParseStrVal(IN char *buff,OUT void *pbuff,IN OUT int *len,IN char * item,IN int item_len)
{
    char *sval,*tbuf;
    buff = skip_space(buff);
	tbuf = buff;
    remove_cnrl(tbuf);
    sval = strstr(buff, "=");
    if( sval && pbuff )
    {
        if( item_len > 0 && item)
        {
            memset( item, 0, item_len);
            memcpy( item, buff, strlen(buff)-strlen(sval));
        }
        memset(pbuff, 0, *len);
        sprintf(pbuff, "%s", sval+1);
        *len = (int)strlen(pbuff);
        return 1;
    }
    return 0;
}

FILE * GetFLocation(IN char *item,IN char *filename,IN char *open_type,IN int *opt)
{
    FILE *fp;
    char rbuf[512];
    int len,tlen=0;
    fp = fopen(filename, open_type);
    if( fp )
    {
        len = (int)strlen(item);
        if( len > 0 )
        {
            memset(rbuf, 0, 512);
            while(fgets(rbuf, 512, fp))
            {
                if(!memcmp( rbuf, item,len))
                {
                    //rewind(fp);
					fseek(fp, 0, SEEK_SET);
                    fseek(fp, tlen, SEEK_SET);
                    *opt = (int)strlen(rbuf);
                    return fp;
                } 
                tlen += (int)strlen(rbuf);
				if( strstr( rbuf, "\r") )
					tlen++;
			    if( strstr( rbuf, "\n") )
					tlen++;
                memset(rbuf, 0, 512);
            }
        }
        if(!*opt)
            fclose(fp);
        else
        {
            *opt = 0;
            return fp;
        }
    }
    *opt = 0;
    return NULL;
}

static int SetPrivateProfileVal(IN char *index,IN char *item,IN char *val,IN int vlen,IN char * filename)
{
    FILE *fp;
    char wbuf[256];
    int i,ifind=-1;
    if(!item || !val || vlen<= 0)
        return 0;
    for(i = 0; i < MAX_ALLOWED_NUM; i++)
    {
        memset(pfItemVal[i].item, 0, 32);
        memset(pfItemVal[i].val, 0, 128);
    }
    i = 0;
    fp = fopen(filename, "r+");
    if( fp)
    {
        int rlen,slen;
        memset(wbuf, 0, 256);
        slen = (int)strlen(item);
        //ilen = strlen(index);
        while(fgets(wbuf, 256, fp))
        {
            if(!memcmp(item, wbuf, slen))
                ifind = i;
            rlen = 128;
            ParseStrVal(wbuf,pfItemVal[i].val, &rlen,pfItemVal[i].item, 32);
            memset(wbuf, 0, 256);
            i++;
        }
        fclose(fp);
    }
    if( ifind < 0)
	{
		ifind = i;
	    memset(pfItemVal[i].item, 0, 32);
	    memcpy(pfItemVal[i].item, item, strlen(item));
		i++;
	}
    memset(pfItemVal[ifind].val, 0, 128);
    memcpy(pfItemVal[ifind].val, val, strlen(val));

    fp = fopen(filename, "w");
    if( fp )
    {
        ifind = i;
        for(i = 0; i< ifind ; i++)
            fprintf(fp, "%s=%s\n", pfItemVal[i].item, pfItemVal[i].val);
        fclose(fp);
        return 1;
    }
    return 0;
}

static int GetPrivateProfileVal(IN char * item,IN char * defVal,OUT char * outbuf,IN int buflen,IN char * filename)
{
    FILE *fp;
    char rbuf[512];
    int opt= 0;
    if( !outbuf || buflen <= 0)
        return 0;
    fp = GetFLocation(item, filename, "r", &opt);
    if(fp)
    {
        memset(rbuf, 0, 512);
        if( fgets(rbuf, 512, fp) )
        {
            int rlen = buflen;
            memset(outbuf, 0, buflen);
            if(ParseStrVal(rbuf, outbuf, &rlen,NULL, 0)> 0)
            {
                fclose(fp);
                return  rlen;
            }
        }
        fclose(fp);
    } 
    sprintf(outbuf, "%s", defVal);
    return (int)strlen(defVal);
}

SKYLIB int LGetPrivateProfileString(IN char * item,IN char * defVal,OUT char * outbuf,IN int buflen,IN char * filename)
{
    return GetPrivateProfileVal(item,defVal,outbuf,buflen,filename);
}

SKYLIB int LSetPrivateProfileString(IN char *index,IN char *item,IN char *val,IN int len,IN char * filename)
{
    return SetPrivateProfileVal(index,item, val, len, filename);
}

SKYLIB int LGetPrivateProfileInt(IN char * item,IN int defVal,IN char * filename)
{
	char outval[128];
	char sdefval[128];
	sprintf(sdefval, "%d", defVal);
	memset(outval, 0, 128);
	if( GetPrivateProfileVal(item, sdefval, outval, 128, filename) > 0 )
		return atoi(outval);
    return 0;
}

SKYLIB int LSetPrivateProfileInt(IN char *index,IN char *item,IN int iVal,IN char *filename)
{
	char inBuf[128];
	sprintf(inBuf,"%d", iVal);
	return SetPrivateProfileVal(index,item, inBuf, (int)strlen(inBuf), filename);
}