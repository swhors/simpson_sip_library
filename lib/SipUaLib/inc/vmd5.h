#ifndef VMD5_H
#define VMD5_H

static const char* const vmd5HeaderVersion =
    "$Id: vmd5.h,v 1.1 2005/10/19 05:44:44 swhors Exp $";

/*
 * This is the header file for the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h'
 * header definitions; now uses stuff from dpkg's config.h
 *  - Ian Jackson <ijackson@nyx.cs.du.edu>.
 * Still in the public domain.
 */


//#include "vtypes.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN + 1]; 

typedef unsigned int u_int32_t;

//#define md5byte unsigned char
typedef unsigned char md5byte;

#define IN
#define OUT


struct MD5Context
{
    u_int32_t buf[4];
    u_int32_t bytes[2];
    u_int32_t in[16];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, md5byte const *buf, unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(u_int32_t buf[4], u_int32_t const in[16]);

void CvtHex( HASH Bin, HASHHEX Hex );

#ifdef __cplusplus
}
#endif

/* !MD5_H */
#endif
