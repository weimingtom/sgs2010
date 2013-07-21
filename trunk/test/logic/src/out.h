// out.h 

#ifndef __OUT_H__
#define __OUT_H__

#include "config.h"
#include "card.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	
	

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;

// tolua_begin

typedef struct tagOutCard
{
	int      trigger;
	int      supply;
	int      target;
	Card     vcard;    //  use as card (virtaul) 
	PosCardList list;
} OutCard;



typedef struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill/weapon skill/armor skill to generate 
	int         num;      // pattern count
	CardPattern patterns[MAX_CARD_LIST_NUM];
} OutCardPattern;

// tolua_end



// out card command process
RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
// pass command process
RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent);
// 
RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player);






// tolua_begin

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* out_card);

// trigger supply from player the card match the  pattern, return through out_card
RESULT game_supply_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const char* pattern, const char* alter_text, OutCard* out_card);

// passive out process
RESULT game_passive_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player,  int target, const char* pattern, const char* alter_text);



// tolua_end

#ifdef __cplusplus
}
#endif



#endif /* __OUT_H__ */



