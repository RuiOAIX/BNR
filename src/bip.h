/**
 * bipart recommandation is carried on between two kinds of objects, such as user and item. 
 * Here, I use "left" and "right" to represent these two kinds. Conventionly, left represent user and right represent item.
 * maybe a biparte can contain some extra attributes, like score that a user give to a item. but any attribute can be treated as a param.
 * this is more flexible.
 *
 * for struct BIP:
 * 		the origin bipartite is coming from a struct LineFile with using createBIP(struct LineFile) function.
 * 		(BTW, the origin LineFile is getting from a file which contain at least two int integer in an line)
 *
 * 		struct BIP contains only half information of the bipartite. 
 * 		(I mean one struct Bip can store a LineFile completely, but it doesn't store detail information.)
 * 		you need two struct Bip to store all detail information of a Bipartite.
 * 		one is indexed by i1(left).
 * 		one is indexed by i2(right).
 *
 */
#ifndef BIP_H
#define BIP_H

#include "utils.h"
#include "core.h"
#include "linefile.h"

typedef struct CORE BIP;

BIP *createBIP(const struct LineFile * const file, enum SIDE s);
void freeBIP(BIP *bip);

BIP * cloneBIP(BIP *bip);
void verifyBIP(BIP *left, BIP *right);
void printBIP(BIP *bip, char *filename);
double *averageBIP(BIP *bip);

//the dividation will guarantee that: 
//	for each available user(degree of this user is at least one), at least there will be an edge related this user in big part.
//	for each available item(degree of this item is at least one), at least there will be an edge related this item in big part.
//so maybe some users/items are not existed in the small part.
void divideBIP(BIP *left, BIP *right, double rate, struct LineFile **small_part, struct LineFile **big_part);


typedef struct CORES BIPS;
void freeBIPS(BIPS *bips);
BIPS *createBIPS(const struct LineFile * const file);
void divideBIPS(BIPS *bs, double rate, struct LineFile **small, struct LineFile **big);

#endif
