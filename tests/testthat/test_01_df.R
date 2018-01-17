library(testthat)
library(dplyr)

test_that( "dplyr backed by data frames works" , {
	tbls <- tpchr::dbgen(1)
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
