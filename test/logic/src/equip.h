#ifndef __EQUIP_H__
#define __EQUIP_H__

#include "config.h"

typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


RESULT game_player_equip_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int pos, Card* pCard);




#endif /* __EQUIP_H__ */


