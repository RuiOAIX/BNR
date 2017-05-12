#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

struct OPTION {
	bool alg_mass;
	bool alg_heats;
	bool alg_hybrid;
	double rate_hybridparam;
	bool alg_HNBI;
	double rate_hnbiparam;
	bool alg_massd;
	double rate_massdparam;
	bool alg_masssc;
	double rate_massscparam;
	bool alg_masssct;
	double rate_masssctparam;
	bool alg_masssctt;
	double rate_massscttscoreparam;
	double rate_massscttdegreeparam;

	char *filename_full;
	char *filename_train;
	char *filename_test;
	char *filename_leftobjectattr;

	double rate_dividefulldataset;
	int num_looptimes;
	int num_toprightused2cmptmetrics;
	unsigned long num_randomseed;

	//
	char *logfilename;
};

struct OPTION *setOPTION(int argc, char **argv);
int algnumOPTION(struct OPTION *op);
void freeOPTION(struct OPTION *op);

#endif
