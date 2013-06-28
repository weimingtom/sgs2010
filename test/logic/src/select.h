#ifndef __SELECT_H__
#define __SELECT_H__

#include "config.h"

typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// select target process
RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel, const char* alter_text, int* out_target);
YESNO game_select_yesno(GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text);




#endif /* __SELECT_H__ */


