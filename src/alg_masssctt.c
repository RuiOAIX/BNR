#include "alg_masssctt.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

static void masssctt_core(int tid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, double *lsource, double *rsource, double srate, double drate, double *rdt, int **lscore) {

	int i, j, lid, rid, degree;
	double source, totalsource;

	//one 

	//two
	memset(lsource, 0, (lmaxId + 1) * sizeof(double));
	for (j = 0; j < ldegree[tid]; ++j) {
		rid = lrela[tid][j];
		degree = rdegree[rid];
		source = 1.0 / degree;
		for (i = 0; i < degree; ++i) {
			lid = rrela[rid][i];
			lsource[lid] += source;
		}
	}

	//three
	memset(rsource, 0, (rmaxId + 1) * sizeof(double));
	for (i = 0; i < lmaxId + 1; ++i) {
		if (lsource[i] > 1E-17) {
			totalsource = 0;
			degree = ldegree[i];
			source = lsource[i];
			for (j = 0; j < degree; ++j) {
				rid = lrela[i][j];
				rdt[rid] = pow(lscore[i][j], srate) / pow(rdegree[rid], drate);
				totalsource += rdt[rid];
			}
			for (j = 0; j < degree; ++j) {
				rid = lrela[i][j];
				rsource[rid] += source * rdt[rid] / totalsource;
			}
		}
	}

	for (j = 0; j < ldegree[tid]; ++j) {
		rsource[lrela[tid][j]] = -1;
	}
}


struct METRICS *masssctt(struct TASK *task) {
	LOG(LOG_INFO, "masssctt enter");
	//1 level, from task
	BIP *trainl = task->train->core[0];
	BIP *trainr = task->train->core[1];
	BIP *trainscorel = task->train->core[2];
	BIP *testl = task->test->core[0];
	int L = task->num_toprightused2cmptmetrics;
	double srate = task->rate_massscttscoreparam;
	double drate = task->rate_massscttdegreeparam;
	double *avescore = task->dt;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;
	int **lscore = trainscorel->rela;

	//3 level, from 2 level
	double *lsource = smalloc((lmaxId + 1)*sizeof(double));
	double *rsource = smalloc((rmaxId + 1)*sizeof(double));
	double *rdt = smalloc((rmaxId + 1)*sizeof(double));
	int *lidtr = smalloc((lmaxId + 1)*sizeof(int));
	int *ridtr = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL, SL;
	R=RL=PL=HL=IL=NL=SL=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rvlts
			masssctt_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lsource, rsource, srate, drate, rdt, lscore);
			//use rvlts, get ridts & rank & topL
			settopLrank(L, rmaxId, rdegree, rsource, ridtr, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, trainl, trainr, testl, &R, &RL, &PL);
		}
	}
	free(lsource); free(rsource);
	free(lidtr); free(ridtr);
	free(rank); free(rdt);

	set_HL_METRICS(L, topL, trainl, trainr, &HL);
	set_IL_METRICS(L, topL, trainl, trainr, task->trainr_cosine_similarity, &IL);
	set_NL_METRICS(L, topL, trainl, trainr, &NL);
	set_SL_METRICS(L, topL, trainl, avescore, &SL);
	free(topL);

	R /= testl->edgesNum;
	RL /= testl->idNum;
	PL /= testl->idNum;

	retn->R = R;
	retn->IL = IL;
	retn->NL = NL;
	retn->HL = HL;
	retn->PL = PL;
	retn->RL = RL;
	retn->SL = SL;
	return retn;
}

struct TASK *massscttT(struct OPTION *op) {
	struct TASK *otl = smalloc(sizeof(struct TASK));
	//default
	otl->train = NULL;
	otl->test = NULL;
	otl->trainr_cosine_similarity = NULL;
	otl->dt = NULL;
	
	//algorithm specialize
	otl->alg = masssctt;
	otl->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
	otl->rate_massscttscoreparam = op->rate_massscttscoreparam;
	otl->rate_massscttdegreeparam = op->rate_massscttdegreeparam;

	return otl;
}
