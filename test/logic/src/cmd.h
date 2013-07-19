#ifndef __CMD_H__
#define __CMD_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


#define MAX_SEL_OPTION_LEN       256
#define MAX_SEL_INPUT_LEN        128

typedef struct tagSelOption SelOption;


typedef struct tagSelOption
{
	char text[MAX_SEL_OPTION_LEN];
	// input string is multi strings, divide by '\0', the twice 0 is used to end of string group. 
	// if the first string in group is empty, means use value to select
	char input[MAX_SEL_INPUT_LEN];
	// when select finished value is return through out_value.
	int  value;
} SelOption;


//typedef int (*FunCmdPerProc)(const char** argv, int argc, GameContext* pContext, void* ud);

// int cmd_loop(GameContext* pContext, const char* alter_text, FunCmdPerProc  funper, void* ud);

RESULT cmd_loop(GameContext* pContext, GameEventContext* pEvent, const char* strAlter);

RESULT select_loop(GameContext* pContext, GameEventContext* pEvent, const SelOption options[], int optnum, const char* strAlter, int* out_value);


#ifdef __cplusplus
}
#endif

#endif /* __CMD_H__ */


