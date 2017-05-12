#ifndef CORE_H
#define CORE_H 

struct CORE {
	//Basic
	long edgesNum;
	int maxId;
	int minId;
	int idNum;
	int degreeMax;
	int degreeMin;
	int *degree;
	int **rela;
	double **aler;

	//
	int relaMax;
	int relaMin;
	double alerMax;
	double alerMin;
};

struct CORES {
	int num;
	enum TYPE *sign;
	struct CORE **core;
};

void freeCORE(struct CORE *core);
void freeCORES(struct CORES *cores);
#endif
