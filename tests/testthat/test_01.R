library(testthat)
library(tpchr)
library(DBI)

if (Sys.info()[['sysname']]=="Windows") {
	memory.limit(memory.size())
}

sf <- 0.1 # can be 0.1 (150MB) or 1 (1.5 GB)

tbls <- dbgen(sf=sf, lean=TRUE)
tbls_dt <- lapply(tbls, data.table::as.data.table)

test_that("dplyr produces correct results on data.frame" , {
	s <- dplyr::src_df(env = list2env(tbls))
	lapply(1:10, function(n) {expect_true(test_dplyr(s, n, sf))})
})

test_that("dplyr produces correct results on data.table" , {
	s <- dplyr::src_df(env = list2env(tbls_dt))
	lapply(1:10, function(n) {expect_true(test_dplyr(s, n, sf))})
})

test_that("dplyr produces correct results using dtplyr" , {
	s <- dtplyr::src_dt(env = list2env(tbls_dt))
	lapply(c(1,2,3,5,6,7,8,9), function(n) {expect_true(test_dplyr(s, n, sf))})
	# query 4 and 10 are broken in dev
})

test_that("data.table produces correct results" , {
	lapply(1:10, function(n) {expect_true(test_dt(tbls_dt, n, sf))})
})

con <- dbConnect(MonetDBLite::MonetDBLite())
lapply(names(tbls), function(n) {dbWriteTable(con, n, tbls[[n]])})

test_that("dbplyr on MonetDBLite produces correct results" , {
	s <- MonetDBLite::src_monetdblite(con=con)
	lapply(c(1,3,4,5,6,10), function(n) {expect_true(test_dplyr(s, n, sf))})
	# q2 etc. broken because of grepl()
})

test_that("MonetDBLite produces correct results with SQL queries" , {
	lapply(c(1:13, 15:22), function(n) {expect_true(test_dbi(con, n, sf))})
	# q14 is broken in CRAN version, fixed in devs
})

dbDisconnect(con, shutdown=T)

