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
	Card    ocard;
	int     base;
	int     inc;
	int     dis;
	int     flag;
}AttackDis;


typedef struct tagSelectTarget
{
	Card   card;
	char   message[MAX_ALTER_TEXT_LEN];  //  if the player is cannot be set to target, return the alter message from this field;
} SelectTarget;

// tolua_end



// tolua_begin

// select target process
RESULT game_select_target(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, 
							YESNO self_select, YESNO force, const char* alter_text, int* out_target = 0);
// ѡ���Ǻͷ�
YESNO game_select_yesno(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text);

// ����ѡ��(ѡ�Ϊ�����ı���ÿ��һ��ѡ� ���� 1~n �кš�)(����з�������Ҫ�������ѡ��ҲҪͨ��items����)
int game_select_items(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* items, const char* alter_text);

RESULT game_check_attack(GameContext* pGame, GameEventContext* pEvent, int player, int target, CardID  cid);

// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __SELECT_H__ */


