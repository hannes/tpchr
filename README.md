This repository holds the data generation and query generation utilities for the TPC-H analytics benchmark for DBMSes:

* The generation utility `dbgen` produces schema data in several flat table files, in a simple textual format, which can then be loaded into a DBMS for running the benchmark; it can theoretically skip this and load data directly into a DBMS, but that may not work so well. 
* The query generation utility `qgen` produces query files which may be sent to a DBMS to actually carry out the benchmark.

| Table of contents|
|:----------------|
| ["What? _Another_ fork of tpch-dbgen? Why?"](#another-fork)<br>  [About the TPC-H benchmark)<br> [Building the generation utilities](#building)<br> [Using `dbgen` to generate data](#using)<br>|

## <a name="another-fork">"What? _Another_ fork of tpch-dbgen? Why?"</a>

The `tpch-dbgen` utility is a minor(ish) m odification of the original utility available on the [TPC-H benchmark's dedicated page](http://tpc.org/tpch/). It's somewhat crufty code with all kinds of warts. It's not the intention of this repository to make all of those disappear, or to effect a rewrite - but avoiding compiler warnings, ensuring the build passes on more platforms, and other such minor issues are what this endeavor is about.

Now, there are several repositories on GitHub with such modifications of the official TPC-H `dbgen` - each resolving some issues and ignoring others; some adding customizations relevant only to some users; and each starting out with a potentially outdated version of the official utility.  This particular repository is an attempt to **unify** all of them, applying all changes to the code which - in my opinion - are generally useful, and merging them all together. Details of what's already been done can be found on the [Closed Issues Page](https://github.com/eyalroz/tpch-dbgen/issues?q=is%3Aissue+is%3Aclosed) and of course by examining the commit comments.

If you are the author of one of the other repositories - please [contact me](mailto:eyalroz@technion.ac.il) for better coordination of this effort.

## <a name="about-ssb">About the TPC-H</a>

The TPC-H is an initiative of the [Transaction Processiong Council](http://www.tpc.org/), an industrial consortium with the major DBMS vendors as members, which is intended to standardize the performance benchmarking of DBMSes in various usage scenarios. It has had benchmarks named TPC-A, TPC-B, TPC-C and so on; some have long been deprecated and are irrelevant. You can read about the various actively-supported benchmarks [here](http://www.tpc.org/information/benchmarks.asp).

Among the TPC benchmarks, TPC-H is intended for **Decision Support**, or in other words - analytic query processing capabilities of DBMSes. It follows a commercial busines scenario involving suppliers, customers, parts, suppliers of parts and orders of parts dispatched to clients, over the course of several business years (1992-1998). Its schema is not very complicated, but not trivial (e.g. it is not a "star schema"); and it has 22 queries ranging from simpler to relatively long and involved - although none of them utilizes more advanced or esoteric SQL features (such as window functions, UDFs and so on). It is in wide use both in its "proper" form - of sending random variants of the queries from the set to a running DBMS over the course of an hour - and in the "artificial" form of running individual queries on a cold DBMS.

## <a name="building">Building the generation utility</a>

The build process is not completely automated, unfortunately (an inheritance from the TPC-H dbgen utility), and comprises of two phases: Semi-manually generating a [Makefile](https://en.wikipedia.org/wiki/Makefile), then an automated build using that Makefile.

#### Generating a Makefile

Luckily, the Makefile is all-but-written for you, in the form of a template, [`makefile.suite`](https://github.com/eyalroz/tpch-dbgen/blob/master/makefile.suite). What remains for you to do is (assuming a non-Windows system):

1. Copy `makefile.suite` to `Makefile`
2. Set the values of the variables `DATABASE`, `MACHINE`, `WORKLOAD` and `CC`:

|Variable   |How to set it?   | List of options |
|-----------|-----------------|-----------------|
| `DATABASE`  | Use `DB2` if you can't tell what you should use; try one of the other options if that's the DBMS you're going to benchmark with  | `INFORMIX`, `DB2`, `TDAT`, `SQLSERVER`, `SYBASE` |
| `MACHINE`  | According to the platform/operating system you're using  | `ATT`, `DOS`, `HP`, `IBM`, `ICL`, `MVS`, `SGI`, `SUN`, `U2200`, `VMS`, `LINUX`, `MAC` |
| `WORKLOAD`  | Use `SSB`   | `SSB`, `TPCH`, `TPCR` (but better not try the last two)
| `CC`  |  Use the base name of your system's C compiler (assuming it's in the search path)  | N/A |

3. Set the values of the object file suffix variable (`OBJ`), exeutable file prefix (`EXE`) and of the libraries necessary to compile TPCH-DBGEN (`LIBS`).

#### Building using the Makefile

Your system should have the following software:

* GNU Make (which is standard on essentially all Unix-like systems today, specifically on Linux distributions), or Microsoft's NMake (which comes bundled with MS Visual Studio).
* A C language compiler (C99/C2011 support is not necessary) and linker. GNU's compiler collection (gcc) is know to work on Linux; and MSVC probably works on Windows. clang, ICC or others should be ok as well.

Now, simply execute `make -C /path/to/your/tpch-dbgen`; on Windows, you will need to be in the repository's directory and execute `nmake`. If you're in a terminal/command prompt session, the output should have several lines looking something like this:
```
gcc -O -DDBNAME=\"dss\" -DLINUX -DDB2  -DSSB    -c -o bm_utils.o bm_utils.c
```
and finally, the executable files `dbgen` and `qgen` (or `dbgen.exe` and `qgen.exe` on Windows) should now appear in the source folder.

## <a name="using">Using `dbgen` to generate schema data</a>

The `dbgen` utility should be run from within the source folder (it can be run from elsewhere but you would need to copy the `dists.dss` file at least); typically you would also want to specify the size scale factor. Thus:

    $ ./dbgen -v -s 10 
    
will create the various table files (e.g. `customer.tbl`, `nation.tbl`, `region.tbl`, `supplier.tbl` and so on) in the current directory, with a scale factor of 10, i.e. 300,000 customer lines. Here are the first few lines of a resulting `customer.tbl`: 
```
1|Customer#000000001|j5JsirBM9P|MOROCCO  0|MOROCCO|AFRICA|25-989-741-2988|BUILDING|
2|Customer#000000002|487LW1dovn6Q4dMVym|JORDAN   1|JORDAN|MIDDLE EAST|23-768-687-3665|AUTOMOBILE|
3|Customer#000000003|fkRGN8n|ARGENTINA7|ARGENTINA|AMERICA|11-719-748-3364|AUTOMOBILE|
4|Customer#000000004|4u58h f|EGYPT    4|EGYPT|MIDDLE EAST|14-128-190-5944|MACHINERY|
```
the fields are separated by a pipe character (`|`), and there's a trailing pipe at the end of the line. 

After generating `.tbl` files for all tables, you should now either load them directly into your DBMS, or apply some textual processing on them.

**Note:** On Unix-like systems, it is also possible to write the generated data into a FIFO filesystem node, reading from the other side with a compression utility, so as to only write compressed data to disk. This may be useful of disk space is limited and you are using a particularly high scale factor.

<br>

Have you encountered some other issue with `dbgen` or `qgen`? Please open a new issue on the [Issues Page](https://github.com/eyalroz/tpch-dbgen/issues); be sure to list exactly what you did and enter a copy of the terminal output of the commands you used. Note that the issue might be with the original, unmodified TPC-H `dbgen`/`qgen` code, in which case the issue may be forwarded rather than resolved.


