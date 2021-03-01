#pragma once
#ifndef __DEBUG_H
#define __DEBUG_H

#include "stdio.h"

#define DBG_INTVAL(d) printf("[%s %s %d]%d\n",__FILE__,__FUNCTION__,__LINE__,d);
#define DBG_STRVAL(d) printf("[%s %s %d]%s\n",__FILE__,__FUNCTION__,__LINE__,d);
#define DBG_CHRVAL(d) printf("[%s %s %d]%c\n",__FILE__,__FUNCTION__,__LINE__,d);

void DBG_INTVAL1(char *file, int line, int d);
void DBG_INTVAL2(char *file, int line, int d);
void DBG_INTVAL3(char *file, int line, int d);
void DBG_INTVAL4(char *file, int line, int d);

void DBG_STRVAL1(char *file, int line, char *d);
void DBG_STRVAL2(char *file, int line, char *d);
void DBG_STRVAL3(char *file, int line, char *d);
void DBG_STRVAL4(char *file, int line, char *d);
void DBG_STRINT1(int d);
void DBG_STRINT2(int d);
void DBG_STRINT3(int d);
void DBG_STRINT4(int d);
void DBG_STRCHR1(char d);
void DBG_STRCHR2(char d);
void DBG_STRCHR3(char d);
void DBG_STRCHR4(char d);

#endif // __DEBUG_H
