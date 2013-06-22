// get.h 

#ifndef  __GET_H__
#define  __GET_H__

#include "config.h"


typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


RESULT game_first_getcard(GameContext* pGame);

RESULT game_getcard(GameContext* pGame, GameEventContext* pEvent, int num);






#endif /* __GET_H__ */



