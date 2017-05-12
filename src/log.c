#include "log.h"
#include "utils.h"

static char *loglevelis(enum LOGLEVEL ll) {
	if (ll == LOG_FATAL) return "LOG_FATAL";
	else if (ll == LOG_WARN) return "LOG_WARN";
	else if (ll == LOG_OP) return "LOG_OP";
	else if (ll == LOG_INFO) return "LOG_INFO";
	else return "LOG_DBG";
}

void loginit(char *logfilename, enum LOGLEVEL ll) {
	loglevel = ll;
	if (loglevel > LOG_DBG || loglevel < LOG_FATAL) {
		loglevel = LOG_DBG;
		logfile = stderr;
	   	LOG(LOG_FATAL, "wrong LOGLEVEL type"); 
	}
	logfile = sfopen(logfilename, "w");
	if (!logfile) {
		logfile = stdout;
		LOG(LOG_INFO, "log file is set to STDOUT, log level is %s", loglevelis(ll));
	}
	else {
		LOG(LOG_INFO, "log file is set to %s, log level is %s", logfilename, loglevelis(ll));
	}
}

void setloglevel(enum LOGLEVEL ll) {
	loglevel = ll;
}

enum LOGLEVEL getloglevel(void) {
	return loglevel;
}

void releaselog(void) {
	if (!(logfile == stdout || logfile == stderr)) {
		fclose(logfile);
	}
}
