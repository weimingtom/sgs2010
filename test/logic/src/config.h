#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <setjmp.h>
#include <errno.h>

#ifdef WIN32

// windows specify definitions

#define snprintf      _snprintf
#define vsnprintf     _vsnprintf
#define strcasecmp    _stricmp
#define strncasecmp   _strnicmp

#define __ATTR_FMT__(f,m,n)

#elif defined(linux) || defined (__linux__)

#ifndef LINUX
#define LINUX
#endif

// linux specified headers
#include <unistd.h>
#include <iconv.h>
#include <readline/readline.h>
#include <readline/history.h>

#define OUTPUT_UTF8   1 // output message use utf-8 encoding


#define __ATTR_FMT__(f,m,n)   __attribute__((format(f,m,n)));


#else

#error "unknow platform!"

#endif

// lua headers

#ifdef __cplusplus
extern "C" {
#endif
	
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
	
	
#ifdef __cplusplus
};
#endif


// tolua headers
#include <tolua++.h>


#ifdef __cplusplus
extern "C" {
#endif


#define PROJ_NAME    "SGS. Test Logic"
#define VERSION_STR  "v0.1.0"


#define  X_REALLOC(p, sz)   realloc((p), (sz))
#define  X_ALLOC(sz)        X_REALLOC(NULL, sz)
#define  X_FREE(p)          (void)free((p))
	
#define  X_TALLOC(st)        (st*)X_ALLOC(sizeof(st))
#define  X_TNALLOC(st, n)        (st*)X_ALLOC(sizeof(st) * (n))


#define  MAX_NAME_LEN    32
#define  MAX_DESC_LEN    512
#define  MAX_PLAYER_NUM  20

#ifndef COUNT
#define COUNT(a)  (sizeof(a)/sizeof((a)[0]))
#endif


#ifndef OFFSET
#define OFFSET(st,m)   ((size_t)(((st*)0)->m))
#endif


#define ST_ZERO(st)   (memset(&st, 0, sizeof(st)))

#ifdef MIN 
#undef MIN
#endif

#ifdef MAX 
#undef MAX
#endif


#define MIN(a,b)   ((a)<(b)?(a):(b))
#define MAX(a,b)   ((a)>(b)?(a):(b))

#define C2I(c)  ((int)(unsigned char)(c))

// tolua_begin
typedef enum _YESNO
{
	NO = 0,
	YES = 1,
} YESNO;
// tolua_end

#define B2YESNO(b) ((b)?YES:NO)


// tolua_begin
typedef enum _RESULT
{
	R_SUCC  = 0,   // success, and continue loop
	R_DEF   = 1,   // do default proc and continue loop
	R_BACK  = 2,   // back to caller ,end cmd loop
	R_EXIT  = 3,   // exit the game ,end cmd loop
	R_CANCEL  = 4,   // op cancel, end cmd loop
	R_ABORT  = 5,   // op abort, end cmd loop
	R_RETRY  = 6,   // retry it , continue loop
	R_SKIP   = 7,   // skip the loop , and as success


	// error code ( <0 )
	R_E_FAIL  = -1,   // general fail, and continue loop
	R_E_PARAM = -2,   // error param
	R_E_MEMORY = -3,  // out of memory
	R_E_STATUS = -4,  // error status
	R_E_TIMEOUT = -5, // op timeout
	R_E_OVERFLOW = -6, // overflow , 
} RESULT;
// tolua_end


#define IS_FAIL(res)  ((res) < 0)
#define IS_SUCC(res)  ((res) == R_SUCC)
#define IS_CANCEL(res) ((res) == R_CANCEL)
#define CHECK_RET(b, r)  do { if( (b) != R_SUCC ) { return (r); } } while(0) 
#define CHECK_RETV(b)  do { if( (b) != R_SUCC ) { return ; } } while(0) 

#ifdef __cplusplus
}
#endif



#endif /* __CONFIG_H__ */


