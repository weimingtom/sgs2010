#ifndef __EQUIP_H__
#define __EQUIP_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


RESULT game_player_equip_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int pos, Card* pCard);



#ifdef __cplusplus
}
#endif

#endif /* __EQUIP_H__ */


