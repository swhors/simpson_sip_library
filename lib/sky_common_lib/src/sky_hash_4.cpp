#include <stdio.h>
#include "sky_hash_4.h"

int hash4(ub1 * val)
{
    unsigned long x;
    x = val[0];
    x = (x << 2)^val[1];
    x = (x << 2)^val[2];
    x = (x << 2)^val[3];
    x ^= x >> 4;
    return x & (HASH_4_SIZE -1 );
}
int hash4_e(ub1 * val)
{
    int i;
    unsigned long x;
    x = val[0];
    for( i = 1; i < 16; i++)
    {
        x = (x << 2)^val[i];
    }
    x ^= x >> 16;
    return x & (HASH_4_E_SIZE -1 );
}
