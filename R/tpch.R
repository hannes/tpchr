check_flag <- function(f) {
    length(f) != 1 || is.na(f) || !is.numeric(f)
}

get_answer <- function(q=1) {
    if (check_flag(q) || q < 1 || q > 22) stop("Need a single query number 1-22 as parameter")
    read.delim(system.file(sprintf("extdata/answers/q%d.ans", q), package="tpchr"), sep="|", stringsAsFactors=FALSE)
}

get_query <- function(q=1) {
    if (check_flag(q) || q < 1 || q > 22) stop("Need a single query number 1-22 as parameter")
    paste(readLines(system.file(sprintf("extdata/queries/q%d.sql", q), package="tpchr")), collapse="\n")
}

dbgen <- function(sf=0.01, lean=FALSE) {
    if (check_flag(sf) || sf < 0) stop("Need a single scale factor > 0 as parameter")
    Sys.setenv(DSS_CONFIG = system.file("extdata", package="tpchr"))
    .Call(dbgen_R, as.numeric(sf), as.logical(lean))
}

data_comparable <- function(df1, df2, dlt=.0001) {
    df1 <- as.data.frame(df1, stringsAsFactors=F)
    df2 <- as.data.frame(df2, stringsAsFactors=F)
    if (!identical(dim(df1), dim(df2))) {
        return(FALSE)
    }
    for (col_i in length(df1)) {
        col1 <- df1[[col_i]]
        col2 <- df2[[col_i]]
        if (is.numeric(col1)) {
            # reference answers are rounded to two decimals
            col1 <- round(col1, 2)
            col2 <- round(col2, 2)
            if (any(abs(col1 - col2) > col1 * dlt)) {
                return(FALSE)
            }
        } else {
            col1 <- trimws(as.character(col1))
            col2 <- trimws(as.character(col2))
            if (any(col1 != col2)) {
                return(FALSE)
            }
        }
    }
    return(TRUE)
}


test_dbi <- function(con, q, ...) {
    data_comparable(dbGetQuery(con, get_query(q)), get_answer(q), ...)
}
