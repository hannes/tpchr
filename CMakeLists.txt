cmake_minimum_required(VERSION 3.0)

project(tpch-dbgen # TPC-H benchmark data (and query) generator utilities
	VERSION 2.17.3 # taken from the TPC-H benchmark release version
	LANGUAGES C)

set(Database "DB2" CACHE STRING "DBMS dialect to target with the qgen query generator")
set_property(CACHE Database PROPERTY STRINGS INFORMIX DB2 TDAT SQLSERVER SYBASE ORACLE VECTORWISE POSTGRES)

set(Machine LINUX CACHE STRING "Machine type")
set_property(CACHE Machine PROPERTY STRINGS ATT DOS HP IBM ICL MVS SGI SUN U2200 VMS LINUX WIN32 MAC)

set(Workload TPCH CACHE STRING "Choice of benchmark / query workload")
set_property(CACHE Workload PROPERTY STRINGS TPCH)

set(SeparatorAtEndOfLine ON CACHE BOOL "Print a separator after the last field of each generated table file line?")

add_executable(dbgen
	bcd2.c
	bm_utils.c
	build.c
	driver.c
	load_stub.c
	permute.c
	print.c
	rnd.c
	rng64.c
	speed_seed.c
	text.c
)

add_executable(qgen
	bm_utils.c
	build.c
	permute.c
	qgen.c
	rnd.c
	rng64.c
	speed_seed.c
	text.c
	varsub.c
)

set_property(
	TARGET dbgen qgen
	APPEND PROPERTY COMPILE_DEFINITIONS
		DBNAME="dss"
		${Machine}
		${Database}
		${Workload}
		RNG_TEST
		FILE_OFFSET_BITS=64
)

if (NOT SeparatorAtEndOfLine)
	set_property(
		TARGET dbgen qgen
		APPEND PROPERTY COMPILE_DEFINITIONS EOL_HANDLING
	)
endif()

set_property(
	TARGET dbgen qgen
	APPEND PROPERTY COMPILE_OPTIONS
		-Wall
)
