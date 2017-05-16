/**
 *  Author: RuiOAIX <rui.oaix@gmail.com>
 */

#ifndef LINEFILE_H 
#define LINEFILE_H

#define MAX_FILE_COLUMN_NUMBER 256
#define LINES_STEP 100000
#define LINE_LENGTH 2000
#define LINES_READIN 1000

struct LineFile {
	//private
	int **ilist;
	double **dlist;
	char ***slist;
	int iNum, dNum, sNum;
	long linesNum;
	long memNum;
};

struct LineFile *create_linefile(char * filename, ...);
void free_linefile(struct LineFile *lf);
void save_linefile(struct LineFile *lf, char *filename);
struct LineFile *union_all_linefile(struct LineFile *lf1, struct LineFile *lf2);
struct LineFile *clone_linefile(struct LineFile *lf);

#endif
