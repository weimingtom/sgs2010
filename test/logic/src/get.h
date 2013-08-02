// get.h 

#ifndef  __GET_H__
#define  __GET_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


RESULT game_first_getcard(GameContext* pGame);

RESULT game_cmd_getcard(GameContext* pGame, GameEventContext* pEvent, int num);

RESULT game_round_do_get(GameContext* pGame, GameEventContext* pEvent, int player, int num);


// tolua_begin
RESULT game_passive_getcard(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int num, YESNO force);

// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __GET_H__ */



