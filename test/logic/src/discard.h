#ifndef __DISCARD_H__
#define __DISCARD_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;



// operator of discard stack
RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard, int* pos);
RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard);
RESULT game_clr_discard_cur(GameContext* pGame, int pos);
void game_flush_discard_cur(GameContext* pGame);





RESULT game_round_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player);

RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num);



// discard card
RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, CardWhere where, int pos);



// tolua_begin
// {{

RESULT game_passive_discard(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int where, int num, YESNO can_cancel, const char* alter_text);
YESNO is_cur_card_valid(GameContext* pGame, CardWhere where, int pos);
RESULT add_cur_card_to_player_hand(GameContext* pGame, CardWhere where, int pos, int player);
RESULT add_cur_card_to_player_judgment(GameContext* pGame, CardWhere where, int pos, int player);
// }}
// tolua_end



#ifdef __cplusplus
}
#endif

#endif /* __DISCARD_H__ */


