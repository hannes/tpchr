library(data.table)

test_dt_q <- list()

test_dt_q[[1]] <- function(s) {
	s[["lineitem"]][
	l_shipdate <= as.Date('1998-09-02'), .(
	sum_qty=sum(l_quantity), 
    sum_base_price=sum(l_extendedprice), 
    sum_disc_price=sum(l_extendedprice * (1 - l_discount)), 
    sum_charge=sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)), 
    avg_qty=mean(l_quantity), 
    avg_price=mean(l_extendedprice), 
    avg_disc=mean(l_discount), count_order=.N), 
    keyby=.(l_returnflag, l_linestatus)]
}

test_dt_q[[2]] <- function(s) {
	inner <- merge(merge(merge(s[["partsupp"]], s[["supplier"]], by.x="ps_suppkey", by.y="s_suppkey"), s[["nation"]], by.x="s_nationkey", by.y="n_nationkey"), s[["region"]][r_name=="EUROPE",], by.x="n_regionkey", by.y="r_regionkey")

	merge(merge(s[["part"]][p_size==15 & grepl(".*BRASS$", p_type),], inner, by.x="p_partkey", by.y="ps_partkey"), inner[, .(min_ps_supplycost = min(ps_supplycost)), by=ps_partkey], by.x=c("p_partkey", "ps_supplycost"), by.y=c("ps_partkey", "min_ps_supplycost"))[order(-rank(s_acctbal), n_name, s_name, p_partkey)[1:100], .(s_acctbal, s_name, n_name, p_partkey, p_mfgr, s_address, s_phone, s_comment)]
}


test_dt_q[[3]] <- function(s) {
	merge(merge(s[["customer"]][c_mktsegment == "BUILDING", ], s[["orders"]][o_orderdate < as.Date('1995-03-15'), ], by.x="c_custkey", by.y="o_custkey"), s[["lineitem"]][l_shipdate > as.Date('1995-03-15')], by.x="o_orderkey", by.y="l_orderkey")[,.(revenue=sum(l_extendedprice * (1 - l_discount))), by=.(o_orderkey, o_orderdate, o_shippriority)][order(-rank(revenue), o_orderdate)[1:10], .(o_orderkey, revenue, o_orderdate, o_shippriority)]
}


test_dt_q[[4]] <- function(s) {
	merge(s[["orders"]][o_orderdate >= as.Date('1993-07-01') & o_orderdate < as.Date('1993-10-01'), ], s[["lineitem"]][l_commitdate < l_receiptdate, .(dummy=1), by=.(l_orderkey)], by.x="o_orderkey", by.y="l_orderkey")[order(o_orderpriority),.(order_count=.N),by=.(o_orderpriority)]
}

test_dt_q[[5]] <- function(s) {
	merge(merge(merge(merge(merge(s[["customer"]], s[["orders"]][o_orderdate >= as.Date('1994-01-01') & o_orderdate < as.Date('1995-01-01'),], by.x="c_custkey", by.y="o_custkey"), s[["lineitem"]], by.x="o_orderkey", by.y="l_orderkey"), s[["supplier"]], by.x=c("l_suppkey", "c_nationkey"), by.y=c("s_suppkey", "s_nationkey")), s[["nation"]], by.x="c_nationkey", by.y="n_nationkey"), s[["region"]][r_name == "ASIA", ], by.x="n_regionkey", by.y="r_regionkey")[, .(revenue = sum(l_extendedprice * (1 - l_discount))), by="n_name"][order(-rank(revenue)), ]
}

test_dt_q[[6]] <- function(s) {
	s[["lineitem"]][l_shipdate >= as.Date('1994-01-01') & l_shipdate < as.Date('1995-01-01')& l_discount >= 0.05 & l_discount <= 0.07 & l_quantity < 24, .(revenue = sum(l_extendedprice * l_discount))]
}


test_dt_q[[7]] <- function(s) {
	merge(merge(merge(merge(merge(s[["supplier"]], s[["lineitem"]][l_shipdate >= as.Date('1995-01-01') & l_shipdate <= as.Date('1996-12-31')], by.x="s_suppkey", by.y="l_suppkey"), s[["orders"]], by.x="l_orderkey", by.y="o_orderkey"), s[["customer"]], by.x="o_custkey", by.y="c_custkey"), s[["nation"]][, .(n1_nationkey=n_nationkey, n1_name=n_name)], by.x="s_nationkey", by.y="n1_nationkey"), s[["nation"]][, .(n2_nationkey=n_nationkey, n2_name=n_name)], by.x="c_nationkey", by.y="n2_nationkey")[(n1_name=='FRANCE' & n2_name == 'GERMANY') | (n1_name=='GERMANY' & n2_name == 'FRANCE'), .(supp_nation=n1_name, cust_nation=n2_name, l_year=as.integer(format(l_shipdate, "%Y")), volume=l_extendedprice * (1 - l_discount))][, .(revenue=sum(volume)), by=.(supp_nation, cust_nation, l_year)][order(supp_nation, cust_nation, l_year), ]
}

test_dt_q[[8]] <- function(s) {
	merge(merge(merge(merge(merge(merge(merge(s[["part"]][p_type == "ECONOMY ANODIZED STEEL", ], s[["lineitem"]], by.x="p_partkey", by.y="l_partkey"), s[["supplier"]], by.x="l_suppkey", by.y="s_suppkey"), s[["orders"]][o_orderdate >= as.Date('1995-01-01') & o_orderdate <= as.Date('1996-12-31'), ], by.x="l_orderkey", by.y="o_orderkey"), s[["customer"]], by.x="o_custkey", by.y="c_custkey"), s[["nation"]][, .(n1_nationkey=n_nationkey, n1_regionkey=n_regionkey)], by.x="c_nationkey", by.y="n1_nationkey"), s[["region"]][r_name=="AMERICA", ], by.x="n1_regionkey", by.y="r_regionkey"), s[["nation"]][, .(n2_nationkey=n_nationkey, n2_name=n_name)], by.x="s_nationkey", by.y= "n2_nationkey")[, .(o_year=as.integer(format(o_orderdate, "%Y")), volume=l_extendedprice * (1 - l_discount), nation=n2_name)][order(o_year), .(mkt_share=sum(ifelse(nation=="BRAZIL", volume, 0))/sum(volume)), by=.(o_year)]
}

test_dt_q[[9]] <- function(s) {
	merge(merge(merge(merge(merge(s[["part"]][grepl(".*green.*", p_name), ], s[["lineitem"]], by.x="p_partkey", by.y="l_partkey"), s[["supplier"]], by.x="l_suppkey", by.y="s_suppkey"), s[["partsupp"]], by.x=c("l_suppkey", "p_partkey"), by.y=c("ps_suppkey", "ps_partkey")), s[["orders"]], by.x="l_orderkey", by.y="o_orderkey"), s[["nation"]], by.x="s_nationkey", by.y="n_nationkey")[, .(nation=n_name, o_year = as.integer(format(o_orderdate, "%Y")), amount = l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity)][order(nation, -rank(o_year)), .(sum_profit=sum(amount)), by=.(nation, o_year)]
}


test_dt_q[[10]] <- function(s) {
	merge(merge(merge(s[["customer"]], s[["orders"]][o_orderdate >= as.Date('1993-10-01') & o_orderdate < as.Date('1994-01-01'), ], by.x="c_custkey", by.y="o_custkey"), s[["lineitem"]][l_returnflag == "R", ], by.x="o_orderkey", by.y="l_orderkey"), s[["nation"]], by.x="c_nationkey", by.y="n_nationkey")[, .(revenue=sum(l_extendedprice * (1 - l_discount))) , by=.(c_custkey, c_name, c_acctbal, c_phone, n_name, c_address, c_comment)][order(-rank(revenue))[1:20], .(c_custkey, c_name, revenue, c_acctbal, n_name, c_address, c_phone, c_comment)]
}


test_dt <- function(src, q, ...) {
    data_comparable(test_dt_q[[q]](src), get_answer(q), ...)
}
