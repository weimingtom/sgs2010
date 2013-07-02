#ifndef __INFO_H__
#define __INFO_H__


#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent);
RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent);
RESULT game_other_player_info(GameContext* pGame, GameEventContext* pEvent, int player);


#ifdef __cplusplus
}
#endif

#endif

