/* main driver for dss banchmark */

#define DECLARER				/* EXTERN references get defined here */
#define NO_FUNC (int (*) ()) NULL	/* to clean up tdefs */
#define NO_LFUNC (long (*) ()) NULL		/* to clean up tdefs */

#include "dss.h"
#include "dsstypes.h"

/*
* seed generation functions; used with '-O s' option
*/
long sd_cust (int child, DSS_HUGE skip_count);
long sd_line (int child, DSS_HUGE skip_count);
long sd_order (int child, DSS_HUGE skip_count);
long sd_part (int child, DSS_HUGE skip_count);
long sd_psupp (int child, DSS_HUGE skip_count);
long sd_supp (int child, DSS_HUGE skip_count);
long sd_order_line (int child, DSS_HUGE skip_count);
long sd_part_psupp (int child, DSS_HUGE skip_count);

tdef tdefs[] =
{
	{"part.tbl", "part table", 200000,
		NULL, sd_part, PSUPP, 0},
	{"partsupp.tbl", "partsupplier table", 200000,
		NULL, sd_psupp, NONE, 0},
	{"supplier.tbl", "suppliers table", 10000,
		NULL, sd_supp, NONE, 0},
	{"customer.tbl", "customers table", 150000,
		NULL, sd_cust, NONE, 0},
	{"orders.tbl", "order table", 150000,
		NULL, sd_order, LINE, 0},
	{"lineitem.tbl", "lineitem table", 150000,
		NULL, sd_line, NONE, 0},
	{"orders.tbl", "orders/lineitem tables", 150000,
		NULL, sd_order, LINE, 0},
	{"part.tbl", "part/partsupplier tables", 200000,
		NULL, sd_part, PSUPP, 0},
	{"nation.tbl", "nation table", NATIONS_MAX,
		NULL, NO_LFUNC, NONE, 0},
	{"region.tbl", "region table", NATIONS_MAX,
		NULL, NO_LFUNC, NONE, 0},
};



/*
* read the distributions needed in the benchamrk
*/
void
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

/*
* generate a particular table
*/
void
gen_tbl (int tnum, DSS_HUGE start, DSS_HUGE count, long upd_num)
{
	static order_t o;
	supplier_t supp;
	customer_t cust;
	part_t part;
	code_t code;
	static int completed = 0;
	DSS_HUGE i;

	DSS_HUGE rows_per_segment=0;
	DSS_HUGE rows_this_segment=-1;
	DSS_HUGE residual_rows=0;

	if (insert_segments)
		{
		rows_per_segment = count / insert_segments;
		residual_rows = count - (rows_per_segment * insert_segments);
		}

	for (i = start; count; count--, i++)
	{
		LIFENOISE (1000, i);
		row_start(tnum);

		switch (tnum)
		{
		case LINE:
		case ORDER:
  		case ORDER_LINE: 
			mk_order (i, &o, upd_num % 10000);

		  if (insert_segments  && (upd_num > 0))
			{
			if((upd_num / 10000) < residual_rows)
				{
				if((++rows_this_segment) > rows_per_segment) 
					{						
					rows_this_segment=0;
					upd_num += 10000;					
					}
				}
			else
				{
				if((++rows_this_segment) >= rows_per_segment) 
					{
					rows_this_segment=0;
					upd_num += 10000;
					}
				}
			}


			if (set_seeds == 0)
				tdefs[tnum].loader(&o, upd_num);
			break;
		case SUPP:
			mk_supp (i, &supp);
			if (set_seeds == 0)
				tdefs[tnum].loader(&supp, upd_num);
			break;
		case CUST:
			mk_cust (i, &cust);
			if (set_seeds == 0)
				tdefs[tnum].loader(&cust, upd_num);
			break;
		case PSUPP:
		case PART:
  		case PART_PSUPP: 
			mk_part (i, &part);
			if (set_seeds == 0)
				tdefs[tnum].loader(&part, upd_num);
			break;
		case NATION:
			mk_nation (i, &code);
			if (set_seeds == 0)
				tdefs[tnum].loader(&code, 0);
			break;
		case REGION:
			mk_region (i, &code);
			if (set_seeds == 0)
				tdefs[tnum].loader(&code, 0);
			break;
		}
		row_stop(tnum);
		if (set_seeds && (i % tdefs[tnum].base) < 2)
		{
			printf("\nSeeds for %s at rowcount " HUGE_FORMAT "\n", tdefs[tnum].comment, i);
			dump_seeds(tnum);
		}
	}
	completed |= 1 << tnum;
}



