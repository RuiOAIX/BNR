#include "log.h"
#include "linefile.h"
#include "utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//create an empty but completive LineFile.
static struct LineFile *init_linefile(void) {
	struct LineFile *lf = smalloc(sizeof(struct LineFile));
	lf->linesNum = 0;
	lf->memNum = 0;
	lf->iNum = 0;
	lf->ilist = 0;
	lf->dNum = 0;
	lf->dlist = 0;
	lf->sNum = 0;
	lf->slist = 0;
	return lf;
}

//alloc memory according to typelist.
static void init_memory_linefile(struct LineFile *lf, unsigned char *types, int typesSize) {
	int flag[256] = {0};

	int i;
	for (i = 0; i < typesSize; ++i) flag[types[i]]++;
	lf->iNum = flag['i'] + flag['I'];
	lf->ilist = smalloc(lf->iNum * sizeof(int *));
	lf->dNum = flag['d'] + flag['D'];
	lf->dlist = smalloc(lf->dNum * sizeof(double *));
	lf->sNum = flag['s'] + flag['S'];
	lf->slist = smalloc(lf->sNum * sizeof(char **));
	
	for (i = 0; i < lf->iNum; i++) lf->ilist[i] = smalloc(LINES_STEP * sizeof(int));
	for (i = 0; i < lf->dNum; i++) lf->dlist[i] = smalloc(LINES_STEP * sizeof(double));
	for (i = 0; i < lf->sNum; i++) lf->slist[i] = smalloc(LINES_STEP * sizeof(char *));

	lf->linesNum = 0;
	lf->memNum = LINES_STEP;
	LOG(LOG_DBG, "Now memNum is %ld, linesNum is %ld.", lf->memNum, lf->linesNum);
}

//increase memory, not need typelist anymore, just check whether point is NULL or not.
static void increase_linefile(struct LineFile *lf) {
	int i;
	for (i=0; i<lf->iNum; ++i) {
		lf->ilist[i] = srealloc(lf->ilist[i], (size_t)(lf->memNum + LINES_STEP) * sizeof(int));
	}
	for (i=0; i<lf->dNum; ++i) {
		lf->dlist[i] = srealloc(lf->dlist[i], (size_t)(lf->memNum + LINES_STEP) * sizeof(double));
	}
	for (i=0; i<lf->sNum; ++i) {
		lf->slist[i] = srealloc(lf->slist[i], (size_t)(lf->memNum + LINES_STEP) * sizeof(char *));
	}
	lf->memNum += LINES_STEP;
	LOG(LOG_DBG, "increase the size of LineFile, now memNum is %ld, linesNum is %ld.", lf->memNum, lf->linesNum);
}

static int save_1line_linefile(struct LineFile *lf, char **parts, unsigned char *types, int typesSize) {
	int IL = 0, DL = 0, SL = 0, has_problem = 0;
	char *pend;
	int i;
	for (i = 0; i < typesSize; ++i) {
		int type = types[i];
		switch(type) {
			case 'i':
			case 'I':
				if (!parts[i]) {
					lf->ilist[IL++][lf->linesNum] = -1;
					continue;
				}
				lf->ilist[IL++][lf->linesNum] = strtol(parts[i], &pend, 10);
				if (pend[0]) {
					LOG(LOG_WARN, "wrong line: line: %ld, i%d part. ASCII: [%d].\n", lf->linesNum, IL, pend[0]);
					has_problem = 1;
				}
				break;
			case 'd':
			case 'D':
				if (!parts[i]) {
					lf->dlist[DL++][lf->linesNum] = -1;
					continue;
				}
				lf->dlist[DL++][lf->linesNum] = strtod(parts[i], &pend);
				if (pend[0]) {
					LOG(LOG_WARN, "wrong line: line: %ld, d%d part. ASCII: [%d].\n", lf->linesNum, DL, pend[0]);
					has_problem = 1;
				}
				break;
			case 's':
			case 'S':
				if (!parts[i]) {
					lf->slist[SL++][lf->linesNum] = 0;
					continue;
				}
				int size = strlen(parts[i]) + 1;
				lf->slist[SL][lf->linesNum] = smalloc(size * sizeof(char));
				memcpy(lf->slist[SL++][lf->linesNum], parts[i], size);
				break;
			default:
				LOG(LOG_FATAL, "wrong type.");
		}
	}
	lf->linesNum++;
	return has_problem;
}

struct LineFile *create_linefile(char *filename, ...) {
	if (!filename) return 0;

	//get types
	char *flag[256] = {0};
	flag['I'] = flag['i'] = "Int";
	flag['D'] = flag['d'] = "Double";
	flag['S'] = flag['s'] = "String";
	va_list vl;
	va_start(vl, filename);
	unsigned char types[MAX_FILE_COLUMN_NUMBER] = {0};
	int typesSize = 0, type = 0;
	while ((type = va_arg(vl, int)) < 256 && type > -1 && flag[type]) {
		if (typesSize == MAX_FILE_COLUMN_NUMBER) LOG(LOG_FATAL, "%s =>> too much args.", __func__);
		types[typesSize++] = type;
	}
	va_end(vl);

	if (!typesSize) return 0;

	//line_style
	LOG(LOG_INFO, "Prepare to read \"%s\" with following style: ", filename);
	char *line_style = scalloc(typesSize * 8 + 1, sizeof(char));
	int i;
	for (i = 0; i < typesSize; ++i) {
		strcat(line_style, "  ");
		strcat(line_style, flag[types[i]]);
	}
	LOG(LOG_INFO, "%s", line_style);
	free(line_style);

	//check filename.
	FILE *fp = sfopen(filename, "r");

	struct LineFile *lf = init_linefile();
	init_memory_linefile(lf, types, typesSize);

	char *delimiter = "\t ,:\n";
	char *line = smalloc(LINE_LENGTH * sizeof(char));
	char **parts = smalloc(typesSize * sizeof(char *));
	int has_problem = 0;
	while(fgets(line, LINE_LENGTH, fp)) {
		parts[0] = strtok(line, delimiter);
		for (i = 1; i < typesSize; ++i) {
			parts[i] = strtok(NULL, delimiter);
		}
		if (lf->linesNum == lf->memNum) increase_linefile(lf);
		has_problem |= save_1line_linefile(lf, parts, types, typesSize);
	}

	fclose(fp);
	free(line);
	free(parts);

	if (has_problem) LOG(LOG_FATAL, "but file \"%s\" has some non-valid lines.", filename); 
	LOG(LOG_INFO, "successfully readin %ld lines, all lines in \"%s\" is valid.", lf->linesNum, filename);
	return lf;
}

void free_linefile(struct LineFile *lf) {
	LOG(LOG_DBG, "free a struct LineFile.");
	int i;
	long j;
	for (i = 0; i < lf->iNum; ++i) {
		free(lf->ilist[i]);
	}
	for (i = 0; i < lf->dNum; ++i) {
		free(lf->dlist[i]);
	}
	for (i = 0; i < lf->sNum; ++i) {
		for (j = 0; j < lf->linesNum; ++j) {
			free(lf->slist[i][j]);
		}
		free(lf->slist[i]);
	}
	free(lf->ilist);
	free(lf->dlist);
	free(lf->slist);
	free(lf);
}

void save_linefile(struct LineFile *lf, char *filename) {
	if (!lf) {
		LOG(LOG_DBG, "lf == NULL, print nothing.\n");
		return;
	}
	FILE *fp = sfopen(filename, "w");
	int i;
	long j;
	for (j = 0; j < lf->linesNum; ++j) {
		for (i=0; i<lf->iNum; ++i) {
			fprintf(fp, "%d\t", lf->ilist[i][j]);
		}
		for (i=0; i<lf->dNum; ++i) {
			fprintf(fp, "%f\t", lf->dlist[i][j]);
		}
		for (i=0; i<lf->sNum; ++i) {
			fprintf(fp, "%s\t", lf->slist[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	LOG(LOG_INFO, "LineFile saved in \"%s\"", filename);
}

struct LineFile *union_all_linefile(struct LineFile *lf1, struct LineFile *lf2) {
	if (!lf1 || !lf2) {
		LOG(LOG_WARN, "lf1 or lf2 is NULL, return NULL.");
		return 0;
	}
	if (lf1->iNum != lf2->iNum || lf1->dNum != lf2->dNum || lf1->sNum == lf2->sNum) {
		LOG(LOG_WARN, "Structures are not same, return NULL.");
		return 0;
	}

	struct LineFile *lf = init_linefile();
	lf->linesNum = lf1->linesNum + lf2->linesNum;
	lf->memNum = lf->linesNum;
	lf->iNum = lf1->iNum;
	lf->dNum = lf1->dNum;
	lf->sNum = lf1->sNum;
	lf->ilist = smalloc(lf->iNum * sizeof(int *));
	lf->dlist = smalloc(lf->dNum * sizeof(double *));
	lf->slist = smalloc(lf->sNum * sizeof(char **));

	int i;
	long j;
	for (i = 0; i < lf->iNum; ++i) {
		lf->ilist[i] = smalloc(lf->linesNum * sizeof(int));
		for (j = 0; j < lf1->linesNum; ++j) {
			lf->ilist[i][j] = lf1->ilist[i][j];
		}
		for (j = 0; j < lf2->linesNum; ++j) {
			lf->ilist[i][j + lf1->linesNum] = lf1->ilist[i][j];
		}
	}
	for (i = 0; i < lf->dNum; ++i) {
		lf->dlist[i] = smalloc(lf->linesNum * sizeof(double));
		for (j = 0; j < lf1->linesNum; ++j) {
			lf->dlist[i][j] = lf1->dlist[i][j];
		}
		for (j = 0; j < lf2->linesNum; ++j) {
			lf->dlist[i][j + lf1->linesNum] = lf1->dlist[i][j];
		}
	}
	for (i = 0; i < lf->sNum; ++i) {
		lf->slist[i] = smalloc(lf->linesNum * sizeof(char *));
		for (j = 0; j < lf1->linesNum; ++j) {
			int size = strlen(lf1->slist[i][j]) + 1;
			lf->slist[i][j] = smalloc(size * sizeof(char));
			memcpy(lf->slist[i][j], lf1->slist[i][j], size * sizeof(char));
		}
		for (j = 0; j < lf2->linesNum; ++j) {
			int size = strlen(lf2->slist[i][j]) + 1;
			lf->slist[i][j + lf1->linesNum] = smalloc(size * sizeof(char));
			memcpy(lf->slist[i][j + lf1->linesNum], lf2->slist[i][j], size * sizeof(char));
		}
	}

	LOG(LOG_INFO, "Union-All two LF: linesNum of the union LF is %ld and linesNum of old LFs is %ld and %ld", lf1->linesNum, lf2->linesNum, lf->linesNum);
	return lf;
}

struct LineFile *clone_linefile(struct LineFile *lf) {
	if (!lf) {
		LOG(LOG_WARN, "source LineFile is NULL, return NULL.");
		return 0;
	}

	struct LineFile *newlf = init_linefile();
	newlf->memNum = newlf->linesNum = lf->linesNum;
	newlf->iNum = lf->iNum;
	newlf->dNum = lf->dNum;
	newlf->sNum = lf->sNum;
	newlf->ilist = smalloc(lf->iNum * sizeof(int *));
	newlf->dlist = smalloc(lf->dNum * sizeof(double *));
	newlf->slist = smalloc(lf->sNum * sizeof(char **));

	int i;
	long j;
	for (i=0; i<lf->iNum; ++i) {
		newlf->ilist[i] = smalloc(lf->linesNum * sizeof(int));
		for (j = 0; j < lf->linesNum; ++j) {
			newlf->ilist[i][j] = lf->ilist[i][j];
		}
	}
	for (i=0; i<lf->dNum; ++i) {
		newlf->dlist[i] = smalloc(lf->linesNum * sizeof(double));
		for (j = 0; j < lf->linesNum; ++j) {
			newlf->dlist[i][j] = lf->dlist[i][j];
		}
	}
	for (i=0; i<lf->sNum; ++i) {
		newlf->slist[i] = smalloc(lf->linesNum * sizeof(char *));
		for (j = 0; j < lf->linesNum; ++j) {
			int size = strlen(lf->slist[i][j]) + 1;
			newlf->slist[i][j] = smalloc(size * sizeof(char));
			memcpy(newlf->slist[i][j], lf->slist[i][j], size * sizeof(char));
		}
	}

	LOG(LOG_INFO, "LF cloned, new LF linesNum is %ld, old LF linesNum is %ld.", lf->linesNum, newlf->linesNum);
	return newlf;
}


