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
typedef enum _GameEvent
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
	GameEvent_RoundDiscardCard,
	GameEvent_PostRoundDiscard,
	GameEvent_RoundEnd,
	GameEvent_PerAttack,
	GameEvent_PostAttack,
	GameEvent_PerBeAttacked,
	GameEvent_PostBeAttacked,
	GameEvent_PerChangeLife,
	GameEvent_ChangeLife,
	GameEvent_PostChangeLife,
	GameEvent_PerDead,
	GameEvent_Dead,
	GameEvent_PostDead,
	GameEvent_PerSkill,
	GameEvent_UseSkill,
	GameEvent_PostKill,
	GameEvent_PerJudgeCard,
	GameEvent_PostJudgeCard,
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PassiveGetCard,
	GameEvent_PerOutCardPrepare,
	GameEvent_OutCardPrepare,
	GameEvent_PerOutCard,
	GameEvent_OutCard,
	GameEvent_PostOutCard,
	GameEvent_PerOutCardCalc,
	GameEvent_OutCardCalc,
	GameEvent_PostOutCardCalc,
	GameEvent_BeforePassiveOut,
	GameEvent_PerPassiveOutCard,
	GameEvent_PassiveOutCard,
	GameEvent_PostPassiveOutCard,
	GameEvent_AfterPassiveOut,
	GameEvent_PerSupplyCard,
	GameEvent_SupplyCard,
	GameEvent_PostSupplyCard,
	GameEvent_PerEquipCard,
	GameEvent_PostEquipCard,
	GameEvent_PerLostCard,
	GameEvent_PostLostCard,
	GameEvent_PerDiscardCard,
	GameEvent_PostDiscardCard,
	GameEvent_PerPassiveDiscardCard,
	GameEvent_PassiveDiscardCard,
	GameEvent_PostPassiveDiscardCard,
	GameEvent_HandCardEmpty,
	GameEvent_PerCardCalc,
	GameEvent_CardCalc,
	GameEvent_PostCardCalc,
	GameEvent_FiniCardCalc,
	GameEvent_CalcAttackDis,
	GameEvent_SelectTarget,
	GameEvent_PerDecideCard,
	GameEvent_PerDecideCardCalc,
	GameEvent_PostDecideCard,
} GameEvent;


typedef enum _AttackDisFlag
{
	AttackDisFlag_UseWeapon = 1,
	AttackDisFlag_UseHorse = 2,
} AttackDisFlag;


// tolua_end


// tolua_begin


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
	int    num;
	YESNO  force;
	char   alter_text[MAX_ALTER_TEXT_LEN];
} GetCard;

// for passive out, supply out
typedef struct tagPatternOut
{
	OutCardPattern pattern;
	OutCard        out;
	char           alter_text[MAX_ALTER_TEXT_LEN];
} PatternOut;

// for before passive out, modify the pattern and alter taxt

typedef struct tagBeforePassiveOut
{
	OutCardPattern pattern;
	char alter_text[MAX_ALTER_TEXT_LEN];
}BeforePassiveOut;


typedef struct tagAttackDis
{
	int     base;
	int     inc;
	int     dis;
	int     flag;
}AttackDis;


typedef struct tagDiscardCard
{
	int    num;
	int    where;
	YESNO  force;
	char   alter_text[MAX_ALTER_TEXT_LEN];
} DiscardCard;

// tolua_end


typedef struct tagGameEventContext GameEventContext;




struct tagGameEventContext
{
	GameEvent  id;
	int        trigger;
	int        target;
	GameEventContext* parent_event;
	RESULT     result;
	YESNO	   block;
	union {
		const NewGameConfig* new_game_config;
		const char* file_name;
		//Card*       card;       // for calc card, judge card ... 
		GetCard*    get_card;        // num for get card, discard card. etc
		AttackDis*  attack_dis; 
		BeforePassiveOut* before_pout; 
		OutCardPattern* card_pattern;
		PatternOut* pattern_out; // for passive out, supply card etc..
 		OutCard*    out_card;       // real out,  per/post out
		PosCard*    pos_card;  // lost card,
		EquipCard*  equip_card; // for per/post equip card
		ChangeLife* change_life; // when life is changed 
		DiscardCard* discard_card; // when round discard card, or passive discard card
	};
	char ud[MAX_UD_LEN];   // user custom string, used for script
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
	if (p) { strncpy((event)->ud, (p)->ud, sizeof((event)->ud)); } \
} while(0)




RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent);
RESULT trigger_player_event(GameContext* pGame, GameEventContext* pEvent, int player);

// tolua_begin


// ...

// tolua_end


// wrapped function from lua
char* get_event_str(GameEvent eid, char* buf, int buflen);


#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */

