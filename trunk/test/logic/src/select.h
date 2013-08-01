#ifndef __SELECT_H__
#define __SELECT_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// tolua_begin

// select target process
RESULT game_select_target(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, 
							YESNO self_select, YESNO may_cancel, const char* alter_text, int* out_target = 0);
// 选择是和否
YESNO game_select_yesno(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text);

// 多项选择(选项串为多行文本，每行一个选项。 返回 1~n 行号。)(如果有放弃的需要，把这个选项也要通过items给出)
int game_select_items(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* items, const char* alter_text);


// tolua_end

#ifdef __cplusplus
}
#endif

#endif /* __SELECT_H__ */


