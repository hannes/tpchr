library(testthat)
library(dplyr)

test_that( "dplyr backed by data frames works" , {
	tbls <- tpchr::dbgen(1)
	src <- src_df(env = list2env(tbls))

	expect_true(test_dplyr(src, 1))
	expect_true(test_dplyr(src, 2))
#	expect_true(test_dplyr(src, 3))
	expect_true(test_dplyr(src, 4))
	expect_true(test_dplyr(src, 5))
	expect_true(test_dplyr(src, 6))
	expect_true(test_dplyr(src, 7))
	expect_true(test_dplyr(src, 8))
#	expect_true(test_dplyr(src, 9))
	expect_true(test_dplyr(src, 10))
})
