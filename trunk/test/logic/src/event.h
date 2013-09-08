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
  �¼���Ϊ�����ͺʹ������Լ������ͣ����в����¼������ڸ��¼�����������Ҳ���ᴥ����
  �ڽű��д����¼���ԭ��
  �������¼� - ��ֻ�����ض���ʱ�䣬���ض��Ľ�ɫ�Ϳ��Ʊ����ã�ֻ��Ҫ��дevent ���̼��ɣ��¼������ķ���ֵ�������¼�����ֵ������Ҫ�����¼��жϺ��¼������
  �������¼� - �������н�ɫ�������ϴ�������Ҫ��д��⺯����can_out(����)��can_use(װ���ͼ���)������дevent������event�ķ���ֵ���ԣ�һ�㷵��R_DEF����R_SUCC���ɣ�
               ����ض����¼���Ҫͨ���¼����ı�������Ķ�������ͨ��event.block ��event.result �����Ƶġ�
  �������¼� - �������н�ɫ�������ϴ�����������Ҫ��д��⺯������������Ӧ��event����ʱ���ú����������á�����ֵ�ķ�ʽͬ�������¼���
*/

typedef enum _GameEvent
{
	GameEvent_None = 0,         // no event
	GameEvent_NewGame = 1,      // ���¼�����ʼ����Ϸ
	GameEvent_LoadGame,         // ���¼������ر���Ľ���
	GameEvent_RoundBegin,       // �����ͣ��غϿ�ʼ�׶�
	GameEvent_PerRoundJudge,    // �����ͣ��غ��ж��׶ο�ʼ
	GameEvent_RoundJudgeCard,   // ���¼����غ��ж��׶�
	GameEvent_PostRoundJudge,   // �����ͣ��غ��ж��׶ν���
	GameEvent_PerRoundGet,      // �����ͣ��غ����ƽ׶ο�ʼ
	GameEvent_RoundGetCard,     // ���¼����غ����ƽ׶�
	GameEvent_PostRoundGet,     // �����ͣ��غ����ƽ׶ν���
	GameEvent_PerRoundOut,      // �����ͣ��غϳ��ƽ׶ο�ʼ
	GameEvent_RoundOutCard,     // ���¼����غϳ��ƽ׶�
	GameEvent_PostRoundOut,     // �����ͣ��غϳ��ƽ׶ν���
	GameEvent_PerRoundDiscard,  // �����ͣ��غ����ƽ׶ο�ʼ
	GameEvent_RoundDiscardCard, // ���¼����غ����ƽ׶�
	GameEvent_PostRoundDiscard, // �����ͣ��غ����ƽ׶ν���
	GameEvent_RoundEnd,         // �����ͣ��غϽ����׶�
	//GameEvent_PerAttack,
	//GameEvent_PostAttack,
	//GameEvent_PerBeAttacked,
	//GameEvent_PostBeAttacked,
	GameEvent_PerChangeLife,    // �����ͣ����������ı�
	GameEvent_ChangeLife,       // ���¼��������ı�
	GameEvent_PostChangeLife,   // �����ͣ������ı�֮��
	GameEvent_PerDead,          // �����ͣ�����������������
	GameEvent_Dead,             // �����ͣ���ɫ����
	GameEvent_PostDead,         // �����ͣ���ɫ����֮��
	//GameEvent_PerSkill,        
	//GameEvent_UseSkill,
	//GameEvent_PostKill,
	//GameEvent_PerJudgeCard,     
	//GameEvent_PostJudgeCard,
	GameEvent_PerGetCard,        // �����ͣ���������
	GameEvent_PostGetCard,       // �����ͣ�����֮��
	GameEvent_PassiveGetCard,    // ���¼�����������
	GameEvent_PerOutCardPrepare, // �����ͣ���������ǰ��׼������
	GameEvent_OutCardPrepare,    // �����ͣ������ƣ�ִ�г���ǰ��׼������
	GameEvent_PerOutCard,        // �����ͣ��������ƣ��������׼�����������Ŀ�꿪ʼ����ǰ��
	GameEvent_BeforeOutCardEffect, // �����ͣ������������Ч�������ÿ��Ŀ�꿪ʼ����ǰ��
	GameEvent_OutCard,           // �����ͣ������ƣ�ִ�г��ƶ���
	GameEvent_PerOutCardCalc,    // �����ͣ������������Ч�������ÿ��Ŀ�꿪ʼ����ǰ��
	GameEvent_OutCardCalc,       // �����ͣ������ƣ�ִ�г��Ƴɹ���Ľ��㶯��
	GameEvent_PostOutCardCalc,   // �����ͣ���ɽ������Ч��֮�����ÿ��Ŀ�����֮��
	GameEvent_PostOutCard,       // �����ͣ�����֮�󣨳����������Ŀ����ɽ����
	GameEvent_BeforePassiveOut,  // �����ͣ��������б������ƶ���֮ǰ
	GameEvent_PerPassiveOutCard, // �����ͣ������������Ʒֽ���֮ǰ������Ƿǹ̶���ʽ���ƣ���ÿ����ʽ�������س��ƣ�
	GameEvent_PassiveOutCard,    // ���¼�����������
	GameEvent_PostPassiveOutCard,// �����ͣ��������Ʒֽ��Ƴ���֮������Ƿǹ̶���ʽ���ƣ���ÿ����ʽ�������س��ƣ�
	GameEvent_AfterPassiveOut,   // �����ͣ����б������ƶ���֮��
	GameEvent_PerSupplyCard,     // �����ͣ����������ṩ�ƶ���֮ǰ
	GameEvent_SupplyCard,        // ���¼����ṩ��
	GameEvent_PostSupplyCard,    // �����ͣ������ṩ�ƶ���֮��
	GameEvent_PerEquipCard,      // �����ͣ���������װ���ƶ���֮ǰ
	GameEvent_PostEquipCard,     // �����ͣ�����װ���ƶ���֮��
	GameEvent_PerLostCard,       // �����ͣ�����ʧȥһ����֮ǰ
	GameEvent_PostLostCard,      // �����ͣ�ʧȥһ����֮��
	GameEvent_PerDiscardCard,    // �����ͣ���������һ����֮ǰ
	GameEvent_PostDiscardCard,   // �����ͣ�����һ����֮��
	GameEvent_PerPassiveDiscardCard,  // �����ͣ�������������һ����֮ǰ
	GameEvent_PassiveDiscardCard,     // ���¼�������������
	GameEvent_PostPassiveDiscardCard, // �����ͣ���������һ����֮��
	GameEvent_HandCardEmpty,      // �����ͣ����ƿ�
	GameEvent_PerCardCalc,        // �����ͣ����ж��׶Σ���������һ����֮ǰ
	GameEvent_CardCalc,           // �����ͣ����ж��׶Σ������ƣ�ִ�н��㶯��
	GameEvent_PostCardCalc,       // �����ͣ����ж��׶Σ�����һ����֮��
	GameEvent_FiniCardCalc,       // �����ͣ����ж��׶Σ�����һ�����������֮��
	GameEvent_GetBaseAttackDis,   // �����ͣ����������Ļ�����������
	GameEvent_CalcAttackDis,      // �����ͣ����㹥�����������Ŀ�����
	GameEvent_SelectTarget,       // �����ͣ�������ѡ��Ϊָ�����Ƶ�Ŀ��ļ��
	GameEvent_PerDecideCard,      // �����ͣ�׼������һ���ж�֮ǰ����û�д��ƶѻ���ƣ�
	GameEvent_PerDecideCardCalc,  // �����ͣ�׼�������ж����֮ǰ���Ѿ���������ж����ƣ�
	GameEvent_PostDecideCard,     // �����ͣ��ж�һ����֮��
	GameEvent_CheckCardCanOut,    // �����ͣ����ָ�����Ƶ�ǰ�Ƿ�ɴ����ʹ��
	GameEvent_CheckCardCanUse,    // �����ͣ����ָ���ģ�װ�����Ƶ�ǰ�Ƿ��ʹ��װ�����ܣ����ߴ�������Ч����
	GameEvent_CheckSkillCanUse,   // �����ͣ����ָ�����佫���ܵ�ǰ�Ƿ��ʹ��(���ߴ�������Ч��)
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


// ��װһ����ָ���¼���ѭ���ȴ����ָ��ĵ���
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

