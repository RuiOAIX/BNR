#ifndef TASK_H
#define TASK_H 
#include "metrics.h"
#include "bip.h"
#include "net.h"
#include "option.h"

struct TASK {
	//common
	struct METRICS * (*alg)(struct TASK *);
	BIPS* train;
	BIPS* test;
	NETS* trainr_cosine_similarity;
	double *dt;
	int num_toprightused2cmptmetrics;

	//special
	double rate_hybridparam;
	double rate_hnbiparam;
	double rate_massscparam;
	double rate_masssctparam;
	double rate_massdparam;
	double rate_massscttscoreparam;
	double rate_massscttdegreeparam;

	//result;
	struct METRICS *mtc;
};

#endif
