#include "alg_masssc.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void masssc_core(int tid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, double *lsource, double *rsource, double rate, double *ldt, double *rdt, int **lscore, int **rscore, int maxscore) {

	int i, j, lid, rid, degree;
	double source, total1source, total2source;

	//one 
	total1source = 0;
	degree = ldegree[tid];
	for (j = 0; j < degree; ++j) {
		rid = lrela[tid][j];
		rsource[rid] = pow(lscore[tid][j], rate);
		total1source += rsource[rid];	
		rsource[rid] *= degree;
		rdt[rid] = lscore[tid][j];
	}

	//two
	memset(lsource, 0, (lmaxId + 1) * sizeof(double));
	for (i = 0; i < ldegree[tid];  ++i) {
		rid = lrela[tid][i];
		degree = rdegree[rid];
		source = rsource[rid] / total1source;
		double tidscore = rdt[rid];
		total2source = 0;
		for (j = 0; j < degree; ++j) {
			lid = rrela[rid][j];
			double score_similarity = pow(maxscore - fabs(rscore[rid][j] - tidscore), rate); 
			total2source += score_similarity;
			ldt[lid] = score_similarity;
		}
		for (j = 0; j < degree; ++j) {
			lid = rrela[rid][j];
			lsource[lid] += source * ldt[lid] / total2source;
		}
	}

	//three
	memset(rsource, 0, (rmaxId + 1) * sizeof(double));
	for (i = 0; i < lmaxId + 1; ++i) {
		if (lsource[i] > 1E-17) {
			total2source = 0;
			degree = ldegree[i];
			source = lsource[i];
			for (j = 0; j < degree; ++j) {
				rid = lrela[i][j];
				rdt[rid] = pow((double)lscore[i][j] / rdegree[rid], rate);
				total2source += rdt[rid];
			}
			for (j = 0; j < degree; ++j) {
				rid = lrela[i][j];
				rsource[rid] += source * rdt[rid] / total2source;
			}
		}
	}

	//set selected item's source to -1
	for (j = 0; j < ldegree[tid]; ++j) {
		rsource[lrela[tid][j]] = -1;
	}
}

struct METRICS *masssc(struct TASK *task) {
	LOG(LOG_INFO, "masssc enter");
	//1 level, from task
	BIP *trainl = task->train->core[0];
	BIP *trainr = task->train->core[1];
	BIP *trainscorel = task->train->core[2];
	BIP *trainscorer = task->train->core[3];
	BIP *testl = task->test->core[0];
	int L = task->num_toprightused2cmptmetrics;
	double rate = task->rate_massscparam;
	double *avescore = task->dt;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;
	int **lscore = trainscorel->rela;
	int **rscore = trainscorer->rela;
	int maxscore = trainscorel->relaMax;

	//3 level, from 2 level
	double *lsource = smalloc((lmaxId + 1)*sizeof(double));
	double *rsource = smalloc((rmaxId + 1)*sizeof(double));
	double *ldt = smalloc((lmaxId + 1)*sizeof(double));
	double *rdt = smalloc((rmaxId + 1)*sizeof(double));
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
			masssc_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lsource, rsource, rate, ldt, rdt, lscore, rscore, maxscore);
			//use rvlts, get ridts & rank & topL
			settopLrank(L, rmaxId, rdegree, rsource, ridtr, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, trainl, trainr, testl, &R, &RL, &PL);
		}
	}
	free(lsource); free(rsource);
	free(ridtr);
	free(ldt); free(rdt); free(rank);

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

struct TASK *massscT(struct OPTION *op) {
	struct TASK *otl = smalloc(sizeof(struct TASK));
	otl->train = NULL;
	otl->test = NULL;
	otl->trainr_cosine_similarity = NULL;
	otl->dt = NULL;

	otl->alg = masssc;
	otl->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
	otl->rate_massscparam = op->rate_massscparam;

	return otl;
}
