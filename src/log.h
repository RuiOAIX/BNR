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
typedef enum LOG_LEVEL {
	LOG_FATAL,
	LOG_WARN,
	LOG_OP,
	LOG_INFO,
	LOG_DBG,
} LOG_LEVEL;

#include <stdio.h>
#include <stdlib.h>

#define LOG(level, ...) do {\
	if (level <= log_level) {\
		if (log_file == stdout || log_file == stderr) { \
			if (level==LOG_FATAL) fprintf(log_file, "[\033[7;31m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_WARN)  fprintf(log_file, "[\033[1;33m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_OP)    fprintf(log_file, "[\033[7;36m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_INFO)  fprintf(log_file, "[\033[1;32m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
			if (level==LOG_DBG)   fprintf(log_file, "[\033[7;37m%17s\033[1;37m:\033[1;34m%-4d \033[1;35m%12s()\033[0m] =>> ", __FILE__, __LINE__, __FUNCTION__);\
		} \
		else { \
			fprintf(log_file, "[%17s:%-4d %12s()] =>> ", __FILE__, __LINE__, __FUNCTION__);\
		} \
		fprintf(log_file, __VA_ARGS__);\
		fprintf(log_file, "\n");\
		fflush(log_file);\
	}\
	if (level == LOG_FATAL) {\
		exit(-1);\
	}\
} while(0)

LOG_LEVEL log_level;
FILE *log_file;

void set_log_level(LOG_LEVEL ll);
LOG_LEVEL get_log_level(void);
void set_log_file(char *logfilename);
void release_log_file(void);

#endif
