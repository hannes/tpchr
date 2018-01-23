\name{test_dplyr}
\alias{test_dplyr}

\title{
Run benchmark queries on dplyr
}
\description{
  Runs the TPC-H queries using dplyr on a provided source and compare the result with the reference answer.
}
\usage{
test_dplyr(src, q, sf) 
}
\arguments{
   \item{src}{A dplyr source object.}
   \item{q}{Query number (1-10 for now).}
   \item{sf}{Scale factor (0.1 or 1 for now).}
  }
\value{
  Returns true if the query result is equal to the reference answer.
}
