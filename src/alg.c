#include "alg.h"
#include "sort.h"
#include <string.h>

void settopLrank(int L, int rmaxId, int *rdegree, double *rsource, int *rids, int *topL, int *rank) {
	
	//set ridts and rank.
	int j=0, i;
	double dtmp;
	for (i = 0; i < rmaxId + 1; ++i) {
		rids[i] = i;
		//set unexisted item's source to -2.
		if (!rdegree[i]) {
			rsource[i] = -2;
		}

		if (rsource[i] > 0) {
			//valid rsource and ridts will be in the top j positions of the array.
			dtmp = rsource[j];
			rsource[j] = rsource[i];
			rsource[i] = dtmp;

			rids[i] = rids[j];
			rids[j] = i;

			++j;
		}
	}
	//to this step, rsource contains four parts: 
	//1, nomral rsource[x] , which (0, 1].
	//2, rsource[x] = 0, which x isn't selected in this target's network. 
	//3, rsource[x] = -1, which x has already been selected by target in train.
	//4, rsource[x] = -2, which x is the hole, x isn't selected by anyone.
	
	//after qsort_di_desc, the id of the item with most source will be in ridts[0];
	qsort_di_desc(rsource, 0, j - 1, rids);
	//copy the top L itemid into topL.
	memcpy(topL, rids, L * sizeof(int));
	//get rank;
	for (i = 0; i < rmaxId + 1; ++i) {
		rank[rids[i]] = i+1;
	}
}
