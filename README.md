# TPC-H for R

[![Build Status](https://travis-ci.org/hannesmuehleisen/tpchr.svg?branch=master)](https://travis-ci.org/hannesmuehleisen/tpchr)

`tpchr` wraps the Transaction Processing Council's [Decision Support Benchmark](http://www.tpc.org/tpch/) (TPC-H). The benchmark consists of a data generator, 22 queries and reference answers for those queries. The benchmark has been [highly influential](http://oai.cwi.nl/oai/asset/21424/21424B.pdf) in analytical data management research. 

This package wraps the `dbgen` data generator from TPC-H, modified so it produces R `data.frames` instead of flat files, functions to read and compare results with the reference answers and some `dplyr` translations of the SQL benchmark queries. 

## Installation

* the latest version from github on the command line

    ```
    devtools::install_github("hannesmuehleisen/tpchr")
    ```

If you encounter a bug, please file a minimal reproducible example on [github](https://github.com/hannesmuehleisen/tpchr/issues). 
