#include <Rdefines.h>
#include <time.h>

#define DECLARER				/* EXTERN references get defined here */

#include "dss.h"
#include "dsstypes.h"


#define RSTR(somestr) mkCharCE(somestr, CE_UTF8)

#define APPEND_INTSXP(val) INTEGER_POINTER(VECTOR_ELT(as, col++))[of] = (int) val;
#define APPEND_NUMSXP(val) NUMERIC_POINTER(VECTOR_ELT(as, col++))[of] = (double) val;
#define APPEND_STRSXP(val) { \
	SEXP s = RSTR(val); \
	if (!s) return -1; \
	SET_STRING_ELT( VECTOR_ELT(as, col++), of, s); \
}

static SEXP df_region = NULL;
static size_t off_region = 0;

// borrowed from MonetDB
#define YEAR_MAX		5867411
#define YEAR_MIN		(-YEAR_MAX)
#define LEAPYEAR(y)		((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))
#define DATE(d,m,y)		((m) > 0 && (m) <= 12 && (d) > 0 && (y) != 0 && (y) >= YEAR_MIN && (y) <= YEAR_MAX && (d) <= MONTHDAYS(m, y))
#define MONTHDAYS(m,y)	((m) != 2 ? LEAPDAYS[m] : LEAPYEAR(y) ? 29 : 28)

static int LEAPDAYS[13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static int CUMDAYS[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304,
		334, 365 };

static int leapyears(int year) {
	/* count the 4-fold years that passed since jan-1-0 */
	int y4 = year / 4;

	/* count the 100-fold years */
	int y100 = year / 100;

	/* count the 400-fold years */
	int y400 = year / 400;

	return y4 + y400 - y100 + (year >= 0); /* may be negative */
}

static int todate(int day, int month, int year) {
	int n = 0;

	if (DATE(day, month, year)) {
		if (year < 0)
			year++; /* HACK: hide year 0 */
		n = (int) (day - 1);
		if (month > 2 && LEAPYEAR(year))
			n++;
		n += CUMDAYS[month - 1];
		/* current year does not count as leapyear */
		n += 365 * year + leapyears(year >= 0 ? year - 1 : year);
	}
	return n;
}

// end of borrowing

// 2017-05-26
static int date_to_int(char* datestr) {
	datestr[4] = 0;
	datestr[7] = 0;
	return todate(atoi(datestr + 8), atoi(datestr + 5), atoi(datestr)) - 719528;
}

static char lean = 0;

static int append_region(code_t *c, int mode) {
	(void) mode;
	int col = 0;
	SEXP as = df_region;
	size_t of = off_region;

	APPEND_INTSXP(c->code);
	APPEND_STRSXP(c->text);
	APPEND_STRSXP(c->comment);

	off_region++;
	return 0;
}

static SEXP df_nation = NULL;
static size_t off_nation = 0;

static int append_nation(code_t *c, int mode) {
	(void) mode;

	int col = 0;
	SEXP as = df_nation;
	size_t of = off_nation;

	APPEND_INTSXP(c->code);
	APPEND_STRSXP(c->text);
	APPEND_INTSXP(c->join);
	APPEND_STRSXP(c->comment);

	off_nation++;
	return 0;
}

static SEXP df_part = NULL;
static size_t off_part = 0;

static int append_part(part_t *part, int mode) {
	(void) mode;

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
	if (!lean) {
		APPEND_NUMSXP((double)part->retailprice/100);
		APPEND_STRSXP(part->comment);
	}
	off_part++;
	return 0;
}

static SEXP df_psupp = NULL;
static size_t off_psupp = 0;

static int append_psupp(part_t *part, int mode) {
	(void) mode;

	SEXP as = df_psupp;

	for (size_t i = 0; i < SUPP_PER_PART; i++) {
		size_t of = off_psupp;
		int col = 0;

		APPEND_INTSXP(part->s[i].partkey);
		APPEND_INTSXP(part->s[i].suppkey);
		APPEND_INTSXP(part->s[i].qty);
		APPEND_NUMSXP((double)part->s[i].scost/100);
		if (!lean) {
			APPEND_STRSXP(part->s[i].comment);
		}
		off_psupp++;
	}
	return 0;
}

static int append_part_psupp(part_t *part, int mode) {
	(void) mode;

	if (append_part(part, mode) != 0) {
		return -1;
	}
	if (append_psupp(part, mode) != 0) {
		return -1;
	}
	return 0;
}

static SEXP df_order = NULL;
static size_t off_order = 0;

static int append_order(order_t *o, int mode) {
	(void) mode;

	int col = 0;
	SEXP as = df_order;
	size_t of = off_order;

	APPEND_INTSXP(o->okey);
	APPEND_INTSXP(o->custkey)
	char str[2] = "X";
	str[0] = o->orderstatus;
	APPEND_STRSXP(str);
	APPEND_NUMSXP((double)o->totalprice/100);
	APPEND_INTSXP(date_to_int(o->odate));
	APPEND_STRSXP(o->opriority);
	if (!lean) {
		APPEND_STRSXP(o->clerk);
	}
	APPEND_INTSXP(o->spriority);
	APPEND_STRSXP(o->comment);

	off_order++;
	return 0;
}

static SEXP df_lineitem = NULL;
static size_t off_lineitem = 0;

static int append_line(order_t *o, int mode) {
	(void) mode;

	SEXP as = df_lineitem;

	for (DSS_HUGE i = 0; i < o->lines; i++) {
		int col = 0;
		size_t of = off_lineitem;

		APPEND_INTSXP(o->l[i].okey);
		APPEND_INTSXP(o->l[i].partkey);
		APPEND_INTSXP(o->l[i].suppkey);
		if (!lean) {
			APPEND_INTSXP(o->l[i].lcnt);
		}
		APPEND_INTSXP(o->l[i].quantity);
		APPEND_NUMSXP((double)o->l[i].eprice/100);
		APPEND_NUMSXP((double)o->l[i].discount/100);
		APPEND_NUMSXP((double)o->l[i].tax/100);
		char str[2] = "X";
		str[0] = o->l[i].rflag[0];
		APPEND_STRSXP(str);
		str[0] = o->l[i].lstatus[0];
		APPEND_STRSXP(str);
		APPEND_INTSXP(date_to_int(o->l[i].sdate));
		APPEND_INTSXP(date_to_int(o->l[i].cdate));
		APPEND_INTSXP(date_to_int(o->l[i].rdate));

		APPEND_STRSXP(o->l[i].shipinstruct);
		APPEND_STRSXP(o->l[i].shipmode);
		if (!lean) {
			APPEND_STRSXP(o->l[i].comment);
		}
		off_lineitem++;
	}
	return 0;
}

static int append_order_line(order_t *o, int mode) {
	(void) mode;

	if (append_order(o, mode) != 0) {
		return -1;
	}
	if (append_line(o, mode) != 0) {
		return -1;
	}
	return 0;
}

static SEXP df_cust = NULL;
static size_t off_cust = 0;

static int append_cust(customer_t *c, int mode) {
	(void) mode;

	int col = 0;
	SEXP as = df_cust;
	size_t of = off_cust;

	APPEND_INTSXP(c->custkey);
	APPEND_STRSXP(c->name);
	APPEND_STRSXP(c->address);
	APPEND_INTSXP(c->nation_code);
	APPEND_STRSXP(c->phone);
	APPEND_NUMSXP((double)c->acctbal/100);
	APPEND_STRSXP(c->mktsegment);
	APPEND_STRSXP(c->comment);

	off_cust++;
	return 0;
}

static SEXP df_supp = NULL;
static size_t off_supp = 0;

static int append_supp(supplier_t *supp, int mode) {
	(void) mode;
	int col = 0;
	SEXP as = df_supp;
	size_t of = off_supp;

	APPEND_INTSXP(supp->suppkey);
	APPEND_STRSXP(supp->name);
	APPEND_STRSXP(supp->address);
	APPEND_INTSXP(supp->nation_code);
	APPEND_STRSXP(supp->phone);
	APPEND_NUMSXP((double)supp->acctbal/100);
	APPEND_STRSXP(supp->comment);

	off_supp++;
	return 0;

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

// need for consistent re-invocations, gets set in R_init_tpchr
extern seed_t Seed[];
seed_t Seed_backup[MAX_STREAM + 1];


static int
gen_tbl_R (int tnum, DSS_HUGE count)
{
	order_t o;
	supplier_t supp;
	customer_t cust;
	part_t part;
	code_t code;

	for (DSS_HUGE i = 1; count; count--, i++) {
		row_start(tnum);
		switch (tnum)
		{
		case LINE:
		case ORDER:
  		case ORDER_LINE:
			mk_order (i, &o, 0);
			if (append_order_line(&o, 0) != 0) {
				return -1;
			}
			break;
		case SUPP:
			mk_supp (i, &supp);
			if (append_supp(&supp, 0) != 0) {
				return -1;
			}
			break;
		case CUST:
			mk_cust (i, &cust);
			if (append_cust(&cust, 0) != 0) {
				return -1;
			}
			break;
		case PSUPP:
		case PART:
  		case PART_PSUPP:
			mk_part (i, &part);
			if (append_part_psupp(&part, 0) != 0) {
				return -1;
			}
			break;
		case NATION:
			mk_nation (i, &code);
			if (append_nation(&code, 0) != 0) {
				return -1;
			}
			break;
		case REGION:
			mk_region (i, &code);
			if (append_region(&code, 0) != 0) {
				return -1;
			}
			break;
		}
		row_stop(tnum);

	}
	return 0;
}

/*
* read the distributions needed in the benchamrk
*/
static void
load_dists (void)
{
	read_dist (env_config (DIST_TAG, DIST_DFLT), "p_cntr", &p_cntr_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "colors", &colors);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "p_types", &p_types_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "nations", &nations);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "regions", &regions);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "o_oprio",
		&o_priority_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "instruct",
		&l_instruct_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "smode", &l_smode_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "category",
		&l_category_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "rflag", &l_rflag_set);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "msegmnt", &c_mseg_set);

	/* load the distributions that contain text generation */
	read_dist (env_config (DIST_TAG, DIST_DFLT), "nouns", &nouns);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "verbs", &verbs);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "adjectives", &adjectives);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "adverbs", &adverbs);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "auxillaries", &auxillaries);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "terminators", &terminators);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "articles", &articles);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "prepositions", &prepositions);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "grammar", &grammar);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "np", &np);
	read_dist (env_config (DIST_TAG, DIST_DFLT), "vp", &vp);

}


tdef tdefs[] =
{
	{"part.tbl", "part table", 200000,
		NULL, NULL, PSUPP, 0},
	{"partsupp.tbl", "partsupplier table", 200000,
		NULL, NULL, NONE, 0},
	{"supplier.tbl", "suppliers table", 10000,
		NULL, NULL, NONE, 0},
	{"customer.tbl", "customers table", 150000,
		NULL, NULL, NONE, 0},
	{"orders.tbl", "order table", 150000,
		NULL, NULL, LINE, 0},
	{"lineitem.tbl", "lineitem table", 150000,
		NULL, NULL, NONE, 0},
	{"orders.tbl", "orders/lineitem tables", 150000,
		NULL, NULL, LINE, 0},
	{"part.tbl", "part/partsupplier tables", 200000,
		NULL, NULL, PSUPP, 0},
	{"nation.tbl", "nation table", NATIONS_MAX,
		NULL, NULL, NONE, 0},
	{"region.tbl", "region table", NATIONS_MAX,
		NULL, NULL, NONE, 0},
};



// 'inspired' by driver.c::main(). also, what a mess
static SEXP dbgen_R(SEXP sf, SEXP leansxp) {
	DSS_HUGE rowcnt = 0;
	double flt_scale;
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

	// restore random seeds from backup
	memcpy(Seed, Seed_backup, sizeof(seed_t) * MAX_STREAM + 1);

	tdefs[PART].base = 200000;
	tdefs[PSUPP].base = 200000;
	tdefs[SUPP].base = 10000;
	tdefs[CUST].base = 150000;
	tdefs[ORDER].base = 150000  * ORDERS_PER_CUST;
	tdefs[LINE].base = 150000 * ORDERS_PER_CUST;
	tdefs[ORDER_LINE].base = 150000 * ORDERS_PER_CUST;
	tdefs[PART_PSUPP].base = 200000;
	tdefs[NATION].base = NATIONS_MAX;
	tdefs[REGION].base = NATIONS_MAX;

	children = 1;
	d_path = NULL;
	lean = 0;

	flt_scale = NUMERIC_POINTER(sf)[0];
	lean = LOGICAL_POINTER(leansxp)[0];

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

	off_cust = 0;
	off_lineitem = 0;
	off_nation = 0;
	off_order = 0;
	off_part = 0;
	off_psupp = 0;
	off_region = 0;
	off_supp = 0;

	SEXP dateClass = PROTECT(mkString("Date"));
	if (!dateClass) {
		error("memory allocation");
		return NULL;
	}

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
	if (!lean) {
		char* names_arr[] =
				{ "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type",
						"p_size", "p_container", "p_retailprice", "p_comment" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP, STRSXP, STRSXP, INTSXP,
		STRSXP, REALSXP, STRSXP };
		df_part =
				PROTECT(
						create_df(9, tdefs[PART_PSUPP].base * scale, names_arr, types_arr));
	} else {
		char* names_arr[] = { "p_partkey", "p_name", "p_mfgr", "p_brand",
				"p_type", "p_size", "p_container" };
		SEXPTYPE types_arr[] = { INTSXP, STRSXP, STRSXP, STRSXP, STRSXP, INTSXP,
		STRSXP };
		df_part =
				PROTECT(
						create_df(7, tdefs[PART_PSUPP].base * scale, names_arr, types_arr));
	}
	if (!lean) {
		char* names_arr[] = { "ps_partkey", "ps_suppkey", "ps_availqty",
				"ps_supplycost", "ps_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, REALSXP, STRSXP };
		df_psupp =
				PROTECT(
						create_df(5, tdefs[PART_PSUPP].base * scale * SUPP_PER_PART, names_arr, types_arr));
	} else {
		char* names_arr[] = { "ps_partkey", "ps_suppkey", "ps_availqty",
				"ps_supplycost" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, REALSXP };
		df_psupp =
				PROTECT(
						create_df(4, tdefs[PART_PSUPP].base * scale * SUPP_PER_PART, names_arr, types_arr));
	}
	if (!lean) {
		char* names_arr[] = { "o_orderkey", "o_custkey", "o_orderstatus",
				"o_totalprice", "o_orderdate", "o_orderpriority", "o_clerk",
				"o_shippriority", "o_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, STRSXP, REALSXP, INTSXP,
		STRSXP, STRSXP, INTSXP, STRSXP };
		df_order =
				PROTECT(
						create_df(9, tdefs[ORDER_LINE].base * scale, names_arr, types_arr));
	} else {
		char* names_arr[] = { "o_orderkey", "o_custkey", "o_orderstatus",
				"o_totalprice", "o_orderdate", "o_orderpriority",
				"o_shippriority", "o_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, STRSXP, REALSXP, INTSXP,
		STRSXP, INTSXP, STRSXP };
		df_order =
				PROTECT(
						create_df(8, tdefs[ORDER_LINE].base * scale, names_arr, types_arr));
	}
	if (!lean) {
		char* names_arr[] = { "l_orderkey", "l_partkey", "l_suppkey",
				"l_linenumber", "l_quantity", "l_extendedprice", "l_discount",
				"l_tax", "l_returnflag", "l_linestatus", "l_shipdate",
				"l_commitdate", "l_receiptdate", "l_shipinstruct", "l_shipmode",
				"l_comment" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, INTSXP, INTSXP,
		REALSXP, REALSXP, REALSXP, STRSXP, STRSXP, INTSXP, INTSXP,
		INTSXP, STRSXP, STRSXP, STRSXP };
		// overestimate lineitem length for allocation, set true length below
		df_lineitem =
				PROTECT(
						create_df(16, tdefs[ORDER_LINE].base * scale * 4.5, names_arr, types_arr));
	} else {
		char* names_arr[] = { "l_orderkey", "l_partkey", "l_suppkey",
				"l_quantity", "l_extendedprice", "l_discount", "l_tax",
				"l_returnflag", "l_linestatus", "l_shipdate", "l_commitdate",
				"l_receiptdate", "l_shipinstruct", "l_shipmode" };
		SEXPTYPE types_arr[] = { INTSXP, INTSXP, INTSXP, INTSXP,
		REALSXP, REALSXP, REALSXP, STRSXP, STRSXP, INTSXP, INTSXP,
		INTSXP, STRSXP, STRSXP };
		// overestimate lineitem length for allocation, set true length below
		df_lineitem =
				PROTECT(
						create_df(14, tdefs[ORDER_LINE].base * scale * 4.5, names_arr, types_arr));

	}
	/*
	 * traverse the tables, invoking the appropriate data generation routine for any to be built
	 */
	for (i = PART; i <= REGION; i++) {
		if (table & (1 << i)) {
			if (i < NATION)
				rowcnt = tdefs[i].base * scale;
			else
				rowcnt = tdefs[i].base;
			// actually doing something
			if (gen_tbl_R((int) i,  rowcnt) != 0) {
				error("memory allocation");
			}
		}
	}

	// Special case, lineitem's length varies randomly
	for (int i = 0; i < LENGTH(df_lineitem); i++) {
		SEXP el = VECTOR_ELT(df_lineitem, i);
		SET_LENGTH(el, off_lineitem);
		SET_VECTOR_ELT(df_lineitem, i, el);
	}
	set_df_len(df_lineitem, off_lineitem);


	// set date class manually for order/lineitem cols
	SET_CLASS(VECTOR_ELT(df_order, 4), dateClass);

	if (!lean) {
		SET_CLASS(VECTOR_ELT(df_lineitem, 10), dateClass);
		SET_CLASS(VECTOR_ELT(df_lineitem, 11), dateClass);
		SET_CLASS(VECTOR_ELT(df_lineitem, 12), dateClass);
	}
	else {
		SET_CLASS(VECTOR_ELT(df_lineitem, 9), dateClass);
		SET_CLASS(VECTOR_ELT(df_lineitem, 10), dateClass);
		SET_CLASS(VECTOR_ELT(df_lineitem, 11), dateClass);
	}

	int ntab = 8;
	SEXP tables = PROTECT(NEW_LIST(ntab));
	SEXP names = PROTECT(NEW_CHARACTER(ntab));
	if (!tables || !names) {
		UNPROTECT(2);
		error("memory allocation");
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

	UNPROTECT(3); // names, tables, dateClass
	return tables;
}

// R native routine registration
#define CALLDEF(name, n)  {#name, (DL_FUNC) &name, n}
static const R_CallMethodDef R_CallDef[] = {
CALLDEF(dbgen_R, 2), { NULL, NULL, 0 } };

void R_init_tpchr(DllInfo *dll) {
	memcpy(Seed_backup, Seed, sizeof(seed_t) * MAX_STREAM + 1);
	R_registerRoutines(dll, NULL, R_CallDef, NULL, NULL);
	R_useDynamicSymbols(dll, FALSE);
}
