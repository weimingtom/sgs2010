#ifndef __INFO_H__
#define __INFO_H__


#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// tolua_begin

RESULT game_cur_info(tolua_notnull GameContext* pGame,tolua_notnull  GameEventContext* pEvent);
RESULT game_global_info(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent);
RESULT game_other_player_info(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player);

RESULT game_event_info(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int detail);
// tolua_end

#ifdef __cplusplus
}
#endif

#endif


