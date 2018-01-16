#include <Rdefines.h>
#include "dss.h"
#include "dsstypes.h"

void gen_tbl (int tnum, DSS_HUGE start, DSS_HUGE count, long upd_num);

static SEXP dbgen_R(SEXP sf) {
	int scale = 1;
	DSS_HUGE rowcnt = (int)(tdefs[ORDER_LINE].base / 10000 * scale * UPD_PCT);

	for (int i = PART; i <= REGION; i++)
		if (table & (1 << i)) {
			int minrow = 1;
			if (i < NATION)
				rowcnt = tdefs[i].base * scale;
			else
				rowcnt = tdefs[i].base;
			//message("Generating data for ", tdefs[i].comment);
			gen_tbl((int) i, minrow, rowcnt, 0);

		}

	return ScalarLogical(1);
}

// R native routine registration
#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}
static const R_CallMethodDef R_CallDef[] = {
CALLDEF(dbgen_R, 1), { NULL, NULL, 0 } };

void R_init_tpchr(DllInfo *dll) {
	R_registerRoutines(dll, NULL, R_CallDef, NULL, NULL);
	R_useDynamicSymbols(dll, FALSE);
}
