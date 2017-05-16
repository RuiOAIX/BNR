#include "log.h"
#include "utils.h"

static FILE *log_file;
static LOG_LEVEL log_level;

void set_log_level(LOG_LEVEL ll) {
	log_level = ll;
}

char *log_level_string[] = {"LOG_FATAL", "LOG_WARN", "LOG_OP", "LOG_INFO", "LOG_DBG"};

void set_log_file(char *filename) {
	if (log_level > LOG_DBG || log_level < LOG_FATAL) {
		log_level = LOG_DBG;
		log_file = stderr;
	   	LOG(LOG_FATAL, "wrong LOGLEVEL type"); 
	}
	log_file = sfopen(filename, "w");
	if (!log_file) {
		log_file = stdout;
		LOG(LOG_INFO, "log file is set to STDOUT, log level is %s", log_level_string[log_level]);
	}
	else {
		LOG(LOG_INFO, "log file is set to %s, log level is %s", filename, log_level_string[log_level]);
	}
}


LOG_LEVEL get_log_level(void) {
	return log_level;
}

void release_log_file(void) {
	if (log_file != stdout && log_file != stderr) fclose(log_file);
}
