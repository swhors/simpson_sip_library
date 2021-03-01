#pragma once

#ifndef _MD5_H
#define _MD5_H
#include "hash_typedef.h"
#define MD5_MODULENAME "md5"
#define MD5_MODULEDESC "MD5 hash [not entirely secure]"
#define MD5_HASHLEN    128

/* MD5 constants */
#define MD5_IVSIZE 4
#define MD5_YSIZE 64
#define MD5_ZSIZE 64
#define MD5_SSIZE 64
#define MD5_PADMULTIPLE 512
void md5_hash(u_int8_t *d, u_int32_t len, u_int8_t *oout);
u_int32_t md5_hashlen(void);
void md5_hash_internal(u_int32_t *X, u_int32_t *H);
#endif // _MD5_H

