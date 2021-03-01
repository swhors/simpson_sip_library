#include "debug.h"

void DBG_STRVAL1(char *file, int line, char *d) {
#ifdef DBG_LEVEL1 
        if( d ) printf("[%s %d] %s\n", file, line, d);
#endif 
	}

void DBG_STRVAL2(char *file, int line, char *d) {
#ifdef DBG_LEVEL2
        if( d ) printf("[%s %d] %s\n", file, line, d);
#endif
        }

void DBG_STRVAL3(char *file, int line, char *d) {
#ifdef DBG_LEVEL3
        if( d ) printf("[%s %d] %s\n", file, line, d);
#endif
        }

void DBG_STRVAL4(char *file, int line, char *d) {
#ifdef DBG_LEVEL4
        if( d ) printf("[%s %d] %s\n", file, line, d);
#endif
        }

void DBG_INTVAL1(char *file, int line, int d) {
#ifdef DBG_LEVEL1 
        printf("[%s %d] %d\n", file, line, d);
#endif 
	}

void DBG_INTVAL2(char *file, int line, int d) {
#ifdef DBG_LEVEL2
        printf("[%s %d] %d\n", file, line, d);
#endif
        }

void DBG_INTVAL3(char *file, int line, int d) {
#ifdef DBG_LEVEL3
        printf("[%s %d] %d\n", file, line, d);
#endif
        }

void DBG_INTVAL4(char *file, int line, int d) {
#ifdef DBG_LEVEL4
        printf("[%s %d] %d\n", file, line, d);
#endif
        }

void DBG_CHRVAL1(char *file, int line, char d) {
#ifdef DBG_LEVEL1 
        printf("[%s %d] %c\n", file, line, d);
#endif 
	}

void DBG_CHRVAL2(char *file, int line, char d) {
#ifdef DBG_LEVEL2
        printf("[%s %d] %c\n", file, line, d);
#endif
        }

void DBG_CHRVAL3(char *file, int line, char d) {
#ifdef DBG_LEVEL3
        printf("[%s %d] %c\n", file, line, d);
#endif
        }

void DBG_CHRVAL4(char *file, int line, char d) {
#ifdef DBG_LEVEL4
        printf("[%s %d] %c\n", file, line, d);
#endif
}

void LogWrite(char *buf)
{
	FILE *fp = fopen("log.txt", "a+t");
	if( fp )
	{
		fprintf(fp, "%s\n",buf);
		//fwrite( buf, strlen(buf), 1, fp);
		fclose(fp);
	}
}
