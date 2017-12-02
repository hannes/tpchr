cmake_minimum_required(VERSION 3.0)

project(tpch-dbgen # TPC-H benchmark data (and query) generator utilities
	VERSION 2.17.3 # taken from the TPC-H benchmark release version
	LANGUAGES C)

# Relevant values for DATABASE are:
# INFORMIX, DB2, TDAT (Teradata), SQLSERVER, SYBASE, ORACLE, VECTORWISE
set(DATABASE DB2 CACHE STRING "DBMS dialect to target with the qgen query generator")
# Relevant values for MACHINE are:
# ATT, DOS, HP, IBM, ICL, MVS, SGI, SUN, U2200, VMS, LINUX, WIN32, MAC
set(MACHINE LINUX CACHE STRING "Machine type")
# Relevant values for MACHINE are:
# TPCH
set(WORKLOAD TPCH CACHE STRING "Workload")

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
	TARGET 
		dbgen qgen 
	APPEND PROPERTY COMPILE_DEFINITIONS 
		DBNAME="dss"
		${MACHINE}
		${DATABASE}
		${WORKLOAD}
		RNG_TEST
		FILE_OFFSET_BITS=64
) 
 
