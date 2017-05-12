/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly by  other functions.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */
#ifndef DEBUG_H
#define DEBUG_H

//#define NDEBUG

/****************VERBOSE LEVEL*******************************************************************/
//#define LOG_FATAL    (1)
//#define LOG_WARN     (2)
//#define LOG_OP       (3)
//#define LOG_INFO     (4)
//#define LOG_DBG      (5)
enum LOGLEVEL {
	LOG_FATAL,
	LOG_WARN,
	LOG_OP,
	LOG_INFO,
	LOG_DBG,
};

#include <stdio.h>
#include <stdlib.h>

#define LOG(level, ...) do {\
	if (level <= loglevel) {\
		if (logfile == stdout || logfile == stderr) { \
			if (level==LOG_FATAL) fprintf(logfile, "[\033[7;31m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_WARN)  fprintf(logfile, "[\033[1;33m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_OP)    fprintf(logfile, "[\033[7;36m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_INFO)  fprintf(logfile, "[\033[1;32m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_DBG)   fprintf(logfile, "[\033[7;37m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
		} \
		else { \
			fprintf(logfile, "[%17s:%-4d %12s()] =>> ", __FILE__, __LINE__, __FUNCTION__);\
		} \
		fprintf(logfile, __VA_ARGS__);\
		fprintf(logfile, "\n");\
		fflush(logfile);\
	}\
	if (level == LOG_FATAL) {\
		exit(-1);\
	}\
} while(0)

FILE *logfile;
enum LOGLEVEL loglevel;

void setloglevel(enum LOGLEVEL ll);
enum LOGLEVEL getloglevel(void);
void loginit(char *logfilename, enum LOGLEVEL ll);
void releaselog(void);

#endif
