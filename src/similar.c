#include "similar.h"
#include "log.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

struct LineFile *cosineSM(BIP *left, BIP *right) {
	int lmaxId = left->maxId;
	int rmaxId = right->maxId;
	int *ldegree = left->degree;
	int **lrela = left->rela;

	int *sign = scalloc((rmaxId + 1),sizeof(int));

	struct LineFile *simfile = createLF(NULL);

	int step = 1000000;
	int con = step;
	int *i1 = smalloc(con * sizeof(int));
	int *i2 = smalloc(con * sizeof(int));
	double *d1 = smalloc(con * sizeof(double));

	long linesNum = 0;
	int i ,j, k, num_samechoice;
	double simvl;

	for (i = 0; i < lmaxId; ++i) {
		if (ldegree[i]) {
			for (k = 0; k < ldegree[i]; ++k) {
				sign[lrela[i][k]] = 1;
			}
			for (j = i + 1; j < lmaxId + 1; ++j) {
				if (ldegree[j]) {
					num_samechoice = 0;
					for (k = 0; k < ldegree[j]; ++k) {
						num_samechoice += sign[lrela[j][k]];
					}
					if (num_samechoice) {
						simvl = (double)num_samechoice/sqrt(ldegree[i] * ldegree[j]);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = simvl;
						++linesNum;
						if (linesNum == con) {
							con += step;
							i1 = srealloc(i1, con * sizeof(int));
							i2 = srealloc(i2, con * sizeof(int));
							d1 = srealloc(d1, con * sizeof(double));
						}
					}
				}
			}
			for (k = 0; k < ldegree[i]; ++k) {
				sign[lrela[i][k]] = 0;
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;

	LOG(LOG_INFO, "Cosine similarity calculated based on a left&right BIP pair, return a LineFile, linesNum: %ld.", linesNum);
	return simfile;
}
