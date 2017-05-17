/**
 *  Author: RuiOAIX <rui.oaix@gmail.com>
 */

#ifndef LINEFILE_H 
#define LINEFILE_H

#define MAX_FILE_COLUMN_NUMBER 256
#define LINES_STEP 100000
#define LINE_LENGTH 2000
#define LINES_READIN 1000

typedef struct LineFile {
	int **ilist;
	double **dlist;
	char ***slist;
	int iNum, dNum, sNum;
	long linesNum;
	long memNum;
} LineFile;

LineFile *create_linefile(char * filename, ...);
void free_linefile(LineFile *lf);

LineFile *union_all_linefile(LineFile *lf1, LineFile *lf2);
LineFile *clone_linefile(LineFile *lf);

void save_linefile_to_file(LineFile *lf, char *filename);

#endif
