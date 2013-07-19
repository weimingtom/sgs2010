// event.h 


#ifndef __EVENT_H__
#define __EVENT_H__

#include "config.h"
#include "card.h"
#include "out.h"
#include "equip.h"
#include "life.h"

#ifdef __cplusplus
extern "C" {
#endif


// foward declare

typedef struct tagGameContext GameContext;


// tolua_begin
enum GameEvent
{
	GameEvent_None = 0,     // no
	GameEvent_NewGame = 1,  // 开始新游戏
	GameEvent_LoadGame,     // 加载保存的进度
	GameEvent_RoundBegin,   // 回合开始时
	GameEvent_PerRoundJudge,    
	GameEvent_RoundJudgeCard,
	GameEvent_PostRoundJudge,
	GameEvent_PerRoundGet,
	GameEvent_RoundGetCard,
	GameEvent_PostRoundGet,
	GameEvent_PerRoundOut,
	GameEvent_RoundOutCard,
	GameEvent_PostRoundOut,
	GameEvent_PerRoundDiscard,
	GameEvent_PostRoundDiscard,
	GameEvent_RoundEnd,
	GameEvent_PerAttack,
	GameEvent_PostAttack,
	GameEvent_PerBeAttacked,
	GameEvent_PostBeAttacked,
	GameEvent_PerChangeLife,
	GameEvent_PostChangeLife,
	GameEvent_PerDead,
	GameEvent_Dead,
	GameEvent_PerKill,
	GameEvent_UseSkill,
	GameEvent_PostKill,
	//GameEvent_PerCardJudge,
	//GameEvent_PostCardJudge,
	GameEvent_PerJudgeCard,
	GameEvent_PostJudgeCard,
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PassiveGetCard,
	GameEvent_OutCardPrepare,
	GameEvent_PerOutCard,
	GameEvent_OutCard,
	GameEvent_PostOutCard,
	GameEvent_PerPassiveOutCard,
	GameEvent_PassiveOutCard,
	GameEvent_PostPassiveOutCard,
	GameEvent_PerEquipCard,
	GameEvent_PostEquipCard,
	GameEvent_PerLostCard,
	GameEvent_PostLostCard,
	GameEvent_PerDiscardCard,
	GameEvent_PostDiscardCard,
	GameEvent_HandCardEmpty,
	GameEvent_PerCardCalc,
	GameEvent_CardCalc,
	GameEvent_PostCardCalc,
	GameEvent_FiniCardCalc,
	GameEvent_SupplyCard,
	GameEvent_CalcAttackDis,
	GameEvent_SelectTarget,
	GameEvent_PerDecideCard,
	GameEvent_PerDecideCardCalc,
	GameEvent_PostDecideCard,

};


// tolua_end


// for create a new game

typedef struct tagNewGameConfig
{
	int players;
	int masters;
	int minsters;
	int spies;
	int mutineers;
}NewGameConfig;

// for get card
typedef struct tagGetCard
{
	int num;
} GetCard;

// for passive out, supply out
typedef struct tagPatternOut
{
	//YESNO          may_cancel;
	OutCardPattern pattern;
	OutCard        out; 
} PatternOut;



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
		const NewGameConfig* pNewGameConfig;
		const char* szFileName;
		//Card*       pCard;       // for calc card, judge card ... 
		GetCard*    pGetCard;        // num for get card, discard card. etc
		AttackDis*  pAttackDis; 
		PatternOut* pPatternOut; // for passive out, supply card etc..
 		OutCard*    pOut;       // real out,  per/post out
		PosCard*    pPosCard;  // lost card,
		EquipCard*  pEquipCard; // for per/post equip card
		ChangeLife* pChangeLife; // when life is changed 
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
RESULT trigger_player_event(GameContext* pGame, GameEventContext* pEvent, int player);

// tolua_begin

GameEvent get_event_id(GameEventContext* pEvent);
int get_event_trigger(GameEventContext* pEvent);
int get_event_target(GameEventContext* pEvent);
GameEventContext*  get_parent_event(GameEventContext* pEvent);
RESULT get_event_result(GameEventContext* pEvent);
void set_event_result(GameEventContext* pEvent, RESULT result);
YESNO  get_event_block(GameEventContext* pEvent);
void  set_event_block(GameEventContext* pEvent, YESNO  yesno);

//Card* get_event_card(GameEventContext* pEvent);

OutCard* get_event_out(GameEventContext* pEvent);

PosCard* get_event_poscard(GameEventContext* pEvent);
EquipCard* get_event_equipcard(GameEventContext* pEvent);
ChangeLife* get_event_changelife(GameEventContext* pEvent);

AttackDis* get_event_attackdis(GameEventContext* pEvent);




//void  set_event_out(GameEventContext* pEvent, OutCard* pOut);


// ...

// tolua_end


#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */

