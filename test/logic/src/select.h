#ifndef __SELECT_H__
#define __SELECT_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// tolua_begin

typedef struct tagAttackDis
{
	Card    card;
	int     base;
	int     inc;
	int     dis;
	int     flag;
}AttackDis;


typedef struct tagSelectTarget
{
	Card    card;
	char    message[MAX_ALTER_TEXT_LEN];  //  if the player is cannot be set to target, return the alter message from this field;
} SelectTarget;

// tolua_end



// tolua_begin

// select target process
RESULT game_select_target(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, CardID cid, 
							YESNO self_select, YESNO force, const char* alter_text, int* out_target = 0);
// 选择是和否
YESNO game_select_yesno(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, const char* alter_text);

// 多项选择(选项串为多行文本，每行一个选项。 返回 1~n 行号。)(如果有放弃的需要，把这个选项也要通过items给出)
int game_select_items(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pParentEvent, int player, tolua_notnull const char* items, const char* alter_text);

RESULT game_check_attack(tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, int target, CardID  cid, int tip);

// 计算基本攻击距离（武器相关，不包括自己与目标的马
int  get_base_attack_dis(tolua_notnull GameContext* pGame, int player, CardID cid);


// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __SELECT_H__ */


