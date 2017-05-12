#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "core.h"
#include "linefile.h"

typedef struct CORE NET;
void freeNET(NET *net);
//no direction, no duplicate pairs even "1 2" and "2 1" is not allowed.
NET *createNET(const struct LineFile * const lf);
void printNET(NET *net, char *filename);


typedef struct CORES NETS; 
void freeNETS(NETS *net);
NETS *createNETS(const struct LineFile * const file);

#endif
