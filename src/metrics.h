#ifndef METRICS_H
#define METRICS_H 
#include "bip.h"
#include "net.h"

struct METRICS {
	double R;
	double RL;
	double PL;
	double HL;
	double IL;
	double NL;
	double SL;

	//
	int degreeMaxplus1;
	double *RK;
	int *RKc;
	double *SLK;
	int *SLKc;
};

struct METRICS *createMTC(void);
void freeMTC(struct METRICS *m);

void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *trainl, BIP *trainr, BIP *testl, double *R, double *RL, double *PL);
void set_HL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *HL);
void set_IL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, NETS *cosin_similarity, double *IL);
void set_NL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *NL);
void set_SL_METRICS(int L, int *alltrianl_topL, BIP *trainl, double *score, double *SL);

void set_RK_METRICS(int lid, int *rank, BIP *trainl, BIP *trainr, BIP *testl, int *RKc, double *RK);
void set_SLK_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *score, int *SLKc, double *SLK);

#endif
