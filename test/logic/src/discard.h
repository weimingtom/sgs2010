#ifndef __DISCARD_H__
#define __DISCARD_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// tolua_begin


typedef struct tagDiscardPattern
{
	int    num;
	int    where;
	YESNO  force;
	char   alter_text[MAX_ALTER_TEXT_LEN];
} DiscardPattern;



// tolua_end




// operator of discard stack
RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard, int* pos);
//RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard);
//RESULT game_clr_discard_cur(GameContext* pGame, int pos);
void   game_flush_discard_cur(GameContext* pGame);





RESULT game_round_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player);

RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num);






// tolua_begin
// {{

// discard card
RESULT game_player_discard_card(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, CardWhere where, int pos);

RESULT game_passive_discard(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, int where, int num, YESNO force, const char* alter_text);
YESNO is_cur_card_valid(GameContext* pGame, CardWhere where, int pos);
RESULT add_cur_card_to_player_hand(lua_State* L, tolua_notnull GameContext* pGame, CardWhere where, int pos, int player);
RESULT add_cur_card_to_player_judgment(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull Card* v, tolua_notnull PosCardList* list, int player);

// 从摸牌堆获得一张牌并放在curdiscard顶，返回pos
int  game_pop_stack_card_dis(tolua_notnull GameContext* pGame);

Card*  game_get_discard_cur(tolua_notnull GameContext* pGame, int pos);

// }}
// tolua_end



#ifdef __cplusplus
}
#endif

#endif /* __DISCARD_H__ */


