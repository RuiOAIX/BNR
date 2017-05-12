#include "net.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <limits.h>
#include <string.h>


void freeNET(NET *net) {
	freeCORE(net);
}

static inline void set_maxid_minid_NET(int *i1, int *i2, long linesNum, int *maxId_retn, int *minId_retn) {
	long i;
	int maxId = i1[0];
	int minId = i1[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > i1[i] ? maxId : i1[i];	
		maxId = maxId > i2[i] ? maxId : i2[i];	
		minId = minId < i1[i] ? minId : i1[i];
		minId = minId < i2[i] ? minId : i2[i];
	}
	*maxId_retn = maxId;
	*minId_retn = minId;
}

static inline void set_degree_degreeMax_degreeMin_idNum_NET(int *i1, int *i2, long linesNum, int maxId, int *degree, int *degreeMax, int *degreeMin, int *idNum_retn) {
	long i;
	for(i=0; i<linesNum; ++i) {
		++degree[i1[i]];
		++degree[i2[i]];
	}
	int j;
	int countMax = 0;
	int countMin = INT_MAX;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		countMax = countMax > degree[j] ? countMax : degree[j];
		countMin = countMin < degree[j] ? countMin : degree[j];
		if (degree[j]>0) {
			++idNum;
		}
	}
	*degreeMax = countMax;
	*degreeMin = countMin;
	*idNum_retn = idNum;
}

static inline void set_rela_aler_NET(int *i1, int *i2, double *dd, int *degree, int maxId, long linesNum, int ***rela_retn, double ***aler_retn) {
	int i;
	int **rela = smalloc((maxId+1)*sizeof(int *));
	double **aler = NULL;
	if (dd) aler = smalloc((maxId+1)*sizeof(double *));

	for(i=0; i<maxId+1; ++i) {
		if (degree[i] != 0) {
			rela[i] = smalloc(degree[i]*sizeof(int));
			if (aler) aler[i] = malloc(degree[i]*sizeof(double));
		}
		else {
			rela[i] = NULL;
			if (aler) aler[i] = NULL;
		}
	}

	int *temp_count=scalloc(maxId+1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		rela[ii1][temp_count[ii1]]=i2[i];
		if (aler) aler[ii1][temp_count[ii1]]=dd[i];
		++temp_count[ii1];
		rela[ii2][temp_count[ii2]]=i1[i];
		if (aler) aler[ii2][temp_count[ii2]]=dd[i];
		++temp_count[ii2];
	}
	free(temp_count);

	*rela_retn = rela;
	*aler_retn = aler;
}

static inline NET *assignNET(int maxId, int minId, long edgesNum, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, double **aler) {
	NET *net = smalloc(sizeof(NET));
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=edgesNum;
	net->idNum=idNum;
	net->degreeMax = degreeMax;
	net->degreeMin = degreeMin;
	net->degree = degree;
	net->rela = rela;
	net->aler = aler;
	return net;
}

NET *createNET(const struct LineFile * const lf) {
	if (!lf) return NULL;
	if (!(lf->linesNum)) return NULL;
	if (!(lf->i1 || lf->i2)) return NULL;

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	double *d1 = lf->d1;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int *degree = scalloc(maxId+1, sizeof(int));
	int degreeMax, degreeMin, idNum;
	set_degree_degreeMax_degreeMin_idNum_NET(i1, i2, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);

	int **rela;
   	double **aler;
	set_rela_aler_NET(i1, i2, d1, degree, maxId, linesNum, &rela, &aler);

	NET *net = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);

	LOG(LOG_INFO, "NET created.");
	LOG(LOG_INFO, "  Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld", maxId, minId, idNum, degreeMax, degreeMin, linesNum);
	return net;
}

void freeNETS(NETS *nets) {
	freeCORES(nets);
}

static inline void set_rela_aler_more_NET(int *i1, int *i2, int *ii, double *dd, int *degree, int maxId, long linesNum, int ***rela_retn, double ***aler_retn) {
	if (!ii && !dd) LOG(LOG_FATAL, "ii and dd can not both be NULL.");
	int i;
	int **rela = NULL;
	if (ii) rela = smalloc((maxId+1)*sizeof(int *));
	double **aler = NULL;
	if (dd) aler = smalloc((maxId+1)*sizeof(double *));

	for(i=0; i<maxId+1; ++i) {
		if (degree[i] != 0) {
			if (rela) rela[i] = smalloc(degree[i]*sizeof(int));
			if (aler) aler[i] = smalloc(degree[i]*sizeof(double));
		}
		else {
			if (rela) rela[i] = NULL;
			if (aler) aler[i] = NULL;
		}
	}

	int *temp_count=scalloc(maxId+1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		if (rela) rela[ii1][temp_count[ii1]]=ii[i];
		if (aler) aler[ii1][temp_count[ii1]]=dd[i];
		++temp_count[ii1];
		if (rela) rela[ii2][temp_count[ii2]]=ii[i];
		if (aler) aler[ii2][temp_count[ii2]]=dd[i];
		++temp_count[ii2];
	}
	free(temp_count);

	*rela_retn = rela;
	*aler_retn = aler;
}

NETS *createNETS(const struct LineFile * const lf) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	NETS *nets = smalloc(sizeof(NETS));
	nets->num = columnsNumLF(lf) - 1;
	nets->sign = smalloc(nets->num * sizeof(enum TYPE));
	nets->core = smalloc(nets->num * sizeof(NET *));

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int *degree = scalloc(maxId+1, sizeof(int));
	int degreeMax, degreeMin, idNum;
	set_degree_degreeMax_degreeMin_idNum_NET(i1, i2, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);

	int **rela;
	double **aler;

	set_rela_aler_NET(i1, i2, NULL, degree, maxId, linesNum, &rela, &aler);
	nets->core[0] = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
	//printNET(nets->core[0], "/tmp/xxx1");
	nets->sign[0] = INT;

	int i, j = 1;
	for (i = 2; i < lf->iNum; ++i) {
		int *ii = *(lf->ilist[i]);
		if (ii) {
			set_rela_aler_more_NET(i1, i2, ii, NULL, degree, maxId, linesNum, &rela, &aler);
			int *local_degree = smalloc((maxId+1)*sizeof(int));
			memcpy(local_degree, degree, (maxId + 1) * sizeof(int));
			nets->core[j] = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, local_degree, rela, aler);
			nets->sign[j] = INT;
			++j;
		}
	}
	for (i = 0; i < lf->dNum; ++i) {
		double *dd = *(lf->dlist[i]);
		if (dd) {
			set_rela_aler_more_NET(i1, i2, NULL, dd, degree, maxId, linesNum, &rela, &aler);
			int *local_degree = smalloc((maxId+1)*sizeof(int));
			memcpy(local_degree, degree, (maxId + 1) * sizeof(int));
			nets->core[j] = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, local_degree, rela, aler);
			//printNET(nets->core[j], "/tmp/xxx2");
			nets->sign[j] = INT;
			++j;
		}
	}

	LOG(LOG_INFO, "NETS created, contains [%3d] NETs.", nets->num);
	LOG(LOG_INFO, "  Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld", maxId, minId, idNum, degreeMax, degreeMin, linesNum);
	return nets;
}

void printNET(NET *net, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i;
	int j;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			fprintf(fp, "%d\t", i);
			if (net->rela) fprintf(fp, "\t%d", net->rela[i][j]);
			if (net->aler) fprintf(fp, "\t%f", net->aler[i][j]);
			
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	LOG(LOG_INFO, "NET printed into \"%s\".", filename);
}
