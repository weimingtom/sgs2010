#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <malloc.h>


#define PROJ_NAME    "SGS. Test Logic"
#define VERSION_STR  "v0.1.0"

enum YESNO
{
	NO = 0,
	YES = 1,
};


#define  MAX_NAME_LEN    32
#define  MAX_DESC_LEN    512
#define  MAX_PLAYER_NUM  10

#ifndef COUNT
#define COUNT(a)  (sizeof(a)/sizeof((a)[0]))
#endif


#ifndef OFFSET
#define OFFSET(st,m)   ((size_t)(((st*)0)->m))
#endif


#define ST_ZREO(st)   (memset(&st, 0, sizeof(st)))

#ifdef MIN 
#undef MIN
#endif

#ifdef MAX 
#undef MAX
#endif


#define MIN(a,b)   ((a)<(b)?(a):(b))
#define MAX(a,b)   ((a)>(b)?(a):(b))


#endif /* __CONFIG_H__ */


