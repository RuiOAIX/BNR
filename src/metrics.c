#include "metrics.h"
#include <stdlib.h>
#include <string.h>

struct METRICS *createMTC(void) {
	struct METRICS *lp = smalloc(sizeof(struct METRICS));
	lp->R = 0;
	lp->RL = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->SL = 0;
	
	//
	lp->degreeMaxplus1 = 0;
	lp->RK = NULL;
	lp->RKc = NULL;
	lp->SLK = NULL;
	lp->SLKc = NULL;

	return lp;
}

void freeMTC(struct METRICS *lp) {
	free(lp->RK);
	free(lp->RKc);
	free(lp->SLK);
	free(lp->SLKc);
	free(lp);
}

//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipii->idNum, not bipii->maxId. 
//	but I believe in linyuan's paper, she use the bipii->maxId. 
//	I think bipii->idNum make more sence.
void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *trainl, BIP *trainr, BIP *testl, double *R, double *RL, double *PL) {
	if (lid < testl->maxId + 1 &&  testl->degree[lid]) {
		int unselected_list_length = trainr->maxId - trainl->degree[lid];
		int sum_objintest_rank_in_all = 0;
		int num_objintest_in_topL = 0;
		int j, id;
		for (j=0; j<testl->degree[lid]; ++j) {
			id = testl->rela[lid][j];
			sum_objintest_rank_in_all += rank[id];
			if (rank[id] <= L) {
				++num_objintest_in_topL;
			}
		}

		*R += (double)sum_objintest_rank_in_all/(double)unselected_list_length;
		*RL += (double)num_objintest_in_topL/(double)(testl->degree[lid]);
		*PL += (double)num_objintest_in_topL/(double)L;
	}
}

void set_RK_METRICS(int lid, int *rank, BIP *trainl, BIP *trainr, BIP *testl, int *RKc, double *RK) {
	if (lid < testl->maxId + 1 &&  testl->degree[lid]) {
		int unselected_list_length = trainr->maxId - trainl->degree[lid];
		int j, rid, degree;
		for (j=0; j<testl->degree[lid]; ++j) {
			rid = testl->rela[lid][j];
			degree = trainr->degree[rid];
			double rankscore = rank[rid] / (double)unselected_list_length;
			RK[degree] += rankscore;
			RKc[degree] += 1;
		}
	}
}

void set_HL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *HL) {
	int *sign = scalloc((trainr->maxId + 1), sizeof(int));

	int i, j, k;
	int num_samerobjs_in2lobjpair = 0;
	*HL = 0;
	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			for (k = i*L; k < i*L+L; ++k) {
				sign[alltrianl_topL[k]] = 1;
			}
			for (j = i + 1; j < trainl->maxId + 1; ++j) {
				if (trainl->degree[j]) {
					int Cij = 0;
					for (k = j*L; k < j*L+L; ++k) {
						if (sign[alltrianl_topL[k]]) {
							++Cij;
						}
					}
					*HL += 1 - ((double)Cij)/(double)L;
					++num_samerobjs_in2lobjpair;
				}
			}
			for (k = i*L; k < i*L+L; ++k) {
				sign[alltrianl_topL[k]] = 0;
			}
		}
	}
	free(sign);
	*HL /= num_samerobjs_in2lobjpair;
}

void set_IL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, NETS *cosin_similarity, double *IL) {
	if (cosin_similarity == NULL) return;
	NET *sim_id = cosin_similarity->core[0];
	NET *sim_vl = cosin_similarity->core[1];
	double *sign = scalloc((trainr->maxId + 1), sizeof(double));
	int i, j, k;
	*IL = 0;
	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			int *topL = alltrianl_topL + i*L;
			for (j = 0; j < L; ++j) {
				int id = topL[j];
				for (k = 0; k < sim_id->degree[id]; ++k) {
					sign[sim_id->rela[id][k]] = sim_vl->aler[id][k];
				}
				for (k = j + 1; k < L; ++k) {
					id = topL[k];
					*IL += sign[id];
				}
				for (k = 0; k < sim_id->degree[id]; ++k) {
					sign[sim_id->rela[id][k]] = 0;
				}
			}
		}
	}
	free(sign);
	*IL = *IL * 2 / (L * (L-1) * trainl->idNum);
}

void set_NL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *NL) {
	int i,j;
	*NL = 0;
	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			int *topL = alltrianl_topL + i*L;
			for (j = 0; j < L; ++j) {
				*NL += trainr->degree[topL[j]];
			}
		}
	}
	*NL /= L * trainl->idNum;
}

void set_SL_METRICS(int L, int *alltrianl_topL, BIP *trainl, double *score, double *SL) {
	int i, j;
	*SL = 0;
	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			int *topL = alltrianl_topL + i*L;
			for (j = 0; j < L; ++j) {
				*SL += score[topL[j]];
			}
		}
	}
	*SL /= L * trainl->idNum;
}

void set_SLK_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *score, int *SLKc, double *SLK) {
	int i, j;
	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			int *topL = alltrianl_topL + i*L;
			for (j = 0; j < L; ++j) {
				int rid = topL[j];
				int degree = trainr->degree[rid];
				SLK[degree] += score[rid];
				SLKc[degree] += 1;
			}
		}
	}
}
