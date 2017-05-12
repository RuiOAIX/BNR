/**
 * this file is used to read text file into struct LineFile.
 * struct LineFile has its limit. 
 * for now, at most, it can process a line with 9 int , 9 double, 9 string(char *), that means 45 parts in one line.
 * lines will be seprated into parts with "\t", space, ":", "\n", ",".
 *
 * when you want to read a file, just use create_LineFile, with right parameters.
 * when you use a struct LineFile, you should have a purpose.
 * 	for example, use lf(a struct LineFile object) to create a iidNet, then you should use i1,i2,d1.
 * 		then you should check lf->i1, lf->i2, lf->d1 != NULL.
 * 			if any one of these three is NULL, there is something wrong.
 *
 * when you get a file contains only 2 parts in a line, but you use create_LineFile(xx, 1, 1, 1, -1);
 * 	then you get -1 for each i3[x];
 *
 * when you get a file contains 3 int parts in a line, but you use create_LineFile(xx, 1, 1, -1);
 * 	then you only fetch the first 2 parts from each line. lf->i3 will be NULL.
 *
 *  Author: RuiXiao <xrfind@gmail.com>
 */

#ifndef LINEFILE_H 
#define LINEFILE_H

struct LineFile {
	//private
	int ***ilist;
	double ***dlist;
	char ****slist;
	int iNum, dNum, sNum;

	//public
	int *i1; int *i2; int *i3; int *i4; int *i5;
	int *i6; int *i7; int *i8; int *i9;
	double *d1; double *d2; double *d3;
	double *d4; double *d5; double *d6;
	double *d7; double *d8; double *d9;
	char **s1; char **s2; char **s3;
	char **s4; char **s5; char **s6;
	char **s7; char **s8; char **s9;

	long linesNum;
	long memNum;
};

/**
 * create_LineFile("fileA", 1, 2, 1, 2, -1) means: 
 * 	for each line in fileA, only read the first four parts into struct.
 * 	the arg "1" means: the first and the third part is an int.
 * 	the arg "2" means: the second and the fourth part is a double.
 * 	the arg "-1" is the guard to let function know the last argument's position.
 * if the number of the parts in one line is less than 4,
 * 	than the non-existed parts of the line are given "-1".
 * if more than 4, only 4 parts is read in.
 *
 * "1" means int.
 * "2" means double.
 * "3" means char *(string).
 * "-1" means guard.
 *
 * if the first argument is "NULL", or
 * if the last argument is not "-1", or
 * if any of the "int" arguments (in the above example, the "int" arguments are 1,2,1,2) between the first and the last arguments is not belong to {1, 2, 3}, or
 * if the number of the "int" arguments is zero.
 * then create_LineFile will return a empty but compete LineFile which can be used in free_LineFile(lf);
 * non-int type arguments are ignored.
 *
 */
int **nextiLF(struct LineFile *lf);
double **nextdLF(struct LineFile *lf);
char ***nextsLF(struct LineFile *lf);

struct LineFile *createLF(char * filename, ...);
/// free;
void freeLF(struct LineFile *lf);

int columnsNumLF(const struct LineFile * const lf);

/// print the struct LineFile's content into a file
void printLF(struct LineFile *lf, char *filename);

//generate a new LineFile, simply join two LineFile, No check on duplicate lines.
struct LineFile *sumLF(struct LineFile *lf1, struct LineFile *lf2);

//clone a new LF
struct LineFile *cloneLF(struct LineFile *lf);


#endif
