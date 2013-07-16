#ifndef __LIFE_H__
#define __LIFE_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	
	




typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


// tolua_begin

RESULT game_player_add_life(GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc);



// tolua_end


#ifdef __cplusplus
}
#endif


#endif /* __LIFE_H__ */
