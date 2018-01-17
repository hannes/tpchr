# TPC-H for R

[![Build Status](https://travis-ci.org/hannesmuehleisen/tpchr.svg?branch=master)](https://travis-ci.org/hannesmuehleisen/tpchr)

`tpchr` wraps the Transaction Processing Council's [Decision Support Benchmark](http://www.tpc.org/tpch/) (TPC-H). The benchmark consists of a data generator, 22 queries and reference answers for those queries. The benchmark has been [highly influential](http://oai.cwi.nl/oai/asset/21424/21424B.pdf) in analytical data management research. 

This package wraps the `dbgen` data generator from TPC-H, modified so it produces R `data.frames` instead of flat files, functions to read and compare results with the reference answers and some `dplyr` translations of the SQL benchmark queries. 

## Installation

* the latest version from github on the command line

    ```R
    devtools::install_github("hannesmuehleisen/tpchr")
    ```

If you encounter a bug, please file a minimal reproducible example on [github](https://github.com/hannesmuehleisen/tpchr/issues). 


## Data Generation
The data generator creates 8 tables, their size can be dependent on a "scale factor" SF.: 
* `region` (25 rows)
* `nation` (25 rows)
* `supplier` (SF * 10,000 rows)
* `customer` (SF * 150,000 rows)
* `part` (SF * 200,000 rows)
* `partsupp` (SF * 800,000 rows)
* `orders` (SF * 1,500,000 rows)
* `lineitem` (~ SF * 6,000,000 rows)

To use the data generator, call the `dbgen` function with the desired scale factor:
```R
tbls <- tpchr::dbgen(0.001)
```

`tbls` is now a named list of `data.frames`, each containing one of the tables. To write those into a database with connection `con`, you could use

````R
lapply(names(tbls), function(n) {dbWriteTable(con, n, tbls[[n]])})

````

## Queries
See the [benchmark specification](http://www.tpc.org/tpc_documents_current_versions/pdf/tpc-h_v2.17.3.pdf) for the full set of queries, but here are SQL and dplyr examples for Query 3:

````SQL
select
	l_orderkey,
	sum(l_extendedprice * (1 - l_discount)) as revenue,
	o_orderdate,
	o_shippriority
from
	customer,
	orders,
	lineitem
where
	c_mktsegment = 'BUILDING'
	and c_custkey = o_custkey
	and l_orderkey = o_orderkey
	and o_orderdate < date '1995-03-15'
	and l_shipdate > date '1995-03-15'
group by
	l_orderkey,
	o_orderdate,
	o_shippriority
order by
	revenue desc,
	o_orderdate
limit 10;
````

````R
tbl(s, "customer") %>% filter(c_mktsegment == "BUILDING") %>% 
	inner_join(tbl(s, "orders") %>% filter(o_orderdate < as.Date('1995-03-15')), by=c("c_custkey" = "o_custkey")) %>% 
	inner_join(tbl(s, "lineitem") %>% filter(l_shipdate > as.Date('1995-03-15')), by=c("o_orderkey" = "l_orderkey")) %>% 
	group_by(o_orderkey, o_orderdate, o_shippriority) %>% 
	summarise(revenue=sum(l_extendedprice * (1 - l_discount))) %>% 
	select(o_orderkey, revenue, o_orderdate, o_shippriority) %>% 
	arrange(desc(revenue), o_orderdate) %>% head(10)
````

This query is expected to produce the following result using a scale factor of 1:

|l_orderkey|revenue  |o_orderdat|o_shippriority  |
| -------- | ------- | -------- | -------------- |
|   2456423|406181.01|1995-03-05|              0 |
|   3459808|405838.70|1995-03-04|              0 |
|    492164|390324.06|1995-02-19|              0 |
|   1188320|384537.94|1995-03-09|              0 |
|   2435712|378673.06|1995-02-26|              0 |
|   4878020|378376.80|1995-03-12|              0 |
|   5521732|375153.92|1995-03-13|              0 |
|   2628192|373133.31|1995-02-22|              0 |
|    993600|371407.46|1995-03-05|              0 |
|   2300070|367371.15|1995-03-13|              0 |


To check this query automatically using your `dplyr` source `s`, you can run
````R
tpchr::test_dplyr(s, 3)
````

