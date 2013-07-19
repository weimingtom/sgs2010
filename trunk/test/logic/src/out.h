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


typedef struct tagOutCard OutCard;

struct tagOutCard
{
	int      trigger;
	int      supply;
	int      target;
	Card     vcard;    //  use as card (virtaul) 
	PosCardList list;
};



typedef struct tagOutCardPattern OutCardPattern;

struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill/weapon skill/armor skill to generate 
	int         num;      // pattern count
	CardPattern patterns[MAX_CARD_LIST_NUM];
};





// out card command process
RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
// pass command process
RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent);
// 
RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player);

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* pOut);



// tolua_begin

int get_outcard_trigger(OutCard* pOutCard);
int get_outcard_supply(OutCard* pOutCard);
int get_outcard_target(OutCard* pOutCard);
Card* get_outcard_vcard(OutCard* pOutCard);
int get_outcard_rcard_num(OutCard* pOutCard);
Card* get_outcard_rcard(OutCard* pOutCard, int index);
CardWhere get_outcard_rcard_where(OutCard* pOutCard, int index);
int get_outcard_rcard_pos(OutCard* pOutCard, int index);


int get_outpattern_where(OutCardPattern* pOutPattern);
YESNO get_outpattern_fixed(OutCardPattern* pOutPattern);
int get_outpattern_num(OutCardPattern* pOutPattern);
CardPattern* get_outpattern_pattern(OutCardPattern* pOutPattern, int index);


void set_out_target(OutCard* pOut, int target);

// tolua_end



// tolua_begin

// trigger supply from player the card match the  pattern, return through pOut
RESULT game_supply_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const char* pattern, const char* alter_text, OutCard* pOut);

// passive out process
RESULT game_passive_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player,  int target, const char* pattern, const char* alter_text);



// tolua_end

#ifdef __cplusplus
}
#endif



#endif /* __OUT_H__ */



