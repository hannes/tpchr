cmake_minimum_required(VERSION 3.5)

project(tpch-dbgen C)

set(DATABASE ORACLE)
set(MACHINE MAC)
set(WORKLOAD TPCH)

add_executable(dbgen
    build.c
    driver.c
    bm_utils.c
    rnd.c
    print.c
    load_stub.c
    bcd2.c
    speed_seed.c
    text.c
    permute.c
    rng64.c
    )

target_compile_definitions(
    dbgen
    PUBLIC
    -DDBNAME="dss"
    -D${MACHINE}
    -D${DATABASE}
    -D${WORKLOAD}
    -DRNG_TEST
    -D_FILE_OFFSET_BITS=64
)