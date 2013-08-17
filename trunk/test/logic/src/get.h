// get.h 

#ifndef  __GET_H__
#define  __GET_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;

// tolua_begin

// for get card
typedef struct tagGetCard
{
	int    num;
	YESNO  force;
	char   alter_text[MAX_ALTER_TEXT_LEN];
} GetCard;


// tolua_end


RESULT game_first_getcard(GameContext* pGame);

RESULT game_cmd_getcard(GameContext* pGame, GameEventContext* pEvent, int num);

RESULT game_round_do_get(GameContext* pGame, GameEventContext* pEvent, int player, int num);


// tolua_begin


RESULT game_passive_getcard(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, int num, YESNO force);

RESULT game_player_getcard_from_player(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, int from_player, CardWhere where, int pos);


// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __GET_H__ */



