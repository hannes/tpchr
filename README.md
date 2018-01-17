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
See the [benchmark specification](http://www.tpc.org/tpc_documents_current_versions/pdf/tpc-h_v2.17.3.pdf) for the full set of queries, but here are SQL and dplyr examples for Query 1:

````SQL
select
	l_returnflag,
	l_linestatus,
	sum(l_quantity) as sum_qty,
	sum(l_extendedprice) as sum_base_price,
	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
	avg(l_quantity) as avg_qty,
	avg(l_extendedprice) as avg_price,
	avg(l_discount) as avg_disc,
	count(*) as count_order
from
	lineitem
where
	l_shipdate <= '1998-09-02'
group by
	l_returnflag,
	l_linestatus
order by
	l_returnflag,
	l_linestatus;
````

````R
tbl(s, "lineitem") %>% 
	filter(l_shipdate <= as.Date('1998-12-01') - 90) %>% 
	group_by(l_returnflag, l_linestatus) %>% 
	summarise(
		sum_qty=sum(l_quantity), 
		sum_base_price=sum(l_extendedprice), 
		sum_disc_price=sum(l_extendedprice * (1 - l_discount)), 
		sum_charge=sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)), 
		avg_qty=mean(l_quantity), 
		avg_price=mean(l_extendedprice), 
		avg_disc=mean(l_discount), count_order=n()) %>% 
	arrange(l_returnflag, l_linestatus)
````

This query is expected to produce the following result using a scale factor of 1:

||||||| | |||       
|-|-|-------|--------------|--------------|----------|--------|---------|--------|-----------|    
|A|F|37734107.00|56586554400.73|53758257134.87|55909065222.83|25.52|38273.13|0.05|    1478493 |
|N|F|991417.00|1487504710.38|1413082168.05|1469649223.19|25.52|38284.47|0.05|           38854 |
|N|O|74476040.00|111701729697.74|106118230307.61|110367043872.50|25.50|38249.12|0.05| 2920374 |
|R|F|37719753.00|56568041380.90|53741292684.60|55889619119.83|25.51|38250.85|0.05|    1478870 |



To check this query automatically using your `dplyr` source `s`, you can run
````R
tpchr::test_dplyr(s, 1)
````

To check this query automatically using a DBI connection `c`, you can run
````R
tpchr::test_dbi(c, 1)
````
