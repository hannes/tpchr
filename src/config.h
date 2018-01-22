/* 
 * this file allows the compilation of DBGEN to be tailored to specific
 * architectures and operating systems. Some options are grouped 
 * together to allow easier compilation on a given vendor's hardware.
 * 
 * The following #defines will effect the code:
 *   SEPARATOR         -- character used to separate fields in flat files
 *   STDLIB_HAS_GETOPT -- to prevent confilcts with gloabal getopt() 
 *   MDY_DATE          -- generate dates as MM-DD-YY
 *   WIN32             -- support for WindowsNT
 *   DSS_HUGE          -- 64 bit data type
 *   HUGE_FORMAT       -- printf string for 64 bit data type
 *   EOL_HANDLING      -- flat files don't need final column separator
 *
 * Certain defines must be provided in the makefile:
 *   MACHINE defines
 *   ==========
 *   ATT        -- getopt() handling
 *   HP         -- posix source inclusion differences
 *   IBM        -- posix source inclusion differences
 *   SGI        -- getopt() handling
 *   SUN        -- getopt() handling
 *   LINUX      
 *   WIN32      -- for WINDOWS
 *
 *   DATABASE defines
 *   ================
 *   DB2        -- use DB2 dialect in QGEN
 *   INFORMIX   -- use Informix dialect in QGEN
 *   SQLSERVER  -- use SQLSERVER dialect in QGEN
 *   SYBASE     -- use Sybase dialect in QGEN
 *   TDAT       -- use Teradata dialect in QGEN
 *
 *   WORKLOAD defines
 *   ================
 *   TPCH              -- make will create TPCH (set in makefile)
 */

#ifdef ATT
#define STDLIB_HAS_GETOPT
#ifdef SQLSERVER
#define WIN32
#else
#define DSS_HUGE long long
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#define HUGE_FORMAT "%LLd"
#define HUGE_DATE_FORMAT "%02LLd"
#endif /* SQLSERVER or MP/RAS */
#endif /* ATT */

#ifdef HP
#define _INCLUDE_POSIX_SOURCE
#define STDLIB_HAS_GETOPT
#define DSS_HUGE long
#define HUGE_COUNT 2
#define HUGE_FORMAT "%ld"
#define HUGE_DATE_FORMAT "%02lld"
#define RNG_C 1ull
#define RNG_A 6364136223846793005ull
#endif /* HP */

#ifdef IBM
#define STDLIB_HAS_GETOPT
#define DSS_HUGE long long
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* IBM */

#ifdef LINUX
#define STDLIB_HAS_GETOPT
#define DSS_HUGE long long int
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* LINUX */

#ifdef MAC
//#define _POSIX_C_SOURCE 200112L
//#define _POSIX_SOURCE
#define STDLIB_HAS_GETOPT
<<<<<<< HEAD:src/config.h
#define SUPPORT_64BITS
#define DSS_HUGE long long
=======
#define DSS_HUGE long
>>>>>>> 463c544041d12eb536475f11054a47c0d8bd0bdd:config.h
#define HUGE_FORMAT	"%ld"
#define HUGE_DATE_FORMAT	"%02ld"
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* MAC */

#ifdef SUN
#define STDLIB_HAS_GETOPT
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#define DSS_HUGE long long
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#endif /* SUN */

#ifdef SGI
#define STDLIB_HAS_GETOPT
#define DSS_HUGE __int64_t
#endif /* SGI */

#ifdef WIN32 // hack
#define _POSIX_
#endif

#if (defined(WIN32)&&!defined(_POSIX_))
#define PATH_SEP	'\\'
#define DSS_HUGE __int64
#define RNG_A	6364136223846793005uI64
#define RNG_C	1uI64
#define HUGE_FORMAT "%I64d"
#define HUGE_DATE_FORMAT "%02I64d"
/* requried by move to Visual Studio 2005 */
#define strdup(x) _strdup(x)
#endif /* WIN32 */

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif /* PATH_SEP */

