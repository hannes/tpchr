#include <Rdefines.h>
#include "dss.h"
#include "dsstypes.h"

#define RSTR(somestr) mkCharCE(somestr, CE_UTF8)

#define APPEND_INTSXP(val) INTEGER_POINTER(VECTOR_ELT(as, col++))[of] = (int) val;
#define APPEND_NUMSXP(val) NUMERIC_POINTER(VECTOR_ELT(as, col++))[of] = (double) val;
#define APPEND_STRSXP(val) SET_STRING_ELT( VECTOR_ELT(as, col++), of, RSTR(val));

void gen_tbl(int tnum, DSS_HUGE start, DSS_HUGE count, long upd_num);
void load_dists(void);

static SEXP df_region = NULL;
static size_t off_region = 0;

static int append_region(code_t *c, int mode) {
	int col = 0;
	SEXP as = df_region;
	size_t of = off_region;

	APPEND_INTSXP(c->code);
	APPEND_STRSXP(c->text);
	APPEND_STRSXP(c->comment);

	off_region++;
	return (0);
}

static SEXP df_nation = NULL;
static size_t off_nation = 0;

static int append_nation(code_t *c, int mode) {
	int col = 0;
	SEXP as = df_nation;
	size_t of = off_nation;

	APPEND_INTSXP(c->code);
	APPEND_STRSXP(c->text);
	APPEND_INTSXP(c->join);
	APPEND_STRSXP(c->comment);

	off_nation++;
	return (0);
}

static SEXP df_part = NULL;
static size_t off_part = 0;

static int append_part(part_t *part, int mode) {
	int col = 0;
	SEXP as = df_part;
	size_t of = off_part;

	APPEND_INTSXP(part->partkey);
	APPEND_STRSXP(part->name);
	APPEND_STRSXP(part->mfgr);
	APPEND_STRSXP(part->brand);
	APPEND_STRSXP(part->type);
	APPEND_INTSXP(part->size);
	APPEND_STRSXP(part->container);
	APPEND_NUMSXP(part->retailprice);
	APPEND_STRSXP(part->comment);

	off_part++;
	return (0);
}

static SEXP df_psupp = NULL;
static size_t off_psupp = 0;

static int append_psupp(part_t *part, int mode) {
	SEXP as = df_psupp;

	for (size_t i = 0; i < SUPP_PER_PART; i++) {
		size_t of = off_psupp;
		int col = 0;

		APPEND_INTSXP(part->s[i].partkey);
		APPEND_INTSXP(part->s[i].suppkey);
		APPEND_INTSXP(part->s[i].qty);
		APPEND_NUMSXP(part->s[i].scost);
		APPEND_STRSXP(part->s[i].comment);
		off_psupp++;
	}

	return (0);
}

static int append_part_psupp(part_t *part, int mode) {
	//  tdefs[PART].name = tdefs[PART_PSUPP].name;
	append_part(part, mode);
	append_psupp(part, mode);
	return (0);
}

static SEXP df_order = NULL;
static size_t off_order = 0;

static int append_order(order_t *o, int mode) {
	int col = 0;
	SEXP as = df_order;
	size_t of = off_order;

	APPEND_INTSXP(o->okey);
	APPEND_INTSXP(o->custkey)
	char str[2] = "\0";
	str[0] = o->orderstatus;
	APPEND_STRSXP(str);
	APPEND_NUMSXP(o->totalprice);
	// TODO: use date objects for this?
	APPEND_STRSXP(o->odate);
	APPEND_STRSXP(o->opriority);
	APPEND_STRSXP(o->clerk);
	APPEND_INTSXP(o->spriority);
	APPEND_STRSXP(o->comment);

	off_order++;
	return (0);
}

static SEXP df_lineitem = NULL;
static size_t off_lineitem = 0;

static int append_line(order_t *o, int mode) {
	SEXP as = df_lineitem;

	for (size_t i = 0; i < o->lines; i++) {
		int col = 0;
		size_t of = off_lineitem;

		APPEND_INTSXP(o->l[i].okey);
		APPEND_INTSXP(o->l[i].partkey);
		APPEND_INTSXP(o->l[i].suppkey);
		APPEND_INTSXP(o->l[i].lcnt);
		APPEND_INTSXP(o->l[i].quantity);
		APPEND_NUMSXP(o->l[i].eprice);
		APPEND_NUMSXP(o->l[i].discount);
		APPEND_NUMSXP(o->l[i].tax);
		char str[2] = "X";
		str[0] = o->l[i].rflag[0];
		APPEND_STRSXP(str);
		str[0] = o->l[i].lstatus[0];
		APPEND_STRSXP(str);
		APPEND_STRSXP(o->l[i].sdate);
		APPEND_STRSXP(o->l[i].cdate);
		APPEND_STRSXP(o->l[i].rdate);
		APPEND_STRSXP(o->l[i].shipinstruct);
		APPEND_STRSXP(o->l[i].shipmode);
		APPEND_STRSXP(o->l[i].comment);

		off_lineitem++;
	}
	return (0);
}

static int append_order_line(order_t *o, int mode) {
	//  tdefs[ORDER].name = tdefs[ORDER_LINE].name;
	append_order(o, mode);
	append_line(o, mode);

	return (0);
}

static SEXP df_cust = NULL;
static size_t off_cust = 0;

static int append_cust(customer_t *c, int mode) {
	int col = 0;
	SEXP as = df_cust;
	size_t of = off_cust;

	APPEND_INTSXP(c->custkey);
	APPEND_STRSXP(c->name);
	APPEND_STRSXP(c->address);
	APPEND_INTSXP(c->nation_code);
	APPEND_STRSXP(c->phone);
	APPEND_NUMSXP(c->acctbal);
	APPEND_STRSXP(c->mktsegment);
	APPEND_STRSXP(c->comment);

	off_cust++;
	return (0);

}

static SEXP df_supp = NULL;
static size_t off_supp = 0;

static int append_supp(supplier_t *supp, int mode) {
	int col = 0;
	SEXP as = df_supp;
	size_t of = off_supp;

	APPEND_INTSXP(supp->suppkey);
	APPEND_STRSXP(supp->name);
	APPEND_STRSXP(supp->address);
	APPEND_INTSXP(supp->nation_code);
	APPEND_STRSXP(supp->phone);
	APPEND_NUMSXP(supp->acctbal);
	APPEND_STRSXP(supp->comment);

	off_supp++;
	return (0);

}

static void set_df_len(SEXP s, size_t len) {
	SEXP row_names;
	PROTECT(row_names = allocVector(INTSXP, 2));
	if (!row_names) {
		UNPROTECT(1);
		error("memory allocation");
		return;
	}
	INTEGER(row_names)[0] = NA_INTEGER;
	INTEGER(row_names)[1] = (int) len;
	setAttrib(s, R_RowNamesSymbol, row_names);
	UNPROTECT(1);
}

#define DF_ADD_COL(constr) \
		new_vec = PROTECT(constr(nrow)); \
		if (!new_vec) { \
			error("memory allocation"); \
			UNPROTECT(2); \
			return NULL; \
		} \
		SET_VECTOR_ELT(df, n, new_vec); \
		UNPROTECT(1);

static SEXP create_df(size_t ncol, size_t nrow, char** names_arr,
		SEXPTYPE* types_arr) {
	SEXP df = PROTECT(NEW_LIST(ncol));
	SEXP names = PROTECT(NEW_CHARACTER(ncol));
	SEXP class = PROTECT(mkString("data.frame"));

	if (!df || !names || !class) {
		UNPROTECT(3);
		error("memory allocation");
		return NULL;
	}
	SET_NAMES(df, names);
	SET_CLASS(df, class);
	UNPROTECT(2); // names and class

	for (size_t n = 0; n < ncol; n++) {
		SEXP name = PROTECT(RSTR(names_arr[n]));
		if (!name) {
			UNPROTECT(2); //df and name
			error("memory allocation");
			return NULL;
		}
		SET_STRING_ELT(names, n, name);
		UNPROTECT(1);
		SEXP new_vec = NULL;
		switch (types_arr[n]) {
		case INTSXP:
			DF_ADD_COL(NEW_INTEGER)
			break;
		case STRSXP:
			DF_ADD_COL(NEW_CHARACTER)
			break;
		case REALSXP:
			DF_ADD_COL(NEW_NUMERIC)
			break;
		default:
			UNPROTECT(1);
			error("unknown type");
			return NULL;
		}
	}
	set_df_len(df, nrow);
	UNPROTECT(1); // df
	return df;
}

// 'inspired' by driver.c::main(). also, what a mess
static SEXP dbgen_R(SEXP sf) {
	DSS_HUGE rowcnt = 0, minrow = 0;
	double flt_scale;
	long upd_num = 0;
	DSS_HUGE i;
	// all tables
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

	flt_scale = NUMERIC_POINTER(sf)[0];

	if (flt_scale < MIN_SCALE) {
		int i;
		int int_scale;

		scale = 1;
		int_scale = (int) (1000 * flt_scale);
		for (i = PART; i < REGION; i++) {
			tdefs[i].base = (DSS_HUGE)(int_scale * tdefs[i].base) / 1000;
			if (tdefs[i].base < 1)
				tdefs[i].base = 1;
		}
	} else
		scale = (long) flt_scale;

	load_dists();

	/* have to do this after init */
	tdefs[NATION].base = nations.count;
	tdefs[REGION].base = regions.count;

	// setup loaders
	tdefs[NATION].loader     = append_nation;
	tdefs[REGION].loader     = append_region;
	tdefs[CUST].loader       = append_cust;
	tdefs[SUPP].loader       = append_supp;
	tdefs[PART_PSUPP].loader = append_part_psupp;
	tdefs[ORDER_LINE].loader = append_order_line;

	// setup the various data frames
	{
		char* names_arr[] = { "n_nationkey", "n_name", "n_regionkey",
				"n_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, INTSXP, STRSXP };
		df_nation = PROTECT(
				create_df(4, tdefs[NATION].base, names_arr, types_arr));
	}
	{
		char* names_arr[] = { "r_regionkey", "r_name", "r_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP };
		df_region = PROTECT(
				create_df(3, tdefs[REGION].base, names_arr, types_arr));
	}
	{
		char* names_arr[] = { "c_custkey", "c_name", "c_address", "c_nationkey",
				"c_phone", "c_acctbal", "c_mktsegment", "c_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP, INTSXP, STRSXP,
				REALSXP, STRSXP, STRSXP };
		df_cust = PROTECT(
				create_df(8, tdefs[CUST].base * scale, names_arr, types_arr));
	}
	{
		char* names_arr[] = { "s_suppkey", "s_name", "s_address", "s_nationkey",
				"s_phone", "s_acctbal", "s_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP, INTSXP, STRSXP,
				REALSXP, STRSXP };
		df_supp = PROTECT(
				create_df(7, tdefs[SUPP].base * scale, names_arr, types_arr));
	}
	{
		char* names_arr[] =
				{ "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type",
						"p_size", "p_container", "p_retailprice", "p_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP, STRSXP, STRSXP, INTSXP,
				STRSXP, REALSXP, STRSXP };
		df_part = PROTECT(
				create_df(9, tdefs[PART_PSUPP].base, names_arr, types_arr));
	}
	{
		char* names_arr[] = { "ps_partkey", "ps_suppkey", "ps_availqty",
				"ps_supplycost", "ps_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, REALSXP, STRSXP };
		df_psupp = PROTECT(
				create_df(5, tdefs[PART_PSUPP].base * SUPP_PER_PART, names_arr,
						types_arr));
	}
	{
		char* names_arr[] = { "o_orderkey", "o_custkey", "o_orderstatus",
				"o_totalprice", "o_orderdate", "o_orderpriority", "o_clerk",
				"o_shippriority", "o_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, STRSXP, REALSXP, STRSXP,
				STRSXP, STRSXP, INTSXP, STRSXP };
		df_order = PROTECT(
				create_df(9, tdefs[ORDER_LINE].base, names_arr, types_arr));
	}
	{
		char* names_arr[] = { "l_orderkey", "l_partkey", "l_suppkey",
				"l_linenumber", "l_quantity", "l_extendedprice", "l_discount",
				"l_tax", "l_returnflag", "l_linestatus", "l_shipdate",
				"l_commitdate", "l_receiptdate", "l_shipinstruct", "l_shipmode",
				"l_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, INTSXP, INTSXP,
				REALSXP, REALSXP, REALSXP, STRSXP, STRSXP, STRSXP, STRSXP,
				STRSXP, STRSXP, STRSXP, STRSXP };
		// overestimate lineitem length for allocation, set true length below
		df_lineitem = PROTECT(
				create_df(16, tdefs[ORDER_LINE].base * 4.5, names_arr,
						types_arr));
	}
	/*
	 * traverse the tables, invoking the appropriate data generation routine for any to be built
	 */
	for (i = PART; i <= REGION; i++) {
		if (table & (1 << i)) {
			minrow = 1;
			if (i < NATION)
				rowcnt = tdefs[i].base * scale;
			else
				rowcnt = tdefs[i].base;

			gen_tbl((int) i, minrow, rowcnt, upd_num);

		}
	}

	// Special case, lineitem's length varies randomly
	for (int i = 0; i < 16; i++) {
		SEXP el = VECTOR_ELT(df_lineitem, i);
		SET_LENGTH(el, off_lineitem);
		SET_VECTOR_ELT(df_lineitem, i, el);
	}
	set_df_len(df_lineitem, off_lineitem);


	// 	SET_CLASS(varvalue, PROTECT(mkString("Date")));
	// TODO: option to not create comments?

	int ntab = 8;
	SEXP tables = PROTECT(NEW_LIST(ntab));
	SEXP names = PROTECT(NEW_CHARACTER(ntab));
	if (!tables || !names) {
		UNPROTECT(2);
		error("memmory allocation");
		return NULL;
	}
	char* names_arr[] = { "region", "nation", "supplier", "customer", "part",
			"partsupp", "orders", "lineitem" };
	for (int n = 0; n < ntab; n++) {
		SET_STRING_ELT(names, n, RSTR(names_arr[n]));
	}
	SET_VECTOR_ELT(tables, 0, df_region);
	SET_VECTOR_ELT(tables, 1, df_nation);
	SET_VECTOR_ELT(tables, 2, df_supp);
	SET_VECTOR_ELT(tables, 3, df_cust);
	SET_VECTOR_ELT(tables, 4, df_part);
	SET_VECTOR_ELT(tables, 5, df_psupp);
	SET_VECTOR_ELT(tables, 6, df_order);
	SET_VECTOR_ELT(tables, 7, df_lineitem);

	UNPROTECT(ntab);
	SET_NAMES(tables, names);

	UNPROTECT(2); // names, tables
	return tables;
}

// R native routine registration
#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}
static const R_CallMethodDef R_CallDef[] = {
CALLDEF(dbgen_R, 1), { NULL, NULL, 0 } };

void R_init_tpchr(DllInfo *dll) {
	R_registerRoutines(dll, NULL, R_CallDef, NULL, NULL);
	R_useDynamicSymbols(dll, FALSE);
}
