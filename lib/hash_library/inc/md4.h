#pragma once


#ifndef _MD4_H
#define _MD4_H
#include "hash_typedef.h"
#define MD4_MODULENAME "md4"
#define MD4_MODULEDESC "MD4 hash [insecure!]"
#define MD4_HASHLEN    128/* MD4 constants */
#define MD4_IVSIZE 4
#define MD4_NROUNDS 3
#define MD4_ZSIZE 48
#define MD4_SSIZE 48
#define MD4_PADMULTIPLE 512
	
void md4_hash(u_int8_t *d, u_int32_t len, u_int8_t *oout);
u_int32_t md4_hashlen(void);
void md4_hash_internal(u_int32_t *X, u_int32_t *H);

#endif //_MD4_H
