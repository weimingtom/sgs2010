#ifndef __CMD_H__
#define __CMD_H__

#include "game.h"



#define CMD_RET_BACK  -2
#define CMD_RET_EXIT  -1
#define CMD_RET_SUCC   0
#define CMD_RET_DEF    1


typedef int (*FunCmdPerProc)(const char** argv, int argc, GameContext* pContext, void* ud);

int cmd_loop(GameContext* pContext, const char* alter_text, FunCmdPerProc  funper, void* ud);




#endif /* __CMD_H__ */


