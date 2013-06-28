#ifndef __DISCARD_H__
#define __DISCARD_H__

#include "config.h"

typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;



// operator of discard stack
RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard);
RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard);
RESULT game_clr_discard_cur(GameContext* pGame, int pos);
void game_flush_discard_cur(GameContext* pGame);


RESULT game_round_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player);

RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num);

// discard card
RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int where, int pos);


#endif /* __DISCARD_H__ */


