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
	GameEvent_PerRoundOut,
	GameEvent_PostRoundOut,
	GameEvent_PerRoundDiscard,
	GameEvent_PostRoundDiscard,
	GameEvent_PerDiscardCard,
	GameEvent_PostDiscardCard,
	GameEvent_RoundEnd,
	GameEvent_PerAttack,
	GameEvent_PostAttack,
	GameEvent_PerBeAttacked,
	GameEvent_PostBeAttacked,
	GameEvent_PerLostLife,
	GameEvent_PostLostLife,
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
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PassiveGetCard,
	GameEvent_TargetFilter,
	GameEvent_OutCardCheck,
	GameEvent_OutCard,
	GameEvent_RoundOutCard,
	GameEvent_PerOutCard,
	GameEvent_PostOutCard,
	GameEvent_PerLostCard,
	GameEvent_PostLostCard,
	GameEvent_HandCardEmpty,
	GameEvent_PerCardCalc,
	GameEvent_CardCalc,
	GameEvent_PostCardCalc,
	GameEvent_FiniCardCalc,
	GameEvent_PerPassiveOutCard,
	GameEvent_PassiveOutCard,
	GameEvent_PostPassiveOutCard,
	GameEvent_SupplyCard,
	GameEvent_CalcAttackDis,
	GameEvent_SelectTarget,
	GameEvent_CheckTarget,
};


// for passive out, supply out
typedef struct tagPassiveOut
{
	//YESNO          may_cancel;
	OutCardPattern pattern;
	OutCard        out; 
} PassiveOut;


enum AttackDisFlag
{
	AttackDisFlag_UseWeapon = 1,
	AttackDisFlag_UseHorse = 2,
};

typedef struct tagAttackDis
{
	int     base;
	int     inc;
	int     dis;
	int     flag;
}AttackDis;

typedef struct tagGameEventContext GameEventContext;


struct tagGameEventContext
{
	GameEvent      id;
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

