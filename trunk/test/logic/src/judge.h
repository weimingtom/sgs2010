#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif



RESULT game_round_do_judge(GameContext* pGame, GameEventContext* pEvent, int player);



#ifdef __cplusplus
}
#endif


#endif /* __JUDGE_H__ */

