#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include "skysipua_g.h"

#ifndef IN
#define IN
#endif // IN
#ifndef OUT
#define OUT
#endif // OUT

#define MAX_ALLOWED_NUM	 64

typedef struct _profile_item_val_t{
    char item[32];
    char val[128];
}PF_ITEM_VAL_T, *P_PF_ITEM_VAL_T;

SKYLIB int LGetPrivateProfileString(IN char *item,IN char *defVal,OUT char *outbuf,IN int buflen,IN char *filename);
SKYLIB int LSetPrivateProfileString(IN char *index,IN char *item,IN char *Val,IN int len,IN char *filename);
SKYLIB int LGetPrivateProfileInt(IN char *item,IN int defVal,IN char * filename);
SKYLIB int LSetPrivateProfileInt(IN char *index,IN char *item,IN int iVal,IN char *filename);
#ifdef __cplusplus
}
#endif
