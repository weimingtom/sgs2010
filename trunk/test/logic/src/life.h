#ifndef __LIFE_H__
#define __LIFE_H__

#include "config.h"

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


RESULT game_player_add_life(GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc);






#endif /* __LIFE_H__ */
