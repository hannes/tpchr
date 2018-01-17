library(testthat)
library(dplyr)

test_that( "dplyr backed by data frames works" , {
	tbls <- tpchr::dbgen(1)
	src <- src_df(env = list2env(tbls))

	lapply(1:10, function(q) expect_true(test_dplyr(src, q)))
})
