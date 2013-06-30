#ifndef  __DECIDE_H__
#define  __DECIDE_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


// get a card from stack and dicide it
YESNO game_decide_card(GameContext* pGame, GameEventContext* pParentEvent, int player, const CardPattern* pPattern);

#ifdef __cplusplus
}
#endif

#endif /* __DECIDE_H__ */


