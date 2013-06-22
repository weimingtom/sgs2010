// event.h 


#ifndef __EVENT_H__
#define __EVENT_H__

#include "config.h"
#include "card.h"
#include "out.h"

// foward declare

typedef struct tagGameContext GameContext;



enum GameEvent
{
	GameEvent_None = 0,     // no
	GameEvent_NewGame = 1,  // 开始新游戏
	GameEvent_RoundBegin,   // 回合开始时
	GameEvent_PerRoundJudge,    
	GameEvent_PostRoundJudge,
	GameEvent_PerJudgeCard,
	GameEvent_PostJudgeCard,
	GameEvent_PerRoundGet,
	GameEvent_PostRoundGet,
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PerRoundOut,
	GameEvent_PostRoundOut,
	GameEvent_PerOutCard,
	GameEvent_PostOutCard,
	GameEvent_PerCardCalc,
	GameEvent_CardCalc,
	GameEvent_PostCardCalc,
	GameEvent_PerRoundDiscard,
	GameEvent_PostRoundDiscard,
	GameEvent_PerDiscardCard,
	GameEvent_PostDiscardCard,
	GameEvent_RoundEnd,
	GameEvent_PerLostCard,
	GameEvent_PostLostCard,
	GameEvent_HandCardEmpty,
	GameEvent_PerAttack,
	GameEvent_PostAttack,
	GameEvent_PerBeAttacked,
	GameEvent_PostBeAttacked,
	GameEvent_PerLostLife,
	GameEvent_PostLostLife,
	GameEvent_LostOneLife,
	GameEvent_PerDie,
	GameEvent_PostDie,
	GameEvent_PerKill,
	GameEvent_UseSkill,
	GameEvent_PostKill,
	GameEvent_PerCardJudge,    
	GameEvent_PostCardJudge,
	GameEvent_PerCardJudgeCalc,    
	GameEvent_PostCardJudgeCalc,
	GameEvent_PassiveGetCard,
	GameEvent_OutCardCheck,
	GameEvent_PassiveOutCard,
	GameEvent_SupplyCard,
};

/*
enum EventResult
{
	Result_None = 0,
	Result_Yes = 1,
	Result_No  = 2,
	Result_Cancel = 3,
};
*/

typedef struct tagGameEventContext GameEventContext;


struct tagGameEventContext
{
	int      id;
	int      trigger;
	int      target;
	GameEventContext* parent_event;
	RESULT   result;
	int		 block;
	union {
		Card     card;   // for 
		OutCard  out;             // for out card 
		OutCardPattern pattern;   // for passive out, supply card, etc...
		int      num;    // num for get card, discard card. etc
	};
};


#define INIT_EVENT(event, eid, tr, tg, p)   \
	do { \
	memset((event), 0, sizeof(*event)); \
	(event)->id = (eid); \
	(event)->trigger = (tr); \
	(event)->target = (tg); \
	(event)->parent_event = (p); \
	(event)->block = 0; \
	(event)->result = R_DEF; \
} while(0)





RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent);




#endif /* __EVENT_H__ */

