library(testthat)
library(DBI)
library(tpchr)

test_that( "MonetDBLite works" , {
	tbls <- dbgen(1)
	c <- dbConnect(MonetDBLite::MonetDBLite(), tempdir())
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})
	lapply(1:22, function(n) {expect_true(test_dbi(c, n))})
})
