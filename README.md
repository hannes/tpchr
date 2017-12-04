This repository holds the data generation and query generation utilities (`dbgen` and `qgen`) for the TPC-H analytics benchmark for DBMSes

| Table of contents|
|:----------------|
| ["What? _Another_ fork of tpch-dbgen? Why?"](#another-fork)<br>  [About the TPC-H benchmark](#about-tpch)<br> [Building the generation utilities](#building)<br> [Using `dbgen` to generate data](#using)<br>|

## <a name="another-fork">"What? _Another_ fork of tpch-dbgen? Why?"</a>

The `tpch-dbgen` utility is a minor(ish) m odification of the original utility available on the [TPC-H benchmark's dedicated page](http://tpc.org/tpch/). It's somewhat crufty code with all kinds of warts. It's not the intention of this repository to make all of those disappear, or to effect a rewrite - but avoiding compiler warnings, ensuring the build passes on more platforms, and other such minor issues are what this endeavor is about.

Now, there are several repositories on GitHub with such modifications of the official TPC-H `dbgen` - each resolving some issues and ignoring others; some adding customizations relevant only to some users; and each starting out with a potentially outdated version of the official utility.  This particular repository is an attempt to **unify** all of them, applying all changes to the code which - in my opinion - are generally useful, and merging them all together. Details of what's already been done can be found on the [Closed Issues Page](https://github.com/eyalroz/tpch-dbgen/issues?q=is%3Aissue+is%3Aclosed) and of course by examining the commit comments.

If you are the author of one of the other repositories - please [contact me](mailto:eyalroz@technion.ac.il) for better coordination of this effort.

## <a name="about-tpch">About the TPC-H benchmark</a>

TPC benchmark H is one of several benchmarks created and maintained by the [Transaction Processiong Council](http://www.tpc.org/) - TPC for short. The TPC, is industrial consortium with the major DBMS vendors as members, which is intended to standardize the performance benchmarking of DBMSes in various usage scenarios. Other TPC-H benchmarks have been named TPC-A, TPC-B, TPC-R, TPC-DS and so on; some are deprecated and no longer irrelevant. You can read about the actively-supported benchmarks [here](http://www.tpc.org/information/benchmarks.asp).

TPC-H is intended for benchmarking the **Decision Support** performance of a DBMS, or in other words - analytic query processing performance. It follows a commercial business scenario involving suppliers, customers, parts, suppliers of parts and orders of parts shipped to clients, over the course of several business years (1992-1998). Its schema is not very complicated, but not trivial either (e.g. it is not a "star schema"). It has 22 queries ranging from simpler to relatively long and involved - although none of them utilizes more advanced or esoteric SQL features (such as window functions, UDFs and so on). It is in wide use both in its "proper" form - of sending random variants of the queries from the set to a running DBMS over the course of an hour - and in the "artificial" form of running individual queries on a cold DBMS.

## <a name="building">Building the generation utility</a>

The original, TPC-distributed version of the code in this repository required manually creating a `Makefile` from am template (`makefile.suite`) - that is **not necessary** with this repository.

The build process is automated using [Kitware CMake](https://www.cmake.org/). There are several settings which you must make before building, which CMake will guide you through if you [invoke it properly](https://cmake.org/runningcmake/) - that is, using the GUI or terminal-based user interface that presents the configuration options for you to choose from.

Following is an explanation of the values you would need to set using the GUI or TUI:


|Variable     | Used by     | How to set it?   |List of options |
|-------------|-------------|--------|-----------------|
| `Database`  | qgen  | Select the name of the DBMS closest to the one you're benchmarking in terms of syntax. If unsure, choose `DB2` |  `INFORMIX` `DB2` `TDAT` `SQLSERVER` `SYBASE` `ORACLE` `VECTORWISE` `POSTGRES` |
| `Platform`  | dbgen, qgen | According to the platform/operating system you're using  | `ATT`, `DOS`, `HP`, `IBM`, `ICL`, `MVS`, `SGI`, `SUN`, `U2200`, `VMS`, `LINUX`, `MAC` |
| `Workload`  | dbgen, qgen | Use `TPCH`   | 
| `SeparatorAtEndOfLine`  | dbgen  | Set to OFF if your DBMS doesn't support loading the data if it has a separator character at the end of each line | `ON` or `OFF` (it's a boolean really) |

CMake will generate files (including a Makefile) which you can then use with your platform-specific build tools, e.g. NMake for Windows or [GNU Make](https://www.gnu.org/software/make/) on Unix-like systems. If you're not sure how to use them, consult the documentation or Goole.

## <a name="using">Using `dbgen` to generate schema data</a>

Typically, dbgen is invoked with a specific scale factor, and it must be directed at the [`dists.dss`](https://github.com/eyalroz/tpch-dbgen/blob/master/dists.dss) file. Thus the command-line should look something like this:

    $ /path/to/dbgen -v -s 10 -b /path/to/dists.dss
    
which will create the various table files (e.g. `customer.tbl`, `nation.tbl`, `region.tbl`, `supplier.tbl` and so on) in the current directory, with a scale factor of 10, i.e. 300,000 customer lines. Here are the first few lines of a resulting `customer.tbl`: 
```
1|Customer#000000001|j5JsirBM9P|MOROCCO  0|MOROCCO|AFRICA|25-989-741-2988|BUILDING|
2|Customer#000000002|487LW1dovn6Q4dMVym|JORDAN   1|JORDAN|MIDDLE EAST|23-768-687-3665|AUTOMOBILE|
3|Customer#000000003|fkRGN8n|ARGENTINA7|ARGENTINA|AMERICA|11-719-748-3364|AUTOMOBILE|
4|Customer#000000004|4u58h f|EGYPT    4|EGYPT|MIDDLE EAST|14-128-190-5944|MACHINERY|
```
the fields are separated by a pipe character (`|`), and there's a trailing pipe at the end of the line - unless you set `SeparatorAtEndOfLine` option to `OFF`.

After generating `.tbl` files for all tables, you should now either load them directly into your DBMS, or apply some textual processing on them.

**Note:** On Unix-like systems, it is also possible to write the generated data into a FIFO filesystem node, reading from the other side with a compression utility, so as to only write compressed data to disk. This may be useful of disk space is limited and you are using a particularly high scale factor.

<br>

Have you encountered some other issue with `dbgen` or `qgen`? Please open a new issue on the [Issues Page](https://github.com/eyalroz/tpch-dbgen/issues); be sure to list exactly what you did and enter a copy of the terminal output of the commands you used. Note that the issue might be with the original, unmodified TPC-H `dbgen`/`qgen` code, in which case the issue may be forwarded rather than resolved.


