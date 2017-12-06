/* 
 * this file allows the compilation of DBGEN to be tailored to specific
 * architectures and operating systems. Some options are grouped 
 * together to allow easier compilation on a given vendor's hardware.
 * 
 * The following #defines will effect the code:
 *   KILL(pid)         -- how to terminate a process in a parallel load
 *   SPAWN             -- name of system call to clone an existing process
 *   SET_HANDLER(proc) -- name of routine to handle signals in parallel load
 *   WAIT(res, pid)    -- how to await the termination of a child
 *   SEPARATOR         -- character used to separate fields in flat files
 *   STDLIB_HAS_GETOPT -- to prevent confilcts with gloabal getopt() 
 *   MDY_DATE          -- generate dates as MM-DD-YY
 *   WIN32             -- support for WindowsNT
 *   SUPPORT_64BITS    -- compiler defines a 64 bit datatype
 *   DSS_HUGE          -- 64 bit data type
 *   HUGE_FORMAT       -- printf string for 64 bit data type
 *   EOL_HANDLING      -- flat files don't need final column separator
 *
 * Certain defines must be provided in the makefile:
 *   MACHINE defines
 *   ==========
 *   ATT        -- getopt() handling
 *   DOS        -- disable all multi-user functionality/dependency
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

#ifdef DOS
#define PATH_SEP	'\\'
#else


#ifdef ATT
#define STDLIB_HAS_GETOPT
#ifdef SQLSERVER
#define WIN32
#else
/* the 64 bit defines are for the Metaware compiler */
#define SUPPORT_64BITS
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
#define SUPPORT_64BITS
#define DSS_HUGE long
#define HUGE_COUNT 2
#define HUGE_FORMAT "%ld"
#define HUGE_DATE_FORMAT "%02lld"
#define RNG_C 1ull
#define RNG_A 6364136223846793005ull
#endif /* HP */

#ifdef IBM
#define STDLIB_HAS_GETOPT
#define SUPPORT_64BITS
#define DSS_HUGE long long
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* IBM */

#ifdef LINUX
#define STDLIB_HAS_GETOPT
#define SUPPORT_64BITS
#define DSS_HUGE long long int
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* LINUX */

#ifdef MAC
#define _POSIX_C_SOURCE 200112L
#define _POSIX_SOURCE
#define STDLIB_HAS_GETOPT
#define SUPPORT_64BITS
#define DSS_HUGE long
#define HUGE_FORMAT	"%ld"
#define HUGE_DATE_FORMAT	"%02ld"
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#endif /* MAC */

#ifdef SUN
#define STDLIB_HAS_GETOPT
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull
#define SUPPORT_64BITS
#define DSS_HUGE long long
#define HUGE_FORMAT	"%lld" 
#define HUGE_DATE_FORMAT	"%02lld" 
#endif /* SUN */

#ifdef SGI
#define STDLIB_HAS_GETOPT
#define SUPPORT_64BITS
#define DSS_HUGE __int64_t
#endif /* SGI */

#if (defined(WIN32)&&!defined(_POSIX_))
#define pid_t int
#define SET_HANDLER(proc) signal(SIGINT, proc)
#define KILL(pid) \
     TerminateProcess(OpenProcess(PROCESS_TERMINATE,FALSE,pid),3)
#if (defined (__WATCOMC__))
#define SPAWN()   spawnv(P_NOWAIT, spawn_args[0], spawn_args)
#define WAIT(res, pid) cwait(res, pid, WAIT_CHILD)
#else
#define SPAWN()   _spawnv(_P_NOWAIT, spawn_args[0], spawn_args)
#define WAIT(res, pid) _cwait(res, pid, _WAIT_CHILD)
#define getpid          _getpid
#endif /* WATCOMC */
#define SIGS_DEFINED
#define PATH_SEP	'\\'
#define SUPPORT_64BITS
#define DSS_HUGE __int64
#define RNG_A	6364136223846793005uI64
#define RNG_C	1uI64
#define HUGE_FORMAT "%I64d"
#define HUGE_DATE_FORMAT "%02I64d"
/* need to define process termination codes to match UNIX */
/* these are copied from Linux/GNU and need to be verified as part of a rework of */
/* process handling under NT (29 Apr 98) */
#define WIFEXITED(s)	((s & 0xFF) == 0)
#define WIFSIGNALED(s)	(((unsigned int)((status)-1) & 0xFFFF) < 0xFF)	
#define WIFSTOPPED(s)	(((s) & 0xff) == 0x7f)
#define WTERMSIG(s)		((s) & 0x7f)
#define WSTOPSIG(s)		(((s) & 0xff00) >> 8)
/* requried by move to Visual Studio 2005 */
#define strdup(x) _strdup(x)
#endif /* WIN32 */

#ifndef SIGS_DEFINED
#define KILL(pid) kill(SIGUSR1, pid)
#define SET_HANDLER(proc) signal(SIGUSR1, proc)
#define SPAWN   fork
#define WAIT(res, pid) wait(res)
#endif /* DEFAULT */

#endif /* DOS */

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif /* PATH_SEP */

#ifndef DSS_HUGE
#error Support for a 64-bit datatype is required in this release
#endif

