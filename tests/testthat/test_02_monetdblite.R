library(testthat)
library(DBI)
library(tpchr)

test_that( "MonetDBLite works" , {
	tbls <- dbgen(1)
	c <- dbConnect(MonetDBLite::MonetDBLite(), ":memory:")
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})

	expect_true(test_dbi(c, 1))
	expect_true(test_dbi(c, 2))
#	expect_true(test_dbi(c, 3))
	expect_true(test_dbi(c, 4))
	expect_true(test_dbi(c, 5))
	expect_true(test_dbi(c, 6))
	expect_true(test_dbi(c, 7))
	expect_true(test_dbi(c, 8))
#	expect_true(test_dbi(c, 9))
	expect_true(test_dbi(c, 10))

})
