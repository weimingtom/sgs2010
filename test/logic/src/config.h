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


// platform functions defined
#include "platform.h"



#ifdef __cplusplus
extern "C" {
#endif


#define PROJ_NAME    "SGS. 2010 Test Logic"
#define VERSION_STR  "v0.1.0"


#ifndef MAX_PATH
#define MAX_PATH    1024
#endif


#define  X_REALLOC(p, sz)   realloc((p), (sz))
#define  X_ALLOC(sz)        X_REALLOC(NULL, sz)
#define  X_FREE(p)          (void)free((p))
	
#define  X_TALLOC(st)        (st*)X_ALLOC(sizeof(st))
#define  X_TNALLOC(st, n)        (st*)X_ALLOC(sizeof(st) * (n))


#define  MAX_NAME_LEN    32
#define  MAX_DESC_LEN    512
#define  MAX_PLAYER_NUM  20
#define  MAX_ALTER_TEXT_LEN    256
#define  MAX_HERO_NUM    256
#define  MAX_UD_LEN      256

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

typedef enum _CANUSE
{
	USE_CANNOT = 0,  // 不能使用
	USE_MANUAL = 1,  // 可选择的发动
	USE_AUTO = 2,    // 自动触发
	USE_QUIET = 3,   // auto 自动安静使用
} CANUSE;

typedef enum _RESULT
{
	R_SUCC  = 0,   // success, and continue loop
	R_DEF,         // no result and continue loop
	R_BACK,        // no result back to caller
	//R_DONE,        // op success back to caller
	R_CANCEL,      // op cancel, back to caller
	R_SKIP,        // op success back to caller
	R_EXIT,        // exit the game ,end any loop, can not be used in event return code


	// error code ( <0 )
	R_E_FAIL  = -1,   // general fail, and continue loop
	R_E_PARAM = -2,   // error param
	R_E_MEMORY = -3,  // out of memory
	R_E_STATUS = -4,  // error status
	R_E_TIMEOUT = -5, // op timeout
	R_E_OVERFLOW = -6, // overflow, 
	R_E_CONFLICT = -7, // conflict,
	R_E_NUM = -8,  // invalid number of card
	R_E_POS = -9, // invalid card pos
	R_E_WHERE = -10, // card where not match
	R_E_PATTERN = -11, // card pattern not match
	R_E_DISTANCE = -12, // out of distance
	R_E_TARGET = -13, // invalid target

} RESULT;

// tolua_end

#define B2YESNO(b) ((b)?YES:NO)

#define YESNO2STR(yesno)  ((yesno)==YES?"YES":"NO")


#define IS_FAIL(res)  ((res) < 0)
#define IS_SUCC(res)  ((res) == R_SUCC)
#define IS_CANCEL(res) ((res) == R_CANCEL)

//#define CHECK_RET(b, r)  do { if( (b) != R_SUCC ) { return (r); } } while(0) 
//#define CHECK_RETV(b)  do { if( (b) != R_SUCC ) { return ; } } while(0) 
//#define CHECK_BACK_RET(r)    do { if((r) == R_CANCEL || (r) == R_ABORT) { return (r); } /* if((r)==R_DONE || (r)==R_BACK) { return R_SUCC; } */  }while(0)


//#define RET_CHECK_BACK(ret)           do { if((ret) == R_BACK || (ret) == R_CANCEL || (ret) == R_SUCC) { return (ret); }  } while(0)
//#define RET_CHECK_DONE_RET(ret, r)    do { if((ret) == R_DONE ) { return (r); }  } while(0)
#define RET_CHECK_CANCEL_RET(ret, r)    do { if((ret) == R_CANCEL) { return (r); }  } while(0)
#define RET_CHECK_RET(ret, r)    do { if((ret) != R_SUCC) { return (r); }  } while(0)
#define RET_CHECK_RETV(ret)    do { if((ret) != R_SUCC) { return; }  } while(0)

#define RET_CANCEL_TO_DEF_RET(ret)   do { if((ret)==R_CANCEL) { return R_DEF; } else { return (ret); } }while(0)

// bit operator
#define FLAG_SET(f, b)   ((f)|=(b))
#define FLAG_CLR(f, b)   ((f)&=~(b))
#define IS_FLAG_SET(f, b)   (((f)&(b))==(b))


#define BIT_SET_N(f, n)      FLAG_SET((f),(1<<(n)))
#define BIT_CLR_N(f, n)      FLAG_CLR((f), (1<<(n)))
#define IS_BIT_SET_N(f, n)   IS_FLAG_SET((f), (1<<(n)))

#ifdef __cplusplus
}
#endif



#endif /* __CONFIG_H__ */


