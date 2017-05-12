#ifndef TASKLIST_H
#define TASKLIST_H 

#include "task.h"
#include "option.h"

typedef struct TASK OTL;
typedef struct TASK *(*ALGS)(struct OPTION *);

struct TASKLIST {
	int listNum;
	int currNum;
	OTL **core;

	//share
	BIPS *train;
	BIPS *test;
	NETS *trainr_cosine_similarity;
	double *dt;

	//private
	ALGS *algs;
	int algsNum;

};

void freeOTL(OTL *otl);
void freeTL(struct TASKLIST *tl);
struct TASKLIST *walkingTL(struct OPTION *op);

#endif
