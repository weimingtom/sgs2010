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
	GameEvent_FiniCardCalc,
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
	GameEvent_RoundGetCard,
	GameEvent_PassiveGetCard,
	GameEvent_OutCardCheck,
	GameEvent_RoundOutCard,
	GameEvent_PassiveOutCard,
	GameEvent_SupplyCard,
	GameEvent_CalcAttackDis,
	GameEvent_SelectTarget,
	GameEvent_CheckTarget,
};


// for passive out, supply out
typedef struct tagPassiveOut
{
	YESNO          may_cancel;
	OutCardPattern pattern;
	OutCard        out; 
} PassiveOut;

typedef struct tagAttackDis
{
	int     base;
	int     inc;
}AttackDis;

typedef struct tagGameEventContext GameEventContext;


struct tagGameEventContext
{
	int      id;
	int      trigger;
	int      target;
	GameEventContext* parent_event;
	RESULT   result;
	YESNO	 block;
	union {
		Card*       pCard;   // for calc card, judge card ... 
		int*        pNum;    // num for get card, discard card. etc
		AttackDis*  pAttackDis; 
		PassiveOut* pPassiveOut; 
		OutCard*    pOut;    // real out
	};
};


#define INIT_EVENT(event, eid, tr, tg, p)   \
	do { \
	ST_ZERO(*(event)); \
	(event)->id = (eid); \
	(event)->trigger = (tr); \
	(event)->target = (tg); \
	(event)->parent_event = (p); \
	(event)->block = NO; \
	(event)->result = R_DEF; \
} while(0)





RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent);




#endif /* __EVENT_H__ */

