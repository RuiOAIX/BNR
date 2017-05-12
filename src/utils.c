#include "utils.h"
#include "log.h"
#include <stdlib.h>

char *trueorfalse(bool b) {
	return b ? "true" : "false";
}

char *leftorright(enum SIDE s) {
	return  s ? "RIGHT" : "LEFT";
}

char *whichtype(enum TYPE t) {
	if (t == INT) return "INT";
	else if (t == DOUBLE) return "DOUBLE";
	else if (t == CSTRING) return "CSTRING";
	else return "NO-IDEA";
}

//memory 
inline void *malloc_safe(size_t size, const char *filename, const int lineNum) {
	if (size == 0) return NULL;
	void *tmp = malloc(size);
	if (tmp == NULL) {
		LOG(LOG_FATAL, "malloc failed: \"%s\" file, %d line.\n",  filename, lineNum);
	}
	return tmp;
}
inline void *calloc_safe(size_t num, size_t size, const char *filename, const int lineNum) {
	if (num == 0 || size == 0) return NULL;
	void *tmp = calloc(num, size);
	if (tmp == NULL) {
		LOG(LOG_FATAL, "calloc failed: \"%s\" file, %d line.\n",  filename, lineNum);
	}
	return tmp;
}
inline void *realloc_safe(void *p, size_t size, const char *filename, const int lineNum) {
	if (size == 0) {
		LOG(LOG_WARN, "realloc size == 0, return the origin pointer");
		return p;
	}
	void *tmp = realloc(p, size);
	if (tmp == NULL) {
		LOG(LOG_FATAL, "realloc failed: \"%s\" file, %d line.\n",  filename, lineNum);
	}
	return tmp;
}

//file
FILE *fopen_safe(const char *openfilename, const char *mode, char *filename, const int lineNum) {
	if (!openfilename) return NULL;
	FILE *fp = fopen(openfilename, mode);
	if (fp == NULL) {
		LOG(LOG_FATAL, "fopen failed: \"%s\" file, %d line, can not open \"%s\"",  filename, lineNum, openfilename);
	}
	return fp;
}
