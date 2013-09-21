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
	int      cur_target;  // 当前目标，用于SetTarget会修改当前目标
	Card     vcard;    //  use as card (virtual) real out card in list following
	PosCardList list;
	char     message[MAX_ALTER_TEXT_LEN];
	char     ud[MAX_UD_LEN];    // 出牌的时候些可能需要在各事件之间传递一些参数
} OutCard;

#define MAX_PATTERN_UD_LEN     256

typedef struct tagOutCardPattern
{
	int         where;    // limited where the card is comes from 
	YESNO       fixed;    // limited the card is fixed or can use hero skill,weapon skill or armor skill to generate 
	YESNO       anyone;   // match of one card matched any one of the patterns 
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
	OutCardFlag_None = 0,           //{{<< @brief 按正常使用（打出） }}
	OutCardFlag_SpecOut = 1,        //{{<< @brie 指定的出牌 }}
	OutCardFlag_WithTarget= 2,      //{{<< @brie 指定目标 }}
	OutCardFlag_NoCheckDis= 4,      //{{<< @brie 忽略攻击距离 }}
	OutCardFlag_NoCheckTarget= 8,   //{{<< @brie 忽略攻击目标条件检查 }}
}OutCardFlag;

// tolua_end


#define PATTERN_MATCH_CARD_NUM(p)  ((p)->anyone == YES ? 1 : (p)->num)


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


/*{{
@brief 检测一张牌当前是否可以主动打出（使用）
@param pGame 当前的游戏对象
@param pEvent 当前事件对象
@param player 调用检测的玩家索引
@param pPosCard  用于检测的牌需要带有牌的坐标信息，可以是存在的手牌/装备等。也可以是一张假定的牌，此时pos应为-1。
@return YES - 可以出牌；NO - 不可以出指定的牌

脚本中通过调用此函数来判断殖家player 在当前事件中是否可以主动打出或者使用一张牌。

@notic 对于被动出牌和提供牌的时候，是通过样式来决定可打出的牌，不使用该函数来判断。
@see GameContext
@see GameEventContext
@see PosCard
@see YESNO
}}*/
YESNO game_card_can_out(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull PosCard* pPosCard);

/*{{
@brief 从字符串解析出牌样式表
@parem out_pattern  出牌样式表对象，用于输出解析完成的样式
@parem s_pattern    通过字符串给出的样式
@return 无
字符串样式格式如下：
<flags>:<card pattern 1>;<card pattern 2>;...?<custom_str>
其中：
flags: 一个或者多个字符表示牌可以从哪里来和一些特殊标志：
      h - 可以从手牌打出
	  e - 可以从装备区打出
	  j - 可以从判定区打出
	  f - 固定格式，只能通过真实牌且同时打出满足完整样式的所有牌，没有该标识，则分别打出样式中的每张牌且可以使用技能来提供
card  pattern: <{sid}><color><val> , 至少需要指定一个样式
      {sid} - 牌的sid名称需要{} 括起来，可选。如果不指定表示可以是任意sid的牌,造价于 [none}。
	  color - 牌的颜色，以下字符之一，可以省略，表示任意颜色的牌
	      s  : spade
	      h  : heart
	      d  : diamond
	      c  : club
	      b  : black (spade or club)
	      r  : red (heart or diamond)
	      n  : none (any color)
	  val : one of '2 - 10, J, Q, K, A' , can use <from-to> format, if it is empty, means any value.
	  custom_str: 可选，自定义串，直接解析到 ud 中，限制为255个字符。


@see OutCardPattern
}}*/
void game_load_out_pattern(lua_State* L, tolua_notnull OutCardPattern* out_pattern, tolua_notnull const char* s_pattern);



RESULT game_real_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, tolua_notnull OutCard* out_card);

// trigger supply from player the card match the  pattern, return through out_card
RESULT game_supply_card(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int trigger, int player
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text, tolua_notnull OutCard* out_card);

// passive out process
RESULT game_passive_out(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player,  int target
						, tolua_notnull OutCardPattern* out_pattern, const char* alter_text);

void game_init_outcard(tolua_notnull OutCard* out_card);

RESULT game_call_out_drive(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, int target, tolua_notnull OutCard* out_card);

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



