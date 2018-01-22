\name{dbgen}
\alias{dbgen}

\title{
  Create the TPC-H Tables
}
\description{
  Generate the TPC-H tables with the given scale factor
}
\usage{
dbgen(sf=0.01, lean=FALSE) 
}
\arguments{
   \item{sf}{Scale factor, controls the size of the generated tables.}
    \item{lean}{Don't generate columns not used in queries.}

  }
\value{
  Returns a list of eight data frames containing the TPC-H tables
}

\examples{
  lapply(dbgen(), str)
}
