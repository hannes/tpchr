library(dplyr)

test_dplyr_q <- list()

test_dplyr_q[[1]] <- function(s) {
    tbl(s, "lineitem") %>% 
        filter(l_shipdate <= "1998-09-02") %>% 
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
}

test_dplyr_q[[2]] <- function(s) {
    # this is a manual optimization, this intermediate table is used twice.
    inner <- tbl(s, "partsupp") %>% 
                inner_join(tbl(s, "supplier"), by=c("ps_suppkey" = "s_suppkey")) %>% 
                inner_join(tbl(s, "nation"), by=c("s_nationkey" = "n_nationkey")) %>% 
                inner_join(tbl(s, "region") %>% filter(r_name=="EUROPE"), by=c("n_regionkey"="r_regionkey"))

    tbl(s, "part") %>% 
        filter(p_size == 15, grepl(".*BRASS$", p_type)) %>% 
        inner_join(inner, by=c("p_partkey" = "ps_partkey")) %>% 
        inner_join(inner %>% 
                group_by(ps_partkey) %>% 
                summarise(min_ps_supplycost = min(ps_supplycost)), 
                by=c("p_partkey" = "ps_partkey", "ps_supplycost" = "min_ps_supplycost")) %>% 
        select(s_acctbal, s_name, n_name, p_partkey, p_mfgr, s_address, s_phone, s_comment) %>%
        arrange(desc(s_acctbal), n_name, s_name, p_partkey) %>% head(100)
}

test_dplyr_q[[3]] <- function(s) {
    tbl(s, "customer") %>% filter(c_mktsegment == "BUILDING") %>% 
        inner_join(tbl(s, "orders") %>% filter(o_orderdate < "1995-03-15"), by=c("c_custkey" = "o_custkey")) %>% 
            inner_join(tbl(s, "lineitem") %>% filter(l_shipdate > "1995-03-15"), by=c("o_orderkey" = "l_orderkey")) %>% 
            group_by(o_orderkey, o_orderdate, o_shippriority) %>% 
            summarise(revenue=sum(l_extendedprice * (1 - l_discount))) %>% 
            select(o_orderkey, revenue, o_orderdate, o_shippriority) %>% 
            arrange(desc(revenue), o_orderdate) %>% head(10)
}

test_dplyr_q[[4]] <- function(s) {
    tbl(s, "orders") %>% 
        filter(o_orderdate >= "1993-07-01", o_orderdate < "1993-10-01") %>% 
        inner_join(tbl(s, "lineitem") %>% filter(l_commitdate < l_receiptdate) %>% 
                group_by(l_orderkey) %>% summarise(), by=c("o_orderkey" = "l_orderkey")) %>% 
        group_by(o_orderpriority) %>% 
        summarise(order_count=n()) %>% 
        arrange(o_orderpriority)
}

test_dplyr_q[[5]] <- function(s) {
    tbl(s, "customer") %>% 
        inner_join(tbl(s, "orders") %>% filter(o_orderdate >= "1994-01-01", o_orderdate < "1995-01-01"), by=c("c_custkey" = "o_custkey")) %>% 
        inner_join(tbl(s, "lineitem"), by=c("o_orderkey" = "l_orderkey")) %>% 
        inner_join(tbl(s, "supplier"), by=c("l_suppkey" = "s_suppkey", "c_nationkey" = "s_nationkey")) %>% 
        inner_join(tbl(s, "nation"), by=c("c_nationkey" = "n_nationkey")) %>% 
        inner_join(tbl(s, "region") %>% filter(r_name == "ASIA"), by=c("n_regionkey"="r_regionkey")) %>% 
        group_by(n_name) %>% 
        summarise(revenue = sum(l_extendedprice * (1 - l_discount))) %>% 
        arrange(desc(revenue))
}

test_dplyr_q[[6]] <- function(s) {
    tbl(s, "lineitem") %>% filter(l_shipdate >= "1994-01-01", l_shipdate < "1995-01-01", l_discount >= 0.05, l_discount <= 0.07, l_quantity < 24) %>% summarise(revenue = sum(l_extendedprice * l_discount))
}

test_dplyr_q[[7]] <- function(s) {
    tbl(s, "supplier") %>% 
        inner_join(tbl(s, "lineitem") %>% filter(l_shipdate >= "1995-01-01", l_shipdate <= "1996-12-31"), by=c("s_suppkey" = "l_suppkey")) %>% 
        inner_join(tbl(s, "orders"), by=c("l_orderkey" = "o_orderkey")) %>% 
        inner_join(tbl(s, "customer"), by=c("o_custkey" = "c_custkey")) %>% 
        inner_join(tbl(s, "nation") %>% select(n1_nationkey=n_nationkey, n1_name=n_name), by=c("s_nationkey"="n1_nationkey")) %>% 
        inner_join(tbl(s, "nation") %>% select(n2_nationkey=n_nationkey, n2_name=n_name), by=c("c_nationkey"="n2_nationkey")) %>% 
        filter((n1_name=="FRANCE" & n2_name == "GERMANY") | (n1_name=="GERMANY" & n2_name == "FRANCE")) %>% 
        mutate(supp_nation=n1_name, cust_nation=n2_name, l_year=as.integer(format(l_shipdate, "%Y")), volume=l_extendedprice * (1 - l_discount)) %>% 
        select(supp_nation, cust_nation, l_year, volume) %>% 
        group_by(supp_nation, cust_nation, l_year) %>% 
        summarise(revenue=sum(volume)) %>% 
        arrange(supp_nation, cust_nation, l_year)
}

test_dplyr_q[[8]] <- function(s) {
    tbl(s, "part") %>% 
        filter(p_type == "ECONOMY ANODIZED STEEL") %>% 
        inner_join(tbl(s, "lineitem"), by=c("p_partkey"="l_partkey")) %>% 
        inner_join(tbl(s, "supplier"), by=c("l_suppkey" = "s_suppkey")) %>% 
        inner_join(tbl(s, "orders") %>% filter(o_orderdate >= "1995-01-01", o_orderdate <= "1996-12-31"), by=c("l_orderkey"="o_orderkey")) %>% 
        inner_join(tbl(s, "customer"), by=c("o_custkey"="c_custkey")) %>% 
        inner_join(tbl(s, "nation") %>% select(n1_nationkey=n_nationkey, n1_regionkey=n_regionkey), by=c("c_nationkey"="n1_nationkey")) %>% 
        inner_join(tbl(s, "region") %>% filter(r_name=="AMERICA"), by=c("n1_regionkey"="r_regionkey")) %>% 
        inner_join(tbl(s, "nation") %>% select(n2_nationkey=n_nationkey, n2_name=n_name), by=c("s_nationkey" = "n2_nationkey")) %>% 
        mutate(o_year=as.integer(format(o_orderdate, "%Y")), volume=l_extendedprice * (1 - l_discount), nation=n2_name) %>%
        select(o_year, volume, nation) %>%
        group_by(o_year) %>%
        summarise(mkt_share=sum(ifelse(nation=="BRAZIL", volume, 0))/sum(volume)) %>%
        arrange(o_year)
}

test_dplyr_q[[9]] <- function(s) {
    tbl(s, "part") %>% 
        filter(grepl(".*green.*", p_name)) %>% 
        inner_join(tbl(s, "lineitem"), by=c("p_partkey"="l_partkey")) %>% 
        inner_join(tbl(s, "supplier"), by=c("l_suppkey"="s_suppkey")) %>% 
        inner_join(tbl(s, "partsupp"), by=c("l_suppkey" = "ps_suppkey", "p_partkey" = "ps_partkey")) %>% 
        inner_join(tbl(s, "orders"), by=c("l_orderkey"="o_orderkey")) %>% 
        inner_join(tbl(s, "nation"), by=c("s_nationkey"="n_nationkey")) %>% 
        mutate(nation=n_name, o_year = as.integer(format(o_orderdate, "%Y")), amount = l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity) %>%
        select(nation, o_year, amount) %>%
        group_by(nation, o_year) %>%
        summarise(sum_profit=sum(amount)) %>%
        arrange(nation, desc(o_year))
}

test_dplyr_q[[10]] <- function(s) {
    tbl(s, "customer") %>% 
        inner_join(tbl(s, "orders") %>% filter(o_orderdate >= "1993-10-01", o_orderdate < "1994-01-01"), by=c("c_custkey" = "o_custkey")) %>% 
        inner_join(tbl(s, "lineitem") %>% filter(l_returnflag == "R"), by=c("o_orderkey" = "l_orderkey")) %>% 
        inner_join(tbl(s, "nation"), by=c("c_nationkey" = "n_nationkey")) %>% 
        group_by(c_custkey, c_name, c_acctbal, c_phone, n_name, c_address, c_comment) %>% 
        summarise(revenue=sum(l_extendedprice * (1 - l_discount))) %>% 
        select(c_custkey, c_name, revenue, c_acctbal, n_name, c_address, c_phone, c_comment) %>% 
        arrange(desc(revenue)) %>% head(20)
}

test_dplyr <- function(src, q, ...) {
    data_comparable(test_dplyr_q[[q]](src), get_answer(q), ...)
}
