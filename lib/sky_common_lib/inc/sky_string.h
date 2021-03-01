#pragma once

char * strcatfm(char * buf, const char * fmt, ...);
char * findString(char *tbuf, char *key, int keylen);

char * skystrlwr(char * inbuf);
char * skystrupr(char *inbuf);

#ifdef LINUX
#define strlwr(a) skystrlwr(a);
#define strupr(a) skystrupr(a);
#endif

