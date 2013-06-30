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




#define MAX_CARD_LIST_NUM  20

typedef struct tagCardList CardList;

struct tagCardList
{
	int    num;
	Card   cards[MAX_CARD_LIST_NUM];
};


typedef struct tagOutCard OutCard;


struct tagOutCard
{
	int      trigger;
	int      supply;
	int      target;
	Card     vcard;    //  use as card (virtaul) 
	CardList list;
	//int     nrcard;   //  real card number ,. if 0 vcard is also real card 
	//Card    rcards[MAX_RCARD_NUM];  // rcard array;
};



typedef struct tagOutCardPattern OutCardPattern;

struct tagOutCardPattern
{
	int where;
	int num;
	CardPattern patterns[MAX_CARD_LIST_NUM];
};



#define INIT_CARDPATTERN_USE_ID(cp, _id)  ((cp)->id=(_id), (cp)->color=CardColor_None, (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_COLOR(cp, _c)  ((cp)->id=CardID_None, (cp)->color=(c), (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_VALUE(cp, _v)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v), (cp)->value_max=(_v))
#define INIT_CARDPATTERN_USE_VALUE_RANGE(cp, _v1, _v2)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v1), (cp)->value_max=(_v2))


// out card command process
RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
// pass command process
RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent);
// 
RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player);

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* pOut);


// trigger supply from player the card match the  pattern, return through pOut
RESULT game_supply_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const OutCardPattern* pattern, const char* alter_text, OutCard* pOut);

// passive out process
RESULT game_passive_out(GameContext* pGame, GameEventContext* pParentEvent, int player,  int target, const OutCardPattern* pattern, const char* alter_text);


#ifdef __cplusplus
}
#endif



#endif /* __OUT_H__ */



