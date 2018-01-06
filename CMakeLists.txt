cmake_minimum_required(VERSION 3.0)

project(tpch-dbgen # TPC-H benchmark data (and query) generator utilities
	VERSION 2.17.3 # taken from the TPC-H benchmark release version
	LANGUAGES C)

set(DATABASE "DB2" CACHE STRING "Command language dialect to target with the qgen query generator (\"DATABASE\"")
set_property(CACHE DATABASE PROPERTY STRINGS INFORMIX DB2 TDAT SQLSERVER SYBASE ORACLE VECTORWISE POSTGRES)

set(MACHINE LINUX CACHE STRING "Platform to target (\"MACHINE\")")
set_property(CACHE MACHINE PROPERTY STRINGS ATT HP IBM ICL MVS SGI SUN U2200 VMS LINUX WIN32 MAC)

set(WORKLOAD TPCH CACHE STRING "Choice of benchmark / query workload (\"WORKLOAD\")")
set_property(CACHE WORKLOAD PROPERTY STRINGS TPCH)

set(EOL_HANDLING OFF CACHE BOOL "Skip separator after the last field in generated lines?")

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
		${MACHINE}
		${DATABASE}
		${WORKLOAD}
		RNG_TEST
		_FILE_OFFSET_BITS=64
)

if (EOL_HANDLING)
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
