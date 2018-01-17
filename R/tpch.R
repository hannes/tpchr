dbgen <- function(sf=0.01) {
	Sys.setenv(DSS_CONFIG = system.file("extdata", package="tpchr"))
	tbls <- .Call(dbgen_R, as.numeric(sf))
	# fixme, performance
	tbls[["lineitem"]][["l_shipdate"]] <- as.Date(tbls[["lineitem"]][["l_shipdate"]])
	tbls[["lineitem"]][["l_commitdate"]] <- as.Date(tbls[["lineitem"]][["l_commitdate"]])
	tbls[["lineitem"]][["l_receiptdate"]] <- as.Date(tbls[["lineitem"]][["l_receiptdate"]])
	tbls[["orders"]][["o_orderdate"]] <- as.Date(tbls[["orders"]][["o_orderdate"]])

	tbls
}
