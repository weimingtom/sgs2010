#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>


#define PROJ_NAME    "SGS. Test Logic"
#define VERSION_STR  "v0.1.0"


#define  MAX_NAME_LEN    32
#define  MAX_PLAYER_NUM  10

#ifndef COUNT
#define COUNT(a)  (sizeof(a)/sizeof((a)[0]))
#endif


#ifndef OFFSET
#define OFFSET(st,m)   ((size_t)(((st*)0)->m))
#endif


#define ST_ZREO(st)   (memset(&st, 0, sizeof(st)))


#endif /* __CONFIG_H__ */


