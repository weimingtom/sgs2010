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

// tolua_begin

typedef struct tagOutCard
{
	int      trigger;
	int      supply;
	int      target_num;
	int      targets[MAX_PLAYER_NUM];
	int      flag;
	Card     vcard;    //  use as card (virtual) real out card in list following
	PosCardList list;
	char     message[MAX_ALTER_TEXT_LEN];
	char     ud[MAX_UD_LEN];    // 出牌的时候些可能需要在各事件之间传递一些参数
} OutCard;

#define MAX_PATTERN_UD_LEN     256

typedef struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill/weapon skill/armor skill to generate 
	int         num;      // pattern count
	CardPattern patterns[MAX_CARD_LIST_NUM];
	char        ud[MAX_PATTERN_UD_LEN];     // 用于脚本通过pattern在各脚本间传递一些暗示信息
} OutCardPattern;


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


typedef enum enOutCardFlag
{
	OutCardFlag_None = 0,
	OutCardFlag_SpecOut = 1,   // 指定的出牌
	OutCardFlag_WithTarget= 2,   // 指定目标
	OutCardFlag_NoCheckDis= 4,   // 忽略攻击距离
	OutCardFlag_NoCheckTarget= 8,   // 忽略攻击目标条件检查
}OutCardFlag;

// tolua_end



///////////////////////////////////////////////////////////////////////////////////////
// c interface

// out card command process
RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent, int* idx, int num);
// pass command process
RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent);
// 
RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player);





///////////////////////////////////////////////////////////////////////////////////////
// lua interface

// tolua_begin

YESNO game_card_can_out(GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pPosCard);


void game_load_out_pattern(lua_State* L, tolua_notnull OutCardPattern* out_pattern, tolua_notnull const char* s_pattern);


RESULT game_real_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull OutCard* out_card);

// trigger supply from player the card match the  pattern, return through out_card
RESULT game_supply_card(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int trigger, int player
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text, tolua_notnull OutCard* out_card);

// passive out process
RESULT game_passive_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player,  int target
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text);

void game_init_outcard(tolua_notnull OutCard* out_card);

/*
 特殊出牌需要2步完成，这里不再提供函数方式
 1、使用game_supply_card 打出指定格式的牌组合
 2、修正虚拟牌或者指定目标，设置Flag
 3、调用game_real_out，走正常的出牌流程
*/

//RESULT game_spec_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player,  int targets[10]
//						, OutCardPattern* out_pattern, const char* alter_text);


// tolua_end

#ifdef __cplusplus
}
#endif



#endif /* __OUT_H__ */



