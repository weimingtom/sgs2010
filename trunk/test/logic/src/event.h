// event.h 


#ifndef __EVENT_H__
#define __EVENT_H__

#include "config.h"
#include "card.h"
#include "out.h"

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
	GameEvent_PerAddLife,
	GameEvent_PostAddLife,
	GameEvent_PerLostLife,
	GameEvent_PostLostLife,
	GameEvent_PerDead,
	GameEvent_Dead,
	GameEvent_PerKill,
	GameEvent_UseSkill,
	GameEvent_PostKill,
	GameEvent_PerCardJudge,
	GameEvent_PostCardJudge,
	GameEvent_RoundGetCard,
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PassiveGetCard,
	GameEvent_OutCardPrepare,
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

typedef struct tagPosCard
{
	Card card;
	int  where;
	int  pos;
}PosCard;



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
		Card*       pCard;       // for calc card, judge card ... 
		int*        pNum;        // num for get card, discard card. etc
		AttackDis*  pAttackDis; 
		PatternOut* pPatternOut; // for passive out, supply card etc..
 		OutCard*    pOut;       // real out,  per/post out
		PosCard*    pPosCard;  // lost card,
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

Card* get_event_card(GameEventContext* pEvent);
void set_card_id(Card* pCard, CardID id);
void set_card_color(Card* pCard, CardColor cl);
void set_card_value(Card* pCard, CardValue val);
void set_card_flag(Card* pCard, CardFlag flag);

OutCard* get_event_out(GameEventContext* pEvent);
void  set_event_out(GameEventContext* pEvent, OutCard* pOut);
void set_out_target(OutCard* pOut, int target);


// ...

// tolua_end


#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */

