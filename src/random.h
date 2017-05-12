#ifndef RANDOM_H
#define RANDOM_H

/**
 * this file is used to generate Pseudo-random number, origin from Mersenne Twister.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
 * user can use function random[\s\d]* without calling function set[\s]* first,
 * 	because I already set a default seed.
 * but if you want to get a different pseudo-random number list,
 * 	you can set a different seed explicitly, or use current unix timestamp as seed.
 * 
 * Author: RuiXiao <xrfind@gmail.com>
 */

/// set a explicit seed.
extern void setRGseed(unsigned long seed);
/// set the seed with unix timestamp.
extern void setRGseedwithTime(void);

/// get a 32 bits prn.
extern unsigned long randomi32(void);

/// get a 31 bits prn.
extern long random(void);

/// get a double prn which is in [0, 1].
extern double randomd01(void);

/// get a double prn which is in [0, 1).
extern double randomd0(void);

/// get a double prn which is in (0, 1).
extern double randomd(void);

/// get a 53-precision double prn which is in [0, 1).
extern double randomd530(void);

#endif
