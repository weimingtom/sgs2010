#ifndef __SELECT_H__
#define __SELECT_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;


// select target process
RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel, const char* alter_text, int* out_target);
YESNO game_select_yesno(GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text);



#ifdef __cplusplus
}
#endif

#endif /* __SELECT_H__ */


