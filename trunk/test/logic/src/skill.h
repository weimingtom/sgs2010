// skill.h 

#ifndef __SKILL_H__
#define __SKILL_H__


#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


RESULT game_cmd_useskill(GameContext* pGame, GameEventContext* pEvent, int idx);

RESULT game_cmd_cancelskill(GameContext* pGame, GameEventContext* pEvent);

#ifdef __cplusplus
}
#endif

#endif /* __SKILL_H__ */




