library(testthat)
library(dplyr)
library(tpchr)
library(DBI)

tbls <- dbgen(1)
q <- c(1,2,4,5,6,7,8,10) # 3 and 9 have issues, 11++ needs work

test_that( "dplyr backed by data frames" , {
	s <- src_df(env = list2env(tbls))
	lapply(q, function(n) {expect_true(test_dplyr(s, n))})
})


test_that( "MonetDBLite" , {
	c <- dbConnect(MonetDBLite::MonetDBLite())
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})
	lapply(q, function(n) {expect_true(test_dbi(c, n))})
	dbDisconnect(c, shutdown=T)
})
