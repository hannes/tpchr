dbgen <- function(sf) {
	Sys.setenv(DSS_CONFIG = system.file("data", package="tpchr"))
	.Call(dbgen_R, as.numeric(sf))
}
