// out.h 

#ifndef __OUT_H__
#define __OUT_H__

#include "config.h"
#include "card.h"


typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;



typedef struct tagPosCard
{
	Card card;
	int  where;
	int  pos;
}PosCard;

typedef struct tagOutCard OutCard;

#define MAX_RCARD_NUM  5

struct tagOutCard
{
	PosCard vcard;    //  use as card (virtaul) 
	int  nrcard;   //  real card number ,. if 0 vcard is also real card 
	PosCard rcards[MAX_RCARD_NUM];  // rcard array;
};



typedef struct tagOutCardPattern OutCardPattern;

struct tagOutCardPattern
{
	int num;
	CardPattern patterns[MAX_RCARD_NUM];
};



#define INIT_CARDPATTERN_USE_ID(cp, _id)  ((cp)->id=(_id), (cp)->color=CardColor_None, (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_COLOR(cp, _c)  ((cp)->id=CardID_None, (cp)->color=(c), (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_VALUE(cp, _v)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v), (cp)->value_max=(_v))
#define INIT_CARDPATTERN_USE_VALUE_RANGE(cp, _v1, _v2)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v1), (cp)->value_max=(_v2))



RESULT game_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
RESULT game_pass(GameContext* pGame, GameEventContext* pEvent);




#endif /* __OUT_H__ */



