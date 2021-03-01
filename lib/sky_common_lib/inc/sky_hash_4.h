#pragma once

#define HASH_4_BITS 8
#define HASH_4_SIZE ( 1 << HASH_4_BITS )
#define HASH_4_E_SIZE ( 1024 << HASH_4_BITS )

#ifndef ub1
typedef unsigned char  ub1;
#endif // define ub1

#ifndef ub4
typedef unsigned int   ub4;
#endif // define ub4

int hash4(ub1 *val);
int hash4_e(ub1 *val);

