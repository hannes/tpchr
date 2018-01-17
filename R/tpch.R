dbgen <- function(sf=0.01) {
	Sys.setenv(DSS_CONFIG = system.file("extdata", package="tpchr"))
	.Call(dbgen_R, as.numeric(sf))
}
