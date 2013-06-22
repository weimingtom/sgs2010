#ifndef __CMD_H__
#define __CMD_H__

#include "game.h"




//typedef int (*FunCmdPerProc)(const char** argv, int argc, GameContext* pContext, void* ud);

// int cmd_loop(GameContext* pContext, const char* alter_text, FunCmdPerProc  funper, void* ud);

RESULT cmd_loop(GameContext* pContext, GameEventContext* pEvent, const char* strAlter);



#endif /* __CMD_H__ */


