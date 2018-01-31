library(data.table)

test_dt_q <- list()

test_dt_q[[1]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

	lineitem[
	l_shipdate <= "1998-09-02", .(
	sum_qty        = sum(l_quantity), 
    sum_base_price = sum(l_extendedprice), 
    sum_disc_price = sum(l_extendedprice * (1 - l_discount)), 
    sum_charge     = sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)), 
    avg_qty        = mean(l_quantity), 
    avg_price      = mean(l_extendedprice), 
    avg_disc       = mean(l_discount), 
    count_order    = .N), 
    keyby=.(l_returnflag, l_linestatus)]
}

test_dt_q[[2]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

    ps <- partsupp[, .(ps_partkey, ps_suppkey, ps_supplycost)]
    p <- part[p_size == 15 & grepl(".*BRASS$", p_type), .(p_partkey, p_mfgr)]
    psp <- merge(ps, p, by.x="ps_partkey", by.y="p_partkey")
    sp <- supplier[, .(s_suppkey, s_nationkey, s_acctbal, s_name, s_address, s_phone, s_comment)]
    psps <- merge(psp, sp, by.x="ps_suppkey", by.y="s_suppkey")[, .(ps_partkey,  ps_supplycost, p_mfgr, s_nationkey, s_acctbal, s_name, s_address, s_phone, s_comment)]
    nr <- merge(nation, region, by.x="n_regionkey", by.y="r_regionkey")[r_name=="EUROPE", .(n_nationkey, n_name)]
    pspsnr <- merge(psps, nr, by.x="s_nationkey", by.y="n_nationkey")[, .(ps_partkey,  ps_supplycost, p_mfgr, n_name, s_acctbal, s_name, s_address, s_phone, s_comment)]
    aggr <- pspsnr[, .(min_ps_supplycost = min(ps_supplycost)), by="ps_partkey"]
    sj <- merge(pspsnr, aggr, by.x=c("ps_partkey", "ps_supplycost"), by.y=c("ps_partkey", "min_ps_supplycost"))
    res <- sj[head(order(-rank(s_acctbal), n_name, s_name, ps_partkey), 100), .(s_acctbal, s_name, n_name, ps_partkey, p_mfgr, s_address, s_phone, s_comment)]
    res
}

test_dt_q[[3]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

    o <- orders[o_orderdate < "1995-03-15", .(o_orderkey, o_custkey, o_orderdate, o_shippriority)]
    c <- customer[c_mktsegment == "BUILDING", .(c_custkey)]
    oc <- merge(o, c, by.x="o_custkey", by.y="c_custkey")[, .(o_orderkey, o_orderdate, o_shippriority)]
    l <- lineitem[l_shipdate > "1995-03-15", .(l_orderkey, l_extendedprice, l_discount)]
    loc <- merge(l, oc, by.x="l_orderkey", by.y="o_orderkey")
    aggr <- loc[,.(revenue=sum(l_extendedprice * (1 - l_discount))), by=.(l_orderkey, o_orderdate, o_shippriority)][order(-rank(revenue), o_orderdate)[1:10], .(l_orderkey, revenue, o_orderdate, o_shippriority)]
    aggr
}

test_dt_q[[4]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])
    
    l <- lineitem[l_commitdate < l_receiptdate, .(l_orderkey)]
    o <- orders[o_orderdate >= "1993-07-01" & o_orderdate < "1993-10-01", .(o_orderkey, o_orderpriority)]
    lo <- unique(merge(l, o, by.x="l_orderkey", by.y="o_orderkey"))

	aggr <- lo[order(o_orderpriority),.(order_count=.N), by=.(o_orderpriority)]
    aggr
}

test_dt_q[[5]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

    # nation & region are very small, thus no pre-projection or early filter
    nr <- merge(nation, region, by.x="n_regionkey", by.y="r_regionkey")[r_name=="ASIA", .(n_nationkey, n_name)]
    snr <- merge(supplier[, .(s_suppkey, s_nationkey)], nr, by.x="s_nationkey", by.y="n_nationkey")[, .(s_suppkey, s_nationkey, n_name)]
    lsnr <- merge(lineitem[, .(l_suppkey, l_orderkey, l_extendedprice, l_discount)], snr, by.x="l_suppkey", by.y="s_suppkey")
    o <- orders[o_orderdate >= "1994-01-01" & o_orderdate < "1995-01-01", .(o_orderkey, o_custkey)]
    oc <- merge(o, customer[, .(c_custkey, c_nationkey)], by.x="o_custkey", by.y="c_custkey")[, .(o_orderkey, c_nationkey)]
    lsnroc <- merge(lsnr, oc, by.x=c("l_orderkey", "s_nationkey"), by.y=c("o_orderkey", "c_nationkey"))
    aggr <- lsnroc[, .(revenue = sum(l_extendedprice * (1 - l_discount))), by="n_name"][order(-rank(revenue)), ]
    aggr
}

test_dt_q[[6]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

	lineitem[l_shipdate >= "1994-01-01" & l_shipdate < "1995-01-01" & l_discount >= 0.05 & l_discount <= 0.07 & l_quantity < 24, .(revenue = sum(l_extendedprice * l_discount))]
}

test_dt_q[[7]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

	cn <- merge(customer[, .(c_custkey, c_nationkey)], nation[n_name %in% c("FRANCE", "GERMANY"), .(n2_nationkey=n_nationkey, n2_name=n_name)], by.x="c_nationkey", by.y="n2_nationkey")[, .(c_custkey, n2_name)]
    cno <- merge(orders[, .(o_custkey, o_orderkey)], cn, by.x="o_custkey", by.y="c_custkey")[, .(o_orderkey, n2_name)]
    cnol <- merge(lineitem[l_shipdate >= "1995-01-01" & l_shipdate <= "1996-12-31", .(l_orderkey, l_suppkey, l_shipdate, l_extendedprice, l_discount)], cno, by.x="l_orderkey", by.y="o_orderkey")[, .(l_suppkey, l_shipdate, l_extendedprice, l_discount, n2_name)]
    sn <- merge(supplier[, .(s_nationkey, s_suppkey)], nation[n_name %in% c("FRANCE", "GERMANY"), .(n1_nationkey=n_nationkey, n1_name=n_name)], by.x="s_nationkey", by.y="n1_nationkey")[, .(s_suppkey, n1_name)]

    all <- merge(cnol, sn, by.x="l_suppkey", by.y="s_suppkey")
    aggr <- all[(n1_name=="FRANCE" & n2_name == "GERMANY") | (n1_name=="GERMANY" & n2_name == "FRANCE"), .(supp_nation=n1_name, cust_nation=n2_name, l_year=as.integer(format(l_shipdate, "%Y")), volume=l_extendedprice * (1 - l_discount))][, .(revenue=sum(volume)), by=.(supp_nation, cust_nation, l_year)][order(supp_nation, cust_nation, l_year), ]
    aggr
}



test_dt_q[[8]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

    nr <- merge(nation, region, by.x="n_regionkey", by.y="r_regionkey")[r_name == "AMERICA", .(n_nationkey)]
    cnr <- merge(customer[, .(c_custkey, c_nationkey)], nr, by.x="c_nationkey", by.y="n_nationkey")[, .(c_custkey)]
    o <- orders[o_orderdate >= "1995-01-01" & o_orderdate <= "1996-12-31", .(o_orderkey, o_custkey, o_orderdate)]
    ocnr <- merge(o, cnr, by.x="o_custkey", by.y="c_custkey")[, .(o_orderkey, o_orderdate)]
    locnr <- merge(lineitem[,.(l_orderkey, l_partkey, l_suppkey, l_extendedprice, l_discount)], ocnr, by.x="l_orderkey", by.y="o_orderkey")[, .(l_partkey, l_suppkey, l_extendedprice, l_discount, o_orderdate)]
    p <- part[p_type == "ECONOMY ANODIZED STEEL", .(p_partkey)]
    locnrp <- merge(locnr, p, by.x="l_partkey", by.y="p_partkey")[, .(l_suppkey, l_extendedprice, l_discount ,o_orderdate)]
    locnrps <- merge(locnrp, supplier[, .(s_suppkey, s_nationkey)], by.x="l_suppkey", by.y="s_suppkey")[, .(l_extendedprice, l_discount, o_orderdate, s_nationkey)]
    locnrpsn <- merge(locnrps, nation[, .(n_nationkey, n_name)], by.x="s_nationkey", by.y="n_nationkey")

    aggr <- locnrpsn[, .(o_year=as.integer(format(o_orderdate, "%Y")), volume=l_extendedprice * (1 - l_discount), nation=n_name)][order(o_year), .(mkt_share=sum(ifelse(nation=="BRAZIL", volume, 0))/sum(volume)), by=.(o_year)]
    aggr
}

test_dt_q[[9]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])

    p <- part[grepl(".*green.*", p_name), .(p_partkey)]
    psp <- merge(partsupp[, .(ps_suppkey, ps_partkey, ps_supplycost)], p, by.x="ps_partkey", by.y="p_partkey")
    sn <- merge(supplier[, .(s_suppkey, s_nationkey)], nation[, .(n_nationkey, n_name)], by.x="s_nationkey", by.y="n_nationkey")[, .(s_suppkey, n_name)]
    pspsn <- merge(psp, sn, by.x="ps_suppkey", by.y="s_suppkey")
    lpspsn <- merge(lineitem[, .(l_suppkey, l_partkey, l_orderkey, l_extendedprice, l_discount, l_quantity)], pspsn, by.x=c("l_partkey", "l_suppkey"), by.y=c("ps_partkey", "ps_suppkey"))[, .(l_orderkey, l_extendedprice ,l_discount, l_quantity, ps_supplycost, n_name)]
    lpspsno <- merge(lpspsn, orders[, .(o_orderkey, o_orderdate)], by.x="l_orderkey", by.y="o_orderkey")
    aggr <- lpspsno[, .(nation=n_name, o_year = as.integer(format(o_orderdate, "%Y")), amount = l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity)][order(nation, -rank(o_year)), .(sum_profit=sum(amount)), by=.(nation, o_year)]
    aggr
}



test_dt_q[[10]] <- function(s) {
    for (n in names(s)) assign(n, s[[n]])
    
    l <- lineitem[l_returnflag == "R", .(l_orderkey, l_extendedprice, l_discount)]
    o <- orders[o_orderdate >= "1993-10-01" & o_orderdate < "1994-01-01", .(o_orderkey, o_custkey)]
    lo_aggr <- merge(l, o, by.x="l_orderkey", by.y="o_orderkey")[, .(revenue=sum(l_extendedprice * (1 - l_discount))), by=o_custkey]
    c <- customer[, .(c_custkey, c_nationkey, c_name, c_acctbal, c_phone, c_address, c_comment)]
    loc <- merge(lo_aggr, c, by.x="o_custkey", by.y="c_custkey")
    locn <- merge(loc, nation[, .(n_nationkey, n_name)], by.x="c_nationkey", by.y="n_nationkey")[order(-rank(revenue))[1:20], .(o_custkey, c_name, revenue, c_acctbal, n_name, c_address, c_phone, c_comment)]
}

test_dt <- function(src, q=1, sf=0.1) {
    data_comparable(test_dt_q[[q]](src), get_answer(q, sf))
}
