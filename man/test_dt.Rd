\name{test_dt}
\alias{test_dt}

\title{
Run benchmark queries on data.table
}
\description{
  Runs the TPC-H queries using data.table on a provided list of data.table's and compare the result with the reference answer.
}
\usage{
test_dt(src, q, ...) 
}
\arguments{
   \item{src}{A named list of data.table's.}
   \item{q}{Query number (1-10 for now).}
   \item{...}{Arguments passed to downstream functions.}
  }
\value{
  Returns true if the query result is equal to the reference answer.
}
