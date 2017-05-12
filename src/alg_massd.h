#ifndef MASSD_H
#define MASSD_H 

#include "metrics.h"
#include "task.h"

struct METRICS *massd(struct TASK *task);
struct TASK *massdT(struct OPTION *op);

#endif
