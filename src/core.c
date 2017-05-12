#include "core.h"

#include <stdlib.h>

void freeCORE(struct CORE *core) {
	if (!core) return;
	int i=0;
	for(i=0; i<core->maxId+1; ++i) {
		if (core->rela) free(core->rela[i]);
		if (core->aler) free(core->aler[i]);
	}
	free(core->degree);
	free(core->rela);
	free(core->aler);
	free(core);
}

void freeCORES(struct CORES *cores) {
	if (!cores) return;
	int i;
	for (i = 0; i < cores->num; ++i) {
		freeCORE(cores->core[i]);
	}
	free(cores->core);
	free(cores->sign);
	free(cores);
}
