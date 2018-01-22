library(testthat)
library(dplyr)
library(tpchr)
library(DBI)

tbls <- dbgen(sf=1, lean=TRUE)

test_that( "dplyr backed by data frames" , {
	s <- src_df(env = list2env(tbls))
	lapply(1:10, function(n) {expect_true(test_dplyr(s, n))})
})

test_that( "MonetDBLite" , {
	con <- dbConnect(MonetDBLite::MonetDBLite())
	lapply(names(tbls), function(n) {dbWriteTable(con, n, tbls[[n]])})
	lapply(c(1:13, 15:22), function(n) {expect_true(test_dbi(con, n))})
	dbDisconnect(con, shutdown=T)
})
