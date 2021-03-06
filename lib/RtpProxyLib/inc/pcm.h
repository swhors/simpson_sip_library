#pragma once

unsigned char linear2alaw(short            pcm_val);
/*
* alaw2linear() - Convert an A-law value to 16-bit linear PCM
*/
short alaw2linear(unsigned char a_val);
/*
* linear2ulaw() - Convert a linear PCM value to u-law
*
* In order to simplify the encoding process, the original linear magnitude
* is biased by adding 33 which shifts the encoding range from (0 - 8158) to
* (33 - 8191). The result can be seen in the following encoding table:
*
*            Biased Linear Input Code            Compressed Code
*            ------------------------            ---------------
*            00000001wxyza                                    000wxyz
*            0000001wxyzab                                    001wxyz
*            000001wxyzabc                                    010wxyz
*            00001wxyzabcd                                    011wxyz
*            0001wxyzabcde                                    100wxyz
*            001wxyzabcdef                                    101wxyz
*            01wxyzabcdefg                                    110wxyz
*            1wxyzabcdefgh                                    111wxyz
*
* Each biased linear code has a leading 1 which identifies the segment
* number. The value of the segment number is equal to 7 minus the number
* of leading 0's. The quantization interval is directly available as the
* four bits wxyz.  * The trailing bits (a - h) are ignored.
*
* Ordinarily the complement of the resulting code word is used for
* transmission, and so the code word is complemented before it is returned.
*/
unsigned char linear2ulaw(short pcm_val);
/*
* ulaw2linear() - Convert a u-law value to 16-bit linear PCM
*
* First, a biased linear code is derived from the code word. An unbiased
* output can then be obtained by subtracting 33 from the biased code.
*
* Note that this function expects to be passed the complement of the
* original code word. This is in keeping with ISDN conventions.
*/
short ulaw2linear(unsigned char u_val);
/* A-law to u-law conversion */
unsigned char alaw2ulaw(unsigned char aval);
/* u-law to A-law conversion */
unsigned char ulaw2alaw(unsigned char uval);
/* 16 bit swapping */
short swap_linear (short pcm_val);