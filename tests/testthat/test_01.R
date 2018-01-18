library(testthat)
library(dplyr)
library(tpchr)
library(DBI)

tbls <- dbgen(1)

test_that( "dplyr backed by data frames" , {
	s <- src_df(env = list2env(tbls))
	lapply(1:10, function(n) {expect_true(test_dplyr(s, n))})
})

test_that( "MonetDBLite" , {
	c <- dbConnect(MonetDBLite::MonetDBLite())
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})
	lapply(1:22, function(n) {expect_true(test_dbi(c, n))})
	dbDisconnect(c, shutdown=T)
})
