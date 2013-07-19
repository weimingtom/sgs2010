#ifndef  __DECIDE_H__
#define  __DECIDE_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// tolua_begin

// get a card from stack and dicide it
YESNO game_decide_card(lua_State* L,  GameContext* pGame, GameEventContext* pParentEvent, int player, const char* szPattern);


// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __DECIDE_H__ */


