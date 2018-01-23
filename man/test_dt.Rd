\name{test_dt}
\alias{test_dt}

\title{
Run benchmark queries on data.table
}
\description{
  Runs the TPC-H queries using data.table on a provided list of data.table's and compare the result with the reference answer.
}
\usage{
test_dt(src, q, sf) 
}
\arguments{
   \item{src}{A named list of data.table's.}
   \item{q}{Query number (1-10 for now).}
   \item{sf}{Scale factor (0.1 or 1 for now).}
  }
\value{
  Returns true if the query result is equal to the reference answer.
}
