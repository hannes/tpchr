#include <Rdefines.h>
#include "dss.h"
#include "dsstypes.h"

void gen_tbl(int tnum, DSS_HUGE start, DSS_HUGE count, long upd_num);
void load_dists(void);

// 'inspired' by driver.c::main(). also, what a mess
static SEXP dbgen_R(SEXP sf) {
	DSS_HUGE rowcnt = 0, minrow = 0;
	double flt_scale;
	long upd_num = 0;
	DSS_HUGE i;

	table = (1 << CUST) | (1 << SUPP) | (1 << NATION) | (1 << REGION)
			| (1 << PART_PSUPP) | (1 << ORDER_LINE);
	force = 0;
	insert_segments = 0;
	delete_segments = 0;
	insert_orders_segment = 0;
	insert_lineitem_segment = 0;
	delete_segment = 0;
	verbose = 0;
	set_seeds = 0;
	scale = 1;
	flt_scale = 1.0;
	updates = 0;
	step = -1;
	tdefs[ORDER].base *=
	ORDERS_PER_CUST; /* have to do this after init */
	tdefs[LINE].base *=
	ORDERS_PER_CUST; /* have to do this after init */
	tdefs[ORDER_LINE].base *=
	ORDERS_PER_CUST; /* have to do this after init */
	children = 1;
	d_path = NULL;

	flt_scale = 0.01;
	if (flt_scale < MIN_SCALE)
	{
		int i;
		int int_scale;

		scale = 1;
		int_scale = (int)(1000 * flt_scale);
		for (i = PART; i < REGION; i++)
		{
			tdefs[i].base = (DSS_HUGE)(int_scale * tdefs[i].base)/1000;
			if (tdefs[i].base < 1)
				tdefs[i].base = 1;
		}
	}
	else
		scale = (long) flt_scale;

	load_dists();

	/* have to do this after init */
	tdefs[NATION].base = nations.count;
	tdefs[REGION].base = regions.count;

	/*
	 * traverse the tables, invoking the appropriate data generation routine for any to be built
	 */
	for (i = PART; i <= REGION; i++)
		if (table & (1 << i)) {
			minrow = 1;
			if (i < NATION)
				rowcnt = tdefs[i].base * scale;
			else
				rowcnt = tdefs[i].base;
			gen_tbl((int) i, minrow, rowcnt, upd_num);
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
