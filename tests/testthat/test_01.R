library(testthat)
library(dplyr)
library(tpchr)

tbls <- dbgen(1)

test_that( "dplyr backed by data frames works" , {
	s <- src_df(env = list2env(tbls))

	expect_true(test_dplyr(s, 1))
	expect_true(test_dplyr(s, 2))
#	expect_true(test_dplyr(s, 3))
	expect_true(test_dplyr(s, 4))
	expect_true(test_dplyr(s, 5))
	expect_true(test_dplyr(s, 6))
	expect_true(test_dplyr(s, 7))
	expect_true(test_dplyr(s, 8))
#	expect_true(test_dplyr(s, 9))
	expect_true(test_dplyr(s, 10))
})


test_that( "MonetDBLite works" , {
	c <- dbConnect(MonetDBLite::MonetDBLite(), ":memory")
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})
	lapply(1:22, function(n) {expect_true(test_dbi(c, n))})
})

test_that( "SQLite works" , {
	c <- dbConnect(RSQLite::SQLite())
	lapply(names(tbls), function(n) {dbWriteTable(c, n, tbls[[n]])})
	lapply(1:22, function(n) {expect_true(test_dbi(c, n))})
})
