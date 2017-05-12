#include "bip.h"
#include "log.h"
#include "random.h"
#include <limits.h>

#include <string.h>

/*********************************************************************/
/***operation on struct Bip*******************************************/
/*********************************************************************/
static inline void set_maxId_minId_BIP(int *il, long edgesNum, int *maxId, int *minId) {
	//set minId&maxId.
	long i;
	*minId = *maxId = il[0];
	for(i=0; i<edgesNum; ++i) {
		*maxId = *maxId > il[i] ? *maxId : il[i];
		*minId = *minId < il[i] ? *minId : il[i];
	}
}

static inline void set_degree_degreeMax_degreeMin_idNum_BIP(int *il, long edgesNum, int maxId, int *degree, int *degreeMax_retn, int *degreeMin_retn, int *idNum_retn) {
	long i;
	for(i = 0; i < edgesNum; ++i) {
		++degree[il[i]];
	}
	int j, idNum = 0;
	int degreeMax = 0;
	int degreeMin = INT_MAX;
	for(j = 0; j < maxId + 1; ++j) {
		degreeMax = degreeMax>degree[j]?degreeMax:degree[j];
		degreeMin = degreeMin<degree[j]?degreeMin:degree[j];
		if (degree[j]) ++idNum;
	}
	*degreeMax_retn = degreeMax;
	*degreeMin_retn = degreeMin;
	*idNum_retn = idNum;
}

static inline void set_rela_aler_BIP(int *il, int *ii, double *dd, int *degree, int maxId, long edgesNum, int ***rela_retn, int *relaMax_retn, int *relaMin_retn, double ***aler_retn, double *alerMax_retn, double *alerMin_retn) {
	if (!ii && !dd) LOG(LOG_FATAL, "ii and dd can not be both NULL");
	if (ii == il) LOG(LOG_FATAL, "il == ii, make noscence");

	int **rela = NULL, relaMax = -1, relaMin = -1;
	double **aler = NULL, alerMax = -1.0, alerMin = -1.0;
	
	if (ii) rela = smalloc((maxId + 1) * sizeof(int *));
	if (dd) aler = smalloc((maxId + 1) * sizeof(double *));
	
	int j;
	if (ii) {
		for(j=0; j<maxId+1; ++j) {
			rela[j]=smalloc(degree[j]*sizeof(int));
		}
	}
	if (dd) {
		for (j = 0; j < maxId + 1; ++j) {
			aler[j] = smalloc(degree[j] * sizeof(double));
		}
	}

	long i;
	int *temp = scalloc(maxId+1, sizeof(int));
	if (ii) {
		relaMax = relaMin = ii[0];
		for(i=0; i<edgesNum; ++i) {
			int ii1 =il[i];
			rela[ii1][temp[ii1]]=ii[i];
			relaMax = relaMax > ii[i] ? relaMax : ii[i];
			relaMin = relaMin < ii[i] ? relaMin : ii[i];
			++temp[ii1];
		}
	}
	memset(temp, 0, (maxId+1)*sizeof(int));
	if (dd) {
		alerMax = alerMin = dd[0];
		for(i=0; i<edgesNum; ++i) {
			int ii1 =il[i];
			aler[ii1][temp[ii1]] = dd[i];
			alerMax = alerMax > dd[i] ? alerMax : dd[i];
			alerMin = alerMin < dd[i] ? alerMin : dd[i];
			++temp[ii1];
		}
	}
	free(temp);

	*rela_retn = rela;
	*aler_retn = aler;
	*relaMax_retn = relaMax;
	*relaMin_retn = relaMin;
	*alerMax_retn = alerMax;
	*alerMin_retn = alerMin;
}

static inline BIP *assignBIP(int maxId, int minId, long edgesNum, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, int relaMax, int relaMin, double **aler, double alerMax, double alerMin) {
	BIP *bip = smalloc(sizeof(BIP));
	bip->maxId=maxId;
	bip->minId=minId;
	bip->edgesNum=edgesNum;
	bip->idNum=idNum;
	bip->degreeMax = degreeMax;
	bip->degreeMin = degreeMin;
	bip->degree = degree;
	bip->rela = rela;
	bip->relaMax = relaMax;
	bip->relaMin = relaMin;
	bip->aler = aler;
	bip->alerMax = alerMax;
	bip->alerMin = alerMin;
	return bip;
}

static BIP* halfBIP(long linesNum, int *il, int *ii, double *dd, int maxId_d, int minId_d, int *degree_d, int degreeMax_d, int degreeMin_d, int idNum_d) {
	int maxId, minId;
	if (maxId_d != -1 && minId_d != -1) {
		maxId = maxId_d;
		minId = minId_d;
	}
	else {
		set_maxId_minId_BIP(il, linesNum, &maxId, &minId);
	}

	int *degree = scalloc(maxId+1, sizeof(int));
	int degreeMax, degreeMin, idNum;
	if (degree_d && degreeMax_d != -1 && degreeMin_d != -1 && idNum_d != -1) {
		memcpy(degree, degree_d, (maxId + 1) * sizeof(int));
		degreeMax = degreeMax_d;
		degreeMin = degreeMin_d;
		idNum = idNum_d;
	}
	else {
		set_degree_degreeMax_degreeMin_idNum_BIP(il, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);
	}

	int **rela, relaMax, relaMin;
	double **aler, alerMax, alerMin;
	set_rela_aler_BIP(il, ii, dd, degree, maxId, linesNum, &rela, &relaMax, &relaMin, &aler, &alerMax, &alerMin);

	return  assignBIP(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, relaMax, relaMin, aler, alerMax, alerMin);
}

BIP *createBIP(const struct LineFile * const lf, enum SIDE s) {
	if (lf == NULL || lf->i1 == NULL || lf->linesNum < 1) 
		LOG(LOG_FATAL, "create_Bip lf: %p, lf->i1: %p, lf->i2: %p, lf->linesNum: %ld", lf, lf->i1, lf->i2, lf->linesNum);

	int *i1 = lf->i1;
	int *i2 = lf->i2;
	long edgesNum = lf->linesNum;
	double *d1 = lf->d1;

	BIP *bip;
	if (LEFT == s)  
		bip = halfBIP(edgesNum, i1, i2, d1, -1, -1, NULL, -1, -1, -1);
	else 
		bip = halfBIP(edgesNum, i2, i1, d1, -1, -1, NULL, -1, -1, -1);
	

	LOG(LOG_INFO, "BIP %s create side.", leftorright(s));
	LOG(LOG_INFO, "  Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld",\
		   	bip->maxId, bip->minId, bip->idNum, bip->degreeMax, bip->degreeMin, bip->edgesNum);
	return bip;
}

void freeBIP(BIP *bip) {
	int i=0;
	if (bip->rela) {
		for(i=0; i<bip->maxId+1; ++i) {
			free(bip->rela[i]);
		}
	}
	if (bip->aler) {
		for(i=0; i<bip->maxId+1; ++i) {
			free(bip->aler[i]);
		}
	}
	free(bip->degree);
	free(bip->rela);
	free(bip->aler);
	free(bip);
	LOG(LOG_DBG, "BIP freed.");
}

BIP* cloneBIP(BIP *bip) {
	BIP *new = smalloc(sizeof(BIP));

	new->maxId = bip->maxId;
	new->minId = bip->minId;
	new->idNum = bip->idNum;
	new->edgesNum = bip->edgesNum;
	new->degreeMax = bip->degreeMax;
	new->degreeMin = bip->degreeMin;

	new->degree = smalloc((bip->maxId + 1) * sizeof(int));
	memcpy(new->degree, bip->degree, (bip->maxId + 1) * sizeof(int));

	new->rela = NULL;
	new->aler = NULL;

	int i;
	if (bip->rela) {
		new->rela = smalloc((new->maxId + 1)*sizeof(int *));
		for (i = 0; i < new->maxId + 1; ++i) {
			new->rela[i] = smalloc(new->degree[i]*sizeof(int));
			memcpy(new->rela[i], bip->rela[i], new->degree[i]*sizeof(int));
		}
	}
	if (bip->aler) {
		new->aler = smalloc((new->maxId + 1)*sizeof(double *));
		for (i = 0; i < bip->maxId + 1; ++i) {
			new->aler[i] = smalloc(new->degree[i]*sizeof(double));
			memcpy(new->aler[i], bip->aler[i], new->degree[i]*sizeof(double));
		}
	}

	LOG(LOG_DBG, "BIP cloned.");
	return new;
}

void verifyBIP(BIP *bl, BIP *br) {
	FILE *fpd = sfopen("/tmp/duplicatePairsinNet", "w");
	FILE *fpc = sfopen("/tmp/NoDuplicatePairsNetFile", "w");
	fprintf(fpd, "the following pairs are duplicate in the net file\n");
	bool sign=false;
	int j, k, i;

	int *place = smalloc((br->maxId+1)*sizeof(int));
	for (k=0; k<br->maxId + 1; ++k) {
		place[k] = -1;
	}
	for (j=0; j<bl->maxId+1; ++j) {
		for (i=0; i<bl->degree[j]; ++i) {
			int rightobj = bl->rela[j][i];
			int holder = place[rightobj];
			if (holder == -1) {
				place[rightobj]= 1;
				fprintf(fpc, "%d\t%d\n", j, rightobj);
				continue;
			}
			fprintf(fpd, "%d\t%d\n", j, rightobj);
			sign=true;
		}
		for (i = 0; i < bl->degree[j]; ++i) {
			place[bl->rela[j][i]] = -1;
		}
	}
	free(place);
	fclose(fpd);
	fclose(fpc);
	if (sign) {
		LOG(LOG_WARN, "the file has duplicate pairs, you can check data/duplicatePairsinNet.");
		LOG(LOG_WARN, "we generate a net file named /tmp/NoDuplicatePairsNetFile which doesn't contain any duplicate pairs.");
		LOG(LOG_WARN, "you should use this file instead the origin wrong one.");
	}
	else {
		LOG(LOG_INFO, "perfect network.");
	}
}

void printBIP(BIP *bip, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i;
	int j;
	for (i = 0; i < bip->maxId + 1; ++i) {
		for (j = 0; j < bip->degree[i]; ++j) {
			fprintf(fp, "%d\t", i);
			if (bip->rela) {
				fprintf(fp, "\t%d", bip->rela[i][j]);
			}
			if (bip->aler) {
				fprintf(fp, "\t%f", bip->aler[i][j]);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	LOG(LOG_INFO, "BIP printed in %s.", filename);
}

double *averageBIP(BIP *bip) {
	double *ave = smalloc((bip->maxId + 1) * sizeof(double));
	int i, j;
	for (i = 0; i < bip->maxId + 1; ++i) {
		ave[i] = 0;
		if (bip->degree[i]) {
			for (j = 0; j < bip->degree[i]; ++j) {
				ave[i] += bip->rela[i][j];	
			}
			ave[i] /= bip->degree[i];
		}
	}
	return ave;	
}

//divide Bip into two parts.
//return two struct LineFile. 
//the first one is always the small one.
//the second is always the large one.
void divideBIP(BIP *bl, BIP *br, double rate, struct LineFile **small, struct LineFile **big) {
	if (rate <=0 || rate >= 1) {
		LOG(LOG_FATAL, "divide_Bip error: wrong rate.");
	}
	rate = rate>0.5?1-rate:rate;

	long l1, l2;
	if (bl->edgesNum > 100000) {
		l1 = (long)(bl->edgesNum*(rate+0.1));
		l2 = (long)(bl->edgesNum*(1-rate+0.1));
	}
	else {
		l2 = l1 = bl->edgesNum;
	}

	*small = createLF(NULL);
	*big = createLF(NULL);

	(*small)->i1 = smalloc(l1*sizeof(int));
	(*big)->i1 = smalloc(l2*sizeof(int));
	
	if (bl->rela) {
		(*small)->i2 = smalloc(l1*sizeof(int));
		(*big)->i2 = smalloc(l2*sizeof(int));
	}
	if (bl->aler) {
		(*small)->d1 = smalloc(l1 * sizeof(double));
		(*big)->d1 = smalloc(l2 * sizeof(double));
	}

	long line1=0, line2=0;

	int *lsign = scalloc(bl->maxId + 1, sizeof(int));
	int *ldegree = smalloc((bl->maxId + 1)*sizeof(int));
	memcpy(ldegree, bl->degree, (bl->maxId + 1)*sizeof(int));

	int *rsign = scalloc(br->maxId + 1, sizeof(int));
	int *rdegree = smalloc((br->maxId + 1)*sizeof(int));
	memcpy(rdegree, br->degree, (br->maxId + 1)*sizeof(int));

	int i, j, neigh;
	for (i=0; i<bl->maxId + 1; ++i) {
		for (j=0; j<bl->degree[i]; ++j) {
			neigh = bl->rela[i][j];

			if (randomd01() < rate) {
				if ((ldegree[i] == 1 && lsign[i] == 0) || (rdegree[neigh] == 1 && rsign[neigh] == 0)) {
					(*big)->i1[line2] = i;
					if (bl->rela) {
						(*big)->i2[line2] = neigh;
					}
					if (bl->aler) {
						(*big)->d1[line2] = bl->aler[i][j];
					}
					--ldegree[i];
					--rdegree[neigh];
					lsign[i] = 1;
					rsign[neigh] = 1;
					++line2;
					continue;
				}
				(*small)->i1[line1] = i;	
				if (bl->rela) {
					(*small)->i2[line1] = neigh;	
				}
				if (bl->aler) {
					(*small)->d1[line1] = bl->aler[i][j];
				}
				--ldegree[i];
				--rdegree[neigh];
				++line1;
				continue;
			}

			(*big)->i1[line2] = i;	
			if (bl->rela) {
				(*big)->i2[line2] = neigh;	
			}
			if (bl->aler) {
				(*big)->d1[line2] = bl->aler[i][j];
			}
			lsign[i] = 1;
			rsign[neigh] = 1;
			--ldegree[i];
			--rdegree[neigh];
			++line2;
		}
	}
	if ((line1 > l1) || (line2 > l2)) {
		LOG(LOG_FATAL, "l1 and l2 two small.");
	}

	free(lsign);
	free(rsign);
	free(ldegree);
	free(rdegree);

	(*small)->linesNum = line1;
	(*big)->linesNum = line2;

	LOG(LOG_INFO, "Divide left&right BIP into big&small LF:");
	LOG(LOG_INFO, "  rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.", rate, line2, line1);
}

void freeBIPS(BIPS *bips) {
	freeCORES(bips);
}

BIPS *createBIPS(const struct LineFile * const lf){
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	BIPS *bips = smalloc(sizeof(BIPS));
	bips->num = 2 * (columnsNumLF(lf) - 1);
	bips->sign = smalloc(bips->num * sizeof(enum TYPE));
	bips->core = smalloc(bips->num * sizeof(BIP *));

	long edgesNum = lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;

	int lmaxId, lminId, *ldegree, ldegreeMax, ldegreeMin, lidNum;
	set_maxId_minId_BIP(i1, edgesNum, &lmaxId, &lminId);
	ldegree = scalloc(lmaxId+1, sizeof(int));
	set_degree_degreeMax_degreeMin_idNum_BIP(i1, edgesNum, lmaxId, ldegree, &ldegreeMax, &ldegreeMin, &lidNum);

	int rmaxId, rminId, *rdegree, rdegreeMax, rdegreeMin, ridNum;
	set_maxId_minId_BIP(i2, edgesNum, &rmaxId, &rminId);
	rdegree = scalloc(rmaxId+1, sizeof(int));
	set_degree_degreeMax_degreeMin_idNum_BIP(i2, edgesNum, rmaxId, rdegree, &rdegreeMax, &rdegreeMin, &ridNum);
	
	bips->core[0] = halfBIP(edgesNum, i1, i2, NULL, lmaxId, lminId, ldegree, ldegreeMax, ldegreeMin, lidNum);
	bips->sign[0] = INT;
	bips->core[1] = halfBIP(edgesNum, i2, i1, NULL, rmaxId, rminId, rdegree, rdegreeMax, rdegreeMin, ridNum);
	bips->sign[1] = INT;

	int i, j = 2;
	for (i = 2; i < lf->iNum; ++i) {
		int *ii = *(lf->ilist[i]);
		if (ii) {
			bips->core[j] = halfBIP(edgesNum, i1, ii, NULL, lmaxId, lminId, ldegree, ldegreeMax, ldegreeMin, lidNum);
			bips->sign[j] = INT;
			++j;
			bips->core[j] = halfBIP(edgesNum, i2, ii, NULL, rmaxId, rminId, rdegree, rdegreeMax, rdegreeMin, ridNum);
			bips->sign[j] = INT;
			++j;
		}
	}
	for (i = 0; i < lf->dNum; ++i) {
		double *dd = *(lf->dlist[i]);
		if (dd) {
			bips->core[j] = halfBIP(edgesNum, i1, NULL, dd, lmaxId, lminId, ldegree, ldegreeMax, ldegreeMin, lidNum);
			bips->sign[j] = DOUBLE;
			++j;
			bips->core[j] = halfBIP(edgesNum, i2, NULL, dd, rmaxId, rminId, rdegree, rdegreeMax, rdegreeMin, ridNum);
			bips->sign[j] = DOUBLE;
			++j;
		}
	}

	free(ldegree);
	free(rdegree);

	LOG(LOG_INFO, "BIPS created, has [%3d] left&right BIP pairs", bips->num/2);
	LOG(LOG_INFO, "  left  = Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld",\
		   	lmaxId, lminId, lidNum, ldegreeMax, ldegreeMin, edgesNum);
	LOG(LOG_INFO, "  right = Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d, edgesNum: %5ld",\
		   	rmaxId, rminId, ridNum, rdegreeMax, rdegreeMin, edgesNum);
	return bips;
}

static void i3d1more(BIPS *bs, struct LineFile *lf, long linenum, int i, int j) {
	int k, ii = 2, di = 0;
	for (k = 2; k < bs->num; k += 2) {
		if (bs->core[k]->rela) {
			(*(lf->ilist[ii++]))[linenum] = bs->core[k]->rela[i][j];
		}
		else if (bs->core[k]->aler) {
			(*(lf->dlist[di++]))[linenum] = bs->core[k]->aler[i][j];
		}
		else {
			LOG(LOG_FATAL, "BIPS is broken.");
		}
	}
}

void divideBIPS(BIPS *bs, double rate, struct LineFile **small, struct LineFile **big) {
	if (rate <= 0 || rate >= 1) {
		LOG(LOG_FATAL, "divide_Bip error: wrong rate.");
	}
	if (bs->num < 2) LOG(LOG_FATAL, "wrong BIPS");
	rate = rate > 0.5 ? 1-rate : rate;

	BIP *bl = bs->core[0];
	BIP *br = bs->core[1];

	long l1, l2;
	if (bl->edgesNum > 100000) {
		l1 = (long)(bl->edgesNum * (rate+0.1));
		l2 = (long)(bl->edgesNum * (1-rate+0.1));
	}
	else {
		l2 = l1 = bl->edgesNum;
	}

	*small = createLF(NULL);
	*big = createLF(NULL);

	(*small)->i1 = smalloc(l1 * sizeof(int));
	(*big)->i1 = smalloc(l2 * sizeof(int));
	
	(*small)->i2 = smalloc(l1 * sizeof(int));
	(*big)->i2 = smalloc(l2 * sizeof(int));


	int i;
	for (i = 2; i < bs->num; i += 2) {
		if (bs->core[i]->rela) {
			*(nextiLF(*small)) = smalloc(l1 * sizeof(int));
			*(nextiLF(*big)) = smalloc(l2 * sizeof(int));
		}
		else if (bs->core[i]->aler) {
			*(nextdLF(*small)) = smalloc(l1 * sizeof(double));
			*(nextdLF(*big)) = smalloc(l2 * sizeof(double));
		}
		else {
			LOG(LOG_FATAL, "BIPS is broken.");
		}
	}

	long line1=0, line2=0;

	int *lsign = scalloc(bl->maxId + 1, sizeof(int));
	int *ldegree = smalloc((bl->maxId + 1)*sizeof(int));
	memcpy(ldegree, bl->degree, (bl->maxId + 1)*sizeof(int));

	int *rsign = scalloc(br->maxId + 1, sizeof(int));
	int *rdegree = smalloc((br->maxId + 1)*sizeof(int));
	memcpy(rdegree, br->degree, (br->maxId + 1)*sizeof(int));

	int j, neigh;
	for (i=0; i<bl->maxId + 1; ++i) {
		for (j=0; j<bl->degree[i]; ++j) {
			neigh = bl->rela[i][j];

			if (randomd01() < rate) {
				if ((ldegree[i] == 1 && lsign[i] == 0) || (rdegree[neigh] == 1 && rsign[neigh] == 0)) {
					(*big)->i1[line2] = i;
					(*big)->i2[line2] = neigh;
					i3d1more(bs, *big, line2, i, j);
					--ldegree[i];
					--rdegree[neigh];
					lsign[i] = 1;
					rsign[neigh] = 1;
					++line2;
					continue;
				}
				(*small)->i1[line1] = i;	
				(*small)->i2[line1] = neigh;	
				i3d1more(bs, *small, line1, i, j);
				--ldegree[i];
				--rdegree[neigh];
				++line1;
				continue;
			}

			(*big)->i1[line2] = i;	
			(*big)->i2[line2] = neigh;	
			i3d1more(bs, *big, line2, i, j);
			lsign[i] = 1;
			rsign[neigh] = 1;
			--ldegree[i];
			--rdegree[neigh];
			++line2;
		}
	}
	if ((line1 > l1) || (line2 > l2)) {
		LOG(LOG_FATAL, "l1 and l2 two small.");
	}

	free(lsign);
	free(rsign);
	free(ldegree);
	free(rdegree);

	(*small)->linesNum = line1;
	(*big)->linesNum = line2;

	LOG(LOG_INFO, "Divide left&right BIP into big&small LF:");
	LOG(LOG_INFO, "  rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.", rate, line2, line1);
}
