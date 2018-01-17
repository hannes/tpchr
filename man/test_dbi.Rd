\name{test_dbi}
\alias{test_dbi}

\title{
Run benchmark queries on DBI
}
\description{
  Runs the TPC-H queries using DBI on a provided connection and compare the result with the reference answer.
}
\usage{
test_dbi(con, q) 
}
\arguments{
   \item{con}{A DBI connection object.}
   \item{q}{Query number (1-22 for now).}
  }
\value{
  Returns true if the query result is equal to the reference answer.
}
