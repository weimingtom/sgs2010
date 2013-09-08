// event.h 


#ifndef __EVENT_H__
#define __EVENT_H__

#include "config.h"
#include "card.h"

#ifdef __cplusplus
extern "C" {
#endif


// foward declare

typedef struct tagGameContext GameContext;


// tolua_begin

/*
  事件分为驱动型和触发型以及调用型，另有部分事件仅用于父事件，不会驱动也不会触发。
  在脚本中处理事件的原则：
  驱动型事件 - 它只会在特定的时间，在特定的角色和卡牌被调用，只需要编写event 过程即可，事件函数的返回值即驱动事件返回值。不需要设置事件中断和事件结果。
  触发型事件 - 它会在有角色，卡牌上触发，需要编写检测函数，can_out(手牌)，can_use(装备和技能)，并编写event函数。event的返回值忽略，一般返回R_DEF或者R_SUCC即可，
               如果特定的事件需要通过事件来改变接下来的动作。是通过event.block 和event.result 来控制的。
  调用型事件 - 它会在有角色，卡牌上触发，但不需要编写检测函数。当存在相应的event函数时，该函数即被调用。返回值的方式同触发型事件。
*/

typedef enum _GameEvent
{
	GameEvent_None = 0,         // no event
	GameEvent_NewGame = 1,      // 父事件：开始新游戏
	GameEvent_LoadGame,         // 父事件：加载保存的进度
	GameEvent_RoundBegin,       // 触发型：回合开始阶段
	GameEvent_PerRoundJudge,    // 触发型：回合判定阶段开始
	GameEvent_RoundJudgeCard,   // 父事件：回合判定阶段
	GameEvent_PostRoundJudge,   // 触发型：回合判定阶段结束
	GameEvent_PerRoundGet,      // 触发型：回合摸牌阶段开始
	GameEvent_RoundGetCard,     // 父事件：回合摸牌阶段
	GameEvent_PostRoundGet,     // 触发型：回合摸牌阶段结束
	GameEvent_PerRoundOut,      // 触发型：回合出牌阶段开始
	GameEvent_RoundOutCard,     // 父事件：回合出牌阶段
	GameEvent_PostRoundOut,     // 触发型：回合出牌阶段结束
	GameEvent_PerRoundDiscard,  // 触发型：回合弃牌阶段开始
	GameEvent_RoundDiscardCard, // 父事件：回合弃牌阶段
	GameEvent_PostRoundDiscard, // 触发型：回合弃牌阶段结束
	GameEvent_RoundEnd,         // 触发型：回合结束阶段
	//GameEvent_PerAttack,
	//GameEvent_PostAttack,
	//GameEvent_PerBeAttacked,
	//GameEvent_PostBeAttacked,
	GameEvent_PerChangeLife,    // 触发型：即将体力改变
	GameEvent_ChangeLife,       // 父事件：体力改变
	GameEvent_PostChangeLife,   // 触发型：体力改变之后
	GameEvent_PerDead,          // 触发型：即将死亡（濒死）
	GameEvent_Dead,             // 触发型：角色死亡
	GameEvent_PostDead,         // 触发型：角色死亡之后
	//GameEvent_PerSkill,        
	//GameEvent_UseSkill,
	//GameEvent_PostKill,
	//GameEvent_PerJudgeCard,     
	//GameEvent_PostJudgeCard,
	GameEvent_PerGetCard,        // 触发型：即将摸牌
	GameEvent_PostGetCard,       // 触发型：摸牌之后
	GameEvent_PassiveGetCard,    // 父事件：被动摸牌
	GameEvent_PerOutCardPrepare, // 触发型：即将出牌前的准备动作
	GameEvent_OutCardPrepare,    // 驱动型：（卡牌）执行出牌前的准备动作
	GameEvent_PerOutCard,        // 触发型：即将出牌（出牌完成准备，针对所有目标开始结算前）
	GameEvent_BeforeOutCardEffect, // 触发型：即将结算出牌效果（针对每个目标开始驱动前）
	GameEvent_OutCard,           // 驱动型：（卡牌）执行出牌动作
	GameEvent_PerOutCardCalc,    // 触发型：即将结算出牌效果（针对每个目标开始结算前）
	GameEvent_OutCardCalc,       // 驱动型：（卡牌）执行出牌成功后的结算动作
	GameEvent_PostOutCardCalc,   // 触发型：完成结算出牌效果之后（针对每个目标结算之后）
	GameEvent_PostOutCard,       // 触发型：出牌之后（出牌针对所有目标完成结算后）
	GameEvent_BeforePassiveOut,  // 触发型：即将进行被动出牌动作之前
	GameEvent_PerPassiveOutCard, // 触发型：即将被动出牌分解牌之前（如果是非固定样式出牌，则每个样式允许单独地出牌）
	GameEvent_PassiveOutCard,    // 父事件：被动出牌
	GameEvent_PostPassiveOutCard,// 触发型：被动出牌分解牌出牌之后（如果是非固定样式出牌，则每个样式允许单独地出牌）
	GameEvent_AfterPassiveOut,   // 触发型：进行被动出牌动作之后
	GameEvent_PerSupplyCard,     // 触发型：即将进行提供牌动作之前
	GameEvent_SupplyCard,        // 父事件：提供牌
	GameEvent_PostSupplyCard,    // 触发型：进行提供牌动作之后
	GameEvent_PerEquipCard,      // 触发型：即将进行装备牌动作之前
	GameEvent_PostEquipCard,     // 触发型：进行装备牌动作之后
	GameEvent_PerLostCard,       // 触发型：即将失去一张牌之前
	GameEvent_PostLostCard,      // 触发型：失去一张牌之后
	GameEvent_PerDiscardCard,    // 触发型：即将弃置一张牌之前
	GameEvent_PostDiscardCard,   // 触发型：弃置一张牌之后
	GameEvent_PerPassiveDiscardCard,  // 触发型：即将被动弃置一张牌之前
	GameEvent_PassiveDiscardCard,     // 父事件：被动弃置牌
	GameEvent_PostPassiveDiscardCard, // 触发型：被动弃置一张牌之后
	GameEvent_HandCardEmpty,      // 触发型：手牌空
	GameEvent_PerCardCalc,        // 触发型：（判定阶段）即将结算一张牌之前
	GameEvent_CardCalc,           // 驱动型：（判定阶段）（卡牌）执行结算动作
	GameEvent_PostCardCalc,       // 触发型：（判定阶段）结算一张牌之后
	GameEvent_FiniCardCalc,       // 触发型：（判定阶段）结算一张牌最终完成之后
	GameEvent_GetBaseAttackDis,   // 驱动型：计算武器的基本攻击距离
	GameEvent_CalcAttackDis,      // 触发型：计算攻击攻击距离和目标距离
	GameEvent_SelectTarget,       // 触发型：即将被选择为指定卡牌的目标的检查
	GameEvent_PerDecideCard,      // 触发型：准备进行一次判定之前（还没有从牌堆获得牌）
	GameEvent_PerDecideCardCalc,  // 触发型：准备计算判定结果之前（已经获得用于判定的牌）
	GameEvent_PostDecideCard,     // 触发型：判定一张牌之后
	GameEvent_CheckCardCanOut,    // 调用型：检查指定的牌当前是否可打出或使用
	GameEvent_CheckCardCanUse,    // 调用型：检查指定的（装备）牌当前是否可使用装备技能（或者触发作用效果）
	GameEvent_CheckSkillCanUse,   // 调用型：检查指定的武将技能当前是否可使用(或者触发作用效果)
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
	int ministers;
	int spies;
	int mutineers;
}NewGameConfig;

// tolua_end


typedef struct tagGameEventContext GameEventContext;


typedef struct tagGetCard GetCard;
typedef struct tagAttackDis AttackDis;
typedef struct tagBeforePassiveOut BeforePassiveOut;
typedef struct tagOutCardPattern OutCardPattern;
typedef struct tagPatternOut PatternOut;
typedef struct tagOutCard OutCard;
typedef struct tagDiscardPattern DiscardPattern;
typedef struct tagEquipCard EquipCard;
typedef struct tagChangeLife ChangeLife;
typedef struct tagSelectTarget SelectTarget;
typedef struct tagSkillCanUse SkillCanUse;
typedef struct tagCardCanUse CardCanUse;
typedef struct tagJudgeCard  JudgeCard;
typedef struct tagDecideCard  DecideCard;


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

		GetCard*    get_card;        // num for get card, discard card. etc
		AttackDis*  attack_dis; 
		BeforePassiveOut* before_pout; 
		OutCardPattern* card_pattern;
		PatternOut*  pattern_out; // for passive out, supply card etc..
 		OutCard*     out_card;       // real out,  per/post out
		DiscardPattern* discard_pattern; // when round discard card, or passive discard card
		//DiscardCard* discard_card;  // discard card
		PosCard*     pos_card;  // lost card,
		PosVCard*    pos_vcard;  // discard card,
		JudgeCard*   judge_card;  // judgement vcard,
		DecideCard*  decide_card;  // decide card
		EquipCard*   equip_card; // for per/post equip card
		ChangeLife*  change_life; // when life is changed 
		SelectTarget* select_target; // when select target checked
		SkillCanUse*  skill_canuse;
		CardCanUse*   card_canuse;
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

#define EVENT_CHECK_BREAK(event)   if((event)->block == YES) { break; } else {}
#define EVENT_CHECK_BREAK_RET(event, ret)   do { if((event)->block == YES) return (ret);  } while(0)
#define EVENT_CHECK_BREAK_BACK(event)   EVENT_CHECK_BREAK_RET((event), R_BACK)
#define EVENT_CHECK_BREAK_RETV(event)   do { if((event)->block == YES) return;  } while(0)


// 包装一个在指定事件下循环等待玩家指令的调用
RESULT player_cmd_event(GameContext* pGame, GameEventContext* pEvent, int player);


RESULT check_player_event(GameContext* pGame, GameEventContext* pEvent, int player, int auto_use);

// trigger event begin from current player, check and call all player's event handles 
RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent);
// trigger event in specified player, check and call the player's event handles d__
RESULT trigger_player_event(GameContext* pGame, GameEventContext* pEvent, int player);



// call all player's event handles without check. (some specified event)
RESULT call_game_event(GameContext* pGame, GameEventContext* pEvent);
//  call the specified player's event handles without check. (some specified event)
RESULT call_player_event(GameContext* pGame, GameEventContext* pEvent, int player);



// tolua_begin
const NewGameConfig* game_event_get_new_game_config(lua_State* L, tolua_notnull GameEventContext* pEvent);
const char* game_event_get_file_name(lua_State* L, tolua_notnull GameEventContext* pEvent);

GetCard* game_event_get_get_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
AttackDis* game_event_get_attack_dis(lua_State* L, tolua_notnull GameEventContext* pEvent);
BeforePassiveOut* game_event_get_before_pout(lua_State* L, tolua_notnull GameEventContext* pEvent);
OutCardPattern* game_event_get_card_pattern(lua_State* L, tolua_notnull GameEventContext* pEvent);
PatternOut* game_event_get_pattern_out(lua_State* L, tolua_notnull GameEventContext* pEvent);
OutCard* game_event_get_out_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
DiscardPattern* game_event_get_discard_pattern(lua_State* L, tolua_notnull GameEventContext* pEvent);
PosCard* game_event_get_pos_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
PosVCard* game_event_get_pos_vcard(lua_State* L, tolua_notnull GameEventContext* pEvent);
JudgeCard* game_event_get_judge_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
DecideCard* game_event_get_decide_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
EquipCard* game_event_get_equip_card(lua_State* L, tolua_notnull GameEventContext* pEvent);
ChangeLife* game_event_get_change_life(lua_State* L, tolua_notnull GameEventContext* pEvent);
SelectTarget* game_event_get_select_target(lua_State* L, tolua_notnull GameEventContext* pEvent);
SkillCanUse* game_event_get_skill_canuse(lua_State* L, tolua_notnull GameEventContext* pEvent);
CardCanUse* game_event_get_card_canuse(lua_State* L, tolua_notnull GameEventContext* pEvent);


// tolua_end



#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */

