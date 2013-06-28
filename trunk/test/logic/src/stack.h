#ifndef __STACK_H__
#define __STACK_H__

#include "config.h"

typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;

// operator of get card stack

RESULT game_pop_stack_card(GameContext* pGame, Card* pCard);
RESULT game_push_stack_card(GameContext* pGame, const Card* pCard, YESNO  top);



#endif /* __STACK_H__ */


