#include "log.h"
#include "tasklist.h"

int main(int argc, char **argv) {
	setloglevel(LOG_INFO);

	struct OPTION *op = setOPTION(argc, argv);
	struct TASKLIST *tl = walkingTL(op);
	freeOPTION(op);

	int i, j;
	for (i = 0; i < tl->listNum; ++i) {
		struct METRICS *result = tl->core[i]->mtc;
		LOG(LOG_OP, "METRICS: R: %f, RL: %f, PL: %f, HL: %f, IL: %f, NL: %f, SL: %f.", result->R, result->RL, result->PL, result->HL, result->IL, result->NL, result->SL);
		for (j = 0; j < result->degreeMaxplus1; ++j) {
			if (result->RK[j] > 1E-17 || result->SLK[j] > 1E-17) {
				LOG(LOG_OP, "K: %d, RK: %f, RKc: %d, SLK: %f, SLKc: %d", j, result->RK[j], result->RKc[j], result->SLK[j], result->SLKc[j]);
			}
		}
	}
	freeTL(tl);
	releaselog();
	return 0;
}
